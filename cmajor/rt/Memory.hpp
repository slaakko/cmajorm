// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_MEMORY_INCLUDED
#define CMAJOR_RT_MEMORY_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API void* RtMemAlloc(int64_t size);
extern "C" RT_API void* RtMemAllocInfo(int64_t size, const char* info);
extern "C" RT_API void RtDispose(void* ptr);
extern "C" RT_API void RtMemFree(void* ptr);
extern "C" RT_API void RtMemZero(void* ptr, int64_t size);

namespace cmajor { namespace rt {

void SetDebugHeap();
void SetDebugAllocation(int allocation);

void InitMemory();
void DoneMemory();

} }  // namespace cmajor::rt

#endif // CMAJOR_RT_MEMORY_INCLUDED
