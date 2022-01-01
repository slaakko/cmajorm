// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/rex/TokenValueParsers.hpp>
#include <soulng/util/Unicode.hpp>

namespace soulng { namespace rex {

using namespace soulng::unicode;

void ParseHexChar(const std::string& fileName, char32_t& value, const char32_t*& p, const char32_t* e, const soulng::lexer::Token& token)
{
    if (p != e)
    {
        switch (static_cast<char32_t>(*p))
        {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        {
            value = static_cast<char32_t>(16 * static_cast<int>(value) + static_cast<int>(*p) - static_cast<int>('0'));
            break;
        }
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        {
            value = static_cast<char32_t>(16 * static_cast<int>(value) + 10 + static_cast<int>(*p) - static_cast<int>('A'));
            break;
        }
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        {
            value = static_cast<char32_t>(16 * static_cast<int>(value) + 10 + static_cast<int>(*p) - static_cast<int>('a'));
            break;
        }
        }
        ++p;
    }
    else
    {
        throw std::runtime_error("hex character expected at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(token.match.ToString()));
    }
}


char32_t ParseEscape(const std::string& fileName, const char32_t*& p, const char32_t* e, const soulng::lexer::Token& token)
{
    char32_t value = '\0';
    if (p != e && (*p == 'x' || *p == 'X'))
    {
        ++p;
        while (p != e && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
        {
            ParseHexChar(fileName, value, p, e, token);
        }
    }
    else if (p != e && (*p == 'd' || *p == 'D'))
    {
        ++p;
        while (p != e && *p >= '0' && *p <= '9')
        {
            value = static_cast<char32_t>(10 * static_cast<int>(value) + static_cast<int>(*p) - static_cast<int>('0'));
            ++p;
        }
    }
    else if (p != e && (*p >= '0' && *p <= '7'))
    {
        while (p != e && *p >= '0' && *p <= '7')
        {
            value = static_cast<char32_t>(8 * static_cast<int>(value) + static_cast<int>(*p) - static_cast<int>('0'));
            ++p;
        }
    }
    else if (p != e && *p == 'u')
    {
        ++p;
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
    }
    else if (p != e && *p == 'U')
    {
        ++p;
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
        ParseHexChar(fileName, value, p, e, token);
    }
    else if (p != e)
    {
        switch (static_cast<char32_t>(*p))
        {
        case 'a': value = '\a'; break;
        case 'b': value = '\b'; break;
        case 'f': value = '\f'; break;
        case 'n': value = '\n'; break;
        case 'r': value = '\r'; break;
        case 't': value = '\t'; break;
        case 'v': value = '\v'; break;
        default: value = *p; break;
        }
        ++p;
    }
    return value;
}

char32_t MakeEscapeValue(const std::string& fileName, const soulng::lexer::Token& token)
{
    char32_t escapeValue = '\0';
    const char32_t* p = token.match.begin;
    const char32_t* e = token.match.end;
    if (p != e && *p == '\\')
    {
        ++p;
    }
    escapeValue = ParseEscape(fileName, p, e, token);
    if (p != e)
    {
        throw std::runtime_error("invalid escape at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(token.match.ToString()));
    }
    return escapeValue;
}

} } // namespace soulng::rex
