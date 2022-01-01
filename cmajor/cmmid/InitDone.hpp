// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MID_INIT_DONE_INCLUDED
#define CMAJOR_MID_INIT_DONE_INCLUDED
#include <cmajor/cmmid/CmmApi.hpp>
#include <cmajor/cmmid/BackEnd.hpp>
#include <stdint.h>

extern "C" CMM_API void CmmInit(cmajor::mid::BackEndKind backEndKind);
extern "C" CMM_API void CmmDone();

#endif // CMAJOR_RT_INIT_DONE_INCLUDED

