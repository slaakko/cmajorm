// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/Graphics.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

HINSTANCE instance;

HINSTANCE Instance()
{
    return instance;
}

std::string WindowsErrorMessage(uint64_t errorCode)
{
    char16_t buf[4096];
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, LANG_SYSTEM_DEFAULT, (LPWSTR)buf, 4096, nullptr);
    return ToUtf8(buf);
}

WindowsException::WindowsException(uint64_t errorCode_) : std::runtime_error(WindowsErrorMessage(errorCode_)), errorCode(errorCode_)
{
}

MessageProcessorFunction messageProcessor;

void SetMessageProcessorFunction(MessageProcessorFunction messageProcessorFun)
{
    messageProcessor = messageProcessorFun;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    bool handled = messageProcessor(hWnd, message, wParam, lParam, result);
    if (!handled)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return result;
}

WNDPROC GetWndProc()
{
    return WndProc;
}

bool KeyPressed(int virtualKeyCode)
{
    return (GetKeyState(virtualKeyCode) & 0x8000) != 0;
}

KeyPreviewFunction keyPreview;

void SetKeyPreviewFunction(KeyPreviewFunction keyPreviewFun)
{
    keyPreview = keyPreviewFun;
}

int Run()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        bool handled = false;
        if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP)
        {
            WPARAM keyCode = msg.wParam;
            if (keyPreview)
            {
                KeyState keyState = KeyState::none;
                if (msg.message == WM_KEYDOWN)
                {
                    keyState = keyState | KeyState::down;
                }
                short shiftState = GetKeyState(VK_SHIFT);
                bool shift = (shiftState & (1 << 16)) != 0;
                if (shift)
                {
                    keyState = keyState | KeyState::shift;
                }
                short controlState = GetKeyState(VK_CONTROL);
                bool control = (controlState & (1 << 16)) != 0;
                if (control)
                {
                    keyState = keyState | KeyState::control;
                }
                short altState = GetKeyState(VK_MENU);
                bool alt = (altState & (1 << 16)) != 0;
                if (alt)
                {
                    keyState = keyState | KeyState::alt;
                }
                keyPreview(keyCode, keyState, handled);
            }
        }
        if (!handled)
        {
            DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}

int MessageLoop()
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
                    if (keyPreview)
                    {
                        KeyState keyState = KeyState::none;
                        if (msg.message == WM_KEYDOWN)
                        {
                            keyState = keyState | KeyState::down;
                        }
                        short shiftState = GetKeyState(VK_SHIFT);
                        bool shift = (shiftState & (1 << 16)) != 0;
                        if (shift)
                        {
                            keyState = keyState | KeyState::shift;
                        }
                        short controlState = GetKeyState(VK_CONTROL);
                        bool control = (controlState & (1 << 16)) != 0;
                        if (control)
                        {
                            keyState = keyState | KeyState::control;
                        }
                        short altState = GetKeyState(VK_MENU);
                        bool alt = (altState & (1 << 16)) != 0;
                        if (alt)
                        {
                            keyState = keyState | KeyState::alt;
                        }
                        keyPreview(keyCode, keyState, handled);
                    }
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

ULONG_PTR gdiplusToken;
Gdiplus::GdiplusStartupInput gdiplusStartupInput;

void WingInit(HINSTANCE instance_)
{
    instance = instance_;
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    if (status != Gdiplus::Status::Ok)
    {
        throw GdiPlusException(status);
    }
}

void WingDone()
{
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

Color GetSystemColor(int index)
{
    DWORD sysColor = GetSysColor(index);
    uint8_t r = GetRValue(sysColor);
    uint8_t g = GetGValue(sysColor);
    uint8_t b = GetBValue(sysColor);
    return Color(r, g, b);
}

void ShowMessageBox(HWND handle, const std::string& caption, const std::string& message)
{
    std::u16string msg = ToUtf16(message);
    std::u16string cap = ToUtf16(caption);
    MessageBoxW(handle, LPCWSTR(msg.c_str()), LPCWSTR(cap.c_str()), MB_OK);
}

void ShowInfoMessageBox(HWND handle, const std::string& message)
{
    ShowMessageBox(handle, "info", message);
}

void ShowErrorMessageBox(HWND handle, const std::string& message)
{
    ShowMessageBox(handle, "error", message);
}

} } // cmajor::wing
