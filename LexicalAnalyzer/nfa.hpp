#ifndef STATE_MACHINE0__
#define STATE_MACHINE0__
#include <string>
#include <vector>
#include <unordered_map>

struct edge {
	int32_t to_state_;
	char terminal_;
	bool any_terminal_;
	bool lambda_;
	explicit edge(int32_t to_state):to_state_(to_state),terminal_(0),any_terminal_(false),lambda_(false){}
};

struct nfa {
public:
	std::string expression_;

	int32_t start_;
	int32_t end_;
	std::vector<int32_t> states_;
	std::unordered_map<int32_t,std::vector<edge>> transition_func_;
	nfa() {}//todo:delete this
	nfa(int32_t start, int32_t end):start_(start),end_(end){
		states_.push_back(start);
		states_.push_back(end);
	}
	void add_edge(int32_t from, edge edge) {
		transition_func_[from].push_back(edge);
	}
	//returns chars on non-dot edges from states
	std::vector<char> transition_chars(std::vector<int32_t> states) {
		std::vector<char> chars;
		for (int32_t state : states) {
			for (edge e : transition_func_[state]) {
				if (!e.any_terminal_ &&
					!e.lambda_ &&
					std::find(chars.begin(), chars.end(), e.terminal_)==chars.end()) {
					chars.push_back(e.terminal_);
				}
			}
		}
		return chars;
	}
};
#endif