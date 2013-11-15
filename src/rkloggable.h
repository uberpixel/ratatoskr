//
//  rkloggable.h
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

#ifndef _RATATOSKR_LOGGABLE_H_
#define _RATATOSKR_LOGGABLE_H_

#include <sstream>
#include "rklogger.h"

namespace ratatoskr
{
	class loggable
	{
	public:
		loggable(log_level level = log_level::info);
		~loggable();
		
		void submit();
		
		loggable& operator << (const std::string& val) { _stream << val; return *this; }
		loggable& operator << (const char *val) { _stream << val; return *this; }
		loggable& operator << (bool val) { _stream << val; return *this; }
		loggable& operator << (short val) { _stream << val; return *this; }
		loggable& operator << (unsigned short val) { _stream << val; return *this; }
		loggable& operator << (int val) { _stream << val; return *this; }
		loggable& operator << (unsigned int val) { _stream << val; return *this; }
		loggable& operator << (long val) { _stream << val; return *this; }
		loggable& operator << (unsigned long val) { _stream << val; return *this; }
		loggable& operator << (long long val) { _stream << val; return *this; }
		loggable& operator << (unsigned long long val) { _stream << val; return *this; }
		loggable& operator << (const void *val) { _stream << val; return *this; }
		loggable& operator << (std::ostream& (*pf)(std::ostream&)) { _stream << pf; return *this; }
		loggable& operator << (std::ios& (*pf)(std::ios&)) { _stream << pf; return *this; };
		loggable& operator << (std::ios_base& (*pf)(std::ios_base&)) { _stream << pf; return *this; }
		
	private:
		log_level _level;
		std::stringstream _stream;
	};
}

#endif /* _RATATOSKR_LOGGABLE_H_ */
