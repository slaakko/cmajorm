// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_PARSER_RANGE_INCLUDED
#define SOULNG_PARSER_RANGE_INCLUDED
#include <soulng/parser/Parser.hpp>

namespace soulng::parser {

struct SOULNG_PARSER_API Range
{
    int first;
    int last;
};

} // namespace soulng::parser;

#endif // SOULNG_PARSER_RANGE_INCLUDED
