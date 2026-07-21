#pragma once

#include <memory>
#include "RegexNode.hpp"

namespace Regex 
{
    enum ParsedSymbolType
    {
        UnknownSymbol,
        WildcardSymbol,
        LetterSymbol,
        MarkSymbol,
        OptionalSymbol,
        PlusSymbol,
        SumSymbol,
        JoinSymbol,
    };
    class RegexParser
    {
    public:
        std::unique_ptr<RegexNode> constructRegexPattern(const std::string&);
    private:
        std::string parseRPNotationFromInfixNotation(const std::string&);
        std::unique_ptr<RegexNode> constructRegexFromRPNotation(const std::string&);
        ParsedSymbolType getParsedSymbolType(char);
    };
}