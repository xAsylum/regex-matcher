#include "RegexNode.hpp"
#include "RegexParser.hpp"

int main(int argc, char** argv) 
{
    //const auto& parsed = Args::ArgsParser(argc, argv);
    //std::cout << parsed.getBundle();
    auto l_regexParser = Regex::RegexParser();
    auto l_node = l_regexParser.constructRegexPattern("ku@ba@@.*d@+");
    Regex::printGraph(l_node);
}