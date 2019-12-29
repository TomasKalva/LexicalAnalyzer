#ifndef DFA0__
#define DFA0__
#include <cstdint>
#include <vector>
#include <unordered_map>

class dfa_builder;
class dfa_runner;

class dfa {
private:
	using state = int32_t;
	struct trans {
		char terminal_;
		bool other_;
		explicit trans(char terminal):terminal_(terminal),other_(false){}
		trans() :terminal_(0), other_(true) {}
		bool operator ==(const trans& o) const{
			return other_ == o.other_ && terminal_ == o.terminal_;
		}
	};
	struct pairhash {
		std::size_t operator()(const std::pair<state, trans>& x) const
		{
			return 17 * std::hash<state>()(x.first) + 23 * ((int32_t)x.second.other_*256 + (int32_t)x.second.terminal_);
		}
	};
	state start_;
	std::vector<state> end_states_;
	std::vector<state> states_;
	std::unordered_map<std::pair<state, trans>, state, pairhash> trans_func_;
public:
	friend dfa_builder;
	friend dfa_runner;

	void add_state(state st) {
		states_.push_back(st);
	}
	void add_symbol_edge(state from, state to, char symbol) {
		std::pair<state, trans> pair(from, trans(symbol));
		trans_func_[pair] = to;
	}
	void add_others_edge(state from, state to) {
		std::pair<state, trans> pair(from, trans());
		trans_func_[pair] = to;
	}
};

#endif