#pragma once
#include <vector>
#include <memory>

#define DEBUG_PRINT
namespace Regex
{

enum NodeType
{
    Leaf = 0,
    UnaryOperator = 1,
    BinaryOperator = 2
};

enum OperatorNodeType
{
    MarkNode,
    OptionalNode,
    StarNode,
    SumNode,
    JoinNode
};

enum LeafNodeType
{
    LetterNode,
    WildcardNode
};
#ifdef DEBUG_PRINT
class RegexNode;

void printGraph(const std::unique_ptr<RegexNode>&);
#endif
class RegexNode
{
public:
    NodeType getOperatorType();
    int getSubtreeSize();
    int getSubtreeDepth();
protected:
    RegexNode(NodeType);
    NodeType m_nodeType;
    std::vector<std::unique_ptr<RegexNode>> m_childNodes;
    int m_subtreeSize;
    int m_subtreeDepth;
private:
    int m_id;
    static int c_id;
#ifdef DEBUG_PRINT
friend void printGraph(const std::unique_ptr<RegexNode> &);
#endif
};


class LeafNode : public RegexNode
{
public:
    LeafNode();
    LeafNode(char);
    char getLetter();
private:
    char m_letter;
    LeafNodeType m_leafNodeType;
};

class OperatorNode : public RegexNode
{
public:
    OperatorNode(OperatorNodeType, std::unique_ptr<RegexNode>&&);
    OperatorNode(OperatorNodeType, std::unique_ptr<RegexNode>&&, std::unique_ptr<RegexNode>&&);
private:
    OperatorNodeType m_operatorType;
};

}