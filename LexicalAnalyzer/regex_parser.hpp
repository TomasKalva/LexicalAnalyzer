#ifndef REGEX_PARSER0__
#define REGEX_PARSER0__

#include <string>
#include <vector>
#include "state_machine.hpp"

class regex_parser {
private:
	std::string input_;
	std::size_t position_;
	std::vector<char> special_symbols_;
	int32_t open_parenth_;
public:
	explicit regex_parser(std::string input) :
		input_(input),
		position_(0),
		open_parenth_(0),
		special_symbols_
	{'|','*','+','.','[',']','(',')','\\'} {

	}
	char next();
	bool match(const char c);
	char peek();
	bool end();
	void error(std::string message);
	bool peek_terminal();
	void move();

	state_machine expr();
	state_machine term();
	state_machine factor();
	state_machine base();
	state_machine repeat_terminal();
	state_machine terminal();
};
#endif