// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_IR_EMITTING_CONTEXT_INCLUDED
#define CMAJOR_IR_EMITTING_CONTEXT_INCLUDED
#include <cmajor/ir/IrApi.hpp>
#include <string>

namespace cmajor { namespace ir {

class IR_API EmittingContext
{
public:
    virtual ~EmittingContext();
    virtual int OptimizationLevel() const = 0;
    virtual const std::string& TargetTriple() const = 0;
    virtual void* DataLayout() = 0;
    virtual void* TargetMachine() = 0;
};

} } // namespace cmajor::ir

#endif // CMAJOR_IR_EMITTING_CONTEXT_INCLUDED
