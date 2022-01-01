// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPP_INTERFACE_INCLUDED
#define CMAJOR_CMCPP_INTERFACE_INCLUDED
#include <cmajor/cmcppbe/CmcppbeApi.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/ir/EmittingContext.hpp>

extern "C" CMCPPBE_API cmajor::ir::EmittingContext * CmcppbeCreateEmittingContext(int optimizationLevel);
extern "C" CMCPPBE_API void CmcppbeDestroyEmittingContext(cmajor::ir::EmittingContext * emittingContext);
extern "C" CMCPPBE_API cmajor::ir::Emitter * CmcppbeCreateEmitter(void* emittingContext);
extern "C" CMCPPBE_API void CmcppbeDestroyEmitter(cmajor::ir::Emitter * emitter);

#endif // CMAJOR_CMSX_INTERFACE_INCLUDED

