#include "pch.h"
#include "tokenizer.hpp"
#include "regex_parser.hpp"
#include "nfa_runner.hpp"
#include "dfa_factory.hpp"

void lex::tokenizer::update_input_start(size_t newPos) {
	for (size_t i = input_start_; i < newPos; i++) {
		if (i >= input_.length())
			break;

		if (input_[i] == '\n') {
			line_number_++;
			line_position_ = 0;
		}
		else {
			line_position_++;
		}
	}
	input_start_ = newPos;
}

void lex::tokenizer::add_token_type(const std::string& regexp, const std::string& name) {
    dfa_factory db;
    dfa d = db.create_dfa(regexp);
    dfa_runner dr(d);
	std::pair<dfa_runner, std::string> pair(dr, name);
    token_types_.push_back(pair);
}

bool lex::tokenizer::next_token() {
	if (finished()) {
		std::cout << "Input was ok.\n";
		return false;
	}

	//run dfas until all of them fail or the input ends
	size_t pos = input_start_;
	while (pos<input_.length()) {
		char c = input_[pos++];

		bool failed = true;
		for (auto& kvp : token_types_) {
			dfa_runner& dr = kvp.first;
			if (!dr.failed()) {
				dr.move(c);
				failed = false;
			}
		}

		if (failed)
			break;
	}

	//find longest matching prefix
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
		update_input_start(input_start_ + max);
		auto kvp = token_types_[max_index];
		token_ = token(kvp.second, value);
		return true;
	}
	else {
		//error - no match
		std::cout << "Can't match token starting at line "<< line_number_ << " position " << line_position_ << ":\n";
		if(input_.length() - input_start_ > 10)
			std::cout << input_.substr(input_start_, 40) << "...\n";
		else
			std::cout << input_.substr(input_start_) << "\n";

		input_start_ = input_.length();
		correct_input_ = false;
		return false;
	}
}
