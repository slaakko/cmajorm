// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_THREAD_INCLUDED
#define CMAJOR_RT_THREAD_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API int32_t RtGetHardwareConcurrency();
extern "C" RT_API int32_t RtStartThreadFunction(void* function);
extern "C" RT_API int32_t RtStartThreadFunctionWithParam(void* function, void* param);
extern "C" RT_API int32_t RtStartThreadMethod(void* classDelegate);
extern "C" RT_API int32_t RtStartThreadMethodWithParam(void* classDelegate, void* param);
extern "C" RT_API bool RtJoinThread(int32_t threadId);
extern "C" RT_API int32_t RtThisThreadId();

namespace cmajor { namespace rt {

void InitThread();
void DoneThread();

} } // namespace cmajor::rt

#endif // CMAJOR_RT_THREAD_INCLUDED
