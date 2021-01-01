// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_SPAN_INCLUDED
#define SOULNG_LEXER_SPAN_INCLUDED
#include <soulng/lexer/LexerApi.hpp>

namespace soulng { namespace lexer {

struct SOULNG_LEXER_API Span
{
    Span() : fileIndex(-1), line(-1), start(-1), end(-1) {}
    Span(int fileIndex_, int line_, int start_) : fileIndex(fileIndex_), line(line_), start(start_), end(start) {}
    Span(int fileIndex_, int line_, int start_, int end_) : fileIndex(fileIndex_), line(line_), start(start_), end(end_) {}
    bool Valid() const { return fileIndex != -1 && line != -1 && start != -1 && end != -1; }
    int fileIndex;
    int line;
    int start;
    int end;
};

SOULNG_LEXER_API bool operator==(const Span& left, const Span& right);

} } // namespace soulng::lexer

#endif // SOULNG_LEXER_SPAN_INCLUDED
