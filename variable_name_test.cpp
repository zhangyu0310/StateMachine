#include <iostream>
#include <string>

#include "state_machine.hpp"

#define BEGIN  1
#define GET    2
#define DONE   3

inline bool isDigital(char c) {
	return (c >= 48) && (c <= 57);
}
inline bool isLetter(char c) {
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

int main() {
	poppin::CStateMachine<int, std::string> sm;
	sm.init(BEGIN, poppin::CStateMachine<int, std::string>::exitFunction);
	sm.addStateChange(BEGIN, "letter", nullptr, GET);
	sm.addStateChange(BEGIN, "underline", nullptr, GET);
	sm.addStateChange(GET, "letter", nullptr, GET);
	sm.addStateChange(GET, "underline", nullptr, GET);
	sm.addStateChange(GET, "number", nullptr, GET);
	sm.addStateChange(GET, "terminator", poppin::CStateMachine<int, std::string>::exitFunction, DONE);

	std::string var_name;
	std::cin >> var_name;
	const char *str = var_name.c_str();
	for (int i = 0; i <= var_name.size(); ++i) {
		char ch = str[i];
		if (isLetter(ch)) {
			sm.receiveImport("letter");
		} else if (isDigital(ch)) {
			sm.receiveImport("number");
		} else if (ch == '_') {
			sm.receiveImport("underline");
		} else if (ch == '\0') {
			sm.receiveImport("terminator");
		} else {
			sm.receiveImport("unknow");
		}
	}
	sm.start();
	if (sm.getState() == DONE) {
		std::cout << "ACCEPT" << std::endl;
	} else {
		std::cout << "REFUSE" << std::endl;
	}
}