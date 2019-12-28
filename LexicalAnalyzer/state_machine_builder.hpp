#ifndef STATE_MACHINE_BUILDER0__
#define STATE_MACHINE_BUILDER0__
#include <cstdint>
#include "state_machine.hpp"

class state_machine_builder {
private:
	//minimal number that can be used for new state
	int32_t min_state_;
	//returns unique state numbers
	int32_t new_state() {
		return min_state_++;
	}
	edge terminal_edge(int32_t to_state,char terminal) {
		edge e(to_state);
		e.terminal_ = terminal;
		return e;
	}
	edge any_terminal_edge(int32_t to_state) {
		edge e(to_state);
		e.any_symbol_ = true;
		return e;
	}
	edge lambda_edge(int32_t to_state) {
		edge e(to_state);
		e.lambda_ = true;
		return e;
	}
	//returns state machine created by merging states and transition functions of sm1 and sm2 
	//start and end are starting and ending states of the new state machine
	state_machine merge(state_machine& sm1, const state_machine& sm2);
public:
	explicit state_machine_builder():min_state_(0){}

	//state machine that accepts '.'
	state_machine dot_sm();

	//state machine that accepts terminal
	state_machine terminal_sm(char terminal);

	//state machine that accepts concatenation of regexps accepted by sm1 and sm2
	state_machine concat_sm(const state_machine& sm1, const state_machine& sm2);

	//state machine that accepts union of regexps accepted by sm1 and sm2
	state_machine union_sm(const state_machine& sm1, const state_machine& sm2);

	//state machine that accepts iteration of regexp accepted by sm1
	state_machine star_sm(const state_machine& sm1);

	//state machine that accepts positive iteration of regexp accepted by sm1
	state_machine plus_sm(const state_machine& sm1);
};

#endif