#pragma once
#include "Grammar.hpp"
#include "RegexNode.hpp"
#include <optional>

namespace CFG
{
using interval = std::pair<int, int>;
class IExtraction
{
public:
virtual std::optional<interval> matchPattern(const std::string& p_text, 
    const std::unique_ptr<Regex::RegexNode>& p_pattern) = 0;
};

class ExtractionCYK : IExtraction
{
public:
ExtractionCYK() : m_grammarFactory() {}

std::optional<interval> matchPattern(const std::string& p_text, 
    const std::unique_ptr<Regex::RegexNode>& p_pattern) override;

private:
GrammarFactory m_grammarFactory;
};

}