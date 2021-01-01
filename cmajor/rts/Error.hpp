// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_ERROR_INCLUDED
#define CMAJOR_RTS_ERROR_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <cmajor/eh/Exception.hpp>
#include <string>
#include <stdint.h>

const int exitCodeAccessViolation = 255;
const int exitCodeInternalError = 254;
const int exitCodeAssertionFailed = 253;
const int exitCodeOutOfMemory = 252;
const int exitCodeExceptionEscapedFromThread = 251;
const int exitCodeSocketInitializationFailed = 250;

using AssertionFailureFunction = void(*)(const char* assertion, const char* function, const char* sourceFilePath, int lineNumber);

extern "C" RT_API bool RtIsUserAssertionFailureFunctionSet();
extern "C" RT_API void RtSetUserAssertionFailureFunction(AssertionFailureFunction userAssertionFailureFunc);
extern "C" RT_API void RtFailAssertion(const char* assertion, const char* function, const char* sourceFilePath, int lineNumber);
extern "C" RT_API const char* RtGetError(int32_t errorId);
extern "C" RT_API void RtDisposeError(int32_t errorId);
extern "C" RT_API void RtThrowException(void* exception, void* exceptionTypeId);
extern "C" RT_API void RtCaptureException(void** exception, uint64_t & exceptionClassIdHi, uint64_t & exceptionClassIdLo);
extern "C" RT_API void RtThrowCapturedException(void* exception, uint64_t exceptionClassIdHi, uint64_t exceptionClassIdLo);
extern "C" RT_API bool RtHandleException(void* exceptionTypeId);
extern "C" RT_API void* RtGetException();
extern "C" RT_API void* RtGetExceptionTypeId();

namespace cmajor { namespace rt {

int32_t InstallError(const std::string& errorMessage);
const char* GetError(int32_t errorId);
void DisposeError(int32_t errorId);
void InitError();
void DoneError();

} } // namespace cmajor::rt

#endif // CMAJOR_RTS_ERROR_INCLUDED
