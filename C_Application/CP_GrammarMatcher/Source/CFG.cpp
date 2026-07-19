#include "CFG.hpp"
#include "RegexNode.hpp"

namespace CFG
{

int Grammar::dfsInitializationHelperProcedure(
    const std::unique_ptr<Regex::RegexNode>& p_root)
{
    int l_idx = m_symbols.size();
    if(!p_root->isMarkNode())
    {
        
        for (const auto& l_child : p_root->getChildNodes()) 
        {
            dfsInitializationHelperProcedure(l_child);
        }
    }
    else
    {
        l_idx = dfsInitializationHelperProcedure(p_root->getChildNodes().front());
        m_symbols[l_idx].m_marked = true;
    }
    
    return l_idx;
}

std::unique_ptr<Grammar> GrammarFactory::parseFromRegex(
    const std::unique_ptr<Regex::RegexNode>& p_regexTree)
{
    Grammar* l_result = new Grammar();
    l_result->m_startingSymbol = l_result->dfsInitializationHelperProcedure(p_regexTree);
}

}