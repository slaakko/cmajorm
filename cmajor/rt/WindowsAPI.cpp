// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/WindowsAPI.hpp>
#include <soulng/util/Unicode.hpp>
#include <Windows.h>
#include <map>
#include <string>

using namespace soulng::unicode;
using namespace soulng::util;

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

void WinSetMessageProcessorFunctionAddress(void* messageProcessorFunctionAddress)
{
    messageProcessor = static_cast<messageProcessorFunction>(messageProcessorFunctionAddress);
}

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

void Tester(HINSTANCE instance)
{
    char16_t className[] = u"TesterApp";
    char16_t title[] = u"Tester";
    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(instance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = (LPWSTR)(&className[0]);
    wc.hIconSm = LoadIcon(instance, IDI_APPLICATION);
    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, (LPWSTR)u"register class failed", (LPWSTR)u"TesterApp", MB_OK);
        return;
    }
    HWND hWnd = CreateWindow(
        (LPWSTR)className,
        (LPWSTR)title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        instance,
        NULL
    );
    if (!hWnd)
    {
        MessageBox(nullptr, (LPWSTR)u"create window failed", (LPWSTR)u"TesterApp", MB_OK);
        return;
    }
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);
}

HINSTANCE applicationInstance = nullptr;

void WinSetInstance()
{
    applicationInstance = GetModuleHandle(nullptr);
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

void WinShowMessageBox(const char* text, const char* caption)
{
    std::u16string str = ToUtf16(text);
    LPCWSTR captionStr = nullptr;
    std::u16string cap;
    if (caption != nullptr)
    {
        cap = ToUtf16(caption);
        captionStr = (LPCWSTR)cap.c_str();
    }
    MessageBoxW(nullptr, (LPCWSTR)str.c_str(), captionStr, MB_OK);
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
