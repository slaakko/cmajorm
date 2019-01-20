// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Utility.hpp>
#include <sstream>
#include <cctype>

namespace cmajor { namespace parsing {

std::string HexEscape(char c)
{
    std::stringstream s;
    s << "\\x" << std::hex << (unsigned int)(unsigned char)c;
    return s.str();
}

std::string XmlCharStr(char c)
{
    switch (c)
    {
        case '&': return "&amp;";
        case '<': return "&lt;";
        case '>': return "&gt;";
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        default:
        {
            if (((int)c >= 32 && (int)c <= 126) || std::isalnum(c) || std::ispunct(c))
            {
                return std::string(1, c);
            }
            else
            {
                return HexEscape(c);
            }
        }
    }
}

std::string XmlEscape(const std::string& s)
{
    std::string result;
    result.reserve(2 * s.length());
    std::string::const_iterator e = s.end();
    for (std::string::const_iterator i = s.begin(); i != e; ++i)
    {
        result.append(XmlCharStr(*i));
    }
    return result;
}

} } // namespace cmajor::parsing
