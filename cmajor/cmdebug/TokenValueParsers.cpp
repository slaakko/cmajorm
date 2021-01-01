// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/TokenValueParsers.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace cmajor { namespace debug {

using namespace soulng::lexer;
using namespace soulng::unicode;

const int noPrefix = 0;
const int utf16Prefix = 1;
const int utf32Prefix = 2;

void ParseHexChar(const std::string& fileName, char32_t& value, const char32_t*& p, const char32_t* e, const Token& token)
{
    if (p != e)
    {
        bool notHex = false;
        switch (*p)
        {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
        {
            value = 16 * value + *p - '0';
            break;
        }
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        {
            value = 16 * value + 10 + *p - 'A';
            break;
        }
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        {
            value = 16 * value + 10 + *p - 'a';
            break;
        }
        default:
        {
            notHex = true;
            break;
        }
        }
        if (notHex)
        {
            throw std::runtime_error("hex character expected at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
        }
        ++p;
    }
    else
    {
        throw std::runtime_error("hex character expected at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
    }
}

char32_t ParseEscape(const std::string& fileName, const char32_t*& p, const char32_t* e, const Token& token)
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
            value = 10 * value + (*p - '0');
            ++p;
        }
    }
    else if (p != e && (*p >= '0' && *p <= '7'))
    {
        while (p != e && *p >= '0' && *p <= '7')
        {
            value = 8 * value + (*p - '0');
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
        switch (*p)
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

void ParseStringLiteral(const std::string& fileName, const Token& token, std::u32string& stringLit, int& stringLitPrefix)
{
    stringLit.clear();
    stringLitPrefix = noPrefix;
    const char32_t* p = token.match.begin;
    const char32_t* e = token.match.end;
    if (p != e && *p == 'w')
    {
        stringLitPrefix = utf16Prefix;
        ++p;
    }
    else if (p != e && *p == 'u')
    {
        stringLitPrefix = utf32Prefix;
        ++p;
    }
    if (p != e && *p == '@')
    {
        ++p;
        if (p != e && *p == '"')
        {
            ++p;
            while (p != e && *p != '"')
            {
                stringLit.append(1, *p);
                ++p;
            }
            if (p != e && *p == '"')
            {
                ++p;
            }
            if (p != e)
            {
                throw std::runtime_error("invalid string literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
            }
        }
    }
    else
    {
        if (p != e && *p == '"')
        {
            ++p;
            while (p != e && *p != '\r' && *p != '\n' && *p != '"')
            {
                if (*p == '\\')
                {
                    ++p;
                    stringLit.append(1, ParseEscape(fileName, p, e, token));
                }
                else
                {
                    stringLit.append(1, *p);
                    ++p;
                }
            }
            if (p != e && *p == '"')
            {
                ++p;
            }
            if (p != e)
            {
                throw std::runtime_error("invalid string literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
            }
        }
        else
        {
            throw std::runtime_error("invalid string literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
        }
    }
}

std::string ParseString(const Token& token)
{
    std::u32string stringLit;
    int stringLitPrefix;
    ParseStringLiteral("", token, stringLit, stringLitPrefix);
    return ToUtf8(stringLit);
}

int ParseInt(const std::string& str)
{
    return boost::lexical_cast<int>(str);
}

int64_t ParseLong(const std::string& str)
{
    return boost::lexical_cast<int64_t>(str);
}

std::string ParseVmtVariableName(const std::string& str)
{
    std::string::size_type leftAnglePos = str.find('<');
    if (leftAnglePos != std::string::npos)
    {
        std::string::size_type rightAnglePos = str.find('>', leftAnglePos + 1);
        if (rightAnglePos != std::string::npos)
        {
            return str.substr(leftAnglePos + 1, rightAnglePos - (leftAnglePos + 1));
        }
    }
    return std::string();
}

} } // namespace cmajor::debug
