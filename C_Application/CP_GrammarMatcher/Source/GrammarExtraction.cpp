#include "GrammarExtraction.hpp"
#include "Grammar.hpp"
#include <optional>

namespace CFG
{

constexpr char c_visited = 1;

bool operator<(const SubmatchInfo& p_old, const SubmatchInfo& p_candidate)
{   
    if(p_candidate.isEmpty())
    {
        return false;
    }
    if(p_old.isEmpty())
    {
        return true;
    }
    if(!p_candidate.containsSubmatch())
    {
        return false;
    }
    if(!p_old.containsSubmatch())
    {
        return true;
    }
    return p_old.m_submatchLength <= p_candidate.m_submatchLength;
}

inline void selectBetter(SubmatchInfo& p_old, SubmatchInfo p_candidate)
{
    if(p_old < p_candidate)
    {
        p_old = p_candidate;
    }
};

std::optional<interval> ExtractionCYK::matchPattern(const std::string& p_text,
    const std::unique_ptr<Regex::RegexNode>& p_pattern)
{
    
    const auto l_grammar = m_grammarFactory.parseFromRegex(p_pattern);
    const auto& l_grammarSymbols = l_grammar->getGrammarSymbols();
    const auto l_grammarSize = l_grammarSymbols.size();
    const auto l_textSize = p_text.size();
    const auto l_intervalsCount = (l_textSize + 1) * (l_textSize + 1);
    
    const auto l_transcribeCoordinates = [&](unsigned p_left, unsigned p_right) -> unsigned
    {
        return p_right * (l_textSize + 1) + p_left;
    };

    std::vector<std::vector<SubmatchInfo>> l_productionMatrix(l_intervalsCount, 
        std::vector<SubmatchInfo>(l_grammarSize, SubmatchInfo{}));

    for (unsigned l_pos = 0; l_pos < l_textSize; l_pos++)
    {
        auto l_letter = p_text[l_pos];
        const unsigned l_fullIntervalIdx = l_transcribeCoordinates(l_pos, l_pos + 1);
        auto l_visited = SubmatchInfo
        {
          .m_status = c_visited,
          .m_textLength = 1,
          .m_submatchLength = 0,
          .m_submatchStatus = SubmatchInfo::SubmatchStatus::NoMarkedSuccessor,
          .m_next = std::nullopt
        };
        for (auto l_symbolIdx : l_grammar->getLetterSymbols(l_letter))
        {
            selectBetter(l_productionMatrix[l_fullIntervalIdx][l_symbolIdx],
                l_visited);
        }
        for (auto l_symbolIdx: l_grammar->getWildcardSymbols())
        {
            selectBetter(l_productionMatrix[l_fullIntervalIdx][l_symbolIdx],
                l_visited);
        }
    }

    for(unsigned l_length = 1; l_length <= l_textSize; l_length++)
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
                    bool l_leftPart = (l_productionMatrix[l_leftIntervalIdx][l_leftSymbol].m_status == c_visited);
                    bool l_rightPart = (l_productionMatrix[l_rightIntervalIdx][l_rightSymbol].m_status == c_visited);

                    if(l_leftPart and l_rightPart)
                    {
                        auto l_status = SubmatchInfo::SubmatchStatus::NoMarkedSuccessor;
                        unsigned l_submatchLength = 0;
                        std::optional<SubmatchInfo::SubmatchSuccessor> l_next = std::nullopt;
                        bool l_leftSubmatch = l_productionMatrix[l_leftIntervalIdx][l_leftSymbol].containsSubmatch();
                        bool l_rightSubmatch = l_productionMatrix[l_rightIntervalIdx][l_rightSymbol].containsSubmatch();
                        unsigned l_leftSubmatchLength = l_productionMatrix[l_leftIntervalIdx][l_leftSymbol].m_submatchLength;
                        unsigned l_rightSubmatchLength = l_productionMatrix[l_rightIntervalIdx][l_rightSymbol].m_submatchLength;
                        if (l_grammarSymbols[l_production.m_producingSymbol].m_marked)
                        {
                            l_status = SubmatchInfo::SubmatchStatus::IsMarked;
                            l_submatchLength = l_length;
                            l_next = SubmatchInfo::SubmatchSuccessor
                                {
                                    .m_covers = {l_leftPointer, l_rightPointer},
                                    .m_symbol = l_production.m_producingSymbol
                                };
                        }
                        else if(l_leftSubmatch or l_rightSubmatch)
                        {
                            auto l_leftNext = SubmatchInfo::SubmatchSuccessor
                                {
                                    .m_covers = {l_leftPointer, l_mid},
                                    .m_symbol = l_leftSymbol
                                };
                            auto l_rightNext = SubmatchInfo::SubmatchSuccessor
                                {
                                    .m_covers = {l_mid, l_rightPointer},
                                    .m_symbol = l_rightSymbol
                                };
                            l_status = SubmatchInfo::SubmatchStatus::ContainsMarkedSuccessor;
                            l_submatchLength = std::max(l_leftSubmatchLength, l_rightSubmatchLength);
                            if (l_leftSubmatch and l_rightSubmatch)
                                {
                                    if (l_leftSubmatchLength < l_rightSubmatchLength)
                                    {
                                        l_next = l_leftNext;
                                    }
                                    else
                                    {
                                        l_next = l_rightNext;
                                    }
                                }
                            else if (l_leftSubmatch)
                            {
                                l_next = l_leftNext;
                            }
                            else
                            {
                                l_next = l_rightNext;
                            }
                        }
                        auto l_candidate = SubmatchInfo
                        {
                            .m_status = c_visited,
                            .m_textLength = l_length,
                            .m_submatchLength = l_submatchLength,
                            .m_submatchStatus = l_status,
                            .m_next = l_next
                        };
                        selectBetter(l_productionMatrix[l_fullIntervalIdx][l_production.m_producingSymbol],
                            l_candidate);
                    }
                }
            }
            // Unary production complement case - we want to check, which symbols should be added to the matrix
            std::vector<bool> l_shouldBeUpdated = std::vector<bool>(l_grammarSize, false);
            for (unsigned l_grammarSymbolIdx = 0; l_grammarSymbolIdx < l_grammarSize; l_grammarSymbolIdx++)
            {
                if(l_productionMatrix[l_fullIntervalIdx][l_grammarSymbolIdx].m_status == c_visited)
                {
                    for (auto l_producer : l_grammar->getGrammarSymbols()
                        [l_grammarSymbolIdx].m_transiviteComplementListOfUnaryProducers)
                    {
                        l_shouldBeUpdated[l_producer] = true;
                    }
                }
            }
            // This is the clever part - for each new candidate, we select best production. One can check that when having
            // the transitive complement of edges, to select best production, it is sufficient to check every outgoing edge.
            for (unsigned l_grammarSymbolIdx = 0; l_grammarSymbolIdx < l_grammarSize; l_grammarSymbolIdx++)
            {
                if (not l_shouldBeUpdated[l_grammarSymbolIdx])
                {
                    continue;
                }
                if (l_grammarSymbols[l_grammarSymbolIdx].m_marked)
                {
                    auto l_candidate = SubmatchInfo
                        {
                            .m_status = c_visited,
                            .m_textLength = l_length,
                            .m_submatchLength = l_length,
                            .m_submatchStatus = SubmatchInfo::SubmatchStatus::IsMarked,
                            .m_next = SubmatchInfo::SubmatchSuccessor
                            {
                                .m_covers = {l_leftPointer, l_rightPointer},
                                .m_symbol = l_grammarSymbolIdx
                            }
                        };
                    selectBetter(l_productionMatrix[l_fullIntervalIdx][l_grammarSymbolIdx],
                        l_candidate);
                }
                for (auto l_candiateIdx : l_grammarSymbols[l_grammarSymbolIdx]
                        .m_transiviteComplementListOfUnaryProductions)
                {
                    selectBetter(l_productionMatrix[l_fullIntervalIdx][l_grammarSymbolIdx], 
                        l_productionMatrix[l_fullIntervalIdx][l_candiateIdx]);
                }
            }
        }
    }

    if(l_productionMatrix[l_transcribeCoordinates(0, l_textSize)]
        [l_grammar->getStartingSymbol()].isEmpty())
    {
        return std::nullopt;
    }
    auto l_pointer = l_productionMatrix[l_transcribeCoordinates(0, l_textSize)]
        [l_grammar->getStartingSymbol()];
    if(!l_pointer.containsSubmatch()) return interval{-1, -1};
    while (l_pointer.m_submatchStatus != SubmatchInfo::SubmatchStatus::IsMarked)
    {
        l_pointer = l_productionMatrix[l_transcribeCoordinates(l_pointer.m_next->m_covers.first, 
                l_pointer.m_next->m_covers.second)]
        [l_pointer.m_next->m_symbol];
    }
    return l_pointer.m_next->m_covers;

}

}