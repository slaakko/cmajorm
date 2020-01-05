// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_WINDOWS_API_INCLUDED
#define CMAJOR_RT_WINDOWS_API_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API uint64_t WinGetLastError();
extern "C" RT_API void WinFormatMessage(uint64_t errorCode, char* buffer);
extern "C" RT_API void* WinCreateEvent();
extern "C" RT_API bool WinSetEvent(void* eventHandle);
extern "C" RT_API bool WinResetEvent(void* eventHandle);
extern "C" RT_API bool WinWaitEvent(void* eventHandle);
extern "C" RT_API bool WinCloseHandle(void* handle);
extern "C" RT_API int32_t WinWaitForMultipleObjects(uint32_t count, void** handles);
extern "C" RT_API void* WinFindFirstChangeNotification(const char* directoryPath);
extern "C" RT_API bool WinFindNextChangeNotification(void* handle);
extern "C" RT_API bool WinFindCloseChangeNotification(void* handle);

#endif // CMAJOR_RT_WINDOWS_API_INCLUDED

