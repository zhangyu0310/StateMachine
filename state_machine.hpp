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
#include <list>
#include <map>
#include <set>
#include <string>
#include <thread>

#include "message_queue.hpp"

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

template<typename _State, typename _Input>
class CStateMachine {
	typedef std::function<void(CStateMachine*, void*)> StateFunction;
	//FIXME:I don't know how to name this struct...
	//FuncStatePair is a set of function and next state.
	typedef std::pair<StateFunction, _State> FuncStatePair;
	typedef std::map<_Input, FuncStatePair> InputFunctionMap;
	typedef std::map<_State, InputFunctionMap> StateChangeMap;
	typedef std::list<_State> StateList;

public:

	CStateMachine()
        : running_(false),
		  custom_function_(NULL) {}
	CStateMachine(const CStateMachine&) = delete;
	CStateMachine& operator= (const CStateMachine&) = delete;
	~CStateMachine() {
		if (running_ && thread_.joinable()) {
			thread_.join();
		}
	}

	void init(const _State& init_state,
		      const StateFunction& custom_func = NULL);
	void start();
	void startInThread() { thread_ = std::thread(&CStateMachine::start, this); }
	void stop() { running_ = false; }
	void receiveImport(const _Input& input);

	void addStateChange(const _State& start_state, const _Input& input,
		                const StateFunction& function,
		                const _State& end_state);
	void removeStateChange(const _State& start_state, const _Input& input);
	std::thread& getThread() { return thread_; }
	_State getState() const { return cur_state_; }
	size_t getQueueSize() const { return msg_queue_.size(); }
	bool isRunning() const { return running_; }
	bool isInited() const { return inited_; }
	static void ignoreFunction(CStateMachine* sm, void* arg) {}
	static void exitFunction(CStateMachine* sm, void* arg) {
		//TODO:Print or log error input and current state.
		running_ = false;
	}

private:
	volatile bool running_;
	bool inited_;
	StateFunction custom_function_;
	std::thread thread_;
	_State cur_state_;
	StateChangeMap state_change_map_;
	StateList state_list_;
	MessageQueue<_Input> msg_queue_;
};

template<typename _State, typename _Input>
inline void CStateMachine<_State, _Input>::init(const _State& init_state,
	                                 const StateFunction& custom_func) {
	cur_state_ = init_state;
	if (custom_function_ == NULL) {
		//using std::placeholders::_1;
		//custom_function_ = std::bind(&ignoreFunction, this, _1);
		custom_function_ = &CStateMachine::ignoreFunction;
	}
	inited_ = true;
}

template<typename _State, typename _Input>
void CStateMachine<_State, _Input>::start() {
	running_ = true;
	while (running_) {
		_Input input = msg_queue_.getMsg();
		auto sc_map_it = state_change_map_.find(cur_state_);
		if (sc_map_it == state_change_map_.end()) {
			//A default final state
			running_ = false;
			continue;
		}
		auto input_func_map = sc_map_it->second;
		auto if_map_it = input_func_map.find(input);
		if (if_map_it == input_func_map.end()) {
			custom_function_(this, &input);
			continue;
		}
	    auto func_state_pair = if_map_it->second;
		func_state_pair.first(this, &input);
		cur_state_ = func_state_pair.second;
	}
}

template<typename _State, typename _Input>
inline void CStateMachine<_State, _Input>::receiveImport(const _Input& input) {
	msg_queue_.putMsg(input);
}

//TODO:Add default parameter, 
//function is custom_function_ and end_state is same as start_state.
template<typename _State, typename _Input>
inline void CStateMachine<_State, _Input>::addStateChange(
	const _State& start_state, 
	const _Input& input, 
	const StateFunction& function, 
	const _State& end_state) {
	//Use lower_bound instead of find to get more higer insert effectiveness.
	auto sc_map_it = state_change_map_.lower_bound(start_state);
	if (sc_map_it != state_change_map_.end() && 
	    !(state_change_map_.key_comp()(start_state, sc_map_it->first))) {
		auto input_map = sc_map_it->second;
		//FIXME:Find if the input already exists, not only replace it.
		FuncStatePair fs_pair = std::make_pair(function, end_state);
		input_map[input] = fs_pair;
		state_change_map_[start_state] = input_map;
	} else {
		FuncStatePair fs_pair = std::make_pair(function, end_state);
		InputFunctionMap input_map;
		input_map.insert(std::make_pair(input, fs_pair));
		state_change_map_.insert(sc_map_it,
			                     std::make_pair(start_state, input_map));
		state_list_.push_back(start_state);
	}
}

//TODO:This function is hard to achieve.
template<typename _State, typename _Input>
inline void CStateMachine<_State, _Input>::removeStateChange(
	const _State& start_state, 
	const _Input& input) {
}

}  // namespace poppin
#endif // !STATE_MACHINE_H