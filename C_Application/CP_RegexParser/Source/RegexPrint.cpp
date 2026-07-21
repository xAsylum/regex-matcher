
#include "RegexNode.hpp"
#include <iostream>
#include <string>

namespace Regex
{


inline std::ostream& operator<<(std::ostream& p_ostream, OperatorNodeType p_operatorNodeType)
{
    switch (p_operatorNodeType)
    {
        case MarkNode:     return p_ostream << "!";
        case OptionalNode: return p_ostream << "?";
        case PlusNode:     return p_ostream << "+";
        case SumNode:      return p_ostream << "|";
        case JoinNode:     return p_ostream << "@";
        default:           return p_ostream << "Unknown";
    }
}


inline void printRegexNodeRecursive(std::ostream& p_ostream, const RegexNode& p_regexNode, int p_depth)
{
    std::string indent(p_depth * 3, ' ');
    p_ostream << indent << "-> [" << p_regexNode.m_id << "]";

    if (p_regexNode.m_nodeType == Leaf)
    {
        const auto& leaf = static_cast<const LeafNode&>(p_regexNode);
        if (leaf.m_leafNodeType == LetterNode)
        {
            p_ostream  << " ('" << leaf.m_letter << "')";
        }
        else 
        {
            p_ostream << " ( . )";
        }

    }
    else
    {
        const auto& op = static_cast<const OperatorNode&>(p_regexNode);
        p_ostream << " (" << op.m_operatorType << ")";
    }

    p_ostream << "\n";

    for (const auto& child : p_regexNode.m_childNodes)
    {
        if (child)
        {
            printRegexNodeRecursive(p_ostream, *child, p_depth + 1);
        }
        else
        {
            p_ostream << indent << "   -> [NULLPTR]\n";
        }
    }
}


std::ostream& operator<<(std::ostream& p_ostream, const RegexNode& p_regexNode)
{
    p_ostream << "=== Regex ===\n";
    printRegexNodeRecursive(p_ostream, p_regexNode, 0);
    p_ostream << "\n";
    return p_ostream;
}

}