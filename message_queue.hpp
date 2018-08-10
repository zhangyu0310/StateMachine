/*Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the message queue.
* You must include this file if you want to use this message queue.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this file; if not, as you like...
*/

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <deque>
#include <mutex>

#include "condition.hpp"
#include "mutex_guard.hpp"

namespace poppin
{
template<typename _Ty>
class MessageQueue {
public:
	MessageQueue() : mutex_(), notEmpty_(mutex_) {}
	MessageQueue(const MessageQueue&) = delete;
	MessageQueue& operator= (const MessageQueue&) = delete;
	~MessageQueue() {}

	void putMsg(const _Ty& msg) {
		MutexGuard<std::mutex> guard(mutex_);
		queue_.push_back(msg);
		notEmpty_.signal();
	}
	_Ty getMsg() {
		MutexGuard<std::mutex> guard(mutex_);
		while (queue_.empty()) {
			notEmpty_.wait();
		}
		_Ty msg(queue_.front());
		queue_.pop_front();
		return msg;
	}
	size_t size() const {
		MutexGuard<std::mutex> guard(mutex_);
		return queue_.size();
	}

private:
	std::deque<_Ty> queue_;
    mutable std::mutex mutex_;
	Condition<std::mutex> notEmpty_;
};
}

#endif // !MESSAGE_QUEUE_H