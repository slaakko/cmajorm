// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_OPERATOR_INCLUDED
#define CMAJOR_CODEDOM_OPERATOR_INCLUDED
#include <string>

namespace cmajor { namespace codedom {

enum class Operator 
{  
    deref, addrOf, plus, minus, and_, or_, not_, neg, dotStar, arrowStar, bitand_, bitxor, bitor_, mul, div, rem, shiftLeft, shiftRight, 
    less, greater, lessOrEq, greaterOrEq, eq, notEq, comma,
    assign, mulAssing, divAssign, remAssign, plusAssign, minusAssing, shiftLeftAssign, shiftRightAssign, andAssing, xorAssing, orAssign
};

Operator GetOperator(const std::u32string& operatorName);
std::u32string GetOperatorStr(Operator op);
void OperatorInit();
void OperatorDone();

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_OPERATOR_INCLUDED
