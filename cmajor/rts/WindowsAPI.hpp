// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_WINDOWS_API_INCLUDED
#define CMAJOR_RTS_WINDOWS_API_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
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
extern "C" RT_API int WinInit(void* messageProcessorFunctionAddress, void* keyPreviewFunctionAddress);
extern "C" RT_API int WinRun();
extern "C" RT_API void WinDone();
extern "C" RT_API void* WinSubClassCommandWndProc(void* windowHandle);
extern "C" RT_API void WinRestoreOriginalWndProc(void* windowHandle, void* originalWndProc);
extern "C" RT_API void* WinGetForegroundWindow();
extern "C" RT_API bool WinEnableWindow(void* windowHandle, bool enable);
extern "C" RT_API int WinMessageLoop(void* windowHandle, void* parentWindowHandle, void* getDialogResultFunc, void* keyPreviewFunc, void* dialogWindowPtr);
extern "C" RT_API uint16_t WinRegisterWindowClass(const char* windowClassName, uint32_t style, int backgroundColor);
extern "C" RT_API void* WinCreateWindowByClassAtom(uint16_t windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle);
extern "C" RT_API void* WinCreateWindowByClassName(const char* windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle);
extern "C" RT_API int WinShowMessageBox(const char* text, const char* caption);
extern "C" RT_API int WinShowMessageBoxWithType(const char* text, const char* caption, void* ownerWindowHandle, uint32_t type);
extern "C" RT_API bool WinMessageBeep(uint32_t messageBeepType);
extern "C" RT_API void WinPostQuitMessage(int exitCode);
extern "C" RT_API bool WinPostMessage(void* windowHandle, uint32_t msg, uint32_t wparam, int64_t lparam);
extern "C" RT_API int64_t WinSendMessage(void* windowHandle, uint32_t msg, uint32_t wparam, int64_t lparam);
extern "C" RT_API bool WinShowWindow(void* windowHandle, int commandShow);
extern "C" RT_API bool WinUpdateWindow(void* windowHandle);
extern "C" RT_API bool WinMoveWindow(void* windowHandle, int x, int y, int w, int h, bool repaint);
extern "C" RT_API bool WinGetClientRect(void* windowHandle, void* rect);
extern "C" RT_API bool WinGetWindowRect(void* windowHandle, void* rect);
extern "C" RT_API bool WinClientToScreen(void* windowHandle, void* point);
extern "C" RT_API bool WinScreenToClient(void* windowHandle, void* point);
extern "C" RT_API bool WinPtInRect(void* rect, void* point);
extern "C" RT_API bool WinSetWindowText(void* windowHandle, const char* text);
extern "C" RT_API int WinGetWindowTextLength(void* windowHandle);
extern "C" RT_API int WinGetWindowText(void* windowHandle, char16_t* textBuffer, int maxCount);
extern "C" RT_API void* WinGetDC(void* windowHandle);
extern "C" RT_API int WinReleaseHDC(void* windowHandle, void* hdc);
extern "C" RT_API bool WinTrackMouseEvent(void* windowHandle, uint32_t flags, uint32_t hoverTimeMs);
extern "C" RT_API void* WinSetParent(void* childWindowHandle, void* parentWindowHandle);
extern "C" RT_API bool WinInvalidateRect(void* windowHandle, void* rect, bool eraseBackground);
extern "C" RT_API void* WinBeginPaint(void* windowHandle, void*& paintStruct);
extern "C" RT_API void WinEndPaint(void* windowHandle, void* paintStruct);
extern "C" RT_API void WinGetClipRect(void* paintStruct, int& x, int& y, int& w, int& h);
extern "C" RT_API void* WinCreateGraphics(void* hdc);
extern "C" RT_API void WinDeleteGraphics(void* nativeGraphics);
extern "C" RT_API void* WinCreateGraphicsFromWindowHandle(void* hwnd);
extern "C" RT_API void* WinCreateGraphicsFromImage(void* image);
extern "C" RT_API int WinGraphicsGetLastStatus(void* grapchis);
extern "C" RT_API void* WinGraphicsGetHDC(void* nativeGraphics);
extern "C" RT_API void* WinGraphicsCreatePen(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue, float width);
extern "C" RT_API void* WinGraphicsClonePen(void* pen);
extern "C" RT_API void WinGraphicsDeletePen(void* pen);
extern "C" RT_API int WinGraphicsPenGetLastStatus(void* pen);
extern "C" RT_API int WinGraphicsPenGetAlignment(void* pen);
extern "C" RT_API int WinGraphicsPenSetAlignment(void* pen, int alignment);
extern "C" RT_API void* WinGraphicsPenGetBrush(void* pen);
extern "C" RT_API int WinGraphicsPenSetBrush(void* pen, void* brush);
extern "C" RT_API int WinGraphicsPenGetColor(void* pen, uint8_t& alpha, uint8_t& red, uint8_t& green, uint8_t& blue);
extern "C" RT_API int WinGraphicsPenSetColor(void* pen, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);
extern "C" RT_API int WinGraphicsPenGetStartCap(void* pen);
extern "C" RT_API int WinGraphicsPenSetStartCap(void* pen, int startCap);
extern "C" RT_API int WinGraphicsPenGetEndCap(void* pen);
extern "C" RT_API int WinGraphicsPenSetEndCap(void* pen, int endCap);
extern "C" RT_API int WinGraphicsPenGetDashCap(void* pen);
extern "C" RT_API int WinGraphicsPenSetDashCap(void* pen, int dashCap);
extern "C" RT_API float WinGraphicsPenGetDashOffset(void* pen);
extern "C" RT_API int WinGraphicsPenSetDashOffset(void* pen, float dashOffset);
extern "C" RT_API int WinGraphicsPenGetDashStyle(void* pen);
extern "C" RT_API int WinGraphicsPenSetDashStyle(void* pen, int dashStyle);
extern "C" RT_API int WinGraphicsPenGetLineJoin(void* pen);
extern "C" RT_API int WinGraphicsPenSetLineJoin(void* pen, int lineJoin);
extern "C" RT_API void* WinGraphicsCreateSolidBrush(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);
extern "C" RT_API void* WinGraphicsCloneSolidBrush(void* solidBrush);
extern "C" RT_API void WinGraphicsDeleteSolidBrush(void* solidBrush);
extern "C" RT_API int WinGraphicsBrushGetLastStatus(void* brush);
extern "C" RT_API void* WinGraphicsCreateEmptyFontFamily();
extern "C" RT_API void* WinGraphicsCreateFontFamily(const char* familyName);
extern "C" RT_API void* WinGraphicsCloneFontFamily(void* fontFamily);
extern "C" RT_API void WinGraphicsDeleteFontFamily(void* fontFamily);
extern "C" RT_API int WinGraphicsGetFontFamilyName(void* fontFamily, void*& str);
extern "C" RT_API const void* WinGraphicsGetGenericMonospaceFontFamily();
extern "C" RT_API const void* WinGraphicsGetGenericSansSerifFontFamily();
extern "C" RT_API const void* WinGraphicsGetGenericSerifFontFamily();
extern "C" RT_API int WinGraphicsFontFamilyGetLastStatus(void* fontFamily);
extern "C" RT_API void* WinGraphicsCreateFont(const void* fontFamily, float emSize, int style, int unit);
extern "C" RT_API void* WinGraphicsCreateFontFromHFont(void* hdc, void* hfont);
extern "C" RT_API void* WinGraphicsCloneFont(void* font);
extern "C" RT_API void WinGraphicsDeleteFont(void* font);
extern "C" RT_API int WinGraphicsFontGetLastStatus(void* font);
extern "C" RT_API int WinGraphicsGetFontFamily(void* font, void* fontFamily);
extern "C" RT_API float WinGraphicsGetFontSize(void* font);
extern "C" RT_API float WinGraphicsGetFontHeight(void* font, const void* graphics);
extern "C" RT_API int WinGraphicsGetFontStyle(void* font);
extern "C" RT_API int WinGraphicsFontToHFont(void* font, void* graphics, void*& hfont);
extern "C" RT_API void* WinGraphicsCreateDefaultStringFormat();
extern "C" RT_API const void* WinGraphicsGetGenericDefaultStringFormat();
extern "C" RT_API const void* WinGraphicsGetGenericTypographicStringFormat();
extern "C" RT_API void* WinGraphicsCreateStringFormat(int formatFlags, uint16_t languageId);
extern "C" RT_API void* WinGraphicsCloneStringFormat(void* stringFormat);
extern "C" RT_API void WinGraphicsDeleteStringFormat(void* stringFormat);
extern "C" RT_API int WinGraphicsStringFormatGetLastStatus(void* stringFormat);
extern "C" RT_API int WinGraphicsStringFormatSetAlignment(void* stringFormat, int alignment);
extern "C" RT_API int WinGraphicsStringFormatSetLineAlignment(void* stringFormat, int alignment);
extern "C" RT_API int WinGraphicsStringFormatSetHotKeyPrefix(void* stringFormat, int hotKeyPrefix);
extern "C" RT_API int WinGraphicsDrawLine(void* graphics, void* pen, int startX, int startY, int endX, int endY);
extern "C" RT_API int WinGraphicsDrawLineF(void* graphics, void* pen, float startX, float startY, float endX, float endY);
extern "C" RT_API int WinGraphicsDrawLines(void* graphics, void* pen, int count, void* points);
extern "C" RT_API int WinGraphicsDrawLinesF(void* graphics, void* pen, int count, void* points);
extern "C" RT_API int WinGraphicsDrawString(void* graphics, const char* str, void* font, float x, float y, void* brush);
extern "C" RT_API int WinGraphicsDrawStringFormatPoint(void* graphics, const char* str, void* font, float x, float y, void* format, void* brush);
extern "C" RT_API int WinGraphicsDrawStringFormatRect(void* graphics, const char* str, void* font, float x, float y, float w, float h, void* format, void* brush);
extern "C" RT_API int WinGraphicsMeasureStringFormatSize(void* graphics, const char* str, void* font, float w, float h, void* format,
    float& outSizeW, float& outSizeH, int* codePointsFitted, int* linesFilled);
extern "C" RT_API int WinGraphicsMeasureStringFormatRect(void* graphics, const char* str, void* font, float x, float y, float w, float h, void* format,
    float& outX, float& outY, float& outW, float& outH, int* codePointsFitted, int* linesFilled);
extern "C" RT_API int WinGraphicsMeasureStringFormatPoint(void* graphics, const char* str, void* font, float x, float y, void* format,
    float& outX, float& outY, float& outW, float& outH);
extern "C" RT_API int WinGraphicsGetTextRenderingHint(void* graphics);
extern "C" RT_API int WinGraphicsSetTextRenderingHint(void* graphics, int textRenderingHint);
extern "C" RT_API int WinGraphicsClear(void* graphics, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue);
extern "C" RT_API int WinGraphicsDrawRectangle(void* graphics, void* pen, int x, int y, int w, int h);
extern "C" RT_API int WinGraphicsDrawRectangleF(void* graphics, void* pen, float x, float y, float w, float h);
extern "C" RT_API int WinGraphicsFillRectangle(void* graphics, void* brush, int x, int y, int w, int h);
extern "C" RT_API int WinGraphicsFillRectangleF(void* graphics, void* brush, float x, float y, float w, float h);
extern "C" RT_API int WinGraphicsFillPolygon(void* graphics, void* brush, int count, void* points);
extern "C" RT_API int WinGraphicsFillPolygonF(void* graphics, void* brush, int count, void* points);
extern "C" RT_API int WinGraphicsDrawArc(void* graphics, void* pen, float x, float y, float w, float h, float startAngle, float sweepAngle);
extern "C" RT_API int WinGraphicsDrawEllipse(void* graphics, void* pen, float x, float y, float w, float h);
extern "C" RT_API int WinGraphicsFillEllipse(void* graphics, void* brush, float x, float y, float w, float h);
extern "C" RT_API void* WinGraphicsCreateRegion();
extern "C" RT_API void* WinGraphicsCloneRegion(void* region);
extern "C" RT_API void WinGraphicsDeleteRegion(void* region);
extern "C" RT_API int WinGraphicsRegionGetLastStatus(void* region);
extern "C" RT_API int WinGraphicsGetClip(void* graphics, void* region);
extern "C" RT_API int WinGraphicsSetClipRect(void* graphics, int x, int y, int w, int h, int combineMode);
extern "C" RT_API int WinGraphicsSetClipRegion(void* graphics, void* region);
extern "C" RT_API void* WinGraphicsCreateImage(const char* fileName, bool useEmbeddedColorManagement);
extern "C" RT_API int WinGraphicsImageGetLastStatus(void* image);
extern "C" RT_API void* WinGraphicsCloneImage(void* image);
extern "C" RT_API void WinGraphicsDeleteImage(void* image);
extern "C" RT_API uint32_t WinGraphicsImageGetHeight(void* image);
extern "C" RT_API uint32_t WinGraphicsImageGetWidth(void* image);
extern "C" RT_API void* WinGraphicsCreateBitmap(const char* fileName, bool useEmbeddedColorManagement);
extern "C" RT_API void* WinGraphicsCreateBitmapWidthHeight(int w, int h, void* graphics);
extern "C" RT_API void* WinGraphicsCreateBitmapWinBitmap(void* winBitmap, void* palette);
extern "C" RT_API void* WinGraphicsCreateBitmapIcon(void* icon);
extern "C" RT_API void* WinGraphicsCreateBitmapResource(const char* resourceName);
extern "C" RT_API uint32_t WinGraphicsSave(void* graphics);
extern "C" RT_API int WinGraphicsRestore(void* graphics, uint32_t graphicsState);
extern "C" RT_API void* WinGraphicsCreateDefaultImageAttributes();
extern "C" RT_API int WinGraphicsImageAttributesGetLastStatus(void* imageAttributes);
extern "C" RT_API void* WinGraphicsCloneImageAttributes(void* imageAttributes);
extern "C" RT_API void WinGraphicsDeleteImageAttributes(void* imageAttributes);
extern "C" RT_API int WinGraphicsImageAttributesSetColorKey(void* imageAttributes, uint8_t colorLowAlpha, uint8_t colorLowRed, uint8_t colorLowGreen, uint8_t colorLowBlue,
    uint8_t colorHighAlpha, uint8_t colorHighRed, uint8_t colorHighGreen, uint8_t colorHighBlue, int colorAdjustType);
extern "C" RT_API int WinGraphicsDrawImagePoint(void* graphics, void* image, float x, float y);
extern "C" RT_API int WinGraphicsDrawImageRect(void* graphics, void* image, float x, float y, float w, float h);
extern "C" RT_API int WinGraphicsDrawImageWithAttributes(void* graphics, void* image, int destRectX, int destRectY, int destRectW, int destRectH, int srcX, int srcY, int srcW, int srcH,
    int srcUnit, void* imageAttributes);
extern "C" RT_API int WinGraphicsDrawImageWithAttributesF(void* graphics, void* image, float destRectX, float destRectY, float destRectW, float destRectH, float srcX, float srcY, float srcW, float srcH,
    int srcUnit, void* imageAttributes);
extern "C" RT_API int WinGraphicsGetTransform(void* graphics, void* matrix);
extern "C" RT_API int WinGraphicsSetTransform(void* graphics, void* matrix);
extern "C" RT_API int WinGraphicsMultiplyTransform(void* graphics, void* matrix, int order);
extern "C" RT_API int WinGraphicsResetTransform(void* graphics);
extern "C" RT_API int WinGraphicsRotateTransform(void* graphics, float angle, int order);
extern "C" RT_API int WinGraphicsScaleTransform(void* graphics, float scaleX, float scaleY, int order);
extern "C" RT_API int WinGraphicsTranslateTransform(void* graphics, float offsetX, float offsetY, int order);
extern "C" RT_API void* WinGraphicsCreateMatrix();
extern "C" RT_API void* WinGraphicsCreateMatrixFromElements(float m11, float m12, float m21, float m22, float dx, float dy);
extern "C" RT_API void* WinGraphicsCreateMatrixRectFPointF(float rectX, float rectY, float rectW, float rectH, float ptX, float ptY);
extern "C" RT_API void* WinGraphicsCreateMatrixRectPoint(int rectX, int rectY, int rectW, int rectH, int ptX, int ptY);
extern "C" RT_API int WinGraphicsMatrixGetLastStatus(void* matrix);
extern "C" RT_API void* WinGraphicsCloneMatrix(void* matrix);
extern "C" RT_API void WinGraphicsDeleteMatrix(void* matrix);
extern "C" RT_API int WinGraphicsMultiplyMatrix(void* matrix, void* factorMatrix, int order);
extern "C" RT_API int WinGraphicsResetMatrix(void* matrix);
extern "C" RT_API int WinGraphicsInvertMatrix(void* matrix);
extern "C" RT_API float WinGraphicsMatrixOffsetX(void* matrix);
extern "C" RT_API float WinGraphicsMatrixOffsetY(void* matrix);
extern "C" RT_API int WinGraphicsMatrixRotate(void* matrix, float angle, int order);
extern "C" RT_API int WinGraphicsMatrixRotateAt(void* matrix, float angle, float centerX, float centerY, int order);
extern "C" RT_API int WinGraphicsMatrixScale(void* matrix, float scaleX, float scaleY, int order);
extern "C" RT_API int WinGraphicsMatrixSetElements(void* matrix, float m11, float m12, float m21, float m22, float dx, float dy);
extern "C" RT_API int WinGraphicsMatrixShear(void* matrix, float shearX, float shearY, int order);
extern "C" RT_API int WinGraphicsMatrixTranslate(void* matrix, float offsetX, float offsetY, int order);
extern "C" RT_API int WinGraphicsMatrixTransformPoints(void* matrix, void* points, int numPoints);
extern "C" RT_API int WinGraphicsMatrixTransformPointsF(void* matrix, void* points, int numPoints);
extern "C" RT_API int WinGraphicsGetPageUnit(void* graphics);
extern "C" RT_API int WinGraphicsSetPageUnit(void* graphics, int unit);
extern "C" RT_API float WinGraphicsGetPageScale(void* graphics);
extern "C" RT_API int WinGraphicsSetPageScale(void* graphics, float scale);
extern "C" RT_API float WinGraphicsGetDpiX(void* graphics);
extern "C" RT_API float WinGraphicsGetDpiY(void* graphics);
extern "C" RT_API int WinGraphicsGetSmoothingMode(void* graphics);
extern "C" RT_API int WinGraphicsSetSmoothingMode(void* graphics, int smoothingMode);
extern "C" RT_API int WinGraphicsGetEncoderClsId(const char* imageFormat, void* clsid);
extern "C" RT_API int WinGraphicsImageSave(void* image, const char* fileName, const void* encoderClsId);
extern "C" RT_API void WinGetSysColor(int index, uint8_t& red, uint8_t& green, uint8_t& blue);
extern "C" RT_API bool WinSetBkColor(void* hdc, uint8_t red, uint8_t green, uint8_t blue);
extern "C" RT_API void WinGetMessagePos(int32_t& x, int32_t& y);
extern "C" RT_API void* WinGetFocus();
extern "C" RT_API void* WinSetFocus(void* windowHandle);
extern "C" RT_API void* WinGetTopWindow(void* windowHandle);
extern "C" RT_API bool WinBringWindowToTop(void* windowHandle);
extern "C" RT_API bool WinSetWindowPos(void* windowHandle, void* insertAfterWindowHandle, int x, int y, int cx, int cy, uint32_t flags);
extern "C" RT_API bool WinGetOpenFileName(void* windowHandle, const char16_t* filter, const char16_t* initialDirectory, char16_t* fileNameBuffer, uint32_t fileNameBufferSize, uint32_t flags, const char16_t* defaultExtension);
extern "C" RT_API bool WinGetSaveFileName(void* windowHandle, const char16_t* filter, const char16_t* initialDirectory, char16_t* fileNameBuffer, uint32_t fileNameBufferSize, uint32_t flags, const char16_t* defaultExtension);
extern "C" RT_API uint32_t WinCommDlgExtendedError();
extern "C" RT_API void* WinLoadImage(const char* name, uint32_t type, int32_t cx, int32_t cy);
extern "C" RT_API void* WinLoadStandardCursor(int cursorId);
extern "C" RT_API void* WinLoadCursor(const char* cursorName);
extern "C" RT_API void* WinGetCursor();
extern "C" RT_API void* WinSetCursor(void* cursorHandle);
extern "C" RT_API bool WinDestroyCursor(void* cursorHandle);
extern "C" RT_API void* WinLoadStandardIcon(int iconId);
extern "C" RT_API void* WinLoadIcon(const char* iconName);
extern "C" RT_API bool WinDestroyIcon(void* iconHandle);
extern "C" RT_API void* WinLoadStandardBitmap(int bitmapId);
extern "C" RT_API void* WinLoadBitmap(const char* bitmapName);
extern "C" RT_API bool WinDestroyBitmap(void* bitmapHandle);
extern "C" RT_API bool WinDeleteObject(void* handle);
extern "C" RT_API int WinGetSystemMetrics(int index);
extern "C" RT_API void* WinSetCapture(void* windowHandle);
extern "C" RT_API bool WinReleaseCapture();
extern "C" RT_API int64_t WinGetClassLong(void* windowHandle, int index);
extern "C" RT_API int64_t WinSetClassLong(void* windowHandle, int index, int64_t value);
extern "C" RT_API int64_t WinGetWindowLong(void* windowHandle, int index);
extern "C" RT_API bool WinSetWindowLong(void* windowHandle, int index, int64_t newValue);
extern "C" RT_API bool WinScrollWindow(void* windowHandle, int xAmount, int yAmount,
    int clientLocX, int clientLocY, int clientSizeW, int clientSizeH, int clipLocX, int clipLocY, int clipSizeW, int clipSizeH);
extern "C" RT_API bool WinGetScrollInfo(void* windowHandle, int nBar, uint32_t& nPage, int32_t& nPos, int32_t& nMin, int32_t& nMax, int32_t& nTrackPos);
extern "C" RT_API int WinSetScrollInfo(void* windowHandle, int nBar, uint32_t fMask, bool redraw, uint32_t nPage, int32_t nPos, int32_t nMin, int32_t nMax);
extern "C" RT_API bool WinShowScrollBar(void* windowHandle, int nBar, bool show);
extern "C" RT_API bool WinCreateCaret(void* windowHandle, void* bitmapHandle, int width, int height);
extern "C" RT_API bool WinDestroyCaret();
extern "C" RT_API bool WinShowCaret(void* windowHandle);
extern "C" RT_API bool WinHideCaret(void* windowHandle);
extern "C" RT_API bool WinGetCaretPos(int& x, int& y);
extern "C" RT_API bool WinSetCaretPos(int x, int y);
extern "C" RT_API bool WinSetTimer(void* windowHandle, uint32_t timerId, uint32_t elapse);
extern "C" RT_API bool WinKillTimer(void* windowHandle, uint32_t timerId);
extern "C" RT_API bool WinRegOpenCurrentUser(void** result);
extern "C" RT_API bool WinRegCloseKey(void* key);
extern "C" RT_API bool WinRegGetDWordValue(void* key, const char* subKey, const char* valueName, uint32_t& value);
extern "C" RT_API uint32_t WinRegisterClipboardFormat(const char* formatName);
extern "C" RT_API bool WinOpenClipboard(void* windowHandle);
extern "C" RT_API bool WinCloseClipboard();
extern "C" RT_API bool WinEmptyClipboard();
extern "C" RT_API void* WinSetClipboardData(uint32_t format, void* mem);
extern "C" RT_API void* WinGetClipboardData(uint32_t format);
extern "C" RT_API bool WinIsClipboardFormatAvailable(uint32_t format);
extern "C" RT_API bool WinAddClipboardFormatListener(void* windowHandle);
extern "C" RT_API bool WinRemoveClipboardFormatListener(void* windowHandle);
extern "C" RT_API void* WinGlobalAlloc(uint32_t flags, uint64_t size);
extern "C" RT_API void* WinGlobalLock(void* memHandle);
extern "C" RT_API bool WinGlobalUnlock(void* memHandle);
extern "C" RT_API void* WinGlobalFree(void* memHandle);
extern "C" RT_API uint64_t WinGlobalSize(void* memHandle);

#endif // CMAJOR_RTS_WINDOWS_API_INCLUDED
