// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGEN_EMITTING_CONTEXT_INCLUDED
#define CMAJOR_CODEGEN_EMITTING_CONTEXT_INCLUDED
#include <cmajor/codegen/CodeGenApi.hpp>
#include <cmajor/ir/EmittingContext.hpp>

namespace cmajor { namespace codegen {

class CODEGEN_API EmittingContext
{
public:
    EmittingContext(int optimizationLevel);
    ~EmittingContext();
    cmajor::ir::EmittingContext* GetBaseEmittingContext() { return baseEmittingContext; }
private:
    cmajor::ir::EmittingContext* baseEmittingContext;
};

} } // namespace cmajor::codegen

#endif // CMAJOR_CODEGEN_CODEGENERATOR_INCLUDED
