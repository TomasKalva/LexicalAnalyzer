#ifndef STATE_MACHINE_BUILDER0__
#define STATE_MACHINE_BUILDER0__
#include <cstdint>
#include "nfa.hpp"

namespace lex {
	class nfa_builder {
	private:
		//minimal number that can be used for new state
		int32_t min_state_;
		//returns unique state numbers
		int32_t new_state() {
			return min_state_++;
		}
		edge terminal_edge(int32_t to_state, char terminal) {
			edge e(to_state);
			e.terminal_ = terminal;
			return e;
		}
		edge any_terminal_edge(int32_t to_state) {
			edge e(to_state);
			e.any_terminal_ = true;
			return e;
		}
		edge lambda_edge(int32_t to_state) {
			edge e(to_state);
			e.lambda_ = true;
			return e;
		}
		//returns state machine created by merging states and transition functions of sm1 and sm2 
		//start and end are starting and ending states of the new state machine
		nfa merge(nfa& sm1, const nfa& sm2);
	public:
		nfa_builder() :min_state_(0) {}

		//state machine that accepts empty word
		nfa empty_nfa();

		//state machine that accepts '.'
		nfa dot_nfa();

		//state machine that accepts terminal
		nfa terminal_nfa(char terminal);

		//state machine that accepts union of terminals, if complement is true,
		//the state machine accepts union of complement of terminals
		nfa union_nfa(const std::vector<char> terminals);

		//state machine that accepts concatenation of terminals
		lex::nfa concat_nfa(const std::vector<char> terminals);

		//state machine that accepts concatenation of regexps accepted by sm1 and sm2
		nfa concat_nfa(const nfa& sm1, const nfa& sm2);

		//state machine that accepts union of regexps accepted by sm1 and sm2
		nfa union_nfa(const nfa& sm1, const nfa& sm2);

		//state machine that accepts iteration of regexp accepted by sm1
		nfa star_nfa(const nfa& sm1);

		//state machine that accepts positive iteration of regexp accepted by sm1
		nfa plus_nfa(const nfa& sm1);
	};
}
#endif