#include <iterator>
#include <list>
#include <string>
#include "ArgsParser.hpp"
#include "ArgsParsed.hpp"

namespace Args
{
    ArgsParser::ArgsParser(int argc, char** argv) {
        const auto l_argsList = sanitizeArgs(argc, argv);
        createBundle(l_argsList);
    }

    const ArgsParsed& ArgsParser::getBundle() const {
        return m_argsBundle;
    }

    void ArgsParser::createBundle(const std::list<std::string>& p_argList) {
        ArgsParsed l_parsedArgs = {};
        for (auto l_it = p_argList.begin(); l_it != p_argList.end(); l_it++)
        {
            const auto& l_str = *l_it;
            if(not l_str.starts_with('-')) 
            {
                continue;
            }
            if (l_str == "-h") 
            {
                l_parsedArgs.m_isHelpFlag = true;
                continue;
            }
            const auto& l_nextIt = std::next(l_it);
            if (l_nextIt == p_argList.end() or l_nextIt->starts_with('-')) 
            {
                break;
            }
            if (l_str == "-p") 
            {
                l_parsedArgs.m_pattern = ArgData
                {
                    .m_source = RawText,
                    .m_content = *l_nextIt
                };
            }
            else if (l_str == "-t") 
            {
                l_parsedArgs.m_text = ArgData
                {
                    .m_source = RawText,
                    .m_content = *l_nextIt
                };
            }
            l_it++;
        }
        m_argsBundle = l_parsedArgs;
    }
    std::list<std::string> ArgsParser::sanitizeArgs(int argc, char** argv) 
    {
        std::list<std::string> l_list = {};
        for (int i = 0; i < argc; i++) {
            l_list.push_back(std::string(*argv));
            argv++;
        }
        return l_list;
    }
}