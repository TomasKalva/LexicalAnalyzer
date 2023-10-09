#ifndef MACHINE_RUNNER0__
#define MACHINE_RUNNER0__
#include "nfa.hpp"

#ifdef LEXICALANALYZER_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

namespace lex {
	class dfa_factory;

	class nfa_runner {
	private:
		nfa& nfa_;
		std::vector<int32_t> active_states_;
		std::string buffer_;
		size_t matching_prefix_length_;
	public:
		friend dfa_factory;

		explicit EXPORT nfa_runner(nfa& nfa) :nfa_(nfa) {
			reset();
		}

		//computes lambda closure of states
		std::vector<int32_t> lambda_closure(const std::vector<int32_t>& states) const;

		//returns lambda closure of states reachable from state by c
		std::vector<int32_t> next_state(const std::vector<int32_t>& state, char c) const;

		//returns lambda closure of states reachable from state by only dot-edges
		std::vector<int32_t> next_state_by_dots(const std::vector<int32_t>& state) const;

		//move active states by c
		void move(char c);

		//resets buffer and active states
		void reset();

		//returns true if states contain end state
		bool contains_end_state(const std::vector<int32_t> states) const {
			return std::find(states.begin(), states.end(), nfa_.end_) != states.end();
		}

		//true if the machine matches the input
		bool matches() {
			return contains_end_state(active_states_);
		}

		//prints active states
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
}

#endif