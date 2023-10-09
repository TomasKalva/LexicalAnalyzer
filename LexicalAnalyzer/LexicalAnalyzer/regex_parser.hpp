#ifndef REGEX_PARSER0__
#define REGEX_PARSER0__

#include <string>
#include <vector>
#include <iostream>
#include "nfa.hpp"
#include "nfa_builder.hpp"

#ifdef LEXICALANALYZER_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

namespace lex {
	class regex_parser {
	private:
		std::string input_;
		std::size_t position_;
		//number of currently open parentheses
		int32_t open_parenth_;
		std::vector<char> special_symbols_;
		std::vector<char> backslashable_letters_;
		nfa_builder nfa_builder_;
		size_t depth_;
		bool print_tree_;

		bool match(const char c);
		char peek();
		bool end();
		void error(std::string message);
		bool peek_terminal();
		void move();
		bool can_be_backslashed(char c);
		char backslashed_value(char c);
		//prints symbol of the grammar with its value at corresponding depth if
		//print_tree_ is true
		void print(std::string grammar_symbol, std::string value) {
			if (!print_tree_)
				return;

			for (size_t i = 0; i < depth_; i++)
				std::cout << " ";
			std::cout << grammar_symbol << "(" << value << ")\n";
		}

		//expands a,-,z with a,b,...,z; A-Z with A,B,...,Z; 0-9 with 0,1,...,9
		void expand(std::vector<char>& terminals);

		nfa expr();
		nfa term();
		nfa factor();
		nfa base();
		std::vector<char> repeat_terminal();
		nfa terminal();
	public:
		EXPORT regex_parser(const std::string& input, bool print_tree) :
			input_(input),
			position_(0),
			open_parenth_(0),
			depth_(0),
			print_tree_(print_tree),
			special_symbols_
		{ '|','*','+','.','[',']','(',')','\\' },
			backslashable_letters_
		{ 'n','t','r' }{

		}
		nfa EXPORT create_machine();
	};
}
#endif