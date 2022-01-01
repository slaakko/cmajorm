// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_WING_INCLUDED
#define CMAJOR_WING_WING_INCLUDED
#include <wing/Keys.hpp>
#include <Windows.h>
#include <gdiplus.h>
#include <stdexcept>
#include <string>
#include <stdint.h>

#pragma comment (lib,"Gdiplus.lib")

namespace cmajor { namespace wing {

class WING_API WindowsException : public std::runtime_error
{
public:
    WindowsException(uint64_t errorCode_);
    uint64_t ErrorCode() const { return errorCode; }
    std::string ErrorMessage() const { return what(); }
private:
    uint64_t errorCode;
};

using MessageProcessorFunction = bool(*)(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result, void*& originalWndProc);

struct WING_API Message
{
    Message(HWND handle_, UINT message_, WPARAM wParam_, LPARAM lParam_, LRESULT result_) : 
        handle(handle_), message(message_), wParam(wParam_), lParam(lParam_), result(result_), originalWndProc(nullptr) 
    {
    }
    uint32_t LParamLoDWord() const { return static_cast<uint32_t>(lParam); }
    uint32_t LParamHiDWord() const { return static_cast<uint32_t>((lParam >> 32) & 0xFFFFFFFF); }
    uint16_t WParamLoWord() const { return static_cast<uint16_t>(wParam & 0xFFFF); }
    uint16_t WParamHiWord() const { return static_cast<uint16_t>((wParam >> 16) & 0xFFFF); }
    HWND handle;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    LRESULT result;
    void* originalWndProc;
};

WING_API void SetMessageProcessorFunction(MessageProcessorFunction messageProcessorFun);

struct WING_API Padding
{
    Padding() : left(0), top(0), right(0), bottom(0) {}
    Padding(int left_, int top_, int right_, int bottom_) : left(left_), top(top_), right(right_), bottom(bottom_) {}
    int Vertical() const { return top + bottom; }
    int Horizontal() const { return left + right; }
    int left;
    int top;
    int right;
    int bottom;
};

WING_API inline bool operator==(const Padding& left, const Padding& right)
{
    return left.left == right.left && left.top == right.top && left.right == right.right && left.bottom == right.bottom;
}

WING_API inline bool operator!=(const Padding& left, const Padding& right)
{
    return !(left == right);
}

enum class KeyState : int
{
    none = 0, shift = 1 << 0, control = 1 << 1, alt = 1 << 2
};

WING_API inline KeyState operator|(KeyState left, KeyState right)
{
    return KeyState(int(left) | int(right));
}

WING_API inline KeyState operator&(KeyState left, KeyState right)
{
    return KeyState(int(left) & int(right));
}

WING_API inline KeyState operator~(KeyState state)
{
    return KeyState(~int(state));
}

WING_API bool KeyPressed(int virtualKeyCode);

WING_API KeyState GetKeyState();

using KeyPreviewFunction = void (*)(Keys key, bool& handled);

WING_API void SetKeyPreviewFunction(KeyPreviewFunction keyPreviewFun);
WING_API Keys MakeKeyWithState(Keys key);

WING_API HINSTANCE Instance();
WING_API WNDPROC GetWndProc();

WING_API int Run();
WING_API int MessageLoop();

using DialogResultFunction = int (*)(void* dialogWindowPtr);
using DialogWindowKeyPreviewFunction = void (*)(void* dialogWindowPtr, Keys key, bool& handled);


WING_API int DialogMessageLoop(HWND handle, HWND parentHandle, DialogResultFunction dialogResultFn, DialogWindowKeyPreviewFunction dialogWindowKeyPreviewFn, void* dialogWindowPtr);

WING_API void WingInit(HINSTANCE instance_);
WING_API void WingDone();

using Color = Gdiplus::Color;

WING_API Color GetSystemColor(int index);

WING_API void ShowMessageBox(HWND handle, const std::string& caption, const std::string& message);
WING_API void ShowInfoMessageBox(HWND handle, const std::string& message);
WING_API void ShowErrorMessageBox(HWND handle, const std::string& message);

WING_API LRESULT CALLBACK CommandSubClassWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

WING_API std::string SelectDirectory(HWND handle, const std::string& directoryPath);

} } // cmajor::wing

#endif // CMAJOR_WING_WING_INCLUDED
