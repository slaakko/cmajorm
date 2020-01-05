// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmid/Interface.hpp>
#include <cmajor/cmmid/BackEnd.hpp>

extern "C" CMM_API cmajor::ir::EmittingContext* CmmCreateEmittingContext(int optimizationLevel)
{
    return cmajor::mid::backEnd->CreateEmittingContext(optimizationLevel);
}

extern "C" CMM_API void CmmDestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext)
{
    cmajor::mid::backEnd->DestroyEmittingContext(emittingContext);
}

extern "C" CMM_API cmajor::ir::Emitter* CmmCreateEmitter(cmajor::ir::EmittingContext* emittingContext)
{
    return cmajor::mid::backEnd->CreateEmitter(emittingContext);
}

extern "C" CMM_API void CmmDestroyEmitter(cmajor::ir::Emitter* emitterHandle)
{
    cmajor::mid::backEnd->DestroyEmitter(emitterHandle);
}

extern "C" CMM_API cmajor::mid::BackEndKind GetBackEnd()
{
    return cmajor::mid::backEnd->GetBackEndKind();
}
