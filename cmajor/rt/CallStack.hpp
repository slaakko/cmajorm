// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_CALL_STACK_INCLUDED
#define CMAJOR_RT_CALL_STACK_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API void RtEnterFunction(const char* functionName, const char* sourceFilePath);
extern "C" RT_API void RtSetLineNumber(int32_t lineNumber);
extern "C" RT_API void RtExitFunction();
extern "C" RT_API void RtPrintCallStack(void* fileHandle);
extern "C" RT_API const char* RtGetStackTrace();
extern "C" RT_API void RtDisposeStackTrace();

#endif // CMAJOR_RT_CALL_STACK_INCLUDED
