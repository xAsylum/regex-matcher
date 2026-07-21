#include <iostream>
#include "Grammar.hpp"

namespace CFG
{

inline std::ostream& operator<<(std::ostream& os, GrammarSymbolType type)
{
    switch (type)
    {
        case WildcardSymbol: return os << "Wildcard";
        case LetterSymbol:   return os << "Letter";
        case UnionSymbol:    return os << "Union";
        case JoinSymbol:     return os << "Join";
        case PlusSymbol:     return os << "Plus";
        default:             return os << "Unknown";
    }
}

inline std::ostream& operator<<(std::ostream& os, const GrammarSymbol& sym)
{
    os << "{ Type: " << sym.m_type 
       << ", Nullable: " << (sym.m_nullable ? "T" : "F") 
       << ", Marked: " << (sym.m_marked ? "T" : "F")
       << ", Unary Prods: [";
       
    for (size_t i = 0; i < sym.m_unaryProductions.size(); ++i)
    {
        os << char('A' + sym.m_unaryProductions[i])
           << (i + 1 < sym.m_unaryProductions.size() ? ", " : "");
    }
    return os << "] }";
}


std::ostream& operator<<(std::ostream& os, const Grammar& g)
{
    os << "=== Grammar ===\n";
    os << "Starting Symbol: " << char('A' + g.m_startingSymbol) << "\n\n";

    os << "--- Symbols (" << g.m_symbols.size() << ") ---\n";
    for (size_t i = 0; i < g.m_symbols.size(); ++i)
    {
        os << "  " << char('A' + i)  << " : " << g.m_symbols[i] << "\n";
    }

    os << "\n--- Binary Productions (" << g.m_binaryProductions.size() << ") ---\n";
    for (const auto& bp : g.m_binaryProductions)
    {
        os << " " << char('A' + bp.m_producingSymbol) << " -> " 
           << char('A' + bp.m_production.first) << " " << char('A' + bp.m_production.second) << "\n";
    }

    os << "\n--- Letter Symbols ---\n";
    bool hasLetters = false;
    for (size_t i = 0; i < 256; ++i)
    {
        if (!g.m_letterSymbols[i].empty())
        {
            hasLetters = true;
            os << "  '" << static_cast<char>(i) << "' : [";
            for (size_t j = 0; j < g.m_letterSymbols[i].size(); ++j)
            {
                os << char('A' + g.m_letterSymbols[i][j])
                   << (j + 1 < g.m_letterSymbols[i].size() ? ", " : "");
            }
            os << "]\n";
        }
    }
    if (!hasLetters) os << "  (None)\n";

    os << "\n--- Wildcard Symbols ---\n  [";
    for (size_t i = 0; i < g.m_wildcardSymbols.size(); ++i)
    {
        os  << char('A' + g.m_wildcardSymbols[i]) 
           << (i + 1 < g.m_wildcardSymbols.size() ? ", " : "");
    }
    os << "]\n\n";
    return os;
}

}