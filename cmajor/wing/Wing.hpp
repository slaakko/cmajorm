// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_WING_INCLUDED
#define CMAJOR_WING_WING_INCLUDED
#include <cmajor/wing/WingApi.hpp>
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

using MessageProcessorFunction = bool(*)(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);

struct WING_API Message
{
    Message(HWND handle_, UINT message_, WPARAM wParam_, LPARAM lParam_, LRESULT result_) : handle(handle_), message(message_), wParam(wParam_), lParam(lParam_), result(result_) {}
    uint32_t LParamLoDWord() const { return static_cast<uint32_t>(lParam); }
    uint32_t LParamHiDWord() const { return static_cast<uint32_t>((lParam >> 32) & 0xFFFFFFFF); }
    uint16_t WParamLoWord() const { return static_cast<uint16_t>(wParam & 0xFFFF); }
    uint16_t WParamHiWord() const { return static_cast<uint16_t>((wParam >> 16) & 0xFFFF); }
    HWND handle;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    LRESULT result;
};

WING_API void SetMessageProcessorFunction(MessageProcessorFunction messageProcessorFun);

enum class KeyState : int
{
    none = 0, down = 1 << 0, shift = 1 << 1, control = 1 << 2, alt = 1 << 3
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

using KeyPreviewFunction = void (*)(WPARAM keyCode, KeyState keyState, bool& handled);

WING_API void SetKeyPreviewFunction(KeyPreviewFunction keyPreviewFun);

WING_API HINSTANCE Instance();
WING_API void SetInstance(HINSTANCE instance_);
WING_API WNDPROC GetWndProc();

WING_API int Run();

WING_API void WingInit();
WING_API void WingDone();

using Color = Gdiplus::Color;

WING_API Color GetSystemColor(int index);

WING_API void ShowErrorMessageBox(HWND handle, const std::string& message);

} } // cmajor::wing

#endif // CMAJOR_WING_WING_INCLUDED
