#ifndef DFA_RUNNER0__
#define DFA_RUNNER0__
#include <cstdint>
#include <vector>
#include "dfa.hpp"
#include <string>

class dfa_runner {
private:
	dfa dfa_;
	dfa::state active_state_;
	std::string buffer_;
	int32_t matching_prefix_length_;
public:
	friend dfa_builder;

	explicit dfa_runner(dfa dfa) :dfa_(dfa) {
		reset();
	}

	//returns state reached by terminal t from s
	dfa::state next_state(dfa::state s, char t);

	//move active states by c
	void move(char c);

	//resets buffer and active state
	void reset();

	//true if the machine matches the input
	bool matches() {
		return std::find(dfa_.end_states_.begin(), dfa_.end_states_.end(), active_state_) != dfa_.end_states_.end();
	}

	//prints active state
	void print_active();

	//returns true if prefix of the input word matches the expression
	bool has_matching_prefix() {
		return matching_prefix_length_ != -1;
	}

	//returns valid value only when has_matching_prefix() return true
	std::string longest_matching_prefix() {
		if (matching_prefix_length_ >= 0)
			return buffer_.substr(0, matching_prefix_length_);
		else
			return "";
	}
};

#endif