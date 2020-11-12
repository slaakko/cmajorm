// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/WindowsAPI.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Process.hpp>
#define OEMRESOURCE
#ifdef __MINGW32__
#define WINVER 0x0600
#define UNICODE
#define _UNICODE
#endif
#include <Windows.h>
#include <map>
#include <string>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

#ifndef __MINGW32__
#include <shobjidl_core.h>
#endif

using namespace soulng::unicode;
using namespace soulng::util;
using namespace Gdiplus;

struct CodeMapping
{
    CodeMapping(long extCode_, long winCode_) : extCode(extCode_), winCode(winCode_)
    {
    }
    long extCode;
    long winCode;
};

long ERROR_OUT_OF_RESOURCES = 0;

CodeMapping code_mapping[] =
{
    CodeMapping(0, ERROR_OUT_OF_RESOURCES),
    CodeMapping(1, ERROR_FILE_NOT_FOUND),
    CodeMapping(2, ERROR_PATH_NOT_FOUND),
    CodeMapping(3, ERROR_BAD_FORMAT),
    CodeMapping(4, SE_ERR_ACCESSDENIED),
    CodeMapping(5, SE_ERR_ASSOCINCOMPLETE),
    CodeMapping(6, SE_ERR_DDEBUSY),
    CodeMapping(7, SE_ERR_DDEFAIL),
    CodeMapping(8, SE_ERR_DDETIMEOUT),
    CodeMapping(9, SE_ERR_DLLNOTFOUND),
    CodeMapping(10, SE_ERR_FNF),
    CodeMapping(11, SE_ERR_NOASSOC),
    CodeMapping(12, SE_ERR_OOM),
    CodeMapping(13, SE_ERR_PNF),
    CodeMapping(14, SE_ERR_SHARE)
};

bool codeMappingInitialized = false;

std::map<long, long> winToExtCodeMap;
std::map<long, long> extToWinCodeMap;

bool InitCodeMapping()
{
    for (const CodeMapping& mapping : code_mapping)
    {
        auto it1 = winToExtCodeMap.find(mapping.winCode);
        if (it1 != winToExtCodeMap.cend())
        {
            return false;
        }
        winToExtCodeMap[mapping.winCode] = mapping.extCode;
        auto it2 = extToWinCodeMap.find(mapping.extCode);
        if (it2 != extToWinCodeMap.cend())
        {
            return false;
        }
        extToWinCodeMap[mapping.extCode] = mapping.winCode;
    }
    codeMappingInitialized = true;
    return true;
}

long MapWinCodeToExtCode(long winCode)
{
    if (!codeMappingInitialized)
    {
        if (!InitCodeMapping())
        {
            return -2;
        }
    }
    auto it = winToExtCodeMap.find(winCode);
    if (it != winToExtCodeMap.cend())
    {
        return it->second;
    }
    return -1;
}

long MapExtCodeToWinCode(long extCode)
{
    if (!codeMappingInitialized)
    {
        if (!InitCodeMapping())
        {
            return -2;
        }
    }
    auto it = extToWinCodeMap.find(extCode);
    if (it != extToWinCodeMap.cend())
    {
        return it->second;
    }
    return -1;
}

uint64_t WinGetLastError()
{
    return GetLastError();
}

void WinFormatMessage(uint64_t errorCode, char16_t* buffer)
{
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, LANG_SYSTEM_DEFAULT, (LPWSTR)buffer, 4096, nullptr);
}

void* WinCreateEvent()
{
    HANDLE handle = CreateEventA(NULL, false, false, NULL);
    return handle;
}

bool WinSetEvent(void* eventHandle)
{
    bool retval = SetEvent(eventHandle);
    return retval;
}

bool WinResetEvent(void* eventHandle)
{
    bool retval = ResetEvent(eventHandle);
    return retval;
}

bool WinWaitEvent(void* eventHandle)
{
    DWORD retval = WaitForSingleObject(eventHandle, INFINITE);
    return retval == WAIT_OBJECT_0;
}

bool WinCloseHandle(void* handle)
{
    bool retval = CloseHandle(handle);
    return retval;
}

int32_t WinWaitForMultipleObjects(uint32_t count, void** handles)
{
    uint32_t retval = WaitForMultipleObjects(count, handles, false, INFINITE);
    if (retval == WAIT_FAILED)
    {
        return -1;
    }
    else if (retval == WAIT_TIMEOUT)
    {
        return -2;
    }
    else if (retval >= WAIT_OBJECT_0 && retval < WAIT_OBJECT_0 + count)
    {
        return retval - WAIT_OBJECT_0;
    }
    else if (retval >= WAIT_ABANDONED_0 && retval < WAIT_ABANDONED_0 + count)
    {
        return -3;
    }
    else
    {
        return -4;
    }
}

void* WinFindFirstChangeNotification(const char* directoryPath)
{
    void* handle = FindFirstChangeNotificationA(directoryPath, false, FILE_NOTIFY_CHANGE_FILE_NAME);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return nullptr;
    }
    return handle;
}

bool WinFindNextChangeNotification(void* handle)
{
    bool retval = FindNextChangeNotification(handle);
    return retval;
}

bool WinFindCloseChangeNotification(void* handle)
{
    bool retval = FindCloseChangeNotification(handle);
    return retval;
}

bool WinShellExecute(const char* filePath, int64_t& errorCode)
{
    HINSTANCE instance = ShellExecuteA(NULL, "open", filePath, NULL, NULL, SW_SHOWNORMAL);
    int code = *reinterpret_cast<int*>(&instance);
    if (code > 32)
    {
        errorCode = 0;
        return true;
    }
    errorCode = MapWinCodeToExtCode(code);
    return false;
}

typedef bool (*messageProcessorFunction)(void* windowHandle, uint32_t message, uint32_t wparam, int64_t lparam, int64_t& result, void*& originalWndProc);
typedef void (*keyPreviewFunction)(uint32_t keycode, bool shift, bool control, bool alt, bool keyDown, bool& handled);

messageProcessorFunction messageProcessor = nullptr;
keyPreviewFunction keyPreview = nullptr;

bool WinKeyPressed(int keyCode)
{
    return (GetKeyState(keyCode) & 0x8000) != 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int64_t result = 0;
    void* originalWndProc = nullptr;
    bool handled = messageProcessor(hWnd, message, wParam, lParam, result, originalWndProc);
    if (!handled)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return result;
}

LRESULT CALLBACK CommandSubClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int64_t result = 0;
    void* originalWndProc = nullptr;
    messageProcessor(hWnd, message, wParam, lParam, result, originalWndProc);
    return CallWindowProcW((WNDPROC)originalWndProc, hWnd, message, wParam, lParam);
}

void* WinSubClassCommandWndProc(void* windowHandle)
{
    LONG_PTR r = SetWindowLongPtrW((HWND)windowHandle, GWLP_WNDPROC, (LONG_PTR)CommandSubClassWndProc);
    WNDPROC wndproc = *reinterpret_cast<WNDPROC*>(&r);
    void* result = *reinterpret_cast<void**>(&wndproc);
    return result;
}

void WinRestoreOriginalWndProc(void* windowHandle, void* originalWndProc)
{
    SetWindowLongPtrW((HWND)windowHandle, GWLP_WNDPROC, (LONG_PTR)originalWndProc);
}

HINSTANCE applicationInstance = nullptr;

void WinSetInstance()
{
    applicationInstance = GetModuleHandle(nullptr);
}

ULONG_PTR gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

int WinInit(void* messageProcessorFunctionAddress, void* keyPreviewFunctionAddress)
{
    messageProcessor = reinterpret_cast<messageProcessorFunction>(messageProcessorFunctionAddress);
    keyPreview = reinterpret_cast<keyPreviewFunction>(keyPreviewFunctionAddress);
    Status status = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    return static_cast<int>(status);
}

void WinDone()
{
    GdiplusShutdown(gdiplusToken);
}

int WinRun()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        bool handled = false;
        if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
        {
            if (keyPreview)
            {
                uint32_t keyCode = msg.wParam;
                bool keyDown = msg.message == WM_KEYDOWN;
                int16_t shiftState = GetKeyState(VK_SHIFT);
                bool shift = (shiftState & (1 << 16)) != 0;
                int16_t controlState = GetKeyState(VK_CONTROL);
                bool control = (controlState & (1 << 16)) != 0;
                int16_t altState = GetKeyState(VK_MENU);
                bool alt = (altState & (1 << 16)) != 0;
                keyPreview(keyCode, shift, control, alt, keyDown, handled);
            }
        }
        if (!handled)
        {
            DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}

int WinApplicationMessageLoop()
{
    DWORD queueStatus = GetQueueStatus(QS_ALLEVENTS);
    bool messageWaiting = HIWORD(queueStatus) != 0;
    if (messageWaiting)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            bool handled = false;
            if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
            {
                if (keyPreview)
                {
                    uint32_t keyCode = msg.wParam;
                    bool keyDown = msg.message == WM_KEYDOWN;
                    int16_t shiftState = GetKeyState(VK_SHIFT);
                    bool shift = (shiftState & (1 << 16)) != 0;
                    int16_t controlState = GetKeyState(VK_CONTROL);
                    bool control = (controlState & (1 << 16)) != 0;
                    int16_t altState = GetKeyState(VK_MENU);
                    bool alt = (altState & (1 << 16)) != 0;
                    keyPreview(keyCode, shift, control, alt, keyDown, handled);
                }
            }
            if (!handled)
            {
                DispatchMessage(&msg);
            }
        }
        return msg.wParam;
    }
    return 1;
}

const uint32_t CM_ENDMODAL = WM_USER + 1;

void WinRunModal()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        if (msg.message == CM_ENDMODAL)
        {
            return;
        }
        bool handled = false;
        if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
        {
            if (keyPreview)
            {
                uint32_t keyCode = msg.wParam;
                bool keyDown = msg.message == WM_KEYDOWN;
                int16_t shiftState = GetKeyState(VK_SHIFT);
                bool shift = (shiftState & (1 << 16)) != 0;
                int16_t controlState = GetKeyState(VK_CONTROL);
                bool control = (controlState & (1 << 16)) != 0;
                int16_t altState = GetKeyState(VK_MENU);
                bool alt = (altState & (1 << 16)) != 0;
                keyPreview(keyCode, shift, control, alt, keyDown, handled);
            }
        }
        if (!handled)
        {
            DispatchMessage(&msg);
        }
    }
}

void* WinGetForegroundWindow()
{
    return GetForegroundWindow();
}

bool WinEnableWindow(void* windowHandle, bool enable)
{
    return EnableWindow((HWND)windowHandle, enable);
}

typedef int (*getDialogResultFunction)(void* dialogWindowPtr);
typedef void (*dialogKeyPreviewFunction)(void* dialogWindowPtr, uint32_t keyCode, bool shift, bool control, bool alt, bool keyDown, bool& handled);

int WinDialogWindowMessageLoop(void* windowHandle, void* parentWindowHandle, void* getDialogResultFunc, void* keyPreviewFunc, void* dialogWindowPtr)
{
    int dialogResult = 0;
    getDialogResultFunction dialogResultFun = reinterpret_cast<getDialogResultFunction>(getDialogResultFunc);
    dialogKeyPreviewFunction keyPreviewFun = reinterpret_cast<dialogKeyPreviewFunction>(keyPreviewFunc);
    EnableWindow((HWND)parentWindowHandle, false);
    ShowWindow((HWND)windowHandle, SW_SHOW);
    BringWindowToTop((HWND)windowHandle);
    SetActiveWindow((HWND)windowHandle);
    bool end = false;
    while (!end)
    {
        MSG msg;
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            bool handled = false;
            if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
            {
                if (keyPreviewFun)
                {
                    uint32_t keyCode = msg.wParam;
                    int16_t shiftState = GetKeyState(VK_SHIFT);
                    bool shift = (shiftState & (1 << 16)) != 0;
                    int16_t controlState = GetKeyState(VK_CONTROL);
                    bool control = (controlState & (1 << 16)) != 0;
                    int16_t altState = GetKeyState(VK_MENU);
                    bool alt = (altState  & (1 << 16)) != 0;
                    bool keyDown = msg.message == WM_KEYDOWN;
                    keyPreviewFun(dialogWindowPtr, keyCode, shift, control, alt, keyDown, handled);
                }
            }
            if (!handled)
            {
                DispatchMessage(&msg);
            }
            dialogResult = dialogResultFun(dialogWindowPtr);
            if (dialogResult != 0)
            {
                end = true;
            }
        }
        else
        {
            PostQuitMessage(msg.wParam);
            end = true;
        }
    }
    EnableWindow((HWND)parentWindowHandle, true);
    PostMessage((HWND)windowHandle, WM_CLOSE, 0u, 0);
    SetActiveWindow((HWND)parentWindowHandle);
    return dialogResult;
}

 uint16_t WinRegisterWindowClass(const char* windowClassName, uint32_t style, int backgroundColor)
{
    std::u16string className = ToUtf16(windowClassName);
    WNDCLASSEXW wc;
    wc.cbSize = sizeof(wc);
    wc.style = style;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = applicationInstance;
    wc.hIcon = LoadIcon(applicationInstance, IDI_APPLICATION);
    wc.hCursor = nullptr;
    int64_t c = backgroundColor + 1;
    HBRUSH bgc = *reinterpret_cast<HBRUSH*>(&c);
    wc.hbrBackground = bgc;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = (LPWSTR)className.c_str();
    wc.hIconSm = LoadIcon(applicationInstance, IDI_APPLICATION);
    uint16_t atom = RegisterClassExW(&wc);
    return atom;
}

int WinShowMessageBox(const char* text, const char* caption)
{
    std::u16string str = ToUtf16(text);
    LPCWSTR captionStr = nullptr;
    std::u16string cap;
    if (caption != nullptr)
    {
        cap = ToUtf16(caption);
        captionStr = (LPCWSTR)cap.c_str();
    }
    return MessageBoxW(nullptr, (LPCWSTR)str.c_str(), captionStr, MB_OK);
}

int WinShowMessageBoxWithType(const char* text, const char* caption, void* ownerWindowHandle, uint32_t type)
{
    std::u16string str = ToUtf16(text);
    LPCWSTR captionStr = nullptr;
    std::u16string cap;
    if (caption != nullptr)
    {
        cap = ToUtf16(caption);
        captionStr = (LPCWSTR)cap.c_str();
    }
    const char16_t* s = str.c_str();
    return MessageBoxW((HWND)ownerWindowHandle, reinterpret_cast<LPCWSTR>(s), captionStr, type);
}

bool WinMessageBeep(uint32_t messageBeepType)
{
    return MessageBeep(messageBeepType);
}

void* WinCreateWindowByClassAtom(uint16_t windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle)
{
    std::u16string name = ToUtf16(windowName);
    uint64_t wc = windowClass;
    LPCWSTR wcs = *reinterpret_cast<LPWSTR*>(&wc);
    HWND handle = CreateWindowExW(exStyle, wcs, (LPCWSTR)name.c_str(), style, x, y, w, h, (HWND)parentHandle, nullptr, applicationInstance, nullptr);
    return handle;
}

void* WinCreateWindowByClassName(const char* windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle)
{
    std::u16string windowClassName = ToUtf16(windowClass);
    std::u16string name = ToUtf16(windowName);
    HWND handle = CreateWindowExW(exStyle, (LPCWSTR)windowClassName.c_str(), (LPCWSTR)name.c_str(), style, x, y, w, h, (HWND)parentHandle, nullptr, applicationInstance, nullptr);
    return handle;
}

void WinPostQuitMessage(int exitCode)
{
    PostQuitMessage(exitCode);
}

bool WinPostMessage(void* windowHandle, uint32_t msg, uint32_t wparam, int64_t lparam)
{
    return PostMessage((HWND)windowHandle, msg, wparam, lparam);
}

int64_t WinSendMessage(void* windowHandle, uint32_t msg, uint32_t wparam, int64_t lparam)
{
    return SendMessage((HWND)windowHandle, msg, wparam, lparam);
}

bool WinShowWindow(void* windowHandle, int commandShow)
{
    return ShowWindow((HWND)windowHandle, commandShow);
}

bool WinUpdateWindow(void* windowHandle)
{
    return UpdateWindow((HWND)windowHandle);
}

bool WinMoveWindow(void* windowHandle, int x, int y, int w, int h, bool repaint)
{
    return MoveWindow((HWND)windowHandle, x, y, w, h, repaint);
}

bool WinGetClientRect(void* windowHandle, void* rect)
{
    return GetClientRect((HWND)windowHandle, (LPRECT)rect);
}

bool WinGetWindowRect(void* windowHandle, void* rect)
{
    return GetWindowRect((HWND)windowHandle, (LPRECT)rect);
}

bool WinClientToScreen(void* windowHandle, void* point)
{
    return ClientToScreen((HWND)windowHandle, (LPPOINT)point);
}

bool WinScreenToClient(void* windowHandle, void* point)
{
    return ScreenToClient((HWND)windowHandle, (LPPOINT)point);
}

bool WinPtInRect(void* rect, void* point)
{
    return PtInRect((const RECT*)rect, *(POINT*)point);
}

bool WinSetWindowText(void* windowHandle, const char* text)
{
    std::u16string str(ToUtf16(text));
    return SetWindowTextW((HWND)windowHandle, (LPWSTR)str.c_str());
}

int WinGetWindowTextLength(void* windowHandle)
{
    return GetWindowTextLengthW((HWND)windowHandle);
}

int WinGetWindowText(void* windowHandle, char16_t* textBuffer, int maxCount)
{
    return GetWindowTextW((HWND)windowHandle, (LPWSTR)textBuffer, maxCount);
}

void* WinGetDC(void* windowHandle)
{
    return GetDC((HWND)windowHandle);
}

int WinReleaseHDC(void* windowHandle, void* hdc)
{
    return ReleaseDC((HWND)windowHandle, (HDC)hdc);
}

bool WinTrackMouseEvent(void* windowHandle, uint32_t flags, uint32_t hoverTimeMs)
{
    TRACKMOUSEEVENT eventTrack;
    eventTrack.hwndTrack = (HWND)windowHandle;
    eventTrack.cbSize = sizeof(eventTrack);
    eventTrack.dwFlags = flags;
    eventTrack.dwHoverTime = hoverTimeMs;
    return TrackMouseEvent(&eventTrack);
}

void* WinSetParent(void* childWindowHandle, void* parentWindowHandle)
{
    return SetParent((HWND)childWindowHandle, (HWND)parentWindowHandle);
}

bool WinInvalidateRect(void* windowHandle, void* rect, bool eraseBackground)
{
    return InvalidateRect((HWND)windowHandle, (const RECT*)rect, eraseBackground);
}

void* WinBeginPaint(void* windowHandle, void*& paintStruct)
{
    paintStruct = new PAINTSTRUCT();
    return BeginPaint((HWND)windowHandle, (LPPAINTSTRUCT)paintStruct);
}

void WinEndPaint(void* windowHandle, void* paintStruct)
{
    EndPaint((HWND)windowHandle, (const PAINTSTRUCT*)paintStruct);
    delete static_cast<PAINTSTRUCT*>(paintStruct);
}

void WinGetClipRect(void* paintStruct, int& x, int& y, int& w, int& h)
{
    PAINTSTRUCT* ps = static_cast<PAINTSTRUCT*>(paintStruct);
    x = ps->rcPaint.left;
    y = ps->rcPaint.top;
    w = ps->rcPaint.right - x;
    h = ps->rcPaint.bottom - y;
}

void* WinCreateGraphics(void* hdc)
{
    Graphics* graphics = new Graphics((HDC)hdc);
    return graphics;
}

void WinDeleteGraphics(void* nativeGraphics)
{
    delete static_cast<Graphics*>(nativeGraphics);
}

void* WinCreateGraphicsFromWindowHandle(void* hwnd)
{
    Graphics* graphics = new Graphics((HWND)hwnd);
    return graphics;
}

void* WinCreateGraphicsFromImage(void* image)
{
    return Graphics::FromImage(static_cast<Image*>(image));
}

int WinGraphicsGetLastStatus(void* graphics)
{
    return static_cast<Graphics*>(graphics)->GetLastStatus();
}

int WinGraphicsFontGetLastStatus(void* font)
{
    return static_cast<Font*>(font)->GetLastStatus();
}

void* WinGraphicsGetHDC(void* nativeGraphics)
{
    return static_cast<Graphics*>(nativeGraphics)->GetHDC();
}

void* WinGraphicsCreatePen(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue, float width)
{
    return new Pen(Color(alpha, red, green, blue), width);
}

void WinGraphicsDeletePen(void* pen)
{
    delete static_cast<Pen*>(pen);
}

void* WinGraphicsClonePen(void* pen)
{
    return static_cast<Pen*>(pen)->Clone();
}

int WinGraphicsPenGetLastStatus(void* pen)
{
    return static_cast<Pen*>(pen)->GetLastStatus();
}

int WinGraphicsPenGetAlignment(void* pen)
{
    return static_cast<Pen*>(pen)->GetAlignment();
}

int WinGraphicsPenSetAlignment(void* pen, int alignment)
{
    return static_cast<Pen*>(pen)->SetAlignment(static_cast<PenAlignment>(alignment));
}

void* WinGraphicsPenGetBrush(void* pen)
{
#ifdef __MINGW32__
    #warning GetBrush not implemented in mingw gdipluspen.h!!!!!!!
    return nullptr; 
#else
    return static_cast<Pen*>(pen)->GetBrush();
#endif
}

int WinGraphicsPenSetBrush(void* pen, void* brush)
{
    return static_cast<Pen*>(pen)->SetBrush(static_cast<const Brush*>(brush));
}

int WinGraphicsPenGetColor(void* pen, uint8_t& alpha, uint8_t& red, uint8_t& green, uint8_t& blue)
{
    Color color;
    int result = static_cast<Pen*>(pen)->GetColor(&color);
    if (result != 0)
    {
        return result;
    }
    alpha = color.GetAlpha();
    red = color.GetRed();
    green = color.GetGreen();
    blue = color.GetBlue();
    return result;
}

int WinGraphicsPenSetColor(void* pen, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    Color color(alpha, red, green, blue);
    return static_cast<Pen*>(pen)->SetColor(color);
}

int WinGraphicsPenGetStartCap(void* pen)
{
    return static_cast<Pen*>(pen)->GetStartCap();
}

int WinGraphicsPenSetStartCap(void* pen, int startCap)
{
    return static_cast<Pen*>(pen)->SetStartCap(static_cast<LineCap>(startCap));
}

int WinGraphicsPenGetEndCap(void* pen)
{
    return static_cast<Pen*>(pen)->GetEndCap();
}

int WinGraphicsPenSetEndCap(void* pen, int endCap)
{
    return static_cast<Pen*>(pen)->SetEndCap(static_cast<LineCap>(endCap));
}

int WinGraphicsPenGetDashCap(void* pen)
{
    return static_cast<Pen*>(pen)->GetDashCap();
}

int WinGraphicsPenSetDashCap(void* pen, int dashCap)
{
    return static_cast<Pen*>(pen)->SetDashCap(static_cast<DashCap>(dashCap));
}

float WinGraphicsPenGetDashOffset(void* pen)
{
    return static_cast<Pen*>(pen)->GetDashOffset();
}

int WinGraphicsPenSetDashOffset(void* pen, float dashOffset)
{
    return static_cast<Pen*>(pen)->SetDashOffset(dashOffset);
}

int WinGraphicsPenGetDashStyle(void* pen)
{
    return static_cast<Pen*>(pen)->GetDashStyle();
}

int WinGraphicsPenSetDashStyle(void* pen, int dashStyle)
{
    return static_cast<Pen*>(pen)->SetDashStyle(static_cast<DashStyle>(dashStyle));
}

int WinGraphicsPenGetLineJoin(void* pen)
{
    return static_cast<Pen*>(pen)->GetLineJoin();
}

int WinGraphicsPenSetLineJoin(void* pen, int lineJoin)
{
    return static_cast<Pen*>(pen)->SetLineJoin(static_cast<LineJoin>(lineJoin));
}

void* WinGraphicsCreateSolidBrush(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    return new SolidBrush(Color(alpha, red, green, blue));
}

void* WinGraphicsCloneSolidBrush(void* solidBrush)
{
    return static_cast<SolidBrush*>(solidBrush)->Clone();
}

void WinGraphicsDeleteSolidBrush(void* solidBrush)
{
    delete static_cast<SolidBrush*>(solidBrush);
}

int WinGraphicsBrushGetLastStatus(void* brush)
{
    return static_cast<Brush*>(brush)->GetLastStatus();
}

void* WinGraphicsCreateEmptyFontFamily()
{
    return new FontFamily();
}

void* WinGraphicsCreateFontFamily(const char* familyName)
{
    std::u16string name = ToUtf16(std::string(familyName));
    return new FontFamily((const WCHAR*)name.c_str(), nullptr);
}

void* WinGraphicsCloneFontFamily(void* fontFamily)
{
    return static_cast<FontFamily*>(fontFamily)->Clone();
}

void WinGraphicsDeleteFontFamily(void* fontFamily)
{
    delete static_cast<FontFamily*>(fontFamily);
}

int WinGraphicsGetFontFamilyName(void* fontFamily, void*& str)
{
    return static_cast<FontFamily*>(fontFamily)->GetFamilyName((LPWSTR)str);
}

const void* WinGraphicsGetGenericMonospaceFontFamily()
{
    return FontFamily::GenericMonospace();
}

const void* WinGraphicsGetGenericSansSerifFontFamily()
{
    return FontFamily::GenericSansSerif();
}

const void* WinGraphicsGetGenericSerifFontFamily()
{
    return FontFamily::GenericSerif();
}

int WinGraphicsFontFamilyGetLastStatus(void* fontFamily)
{
    if (fontFamily != nullptr)
    {
        return static_cast<FontFamily*>(fontFamily)->GetLastStatus();
    }
    return Status::GenericError;
}

void* WinGraphicsCreateFont(const void* fontFamily, float emSize, int style, int unit)
{
    return new Font(static_cast<const FontFamily*>(fontFamily), emSize, style, static_cast<Gdiplus::Unit>(unit));
}

void* WinGraphicsCreateFontFromHFont(void* hdc, void* hfont)
{
    return new Font((HDC)hdc, (const HFONT)hfont);
}

void* WinGraphicsCloneFont(void* font)
{
    return static_cast<Font*>(font)->Clone();
}

void WinGraphicsDeleteFont(void* font)
{
    delete static_cast<Font*>(font);
}

int WinGraphicsGetFontFamily(void* font, void* fontFamily)
{
    return static_cast<Font*>(font)->GetFamily(static_cast<FontFamily*>(fontFamily));
}

float WinGraphicsGetFontSize(void* font)
{
    return static_cast<Font*>(font)->GetSize();
}

float WinGraphicsGetFontHeight(void* font, const void* graphics)
{
    return static_cast<Font*>(font)->GetHeight(static_cast<const Graphics*>(graphics));
}

int WinGraphicsGetFontStyle(void* font)
{
    return static_cast<Font*>(font)->GetStyle();
}

int WinGraphicsFontToHFont(void* font, void* graphics, void*& hfont)
{
    LOGFONTW logFont;
    int status = static_cast<Font*>(font)->GetLogFontW(static_cast<const Graphics*>(graphics), &logFont);
    if (status != 0)
    {
        return status;
    }
    hfont = CreateFontIndirectW(&logFont);
    if (hfont == nullptr)
    {
        return Status::GenericError;
    }
    return 0;
}

void* WinGraphicsCreateDefaultStringFormat()
{
    return new StringFormat();
}

const void* WinGraphicsGetGenericDefaultStringFormat()
{
    return StringFormat::GenericDefault();
}

const void* WinGraphicsGetGenericTypographicStringFormat()
{
    return StringFormat::GenericTypographic();
}

void* WinGraphicsCreateStringFormat(int formatFlags, uint16_t languageId)
{
    return new StringFormat(formatFlags, languageId);
}

void* WinGraphicsCloneStringFormat(void* stringFormat)
{
    return new StringFormat(static_cast<const StringFormat*>(stringFormat));
}

void WinGraphicsDeleteStringFormat(void* stringFormat)
{
    delete static_cast<StringFormat*>(stringFormat);
}

int WinGraphicsStringFormatGetLastStatus(void* stringFormat)
{
    return static_cast<StringFormat*>(stringFormat)->GetLastStatus();
}

int WinGraphicsStringFormatSetAlignment(void* stringFormat, int alignment)
{
    return static_cast<StringFormat*>(stringFormat)->SetAlignment(static_cast<StringAlignment>(alignment));
}

int WinGraphicsStringFormatSetLineAlignment(void* stringFormat, int alignment)
{
    return static_cast<StringFormat*>(stringFormat)->SetLineAlignment(static_cast<StringAlignment>(alignment));
}

int WinGraphicsStringFormatSetHotKeyPrefix(void* stringFormat, int hotKeyPrefix)
{
    return static_cast<StringFormat*>(stringFormat)->SetHotkeyPrefix(static_cast<HotkeyPrefix>(hotKeyPrefix));
}

int WinGraphicsDrawLine(void* graphics, void* pen, int startX, int startY, int endX, int endY)
{
    return static_cast<Graphics*>(graphics)->DrawLine(static_cast<Pen*>(pen), Point(startX, startY), Point(endX, endY));
}

int WinGraphicsDrawLineF(void* graphics, void* pen, float startX, float startY, float endX, float endY)
{
    return static_cast<Graphics*>(graphics)->DrawLine(static_cast<Pen*>(pen), PointF(startX, startY), PointF(endX, endY));
}

int WinGraphicsDrawLines(void* graphics, void* pen, int count, void* points)
{
    return static_cast<Graphics*>(graphics)->DrawLines(static_cast<Pen*>(pen), static_cast<const Point*>(points), count);
}

int WinGraphicsDrawLinesF(void* graphics, void* pen, int count, void* points)
{
    return static_cast<Graphics*>(graphics)->DrawLines(static_cast<Pen*>(pen), static_cast<const PointF*>(points), count);
}

int WinGraphicsDrawString(void* graphics, const char* str, void* font, float x, float y, void* brush)
{
    std::u16string s(ToUtf16(std::string(str)));
    return static_cast<Graphics*>(graphics)->DrawString((const WCHAR*)s.c_str(), s.length(), static_cast<const Font*>(font), PointF(x, y), static_cast<const Brush*>(brush));
}

int WinGraphicsDrawStringFormatPoint(void* graphics, const char* str, void* font, float x, float y, void* format, void* brush)
{
    std::u16string s(ToUtf16(std::string(str)));
    return static_cast<Graphics*>(graphics)->DrawString((const WCHAR*)s.c_str(), s.length(), static_cast<const Font*>(font), PointF(x, y),
        static_cast<const StringFormat*>(format), static_cast<const Brush*>(brush));
}

int WinGraphicsDrawStringFormatRect(void* graphics, const char* str, void* font, float x, float y, float w, float h, void* format, void* brush)
{
    std::u16string s(ToUtf16(std::string(str)));
    return static_cast<Graphics*>(graphics)->DrawString((const WCHAR*)s.c_str(), s.length(), static_cast<const Font*>(font), RectF(x, y, w, h),
        static_cast<const StringFormat*>(format), static_cast<const Brush*>(brush));
}

int WinGraphicsMeasureStringFormatSize(void* graphics, const char* str, void* font, float w, float h, void* format,
    float& outSizeW, float& outSizeH, int* codePointsFitted, int* linesFilled)
{
    std::u16string s(ToUtf16(std::string(str)));
    SizeF size;
    Status status = static_cast<Graphics*>(graphics)->MeasureString((const WCHAR*)s.c_str(), s.length(), static_cast<const Font*>(font), SizeF(w, h),
        static_cast<const StringFormat*>(format), &size, codePointsFitted, linesFilled);
    outSizeW = size.Width;
    outSizeH = size.Height;
    return status;
}

int WinGraphicsMeasureStringFormatRect(void* graphics, const char* str, void* font, float x, float y, float w, float h, void* format,
    float& outX, float& outY, float& outW, float& outH, int* codePointsFitted, int* linesFilled)
{
    std::u16string s(ToUtf16(std::string(str)));
    RectF boundingBox;
    Status status = static_cast<Graphics*>(graphics)->MeasureString((const WCHAR*)s.c_str(), s.length(), static_cast<const Font*>(font), RectF(x, y, w, h),
        static_cast<const StringFormat*>(format), &boundingBox, codePointsFitted, linesFilled);
    outX = boundingBox.X;
    outY = boundingBox.Y;
    outW = boundingBox.Width;
    outH = boundingBox.Height;
    return status;
}

int WinGraphicsMeasureStringFormatPoint(void* graphics, const char* str, void* font, float x, float y, void* format,
    float& outX, float& outY, float& outW, float& outH)
{
    std::u16string s(ToUtf16(std::string(str)));
    RectF boundingBox;
    Status status = static_cast<Graphics*>(graphics)->MeasureString((const WCHAR*)s.c_str(), s.length(), static_cast<const Font*>(font), PointF(x, y), &boundingBox);
    outX = boundingBox.X;
    outY = boundingBox.Y;
    outW = boundingBox.Width;
    outH = boundingBox.Height;
    return status;
}

int WinGraphicsGetTextRenderingHint(void* graphics)
{
    return static_cast<Graphics*>(graphics)->GetTextRenderingHint();
}

int WinGraphicsSetTextRenderingHint(void* graphics, int textRenderingHint)
{
    return static_cast<Graphics*>(graphics)->SetTextRenderingHint(static_cast<TextRenderingHint>(textRenderingHint));
}

int WinGraphicsClear(void* graphics, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    return static_cast<Graphics*>(graphics)->Clear(Color(alpha, red, green, blue));
}

int WinGraphicsDrawRectangle(void* graphics, void* pen, int x, int y, int w, int h)
{
    return static_cast<Graphics*>(graphics)->DrawRectangle(static_cast<const Pen*>(pen), Rect(x, y, w, h));
}

int WinGraphicsDrawRectangleF(void* graphics, void* pen, float x, float y, float w, float h)
{
    return static_cast<Graphics*>(graphics)->DrawRectangle(static_cast<const Pen*>(pen), RectF(x, y, w, h));
}

int WinGraphicsFillRectangle(void* graphics, void* brush, int x, int y, int w, int h)
{
    return static_cast<Graphics*>(graphics)->FillRectangle(static_cast<const Brush*>(brush), Rect(x, y, w, h));
}

int WinGraphicsFillRectangleF(void* graphics, void* brush, float x, float y, float w, float h)
{
    return static_cast<Graphics*>(graphics)->FillRectangle(static_cast<const Brush*>(brush), x, y, w, h);
}

int WinGraphicsFillPolygon(void* graphics, void* brush, int count, void* points)
{
    return static_cast<Graphics*>(graphics)->FillPolygon(static_cast<const Brush*>(brush), static_cast<const Point*>(points), count);
}

int WinGraphicsFillPolygonF(void* graphics, void* brush, int count, void* points)
{
    return static_cast<Graphics*>(graphics)->FillPolygon(static_cast<const Brush*>(brush), static_cast<const PointF*>(points), count);
}

int WinGraphicsDrawArc(void* graphics, void* pen, float x, float y, float w, float h, float startAngle, float sweepAngle)
{
    return static_cast<Graphics*>(graphics)->DrawArc(static_cast<const Pen*>(pen), x, y, w, h, startAngle, sweepAngle);
}

int WinGraphicsDrawEllipse(void* graphics, void* pen, float x, float y, float w, float h)
{
    return static_cast<Graphics*>(graphics)->DrawEllipse(static_cast<const Pen*>(pen), x, y, w, h);
}

int WinGraphicsFillEllipse(void* graphics, void* brush, float x, float y, float w, float h)
{
    return static_cast<Graphics*>(graphics)->FillEllipse(static_cast<const Brush*>(brush), x, y, w, h);
}

void* WinGraphicsCreateRegion()
{
    return new Region();
}

void* WinGraphicsCloneRegion(void* region)
{
    return static_cast<Region*>(region)->Clone();
}

void WinGraphicsDeleteRegion(void* region)
{
    delete static_cast<Region*>(region);
}

int WinGraphicsRegionGetLastStatus(void* region)
{
    return static_cast<Region*>(region)->GetLastStatus();
}

int WinGraphicsGetClip(void* graphics, void* region)
{
    return static_cast<Graphics*>(graphics)->GetClip(static_cast<Region*>(region));
}

int WinGraphicsSetClipRect(void* graphics, int x, int y, int w, int h, int combineMode)
{
    return static_cast<Graphics*>(graphics)->SetClip(Rect(x, y, w, h), static_cast<CombineMode>(combineMode));
}

int WinGraphicsSetClipRegion(void* graphics, void* region)
{
    return static_cast<Graphics*>(graphics)->SetClip(static_cast<Region*>(region));
}

void* WinGraphicsCreateImage(const char* fileName, bool useEmbeddedColorManagement)
{
    std::u16string fname(ToUtf16(fileName));
    Image* image = new Image((const WCHAR*)fname.c_str(), useEmbeddedColorManagement);
    return image;
}

int WinGraphicsImageGetLastStatus(void* image)
{
    return static_cast<Image*>(image)->GetLastStatus();
}

void* WinGraphicsCloneImage(void* image)
{
    return static_cast<Image*>(image)->Clone();
}

void WinGraphicsDeleteImage(void* image)
{
    delete static_cast<Image*>(image);
}

uint32_t WinGraphicsImageGetHeight(void* image)
{
    return static_cast<Image*>(image)->GetHeight();
}

uint32_t WinGraphicsImageGetWidth(void* image)
{
    return static_cast<Image*>(image)->GetWidth();
}

void* WinGraphicsCreateBitmap(const char* fileName, bool useEmbeddedColorManagement)
{
    std::u16string fname(ToUtf16(fileName));
    return new Bitmap((const WCHAR*)fname.c_str(), useEmbeddedColorManagement);
}

void* WinGraphicsCreateBitmapWidthHeight(int w, int h, void* graphics)
{
    return new Bitmap(w, h, static_cast<Graphics*>(graphics));
}

void* WinGraphicsCreateBitmapWinBitmap(void* winBitmap, void* palette)
{
    return new Bitmap((HBITMAP)winBitmap, (HPALETTE)palette);
}

void* WinGraphicsCreateBitmapIcon(void* icon)
{
    return new Bitmap((HICON)icon);
}

void* WinGraphicsCreateBitmapResource(const char* resourceName)
{
    std::u16string rname = ToUtf16(resourceName);
    return Bitmap::FromResource(applicationInstance, (const WCHAR*)rname.c_str());
}

void* WinGraphicsCloneBitmap(void* nativeBitmap, int x, int y, int w, int h, int pixelFormat)
{
    Gdiplus::PixelFormat format = static_cast<Gdiplus::PixelFormat>(pixelFormat);
    return static_cast<Bitmap*>(nativeBitmap)->Clone(x, y, w, h, format);
}

int WinGraphicsBitmapGetPixel(void* nativeBitmap, int x, int y, uint8_t& alpha, uint8_t& red, uint8_t& green, uint8_t& blue)
{
    Gdiplus::Color color;
    int status = static_cast<Bitmap*>(nativeBitmap)->GetPixel(x, y, &color);
    alpha = color.GetAlpha();
    red = color.GetRed();
    green = color.GetGreen();
    blue = color.GetBlue();
    return status;
}

int WinGraphicsBitmapSetPixel(void* nativeBitmap, int x, int y, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    Gdiplus::Color color(alpha, red, green, blue);
    return static_cast<Bitmap*>(nativeBitmap)->SetPixel(x, y, color);
}

uint32_t WinGraphicsSave(void* graphics)
{
    return static_cast<Graphics*>(graphics)->Save();
}

int WinGraphicsRestore(void* graphics, uint32_t graphicsState)
{
    return static_cast<Graphics*>(graphics)->Restore(graphicsState);
}

void* WinGraphicsCreateDefaultImageAttributes()
{
    return new ImageAttributes();
}

int WinGraphicsImageAttributesGetLastStatus(void* imageAttributes)
{
    return static_cast<ImageAttributes*>(imageAttributes)->GetLastStatus();
}

void* WinGraphicsCloneImageAttributes(void* imageAttributes)
{
    return static_cast<ImageAttributes*>(imageAttributes)->Clone();
}

void WinGraphicsDeleteImageAttributes(void* imageAttributes)
{
    delete static_cast<ImageAttributes*>(imageAttributes);
}

int WinGraphicsImageAttributesSetColorKey(void* imageAttributes, uint8_t colorLowAlpha, uint8_t colorLowRed, uint8_t colorLowGreen, uint8_t colorLowBlue,
    uint8_t colorHighAlpha, uint8_t colorHighRed, uint8_t colorHighGreen, uint8_t colorHighBlue, int colorAdjustType)
{
    return static_cast<ImageAttributes*>(imageAttributes)->SetColorKey(Color(colorLowAlpha, colorLowRed, colorLowGreen, colorLowBlue), Color(colorHighAlpha, colorHighRed, colorHighGreen, colorHighBlue),
        static_cast<ColorAdjustType>(colorAdjustType));
}

int WinGraphicsDrawImagePoint(void* graphics, void* image, float x, float y)
{
    return static_cast<Graphics*>(graphics)->DrawImage(static_cast<Image*>(image), PointF(x, y));
}

int WinGraphicsDrawImageRect(void* graphics, void* image, float x, float y, float w, float h)
{
    return static_cast<Graphics*>(graphics)->DrawImage(static_cast<Image*>(image), RectF(x, y, w, h));
}

int WinGraphicsDrawImageWithAttributes(void* graphics, void* image, int destRectX, int destRectY, int destRectW, int destRectH, int srcX, int srcY, int srcW, int srcH,
    int srcUnit, void* imageAttributes)
{
    return static_cast<Graphics*>(graphics)->DrawImage(static_cast<Image*>(image), Rect(destRectX, destRectY, destRectW, destRectH), srcX, srcY, srcW, srcH, static_cast<Unit>(srcUnit),
        static_cast<ImageAttributes*>(imageAttributes), nullptr, nullptr);
}

int WinGraphicsDrawImageWithAttributesF(void* graphics, void* image, float destRectX, float destRectY, float destRectW, float destRectH, float srcX, float srcY, float srcW, float srcH,
    int srcUnit, void* imageAttributes)
{
    return static_cast<Graphics*>(graphics)->DrawImage(static_cast<Image*>(image), RectF(destRectX, destRectY, destRectW, destRectH), srcX, srcY, srcW, srcH, static_cast<Unit>(srcUnit),
        static_cast<ImageAttributes*>(imageAttributes), nullptr, nullptr);
}

int WinGraphicsGetTransform(void* graphics, void* matrix)
{
    return static_cast<Graphics*>(graphics)->GetTransform(static_cast<Matrix*>(matrix));
}

int WinGraphicsSetTransform(void* graphics, void* matrix)
{
    return static_cast<Graphics*>(graphics)->SetTransform(static_cast<const Matrix*>(matrix));
}

int WinGraphicsMultiplyTransform(void* graphics, void* matrix, int order)
{
    return static_cast<Graphics*>(graphics)->MultiplyTransform(static_cast<const Matrix*>(matrix), static_cast<MatrixOrder>(order));
}

int WinGraphicsResetTransform(void* graphics)
{
    return static_cast<Graphics*>(graphics)->ResetTransform();
}

int WinGraphicsRotateTransform(void* graphics, float angle, int order)
{
    return static_cast<Graphics*>(graphics)->RotateTransform(angle, static_cast<MatrixOrder>(order));
}

int WinGraphicsScaleTransform(void* graphics, float scaleX, float scaleY, int order)
{
    return static_cast<Graphics*>(graphics)->ScaleTransform(scaleX, scaleY, static_cast<MatrixOrder>(order));
}

int WinGraphicsTranslateTransform(void* graphics, float offsetX, float offsetY, int order)
{
    return static_cast<Graphics*>(graphics)->TranslateTransform(offsetX, offsetY, static_cast<MatrixOrder>(order));
}

void* WinGraphicsCreateMatrixFromElements(float m11, float m12, float m21, float m22, float dx, float dy)
{
    return new Matrix(m11, m12, m21, m22, dx, dy);
}

void* WinGraphicsCreateMatrix()
{
    return new Matrix();
}

void* WinGraphicsCreateMatrixRectFPointF(float rectX, float rectY, float rectW, float rectH, float ptX, float ptY)
{
    RectF rect(PointF(rectX, rectY), SizeF(rectW, rectH));
    PointF pt(ptX, ptY);
    return new Matrix(rect, &pt);
}

void* WinGraphicsCreateMatrixRectPoint(int rectX, int rectY, int rectW, int rectH, int ptX, int ptY)
{
    Rect rect(Point(rectX, rectY), Size(rectW, rectH));
    Point pt(ptX, ptY);
    return new Matrix(rect, &pt);
}

int WinGraphicsMatrixGetLastStatus(void* matrix)
{
    return static_cast<Matrix*>(matrix)->GetLastStatus();
}

void* WinGraphicsCloneMatrix(void* matrix)
{
    return static_cast<Matrix*>(matrix)->Clone();
}

void WinGraphicsDeleteMatrix(void* matrix)
{
    delete static_cast<Matrix*>(matrix);
}

int WinGraphicsMultiplyMatrix(void* matrix, void* factorMatrix, int order)
{
    return static_cast<Matrix*>(matrix)->Multiply(static_cast<const Matrix*>(factorMatrix), static_cast<MatrixOrder>(order));
}

int WinGraphicsResetMatrix(void* matrix)
{
    return static_cast<Matrix*>(matrix)->Reset();
}

int WinGraphicsInvertMatrix(void* matrix)
{
    return static_cast<Matrix*>(matrix)->Invert();
}

float WinGraphicsMatrixOffsetX(void* matrix)
{
    return static_cast<Matrix*>(matrix)->OffsetX();
}

float WinGraphicsMatrixOffsetY(void* matrix)
{
    return static_cast<Matrix*>(matrix)->OffsetY();
}

int WinGraphicsMatrixRotate(void* matrix, float angle, int order)
{
    return static_cast<Matrix*>(matrix)->Rotate(angle, static_cast<MatrixOrder>(order));
}

int WinGraphicsMatrixRotateAt(void* matrix, float angle, float centerX, float centerY, int order)
{
    PointF center(centerX, centerY);
    return static_cast<Matrix*>(matrix)->RotateAt(angle, center, static_cast<MatrixOrder>(order));
}

int WinGraphicsMatrixScale(void* matrix, float scaleX, float scaleY, int order)
{
    return static_cast<Matrix*>(matrix)->Scale(scaleX, scaleY, static_cast<MatrixOrder>(order));
}

int WinGraphicsMatrixSetElements(void* matrix, float m11, float m12, float m21, float m22, float dx, float dy)
{
    return static_cast<Matrix*>(matrix)->SetElements(m11, m12, m21, m22, dx, dy);
}

int WinGraphicsMatrixShear(void* matrix, float shearX, float shearY, int order)
{
    return static_cast<Matrix*>(matrix)->Shear(shearX, shearY, static_cast<MatrixOrder>(order));
}

int WinGraphicsMatrixTranslate(void* matrix, float offsetX, float offsetY, int order)
{
    return static_cast<Matrix*>(matrix)->Translate(offsetX, offsetY, static_cast<MatrixOrder>(order));
}

int WinGraphicsMatrixTransformPoints(void* matrix, void* points, int numPoints)
{
    return static_cast<Matrix*>(matrix)->TransformPoints(static_cast<Point*>(points), numPoints);
}

int WinGraphicsMatrixTransformPointsF(void* matrix, void* points, int numPoints)
{
    return static_cast<Matrix*>(matrix)->TransformPoints(static_cast<PointF*>(points), numPoints);
}

int WinGraphicsGetPageUnit(void* graphics)
{
    return static_cast<int>(static_cast<Graphics*>(graphics)->GetPageUnit());
}

int WinGraphicsSetPageUnit(void* graphics, int unit)
{
    return static_cast<Graphics*>(graphics)->SetPageUnit(static_cast<Unit>(unit));
}

float WinGraphicsGetPageScale(void* graphics)
{
    return static_cast<Graphics*>(graphics)->GetPageScale();
}

int WinGraphicsSetPageScale(void* graphics, float scale)
{
    return static_cast<Graphics*>(graphics)->SetPageScale(scale);
}

float WinGraphicsGetDpiX(void* graphics)
{
    return static_cast<Graphics*>(graphics)->GetDpiX();
}

float WinGraphicsGetDpiY(void* graphics)
{
    return static_cast<Graphics*>(graphics)->GetDpiY();
}

int WinGraphicsGetSmoothingMode(void* graphics)
{
    return static_cast<Graphics*>(graphics)->GetSmoothingMode();
}

int WinGraphicsSetSmoothingMode(void* graphics, int smoothingMode)
{
    return static_cast<Graphics*>(graphics)->SetSmoothingMode(static_cast<SmoothingMode>(smoothingMode));
}

// image formats: image/bmp, image/jpeg, image/gif, image/tiff, image/png

int WinGraphicsGetEncoderClsId(const char* imageFormat, void* clsid)
{
    std::u16string imageFormatStr = ToUtf16(imageFormat);
    ImageCodecInfo* imageCodecInfo = nullptr;
    uint32_t n = 0;
    uint32_t size = 0;
    int result = GetImageEncodersSize(&n, &size);
    if (result != 0 || size == 0)
    {
        return result;
    }
    imageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (!imageCodecInfo) return 3;
    result = GetImageEncoders(n, size, imageCodecInfo);
    if (result != 0)
    {
        return result;
    }
    for (uint32_t i = 0; i < n; ++i)
    {
        std::u16string mimetype = reinterpret_cast<const char16_t*>(imageCodecInfo[i].MimeType);
        if (mimetype == imageFormatStr)
        {
            CLSID guid = imageCodecInfo[i].Clsid;
            memcpy(clsid, &guid, sizeof(guid));
            free(imageCodecInfo);
            return 0;
        }
    }
    free(imageCodecInfo);
    return -1;
}

int WinGraphicsImageSave(void* image, const char* fileName, const void* encoderClsId)
{
    std::u16string fileNameStr = ToUtf16(fileName);
#ifdef __MINGW32__
    return static_cast<Image*>(image)->Save((const WCHAR*)fileNameStr.c_str(), static_cast<const CLSID*>(encoderClsId), nullptr);
#else
    return static_cast<Image*>(image)->Save((const WCHAR*)fileNameStr.c_str(), static_cast<const CLSID*>(encoderClsId));
#endif
}

void WinGetSysColor(int index, uint8_t& red, uint8_t& green, uint8_t& blue)
{
    uint32_t rgb = GetSysColor(index);
    red = GetRValue(rgb);
    green = GetGValue(rgb);
    blue = GetBValue(rgb);
}

bool WinSetBkColor(void* hdc, uint8_t red, uint8_t green, uint8_t blue)
{
    COLORREF color = RGB(red, green, blue);
    COLORREF col = SetBkColor((HDC)hdc, color);
    if (col == CLR_INVALID)
    {
        return false;
    }
    return true;
}

void WinGetMessagePos(int32_t& x, int32_t& y)
{
    DWORD pos = GetMessagePos();
    x = pos & 0xFFFF;
    y = (pos >> 16) & 0xFFFF;
}

void* WinGetFocus()
{
    return GetFocus();
}

void* WinSetFocus(void* windowHandle)
{
    return SetFocus((HWND)windowHandle);
}

void* WinGetTopWindow(void* windowHandle)
{
    return GetTopWindow((HWND)windowHandle);
}

bool WinBringWindowToTop(void* windowHandle)
{
    return BringWindowToTop((HWND)windowHandle);
}

bool WinSetWindowPos(void* windowHandle, void* insertAfterWindowHandle, int x, int y, int cx, int cy, uint32_t flags)
{
    return SetWindowPos((HWND)windowHandle, (HWND)insertAfterWindowHandle, x, y, cx, cy, flags);
}

bool WinGetOpenFileName(void* windowHandle, const char16_t* filter, const char16_t* initialDirectory, char16_t* fileNameBuffer, uint32_t fileNameBufferSize, uint32_t flags, const char16_t* defaultExtension)
{
    OPENFILENAMEW openFileName;
    openFileName.lStructSize = sizeof(openFileName);
    openFileName.hwndOwner = (HWND)windowHandle;
    openFileName.hInstance = applicationInstance;
    openFileName.lpstrFilter = (LPCWSTR)filter;
    openFileName.lpstrCustomFilter = nullptr;
    openFileName.nMaxCustFilter = 0;
    openFileName.nFilterIndex = 0;
    openFileName.lpstrFile = reinterpret_cast<LPWSTR>(fileNameBuffer);
    openFileName.nMaxFile = fileNameBufferSize;
    openFileName.lpstrFileTitle = nullptr;
    openFileName.nMaxFileTitle = 0;
    openFileName.lpstrInitialDir = (LPCWSTR)initialDirectory;
    openFileName.lpstrTitle = nullptr;
    openFileName.Flags = flags;
    openFileName.nFileOffset = 0;
    openFileName.nFileExtension = 0;
    openFileName.lpstrDefExt = (LPCWSTR)defaultExtension;
    openFileName.lCustData = 0;
    openFileName.lpfnHook = nullptr;
    openFileName.lpTemplateName = nullptr;
    openFileName.pvReserved = nullptr;
    openFileName.dwReserved = 0;
    openFileName.FlagsEx = 0;
    return GetOpenFileNameW(&openFileName);
}

bool WinGetSaveFileName(void* windowHandle, const char16_t* filter, const char16_t* initialDirectory, char16_t* fileNameBuffer, uint32_t fileNameBufferSize, uint32_t flags, const char16_t* defaultExtension)
{
    OPENFILENAMEW openFileName;
    openFileName.lStructSize = sizeof(openFileName);
    openFileName.hwndOwner = (HWND)windowHandle;
    openFileName.hInstance = applicationInstance;
    openFileName.lpstrFilter = (LPCWSTR)filter;
    openFileName.lpstrCustomFilter = nullptr;
    openFileName.nMaxCustFilter = 0;
    openFileName.nFilterIndex = 0;
    openFileName.lpstrFile = reinterpret_cast<LPWSTR>(fileNameBuffer);
    openFileName.nMaxFile = fileNameBufferSize;
    openFileName.lpstrFileTitle = nullptr;
    openFileName.nMaxFileTitle = 0;
    openFileName.lpstrInitialDir = (LPCWSTR)initialDirectory;
    openFileName.lpstrTitle = nullptr;
    openFileName.Flags = flags;
    openFileName.nFileOffset = 0;
    openFileName.nFileExtension = 0;
    openFileName.lpstrDefExt = (LPCWSTR)defaultExtension;
    openFileName.lCustData = 0;
    openFileName.lpfnHook = nullptr;
    openFileName.lpTemplateName = nullptr;
    openFileName.pvReserved = nullptr;
    openFileName.dwReserved = 0;
    openFileName.FlagsEx = 0;
    return GetSaveFileNameW(&openFileName);
}

uint32_t WinCommDlgExtendedError()
{
    return CommDlgExtendedError();
}

void* WinLoadImage(const char* name, uint32_t type, int32_t cx, int32_t cy)
{
    std::u16string wname(ToUtf16(name));
    uint32_t fuLoad = 0;
    if (cx == 0 && cy == 0)
    {
        fuLoad = LR_DEFAULTSIZE;
    }
    return LoadImageW(applicationInstance, (LPCWSTR)wname.c_str(), type, cx, cy, fuLoad);
}

void* WinLoadStandardCursor(int cursorId)
{
    HCURSOR cursor = LoadCursor(nullptr, MAKEINTRESOURCE(cursorId));
    return cursor;
}

void* WinLoadCursor(const char* cursorName)
{
    return WinLoadImage(cursorName, IMAGE_CURSOR, 0, 0);
}

void* WinGetCursor()
{
    return GetCursor();
}

void* WinSetCursor(void* cursorHandle)
{
    return SetCursor((HCURSOR)cursorHandle);
}

bool WinDestroyCursor(void* cursorHandle)
{
    return DestroyCursor((HCURSOR)cursorHandle);
}

void* WinLoadStandardIcon(int iconId)
{
    HICON icon = LoadIcon(nullptr, MAKEINTRESOURCE(iconId));
    return icon;
}

void* WinLoadIcon(const char* iconName)
{
    return WinLoadImage(iconName, IMAGE_ICON, 0, 0);
}

bool WinDestroyIcon(void* iconHandle)
{
    return DestroyIcon((HICON)iconHandle);
}

void* WinLoadStandardBitmap(int bitmapId)
{
    HBITMAP bitmap = LoadBitmap(nullptr, MAKEINTRESOURCE(bitmapId));
    return bitmap;
}

void* WinLoadBitmap(const char* bitmapName)
{
    return WinLoadImage(bitmapName, IMAGE_BITMAP, 0, 0);
}

bool WinDestroyBitmap(void* bitmapHandle)
{
    return DeleteObject((HGDIOBJ)bitmapHandle);
}

bool WinDeleteObject(void* handle)
{
    return DeleteObject((HGDIOBJ)handle);
}

int WinGetSystemMetrics(int index)
{
    return GetSystemMetrics(index);
}

void* WinSetCapture(void* windowHandle)
{
    return SetCapture((HWND)windowHandle);
}

bool WinReleaseCapture()
{
    return ReleaseCapture();
}

int64_t WinGetClassLong(void* windowHandle, int index)
{
    return GetClassLongPtr((HWND)windowHandle, index);
}

int64_t WinSetClassLong(void* windowHandle, int index, int64_t value)
{
    return SetClassLongPtr((HWND)windowHandle, index, value);
}

int64_t WinGetWindowLong(void* windowHandle, int index)
{
    LONG_PTR result = GetWindowLongPtrW((HWND)windowHandle, index);
    return (int64_t)result;
}

bool WinSetWindowLong(void* windowHandle, int index, int64_t newValue)
{
    LONG_PTR result = SetWindowLongPtrW((HWND)windowHandle, index, (LONG_PTR)newValue);
    return result != 0;
}

bool WinScrollWindow(void* windowHandle, int xAmount, int yAmount, int clientLocX, int clientLocY, int clientSizeW, int clientSizeH, int clipLocX, int clipLocY, int clipSizeW, int clipSizeH)
{
    const RECT* pclientRect = nullptr;
    RECT clientRect;
    if (clientSizeW != 0 && clientSizeH != 0)
    {
        clientRect.left = clientLocX;
        clientRect.top = clientLocY;
        clientRect.right = clientLocX + clientSizeW;
        clientRect.bottom = clientLocY + clientSizeH;
        pclientRect = &clientRect;
    }
    const RECT* pclipRect = nullptr;
    RECT clipRect;
    if (clipSizeW != 0 && clipSizeH != 0)
    {
        clipRect.left = clipLocX;
        clipRect.top = clipLocY;
        clipRect.right = clipLocX + clipSizeW;
        clipRect.bottom = clipLocY + clipSizeH;
    }
    return ScrollWindow((HWND)windowHandle, xAmount, yAmount, pclientRect, pclipRect);
}

bool WinGetScrollInfo(void* windowHandle, int nBar, uint32_t& nPage, int32_t& nPos, int32_t& nMin, int32_t& nMax, int32_t& nTrackPos)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
    bool succeeded = GetScrollInfo((HWND)windowHandle, nBar, &si);
    if (succeeded)
    {
        nPage = si.nPage;
        nPos = si.nPos;
        nMin = si.nMin;
        nMax = si.nMax;
        nTrackPos = si.nTrackPos;
    }
    return succeeded;
}

int WinSetScrollInfo(void* windowHandle, int nBar, uint32_t fMask, bool redraw, uint32_t nPage, int32_t nPos, int32_t nMin, int32_t nMax)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = fMask;
    si.nPage = nPage;
    si.nPos = nPos;
    si.nMin = nMin;
    si.nMax = nMax;
    return SetScrollInfo((HWND)windowHandle, nBar, &si, redraw);
}

int WinGetScrollPos(void* windowHandle, int nBar)
{
    return GetScrollPos((HWND)windowHandle, nBar);
}

bool WinShowScrollBar(void* windowHandle, int nBar, bool show)
{
    return ShowScrollBar((HWND)windowHandle, nBar, show);
}

bool WinCreateCaret(void* windowHandle, void* bitmapHandle, int width, int height)
{
    return CreateCaret((HWND)windowHandle, (HBITMAP)bitmapHandle, width, height);
}

bool WinDestroyCaret()
{
    return DestroyCaret();
}

bool WinShowCaret(void* windowHandle)
{
    return ShowCaret((HWND)windowHandle);
}

bool WinHideCaret(void* windowHandle)
{
    return HideCaret((HWND)windowHandle);
}

bool WinGetCaretPos(int& x, int& y)
{
    POINT pt;
    bool retval = GetCaretPos(&pt);
    x = pt.x;
    y = pt.y;
    return retval;
}

bool WinSetCaretPos(int x, int y)
{
    return SetCaretPos(x, y);
}

bool WinSetTimer(void* windowHandle, uint32_t timerId, uint32_t elapse)
{
    int retval = SetTimer((HWND)windowHandle, timerId, elapse, nullptr);
    return retval != 0;
}

bool WinKillTimer(void* windowHandle, uint32_t timerId)
{
    return KillTimer((HWND)windowHandle, timerId);
}

bool WinRegOpenCurrentUser(void** result)
{
    LSTATUS status = RegOpenCurrentUser(KEY_READ, (PHKEY)result);
    return status == ERROR_SUCCESS;
}

bool WinRegCloseKey(void* key)
{
    LSTATUS status = RegCloseKey((HKEY)key);
    return status == ERROR_SUCCESS;
}

bool WinRegGetDWordValue(void* key, const char* subKey, const char* valueName, uint32_t& value)
{
    std::u16string subkeyStr = ToUtf16(subKey);
    std::u16string valueNameStr = ToUtf16(valueName);
    uint32_t flags = RRF_RT_REG_DWORD;
    uint32_t type = 0u;
    uint32_t data = 0u;
    uint32_t dataSize = 4u;
    LSTATUS status = RegGetValueW((HKEY)key, (LPCWSTR)subkeyStr.c_str(), (LPCWSTR)valueNameStr.c_str(), flags, (LPDWORD)&type, &data, (LPDWORD)&dataSize);
    if (status == ERROR_SUCCESS)
    {
        value = data;
    }
    return status == ERROR_SUCCESS;
}

uint32_t WinRegisterClipboardFormat(const char* formatName)
{
    std::u16string format = ToUtf16(formatName);
    return RegisterClipboardFormatW((LPCWSTR)format.c_str());
}

bool WinOpenClipboard(void* windowHandle)
{
    return OpenClipboard((HWND)windowHandle);
}

bool WinCloseClipboard()
{
    return CloseClipboard();
}

bool WinEmptyClipboard()
{
    return EmptyClipboard();
}

void* WinSetClipboardData(uint32_t format, void* mem)
{
    return SetClipboardData(format, (HANDLE)mem);
}

void* WinGetClipboardData(uint32_t format)
{
    return GetClipboardData(format);
}

bool WinIsClipboardFormatAvailable(uint32_t format)
{
    return IsClipboardFormatAvailable(format);
}

bool WinAddClipboardFormatListener(void* windowHandle)
{
    return AddClipboardFormatListener((HWND)windowHandle);
}

bool WinRemoveClipboardFormatListener(void* windowHandle)
{
    return RemoveClipboardFormatListener((HWND)windowHandle);
}

void* WinGlobalAlloc(uint32_t flags, uint64_t size)
{
    return GlobalAlloc(flags, size);
}

void* WinGlobalLock(void* memHandle)
{
    return GlobalLock((HGLOBAL)memHandle);
}

bool WinGlobalUnlock(void* memHandle)
{
    return GlobalUnlock((HGLOBAL)memHandle);
}

void* WinGlobalFree(void* memHandle)
{
    return GlobalFree((HGLOBAL)memHandle);
}

uint64_t WinGlobalSize(void* memHandle)
{
    return GlobalSize((HGLOBAL)memHandle);
}

bool WinGetCursorPos(int& x, int& y)
{
    POINT pt;
    if (GetCursorPos(&pt))
    {
        x = pt.x;
        y = pt.y;
        return true;
    }
    return false;
}

int WinGetCurrentProcessId()
{
    return soulng::util::GetPid();
}

#ifdef __MINGW32__

bool WinGetFolder(void* windowHandle, const char16_t* defaultDirectory, char16_t* folderNameBuffer, uint32_t folderNameBufferSize)
{
    return false;
}

#else

bool WinGetFolder(void* windowHandle, const char16_t* defaultDirectory, char16_t* folderNameBuffer, uint32_t folderNameBufferSize)
{
    IFileDialog* fileDialog = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog))))
    {
        IShellItem* defaultFolder = nullptr;
        if (defaultDirectory && *defaultDirectory != '\0')
        {
            if (SUCCEEDED(SHCreateItemFromParsingName((PCWSTR)defaultDirectory, nullptr, IID_IShellItem, (void**)&defaultFolder)))
            {
                if (!SUCCEEDED(fileDialog->SetDefaultFolder(defaultFolder)))
                {
                    fileDialog->Release();
                    return false;
                }
            }
        }
        DWORD options = 0;
        if (SUCCEEDED(fileDialog->GetOptions(&options)))
        {
            if (SUCCEEDED(fileDialog->SetOptions(options | FOS_PICKFOLDERS)))
            {
                if (SUCCEEDED(fileDialog->Show(HWND(windowHandle))))
                {
                    IShellItem* result = nullptr;
                    if (SUCCEEDED(fileDialog->GetResult(&result)))
                    {
                        LPWSTR name = nullptr;
                        if (SUCCEEDED(result->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &name)))
                        {
                            wcsncpy((wchar_t*)folderNameBuffer, name, folderNameBufferSize);
                            fileDialog->Release();
                            result->Release();
                            CoTaskMemFree(name);
                            return true;
                        }
                        else
                        {
                            result->Release();
                            fileDialog->Release();
                            return false;
                        }
                    }
                    else
                    {
                        fileDialog->Release();
                        return false;
                    }
                }
                else
                {
                    fileDialog->Release();
                    return false;
                }
            }
            else
            {
                fileDialog->Release();
                return false;
            }
        }
        else
        {
            fileDialog->Release();
            return false;
        }
    }
    else
    {
        return false;
    }
}

#endif
