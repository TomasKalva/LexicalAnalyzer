# 1. Description

The main project is a dynamically linked library for Windows, implementing text parsing to lexical tokens defined using regular expressions. A secondary testing project demonstrates potential library usage.

# 2. Representation of Regular Expressions

A regular expression is:
- a character
- a backslashed special character
- [...] - a set of values (non-special characters, backslashed special characters, or ranges of values like a-z, A-Z, 0-9 may be present inside the brackets)
- .

If `e1`, `e2` are regular expressions, then a regular expression is also:
- `e1*`
- `e1+`
- `(e1)`
- `e1e2`
- `e1|e2`

# 3. Testing Project

The testing program tests the parsing of regular expressions, deterministic automatons created from regular expressions, and file parsing into tokens. Testing is performed using console commands. The library is statically linked to the project. Before running the program, the LexicalAnalyzer project must be built. `LexicalAnalyzer.dll` is copied from the library output to the output directory of the testing project in the post-build event.

All files take a file with the .in extension as the first argument, only the name without the extension is given as an argument.

- The command `t` outputs syntactic trees of regular expressions from the file specified as the first argument. Each line of the file contains one regular expression.
- The command `m` has a file as an argument, which has regular expressions on odd lines, and text to be tested by the regular expression from the previous line on even lines. The command outputs whether the text matches the reg. expression and the deterministic automaton corresponding to the reg. expression.
- The command `a` takes two arguments: a file that has token names on odd lines and regular expressions corresponding to the token on the previous line on even lines. The second argument is the file name including the extension, containing text to be parsed. The command prints the tokens and their values, or that the text could not be parsed using the expressions.

# 4. Library
## Implementation

The library consists of classes representing state machines - `dfa, nfa` classes creating state machines fromregular expressions - `dfa_builder, nfa_builder` classes allowing the running of state machines - `dfa_runner, nfa_runner` a class creating `dfa` from a regular expression `regexp_parser` and a tokenizer class, which divides text into tokens using regular expressions.

During loading, a non-deterministic state machine is recursively constructed straight away using the`create_machine` method. Functions returning a state machine for non-terminals and functions composing machines are in the `nfa_builder` class, which also takes care of state numbering. If an invalid regular expression is provided, an `invalid_argument` exception is thrown.

`nfa` contains three types of edges in the transition function: an edge corresponding to a non-terminal (any value of type char), an edge corresponding to a lambda transition, and an edge corresponding to any terminal (this could be replaced by 256 non-terminal edges, but this optimization saves time and space when converting to a deterministic machine). The run of `nfa` is ensured by `nfa_runner`, which is also used in the construction of the deterministic machine.

`dfa` is created using the `dfa_factory` class. `dfa_factory` simulates the passage through `nfa`: for each state reachable from the initial state, it finds all letters on the outgoing edges. For each found letter, it finds the lambda closure of the transition using this letter and adds it with the relevant edge to the `dfa` states, if it has not been added yet. If some letters are not used on the edges out of the state, a transition to the empty state is added using a special edge. Finally, in the created `dfa`, states from which no final state can be reached are found using the Floyd-Warshall algorithm.


## Interface

The library exports methods of the classes `regex_parser`, `dfa_runner`, `dfa_factory`, and `tokenizer`.

Using the `create_machine` method of the `regex_parser` class, it's possible to check if the provided regular expression is syntactically correct. The `create_dfa` method of the `dfa_factory` class creates `dfa_runner` instances corresponding to the provided regular expression.

`dfa_runner` allows testing whether text matches a regular expression. Text is input character by character using the `move` method. The `matches` method ensures testing if the text corresponds to the expression. The class also contains methods: 
- `longest_matching_prefix`, which returns the longest prefix matching the regular expression, 
- `has_matching_prefix`, which indicates whether such a prefix exists, and 
- `failed`, which indicates whether the automaton has failed.

`tokenizer` allows adding and removing regular expressions with names of corresponding tokens using the `add_token_type` and `remove_token_type` methods, respectively. After specifying tokens, text to be parsed can be input using the `set_input` method. The `next_token` method parses the next token, and the `get_token` method returns this token. Using the `finished` and `correct_input` methods, it's possible to determine whether the parsing is complete and whether an error occurred during it. If an error occurs during parsing, an error message is output with the line number and position where the error occurred.

