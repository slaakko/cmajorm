// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_PARSER_OPERATOR_INCLUDED
#define SNGCM_PARSER_OPERATOR_INCLUDED
#include <stdint.h>

enum class Operator : uint8_t
{
    none = 0, or_, and_, eq, neq, less, greater, lessOrEq, greaterOrEq, is, as, plusplus, minusminus, plus, minus, mul, div, rem, not_, bitOr, bitXor, bitAnd, shiftLeft, shiftRight, complement, deref, addrOf
};

#endif // SNGCM_PARSER_OPERATOR_INCLUDED
