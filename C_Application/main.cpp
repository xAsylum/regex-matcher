#include "Grammar.hpp"
#include "RegexParser.hpp"
#include <iostream>

int main(int argc, char** argv) 
{
    //const auto& parsed = Args::ArgsParser(argc, argv);
    //std::cout << parsed.getBundle();
    auto l_regexParser = Regex::RegexParser();
    auto l_grammarFactory = CFG::GrammarFactory();
    auto l_node = l_regexParser.constructRegexPattern("ku@ba@@.+d@|!");
    std::cout << *l_node.get();
    auto l_grammar = l_grammarFactory.parseFromRegex(l_node);
    std::cout << *l_grammar.get();

}