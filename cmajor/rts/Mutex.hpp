// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_MUTEX_INCLUDED
#define CMAJOR_RTS_MUTEX_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API void* RtAllocateMutex();
extern "C" RT_API void RtFreeMutex(void* nativeHandle);
extern "C" RT_API void RtLockMutex(void* nativeHandle);
extern "C" RT_API void RtUnlockMutex(void* nativeHandle);

extern "C" RT_API void* RtAllocateRecursiveMutex();
extern "C" RT_API void RtFreeRecursiveMutex(void* nativeHandle);
extern "C" RT_API void RtLockRecursiveMutex(void* nativeHandle);
extern "C" RT_API void RtUnlockRecursiveMutex(void* nativeHandle);

#endif // CMAJOR_RTS_MUTEX_INCLUDED
