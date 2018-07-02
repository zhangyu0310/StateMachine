/*Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the mutex guard.
* You must include this file if you want to use this mutex guard.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this file; if not, as you like...
*/

#ifndef MUTEX_GUARD_H
#define MUTEX_GUARD_H

namespace poppin
{
/*
* This class is work with mutex.
* The mutex must have lock() & unlock() function.
***************************************
* Use like this:
***************************************
* {
*     Mutex mutex;
*     MutexGuard<Mutex> guard(mutex);
* }
***************************************
*/
template<typename _Ty>
class MutexGuard
{
public:
	explicit MutexGuard(_Ty &mutex) : mutex_(mutex)
	{
		mutex_.lock();
	}
	MutexGuard(const MutexGuard<_Ty> &) = delete;
	MutexGuard &operator= (const MutexGuard<_Ty> &) = delete;
	~MutexGuard()
	{
		mutex_.unlock();
	}
private:
	_Ty &mutex_;
};
}
#endif // !MUTEX_GUARD_H