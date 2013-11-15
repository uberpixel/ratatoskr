//
//  timer.h
//  ratatoskr
//
//  Created by Sidney Just on 15.11.13.
//  Copyright (c) 2013 Sidney Just. All rights reserved.
//

#ifndef ratatoskr_timer_h
#define ratatoskr_timer_h

#include <chrono>

class timer
{
public:
	timer() :
		_start(std::chrono::high_resolution_clock::now())
	{}
	
	long time()
	{
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now - _start).count();
	}
	
private:
	std::chrono::high_resolution_clock::time_point _start;
};

#endif
