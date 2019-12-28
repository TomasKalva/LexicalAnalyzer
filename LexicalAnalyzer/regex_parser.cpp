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
	if (std::find(special_symbols_.begin(), special_symbols_.end(), input_[position_]) != special_symbols_.end()) {
		//the next symbol is a special symbol
		return false;
	}
	else {
		if (input_[position_] == '\\') {
			if (position_ + 1 < input_.length() &&
				std::find(special_symbols_.begin(), special_symbols_.end(), input_[position_ + 1]) != special_symbols_.end()){
				//the next symbol is a backslashed special symbol
				return true;
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

state_machine regex_parser::expr() {
	auto sm = term();
	sm.info_ = "E("+sm.info_;

	if (peek() == '|') {
		move();
		sm.info_+='|'+expr().info_;
	}
	sm.info_ += ")";
	std::cout << sm.info_ << '\n';
	return sm;
}
state_machine regex_parser::term() {
	auto sm = factor();
	sm.info_ = "T(" + sm.info_;
	if (!end()) {
		//term can end only by '|', ')' or end of input
		if (peek() != '|' && peek() != ')') {
			sm.info_ += term().info_;
		}
		//check if parenthesis are paired correctly
		if (peek() == ')' && open_parenth_ == 0) {
			error("closing parenthesis withou opening one");
		}
	}
	sm.info_ += ")";
	std::cout << sm.info_ << '\n';
	return sm;
}
state_machine regex_parser::factor() {
	auto sm = base();
	sm.info_ = "F(" + sm.info_;

	switch (peek()) {
	case '*':
		sm.info_ += "*";
		move();
		break;
	case '+':
		sm.info_ += "+";
		move();
		break;
	}
	sm.info_ += ")";
	std::cout << sm.info_ << '\n';
	return sm;
}
state_machine regex_parser::base() {
	state_machine sm;
	sm.info_ = "B(" + sm.info_;
	switch (peek()) {
	case '.':
		move();
		sm.info_ = ".";
		break;
	case '[':
		move();
		sm.info_+='['+repeat_terminal().info_;
		if (!match(']'))error("missing ']'");
		move();
		sm.info_ += ']';
		break;
	case '(':
		open_parenth_++;
		move();
		sm.info_ += '(' + expr().info_;
		if (!match(')'))error("missing ')'");
		open_parenth_--;
		move();
		sm.info_ += ')';
		break;
	default:
		//terminal
		sm.info_ += terminal().info_;
		break;
	}
	sm.info_ += ")";
	std::cout << sm.info_ << '\n';
	return sm;
}
state_machine regex_parser::repeat_terminal() {
	auto sm = terminal();
	//repeat terminal ends with character that isn't terminal
	if (peek_terminal()) {
		sm.info_+=repeat_terminal().info_;
	}
	std::cout << sm.info_ << '\n';
	return sm;
}
state_machine regex_parser::terminal() {
	state_machine sm;
	if(peek_terminal()){
		if (input_[position_] == '\\') {
			//the next terminal has 2 characters
			sm.info_ = input_.substr(position_, 2);
			move(); move();
		}
		else {
			//the next terminal has 1 character
			sm.info_ = input_.substr(position_, 1);
			move();
		}
	}
	else {
		error("expected terminal");
	}
	std::cout << sm.info_<<'\n';
	return sm;
}