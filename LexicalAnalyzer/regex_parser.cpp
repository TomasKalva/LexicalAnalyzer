#include "regex_parser.hpp"
#include <iostream>
#include <algorithm>

void regex_parser::move() {
	position_++;
}
char regex_parser::next() {
	move();
	return input_[position_-1];
}
bool regex_parser::match(const char c) {
	if (end())
		return false;
	return input_[position_] == c;
}
char regex_parser::peek() {
	return input_[position_];
}
bool regex_parser::end() {
	return position_ >= input_.length();
}
void regex_parser::error(std::string message) {
	throw std::invalid_argument("Error at position " + std::to_string(position_)+": "+message);
}
bool regex_parser::peek_terminal() {
	if (input_[position_]!='\\' &&
		std::find(special_symbols_.begin(), special_symbols_.end(), input_[position_]) != special_symbols_.end()) {
		//the next symbol is a special symbol
		return false;
	}
	else {
		if (input_[position_] == '\\') {
			if (position_ + 1 < input_.length()){
				if (can_be_backslashed(input_[position_+1])) {
					//the next symbol is a backslashed special symbol
					return true;
				}
				else {
					error("character " + std::string(1,input_[position_ + 1]) + " can\'t be backslashed");
				}
			}
			else {
				//input cant be ended with '\' and non special symbol can't be backslashed
				error("input ends with '\'");
			}
		}
		else {
			//the next symbol is a terminal
			return true;
		}
	}
}
bool regex_parser::can_be_backslashed(char c) {
	if (std::find(special_symbols_.begin(), special_symbols_.end(), c) != special_symbols_.end() ||
		std::find(backslashable_letters_.begin(), backslashable_letters_.end(), c) != backslashable_letters_.end()) {
		return true;
	}
	else {
		return false;
	}
}
char regex_parser::backslashed_value(char c) {
	switch (c) {
	case 'n':return '\n';
	case 't':return '\t';
	case 'r':return '\r';
	default: return c;
	}
}

nfa regex_parser::create_machine() {
	if (input_.length() == 0)
		return smb_.empty_sm();
	else
		return expr();
}

nfa regex_parser::expr() {
	auto sm = term();

	if (peek() == '|') {
		move();
		auto expr_sm = expr();
		sm = smb_.union_sm(sm, expr_sm);
	}
	std::cout << sm.expression_ << '\n';
	return sm;
}

nfa regex_parser::term() {
	auto sm = factor();

	if (!end()) {
		//term can end only by '|', ')' or end of input
		if (peek() != '|' && peek() != ')') {
			auto term_sm = term();
			sm = smb_.concat_sm(sm, term_sm);
		}
		//check if parenthesis are paired correctly
		if (peek() == ')' && open_parenth_ == 0) {
			error("closing parenthesis withou opening one");
		}
	}
	std::cout << sm.expression_ << '\n';
	return sm;
}

nfa regex_parser::factor() {
	auto sm = base();

	switch (peek()) {
	case '*':
		sm = smb_.star_sm(sm);
		move();
		break;
	case '+':
		sm = smb_.plus_sm(sm);
		move();
		break;
	}
	std::cout << sm.expression_ << '\n';
	return sm;
}

nfa regex_parser::base() {
	nfa sm;

	switch (peek()) {
	case '.':
		move();
		sm = smb_.dot_sm();
		break;
	case '[':
		move();
		sm = repeat_terminal();
		if (!match(']'))error("missing ']'");
		move();
		break;
	case '(':
		open_parenth_++;
		move();
		sm = expr();
		if (!match(')'))error("missing ')'");
		open_parenth_--;
		move();
		break;
	default:
		//terminal
		sm = terminal();
		break;
	}
	std::cout << sm.expression_ << '\n';
	return sm;
}

nfa regex_parser::repeat_terminal() {
	auto sm = terminal();
	//repeat terminal ends with character that isn't terminal
	if (!end() && peek_terminal()) {
		auto repeat_sm = repeat_terminal();
		sm = smb_.union_sm(sm, repeat_sm);
	}
	std::cout << sm.expression_ << '\n';
	return sm;
}

nfa regex_parser::terminal() {
	nfa sm;
	if(peek_terminal()){
		if (input_[position_] == '\\') {
			//the next terminal has 2 characters
			char c = input_[position_ + 1];
			char terminal = backslashed_value(c);
			sm = smb_.terminal_sm(terminal);
			move(); move();
		}
		else {
			//the next terminal has 1 character
			sm = smb_.terminal_sm(input_[position_]);
			move();
		}
	}
	else {
		error("expected terminal");
	}
	std::cout << sm.expression_<<'\n';
	return sm;
}