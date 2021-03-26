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
#include <functional>

namespace cmajor { namespace wing {

class Window;

using KeyPreviewFn = std::function<void(Keys, bool&)>;

class KeyPreviewMethod
{
public:
    KeyPreviewMethod()
    {
    }
    template<class T>
    void SetHandlerFunction(T* t, void (T::* pm)(Keys, bool&))
    {
        fn = std::bind(pm, t, std::placeholders::_1, std::placeholders::_2);
    }
    void operator()(Keys key, bool& handled)
    {
        fn(key, handled);
    }
private:
    KeyPreviewFn fn;
};

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
    static void ProcessMessages();
private:
    static bool ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result, void*& originalWndProc);
    static void ModelessWindowKeyPreview(Keys key, bool& handled);
    friend WING_API void ApplicationInit();
    static void Init();
    static WindowManager windowManager;
    static ResourceManager resourceManager;
    static Window* mainWindow;
    static Window* activeWindow;
};

WING_API void ApplicationInit();
WING_API void ApplicationDone();

} } // cmajor::wing

#endif // CMAJOR_WING_APPLICATION_INCLUDED
