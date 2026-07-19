#include <ostream>
#include "ArgsParsed.hpp"


std::string Args::getSourceName(const ArgSource& p_source)
{
    switch (p_source) 
    {
        case None:
            return "None";
        case FromFile:
            return "From file";
        case RawText:
            return "From argument";
    }
    return "";
}

std::ostream& Args::operator<<(std::ostream& p_stream, const ArgData& p_argData)
{
    p_stream << "["  << p_argData.m_source << ": " << p_argData.m_content << "]";
    return p_stream;
}

std::ostream& Args::operator<<(std::ostream& p_stream, const ArgsParsed& p_argsParsed)
{
    p_stream << "Help: " << p_argsParsed.m_isHelpFlag << "\n"
             << "Pattern: " << p_argsParsed.m_pattern << "\n"
             << "Text: " << p_argsParsed.m_text << "\n";
    return p_stream;
}