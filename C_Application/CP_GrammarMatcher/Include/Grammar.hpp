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
    std::vector<unsigned> m_transiviteComplementListOfUnaryProducers;
    std::vector<unsigned> m_transiviteComplementListOfUnaryProductions;
    std::vector<unsigned> m_unaryProductions;
};


struct BinaryProduction
{
    unsigned m_producingSymbol;
    std::pair<unsigned, unsigned> m_production;
};

class Grammar
{
public:
    const std::vector<GrammarSymbol>& getGrammarSymbols();
    const std::vector<unsigned>& getLetterSymbols(char);
    const std::vector<unsigned>& getWildcardSymbols();
    const std::vector<BinaryProduction>& getBinaryProductions();
    int getStartingSymbol();

private:
    int dfsInitializationHelperProcedure(const std::unique_ptr<Regex::RegexNode>&);
    void transitiveComplementOfUnaryProducersAndProductions();
    std::vector<GrammarSymbol> m_symbols;
    std::vector<BinaryProduction> m_binaryProductions;
    std::vector<unsigned> m_letterSymbols[256];
    std::vector<unsigned> m_wildcardSymbols;
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