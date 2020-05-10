// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppbe/Interface.hpp>
#include <cmajor/cmcppbe/EmittingContext.hpp>
#include <cmajor/cmcppbe/Emitter.hpp>

extern "C" CMCPPBE_API cmajor::ir::EmittingContext * CmcppbeCreateEmittingContext(int optimizationLevel)
{
    return new cmcppbe::EmittingContext(optimizationLevel);
}

extern "C" CMCPPBE_API void CmcppbeDestroyEmittingContext(cmajor::ir::EmittingContext * emittingContext)
{
    delete emittingContext;
}

extern "C" CMCPPBE_API cmajor::ir::Emitter * CmcppbeCreateEmitter(void* emittingContext)
{
    cmcppbe::EmittingContext* beEmittingContext = static_cast<cmcppbe::EmittingContext*>(emittingContext);
    return new cmcppbe::Emitter(beEmittingContext);
}

extern "C" CMCPPBE_API void CmcppbeDestroyEmitter(cmajor::ir::Emitter * emitter)
{
    delete emitter;
}
