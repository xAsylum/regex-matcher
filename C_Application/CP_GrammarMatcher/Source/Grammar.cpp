#include "Grammar.hpp"
#include "RegexNode.hpp"
#include <memory>

namespace CFG
{

GrammarSymbol::GrammarSymbol(GrammarSymbolType p_type) : m_type(p_type) {};
int Grammar::dfsInitializationHelperProcedure(
    const std::unique_ptr<Regex::RegexNode>& p_root)
{
    int l_idx = m_symbols.size();
    auto l_nodeType = p_root->getNodeType();
    bool l_isMarkNode = (l_nodeType == Regex::NodeType::UnaryOperator
        and static_cast<Regex::OperatorNode*>(p_root.get())->isMarkNode());

    if(l_isMarkNode)
    {   
        l_idx = dfsInitializationHelperProcedure(p_root->getChildNodes().front());
        m_symbols[l_idx].m_marked = true;
        return l_idx;
    }

    if(l_nodeType == Regex::Leaf)
    {
        bool l_isWildcardNode = static_cast<Regex::LeafNode*>(p_root.get())->isLeafNodeWildcard();
        char l_symbol = static_cast<Regex::LeafNode*>(p_root.get())->getLeafNodeLetter();
        if(l_isWildcardNode)
        {
            m_symbols.emplace_back(GrammarSymbolType::WildcardSymbol);
            m_wildcardSymbols.emplace_back(l_idx);  
        }
        else
        {
            m_symbols.emplace_back(GrammarSymbolType::LetterSymbol);
            m_letterSymbols[static_cast<unsigned char>(l_symbol)].emplace_back(l_idx);
        }
    }
    else if(l_nodeType == Regex::UnaryOperator)
    {
        m_symbols.emplace_back(GrammarSymbolType::PlusSymbol);
        int l_childIdx = dfsInitializationHelperProcedure(p_root->getChildNodes()[0]);
        m_binaryProductions.emplace_back(BinaryProduction
            {
                .m_producingSymbol = l_idx,
                .m_production = {l_childIdx, l_idx}
            });
        m_symbols[l_idx].m_unaryProductions.emplace_back(l_childIdx);
    }
    else
    {
        Regex::OperatorNodeType l_operatorType = static_cast<Regex::OperatorNode*>(p_root.get())->getNodeOperatorType();
        m_symbols.emplace_back((l_operatorType == Regex::OperatorNodeType::SumNode) ?
            GrammarSymbolType::UnionSymbol : GrammarSymbolType::JoinSymbol);
        int l_leftChildIdx = dfsInitializationHelperProcedure(p_root->getChildNodes()[0]);
        int l_rightChildIdx = dfsInitializationHelperProcedure(p_root->getChildNodes()[1]);
        if(l_operatorType == Regex::OperatorNodeType::SumNode)
        {
            m_symbols[l_idx].m_unaryProductions.emplace_back(l_leftChildIdx);
            m_symbols[l_idx].m_unaryProductions.emplace_back(l_rightChildIdx);
        }
        else
        {
            m_binaryProductions.emplace_back(BinaryProduction
                {
                    .m_producingSymbol = l_idx,
                    .m_production = {l_leftChildIdx, l_rightChildIdx}
                });
        }
    }
    return l_idx;
}

std::unique_ptr<Grammar> GrammarFactory::parseFromRegex(
    const std::unique_ptr<Regex::RegexNode>& p_regexTree)
{
    Grammar* l_result = new Grammar();
    l_result->m_startingSymbol = l_result->dfsInitializationHelperProcedure(p_regexTree);
    return std::unique_ptr<Grammar>(l_result);
}

}