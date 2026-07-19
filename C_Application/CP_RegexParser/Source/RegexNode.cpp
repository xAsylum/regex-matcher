#include "RegexNode.hpp"
#include <cassert>
#include <cctype>
#include <memory>
#ifdef DEBUG_PRINT
#include <iostream>
#endif
namespace Regex
{
#ifdef DEBUG_PRINT
void printGraph(const std::unique_ptr<RegexNode>& p_parent)
{
    for(const auto &l_child : p_parent->m_childNodes)
    {
        std::cout << p_parent->m_id << " " << l_child->m_id << "\n";
        printGraph(l_child);
    }
}
#endif
int RegexNode::c_id = 0;

RegexNode::RegexNode(NodeType p_nodeType) : 
    m_nodeType(p_nodeType), m_subtreeSize(1), m_subtreeDepth(1) 
{
    m_id = c_id;
    c_id++;
}

NodeType RegexNode::getOperatorType()
{
    return m_nodeType;
}

int RegexNode::getSubtreeDepth()
{
    return m_subtreeDepth;
}

int RegexNode::getSubtreeSize()
{
    return m_subtreeSize;
}

const std::vector<std::unique_ptr<RegexNode>>& RegexNode::getChildNodes() 
{
    return m_childNodes;
}

bool RegexNode::isMarkNode()
{
    return false;
}

LeafNode::LeafNode() : 
    RegexNode(NodeType::Leaf), m_letter('.'), m_leafNodeType(LeafNodeType::WildcardNode) {}

LeafNode::LeafNode(char p_letter) : 
    RegexNode(NodeType::Leaf), m_letter(p_letter), m_leafNodeType(LeafNodeType::LetterNode) {}

OperatorNode::OperatorNode(OperatorNodeType p_operatorType, std::unique_ptr<RegexNode>&& p_child) : 
    RegexNode(NodeType::UnaryOperator), m_operatorType(p_operatorType) 
{
    assert(p_operatorType != OperatorNodeType::JoinNode and p_operatorType != OperatorNodeType::SumNode);
    m_subtreeSize = p_child->getSubtreeSize() + 1;
    m_subtreeDepth = p_child->getSubtreeDepth() + 1;
    m_childNodes.push_back(std::move(p_child));
}

OperatorNode::OperatorNode(OperatorNodeType p_operatorType, std::unique_ptr<RegexNode>&& p_leftChild,
    std::unique_ptr<RegexNode>&& p_rightChild) : RegexNode(NodeType::BinaryOperator), 
    m_operatorType(p_operatorType) 
{
    assert(p_operatorType == OperatorNodeType::JoinNode or p_operatorType == OperatorNodeType::SumNode);
    m_subtreeSize = p_leftChild->getSubtreeSize() + p_rightChild->getSubtreeSize() + 1;
    m_subtreeDepth = std::max(p_leftChild->getSubtreeDepth(), p_rightChild->getSubtreeDepth()) + 1;
    m_childNodes.push_back(std::move(p_leftChild));
    m_childNodes.push_back(std::move(p_rightChild));
}

bool OperatorNode::isMarkNode()
{
    return m_operatorType == OperatorNodeType::MarkNode;
}
}