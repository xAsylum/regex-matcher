#include "RegexNode.hpp"
#include "RegexNodeFactory.hpp"

#ifdef DEBUG
#include <deque>
#endif
namespace Regex
{

RegexNode* RegexNodeFactory::createLetterNode(char p_letter)
{
    RegexNode* l_result = nullptr;
    l_result = new LeafNode(p_letter);
    return l_result;
}

RegexNode* RegexNodeFactory::creatrWildcardNode()
{
    RegexNode* l_result = nullptr;
    #ifdef DEBUG
    std::deque<RegexNode*> l_helperQueue = {};
    for (char i = 0; i <= 'z' - 'a'; i++)
    {
            l_helperQueue.push_back(new LeafNode('a' + i));
            l_helperQueue.push_back(new LeafNode('A' + i));
    }
    while (l_helperQueue.size() > 1) 
    {
        auto l_first = std::unique_ptr<RegexNode>(l_helperQueue.front());
        l_helperQueue.pop_front();
        auto l_second = std::unique_ptr<RegexNode>(l_helperQueue.front());
        l_helperQueue.pop_front();
        auto l_new = new OperatorNode(OperatorNodeType::SumNode,
            std::move(l_first), std::move(l_second));
        l_helperQueue.push_back(l_new);
    }
    l_result = l_helperQueue.back();
    l_helperQueue.pop_back();
    #endif
    #ifndef DEBUG
    l_result = new LeafNode();
    #endif
    return l_result;
}

RegexNode* RegexNodeFactory::createSumNode(std::unique_ptr<RegexNode>&& p_leftChild,
    std::unique_ptr<RegexNode>&& p_rightChild)
{
    return new OperatorNode(OperatorNodeType::SumNode, 
        std::move(p_leftChild), std::move(p_rightChild));
}

RegexNode* RegexNodeFactory::createJoinNode(std::unique_ptr<RegexNode>&& p_leftChild,
    std::unique_ptr<RegexNode>&& p_rightChild)
{
    return new OperatorNode(OperatorNodeType::JoinNode, 
        std::move(p_leftChild), std::move(p_rightChild));
}

RegexNode* RegexNodeFactory::createStarNode(std::unique_ptr<RegexNode>&& p_child)
{
    return new OperatorNode(OperatorNodeType::StarNode, std::move(p_child));
}

RegexNode* RegexNodeFactory::createMarkNode(std::unique_ptr<RegexNode>&& p_child)
{
    return new OperatorNode(OperatorNodeType::MarkNode, std::move(p_child));
}

RegexNode* RegexNodeFactory::createOptionalNode(std::unique_ptr<RegexNode>&& p_child)
{
    return new OperatorNode(OperatorNodeType::OptionalNode, std::move(p_child));
}
}
    