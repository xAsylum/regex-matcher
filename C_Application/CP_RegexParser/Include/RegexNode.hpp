#pragma once
#include <vector>
#include <memory>

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
    PlusNode,
    SumNode,
    JoinNode
};

enum LeafNodeType
{
    LetterNode,
    WildcardNode
};

class RegexNode
{
public:
    virtual ~RegexNode() = default;
    
    const std::vector<std::unique_ptr<RegexNode>>& getChildNodes() const;
    NodeType getNodeType() const;

    int getSubtreeSize() const;
    int getSubtreeDepth() const;

protected:
    RegexNode(NodeType);
    NodeType m_nodeType;
    std::vector<std::unique_ptr<RegexNode>> m_childNodes;
    int m_subtreeSize;
    int m_subtreeDepth;
private:
    int m_id;
    static int c_id;

friend std::ostream& operator<<(std::ostream&, const RegexNode&);
friend void printRegexNodeRecursive(std::ostream&, const RegexNode&, int);
};


class LeafNode : public RegexNode
{
public:
    LeafNode();
    LeafNode(char);
    char getLeafNodeLetter() const;
    bool isLeafNodeWildcard() const;
private:
    char m_letter;
    LeafNodeType m_leafNodeType;

friend void printRegexNodeRecursive(std::ostream&, const RegexNode&, int);
};

class OperatorNode : public RegexNode
{
public:
    OperatorNode(OperatorNodeType, std::unique_ptr<RegexNode>&&);
    OperatorNode(OperatorNodeType, std::unique_ptr<RegexNode>&&, std::unique_ptr<RegexNode>&&);
    bool isMarkNode() const;
    OperatorNodeType getNodeOperatorType() const;
private:
    OperatorNodeType m_operatorType;
    
friend void printRegexNodeRecursive(std::ostream&, const RegexNode&, int);
};

}