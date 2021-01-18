// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_IO_INCLUDED
#define CMAJOR_RT_IO_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdexcept>
#include <stdint.h>

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

extern "C" RT_API int32_t RtOpen(const char* filePath, OpenMode openMode);
extern "C" RT_API int32_t RtClose(int32_t fileHandle);
extern "C" RT_API int32_t RtWrite(int32_t fileHandle, const uint8_t* buffer, int64_t count);
extern "C" RT_API int32_t RtWriteByte(int32_t fileHandle, uint8_t x);
extern "C" RT_API int64_t RtRead(int32_t fileHandle, uint8_t* buffer, int64_t bufferSize);
extern "C" RT_API int32_t RtReadByte(int32_t fileHandle);
extern "C" RT_API int32_t RtSeek(int32_t fileHandle, int64_t pos,  Origin origin);
extern "C" RT_API int64_t RtTell(int32_t fileHandle);
extern "C" RT_API bool RtFileExists(const char* filePath);
extern "C" RT_API bool RtLastWriteTimeLess(const char* filePath1, const char* filePath2);
extern "C" RT_API int32_t RtGetFileSize(const char* filePath, uint64_t* fileSize);
extern "C" RT_API int32_t RtRemoveFile(const char* filePath);
extern "C" RT_API int32_t RtRenameFile(const char* sourceFilePath, const char* targetFilePath);
extern "C" RT_API int32_t RtCopyFile(const char* sourceFilePath, const char* targetFilePath);
extern "C" RT_API void RtFlushAll();
extern "C" RT_API bool RtIsConsoleHandle(int handle);
extern bool fileTableInitialized;

namespace cmajor { namespace rt {

class FileSystemError : public std::runtime_error
{
public:
    FileSystemError(const std::string& message_);
};

void InitIo();
void DoneIo();

} }  // namespace cmajor::rt

#endif // CMAJOR_RT_IO_INCLUDED
