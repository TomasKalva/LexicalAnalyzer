#include "pch.h"
#include "regex_parser.hpp"
#include <iostream>
#include <algorithm>

void lex::regex_parser::move() {
	position_++;
}

bool lex::regex_parser::match(const char c) {
	if (end())
		return false;
	return input_[position_] == c;
}

char lex::regex_parser::peek() {
	return input_[position_];
}

bool lex::regex_parser::end() {
	return position_ >= input_.length();
}

void lex::regex_parser::error(std::string message) {
	throw std::invalid_argument("Error at position " + std::to_string(position_)+": "+message);
}

bool lex::regex_parser::peek_terminal() {
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
					//non special symbol can't be backslashed
					error("character " + std::string(1,input_[position_ + 1]) + " can\'t be backslashed");
					return false;//to prevent warning not all control paths return value
				}
			}
			else {
				//input cant be ended with '\' and
				error("input ends with '\'");
				return false;//to prevent warning not all control paths return value
			}
		}
		else {
			//the next symbol is a terminal
			return true;
		}
	}
}

bool lex::regex_parser::can_be_backslashed(char c) {
	if (std::find(special_symbols_.begin(), special_symbols_.end(), c) != special_symbols_.end() ||
		std::find(backslashable_letters_.begin(), backslashable_letters_.end(), c) != backslashable_letters_.end()) {
		return true;
	}
	else {
		return false;
	}
}

char lex::regex_parser::backslashed_value(char c) {
	switch (c) {
	case 'n':return '\n';
	case 't':return '\t';
	case 'r':return '\r';
	default: return c;
	}
}

void lex::regex_parser::expand(std::vector<char>& terminals) {
	if (terminals.size() == 0)
		return;

	bool expaz = false;;
	bool expAZ = false;;
	bool exp09 = false;;
	for (size_t i = 1; i < terminals.size() - 1; i++) {
		if (terminals[i] == '-') {
			bool remove = false;
			//a-z
			if (terminals[i - 1] == 'a' &&
				terminals[i + 1] == 'z' &&
				!expaz) {
				remove = true;
				expaz = true;
				for (size_t c = 'a'; c <= 'z'; c++)
					terminals.push_back((char)c);
			}
			//A-Z
			if (terminals[i - 1] == 'A' &&
				terminals[i + 1] == 'Z' &&
				!expAZ) {
				remove = true;
				expAZ = true;
				for (size_t c = 'A'; c <= 'Z'; c++)
					terminals.push_back((char)c);
			}
			//0-9
			if (terminals[i - 1] == '0' &&
				terminals[i + 1] == '9' &&
				!exp09) {
				remove = true;
				exp09 = true;
				for (size_t c = '0'; c <= '9'; c++)
					terminals.push_back((char)c);
			}
			if (remove) {
				terminals.erase(terminals.begin() + i + 1);
				terminals.erase(terminals.begin() + i);
				terminals.erase(terminals.begin() + i - 1);
				i--;
			}
		}
	}
}

lex::nfa lex::regex_parser::create_machine() {
	if (input_.length() == 0)
		return nfa_builder_.empty_nfa();
	else
		return expr();
}

lex::nfa lex::regex_parser::expr() {
	depth_++;
	auto nfa = term();

	if (peek() == '|') {
		move();
		if (end())
			error("empty right side of '|'");
		auto expr_nfa = expr();
		nfa = nfa_builder_.union_nfa(nfa, expr_nfa);
	}
	depth_--;
	print("e", nfa.expression());
	return nfa;
}

lex::nfa lex::regex_parser::term() {
	depth_++;
	auto nfa = factor();

	if (!end()) {
		//term can end only by '|', ')' or end of input
		if (peek() != '|' && peek() != ')') {
			auto term_nfa = term();
			nfa = nfa_builder_.concat_nfa(nfa, term_nfa);
		}
		//check if parenthesis are paired correctly
		if (peek() == ')' && open_parenth_ == 0) {
			error("closing parenthesis without opening one");
		}
	}
	depth_--;
	print("t", nfa.expression());
	return nfa;
}

lex::nfa lex::regex_parser::factor() {
	depth_++;
	auto nfa = base();

	switch (peek()) {
	case '*':
		nfa = nfa_builder_.star_nfa(nfa);
		move();
		break;
	case '+':
		nfa = nfa_builder_.plus_nfa(nfa);
		move();
		break;
	}
	depth_--;
	print("f", nfa.expression());
	return nfa;
}

lex::nfa lex::regex_parser::base() {
	depth_++;
	nfa nfa;

	switch (peek()) {
	case '.': {
		move();
		nfa = nfa_builder_.dot_nfa();
		break;
	}
	case '[': {
		move();
		std::vector<char> terminals = repeat_terminal();
		expand(terminals);
		nfa = nfa_builder_.union_nfa(terminals);
		if (!match(']'))error("missing ']'");
		move();
		break;
	}
	case '(': {
		open_parenth_++;
		move();
		nfa = expr();
		if (!match(')'))error("missing ')'");
		open_parenth_--;
		move();
		break;
	}
	default: {
		//terminal
		std::vector<char> terminals = repeat_terminal();
		if (terminals.size() == 0) error("expected terminal");
		nfa = nfa_builder_.concat_nfa(terminals);
		break;
	}
	}

	depth_--;
	print("b", nfa.expression());
	return nfa;
}

std::vector<char> lex::regex_parser::repeat_terminal() {
	//repeat terminal ends with character that isn't terminal
	std::vector<char> terminals;
	while (!end() && peek_terminal()) {
		auto nfa = terminal();
		terminals.push_back(nfa.expression()[0]);
	}
	return terminals;
}

lex::nfa lex::regex_parser::terminal() {
	nfa nfa;
	if(peek_terminal()){
		if (input_[position_] == '\\') {
			//the next terminal has 2 characters
			char c = input_[position_ + 1];
			char terminal = backslashed_value(c);
			nfa = nfa_builder_.terminal_nfa(terminal);
			move(); move();
		}
		else {
			//the next terminal has 1 character
			nfa = nfa_builder_.terminal_nfa(input_[position_]);
			move();
		}
	}
	else {
		error("expected terminal");
	}
	return nfa;
}