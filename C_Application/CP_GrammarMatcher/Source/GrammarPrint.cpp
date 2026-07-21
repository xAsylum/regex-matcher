#include <iostream>
#include "Grammar.hpp"

namespace CFG
{

inline std::ostream& operator<<(std::ostream& p_ostream, GrammarSymbolType p_grammarSymbolType)
{
    switch (p_grammarSymbolType)
    {
        case WildcardSymbol: return p_ostream << "Wildcard";
        case LetterSymbol:   return p_ostream << "Letter";
        case UnionSymbol:    return p_ostream << "Union";
        case JoinSymbol:     return p_ostream << "Join";
        case PlusSymbol:     return p_ostream << "Plus";
        default:             return p_ostream << "Unknown";
    }
}

inline std::ostream& operator<<(std::ostream& p_ostream, const GrammarSymbol& p_grammarSymbol)
{
    p_ostream << "{ Type: " << p_grammarSymbol.m_type 
       << ", Nullable: " << (p_grammarSymbol.m_nullable ? "T" : "F") 
       << ", Marked: " << (p_grammarSymbol.m_marked ? "T" : "F")
       << ", Unary Productions: [";
       
    for (size_t l_id = 0; l_id < p_grammarSymbol.m_unaryProductions.size(); ++l_id)
    {
        p_ostream << char('A' + p_grammarSymbol.m_unaryProductions[l_id])
           << (l_id + 1 < p_grammarSymbol.m_unaryProductions.size() ? ", " : "");
    }
    return p_ostream << "] }";
}


std::ostream& operator<<(std::ostream& p_ostream, const Grammar& p_grammar)
{
    p_ostream << "=== Grammar ===\n";
    p_ostream << "Starting Symbol: " << char('A' + p_grammar.m_startingSymbol) << "\n\n";

    p_ostream << "--- Symbols (" << p_grammar.m_symbols.size() << ") ---\n";
    for (size_t l_id = 0; l_id < p_grammar.m_symbols.size(); ++l_id)
    {
        p_ostream << "  " << char('A' + l_id)  << " : " << p_grammar.m_symbols[l_id] << "\n";
    }

    p_ostream << "\n--- Binary Productions (" << p_grammar.m_binaryProductions.size() << ") ---\n";
    for (const auto& l_production : p_grammar.m_binaryProductions)
    {
        p_ostream << " " << char('A' + l_production.m_producingSymbol) << " -> " 
           << char('A' + l_production.m_production.first) << " " << char('A' + l_production.m_production.second) << "\n";
    }

    p_ostream << "\n--- Letter Symbols ---\n";
    bool l_anyLettersFound = false;
    for (size_t l_id = 0; l_id < 256; ++l_id)
    {
        if (!p_grammar.m_letterSymbols[l_id].empty())
        {
            l_anyLettersFound = true;
            p_ostream << "  '" << static_cast<char>(l_id) << "' : [";
            for (size_t j = 0; j < p_grammar.m_letterSymbols[l_id].size(); ++j)
            {
                p_ostream << char('A' + p_grammar.m_letterSymbols[l_id][j])
                   << (j + 1 < p_grammar.m_letterSymbols[l_id].size() ? ", " : "");
            }
            p_ostream << "]\n";
        }
    }
    if (!l_anyLettersFound) p_ostream << "  (None)\n";

    p_ostream << "\n--- Wildcard Symbols ---\n  [";
    for (size_t l_id = 0; l_id < p_grammar.m_wildcardSymbols.size(); ++l_id)
    {
        p_ostream  << char('A' + p_grammar.m_wildcardSymbols[l_id]) 
           << (l_id + 1 < p_grammar.m_wildcardSymbols.size() ? ", " : "");
    }
    p_ostream << "]\n\n";
    return p_ostream;
}

}