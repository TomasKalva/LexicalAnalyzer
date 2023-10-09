#ifndef TOKENIZER0__
#define TOKENIZER0__
#include <vector>
#include <string>
#include "dfa_runner.hpp"
#include <istream>
#include <sstream>
#include <algorithm>

#ifdef LEXICALANALYZER_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

namespace lex {

	struct token {
	public:
		std::string token_type_;
		std::string value_;

		token(std::string token_type, std::string value) :
			token_type_(token_type),
			value_(value) {}

		void print(std::ostream& ostream) {
			ostream << token_type_ << '(' << value_ << ')' << '\n';
		}
	};

	//used for spliting text by regular expression
	class tokenizer {
	private:
		std::vector<std::pair<dfa_runner, std::string>> token_types_;
		std::string input_;
		size_t input_start_;
		//last created token
		token token_;
		//true if input can be parsed
		bool correct_input_;

		//for printing errors
		size_t line_number_;
		size_t line_position_;

		//sets input_start_ to newPos and updates line number and position
		void update_input_start(size_t newPos);
	public:
		EXPORT tokenizer(std::string&& input) :
			token_("", ""),
			correct_input_(true),
			line_number_(1),
			line_position_(1){
			input_ = std::move(input);
		}

		//adds dfa_runner corresponding to regexp and the name to token_types_
		void EXPORT add_token_type(const std::string& regexp, const std::string& name);

		//removes all tokens with name from token_types_
		void EXPORT remove_token_type(const std::string& name) {
			token_types_.erase(std::remove_if(token_types_.begin(), token_types_.end(),
				[&name](const std::pair<dfa_runner, std::string>& value) { return value.second == name; }));
		}

		//parses next token from the input, returns true if parsing was successful
		bool EXPORT next_token();

		//returns last token parsed, returns valid value only after next_token() returned true
		token EXPORT get_token() {
			return token_;
		}

		//true if parsing of the file is finished
		bool EXPORT finished() {
			return input_.length() == input_start_;
		}

		//returns false if the proccessed part of input can't
		//be tokenized
		bool EXPORT correct_input() {
			return correct_input_;
		}

		//removes the old input and sets new input
		void EXPORT set_input(std::string&& input) {
			correct_input_ = true;
			line_number_ = 1;
			line_position_ = 1;
			input_ = std::move(input);
		}
	};
}

#endif