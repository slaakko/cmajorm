// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_MUTEX_INCLUDED
#define CMAJOR_RT_MUTEX_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>
#include <mutex>

extern "C" RT_API int32_t RtAllocateMutex();
extern "C" RT_API void RtFreeMutex(int32_t mutexId);
extern "C" RT_API void RtLockMutex(int32_t mutexId);
extern "C" RT_API void RtUnlockMutex(int32_t mutexId);

extern "C" RT_API int32_t RtAllocateRecursiveMutex();
extern "C" RT_API void RtFreeRecursiveMutex(int32_t mutexId);
extern "C" RT_API void RtLockRecursiveMutex(int32_t mutexId);
extern "C" RT_API void RtUnlockRecursiveMutex(int32_t mutexId);

namespace cmajor { namespace rt { 

std::recursive_mutex& GetRecursiveMutex(int32_t mutexId);
void InitMutex();
void DoneMutex();

} } // namespace cmajor::rt

#endif // CMAJOR_RT_MUTEX_INCLUDED
