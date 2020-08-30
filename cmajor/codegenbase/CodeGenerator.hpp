// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENBASE_CODEGENERATOR_INCLUDED
#define CMAJOR_CODEGENBASE_CODEGENERATOR_INCLUDED
#include <cmajor/codegenbase/CodeGenBaseApi.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/ir/EmittingContext.hpp>

namespace cmajor { namespace codegenbase {

class CODEGENBASE_API CodeGenerator
{
public:
    CodeGenerator(cmajor::ir::EmittingContext& emittingContext);
    cmajor::ir::Emitter* GetEmitter() { return emitter; }
    virtual ~CodeGenerator();
    virtual void GenerateCode(void* boundCompileUnit) = 0;
private:
    cmajor::ir::Emitter* emitter;
};

} } // namespace cmajor::codegenbase

#endif // CMAJOR_CODEGENBASE_CODEGENERATOR_INCLUDED
