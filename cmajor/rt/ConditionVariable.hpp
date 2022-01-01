// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_CONDITION_VARIABLE_INCLUDED
#define CMAJOR_RT_CONDITION_VARIABLE_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API void* RtAllocateConditionVariable();
extern "C" RT_API void RtFreeConditionVariable(void* nativeHandle);
extern "C" RT_API int32_t RtNotifyOne(void* nativeHandle);
extern "C" RT_API int32_t RtNotifyAll(void* nativeHandle);
extern "C" RT_API int32_t RtWaitConditionVariable(void* nativeHandle, void* recursiveMutexHandle);
extern "C" RT_API int32_t RtWaitConditionVariableDuration(void* nativeHandle, void* recursiveMutexHandle, int64_t nanoseconds);

#endif // CMAJOR_RT_CONDITION_VARIABLE_INCLUDED
