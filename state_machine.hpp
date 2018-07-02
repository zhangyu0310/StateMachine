/*Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the state machine.
* You must include this file if you want to use this state machine.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this file; if not, as you like...
*/

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <functional>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>

namespace poppin
{
/*
* This is a status machine. It can accept diffient type input.
* It can run in a new thread if you use startInThread to start it.
* 
* You need add or delete states. Then you can start this machine.
* If you wanna add new states or remove states, pls stop it first.
* Use receiveImport to change state. State can bind with a function,
* there has some default function, you can bind it.
*
* This class IS NOT thread-safe, be careful!
*/
//typedef int _Ty;
template<typename _Ty>
class CStateMachine
{
	//FIXME:The name of the map 'StateChange' is asshole, pls revise...
	typedef std::unordered_map<std::string, std::string> StateChange;
    typedef std::unordered_map<_Ty, StateChange> StateChangeMap;
	typedef std::function<void(void*)> StateFunction;
	typedef std::unordered_map<std::string, StateFunction> StateFunctionMap;
public:

	CStateMachine()
        : running_(false),
		  custom_function_(NULL),
		  cur_state_("") {}
	CStateMachine(const CStateMachine<_Ty> &) = delete;
	CStateMachine<_Ty> &operator= (const CStateMachine<_Ty> &) = delete;
	~CStateMachine() 
	{
		if (running_ && thread_.joinable())
		{
			thread_.join();
		}
	}

	void init(const std::string &init_state, 
		      const StateFunction &custom_func = NULL);
	void start();
	void startInThread() { thread_ = std::thread(&start); }
	void stop() { running_ = false; }
	void receiveImport();

	void addStateChange();
	void removeStateChange();
	void addStateFunc();
	void removeStateFunc();
	std::thread &getThread() { return thread_; }
	std::string getState() { return cur_state_; }

private:
	void ignoreFunction(void *arg) {}

	//TODO:Make a set to collection states.
	bool running_;
	StateFunction custom_function_;
	std::thread thread_;
	std::string cur_state_;
	StateChangeMap state_change_map_;
	StateFunctionMap state_function_map_;
};

template<typename _Ty>
inline void CStateMachine<_Ty>::init(const std::string &init_state,
	                                     const StateFunction &custom_func)
{
	cur_state_ = init_state;
	if (custom_function_ == NULL)
	{
		using std::placeholders::_1;
		custom_function_ = std::bind(&ignoreFunction, this, _1);
	}
}

template<typename _Ty>
inline void CStateMachine<_Ty>::start()
{
	running_ = true;
	while(running_)
	{

	}
}

}  // namespace poppin
#endif // !STATE_MACHINE_H