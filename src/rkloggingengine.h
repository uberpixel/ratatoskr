//
//  rkloggingengine.h
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

#ifndef _RATATOSKR_LOGGINGENGINE_H_
#define _RATATOSKR_LOGGINGENGINE_H_

#include <iostream>
#include <atomic>
#include "rklogger.h"

namespace ratatoskr
{
	class logging_engine
	{
	public:
		virtual ~logging_engine() {}
		
		virtual bool is_good() const = 0;
		virtual void flush() = 0;
		virtual void finalize() = 0;
		
		virtual void write(const message& message) = 0;
		virtual void significant_time_passed() {}
		
		void set_log_level(log_level level);
		log_level get_log_level() const { return _level.load(); }
		
		static const char *translate_log_level(log_level level);
		
	protected:
		logging_engine() :
			_level(log_level::info)
		{}
		
		std::atomic<log_level> _level;
	};
	
	class stream_logging_engine : public logging_engine
	{
	public:
		stream_logging_engine(std::ostream& stream);
		
		bool is_good() const final;
		void flush() final;
		void finalize() final;
		
		void write(const message& message) final;
		
	private:
		std::ostream& _stream;
	};
}

#endif /* _RATATOSKR_LOGGINGENGINE_H_ */
