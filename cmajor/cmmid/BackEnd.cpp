// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmid/BackEnd.hpp>
#include <cmajor/cmmid/LlvmBackEnd.hpp>
#include <cmajor/cmmid/CmsxBackEnd.hpp>

namespace cmajor { namespace mid {

LLvmBackEnd llvmBackEnd;
CmsxBackEnd cmsxBackEnd;

BackEnd* backEnd = &llvmBackEnd;

BackEnd::~BackEnd()
{
}

void SetBackEnd(BackEndKind backEndKind)
{
    switch (backEndKind)
    {
        case BackEndKind::llvmBackEnd: backEnd = &llvmBackEnd; break;
        case BackEndKind::cmsxBackEnd: backEnd = &cmsxBackEnd; break;
    }
}

BackEndKind GetBackEnd()
{
    return backEnd->GetBackEndKind();
}

} } // namespace cmajor::mid
