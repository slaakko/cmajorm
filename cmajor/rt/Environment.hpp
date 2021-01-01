// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_ENVIRONMENT_INCLUDED
#define CMAJOR_RT_ENVIRONMENT_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

namespace cmajor { namespace rt {

void InitEnvironment();
void DoneEnvironment();

} } // namespace cmajor::rt

extern "C" RT_API const char* RtGetEnvironmentVariable(const char* environmentVariableName);
extern "C" RT_API int32_t RtGetCurrentWorkingDirectoryHandle();
extern "C" RT_API const char* RtGetCurrentWorkingDirectory(int32_t currentWorkingDirectoryHandle);
extern "C" RT_API void RtFreeCurrentWorkingDirectoryHandle(int32_t currentWorkingDirectoryHandle);
extern "C" RT_API int32_t RtExecute(const char* command);
extern "C" RT_API int32_t RtBeginExec(const char* command);
extern "C" RT_API void RtAddRedirection(int32_t execHandle, int handle, const char* file);
extern "C" RT_API int32_t RtEndExec(int32_t execHandle);
extern "C" RT_API int32_t RtBeginGetPathToExecutable();
extern "C" RT_API const char* RtGetPathToExecutable(int32_t pathHandle);
extern "C" RT_API void RtEndGetPathToExecutable(int32_t pathHandle);
extern "C" RT_API const char* RtGetOsInfo();

#endif // CMAJOR_RT_ENVIRONMENT_INCLUDED
