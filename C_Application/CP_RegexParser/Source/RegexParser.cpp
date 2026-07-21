#include "RegexParser.hpp"
#include "RegexNode.hpp"
#include "RegexNodeFactory.hpp"
#include <cassert>
#include <cctype>
#include <memory>
#include <stack>

namespace Regex
{

ParsedSymbolType RegexParser::getParsedSymbolType(char p_nextChar)
{
    if (p_nextChar == '.')
    {
        return ParsedSymbolType::WildcardSymbol;
    }
    else if (p_nextChar == '|')
    {
        return ParsedSymbolType::SumSymbol;
    }
    else if (p_nextChar == '@')
    {
        return ParsedSymbolType::JoinSymbol;
    }
    else if (p_nextChar == '!')
    {
        return ParsedSymbolType::MarkSymbol;
    }
    else if (p_nextChar == '?')
    {
        return ParsedSymbolType::OptionalSymbol;
    }
    else if (p_nextChar == '+')
    {
        return ParsedSymbolType::PlusSymbol;
    }
    else if (std::isalnum(p_nextChar))
    {
        return ParsedSymbolType::LetterSymbol;
    }
    return ParsedSymbolType::UnknownSymbol;
}
// For now Regex is parsed from RPN
std::unique_ptr<RegexNode> RegexParser::constructRegexPattern(const std::string& p_patternInInfixNotation)
{
    std::string l_patternInRPNotation = parseRPNotationFromInfixNotation(p_patternInInfixNotation);
    std::unique_ptr<RegexNode> l_result = constructRegexFromRPNotation(l_patternInRPNotation);
    return l_result;
}

std::unique_ptr<RegexNode> RegexParser::constructRegexFromRPNotation(const std::string& p_patternInRPNotation)
{
    RegexNodeFactory l_factory;
    std::stack<RegexNode*> l_stack;
    for (auto l_nextChar : p_patternInRPNotation) 
    {
        auto l_nextSymbolType = getParsedSymbolType(l_nextChar);
        switch (l_nextSymbolType)
        {
            case ParsedSymbolType::SumSymbol:
                {
                    auto l_first = std::unique_ptr<RegexNode>(l_stack.top());
                    l_stack.pop();
                    auto l_second = std::unique_ptr<RegexNode> (l_stack.top());
                    l_stack.pop();
                    l_stack.push(l_factory.createSumNode(std::move(l_second), std::move(l_first)));
                }
                break;
            case ParsedSymbolType::JoinSymbol:
                {
                    auto l_first = std::unique_ptr<RegexNode>(l_stack.top());
                    l_stack.pop();
                    auto l_second = std::unique_ptr<RegexNode> (l_stack.top());
                    l_stack.pop();
                    l_stack.push(l_factory.createJoinNode(std::move(l_second), std::move(l_first)));
                }
                break;
            case ParsedSymbolType::PlusSymbol:
                {
                    auto l_top = std::unique_ptr<RegexNode>(l_stack.top());
                    l_stack.pop();
                    l_stack.push(l_factory.createStarNode(std::move(l_top)));
                }
                break;
            case ParsedSymbolType::MarkSymbol:
                {
                    auto l_top = std::unique_ptr<RegexNode>(l_stack.top());
                    l_stack.pop();
                    l_stack.push(l_factory.createMarkNode(std::move(l_top)));
                }
                break;
            case ParsedSymbolType::OptionalSymbol:
                {
                    auto l_top = std::unique_ptr<RegexNode>(l_stack.top());
                    l_stack.pop();
                    l_stack.push(l_factory.createOptionalNode(std::move(l_top)));
                }
                break;
            case ParsedSymbolType::WildcardSymbol:
                {
                    l_stack.push(l_factory.creatrWildcardNode());
                }
                break;
            case ParsedSymbolType::LetterSymbol:
                {
                    l_stack.push(l_factory.createLetterNode(l_nextChar));
                }
                break;
            default:
                break;
        }
    }
    assert(l_stack.size() == 1);
    auto l_result = l_stack.top();
    l_stack.pop();
    return std::unique_ptr<RegexNode>(l_result);
}
}