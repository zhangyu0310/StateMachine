/*
* Copyright (C) 2018 poppinzhang.
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
#include <memory>
#include <set>
#include <string>
#include <thread>

#include "message_queue.hpp"
#include "state.hpp"

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
//typedef int _State;
//typedef int _Input;
class CStateMachine {
	typedef std::shared_ptr<State<_State>> StatePtr;
	typedef std::function<void(CStateMachine*, void*)> StateFunction;
	//FIXME:I don't know how to name this struct...
	//FuncStatePair is a set of function and next state.
	typedef std::pair<StateFunction, StatePtr> FuncStatePair;
	typedef std::map<_Input, FuncStatePair> InputFunctionMap;
	typedef std::map<StatePtr, InputFunctionMap> StateChangeMap;
	typedef std::set<StatePtr> StateSet;

public:

	CStateMachine()
        : running_(false),
		  custom_function_(nullptr),
	      inited_(false) {}
	CStateMachine(const CStateMachine&) = delete;
	CStateMachine& operator= (const CStateMachine&) = delete;
	~CStateMachine() {
		if (running_ && thread_.joinable()) {
			thread_.join();
		}
	}

	void init(const _State& init_state,
		      const StateFunction& custom_func = nullptr);
	void start();
	void startInThread() { thread_ = std::thread(&CStateMachine::start, this); }
	void stop() { running_ = false; }
	void receiveImport(const _Input& input);

	void addStateChange(const _State& start_state, 
		                const _Input& input,
		                const StateFunction& function,
		                const _State& end_state
		                /*NodeType node_type = State<_State>::ordinary*/);
	void removeStateChange(const _State& start_state, const _Input& input);
	std::thread& getThread() { return thread_; }
	const _State& getState() const { return cur_state_->state_id_; }
	//size_t getQueueSize() const { return msg_queue_.size(); }
	bool isRunning() const { return running_; }
	bool isInited() const { return inited_; }
	static void ignoreFunction(CStateMachine* sm, void* arg) {}
	static void exitFunction(CStateMachine* sm, void* arg) {
		//TODO:Print or log error input and current state.
		sm->stop();
	}

private:
	volatile bool running_;
	bool inited_;
	StateFunction custom_function_;
	std::thread thread_;
	StatePtr cur_state_;
	StateChangeMap state_change_map_;
	StateSet state_set_;
	MessageQueue<_Input> msg_queue_;
};

template<typename _State, typename _Input>
inline void CStateMachine<_State, _Input>::init(const _State& init_state,
	                                 const StateFunction& custom_func) {
	if (inited_) {
		//TODO:Output "State Machine had been inited."
		return;
	}
	cur_state_ = std::shared_ptr<State<_State>>(new State<_State>(init_state, State<_State>::begin));
	state_set_.insert(cur_state_);
	if (custom_func == nullptr) {
		//using std::placeholders::_1;
		//custom_function_ = std::bind(&ignoreFunction, this, _1);
		custom_function_ = &CStateMachine::ignoreFunction;
	} else {
		custom_function_ = custom_func;
	}
	inited_ = true;
}

template<typename _State, typename _Input>
void CStateMachine<_State, _Input>::start() {
	if (!inited_) {
		//TODO:Output "Not init the state machine"
		stop();
		return;
	}
	if (running_) {
		//TODO:Output "State machine is running."
		return;
	}

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
		if (func_state_pair.first != nullptr) {
			func_state_pair.first(this, &input);
		}
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
	const _State& end_state
	/*NodeType node_type*/) {
	if (!inited_) {
		//TODO:Output "State Machine not been inited."
		return;
	}
	std::shared_ptr<State<_State>> start_ptr;
	std::shared_ptr<State<_State>> end_ptr;
	for (auto it : state_set_) {
		if (it->state_id_ == start_state) {
			start_ptr = it;
		}
		if (it->state_id_ == end_state) {
			end_ptr = it;
		}
	}
	if (start_ptr == nullptr) {
		start_ptr = std::shared_ptr<State<_State>>(new State<_State>(start_state));
		state_set_.insert(start_ptr);
	}
	if (end_ptr == nullptr) {
		end_ptr = std::shared_ptr<State<_State>>(new State<_State>(end_state));
		state_set_.insert(end_ptr);
	}
	//Use lower_bound instead of find to get more higer insert effectiveness.
	auto sc_map_it = state_change_map_.lower_bound(start_ptr);
	if (sc_map_it != state_change_map_.end() && 
	    !(state_change_map_.key_comp()(start_ptr, sc_map_it->first))) {
		auto input_map = sc_map_it->second;
		//FIXME:Find if the input already exists, not only replace it.
		FuncStatePair fs_pair = std::make_pair(function, end_ptr);
		input_map[input] = fs_pair;
		state_change_map_[start_ptr] = input_map;
	} else {
		FuncStatePair fs_pair = std::make_pair(function, end_ptr);
		InputFunctionMap input_map;
		input_map.insert(std::make_pair(input, fs_pair));
		state_change_map_.insert(sc_map_it,
			                     std::make_pair(start_ptr, input_map));
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
