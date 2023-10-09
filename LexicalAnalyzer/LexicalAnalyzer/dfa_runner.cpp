#include "pch.h"
#include "dfa.hpp"
#include "dfa_runner.hpp"
#include <algorithm>
#include <iostream>

void lex::dfa_runner::move(char c) {
	active_state_ = next_state(active_state_, c);
	buffer_ += std::string(1, c);

	//update length of longest matching prefix
	if (matches()) {
		matching_prefix_length_ = buffer_.size();
	}
}

lex::dfa::state lex::dfa_runner::next_state(dfa::state s, char t) {
	auto next = dfa_.trans_func_.find(std::pair<dfa::state, dfa::trans>(s, dfa::trans(t)));
	if (next != dfa_.trans_func_.end()) {
		//move by edge with normal terminal
		return next->second;
	}
	else {
		//move by edge with dot
		return dfa_.trans_func_[std::pair<dfa::state, dfa::trans>(s, dfa::trans())];
	}
}

void lex::dfa_runner::reset() {
	buffer_ = "";
	active_state_ = dfa_.start_;
	matching_prefix_length_ = -1;
}

void lex::dfa_runner::print_active() {
	std::cout << active_state_ << '\n';
}