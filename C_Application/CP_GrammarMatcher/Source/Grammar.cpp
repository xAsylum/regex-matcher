#include "Grammar.hpp"
#include "RegexNode.hpp"
#include <algorithm>
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
    l_result->transitiveComplementOfUnaryProducers();
    return std::unique_ptr<Grammar>(l_result);
}

const std::vector<GrammarSymbol>& Grammar::getGrammarSymbols()
{
    return this->m_symbols;
}
const std::vector<int>& Grammar::getLetterSymbols(char p_letter)
{
    return this->m_letterSymbols[unsigned(p_letter)];
}
const std::vector<int>& Grammar::getWildcardSymbols()
{
    return this->m_wildcardSymbols;
}
const std::vector<BinaryProduction>& Grammar::getBinaryProductions()
{
    return this->m_binaryProductions;
}
int Grammar::getStartingSymbol()
{
    return this->m_startingSymbol;
}

void Grammar::transitiveComplementOfUnaryProducers()
{
    // This will use Warshall-Floyd algorithm. I'm aware that there is more efficient approach, 
    // but this will do for the sake of this project and grammar preprocessing.
    const unsigned l_graphSize = m_symbols.size();
    std::vector<std::vector<bool>> l_adjacencyMatrix(l_graphSize, 
        std::vector<bool>(l_graphSize, false));
    for (unsigned l_symbolIdx = 0; l_symbolIdx < l_graphSize; l_symbolIdx++)
    {
        const auto& l_symbol = m_symbols[l_symbolIdx];
        for (unsigned l_destination : l_symbol.m_unaryProductions)
        {
            l_adjacencyMatrix[l_destination][l_symbolIdx] = true;
        }
        l_adjacencyMatrix[l_symbolIdx][l_symbolIdx] = true;
    }
    for (unsigned l_mid = 0; l_mid < l_graphSize; l_mid++)
    {
        for (unsigned l_left = 0; l_left < l_graphSize; l_left++)
        {
            for (unsigned l_right = 0; l_right < l_graphSize; l_right++)
            {
                l_adjacencyMatrix[l_left][l_right] = (l_adjacencyMatrix[l_left][l_right]) 
                    or ((l_adjacencyMatrix[l_left][l_mid]) and (l_adjacencyMatrix[l_mid][l_right]));
            }
        }
    }
    for (unsigned l_symbolIdx = 0; l_symbolIdx < l_graphSize; l_symbolIdx++)
    {
        auto& l_symbol = m_symbols[l_symbolIdx];
        
        for (unsigned l_destination = 0; l_destination < l_graphSize; l_destination++)
        {
            if (l_adjacencyMatrix[l_symbolIdx][l_destination] and l_symbolIdx != l_destination)
            {
                l_symbol.m_transiviteComplementListOfUnaryProducers.push_back(l_destination);
            }
        }
    }
}

}