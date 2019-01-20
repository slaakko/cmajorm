// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_CONDITION_VARIABLE_INCLUDED
#define CMAJOR_RT_CONDITION_VARIABLE_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

namespace cmajor { namespace rt {

void InitConditionVariable();
void DoneConditionVariable();

} } // namespace cmajor::rt

extern "C" RT_API int32_t RtAllocateConditionVariable();
extern "C" RT_API int32_t RtFreeConditionVariable(int32_t conditionVariableId);
extern "C" RT_API int32_t RtNotifyOne(int32_t conditionVariableId);
extern "C" RT_API int32_t RtNotifyAll(int32_t conditionVariableId);
extern "C" RT_API int32_t RtWaitConditionVariable(int32_t conditionVariableId, void* recursiveMutex);
extern "C" RT_API int32_t RtWaitConditionVariableDuration(int32_t conditionVariableId, void* recursiveMutex, int64_t nanoseconds);

#endif // CMAJOR_RT_CONDITION_VARIABLE_INCLUDED
