// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/Graphics.hpp>
#include <soulng/util/Unicode.hpp>
#include <shobjidl_core.h>

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

WNDPROC GetWndProc()
{
    return WndProc;
}

bool KeyPressed(int virtualKeyCode)
{
    return (::GetKeyState(virtualKeyCode) & 0x8000) != 0;
}

KeyState GetKeyState()
{
    KeyState keyState = KeyState::none;
    if (KeyPressed(VK_CONTROL))
    {
        keyState = keyState | KeyState::control;
    }
    if (KeyPressed(VK_SHIFT))
    {
        keyState = keyState | KeyState::shift;
    }
    if (KeyPressed(VK_MENU))
    {
        keyState = keyState | KeyState::alt;
    }
    return keyState;
}

KeyPreviewFunction keyPreview;

void SetKeyPreviewFunction(KeyPreviewFunction keyPreviewFun)
{
    keyPreview = keyPreviewFun;
}

Keys MakeKeyWithState(Keys key)
{
    Keys keys = key;
    KeyState keyState = GetKeyState();
    if ((keyState & KeyState::control) != KeyState::none)
    {
        keys = keys | Keys::controlModifier;
    }
    if ((keyState & KeyState::shift) != KeyState::none)
    {
        keys = keys | Keys::shiftModifier;;
    }
    if ((keyState & KeyState::alt) != KeyState::none)
    {
        keys = keys | Keys::altModifier;
    }
    return keys;
}

int Run()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        bool handled = false;
        if (msg.message == WM_KEYDOWN)
        {
            WPARAM keyCode = msg.wParam;
            if (keyPreview)
            {
                Keys key = static_cast<Keys>(keyCode);
                Keys keys = MakeKeyWithState(key);
                keyPreview(keys, handled);
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
            if (msg.message == WM_KEYDOWN)
            {
                if (keyPreview)
                {
                    Keys key = static_cast<Keys>(msg.wParam);
                    Keys keys = MakeKeyWithState(key);
                    keyPreview(keys, handled);
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

int DialogMessageLoop(HWND handle, HWND parentHandle, DialogResultFunction dialogResultFn, DialogWindowKeyPreviewFunction dialogWindowKeyPreviewFn, void* dialogWindowPtr) 
{
    int dialogResult = 0;
    EnableWindow(parentHandle, false);
    ShowWindow(handle, SW_SHOW);
    BringWindowToTop(handle);
    SetActiveWindow(handle);
    bool end = false;
    while (!end)
    {
        MSG msg;
        if (GetMessage(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            bool handled = false;
            if (msg.message == WM_KEYDOWN)
            {
                if (dialogWindowKeyPreviewFn)
                {
                    Keys key = static_cast<Keys>(msg.wParam);
                    Keys keys = MakeKeyWithState(key);
                    dialogWindowKeyPreviewFn(dialogWindowPtr, keys, handled);
                }
            }
            if (!handled)
            {
                DispatchMessage(&msg);
            }
            dialogResult = dialogResultFn(dialogWindowPtr);
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
    EnableWindow(parentHandle, true);
    PostMessage(handle, WM_CLOSE, 0u, 0);
    SetActiveWindow(parentHandle);
    return dialogResult;
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

std::string SelectDirectory(HWND handle, const std::string& directoryPath)
{
    std::u16string dirPath = ToUtf16(directoryPath);
    int bufferSize = 1024;
    std::unique_ptr<char16_t> buffer(new char16_t[bufferSize]);
    IFileDialog* fileDialog = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog))))
    {
        IShellItem* defaultFolder = nullptr;
        if (!dirPath.empty())
        {
            if (SUCCEEDED(SHCreateItemFromParsingName((PCWSTR)dirPath.c_str(), nullptr, IID_IShellItem, (void**)&defaultFolder)))
            {
                if (!SUCCEEDED(fileDialog->SetDefaultFolder(defaultFolder)))
                {
                    fileDialog->Release();
                    return std::string();
                }
            }
        }
        DWORD options = 0;
        if (SUCCEEDED(fileDialog->GetOptions(&options)))
        {
            if (SUCCEEDED(fileDialog->SetOptions(options | FOS_PICKFOLDERS)))
            {
                if (SUCCEEDED(fileDialog->Show(handle)))
                {
                    IShellItem* result = nullptr;
                    if (SUCCEEDED(fileDialog->GetResult(&result)))
                    {
                        LPWSTR name = nullptr;
                        if (SUCCEEDED(result->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &name)))
                        {
                            wcsncpy((wchar_t*)buffer.get(), name, bufferSize);
                            fileDialog->Release();
                            result->Release();
                            CoTaskMemFree(name);
                            return ToUtf8(buffer.get());
                        }
                        else
                        {
                            result->Release();
                            fileDialog->Release();
                            return std::string();
                        }
                    }
                    else
                    {
                        fileDialog->Release();
                        return std::string();
                    }
                }
                else
                {
                    fileDialog->Release();
                    return std::string();
                }
            }
            else
            {
                fileDialog->Release();
                return std::string();
            }
        }
        else
        {
            fileDialog->Release();
            return std::string();
        }
    }
    else
    {
        return std::string();
    }
}

} } // cmajor::wing
