#ifndef DFA_BUILDER0__
#define DFA_BUILDER0__
#include "nfa_runner.hpp"
#include <algorithm>
#include "dfa.hpp"

#ifdef LEXICALANALYZER_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

namespace lex {
	class dfa_factory {
	private:
		//minimal number that can be used for new state
		int32_t min_state_;
		//contains comma separated list of nfa states with corresponding dfa state
		std::unordered_map<std::string, int32_t> state_numbers_;
		dfa dfa_;

		//resets min_state_ and state_numbers_
		void reset() {
			min_state_ = 0;
			state_numbers_.clear();
			dfa dfa;
		}
	public:
		dfa_factory() :min_state_(0) {}

		dfa EXPORT create_dfa(std::string regexp);

		//returns key of state to state_numbers_
		std::string state_key(std::vector<int32_t> state) {
			std::sort(state.begin(), state.end());
			std::string key;
			for (auto s : state)
				key += std::to_string(s) + ",";
			return key;
		}

		dfa::state get_state_number(std::vector<int32_t> state) {
			std::string key = state_key(state);
			auto st = state_numbers_.find(key);
			if (st != state_numbers_.end())
				return st->second;
			else
				return -1;
		}

		//generates number for state and adds it to state_numbers_
		void add_state_number(std::vector<int32_t> state) {
			std::string key = state_key(state);
			state_numbers_[key] = min_state_++;
		}
	};
}

#endif