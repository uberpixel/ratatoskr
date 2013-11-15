//
//  rkloggingengine.cpp
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

#include "rkloggingengine.h"

using namespace ratatoskr;

// ---------------------
// MARK: -
// MARK: logging_engine
// ---------------------

void logging_engine::set_log_level(log_level level)
{
	_level.store(level);
}

const char *logging_engine::translate_log_level(log_level level)
{
	switch(level)
	{
		case log_level::debug:
			return "(dbg)";
		case log_level::info:
			return "(info)";
		case log_level::warning:
			return "(warn)";
		case log_level::error:
			return "(err)";
		case log_level::critical:
			return "(crit)";
	}
}

// ---------------------
// MARK: -
// MARK: stream_logging_engine
// ---------------------

stream_logging_engine::stream_logging_engine(std::ostream& stream) :
	_stream(stream)
{}


bool stream_logging_engine::is_good() const
{
	return _stream.good();
}

void stream_logging_engine::flush()
{
	_stream.flush();
}

void stream_logging_engine::write(const message& message)
{
	_stream << translate_log_level(message.get_level()) << " " << message.get_message() << "\n";
}

void stream_logging_engine::finalize()
{
	flush();
}
