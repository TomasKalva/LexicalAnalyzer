#ifndef REGEX_PARSER0__
#define REGEX_PARSER0__

#include <string>
#include <vector>
#include "nfa.hpp"
#include "nfa_builder.hpp"

class regex_parser {
private:
	std::string input_;
	std::size_t position_;
	//number of currently open parentheses
	int32_t open_parenth_;
	std::vector<char> special_symbols_;
	std::vector<char> backslashable_letters_;
	nfa_builder smb_;

	char next();
	bool match(const char c);
	char peek();
	bool end();
	void error(std::string message);
	bool peek_terminal();
	void move();
	bool can_be_backslashed(char c);
	char backslashed_value(char c);

	nfa expr();
	nfa term();
	nfa factor();
	nfa base();
	nfa repeat_terminal();
	nfa terminal();
public:
	explicit regex_parser(const std::string& input) :
		input_(input),
		position_(0),
		open_parenth_(0),
		special_symbols_
	{'|','*','+','.','[',']','(',')','\\'},
		backslashable_letters_
	{'n','t','r'}{

	}
	nfa create_machine();
};
#endif