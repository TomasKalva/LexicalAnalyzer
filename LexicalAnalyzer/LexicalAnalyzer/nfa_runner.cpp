#include "pch.h"
#include "nfa.hpp"
#include "nfa_runner.hpp"
#include <algorithm>
#include <iostream>

void lex::nfa_runner::move(char c) {
	active_states_ = next_state(active_states_, c);
	buffer_ += std::string(1,c);

	//update length of longest matching prefix
	if (matches()) {
		matching_prefix_length_ = buffer_.size();
	}
}

std::vector<int32_t> lex::nfa_runner::lambda_closure(const std::vector<int32_t>& states) const {
	std::vector<int32_t> lambda_closure = states;
	bool added_something = true;
	while (added_something) {
		added_something = false;
		for (size_t i = 0; i < lambda_closure.size();i++) {
			int32_t state = lambda_closure[i];
			for (edge e : nfa_.transition_func_[state]) {
				if (e.lambda_ && 
					std::find(lambda_closure.begin(),lambda_closure.end(),e.to_state_)==lambda_closure.end()) {
					lambda_closure.push_back(e.to_state_);
					added_something = true;
				}
			}
		}
	}
	return lambda_closure;
}

std::vector<int32_t> lex::nfa_runner::next_state(const std::vector<int32_t>& states, char c) const {
	std::vector<int32_t> new_states;
	for (int32_t state : states) {
		for (edge e : nfa_.transition_func_[state]) {
			if (e.any_terminal_ ||
				e.terminal_ == c) {
				new_states.push_back(e.to_state_);
			}
		}
	}
	new_states = lambda_closure(new_states);
	return new_states;
}

std::vector<int32_t> lex::nfa_runner::next_state_by_dots(const std::vector<int32_t>& states) const {
	std::vector<int32_t> new_states;
	for (int32_t state : states) {
		for (edge e : nfa_.transition_func_[state]) {
			if (e.any_terminal_) {
				new_states.push_back(e.to_state_);
			}
		}
	}
	new_states = lambda_closure(new_states);
	return new_states;
}

void lex::nfa_runner::reset() {
	buffer_ = "";
	active_states_ = lambda_closure(std::vector<int32_t>{ nfa_.start_ });
	matching_prefix_length_ = -1;
}

void lex::nfa_runner::print_active() {
	for (auto state : active_states_) {
		std::cout << state << ' ';
	}
	std::cout << '\n';
}