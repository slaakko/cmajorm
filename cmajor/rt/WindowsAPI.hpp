// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_WINDOWS_API_INCLUDED
#define CMAJOR_RT_WINDOWS_API_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

extern "C" RT_API void WinSetInstance();
extern "C" RT_API uint64_t WinGetLastError();
extern "C" RT_API void WinFormatMessage(uint64_t errorCode, char16_t* buffer);
extern "C" RT_API void* WinCreateEvent();
extern "C" RT_API bool WinSetEvent(void* eventHandle);
extern "C" RT_API bool WinResetEvent(void* eventHandle);
extern "C" RT_API bool WinWaitEvent(void* eventHandle);
extern "C" RT_API bool WinCloseHandle(void* handle);
extern "C" RT_API int32_t WinWaitForMultipleObjects(uint32_t count, void** handles);
extern "C" RT_API void* WinFindFirstChangeNotification(const char* directoryPath);
extern "C" RT_API bool WinFindNextChangeNotification(void* handle);
extern "C" RT_API bool WinFindCloseChangeNotification(void* handle);
extern "C" RT_API bool WinShellExecute(const char* filePath, int64_t& errorCode);
extern "C" RT_API int WinRun();
extern "C" RT_API uint16_t WinRegisterWindowClass(const char* windowClassName, uint32_t style, int backgroundColor);
extern "C" RT_API void* WinCreateWindowByClassAtom(uint16_t windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle);
extern "C" RT_API void* WinCreateWindowByClassName(const char* windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle);
extern "C" RT_API void WinShowMessageBox(const char* text, const char* caption);
extern "C" RT_API int WinShowMessageBoxWithType(const char* text, const char* caption, void* ownerWindowHandle, uint32_t type);
extern "C" RT_API void WinSetMessageProcessorFunctionAddress(void* messageProcessorFunctionAddress);
extern "C" RT_API void WinPostQuitMessage(int exitCode);
extern "C" RT_API bool WinShowWindow(void* windowHandle, int commandShow);
extern "C" RT_API bool WinUpdateWindow(void* windowHandle);
extern "C" RT_API bool WinMoveWindow(void* windowHandle, int x, int y, int w, int h, bool repaint);
extern "C" RT_API bool WinGetClientRect(void* windowHandle, void* rect);
extern "C" RT_API bool WinGetWindowRect(void* windowHandle, void* rect);
extern "C" RT_API bool WinClientToScreen(void* windowHandle, void* point);
extern "C" RT_API bool WinScreenToClient(void* windowHandle, void* point);
extern "C" RT_API bool WinPtInRect(void* rect, void* point);
extern "C" RT_API bool WinSetWindowText(void* windowHandle, const char* text);
extern "C" RT_API void* WinSetParent(void* childWindowHandle, void* parentWindowHandle);

#endif // CMAJOR_RT_WINDOWS_API_INCLUDED

