// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSER_OPERATOR_INCLUDED
#define CMAJOR_PARSER_OPERATOR_INCLUDED
#include <stdint.h>

namespace cmajor { namespace parser {

enum class Operator : uint8_t
{
    or_, and_, eq, neq, less, greater, lessOrEq, greaterOrEq, is, as, plusplus, minusminus, plus, minus, mul, div, rem, not_, bitOr, bitXor, bitAnd, shiftLeft, shiftRight, complement, deref, addrOf
};

} } // namespace cmajor::parser

#endif // CMAJOR_PARSER_OPERATOR_INCLUDED
