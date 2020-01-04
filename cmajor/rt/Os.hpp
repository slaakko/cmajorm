// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_OS_INCLUDED
#define CMAJOR_RT_OS_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API uint64_t OsAllocateMemoryPage(uint64_t pageSize);
extern "C" RT_API bool OsFreeMemoryPage(uint64_t pageAddress);
extern "C" RT_API void OsCopyMemoryPage(uint64_t fromPageAddress, uint64_t toPageAddress, uint64_t pageSize);
extern "C" RT_API void OsWriteMemoryByte(uint64_t address, uint8_t b);
extern "C" RT_API uint8_t OsReadMemoryByte(uint64_t address);
extern "C" RT_API void* OsGetStdHandle(int32_t stdInOutOrErr);
extern "C" RT_API bool OsRegisterConsoleCallback(void* consoleInputHandle, uint64_t callBackValue);
extern "C" RT_API bool OsReadConsoleInput(void* consoleInputHandle, char32_t* c);
extern "C" RT_API bool OsGetConsoleScreenBufferInfo(void* consoleOutputHandle, int32_t* cursorPosX, int32_t* cursorPosY, int32_t* screenSizeX, int32_t* screenSizeY, uint16_t* attrs);
extern "C" RT_API bool OsSetConsoleCursorPosition(void* consoleOutputHandle, int cursorPosX, int cursorPosY);
extern "C" RT_API bool OsWriteConsole(void* consoleOutputHandle, const char32_t* chars);
extern "C" RT_API bool OsSetConsoleTextAttribute(uint16_t attrs);
extern "C" RT_API void* OsCreateHostFile(const char* filePath, bool randomAccess);
extern "C" RT_API void* OsOpenHostFile(const char* filePath, bool randomAccess);
extern "C" RT_API void OsCloseHostFile(void* fileHandle);
extern "C" RT_API void* OsCreateIoCompletionPort();
extern "C" RT_API void* OsAssociateFileWithCompletionPort(void* fileHandle, void* completionPort, uint64_t completionKey);
extern "C" RT_API void OsCloseIoCompletionPort(void* completionPortHandle);
extern "C" RT_API bool OsGetQueuedCompletionStatus(void* completionPortHandle, uint64_t* numberOfBytes, uint64_t* completionKey, void** overlapped);
extern "C" RT_API bool OsPostQueuedCompletionStatus(void* completionPortHandle, uint64_t numberOfBytes, uint64_t completionKey);
extern "C" RT_API void* OsCreateOverlapped(uint64_t offset);
extern "C" RT_API void OsDestroyOverlapped(void* overlapped);
extern "C" RT_API bool OsReadFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToRead, void* overlapped);
extern "C" RT_API bool OsWriteFile(void* fileHandle, void* buffer, uint32_t numberOfBytesToWrite, void* overlapped);
extern "C" RT_API void* OsCreateEvent();
extern "C" RT_API bool OsSetEvent(void* eventHandle);
extern "C" RT_API bool OsResetEvent(void* eventHandle);
extern "C" RT_API void OsWaitEvent(void* eventHandle);
extern "C" RT_API void OsCloseEvent(void* eventHandle);
extern "C" RT_API int32_t OsWaitForMultipleObjects(uint32_t count, void** handles);
extern "C" RT_API void* OsConvertThreadToFiber(void* param);
extern "C" RT_API void* OsCreateFiber(uint64_t stackSize, void* startAddress, void* param);
extern "C" RT_API void OsSwitchToFiber(void* fiber);
extern "C" RT_API void* OsGetFiberData();
extern "C" RT_API void OsDeleteFiber(void* fiber);
extern "C" RT_API uint64_t OsGetLastError();
extern "C" RT_API void OsFormatMessage(uint64_t errorCode, char* buffer);
extern "C" RT_API bool OsGetLogicalDrives(char* buffer, int bufSize);
extern "C" RT_API uint32_t OsGetDriveType(const char* rootPathName);
extern "C" RT_API int64_t OsGetFileSize(void* fileHandle);
extern "C" RT_API uint32_t OsGetFileAttributes(const char* filePath);
extern "C" RT_API void* OsFindFirstFile(const char* pathMask, char* fileName);
extern "C" RT_API bool OsFindNextFile(void* findHandle, char* fileName);
extern "C" RT_API void OsFindClose(void* findHandle);
extern "C" RT_API bool OsGetFileTimes(const char* filePath, uint8_t* ctime, uint8_t* mtime, uint8_t* atime);
extern "C" RT_API int OsGetMaxComputerNameLength();
extern "C" RT_API bool OsGetComputerName(char* buffer, int size);
extern "C" RT_API void* OsFindFirstChangeNotification(const char* directoryPath);
extern "C" RT_API bool OsFindNextChangeNotification(void* handle);
extern "C" RT_API void OsFindCloseChangeNotification(void* handle);

#endif // CMAJOR_RT_OS_INCLUDED
