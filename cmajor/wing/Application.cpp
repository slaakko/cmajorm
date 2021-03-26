// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmajor { namespace wing {

ResourceManager Application::resourceManager;
WindowManager Application::windowManager;
Window* Application::mainWindow = nullptr;
Window* Application::activeWindow = nullptr;

void Application::Init()
{
    SetMessageProcessorFunction(Application::ProcessMessage);
    SetKeyPreviewFunction(Application::ModelessWindowKeyPreview);
}

void Application::Exit()
{
    Exit(0);
}

void Application::Exit(int exitCode)
{
    PostQuitMessage(exitCode);
}

int Application::Run(Window& mainWindow)
{
    mainWindow.SetAsMainWindow();
    SetMainWindow(&mainWindow);
    switch (mainWindow.GetWindowState())
    {
        case WindowState::normal:
        {
            mainWindow.ShowWindow(SW_SHOWNORMAL);
            break;
        }
        case WindowState::maximized:
        {
            mainWindow.ShowWindow(SW_MAXIMIZE);
            break;
        }
        case WindowState::minimized:
        {
            mainWindow.ShowWindow(SW_MINIMIZE);
            break;
        }
    }
    mainWindow.Show();
    mainWindow.Update();
    return wing::Run();
}

void Application::ProcessMessages()
{
    wing::MessageLoop();
}

bool Application::ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result, void*& originalWndProc)
{
    Control* window = windowManager.GetWindow(handle);
    if (window)
    {
        Message msg(handle, message, wParam, lParam, result);
        bool handled = window->ProcessMessageInternal(msg); 
        if (msg.originalWndProc != nullptr)
        {
            originalWndProc = msg.originalWndProc;
        }
        if (handled)
        {
            result = msg.result;
        }
        return handled;
    }
    return false;
}

void Application::ModelessWindowKeyPreview(Keys key, bool& handled)
{
    Window* activeWindow = ActiveWindow();
    if (activeWindow)
    {
        KeyPreviewMethod keyPreviewMethod = activeWindow->GetKeyPreviewMethod();
        keyPreviewMethod(key, handled);
    }
}

void ApplicationInit()
{
    Application::Init();
}

void ApplicationDone()
{
}

} } // cmajor::wing