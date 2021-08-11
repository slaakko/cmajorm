// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_IO_INCLUDED
#define CMAJOR_RT_IO_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdexcept>
#include <stdint.h>

namespace cmajor { namespace rt {

const int stdInFileHandle = 0;
const int stdOutFileHandle = 1;
const int stdErrFileHandle = 2;

enum class OpenMode : uint8_t
{
    none = 0,
    read = 1 << 0, 
    write = 1 << 1,
    append = 1 << 2,
    binary = 1 << 3
};

enum class Origin : uint8_t
{
    seekSet, seekCur, seekEnd
};

inline OpenMode operator&(OpenMode left, OpenMode right)
{
    return OpenMode(uint8_t(left) & uint8_t(right));
}

inline OpenMode operator|(OpenMode left, OpenMode right)
{
    return OpenMode(uint8_t(left) | uint8_t(right));
}

} } // namespace cmajor::rt

extern "C" RT_API void* RtOpen(const char* filePath, cmajor::rt::OpenMode openMode, int32_t& errorStringHandle);
extern "C" RT_API void* RtOpenStdFile(int handle, int32_t& errorStringHandle);
extern "C" RT_API bool RtClose(void* fileHandle, int32_t& errorStringHandle);
extern "C" RT_API bool RtDisposeFile(void* fileHandle, int32_t& errorStringHandle);
extern "C" RT_API int64_t RtWrite(void* fileHandle, const uint8_t* buffer, int64_t count, int32_t& errorStringHandle);
extern "C" RT_API bool RtWriteByte(void* fileHandle, uint8_t x, int32_t& errorStringHandle);
extern "C" RT_API int64_t RtRead(void* fileHandle, uint8_t* buffer, int64_t bufferSize, int32_t& errorStringHandle);
extern "C" RT_API int32_t RtReadByte(void* fileHandle);
extern "C" RT_API bool RtEof(void* fileHandle);
extern "C" RT_API bool RtGetFileError(void* fileHandle, int32_t& errorStringHandle);
extern "C" RT_API bool RtSeek(void* fileHandle, int64_t pos, cmajor::rt::Origin origin, int32_t& errorStringHandle);
extern "C" RT_API int64_t RtTell(void* fileHandle, int32_t& errorStringHandle);
extern "C" RT_API bool RtFlush(void* fileHandle, int32_t& errorStringHandle);
extern "C" RT_API bool RtFileExists(const char* filePath);
extern "C" RT_API bool RtLastWriteTimeLess(const char* filePath1, const char* filePath2);
extern "C" RT_API int64_t RtGetFileSize(const char* filePath, int32_t& errorStringHandle);
extern "C" RT_API bool RtRemoveFile(const char* filePath, int32_t& errorStringHandle);
extern "C" RT_API bool RtCopyFile(const char* sourceFilePath, const char* targetFilePath, int32_t& errorStringHandle);
extern "C" RT_API bool RtMoveFile(const char* sourceFilePath, const char* targetFilePath, int32_t & errorStringHandle);
extern "C" RT_API bool RtIsConsoleHandle(int handle);

extern "C" RT_API void InitIo();
extern "C" RT_API void DoneIo();

#endif // CMAJOR_RT_IO_INCLUDED
