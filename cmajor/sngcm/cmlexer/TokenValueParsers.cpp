// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/cmlexer/TokenValueParsers.hpp>
#include <soulng/util/Unicode.hpp>
#include <sstream>

using namespace soulng::unicode;

void ParseFloatingLiteral(const std::string& fileName, const Token& token, double& floatingLit, bool& floatingLitFloat)
{
    floatingLit = 0.0;
    floatingLitFloat = false;
    const char32_t* p = token.match.begin;
    const char32_t* e = token.match.end;
    std::string str;
    while (p != e && ((*p >= '0' && *p <= '9') || *p == '.' || *p == 'e' || *p == 'E' || *p == '-' || *p == '+'))
    {
        str.append(1, static_cast<unsigned char>(*p));
        ++p;
    }
    if (p != e && (*p == 'f' || *p == 'F'))
    {
        ++p;
        floatingLitFloat = true;
    }
    if (p != e)
    {
        throw std::runtime_error("invalid floating literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
    }
    std::stringstream s;
    s.str(str);
    s >> floatingLit;
    if (s.fail() || s.bad())
    {
        throw std::runtime_error("invalid floating literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
    }
}

void ParseIntegerLiteral(const std::string& fileName, const Token& token, uint64_t& intLit, bool& intLitUnsigned)
{
    intLit = 0;
    intLitUnsigned = false;
    const char32_t* p = token.match.begin;
    const char32_t* e = token.match.end;
    if (p != e && *p == '0')
    {
        ++p;
        if (p != e && (*p == 'x' || *p == 'X'))
        {
            ++p;
            while (p != e && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
            {
                switch (*p)
                {
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    {
                        intLit = 16 * intLit + *p - '0';
                        break;
                    }
                    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                    {
                        intLit = 16 * intLit + 10 + *p - 'A';
                        break;
                    }
                    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                    {
                        intLit = 16 * intLit + 10 + *p - 'a';
                        break;
                    }
                }
                ++p;
            }
        }
        else 
        {
            while (p != e && *p >= '0' && *p <= '7')
            {
                intLit = 8 * intLit + (*p - '0');
                ++p;
            }
        }
    }
    else if (p != e && *p >= '1' && *p <= '9')
    {
        while (p != e && *p >= '0' && *p <= '9')
        {
            intLit = 10 * intLit + (*p - '0');
            ++p;
        }
    }
    else
    {
        throw std::runtime_error("invalid integer literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
    }
    if (p != e && (*p == 'u' || *p == 'U'))
    {
        ++p;
        intLitUnsigned = true;
    }
    if (p != e)
    {
        throw std::runtime_error("invalid integer literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
    }
}

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

void ParseCharacterLiteral(const std::string& fileName, const Token& token, char32_t& charLit, int& charLitPrefix)
{
    charLit = '\0';
    charLitPrefix = noPrefix;
    const char32_t* p = token.match.begin;
    const char32_t* e = token.match.end;
    if (p != e && *p == 'w')
    {
        charLitPrefix = utf16Prefix;
        ++p;
    }
    else if (p != e && *p == 'u')
    {
        charLitPrefix = utf32Prefix;
        ++p;
    }
    if (p != e && *p == '\'')
    {
        ++p;
        if (p != e && *p == '\\')
        {
            ++p;
            charLit = ParseEscape(fileName, p, e, token);
        }
        else 
        {
            std::u32string s;
            while (p != e && *p != '\r' && *p != '\n' && *p != '\'')
            {
                s.append(1, *p);
                ++p;
            }
            std::u32string u = s;
            if (u.size() != 1)
            {
                throw std::runtime_error("invalid character literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
            }
            charLit = u.front();
        }
        if (p != e && *p == '\'')
        {
            ++p;
        }
        if (p != e)
        {
            throw std::runtime_error("invalid character literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
        }
    }
    else
    {
        throw std::runtime_error("invalid character literal at " + fileName + ":" + std::to_string(token.line) + ": " + ToUtf8(std::u32string(token.match.begin, token.match.end)));
    }
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

std::string MakeFilePath(const Lexeme& lexeme)
{
    std::u32string s;
    const char32_t* p = lexeme.begin;
    const char32_t* e = lexeme.end;
    if (p != e && *p == '<')
    {
        ++p;
    }
    while (p != e && *p != '>')
    {
        s.append(1, *p);
        ++p;
    }
    if (p != e && *p == '>')
    {
        ++p;
    }
    if (p != e)
    {
        throw std::runtime_error("invalid file path '" + ToUtf8(std::u32string(lexeme.begin, lexeme.end)));
    }
    return ToUtf8(s);
}
