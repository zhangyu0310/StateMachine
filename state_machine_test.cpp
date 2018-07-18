#include <iostream>
#include <string>

#include "state_machine.hpp"

#define MODE    0
#define ACCEPT  1
#define REFUSE  2
#define DONE    3

#define INPUT   999
#define DIGITAL 1000
#define LETTER  1001
#define OVER    1002

inline const char* mode2String(int mode) {
	switch (mode) {
	case ACCEPT:
		return "ACCEPT";
	case REFUSE:
		return "REFUSE";
	case DONE:
		return "DONE";
	default:
		return "ERROR MODE";
	}
}

inline const char* input2String(int input) {
	switch (input) {
	case DIGITAL:
		return "DIGITAL";
	case LETTER:
		return "LETTER";
	case OVER:
		return "OVER";
	default:
		return "ERROR INPUT";
	}
}

inline bool isDigital(char c) {
	return (c >= 48) && (c <= 57);
}

inline bool isZeroOne(char c) {
	return (c == 48) || (c == 49);
}

void func(poppin::CStateMachine<int, int>* sm, void* arg) {
	//std::cout << input2String(*(int*)arg);
	//std::cout << " State: " << mode2String(sm->getState()) << std::endl;
}

void exitFunc(poppin::CStateMachine<int, int>* sm, void* arg) {
	sm->stop();
}

int main() {
	poppin::CStateMachine<int, int> sm;
	//sm.addStateChange(ACCEPT, DIGITAL, func, ACCEPT);
	sm.addStateChange(ACCEPT, LETTER, exitFunc, REFUSE);
	sm.addStateChange(ACCEPT, OVER, exitFunc, DONE);
	sm.init(ACCEPT, exitFunc);
	//sm.startInThread();
	std::string input;
	std::cin >> input;
	for (size_t i = 0; i < input.size(); ++i) {
		if (isDigital(input[i])) {
			sm.receiveImport(DIGITAL);
		} else {
			sm.receiveImport(LETTER);
		}
	}
	sm.receiveImport(OVER);
	sm.startInThread();
	auto& thread = sm.getThread();
	if (thread.joinable()) {
		thread.join();
	}
	std::cout << mode2String(sm.getState()) << std::endl;
	std::cin >> input;
}