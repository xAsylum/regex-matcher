#pragma once
#include "Grammar.hpp"
#include "RegexNode.hpp"
#include <optional>

namespace CFG
{
using interval = std::pair<int, int>;

struct SubmatchInfo
{
    bool isEmpty() const
    {
        return m_status == 0;
    }

    bool containsSubmatch() const
    {
        return m_submatchStatus != SubmatchStatus::NoMarkedSuccessor;
    }

    char m_status;
    unsigned m_textLength;

    struct SubmatchSuccessor
    {
        interval m_covers;
        unsigned m_symbol;
    };

    enum class SubmatchStatus
    {
        NoMarkedSuccessor,
        ContainsMarkedSuccessor,
        IsMarked
    };

    unsigned m_submatchLength;
    SubmatchStatus m_submatchStatus;
    std::optional<SubmatchSuccessor> m_next;
};

bool operator<(const SubmatchInfo& p_old, const SubmatchInfo& p_new);

class IExtraction
{
public:
virtual ~IExtraction() = default;

virtual std::optional<interval> matchPattern(const std::string& p_text, 
    const std::unique_ptr<Regex::RegexNode>& p_pattern) = 0;
};

class ExtractionCYK : public IExtraction
{
public:
ExtractionCYK() : m_grammarFactory() {}

std::optional<interval> matchPattern(const std::string& p_text, 
    const std::unique_ptr<Regex::RegexNode>& p_pattern) override;

private:
GrammarFactory m_grammarFactory;
};

}