// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_TRACE_INCLUDED
#define CMAJOR_RT_TRACE_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API void RtBeginTracing();
extern "C" RT_API void RtEndTracing();
extern "C" RT_API void RtSetThreadId(char threadId);
extern "C" RT_API void RtBeginTraceFunction(int32_t traceFunctionId);
extern "C" RT_API void RtEndTraceFunction(int32_t traceFunctionId);
extern "C" RT_API void RtWindowsMessage(int32_t messageId);

#endif // CMAJOR_RT_TRACE_INCLUDED
