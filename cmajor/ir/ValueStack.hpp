// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_IR_VALUE_STACK_INCLUDED
#define CMAJOR_IR_VALUE_STACK_INCLUDED
#include <cmajor/ir/IrApi.hpp>

namespace cmajor { namespace ir {

class IR_API ValueStack
{
public:
    virtual void Push(void* value) = 0;
    virtual void* Pop() = 0;
    virtual void Dup() = 0;
    virtual void Swap() = 0;
    virtual void Rotate() = 0;
};

} } // namespace cmajor::ir

#endif // CMAJOR_IR_VALUE_STACK_INCLUDED
