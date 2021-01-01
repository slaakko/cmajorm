// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_LEXEME_INCLUDED
#define SOULNG_LEXER_LEXEME_INCLUDED
#include <soulng/lexer/LexerApi.hpp>
#include <string>

namespace soulng { namespace lexer {

struct SOULNG_LEXER_API Lexeme
{
    Lexeme() : begin(nullptr), end(nullptr) { }
    Lexeme(const char32_t* begin_, const char32_t* end_) : begin(begin_), end(end_) { }
    std::u32string ToString() const { return std::u32string(begin, end); }
    const char32_t* begin;
    const char32_t* end;
};

SOULNG_LEXER_API inline bool operator==(const Lexeme& left, const Lexeme& right)
{
    if (left.end - left.begin != right.end - right.begin) return false;
    const char32_t* p = left.begin;
    const char32_t* q = right.begin;
    while (p != left.end)
    {
        if (*p != *q) return false;
        ++p;
        ++q;
    }
    return true;
}

SOULNG_LEXER_API inline bool operator<(const Lexeme& left, const Lexeme& right)
{
    const char32_t* p = left.begin;
    const char32_t* q = right.begin;
    while (p != left.end && q != right.end)
    {
        if (*p < *q) return true;
        if (*p > *q) return false;
        ++p;
        ++q;
    }
    if (p == left.end) return q != right.end;
    return false;
}

} } // namespace soulng::lexer

#endif // SOULNG_LEXER_LEXEME_INCLUDED
