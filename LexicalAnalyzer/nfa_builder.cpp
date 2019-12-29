#include "nfa_builder.hpp"
#include "nfa.hpp"

nfa nfa_builder::merge(nfa& sm1, const nfa& sm2) {
	sm1.states_.insert(sm1.states_.end(), sm2.states_.begin(), sm2.states_.end());
	sm1.transition_func_.insert(sm2.transition_func_.begin(), sm2.transition_func_.end());
	return sm1;
}

nfa nfa_builder::empty_sm() {
	auto start = new_state();
	nfa sm(start, start);

	sm.expression_ = "";
	return sm;
}

nfa nfa_builder::dot_sm() {
	auto start = new_state();
	auto end = new_state();
	nfa sm(start,end);

	sm.add_edge(start,(any_terminal_edge(end)));

	sm.expression_ = ".";
	return sm;
}

nfa nfa_builder::terminal_sm(char terminal) {
	auto start = new_state();
	auto end = new_state();
	nfa sm(start, end);

	sm.add_edge(start, (terminal_edge(end, terminal)));

	sm.expression_ = terminal;
	return sm;
}

nfa nfa_builder::concat_sm(const nfa& sm1, const nfa& sm2) {
	auto start = new_state();
	auto end = new_state();
	nfa sm(start, end);
	sm = merge(sm, sm1);
	sm = merge(sm, sm2);

	sm.add_edge(start, (lambda_edge(sm1.start_)));
	sm.add_edge(sm1.end_, (lambda_edge(sm2.start_)));
	sm.add_edge(sm2.end_, (lambda_edge(end)));

	sm.expression_ = sm1.expression_+sm2.expression_;
	return sm;
}

nfa nfa_builder::union_sm(const nfa& sm1, const nfa& sm2) {
	auto start = new_state();
	auto end = new_state();
	nfa sm(start, end);
	sm = merge(sm, sm1);
	sm = merge(sm, sm2);

	sm.add_edge(start, (lambda_edge(sm1.start_)));
	sm.add_edge(start, (lambda_edge(sm2.start_)));
	sm.add_edge(sm1.end_, (lambda_edge(end)));
	sm.add_edge(sm2.end_, (lambda_edge(end)));

	sm.expression_ = '(' + sm1.expression_ + ")|(" + sm2.expression_ + ')';
	return sm;
}

nfa nfa_builder::star_sm(const nfa& sm1) {
	nfa sm = plus_sm(sm1);

	sm.add_edge(sm.start_, (lambda_edge(sm.end_)));

	sm.expression_ = '(' + sm1.expression_ + ')' + '*';
	return sm;
}

nfa nfa_builder::plus_sm(const nfa& sm1) {
	auto start = new_state();
	auto end = new_state();
	nfa sm(start, end);
	sm = merge(sm, sm1);

	sm.add_edge(start, (lambda_edge(sm1.start_)));
	sm.add_edge(sm1.end_, (lambda_edge(end)));
	sm.add_edge(end, (lambda_edge(start)));

	sm.expression_ = '(' + sm1.expression_ + ')' + '+';
	return sm;
}