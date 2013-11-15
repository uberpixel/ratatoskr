//
//  rklogger.h
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

#ifndef _RATATOSKR_LOGGER_H_
#define _RATATOSKR_LOGGER_H_

#include <sstream>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>

#include "rksingleton.h"
#include "rkspinlock.h"

namespace ratatoskr
{
	enum class log_level : int
	{
		debug,
		info,
		warning,
		error,
		critical
	};
	
	class message
	{
	public:
		message(log_level level, const std::string& message);
		message(log_level level, std::string&& message);
		
		log_level get_level() const { return _level; }
		std::chrono::system_clock::time_point get_time() const { return _time; }
		
		const std::string& get_message() const;
		
	private:
		log_level _level;
		std::string _message;
		mutable std::string _formatted_time;
		std::chrono::system_clock::time_point _time;
	};
	
	class logging_engine;
	class logger : public singleton<logger>
	{
	public:
		logger();
		~logger() override;
		
		void log(const message& message);
		void log(message&& message);
		void log(log_level level, const std::string& message);
		void log(log_level level, std::string&& message);
		
		void set_flush_delay(size_t delay); // Defaults to 250ms
		void set_flush_buffer_threshold(size_t threshold); // Defaults to 1024 messages
		void set_significant_time(size_t time); // Defaults to 10s
		
		void add_logging_engine(logging_engine *engine);
		void remove_logging_engine(logging_engine *engine);
		std::vector<logging_engine *> get_logging_engines();
		
		void flush(bool wait = false);
		
	private:
		struct flush_data
		{
			flush_data(std::chrono::system_clock::time_point t) :
				time(t)
			{}
			
			std::chrono::system_clock::time_point time;
			std::vector<message> buffer;
		};
		
		void force_flush();
		void flush_run_loop();
		void flush_engine(logging_engine *engine, const flush_data& data);
		
		std::atomic<bool> _teardown_flag;
		std::vector<message> _buffer;
		std::chrono::system_clock::time_point _last_message;
		
		std::vector<logging_engine *> _engines;
		
		spinlock _lock;
		size_t _significant_time;
		
		std::mutex _signal_lock;
		std::condition_variable _signal;
		
		size_t _flush_delay;
		size_t _flush_buffer_threshold;
		std::thread _flush_thread;
		std::mutex _flush_lock;
		std::atomic_flag _flush_flag;
	};
}

#define __rklog(level, str) ratatoskr::logger::get_shared_instance()->log(ratatoskr::log_level::level, str)

#define rkdebug(str)    __rklog(debug, str)
#define rkinfo(str)     __rklog(info, str)
#define rkwarning(str)  __rklog(warning, str)
#define rkerror(str)    __rklog(error, str)
#define rkcritical(str) __rklog(critical, str)

#endif /* _RATATOSKR_LOGGER_H_ */
