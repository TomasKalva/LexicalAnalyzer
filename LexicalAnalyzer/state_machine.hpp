#ifndef STATE_MACHINE0__
#define STATE_MACHINE0__
#include <string>
#include <vector>
#include <unordered_map>

struct edge {
	int32_t to_state_;
	char terminal_;
	bool any_symbol_;
	bool lambda_;
	explicit edge(int32_t to_state):to_state_(to_state),terminal_(0),any_symbol_(false),lambda_(false){}
};

struct state_machine {
public:
	std::string info_;

	int32_t start_;
	int32_t end_;
	std::vector<int32_t> states_;
	std::unordered_map<int32_t,std::vector<edge>> transition_func_;
	state_machine() {}//todo:delete this
	state_machine(int32_t start, int32_t end):start_(start),end_(end){
		states_.push_back(start);
		states_.push_back(end);
	}
	void add_edge(int32_t from, edge edge) {
		transition_func_[from].push_back(edge);
	}
};
#endif