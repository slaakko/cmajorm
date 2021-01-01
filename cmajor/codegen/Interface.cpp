// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegen/Interface.hpp>
#include <cmajor/codegen/CodeGenerator.hpp>
#include <cmajor/ir/EmittingContext.hpp>

namespace cmajor { namespace codegen {

CODEGEN_API void GenerateCode(cmajor::codegen::EmittingContext& emittingContext, cmajor::binder::BoundCompileUnit& boundCompileUnit)
{
    cmajor::ir::EmittingContext* baseEmittingContext = emittingContext.GetBaseEmittingContext();
    cmajor::codegen::CodeGenerator codeGenerator(*baseEmittingContext);
    cmajor::codegenbase::CodeGenerator* baseCodeGenerator = codeGenerator.GetBaseCodeGenerator();
    baseCodeGenerator->GenerateCode(&boundCompileUnit);
}

} } // namespace cmajor::codegen
