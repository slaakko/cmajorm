// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_INTERFACE_INCLUDED
#define CMAJOR_CMSX_INTERFACE_INCLUDED
#include <cmajor/cmsxbe/CmsxbeApi.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/ir/EmittingContext.hpp>

extern "C" CMSXBE_API cmajor::ir::EmittingContext* CmsxbeCreateEmittingContext(int optimizationLevel);
extern "C" CMSXBE_API void CmsxbeDestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext);
extern "C" CMSXBE_API cmajor::ir::Emitter* CmsxbeCreateEmitter(void* emittingContext);
extern "C" CMSXBE_API void CmsxbeDestroyEmitter(cmajor::ir::Emitter* emitter);

#endif // CMAJOR_CMSX_INTERFACE_INCLUDED
