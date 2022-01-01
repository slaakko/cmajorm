// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/XmlParsingLog.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>
#include <sstream>
#include <cctype>

namespace soulng { namespace lexer {

using namespace soulng::unicode;

std::u32string XmlHexEscape(char32_t c)
{
    std::stringstream s;
    s << "&#x" << std::hex << (unsigned int)c << ";";
    return ToUtf32(s.str());
}

std::u32string XmlCharStr(char32_t c)
{
    switch (c)
    {
        case '&': return U"&amp;";
        case '<': return U"&lt;";
        case '>': return U"&gt;";
        case '\a': return U"\\a";
        case '\b': return U"\\b";
        case '\f': return U"\\f";
        case '\n': return U"\\n";
        case '\r': return U"\\r";
        case '\t': return U"\\t";
        case '\v': return U"\\v";
        default:
        {
            if (((int)c >= 32 && (int)c <= 126))
            {
                return std::u32string(1, c);
            }
            else
            {
                return XmlHexEscape(c);
            }
        }
    }
    return std::u32string();
}

std::u32string XmlEscape(const std::u32string& s)
{
    std::u32string result;
    result.reserve(2 * s.length());
    std::u32string::const_iterator e = s.end();
    for (std::u32string::const_iterator i = s.begin(); i != e; ++i)
    {
        result.append(XmlCharStr(*i));
    }
    return result;
}

XmlParsingLog::XmlParsingLog(std::ostream& stream_) : ParsingLog(), formatter(stream_)
{
    formatter.SetIndentSize(1);
}

XmlParsingLog::XmlParsingLog(std::ostream& stream_, int maxLineLength_) : ParsingLog(maxLineLength_), formatter(stream_)
{
    formatter.SetIndentSize(1);
}

void XmlParsingLog::IncIndent()
{
    formatter.IncIndent();
}

void XmlParsingLog::DecIndent()
{
    formatter.DecIndent();
}

void XmlParsingLog::WriteBeginRule(const std::u32string& ruleName)
{
    Write(U"<" + ruleName + U">");
}

void XmlParsingLog::WriteEndRule(const std::u32string& ruleName)
{
    Write(U"</" + ruleName + U">");
}

void XmlParsingLog::WriteTry(const std::u32string& s)
{
    WriteElement(U"try", s);
}

void XmlParsingLog::WriteSuccess(const std::u32string& match)
{
    WriteElement(U"success", match);
}

void XmlParsingLog::WriteFail()
{
    Write(U"<fail/>");
}

void XmlParsingLog::WriteElement(const std::u32string& elementName, const std::u32string& elementContent)
{
    std::u32string converted = XmlEscape(elementContent);
    int convertedLength = static_cast<int>(converted.length());
    int lineLength = 2 * static_cast<int>(elementName.length()) + 5 + convertedLength;
    std::u32string s = converted;
    if (lineLength > MaxLineLength())
    {
        lineLength += 3;
        s = converted.substr(0, std::max(0, convertedLength - (lineLength - MaxLineLength()))) + U"...";
    }
    Write(U"<" + elementName + U">" + s + U"</" + elementName + U">");
}

void XmlParsingLog::Write(const std::u32string& s)
{
    formatter.WriteLine(ToUtf8(s));
}

} } // namespace soulng::lexer
