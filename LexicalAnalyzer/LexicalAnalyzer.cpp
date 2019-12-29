// LexicalAnalyzer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "regex_parser.hpp"
#include "nfa_builder.hpp"
#include "nfa_runner.hpp"
#include "dfa_builder.hpp"
#include "dfa_runner.hpp"
#include "tokenizer.hpp"

int main()
{
    std::cout << "Hello World!\n";
    std::string re = "(..)+";
    regex_parser rp(re);
    try {
        std::cout << re << '\n';
        nfa sm = rp.create_machine();
        std::cout<<sm.expression_;

        nfa_runner mr(sm);

        
        /*//mr.print_active();
        mr.move('a');
        std::cout << mr.matches();
        //mr.print_active();
        mr.move('a');
        std::cout << mr.matches();
        //mr.print_active();
        mr.move('a');
        std::cout << mr.matches();
        //mr.print_active();
        mr.move('a');
        std::cout << mr.matches();
        //mr.print_active();
        mr.move('a');
        std::cout << mr.matches();
        //mr.print_active();
        std::cout << mr.longest_matching_prefix();*/
        /*
        dfa_builder db;
        dfa d = db.create_dfa(mr);
        dfa_runner dr(d);
        dr.move('a');
        std::cout << dr.matches();
        dr.move('a');
        std::cout << dr.matches();
        dr.move('a');
        std::cout << dr.matches();
        dr.move('a');
        std::cout << dr.matches();
        dr.move('a');
        std::cout << dr.matches();
        std::cout << dr.longest_matching_prefix();*/
    }
    catch (std::invalid_argument & e) {
        std::cout << e.what();
    }
    /*nfa_builder smb;
    auto x = smb.dot_sm();
    auto y = smb.dot_sm();
    auto xy = smb.union_sm(x, y);*/
    //std::cout << std::to_string(std::find(v.begin(), v.end(), v, []() {return true; }) != v.end());

    tokenizer tok("stttrss0t1001101str");
    tok.add_token_type("[str]*", "string");
    tok.add_token_type("[01]*", "integer");
    while (tok.next_token()) {
        auto x = tok.get_token();
        x.print(std::cout);
    }
    std::cout << tok.correct_input();
    return 0;
}