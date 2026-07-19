#pragma once
#include <ostream>
#include <string>

namespace Args
{

enum ArgType
{
    HelpArg,
    PatternArg,
    TextArg,
};

enum ArgSource
{
    None,
    RawText,
    FromFile,
};

std::string getSourceName(const ArgSource&);

struct ArgData
{
    ArgSource m_source = None;
    std::string m_content;
};

struct ArgsParsed
{
    bool m_isHelpFlag;
    ArgData m_pattern;
    ArgData m_text;
};

std::ostream& operator<<(std::ostream&, const ArgData&);
std::ostream& operator<<(std::ostream&, const ArgsParsed&);

}