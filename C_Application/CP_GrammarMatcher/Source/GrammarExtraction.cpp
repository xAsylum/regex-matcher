#include "GrammarExtraction.hpp"
#include "Grammar.hpp"
#include <optional>

namespace CFG
{

std::optional<interval> ExtractionCYK::matchPattern(const std::string& p_text,
    const std::unique_ptr<Regex::RegexNode>& p_pattern)
{
    constexpr char c_empty = 0;
    constexpr char c_visited = 1;
    constexpr char c_marked = 2;

    const auto l_grammar = m_grammarFactory.parseFromRegex(p_pattern);
    const auto l_grammarSize = l_grammar->getGrammarSymbols().size();
    const auto l_textSize = p_text.size();
    const auto l_intervalsCount = (l_textSize + 1) * (l_textSize + 1);

    const auto l_transcribeCoordinates = [&](unsigned p_left, unsigned p_right) -> unsigned
    {
        return p_right * (l_textSize + 1) + p_left;
    };

    std::vector<std::vector<char>> l_productionMatrix(l_intervalsCount, 
        std::vector<char>(l_grammarSize, c_empty));


    
    for (unsigned l_pos = 0; l_pos < l_textSize; l_pos++)
    {
        auto l_letter = p_text[l_pos];
        const unsigned l_fullIntervalIdx = l_transcribeCoordinates(l_pos, l_pos + 1);

        for (auto l_symbol : l_grammar->getLetterSymbols(l_letter))
        {
            l_productionMatrix[l_fullIntervalIdx][l_symbol] = c_visited;
        }
        for (auto l_symbol: l_grammar->getWildcardSymbols())
        {
            l_productionMatrix[l_fullIntervalIdx][l_symbol] = c_visited;
        }
        for (unsigned l_grammarSymbolIdx = 0; l_grammarSymbolIdx < l_grammarSize; l_grammarSymbolIdx++)
        {
            if(l_productionMatrix[l_fullIntervalIdx][l_grammarSymbolIdx] == c_visited)
            {
                for (auto l_neighbor : l_grammar->getGrammarSymbols()[l_grammarSymbolIdx].m_transiviteComplementListOfUnaryProducers)
                {
                    l_productionMatrix[l_fullIntervalIdx][l_neighbor] = c_visited;
                }
            }
        }
    }

    for(unsigned l_length = 2; l_length <= l_textSize; l_length++)
    {
        for(unsigned l_leftPointer = 0; l_leftPointer + l_length <= l_textSize; l_leftPointer++)
        {
            // Iterate over [l_left, l_right) intervals
            const unsigned l_rightPointer = l_leftPointer + l_length;
            const unsigned l_fullIntervalIdx = l_transcribeCoordinates(l_leftPointer, l_rightPointer);

            // Binary production case
            for (unsigned l_mid = l_leftPointer + 1; l_mid < l_rightPointer; l_mid++)
            {
                // Split to [l_left, l_mid) and [l_mid, l_right) intervals
                for (const auto& l_production : l_grammar->getBinaryProductions())
                {
                    const unsigned l_leftIntervalIdx = l_transcribeCoordinates(l_leftPointer, l_mid);
                    const unsigned l_rightIntervalIdx = l_transcribeCoordinates(l_mid, l_rightPointer);
                    auto [l_leftSymbol, l_rightSymbol] = l_production.m_production;
                    bool l_leftPart = (l_productionMatrix[l_leftIntervalIdx][l_leftSymbol] == c_visited);
                    bool l_rightPart = (l_productionMatrix[l_rightIntervalIdx][l_rightSymbol] == c_visited);

                    if(l_leftPart and l_rightPart)
                    {
                        l_productionMatrix[l_fullIntervalIdx][l_production.m_producingSymbol] = c_visited;
                    }
                }
            }
            // Unary production complement case
            for (unsigned l_grammarSymbolIdx = 0; l_grammarSymbolIdx < l_grammarSize; l_grammarSymbolIdx++)
            {
                if(l_productionMatrix[l_fullIntervalIdx][l_grammarSymbolIdx] == c_visited)
                {
                    for (auto l_neighbor : l_grammar->getGrammarSymbols()[l_grammarSymbolIdx].m_transiviteComplementListOfUnaryProducers)
                    {
                        l_productionMatrix[l_fullIntervalIdx][l_neighbor] = c_marked;
                    }
                }
            }
            // Cleanup after call optimization
            for (unsigned l_grammarSymbolIdx = 0; l_grammarSymbolIdx < l_grammarSize; l_grammarSymbolIdx++)
            {
                if(l_productionMatrix[l_fullIntervalIdx][l_grammarSymbolIdx] == c_marked)
                {
                    l_productionMatrix[l_fullIntervalIdx][l_grammarSymbolIdx] = c_visited;
                }
            }
        }
    }


    if(l_productionMatrix[l_transcribeCoordinates(0, l_textSize)]
        [l_grammar->getStartingSymbol()] == c_empty)
    {
        return std::nullopt;
    }
    return interval{0, l_textSize};

}

}