/*
* Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the struct state.
* You must include this file if you want to use state machine.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this file; if not, as you like...
*/

#ifndef STATE_H
#define STATE_H
namespace poppin {

template<typename _State> //typedef int _State;
struct State {
	enum NodeType { begin, ordinary, end };
	State(_State id, NodeType type = ordinary) : state_id_(id), type_(type) {}
	_State state_id_;
	NodeType type_;
};

}
#endif // STATE_H