#include "pch.h"
#include "nfa_runner.hpp"
#include "dfa_factory.hpp"
#include <stack>
#include "regex_parser.hpp"

lex::dfa lex::dfa_factory::create_dfa(std::string regexp) {
	reset();

	regex_parser rp(regexp, false);
	nfa nfa = rp.create_machine();
	nfa_runner nfa_runner(nfa);
	dfa dfa;

	using vec_state = std::vector<int32_t>;

	add_state_number(nfa_runner.active_states_);
	dfa.start_ = get_state_number(nfa_runner.active_states_);
	dfa.add_state(dfa.start_);

	std::stack<vec_state> discovered;
	discovered.push(nfa_runner.active_states_);
	while (!discovered.empty()) {
		vec_state from = discovered.top();
		dfa::state from_num = get_state_number(from);
		discovered.pop();

		//make the state end state if it contains end state
		if (nfa_runner.contains_end_state(from)) {
			dfa.end_states_.insert(from_num);
		}

		//check all edges from from
		auto terminals_ = nfa_runner.nfa_.transition_chars(from);
		for (char t : terminals_) {
			vec_state to = nfa_runner.next_state(from, t);
			dfa::state to_num = get_state_number(to);
			if (to_num == -1) {
				//discover and add new state
				add_state_number(to);
				to_num = get_state_number(to);
				discovered.push(to);
				dfa.add_state(to_num);
			}
			//add new edge
			dfa.add_terminal_edge(from_num, to_num, t);
		}

		if (terminals_.size() < 256) {
			//add transition by other symbols
			vec_state to = nfa_runner.next_state_by_dots(from);
			dfa::state to_num = get_state_number(to);
			if (to_num == -1) {
				//discover and add new state
				add_state_number(to);
				to_num = get_state_number(to);
				discovered.push(to);
				dfa.add_state(to_num);
			}
			//add new edge
			dfa.add_others_edge(from_num, to_num);
		}
	}
	dfa.find_fail_states();

	return dfa;
} 

