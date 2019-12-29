#ifndef TOKENIZER0__
#define TOKENIZER0__
#include <vector>
#include <string>
#include "dfa_runner.hpp"
#include <istream>
#include <sstream>

struct token {
public:
	std::string token_type_;
	std::string value_;
	token(std::string token_type, std::string value) :
		token_type_(token_type),
		value_(value) {}
	void print(std::ostream& ostream) {
		ostream << token_type_ << '(' << value_ << ')'<<'\n';
	}
};

class tokenizer {
private:
	std::vector<std::pair<dfa_runner, std::string>> token_types_;
	size_t input_start_;
	std::string input_;
	//last created token
	token token_;
	//true if input can be parsed
	bool correct_input_;
public:
	tokenizer(std::string&& input):token_("", ""), correct_input_(true) {
		//std::istringstream str(input);
		//input_ = std::make_unique<std::istream>(str);
		input_ = std::move(input);
	}

	//adds dfa_runner corresponding to regexp and the name to token_types_
	void add_token_type(const std::string& regexp, const std::string& name);

	//parses next token from the input, returns true if parsing was successful
	bool next_token();

	//returns last token parsed, returns valid value only after next_token() returned true
	token get_token() {
		return token_;
	}

	//true if parsing of the file is finished
	bool finished() {
		return input_.length() == input_start_;
	}

	bool correct_input() {
		return correct_input_;
	}
};

#endif