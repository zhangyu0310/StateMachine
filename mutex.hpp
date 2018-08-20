/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the mutex.
* You must include this file if you want to use this mutex.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this file; if not, as you like...
*/

#ifndef MUTEX_H
#define MUTEX_H

#include <mutex>

namespace poppin
{
class Mutex {
public:
	Mutex() : mutex_(new std::mutex) {}
	Mutex(const Mutex&) = delete;
	Mutex& operator= (const Mutex&) = delete;
	~Mutex() { delete mutex_; }
	void lock() { mutex_->lock(); }
	void unlock() { mutex_->unlock(); }
private:
	std::mutex* mutex_;
};
} // namespace poppin

#endif // !MUTEX_H