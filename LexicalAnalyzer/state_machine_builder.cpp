
#include "state_machine_builder.hpp"
#include "state_machine.hpp"

state_machine state_machine_builder::merge(state_machine& sm1, const state_machine& sm2) {
	sm1.states_.insert(sm1.states_.end(), sm2.states_.begin(), sm2.states_.end());
	sm1.transition_func_.insert(sm2.transition_func_.begin(), sm2.transition_func_.end());
	return sm1;
}

state_machine state_machine_builder::dot_sm() {
	auto start = new_state();
	auto end = new_state();
	state_machine sm(start,end);

	sm.add_edge(start,(any_terminal_edge(end)));
	return sm;
}

state_machine state_machine_builder::terminal_sm(char terminal) {
	auto start = new_state();
	auto end = new_state();
	state_machine sm(start, end);

	sm.add_edge(start, (terminal_edge(end, terminal)));
	return sm;
}

state_machine state_machine_builder::concat_sm(const state_machine& sm1, const state_machine& sm2) {
	auto start = new_state();
	auto end = new_state();
	state_machine sm(start, end);
	sm = merge(sm, sm1);
	sm = merge(sm, sm2);

	sm.add_edge(start, (lambda_edge(sm1.start_)));
	sm.add_edge(sm1.end_, (lambda_edge(sm2.start_)));
	sm.add_edge(sm2.end_, (lambda_edge(end)));
	return sm;
}

state_machine state_machine_builder::union_sm(const state_machine& sm1, const state_machine& sm2) {
	auto start = new_state();
	auto end = new_state();
	state_machine sm(start, end);
	sm = merge(sm, sm1);
	sm = merge(sm, sm2);

	sm.add_edge(start, (lambda_edge(sm1.start_)));
	sm.add_edge(start, (lambda_edge(sm2.start_)));
	sm.add_edge(sm1.end_, (lambda_edge(end)));
	sm.add_edge(sm2.end_, (lambda_edge(end)));
	return sm;
}

state_machine state_machine_builder::star_sm(const state_machine& sm1) {
	state_machine sm = plus_sm(sm1);

	sm.add_edge(sm.start_, (lambda_edge(sm.end_)));
	return sm;
}

state_machine state_machine_builder::plus_sm(const state_machine& sm1) {
	auto start = new_state();
	auto end = new_state();
	state_machine sm(start, end);
	sm = merge(sm, sm1);

	sm.add_edge(start, (lambda_edge(sm1.start_)));
	sm.add_edge(sm1.end_, (lambda_edge(end)));
	sm.add_edge(end, (lambda_edge(start)));
	return sm;
}