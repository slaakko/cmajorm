// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmajor { namespace wing {

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
    mainWindow.Show();
    mainWindow.Update();
    return wing::Run();
}

bool Application::ProcessMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    Control* window = windowManager.GetWindow(handle);
    if (window)
    {
        Message msg(handle, message, wParam, lParam, result);
        bool handled = window->ProcessMessageInternal(msg); 
        // todo
        if (handled)
        {
            result = msg.result;
        }
        return handled;
    }
    return false;
}

void Application::ModelessWindowKeyPreview(WPARAM keyCode, KeyState keyState, bool& handled)
{
    // todo
}

void ApplicationInit()
{
    Application::Init();
}

void ApplicationDone()
{
}

} } // cmajor::wing