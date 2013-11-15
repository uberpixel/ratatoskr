//
//  main.cpp
//  example
//
//  Created by Sidney Just on 15.11.13.
//  Copyright (c) 2013 Sidney Just. All rights reserved.
//

#include "ratatoskr.h"
#include "stresstest.h"

int main(int argc, const char * argv[])
{
	rkdebug("Hello World");
	stress_test::run_test();
	
    return 0;
}

