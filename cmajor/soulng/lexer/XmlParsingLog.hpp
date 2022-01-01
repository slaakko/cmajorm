// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_XML_PARSING_LOG_INCLUDED
#define SOULNG_LEXER_XML_PARSING_LOG_INCLUDED
#include <soulng/lexer/ParsingLog.hpp>

namespace soulng { namespace lexer {

class SOULNG_LEXER_API XmlParsingLog : public ParsingLog
{
public:
    XmlParsingLog(std::ostream& stream_);
    XmlParsingLog(std::ostream& stream_, int maxLineLength_);
    void IncIndent() override;
    void DecIndent() override;
    void WriteBeginRule(const std::u32string& ruleName) override;
    void WriteEndRule(const std::u32string& ruleName) override;
    void WriteTry(const std::u32string& s) override;
    void WriteSuccess(const std::u32string& match) override;
    void WriteFail() override;
    void WriteElement(const std::u32string& elementName, const std::u32string& elementContent);
    void Write(const std::u32string& s);
    soulng::util::CodeFormatter& Formatter() { return formatter; }
private:
    soulng::util::CodeFormatter formatter;
};

SOULNG_LEXER_API std::u32string XmlHexEscape(char32_t c);
SOULNG_LEXER_API std::u32string XmlCharStr(char32_t c);
SOULNG_LEXER_API std::u32string XmlEscape(const std::u32string& s);

} } // namespace soulng::lexer

#endif // SOULNG_LEXER_XML_PARSING_LOG_INCLUDED
