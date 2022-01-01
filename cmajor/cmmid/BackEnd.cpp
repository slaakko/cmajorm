// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmid/BackEnd.hpp>
#include <cmajor/cmmid/LlvmBackEnd.hpp>
#ifdef _WIN32
#include <cmajor/cmmid/CmsxBackEnd.hpp>
#endif
#include <cmajor/cmmid/CmcppBackEnd.hpp>

namespace cmajor { namespace mid {

LLvmBackEnd llvmBackEnd;
#ifdef _WIN32
CmsxBackEnd cmsxBackEnd;
#endif
CmcppBackEnd cmcppBackEnd;

BackEnd* backEnd = &llvmBackEnd;

BackEnd::~BackEnd()
{
}

void SetBackEnd(BackEndKind backEndKind)
{
    switch (backEndKind)
    {
        case BackEndKind::llvmBackEnd: backEnd = &llvmBackEnd; break;
#ifdef _WIN32
        case BackEndKind::cmsxBackEnd: backEnd = &cmsxBackEnd; break;
#endif
        case BackEndKind::cmcppBackEnd: backEnd = &cmcppBackEnd; break;
    }
}

BackEndKind GetBackEnd()
{
    return backEnd->GetBackEndKind();
}

} } // namespace cmajor::mid
