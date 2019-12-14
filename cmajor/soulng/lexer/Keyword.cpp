// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/Keyword.hpp>

namespace soulng { namespace lexer {

inline const char32_t* StrEnd(const char32_t* s)
{
    while (*s)
    {
        ++s;
    }
    return s;
}

KeywordMap::KeywordMap(const Keyword* keywords_) : keywords(keywords_)
{
    const Keyword* kw = keywords;
    while (kw->str)
    {
        Lexeme lexeme(kw->str, StrEnd(kw->str));
        keywordMap[lexeme] = kw->tokenID;
        ++kw;
    }
}

int KeywordMap::GetKeywordToken(const Lexeme& lexeme) const
{
    std::map<Lexeme, int>::const_iterator it = keywordMap.find(lexeme);
    if (it != keywordMap.cend())
    {
        return it->second;
    }
    else
    {
        return INVALID_TOKEN;
    }
}

} } // namespace soulng::lexer
