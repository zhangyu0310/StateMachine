/*Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the condition variable.
* You must include this file if you want to use this condition variable.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this file; if not, as you like...
*/

#ifndef CONDITION_H
#define CONDITION_H

#include <condition_variable>

namespace poppin
{
/*
* This class is a condition variable.
* The mutex must have lock() & unlock() function.
***************************************
* Use like this:
***************************************
* Mutex mutex;
* Condition<Mutex> condition(mutex);
* bool ready;
* Thread A:
* {
*     MutexGuard<Mutex> guard(mutex);
*     while(!ready)
*     {
*         condition.wait();
*     }
* }
*
* Thread B:
* {
*     doSomething();
*     ready = true;
*     condition.signal();
* }
***************************************
*/
template<typename _Ty>
class Condition
{
public:
	explicit Condition(_Ty& mutex) : mutex_(mutex) {}
	Condition(const Condition&) = delete;
	Condition& operator= (const Condition&) = delete;
	~Condition() {}

	//FIXME:Return value should be 'int'.
	void signal() {
		condition_.notify_one();
	}
	void broadcast() {
		condition_.notify_all();
	}
	//TODO:wait for time.
	void wait() {
		condition_.wait(mutex_);
	}

private:
	_Ty& mutex_;
	std::condition_variable_any condition_;
};
} // namespace poppin

#endif // !CONDITION_H