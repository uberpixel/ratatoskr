//
//  stresstest.cpp
//  ratatoskr
//
//  Created by Sidney Just on 15.11.13.
//  Copyright (c) 2013 Sidney Just. All rights reserved.
//

#include <random>
#include <iostream>
#include "ratatoskr.h"

#include "stresstest.h"
#include "timer.h"

#define STRESS_TEST_MESSAGES (UINT16_MAX * 5)
#define USE_RATATOSKR 1
#define USE_MULTIPLE_THREADS 1

namespace stress_test
{
	void stress_test_thread(size_t count)
	{
		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<> distribution(0, UINT16_MAX);
		
		for(size_t i = 0; i < count; i ++)
		{
			int result = distribution(engine);
			
#if USE_RATATOSKR
			ratatoskr::loggable loggable;
			loggable << "result " << result;
#else
			std::cout << "result " << result << std::endl;
#endif
		}
	}
	
	
	void run_test()
	{
		std::vector<std::thread> threads;
		size_t count = std::thread::hardware_concurrency();
		
		timer timer;
		
#if USE_MULTIPLE_THREADS
		for(size_t i = 0; i < count; i ++)
		{
			threads.emplace_back(std::thread(std::bind(&stress_test_thread, STRESS_TEST_MESSAGES)));
		}
		
		for(auto& thread : threads)
		{
			try
			{
				thread.join();
			}
			catch(std::system_error e)
			{}
		}
#else 
		size_t messages = STRESS_TEST_MESSAGES * count;
		stress_test_thread(messages);
#endif
		
#if USE_RATATOSKR
		ratatoskr::logger::get_shared_instance()->flush(true);
#endif
		
		std::cout << "It took " << timer.time() << " milliseconds to output " << (STRESS_TEST_MESSAGES * count) << " messages"  << std::endl;
	}
}
