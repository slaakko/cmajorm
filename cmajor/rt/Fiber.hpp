// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_FIBER_INCLUDED
#define CMAJOR_RT_FIBER_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API void* RtConvertThreadToFiber(void* param);
extern "C" RT_API void* RtCreateFiber(uint64_t stackSize, void* startAddress, void* param);
extern "C" RT_API void RtSwitchToFiber(void* fiber);
extern "C" RT_API void* RtGetFiberData();
extern "C" RT_API void RtDeleteFiber(void* fiber);

#endif // CMAJOR_RT_FIBER_INCLUDED
