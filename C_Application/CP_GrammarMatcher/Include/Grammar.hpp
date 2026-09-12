#pragma once
#include "RegexNode.hpp"
#include <vector>

namespace CFG
{

enum GrammarSymbolType
{
    WildcardSymbol,
    LetterSymbol,
    UnionSymbol,
    JoinSymbol,
    PlusSymbol,
};

struct GrammarSymbol
{
    GrammarSymbol(GrammarSymbolType);
    bool m_marked;
    GrammarSymbolType m_type;
    bool m_nullable;
    std::vector<int> m_transiviteComplementListOfUnaryProducers;
    std::vector<int> m_unaryProductions;
};


struct BinaryProduction
{
    int m_producingSymbol;
    std::pair<int, int> m_production;
};

class Grammar
{
public:
    const std::vector<GrammarSymbol>& getGrammarSymbols();
    const std::vector<int>& getLetterSymbols(char);
    const std::vector<int>& getWildcardSymbols();
    const std::vector<BinaryProduction>& getBinaryProductions();
    int getStartingSymbol();

private:
    int dfsInitializationHelperProcedure(const std::unique_ptr<Regex::RegexNode>&);
    void transitiveComplementOfUnaryProducers();
    std::vector<GrammarSymbol> m_symbols;
    std::vector<BinaryProduction> m_binaryProductions;
    std::vector<int> m_letterSymbols[256];
    std::vector<int> m_wildcardSymbols;
    int m_startingSymbol;

friend class GrammarFactory;
friend std::ostream& operator<<(std::ostream&, const Grammar&);
};

class GrammarFactory
{
public:
    std::unique_ptr<Grammar> parseFromRegex(const std::unique_ptr<Regex::RegexNode>&);
};

}