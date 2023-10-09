#ifndef DFA0__
#define DFA0__
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>

#ifdef LEXICALANALYZER_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

namespace lex {
	class dfa_factory;
	class dfa_runner;

	class dfa {
	private:
		using state = int32_t;
		//represents transition value
		struct trans {
			char terminal_;
			bool other_;
			explicit trans(char terminal) :terminal_(terminal), other_(false) {}
			trans() :terminal_(0), other_(true) {}
			bool operator ==(const trans& o) const {
				return other_ == o.other_ && terminal_ == o.terminal_;
			}
		};
		//hash for pair (state,trans)
		struct pairhash {
			std::size_t operator()(const std::pair<state, trans>& x) const
			{
				return 17 * std::hash<state>()(x.first) + 23 * ((int64_t)x.second.other_ * 256 + (int64_t)x.second.terminal_);
			}
		};
		state start_;
		std::set<state> end_states_;
		std::vector<state> states_;
		std::set<state> fail_states_;
		std::unordered_map<std::pair<state, trans>, state, pairhash> trans_func_;
	public:
		friend dfa_factory;
		friend dfa_runner;

		dfa() :start_(0) {}

		void add_state(state st) {
			states_.push_back(st);
		}
		void add_terminal_edge(state from, state to, char terminal) {
			std::pair<state, trans> pair(from, trans(terminal));
			trans_func_[pair] = to;
		}
		void add_others_edge(state from, state to) {
			std::pair<state, trans> pair(from, trans());
			trans_func_[pair] = to;
		}

		void EXPORT print() const;

		//i,j are indexes to states_
		bool exists_edge(size_t i, size_t j) {
			size_t state_i = states_[i];
			size_t state_j = states_[j];
			for (auto& kvp : trans_func_) {
				if (kvp.first.first == state_i && kvp.second == state_j)
					return true;
			}
			return false;
		}

		void find_fail_states();
	};
}

#endif