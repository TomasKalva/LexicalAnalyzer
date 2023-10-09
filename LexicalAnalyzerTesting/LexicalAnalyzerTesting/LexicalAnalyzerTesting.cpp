#include <iostream>
#include <fstream>
#include "tokenizer.hpp"
#include "regex_parser.hpp"
#include "dfa_factory.hpp"
#include "nfa_runner.hpp"
#include "nfa.hpp"
#include <nfa_runner.hpp>

//splits line by the delimiters and removes empty strings
std::vector<std::string> split(std::string& line, std::string delims) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    size_t next_pos = 0;
    while (next_pos != std::string::npos) {
        next_pos = line.find_first_of(delims, pos);
        std::string substr = line.substr(pos, next_pos - pos);
        if(substr!="")
            tokens.push_back(substr);

        pos = next_pos + 1;
    }
    return tokens;
}

void error(std::string message) {
    std::cout << "Error: " << message << "\n";
}

//prints tree of the regexp and its dfa or an error and its position in the regexp
void print_regexp_trees(std::istream& input, std::ostream& output) {
    std::string line;
    while (std::getline(input, line)) {
        lex::regex_parser rp(line, true);
        try {
            std::cout << line << "\n";
            lex::nfa nfa = rp.create_machine();
            lex::dfa_factory dfa_builder;
            lex::dfa dfa = dfa_builder.create_dfa(line);
            lex::dfa_runner dfa_runner(dfa);
            std::cout << nfa.expression() << "\n";
            dfa.print();
        }
        catch (std::invalid_argument& e) {
            error(e.what());
        }
        std::cout << "\n\n";
    }
}

//prints true if the string matches the regexp
void match_regexps(std::istream& input, std::ostream& output) {
    std::string regexp;
    std::string text;
    while (std::getline(input, regexp)) {
        std::getline(input, text);
        lex::regex_parser rp(regexp, false);
        try {
            //create dfa for the regexp
            lex::dfa_factory dfa_factory;
            lex::dfa dfa = dfa_factory.create_dfa(regexp);
            lex::dfa_runner dfa_runner(dfa);

            //iterate text with dfa_runner
            for (char c : text) {
                dfa_runner.move(c);
            }
            std::cout << regexp << " " << text << " " << dfa_runner.matches()<< "\n";
        }
        catch (std::invalid_argument & e) {
            error(e.what());
        }
        std::cout << "\n";
    }
}

//prints the file split into tokens or error and its position
void split_file(std::istream& regexps, std::istream& text, std::ostream& output) {
    //create tokenizer
    std::string s(std::istreambuf_iterator<char>(text), {});
    lex::tokenizer tokenizer(std::move(s));

    //load regexps
    std::string regexp;
    std::string name;
    while (std::getline(regexps, name)) {
        std::getline(regexps, regexp);
        try {
            tokenizer.add_token_type(regexp, name);
        }
        catch (std::invalid_argument & e) {
            error(e.what());
        }
    }

    //tokenize input
    while (tokenizer.next_token()) {
        auto token = tokenizer.get_token();
        token.print(output);
    }
    std::cout << "\n";
}

int main()
{
    std::string line;
    do {
        std::cout << "Enter command:\n";
        std::cout << "\tShow regexp tree: t <regexp file>\n";
        std::cout << "\tMatch regexp: m <regexp + string file>\n";
        std::cout << "\tAnalyze file: a <regexp file> <text file> [<output file>]\n";
        std::cout << ">";
        std::getline(std::cin, line);
        auto tokens = split(line, " ");

        if (tokens.size() < 1 || tokens[0].size() != 1) {
            error("invalid command");
            continue;
        }
        char command = tokens[0][0];
        if (command != 't' &&
            command != 'm' &&
            command != 'a') {
            error("invalid command");
            continue;
        }

        //all commands have at least one input file
        std::ifstream first_input;
        if (tokens.size() >= 2) {
            std::string file0 = tokens[1] + ".in";
            first_input.open(file0);
            if (!first_input.is_open()) {
                error( file0 + " can't be opened");
                continue;
            }
        }
        else {
            error("incorrect number of parameters");
            continue;
        }
        std::ifstream textInput;
        switch(command){
        case 't':
            print_regexp_trees(first_input, std::cout);
            break;
        case 'm':
            match_regexps(first_input, std::cout);
            break;
        case 'a':
            //split file to tokens

            //load the file to split
            if (tokens.size() >= 3) {
                textInput.open(tokens[2]);
                if (!textInput.is_open()) {
                    error( tokens[2] + " can't be opened\n");
                    continue;
                }
            }
            else {
                error("incorrect number of parameters");
                continue;
            }

            if (tokens.size() == 4) {
                std::ofstream o_file(tokens[3]);
                if (!o_file.is_open()) {
                    error(tokens[3] + " can't be opened");
                    continue;
                }

                split_file(first_input, textInput, o_file);
            }
            else {
                split_file(first_input, textInput, std::cout);
            }
            break;
        }

    } while (true);
}