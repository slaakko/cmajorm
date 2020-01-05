// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_LLVM_INTERFACE_INCLUDED
#define CMAJOR_LLVM_INTERFACE_INCLUDED
#include <cmajor/cmllvm/CmllvmApi.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/ir/EmittingContext.hpp>

extern "C" CMLLVM_API cmajor::ir::EmittingContext* CmllvmCreateEmittingContext(int optimizationLevel);
extern "C" CMLLVM_API void CmllvmDestroyEmittingContext(cmajor::ir::EmittingContext* emittingContextHandle);
extern "C" CMLLVM_API cmajor::ir::Emitter* CmllvmCreateEmitter(void* emittingContextHandle);
extern "C" CMLLVM_API void CmllvmDestroyEmitter(cmajor::ir::Emitter* emitterHandle);


#endif // CMAJOR_LLVM_INTERFACE_INCLUDED
