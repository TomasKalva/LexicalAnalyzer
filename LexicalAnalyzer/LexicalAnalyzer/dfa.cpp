#include "pch.h"
#include "dfa.hpp"

void lex::dfa::print() const {
	//header
	std::string delim = "------------------------------\n";
	std::cout << delim;
	std::cout << "DFA\n";
	std::cout << delim;

	//start
	std::cout << "start\t" << start_ << "\n";

	//transition function
	std::cout << "transition function\n";
	for (auto state : states_) {
		std::cout << state << "\t";
		for (auto& kvp : trans_func_) {
			if (kvp.first.first == state)
				if (kvp.first.second.other_)
					std::cout << "(" << "'^'" << "," << kvp.second << ") ";
				else
					std::cout << "(" << kvp.first.second.terminal_ << "," << kvp.second << ") ";
		}
		std::cout << "\n";
	}

	//end states
	std::cout << "end states\n";
	for (auto state : end_states_) {
		std::cout << state << " ";
	}
	std::cout << "\n";

	//fail states
	std::cout << "fail states\n";
	for (auto state : fail_states_) {
		std::cout << state << " ";
	}
	std::cout << "\n";

	std::cout << delim;
}

void lex::dfa::find_fail_states() {
	//use Floyd-Warshall algorithm to find all reachable states, then check
	//if end state is reachable from given state
	using matrix = std::vector<std::vector<bool>>;
	matrix m(states_.size(), std::vector<bool>(states_.size()));

	//initialize matrix
	for (size_t i = 0; i < states_.size(); i++)
		for (size_t j = 0; j < states_.size(); j++) {
			if (i == j || exists_edge(i, j))
				m[i][j] = true;
			else
				m[i][j] = false;
		}

	//Floyd-Warshall
	for (size_t n = 0; n < states_.size(); n++) {
		for (size_t i = 0; i < states_.size(); i++)
			for (size_t j = 0; j < states_.size(); j++) {
				if (m[i][n] && m[n][j])
					m[i][j] = true;
			}
	}

	//for each state find if it can reach some end state
	for (size_t i = 0; i < states_.size(); i++) {
		bool canReachEnd = false;
		for (size_t j = 0; j < states_.size(); j++) {
			if (m[i][j] && std::find(end_states_.begin(), end_states_.end(), states_[j]) != end_states_.end()) {
				canReachEnd = true;
				break;
			}
		}
		if (!canReachEnd) {
			fail_states_.insert(states_[i]);
		}
	}
}