// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegenbase/CodeGenerator.hpp>
#include <cmajor/cmmid/Interface.hpp>

namespace cmajor { namespace codegenbase {

CodeGenerator::CodeGenerator(cmajor::ir::EmittingContext& emittingContext) : emitter(CmmCreateEmitter(&emittingContext))
{
}

CodeGenerator::~CodeGenerator()
{
    CmmDestroyEmitter(emitter);
}

} } // namespace cmajor::codegen
