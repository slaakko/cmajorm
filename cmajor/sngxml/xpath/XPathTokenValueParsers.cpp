// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/XPathTokenValueParsers.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngxml { namespace xpath {

std::u32string ParseDQString(const std::string& fileName, const soulng::lexer::Token& token)
{
    std::u32string s;
    const char32_t* p = token.match.begin;
    const char32_t* e = token.match.end;
    if (p != e && *p == '"')
    {
        ++p;
    }
    while (p != e && *p != '"')
    {
        s.append(1, *p);
        ++p;
    }
    if (p != e && *p == '"')
    {
        ++p;
    }
    if (p != e)
    {
        throw std::runtime_error("invalid string literal at " + fileName + ":" + std::to_string(token.line) + ": " + soulng::unicode::ToUtf8(token.match.ToString()));
    }
    return s;
}

std::u32string ParseSQString(const std::string& fileName, const soulng::lexer::Token& token)
{
    std::u32string s;
    const char32_t* p = token.match.begin;
    const char32_t* e = token.match.end;
    if (p != e && *p == '\'')
    {
        ++p;
    }
    while (p != e && *p != '\'')
    {
        s.append(1, *p);
        ++p;
    }
    if (p != e && *p == '\'')
    {
        ++p;
    }
    if (p != e)
    {
        throw std::runtime_error("invalid string literal at " + fileName + ":" + std::to_string(token.line) + ": " + soulng::unicode::ToUtf8(token.match.ToString()));
    }
    return s;
}

} } // namespace sngxml::xpath
