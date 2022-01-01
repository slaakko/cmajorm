// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/WindowManager.hpp>
#include <wing/Control.hpp>
#include <wing/Wing.hpp>
#include <soulng/util/Unicode.hpp>
#include <Windows.h>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

WindowManager::WindowManager()
{
    systemClassNames.insert("BUTTON");
    systemClassNames.insert("STATIC");
    systemClassNames.insert("EDIT");
    systemClassNames.insert("LISTBOX");
    systemClassNames.insert("COMBOBOX");
    systemClassNames.insert("SCROLLBAR");
}

bool WindowManager::IsSystemClassName(const std::string& windowClassName) const
{
    auto it = systemClassNames.find(windowClassName);
    return it != systemClassNames.cend();
}

uint16_t WindowManager::Register(const std::string& windowClassName, uint32_t windowClassStyle, HBRUSH backgroundBrush)
{
    auto it = registeredWindowClasses.find(windowClassName);
    if (it != registeredWindowClasses.cend())
    {
        return it->second;
    }
    std::u16string className = ToUtf16(windowClassName);
    HINSTANCE instance = Instance();
    WNDCLASSEXW wc;
    wc.cbSize = sizeof(wc);
    wc.style = windowClassStyle;
    wc.lpfnWndProc = GetWndProc();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = nullptr;
    wc.hbrBackground = backgroundBrush;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = (LPWSTR)className.c_str();
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    uint16_t windowClass = RegisterClassExW(&wc);
    if (windowClass == 0)
    {
        throw WindowsException(GetLastError());
    }
    registeredWindowClasses[windowClassName] = windowClass;
    return windowClass;
}

void WindowManager::AddWindow(Control* window)
{
    windowMap[window->Handle()] = window;
}

void WindowManager::RemoveWindow(Control* window)
{
    windowMap.erase(window->Handle());
}

Control* WindowManager::GetWindow(HWND handle) const
{
    auto it = windowMap.find(handle);
    if (it != windowMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

} } // cmajor::wing
