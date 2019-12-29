#ifndef DFA_BUILDER0__
#define DFA_BUILDER0__
#include "nfa_runner.hpp"
#include <algorithm>
#include "dfa.hpp"

class dfa_builder {
private:
	int32_t min_state_;
	std::unordered_map<std::string, int32_t> state_numbers_;
	dfa dfa_;
public:
	dfa create_dfa(nfa_runner& mr);

	//returns key of state to state_numbers_
	std::string state_key(std::vector<int32_t> state) {
		std::sort(state.begin(), state.end());
		std::string key;
		for (auto s : state)
			key += std::to_string(s) + ",";
		return key;
	}

	dfa::state get_state_number(std::vector<int32_t> state);

	void add_state_number(std::vector<int32_t> state) {
		std::string key = state_key(state);
		state_numbers_[key] = min_state_++;
	}
};

#endif