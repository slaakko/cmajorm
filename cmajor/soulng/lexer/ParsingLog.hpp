// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_PARSING_LOG_INCLUDED
#define SOULNG_LEXER_PARSING_LOG_INCLUDED
#include <soulng/lexer/LexerApi.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <string>

namespace soulng { namespace lexer {

class SOULNG_LEXER_API ParsingLog
{
public:
    ParsingLog();
    ParsingLog(int maxLineLength_);
    virtual ~ParsingLog();
    virtual void IncIndent() = 0;
    virtual void DecIndent() = 0;
    virtual void WriteBeginRule(const std::u32string& ruleName) = 0;
    virtual void WriteEndRule(const std::u32string& ruleName) = 0;
    virtual void WriteTry(const std::u32string& s) = 0;
    virtual void WriteSuccess(const std::u32string& match) = 0;
    virtual void WriteFail() = 0;
    virtual int MaxLineLength() const;
private:
    int maxLineLength;
};

} } // namespace soulng::lexer

#endif //  SOULNG_LEXER_PARSING_LOG_INCLUDED
