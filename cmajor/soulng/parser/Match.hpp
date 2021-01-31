// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_PARSER_MATCH_INCLUDED
#define SOULNG_PARSER_MATCH_INCLUDED
#include <soulng/parser/ParserApi.hpp>
#include <soulng/lexer/Lexer.hpp>

namespace soulng { namespace parser {

struct SOULNG_PARSER_API Match
{
    Match(bool hit_) : hit(hit_), value(nullptr) {}
    Match(bool hit_, void* value_) : hit(hit_), value(value_) {}
    bool hit;
    void* value;
};

} } // namespace soulng::parser;

#endif // SOULNG_PARSER_MATCH_INCLUDED
