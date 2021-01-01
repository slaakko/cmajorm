// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmmid/InitDone.hpp>
#include <cmajor/cmmid/BackEnd.hpp>

extern "C" CMM_API void CmmInit(cmajor::mid::BackEndKind backEndKind)
{
    cmajor::mid::SetBackEnd(backEndKind);
    cmajor::mid::backEnd->Init();
}

extern "C" CMM_API void CmmDone()
{
    cmajor::mid::backEnd->Done();
}
