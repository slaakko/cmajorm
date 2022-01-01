// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_PARSER_VALUE_INCLUDED
#define SOULNG_PARSER_VALUE_INCLUDED
#include <soulng/parser/Parser.hpp>

namespace soulng::parser {

template<class T>
struct Value
{
    Value(const T& value_) : value(value_) {}
    T value;
};

} // namespace soulng::parser;

#endif // SOULNG_PARSER_VALUE_INCLUDED
