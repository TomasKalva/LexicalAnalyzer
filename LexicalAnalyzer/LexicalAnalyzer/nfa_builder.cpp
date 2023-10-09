#include "pch.h"
#include "nfa_builder.hpp"
#include "nfa.hpp"

lex::nfa lex::nfa_builder::merge(nfa& nfa1, const nfa& nfa2) {
	nfa1.states_.insert(nfa1.states_.end(), nfa2.states_.begin(), nfa2.states_.end());
	nfa1.transition_func_.insert(nfa2.transition_func_.begin(), nfa2.transition_func_.end());
	return nfa1;
}

lex::nfa lex::nfa_builder::empty_nfa() {
	auto start = new_state();
	nfa nfa(start, start);

	nfa.expression_ = "";
	return nfa;
}

lex::nfa lex::nfa_builder::dot_nfa() {
	auto start = new_state();
	auto end = new_state();
	nfa nfa(start,end);

	nfa.add_edge(start,(any_terminal_edge(end)));

	nfa.expression_ = ".";
	return nfa;
}

lex::nfa lex::nfa_builder::terminal_nfa(char terminal) {
	auto start = new_state();
	auto end = new_state();
	nfa nfa(start, end);

	nfa.add_edge(start, (terminal_edge(end, terminal)));

	nfa.expression_ = terminal;
	return nfa;
}

lex::nfa lex::nfa_builder::union_nfa(const std::vector<char> terminals) {
	auto start = new_state();
	auto end = new_state();
	nfa nfa(start, end);

	nfa.expression_ = "[";
	for (char terminal : terminals) {
		nfa.add_edge(start, (terminal_edge(end, terminal)));
		nfa.expression_ += terminal;
	}
	nfa.expression_ += "]";

	return nfa;
}

lex::nfa lex::nfa_builder::concat_nfa(const std::vector<char> terminals) {
	auto start = new_state();
	auto end = new_state();
	nfa nfa(start, end);
	int32_t prev = start;
	int32_t next;
	for (char terminal : terminals) {
		next = new_state();
		nfa.add_edge(prev, (terminal_edge(next, terminal)));
		nfa.expression_ += terminal;
		prev = next;
	}
	nfa.add_edge(prev, (lambda_edge(end)));

	return nfa;
}

lex::nfa lex::nfa_builder::concat_nfa(const nfa& nfa1, const nfa& nfa2) {
	auto start = new_state();
	auto end = new_state();
	nfa nfa(start, end);
	nfa = merge(nfa, nfa1);
	nfa = merge(nfa, nfa2);

	nfa.add_edge(start, (lambda_edge(nfa1.start_)));
	nfa.add_edge(nfa1.end_, (lambda_edge(nfa2.start_)));
	nfa.add_edge(nfa2.end_, (lambda_edge(end)));

	nfa.expression_ = nfa1.expression_+nfa2.expression_;
	return nfa;
}

lex::nfa lex::nfa_builder::union_nfa(const nfa& nfa1, const nfa& nfa2) {
	auto start = new_state();
	auto end = new_state();
	nfa nfa(start, end);
	nfa = merge(nfa, nfa1);
	nfa = merge(nfa, nfa2);

	nfa.add_edge(start, (lambda_edge(nfa1.start_)));
	nfa.add_edge(start, (lambda_edge(nfa2.start_)));
	nfa.add_edge(nfa1.end_, (lambda_edge(end)));
	nfa.add_edge(nfa2.end_, (lambda_edge(end)));

	nfa.expression_ = '(' + nfa1.expression_ + ")|(" + nfa2.expression_ + ')';
	return nfa;
}

lex::nfa lex::nfa_builder::star_nfa(const nfa& nfa1) {
	nfa nfa = plus_nfa(nfa1);

	nfa.add_edge(nfa.start_, (lambda_edge(nfa.end_)));

	nfa.expression_ = '(' + nfa1.expression_ + ')' + '*';
	return nfa;
}

lex::nfa lex::nfa_builder::plus_nfa(const nfa& nfa1) {
	auto start = new_state();
	auto end = new_state();
	nfa nfa(start, end);
	nfa = merge(nfa, nfa1);

	nfa.add_edge(start, (lambda_edge(nfa1.start_)));
	nfa.add_edge(nfa1.end_, (lambda_edge(end)));
	nfa.add_edge(end, (lambda_edge(start)));

	nfa.expression_ = '(' + nfa1.expression_ + ')' + '+';
	return nfa;
}