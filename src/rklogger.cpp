//
//  rklogger.cpp
//  ratatoskr
//
//  Created by Sidney Just
//  Copyright (c) 2013 by Überpixel
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
//  documentation files (the "Software"), to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
//  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//  PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
//  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include "rklogger.h"
#include "rkloggingengine.h"

using namespace ratatoskr;

// Used as fallback when there is no engine registered with the logger
static stream_logging_engine __fallback_engine(std::cout);

// ---------------------
// MARK: -
// MARK: message
// ---------------------

message::message(log_level level, const std::string& message) :
	_level(level),
	_time(std::chrono::system_clock::now()),
	_message(message)
{}

message::message(log_level level, std::string&& message) :
	_level(level),
	_time(std::chrono::system_clock::now()),
	_message(std::move(message))
{}


const std::string& message::get_message() const
{
	return _message;
}

// ---------------------
// MARK: -
// MARK: logger
// ---------------------

logger::logger() :
	_last_message(std::chrono::system_clock::now()),
	_significant_time(10),
	_teardown_flag(false),
	_flush_delay(250),
	_flush_buffer_threshold(1024),
	_flush_thread(std::thread(std::bind(&logger::flush_run_loop, this)))
{}

logger::~logger()
{
	_teardown_flag.store(true);
	_signal.notify_one();
	
	try
	{
		// Just try to join the thread, checking for joinable() will just
		// result in a race condition
		_flush_thread.join();
	}
	catch(std::system_error e)
	{}
	
	force_flush();
	
	for(auto engine : _engines)
		engine->finalize();
}



void logger::set_flush_delay(size_t delay)
{
	std::lock_guard<decltype(_signal_lock)> lock(_signal_lock);
	_flush_delay = delay;
}

void logger::set_flush_buffer_threshold(size_t threshold)
{
	std::lock_guard<decltype(_lock)> lock(_lock);
	_flush_buffer_threshold = threshold;
}

void logger::set_significant_time(size_t time)
{
	std::lock_guard<decltype(_flush_lock)> lock(_flush_lock);
	_significant_time = time;
}



void logger::log(const message& message)
{
	std::lock_guard<decltype(_lock)> lock(_lock);
	_buffer.push_back(message);
	
	if(_buffer.size() >= _flush_buffer_threshold)
		flush();
}

void logger::log(message&& message)
{
	std::lock_guard<decltype(_lock)> lock(_lock);
	_buffer.push_back(std::move(message));
	
	if(_buffer.size() >= _flush_buffer_threshold)
		flush();
}

void logger::log(log_level level, const std::string& tmessage)
{
	message message(level, tmessage);
	log(std::move(message));
}

void logger::log(log_level level, std::string&& tmessage)
{
	message message(level, std::move(tmessage));
	log(std::move(message));
}



void logger::add_logging_engine(logging_engine *engine)
{
	std::lock_guard<decltype(_flush_lock)> lock(_flush_lock);
	_engines.push_back(engine);
}

void logger::remove_logging_engine(logging_engine *engine)
{
	std::lock_guard<decltype(_flush_lock)> lock(_flush_lock);
	_engines.erase(std::remove(_engines.begin(), _engines.end(), engine), _engines.end());
	
	engine->finalize();
}

std::vector<logging_engine *> logger::get_logging_engines()
{
	std::lock_guard<decltype(_flush_lock)> lock(_flush_lock);
	std::vector<logging_engine *> engines(_engines);
	
	return engines;
}




void logger::flush(bool wait)
{
	if(!wait)
	{
		if(!_flush_flag.test_and_set())
			_signal.notify_one();
		
		return;
	}
	
	force_flush();
}

void logger::flush_run_loop()
{
	while(!_teardown_flag.load())
	{
		std::unique_lock<decltype(_signal_lock)> lock(_signal_lock);
		_signal.wait_for(lock, std::chrono::milliseconds(_flush_delay));
		
		force_flush();
	}
}

void logger::force_flush()
{
	std::lock(_lock, _flush_lock);
	
	std::unique_lock<decltype(_lock)> lock(_lock, std::adopt_lock);
	std::unique_lock<decltype(_flush_lock)> flush_lock(_flush_lock, std::adopt_lock);
	
	flush_data data(_last_message);
	std::swap(data.buffer, _buffer);
	
	lock.unlock();
	
	if(!data.buffer.empty())
	{
		std::stable_sort(data.buffer.begin(), data.buffer.end(), [](const message& a, const message& b) { return (a.get_time() < b.get_time()); });
		
		if(!_engines.empty())
		{
			for(auto engine : _engines)
			{
				flush_engine(engine, data);
			}
		}
		else
		{
			flush_engine(&__fallback_engine, data);
		}
		
		auto& message = data.buffer.back();
		_last_message = message.get_time();
	}
	
	_flush_flag.clear();
}

void logger::flush_engine(logging_engine *engine, const flush_data& data)
{
	if(!engine->is_good())
		return;
	
	auto last  = data.time;
	auto level = engine->get_log_level();
	
	for(auto& message : data.buffer)
	{
		long offset = std::chrono::duration_cast<std::chrono::seconds>(message.get_time() - last).count();
		last = message.get_time();
		
		if(offset >= _significant_time)
			engine->significant_time_passed();
		
		if(message.get_level() >= level)
			engine->write(message);
	}
	
	engine->flush();
}
