// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_PROCESS_INCLUDED
#define CMAJOR_RT_PROCESS_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API void* RtCreateProcess(const char* command, int32_t & errorStringHandle);
extern "C" RT_API void RtDeleteProcess(void* process);
extern "C" RT_API const char* RtGetString(int32_t stringHandle);
extern "C" RT_API void RtDisposeString(int32_t stringHandle);
extern "C" RT_API bool RtProcessRunning(void* process);
extern "C" RT_API void RtProcessWaitForExit(void* process);
extern "C" RT_API int RtProcessExitCode(void* process);
extern "C" RT_API bool RtProcessEof(void* process, int handle);
extern "C" RT_API int32_t RtProcessReadLine(void* process, int handle);
extern "C" RT_API int32_t RtProcessReadToEnd(void* process, int handle);
extern "C" RT_API void RtProcessWriteLine(void* process, const char* line);

#endif // CMAJOR_RT_PROCESS_INCLUDED

