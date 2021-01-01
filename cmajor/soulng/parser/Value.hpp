// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_PARSER_VALUE_INCLUDED
#define SOULNG_PARSER_VALUE_INCLUDED
#include <soulng/parser/ParserApi.hpp>

namespace soulng { namespace parser {

template<class T>
struct Value
{
    Value(const T& value_) : value(value_) {}
    T value;
};

} } // namespace soulng::parser;

#endif // SOULNG_PARSER_VALUE_INCLUDED
