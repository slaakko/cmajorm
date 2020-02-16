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
extern "C" RT_API int WinInit(void* messageProcessorFunctionAddress);
extern "C" RT_API int WinRun();
extern "C" RT_API void WinDone();
extern "C" RT_API uint16_t WinRegisterWindowClass(const char* windowClassName, uint32_t style, int backgroundColor);
extern "C" RT_API void* WinCreateWindowByClassAtom(uint16_t windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle);
extern "C" RT_API void* WinCreateWindowByClassName(const char* windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle);
extern "C" RT_API int WinShowMessageBox(const char* text, const char* caption);
extern "C" RT_API int WinShowMessageBoxWithType(const char* text, const char* caption, void* ownerWindowHandle, uint32_t type);
extern "C" RT_API bool WinMessageBeep(uint32_t messageBeepType);
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
extern "C" RT_API void* WinGetDC(void* windowHandle);
extern "C" RT_API bool WinTrackMouseEvent(void* windowHandle, uint32_t flags, uint32_t hoverTimeMs);
extern "C" RT_API void* WinSetParent(void* childWindowHandle, void* parentWindowHandle);
extern "C" RT_API bool WinInvalidateRect(void* windowHandle, void* rect, bool eraseBackground);
extern "C" RT_API void* WinBeginPaint(void* windowHandle, void*& paintStruct);
extern "C" RT_API void WinEndPaint(void* windowHandle, void* paintStruct);
extern "C" RT_API void WinGetClipRect(void* paintStruct, int& x, int& y, int& w, int& h);
extern "C" RT_API void* WinCreateGraphics(void* hdc);
extern "C" RT_API void WinDeleteGraphics(void* nativeGraphics);
extern "C" RT_API void* WinCreateGraphicsFromWindowHandle(void* hwnd);
extern "C" RT_API int WinGraphicsGetLastStatus(void* grapchis);
extern "C" RT_API void* WinGraphicsGetHDC(void* nativeGraphics);
extern "C" RT_API void* WinGraphicsCreatePen(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue, float width);
extern "C" RT_API void* WinGraphicsClonePen(void* pen);
extern "C" RT_API void WinGraphicsDeletePen(void* pen);
extern "C" RT_API int WinGraphicsPenGetLastStatus(void* pen);
extern "C" RT_API void* WinGraphicsCreateSolidBrush(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);
extern "C" RT_API void* WinGraphicsCloneSolidBrush(void* solidBrush);
extern "C" RT_API void WinGraphicsDeleteSolidBrush(void* solidBrush);
extern "C" RT_API int WinGraphicsBrushGetLastStatus(void* brush);
extern "C" RT_API void* WinGraphicsCreateFontFamily(const char* familyName);
extern "C" RT_API void* WinGraphicsCloneFontFamily(void* fontFamily);
extern "C" RT_API void WinGraphicsDeleteFontFamily(void* fontFamily);
extern "C" RT_API const void* WinGraphicsGetGenericMonospaceFontFamily();
extern "C" RT_API const void* WinGraphicsGetGenericSansSerifFontFamily();
extern "C" RT_API const void* WinGraphicsGetGenericSerifFontFamily();
extern "C" RT_API int WinGraphicsFontFamilyGetLastStatus(void* fontFamily);
extern "C" RT_API void* WinGraphicsCreateFont(const void* fontFamily, float emSize, int style, int unit);
extern "C" RT_API void* WinGraphicsCloneFont(void* font);
extern "C" RT_API void WinGraphicsDeleteFont(void* font);
extern "C" RT_API int WinGraphicsFontGetLastStatus(void* font);
extern "C" RT_API float WinGraphicsGetFontHeight(void* font, const void* graphics);
extern "C" RT_API void* WinGraphicsCreateDefaultStringFormat();
extern "C" RT_API void* WinGraphicsCreateStringFormat(int formatFlags, uint16_t languageId);
extern "C" RT_API void* WinGraphicsCloneStringFormat(void* stringFormat);
extern "C" RT_API void WinGraphicsDeleteStringFormat(void* stringFormat);
extern "C" RT_API int WinGraphicsStringFormatGetLastStatus(void* stringFormat);
extern "C" RT_API int WinGraphicsStringFormatSetAlignment(void* stringFormat, int alignment);
extern "C" RT_API int WinGraphicsStringFormatSetLineAlignment(void* stringFormat, int alignment);
extern "C" RT_API int WinGraphicsStringFormatSetHotKeyPrefix(void* stringFormat, int hotKeyPrefix);
extern "C" RT_API int WinGraphicsDrawLine(void* graphics, void* pen, int startX, int startY, int endX, int endY);
extern "C" RT_API int WinGraphicsDrawLines(void* graphics, void* pen, int count, void* points);
extern "C" RT_API int WinGraphicsDrawString(void* graphics, const char* str, void* font, float x, float y, void* brush);
extern "C" RT_API int WinGraphicsDrawStringFormatPoint(void* graphics, const char* str, void* font, float x, float y, void* format, void* brush);
extern "C" RT_API int WinGraphicsDrawStringFormatRect(void* graphics, const char* str, void* font, float x, float y, float w, float h, void* format, void* brush);
extern "C" RT_API int WinGraphicsMeasureStringFormatSize(void* graphics, const char* str, void* font, float w, float h, void* format,
    float& outSizeW, float& outSizeH, int* codePointsFitted, int* linesFilled);
extern "C" RT_API int WinGraphicsMeasureStringFormatRect(void* graphics, const char* str, void* font, float x, float y, float w, float h, void* format,
    float& outX, float& outY, float& outW, float& outH, int* codePointsFitted, int* linesFilled);
extern "C" RT_API int WinGraphicsMeasureStringFormatPoint(void* graphics, const char* str, void* font, float x, float y, void* format,
    float& outX, float& outY, float& outW, float& outH);
extern "C" RT_API int WinGraphicsClear(void* graphics, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);
extern "C" RT_API int WinGraphicsDrawRectangle(void* graphics, void* pen, int x, int y, int w, int h);
extern "C" RT_API int WinGraphicsDrawRectangleF(void* graphics, void* pen, float x, float y, float w, float h);
extern "C" RT_API int WinGraphicsFillRectangle(void* graphics, void* brush, int x, int y, int w, int h);
extern "C" RT_API int WinGraphicsFillRectangleF(void* graphics, void* brush, float x, float y, float w, float h);
extern "C" RT_API int WinGraphicsFillPolygon(void* graphics, void* brush, int count, void* points);
extern "C" RT_API void* WinGraphicsCreateRegion();
extern "C" RT_API void* WinGraphicsCloneRegion(void* region);
extern "C" RT_API void WinGraphicsDeleteRegion(void* region);
extern "C" RT_API int WinGraphicsRegionGetLastStatus(void* region);
extern "C" RT_API int WinGraphicsGetClip(void* graphics, void* region);
extern "C" RT_API int WinGraphicsSetClipRect(void* graphics, int x, int y, int w, int h, int combineMode);
extern "C" RT_API int WinGraphicsSetClipRegion(void* graphics, void* region);
extern "C" RT_API void WinGetSysColor(int index, uint8_t& red, uint8_t& green, uint8_t& blue);
extern "C" RT_API bool WinSetBkColor(void* hdc, uint8_t red, uint8_t green, uint8_t blue);
extern "C" RT_API void WinGetMessagePos(int32_t& x, int32_t& y);
extern "C" RT_API void* WinGetFocus();
extern "C" RT_API void* WinSetFocus(void* windowHandle);

#endif // CMAJOR_RT_WINDOWS_API_INCLUDED
