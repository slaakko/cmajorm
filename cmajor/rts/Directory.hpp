// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_DIRECTORY_INCLUDED
#define CMAJOR_RTS_DIRECTORY_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

namespace cmajor { namespace rt {

void InitDirectory();
void DoneDirectory();

} } // namespace cmajor::rt

extern "C" RT_API bool RtDirectoryExists(const char* directoryPath);
extern "C" RT_API void RtCreateDirectories(const char* directoryPath);
extern "C" RT_API int32_t RtBeginIterateDirectory(const char* directoryPath);
extern "C" RT_API const char* RtGetNextFilePath(int32_t directoryIterationHandle);
extern "C" RT_API void RtEndIterateDirectory(int32_t directoryIterationHandle);

#endif // CMAJOR_RTS_DIRECTORY_INCLUDED
