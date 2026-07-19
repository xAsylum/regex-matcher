#pragma once

#include "ArgsParsed.hpp"
#include <list>
#include <string>
namespace Args
{

class ArgsParser
{
public:
    ArgsParser(int argc, char** argv);
    const ArgsParsed& getBundle() const;
private:
    std::list<std::string> sanitizeArgs(int argc, char** argv);
    void createBundle(const std::list<std::string>& p_argList);
    ArgsParsed m_argsBundle;
};

}