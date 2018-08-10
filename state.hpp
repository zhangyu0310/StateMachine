/*Copyright (C) 2018 poppinzhang.
*
* Written by poppinzhang with C++11 <poppinzhang@tencent.com>
*
* This file is head file of the class state.
* You must include this file if you want to use state machine.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this file; if not, as you like...
*/

#ifndef STATE_H
#define STATE_H
typedef int _State;
typedef int _Input;
class State {
public:
private:
	_State state_id_;
};
#endif // STATE_H