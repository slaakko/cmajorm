// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmid/LlvmBackEnd.hpp>
#include <cmajor/cmllvm/Interface.hpp>

namespace cmajor { namespace mid {

void LLvmBackEnd::Init()
{
}

void LLvmBackEnd::Done()
{
}

cmajor::ir::EmittingContext* LLvmBackEnd::CreateEmittingContext(int optimizationLevel)
{
    return CmllvmCreateEmittingContext(optimizationLevel);
}

void LLvmBackEnd::DestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext)
{
    CmllvmDestroyEmittingContext(emittingContext);
}

cmajor::ir::Emitter* LLvmBackEnd::CreateEmitter(cmajor::ir::EmittingContext* emittingContext)
{
    return CmllvmCreateEmitter(emittingContext);
}

void LLvmBackEnd::DestroyEmitter(cmajor::ir::Emitter* emitterHandle)
{
    CmllvmDestroyEmitter(emitterHandle);
}

} } // namespace cmajor::mid
