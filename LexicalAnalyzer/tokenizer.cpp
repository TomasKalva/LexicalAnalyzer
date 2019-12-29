#include "tokenizer.hpp"
#include "regex_parser.hpp"
#include "nfa_runner.hpp"
#include "dfa_builder.hpp"

void tokenizer::add_token_type(const std::string& regexp, const std::string& name) {
    regex_parser rp(regexp);
    nfa nfa = rp.create_machine();
    nfa_runner nr(nfa);
    dfa_builder db;
    dfa d = db.create_dfa(nr);
    dfa_runner dr(d);
	std::pair<dfa_runner, std::string> pair(dr, name);
    token_types_.push_back(pair);
}

bool tokenizer::next_token() {
	if (finished())
		return false;

	size_t pos = input_start_;
	while (pos<input_.length()) {
		char c = input_[pos++];

		for (auto& kvp : token_types_) {
			dfa_runner& dr = kvp.first;
			dr.move(c);
		}
	}

	size_t max = 0;
	size_t max_index = 0;
	bool found_valid = false;
	std::string value = "";
	for (size_t i = 0; i < token_types_.size(); i++) {
		dfa_runner& dr = token_types_[i].first;
		if (dr.has_matching_prefix()) {
			std::string pref = dr.longest_matching_prefix();
			if (pref.size() > max) {
				max = pref.length();
				max_index = i;
				found_valid = true;
				value = pref;
			}
		}
		dr.reset();
	}

	if (found_valid) {
		input_start_ += max;
		auto kvp = token_types_[max_index];
		token_ = token(kvp.second, value);
		return true;
	}
	else {
		//error - no match
		correct_input_ = false;
		return false;
	}
}
