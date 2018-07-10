#include <iostream>
#include <mutex>
#include <thread>

#include "mutex.hpp"
#include "mutex_guard.hpp"
#include "condition.hpp"

#define POPPIN

#ifdef POPPIN
void func(poppin::Mutex &mutex, bool &ready,
	poppin::Condition<poppin::Mutex> &cond)
{
	std::cout << "In the Thread!" << std::endl;
	poppin::MutexGuard<poppin::Mutex> guard(mutex);
	while (!ready)
	{
		std::cout << "In the while!" << std::endl;
		cond.wait();
	}
	std::cout << "Thread is end!" << std::endl;
}
#else
void func(std::mutex &mutex, bool &ready,
	      poppin::Condition<std::mutex> &cond)
{
	std::cout << "In the Thread!" << std::endl;
	poppin::MutexGuard<std::mutex> guard(mutex);
	while (!ready)
	{
		std::cout << "In the while!" << std::endl;
		cond.wait();
	}
	std::cout << "Thread is end!" << std::endl;
}
#endif

int main()
{
#ifdef POPPIN
	poppin::Mutex mutex;
#else
	std::mutex mutex;
#endif
	bool ready = false;
#ifdef POPPIN
	poppin::Condition<poppin::Mutex> cond(mutex);
#else
	poppin::Condition<std::mutex> cond(mutex);
#endif
	std::thread thr(func, std::ref(mutex), std::ref(ready), std::ref(cond));
	
	int haha;
	std::cin >> haha;
	{
#ifdef POPPIN
		poppin::MutexGuard<poppin::Mutex> guard(mutex);
#else
		poppin::MutexGuard<std::mutex> guard(mutex);
#endif
		std::cout << "Doing something..." << std::endl;
		ready = true;
		cond.signal();
	}

	if(thr.joinable())
	    thr.join();
	return 0;
}