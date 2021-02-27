// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_APPLICATION_INCLUDED
#define CMAJOR_WING_APPLICATION_INCLUDED
#include <cmajor/wing/Keys.hpp>
#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/ResourceManager.hpp>
#include <cmajor/wing/WindowManager.hpp>

namespace cmajor { namespace wing {

class Window;

class WING_API Application
{
public:
    static int Run(Window& mainWindow);
    static void Exit();
    static void Exit(int exitCode);
    static ResourceManager& GetResourceManager() { return resourceManager; }
    static WindowManager& GetWindowManager() { return windowManager; }
    static void SetMainWindow(Window* mainWindow_) { mainWindow = mainWindow_; }
    static Window* MainWindow() { return mainWindow; }
    static void SetActiveWindow(Window* activeWindow_) { activeWindow = activeWindow_; }
    static Window* ActiveWindow() { return activeWindow; }
    static Keys GetKeyboardModifiers() { return keyboardModifiers; }
    static void SetKeyboardModifiers(Keys keyboardModifiers_) { keyboardModifiers = keyboardModifiers_; }
private:
    static bool ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);
    static void ModelessWindowKeyPreview(WPARAM keyCode, KeyState keyState, bool& handled);
    friend WING_API void ApplicationInit();
    static void Init();
    static WindowManager windowManager;
    static ResourceManager resourceManager;
    static Window* mainWindow;
    static Window* activeWindow;
    static Keys keyboardModifiers;
};

WING_API void ApplicationInit();
WING_API void ApplicationDone();

} } // cmajor::wing

#endif // CMAJOR_WING_APPLICATION_INCLUDED
