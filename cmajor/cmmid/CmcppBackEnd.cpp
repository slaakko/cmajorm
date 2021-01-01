// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmid/CmcppBackEnd.hpp>
#include <cmajor/cmcppbe/Interface.hpp>

namespace cmajor { namespace mid {

void CmcppBackEnd::Init()
{
}

void CmcppBackEnd::Done()
{
}

cmajor::ir::EmittingContext* CmcppBackEnd::CreateEmittingContext(int optimizationLevel)
{
    return CmcppbeCreateEmittingContext(optimizationLevel);
}

void CmcppBackEnd::DestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext)
{
    CmcppbeDestroyEmittingContext(emittingContext);
}

cmajor::ir::Emitter* CmcppBackEnd::CreateEmitter(cmajor::ir::EmittingContext* emittingContext)
{
    return CmcppbeCreateEmitter(emittingContext);
}

void CmcppBackEnd::DestroyEmitter(cmajor::ir::Emitter* emitter)
{
    CmcppbeDestroyEmitter(emitter);
}

} } // namespace cmajor::mid
