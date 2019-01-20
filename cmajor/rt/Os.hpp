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

#endif // CMAJOR_RT_OS_INCLUDED
