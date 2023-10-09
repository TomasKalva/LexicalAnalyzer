#ifndef DFA_RUNNER0__
#define DFA_RUNNER0__
#include <cstdint>
#include <vector>
#include "dfa.hpp"
#include <string>

#ifdef LEXICALANALYZER_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

namespace lex {
	class dfa_runner {
	private:
		dfa dfa_;
		dfa::state active_state_;
		std::string buffer_;
		size_t matching_prefix_length_;
	public:
		friend dfa_factory;

		explicit EXPORT dfa_runner(dfa dfa) :dfa_(dfa) {
			reset();
		}

		//returns state reached by terminal t from s
		dfa::state next_state(dfa::state s, char t);

		//move active states by c
		void EXPORT move(char c);

		//resets buffer and active state
		void EXPORT reset();

		//true if the machine matches the input
		bool EXPORT matches() {
			return std::find(dfa_.end_states_.begin(), dfa_.end_states_.end(), active_state_) != dfa_.end_states_.end();
		}

		//prints active state
		void print_active();

		//returns true if prefix of the input word matches the expression
		bool EXPORT has_matching_prefix() {
			return matching_prefix_length_ != -1;
		}

		//returns valid value only when has_matching_prefix() return true
		std::string EXPORT longest_matching_prefix() {
			if (matching_prefix_length_ >= 0)
				return buffer_.substr(0, matching_prefix_length_);
			else
				return "";
		}

		bool EXPORT failed() const {
			return dfa_.fail_states_.find(active_state_) != dfa_.fail_states_.end();
		}
	};
}

#endif