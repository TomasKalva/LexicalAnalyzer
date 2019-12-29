#include "nfa_runner.hpp"
#include "dfa_builder.hpp"
#include <stack>

dfa dfa_builder::create_dfa(nfa_runner& mr) {
	using vec_state = std::vector<int32_t>;
	dfa dfa;
	mr.reset();

	add_state_number(mr.active_states_);
	dfa.start_ = get_state_number(mr.active_states_);

	std::stack<vec_state> discovered;
	discovered.push(mr.active_states_);
	while (!discovered.empty()) {
		vec_state from = discovered.top();
		dfa::state from_num = get_state_number(from);
		discovered.pop();


		//check all edges from from
		auto terminals_ = mr.nfa_.transition_chars(from);
		for (char t : terminals_) {
			vec_state to = mr.next_state(from, t);
			dfa::state to_num = get_state_number(to);
			if (to_num == -1) {
				//discover and add new state
				add_state_number(to);
				to_num = get_state_number(to);
				discovered.push(to);
				dfa.add_state(to_num);
				
				//make the state end state if it contains end state
				if (mr.contains_end_state(to)) {
					dfa.end_states_.push_back(to_num);
				}
			}
			//add new edge
			dfa.add_symbol_edge(from_num, to_num, t);
		}

		if (terminals_.size() < 256) {
			//add transition by dot edge
			vec_state to = mr.next_state_by_dots(from);
			dfa::state to_num = get_state_number(to);
			if (to_num == -1) {
				//discover and add new state
				add_state_number(to);
				to_num = get_state_number(to);
				discovered.push(to);
				dfa.add_state(to_num);

				//make the state end state if it contains end state
				if (mr.contains_end_state(to)) {
					dfa.end_states_.push_back(to_num);
				}
			}
			//add new edge
			dfa.add_others_edge(from_num, to_num);
		}
	}
	return dfa;
} 

dfa::state dfa_builder::get_state_number(std::vector<int32_t> state) {
	std::string key = state_key(state);
	auto st = state_numbers_.find(key);
	if (st != state_numbers_.end())
		return st->second;
	else
		return -1;
}