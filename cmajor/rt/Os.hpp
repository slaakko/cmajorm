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
extern "C" RT_API bool OsGetConsoleScreenBufferInfo(void* consoleOutputHandle, int32_t* cursorPosX, int32_t* cursorPosY, int32_t* screenSizeX, int32_t* screenSizeY);
extern "C" RT_API bool OsSetConsoleCursorPosition(void* consoleOutputHandle, int cursorPosX, int cursorPosY);
extern "C" RT_API bool OsWriteConsole(void* consoleOutputHandle, const char32_t* chars);
extern "C" RT_API void* OsCreateHostFile(const char* filePath);
extern "C" RT_API void* OsOpenHostFile(const char* filePath);
extern "C" RT_API void OsCloseHostFile(void* fileHandle);
extern "C" RT_API void* OsCreateIoCompletionPort(void* fileHandle, uint64_t completionKey);
extern "C" RT_API void OsCloseIoCompletionPort(void* completionPortHandle);
extern "C" RT_API bool OsGetQueuedCompletionStatus(void* completionPortHandle, uint64_t* numberOfBytes, uint64_t* completionKey, void** overlapped);
extern "C" RT_API bool OsPostQueuedCompletionStatus(void* completionPortHandle, uint64_t numberOfBytes, uint64_t completionKey);
extern "C" RT_API void* OsCreateOverlapped(uint64_t offset, void* evnt);
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

#endif // CMAJOR_RT_OS_INCLUDED
