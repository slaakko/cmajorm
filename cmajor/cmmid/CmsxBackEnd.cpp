// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmid/CmsxBackEnd.hpp>
#include <cmajor/cmsxbe/Interface.hpp>

namespace cmajor { namespace mid {

void CmsxBackEnd::Init()
{
}

void CmsxBackEnd::Done()
{
}

cmajor::ir::EmittingContext* CmsxBackEnd::CreateEmittingContext(int optimizationLevel)
{
    return CmsxbeCreateEmittingContext(optimizationLevel);
}

void CmsxBackEnd::DestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext)
{
    CmsxbeDestroyEmittingContext(emittingContext);
}

cmajor::ir::Emitter* CmsxBackEnd::CreateEmitter(cmajor::ir::EmittingContext* emittingContext)
{
    return CmsxbeCreateEmitter(emittingContext);
}

void CmsxBackEnd::DestroyEmitter(cmajor::ir::Emitter* emitter)
{
    CmsxbeDestroyEmitter(emitter);
}

} } // namespace cmajor::mid
