#pragma once
#include "RegexNode.hpp"
#include <vector>

namespace CFG
{

enum GrammarSymbolType
{
    DotSymbol,
    LetterSymbol,
    UnionSymbol,
    JoinSymbol,
    StarSymbol,
};

struct GrammarSymbol
{
    bool m_marked;
    GrammarSymbolType m_type;
};

union Product
{
    int m_singleProduction;
    std::pair<int, int> m_doubleProduction;
};

struct Production
{
    int m_producingSymbol;
    Product m_production;
};

class Grammar
{
public:

private:
    int dfsInitializationHelperProcedure(const std::unique_ptr<Regex::RegexNode>&);

    std::vector<GrammarSymbol> m_symbols;
    std::vector<Production> m_productions;
    std::vector<int> m_letterSymbols[sizeof(char)];
    std::vector<int> m_dotSymbols;
    int m_startingSymbol;

friend class GrammarFactory;
};

class GrammarFactory
{
public:
    std::unique_ptr<Grammar> parseFromRegex(const std::unique_ptr<Regex::RegexNode>&);
};

}