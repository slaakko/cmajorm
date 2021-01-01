// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_UNWIND_INCLUDED
#define CMAJOR_RT_UNWIND_INCLUDED
#include <cmajor/rt/RtApi.hpp>

extern "C" RT_API void* RtPushUnwindInfo(void* unwindInfo);
extern "C" RT_API void RtPopUnwindInfo(void* prevUnwindInfo);
extern "C" RT_API void RtAddCompileUnitFunction(void* functionAddress, const char* functionName, const char* sourceFilePath);
extern "C" RT_API const char* RtGetCallStack();
extern "C" RT_API void RtDisposeCallStack();
void InitUnwind();
void DoneUnwind();

#endif // CMAJOR_RT_UNWIND_INCLUDED
