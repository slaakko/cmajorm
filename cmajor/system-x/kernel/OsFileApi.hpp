// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_OS_FILE_API_INCLUDED
#define CMSX_KERNEL_OS_FILE_API_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <string>
#include <stdint.h>

namespace cmsx::kernel {

CMSX_KERNEL_API void* OsCreateHostFile(const char* filePath, bool randomAccess);
CMSX_KERNEL_API void* OsOpenHostFile(const char* filePath, bool randomAccess);
CMSX_KERNEL_API void OsCloseHostFile(void* fileHandle);
CMSX_KERNEL_API void* OsCreateIoCompletionPort();
CMSX_KERNEL_API void* OsAssociateFileWithCompletionPort(void* fileHandle, void* completionPort, uint64_t completionKey);
CMSX_KERNEL_API void OsCloseIoCompletionPort(void* completionPortHandle);
CMSX_KERNEL_API bool OsGetQueuedCompletionStatus(void* completionPortHandle, uint64_t * numberOfBytes, uint64_t * completionKey, void** overlapped);
CMSX_KERNEL_API bool OsPostQueuedCompletionStatus(void* completionPortHandle, uint64_t numberOfBytes, uint64_t completionKey);
CMSX_KERNEL_API void* OsCreateOverlapped(uint64_t offset);
CMSX_KERNEL_API void OsDestroyOverlapped(void* overlapped);
CMSX_KERNEL_API bool OsReadFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToRead, void* overlapped);
CMSX_KERNEL_API bool OsWriteFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToWrite, void* overlapped);
CMSX_KERNEL_API std::string OsGetLogicalDrives();
CMSX_KERNEL_API uint32_t OsGetDriveType(const char* rootPathName);
CMSX_KERNEL_API bool OsFindFirstFile(const std::string& directoryName, std::string& entry, void*& searchHandle);
CMSX_KERNEL_API bool OsFindNextFile(void* searchHandle, std::string& entry);
CMSX_KERNEL_API int OsGetConsoleNumberOfColumns();
CMSX_KERNEL_API int OsGetConsoleNumberOfRows();
extern const uint32_t fixedDriveType;

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_OS_FILE_API_INCLUDED
