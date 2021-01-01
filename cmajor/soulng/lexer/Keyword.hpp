// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_KEYWORD_INCLUDED
#define SOULNG_LEXER_KEYWORD_INCLUDED
#include <soulng/lexer/Token.hpp>
#include <map>

namespace soulng { namespace lexer {

struct SOULNG_LEXER_API Keyword
{
    Keyword() : str(nullptr), tokenID(INVALID_TOKEN) {}
    Keyword(const char32_t* str_, int tokenID_) : str(str_), tokenID(tokenID_) { }
    const char32_t* str;
    int tokenID;
};

class SOULNG_LEXER_API KeywordMap
{
public:
    KeywordMap(const Keyword* keywords_);
    int GetKeywordToken(const Lexeme& lexeme) const;
private:
    const Keyword* keywords;
    std::map<Lexeme, int> keywordMap;
};

} } // namespace soulng::lexer

#endif // SOULNG_LEXER_KEYWORD_INCLUDED
