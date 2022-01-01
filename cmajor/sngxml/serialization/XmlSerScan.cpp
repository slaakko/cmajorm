// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlSerScan.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngxml { namespace xmlser {

using namespace soulng::unicode;

std::string ScanEndCpp(soulng::lexer::Lexer& lexer)
{
    std::u32string cppText;
    soulng::lexer::Token token = lexer.GetToken(lexer.GetPos());
    if (token.id != soulng::lexer::END_TOKEN)
    {
        cppText.append(token.match.ToString());
    }
    bool inString = false;
    bool prevWasBackslash = false;
    const char32_t* p = lexer.Pos();
    while (p != lexer.End())
    {
        if (*p == '"')
        {
            if (!inString)
            {
                inString = true;
                cppText.append(1, *p);
                ++p;
            }
            else if (!prevWasBackslash)
            {
                inString = false;
                cppText.append(1, *p);
                ++p;
            }
            else
            {
                prevWasBackslash = false;
                cppText.append(1, *p);
                ++p;
            }
        }
        else if (!inString && *p == '%')
        {
            ++p;
            if (*p == '>')
            {
                ++p;
                lexer.SetPos(lexer.Tokens().size() - 1);
                lexer.SetPos(p);
                ++lexer;
                break;
            }
            else
            {
                cppText.append(1, '%');
                cppText.append(1, *p);
                ++p;
            }
        }
        else
        {
            if (*p == '\\')
            {
                prevWasBackslash = true;
            }
            else
            {
                prevWasBackslash = false;
            }
            cppText.append(1, *p);
            ++p;
        }
    }
    if (p == lexer.End())
    {
        throw std::runtime_error("end of C++ block: token '%>' not found");
    }
    return ToUtf8(cppText);
}

} } // namespace sngxml::xmlser
