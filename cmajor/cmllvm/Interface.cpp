// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmllvm/Interface.hpp>
#include <cmajor/cmllvm/EmittingContext.hpp>
#include <cmajor/cmllvm/Emitter.hpp>

extern "C" CMLLVM_API cmajor::ir::EmittingContext* CmllvmCreateEmittingContext(int optimizationLevel)
{
    return new cmllvm::EmittingContext(optimizationLevel);
}

extern "C" CMLLVM_API void CmllvmDestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext)
{
    delete emittingContext;
}

extern "C" CMLLVM_API cmajor::ir::Emitter* CmllvmCreateEmitter(void* emittingContextHandle)
{
    cmllvm::EmittingContext* emittingContext = static_cast<cmllvm::EmittingContext*>(emittingContextHandle);
    cmllvm::Emitter* emitter = new cmllvm::Emitter(emittingContext->Context(), *emittingContext);
    return emitter;
}

extern "C" CMLLVM_API void CmllvmDestroyEmitter(cmajor::ir::Emitter* emitterHandle)
{
    cmllvm::Emitter* emitter = static_cast<cmllvm::Emitter*>(emitterHandle);
    delete emitter;
}
