// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/WindowsAPI.hpp>
#include <soulng/util/Unicode.hpp>
#include <Windows.h>
#include <map>
#include <string>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

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
    int code = (int)ShellExecuteA(NULL, "open", filePath, NULL, NULL, SW_SHOWNORMAL);
    if (code > 32)
    {
        errorCode = 0;
        return true;
    }
    errorCode = MapWinCodeToExtCode(code);
    return false;
}

typedef bool (*messageProcessorFunction)(void* windowHandle, uint32_t message, uint32_t wparam, int64_t lparam, int64_t& result);

messageProcessorFunction messageProcessor = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int64_t result = 0;
    bool handled = messageProcessor(hWnd, message, wParam, lParam, result);
    if (!handled)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return result;
}

HINSTANCE applicationInstance = nullptr;

void WinSetInstance()
{
    applicationInstance = GetModuleHandle(nullptr);
}

ULONG_PTR gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

int WinInit(void* messageProcessorFunctionAddress)
{
    messageProcessor = static_cast<messageProcessorFunction>(messageProcessorFunctionAddress);
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
        DispatchMessage(&msg);
    }
    return msg.wParam;
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
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(backgroundColor + 1);
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
    return MessageBoxW((HWND)ownerWindowHandle, (LPCWSTR)str.c_str(), captionStr, type);
}

void* WinCreateWindowByClassAtom(uint16_t windowClass, const char* windowName, int64_t style, int64_t exStyle, int x, int y, int w, int h, void* parentHandle)
{
    std::u16string name = ToUtf16(windowName);
    HWND handle = CreateWindowExW(exStyle, (LPCWSTR)windowClass, (LPCWSTR)name.c_str(), style, x, y, w, h, (HWND)parentHandle, nullptr, applicationInstance, nullptr);
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
    delete paintStruct;
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
    return static_cast<FontFamily*>(fontFamily)->GetLastStatus();
}

void* WinGraphicsCreateFont(const void* fontFamily, float emSize, int style, int unit)
{
    return new Font(static_cast<const FontFamily*>(fontFamily), emSize, style, static_cast<Gdiplus::Unit>(unit));
}

void* WinGraphicsCloneFont(void* font)
{
    return static_cast<Font*>(font)->Clone();
}

void WinGraphicsDeleteFont(void* font)
{
    delete static_cast<Font*>(font);
}

float WinGraphicsGetFontHeight(void* font, const void* graphics)
{
    return static_cast<Font*>(font)->GetHeight(static_cast<const Graphics*>(graphics));
}

int WinGraphicsDrawLine(void* graphics, void* pen, int startX, int startY, int endX, int endY)
{
    return static_cast<Graphics*>(graphics)->DrawLine(static_cast<Pen*>(pen), Point(startX, startY), Point(endX, endY));
}

int WinGraphicsDrawString(void* graphics, const char* str, void* font, float x, float y, void* brush)
{
    std::u16string s(ToUtf16(std::string(str)));
    return static_cast<Graphics*>(graphics)->DrawString((const WCHAR*)s.c_str(), s.length(), static_cast<const Font*>(font), PointF(x, y), static_cast<const Brush*>(brush));
}

int WinGraphicsClear(void* graphics, uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    return static_cast<Graphics*>(graphics)->Clear(Color(alpha, red, green, blue));
}
