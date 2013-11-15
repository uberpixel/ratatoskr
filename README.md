Ratatoskr is a simple, yet fast logging system written in C++11 which is especially targeted towards multi-threaded applications. It is extendable, meaning that you can write your own logging engines for it to customize the output of the logs. For example, you can have a logging engine that outputs to `std::cout` and another one writing to disk at the same time.

Log messages are queued up and flushed back regularly, or on demand, using a low-overhead background thread to avoid stalling the application when writing many debug messages. Furthermore, Ratatoskr allows filtering of log messages by their log level, to simply disable certain message from appearing in selected logging engines.

Unlike `std::cout`, or similar output mechanisms, Ratatoskr doesn't require the programmer to synchronize access to the logger, you can simply log messages to it from as many threads as you like and be sure that they appear in exactly the order they were submitted to the logger.

## Example
	int main()
	{
		ratatoskr::loggable loggable;
		loggable << "Hello World";
		
		return 0;
	}
	
Output:

	(info) Hello World
	
## Performance
Ratatoskr is made for multithreading, where it outperforms a simple `std::cout` by a wide margin. However, due to the added overhead, in a single threaded environment, `std::cout` will beat the pants off of Ratatoskr with ease (due it still might be interesting for you because of customizable logging engines and log levels). Anyways, long story short, here are the facts:

### Test case
	void stress_test_thread(size_t count)
	{
		std::random_device rd;
		std::mt19937 engine(rd());
		std::uniform_int_distribution<> distribution(0, UINT16_MAX);
		
		for(size_t i = 0; i < count; i ++)
		{
			int result = distribution(engine);
			
	#if USE_RATATOSKR
			// Using a single logging engine that outputs to std::cout as well
			ratatoskr::loggable loggable;
			loggable << "result " << result;
	#else
			std::cout << "result " << result << std::endl;
	#endif
		}
	}

### Results
Running the above test function on my 2.4GhZ Intel i7 MacBook Pro yields the following results (note that MP tests are run on 8 cores using 8 threads)
	
Messages | SP std::cout | SP ratatoskr | MP std::cout | MP ratatoskr
---------|-------------:|-------------:|-------------:|------------:
524280   | 4875ms       | 5646ms       | 80426ms      | 6051ms
1048560  | 9878ms       | 11989ms      | 132992ms     | 11508ms
2621400  | 13764ms      | 34555ms      | 336590ms     | 37734ms

### Note
The multithreaded tests didn't employ any kind of synchronization effort, hence for the `std::cout` version, the output looked similar to the following:

	3 03r71e81s
	0u8lr
	te sr2ue6ls6tu2 l51t
	8 11r98e04s
	8u7lr
	te sr1ue8ls3tu
	 l2t2 126277
	28
	
## Concepts
Ratatoskr is structured around the `logger` class, which is a singleton (though you are free to create your instances, it just isn't really useful), and which accepts `messages`. `messages` are an abstraction over one log message and holds the actual message itself as well as some additional information like the log level and when the message was posted.

The `logger` class queues up all messages it receives and will flush the queued messages periodically, or on demand, either synchronously or asynchronously. When the message queue gets flushed, the `logger` will ask all `logging_engine`'s that were added to it to write the messages.

The `logging_engine`'s in turn are responsible for actually writing the messages to wherever they are supposed to write them. Ratatoskr comes with the `stream_logging_engine`, which allows writing to any `std::ostream`, however, you can write your own `logging_engine` subclasses to customize the output and logging however you seem fit.

*Note* by default there is no `logging_engine` registered with the `logger`, which means that it will automatically output all logs via `std::cout`. You can add your own logging engines via the `logger::add_logging_engine` method.

There is no direct replacement for `std::cout` or similar, since the overloaded `<<` operator makes multithreading impossible. Instead, there is the `loggable` class which provides the same `<<` operator overloads as `std::basic_ostream`, but represents a single message, which gets flushed automatically once the `loggable` gets deallocated or its `submit` method is invoked. As an alternative, you can also use the `rkdebug()`, `rkinfo()`, `rkwarning()`, `rkerror()` and `rkcritical()` macros, which create log messages with their respective logging level (ie `rkinfo()` generates an info level message).

## License
Ratatoskr is released under the MIT license, which basically means that you can do whatever you want with it.