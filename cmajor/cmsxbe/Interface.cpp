// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxbe/Interface.hpp>
#include <cmajor/cmsxbe/EmittingContext.hpp>
#include <cmajor/cmsxbe/Emitter.hpp>

extern "C" CMSXBE_API cmajor::ir::EmittingContext* CmsxbeCreateEmittingContext(int optimizationLevel)
{
    return new cmsxbe::EmittingContext(optimizationLevel);
}

extern "C" CMSXBE_API void CmsxbeDestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext)
{
    delete emittingContext;
}

extern "C" CMSXBE_API cmajor::ir::Emitter* CmsxbeCreateEmitter(void* emittingContext)
{
    cmsxbe::EmittingContext* beEmittingContext = static_cast<cmsxbe::EmittingContext*>(emittingContext);
    return new cmsxbe::Emitter(beEmittingContext);
}

extern "C" CMSXBE_API void CmsxbeDestroyEmitter(cmajor::ir::Emitter* emitter)
{
    delete emitter;
}
