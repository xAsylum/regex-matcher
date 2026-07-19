#pragma once
#include "RegexNode.hpp"

namespace Regex
{

class RegexNodeFactory
{
public:
    RegexNode* creatrWildcardNode();
    RegexNode* createLetterNode(char);
    RegexNode* createSumNode(std::unique_ptr<RegexNode>&&, std::unique_ptr<RegexNode>&&);
    RegexNode* createJoinNode(std::unique_ptr<RegexNode>&&, std::unique_ptr<RegexNode>&&);
    RegexNode* createStarNode(std::unique_ptr<RegexNode>&&);
    RegexNode* createMarkNode(std::unique_ptr<RegexNode>&&);
    RegexNode* createOptionalNode(std::unique_ptr<RegexNode>&&);
};

}
