// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_WINDOW_MANAGER_INCLUDED
#define CMAJOR_WING_WINDOW_MANAGER_INCLUDED
#include <Windows.h>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <stdint.h>

namespace cmajor { namespace wing {

class Control;

class WindowManager
{
public:
    WindowManager();
    bool IsSystemClassName(const std::string& windowClassName) const;
    uint16_t Register(const std::string& windowClassName, uint32_t windowClassStyle, int64_t windowClassBackgroundColor);
    void AddWindow(Control* window);
    void RemoveWindow(Control* window);
    Control* GetWindow(HWND handle) const;
private:
    std::set<std::string> systemClassNames;
    std::map<std::string, uint16_t> registeredWindowClasses;
    std::unordered_map<HWND, Control*> windowMap;
};

} } // cmajor::wing

#endif // CMAJOR_WING_WINDOW_MANAGER_INCLUDED
