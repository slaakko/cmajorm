// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_UNWIND_INCLUDED
#define CMAJOR_RTS_UNWIND_INCLUDED
#include <cmajor/rts/RtsApi.hpp>

extern "C" RT_API void* RtPushUnwindInfo(void* unwindInfo);
extern "C" RT_API void RtPopUnwindInfo(void* prevUnwindInfo);
extern "C" RT_API void RtAddCompileUnitFunction(void* functionAddress, const char* functionName, const char* sourceFilePath);
extern "C" RT_API const char* RtGetCallStack();
extern "C" RT_API void RtDisposeCallStack();
void InitUnwind();
void DoneUnwind();

#endif // CMAJOR_RTS_UNWIND_INCLUDED
