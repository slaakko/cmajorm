// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmajor { namespace wing {

Keys Application::keyboardModifiers;
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
    switch (message)
    {
        case WM_KEYDOWN:
        {
            int virtualKeyCode = static_cast<int>(wParam);
            Keys keyCode = static_cast<Keys>(virtualKeyCode);
            switch (keyCode)
            {
                case Keys::controlKey:
                {
                    Keys modifiers = GetKeyboardModifiers();
                    modifiers = modifiers | Keys::controlModifier;
                    SetKeyboardModifiers(modifiers);
                    result = 0;
                    return true;
                }
                case Keys::shiftKey:
                {
                    Keys modifiers = GetKeyboardModifiers();
                    modifiers = modifiers | Keys::shiftModifier;
                    SetKeyboardModifiers(modifiers);
                    result = 0;
                    return true;
                }
                case Keys::menu:
                {
                    Keys modifiers = GetKeyboardModifiers();
                    modifiers = modifiers | Keys::altModifier;
                    SetKeyboardModifiers(modifiers);
                    result = 0;
                    return true;
                }
            }
            break;
        }
        case WM_KEYUP:
        {
            int virtualKeyCode = static_cast<int>(wParam);
            Keys keyCode = static_cast<Keys>(virtualKeyCode);
            switch (keyCode)
            {
                case Keys::controlKey:
                {
                    Keys modifiers = GetKeyboardModifiers();
                    modifiers = modifiers & ~Keys::controlModifier;
                    SetKeyboardModifiers(modifiers);
                    result = 0;
                    return true;
                }
                case Keys::shiftKey:
                {
                    Keys modifiers = GetKeyboardModifiers();
                    modifiers = modifiers & ~Keys::shiftModifier;
                    SetKeyboardModifiers(modifiers);
                    result = 0;
                    return true;
                }
                case Keys::menu:
                {
                    Keys modifiers = GetKeyboardModifiers();
                    modifiers = modifiers & ~Keys::altModifier;
                    SetKeyboardModifiers(modifiers);
                    result = 0;
                    return true;
                }
            }
            break;
        }
    }
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

void Application::ModelessWindowKeyPreview(WPARAM keyCode, KeyState keyState, bool& handled)
{
    Window* activeWindow = ActiveWindow();
    if (activeWindow)
    {
        Keys key = static_cast<Keys>(static_cast<int>(keyCode));
        KeyPreviewMethod keyPreviewMethod = activeWindow->GetKeyPreviewMethod();
        keyPreviewMethod(key, keyState, handled);
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