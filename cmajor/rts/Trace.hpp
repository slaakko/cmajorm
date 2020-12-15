// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_TRACE_INCLUDED
#define CMAJOR_RTS_TRACE_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API void RtBeginTracing();
extern "C" RT_API void RtEndTracing();
extern "C" RT_API void RtSetThreadId(char threadId);
extern "C" RT_API void RtBeginTraceFunction(int32_t traceFunctionId);
extern "C" RT_API void RtEndTraceFunction(int32_t traceFunctionId);

#endif // CMAJOR_RTS_TRACE_INCLUDED

