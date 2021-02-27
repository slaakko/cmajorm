// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/InitDone.hpp>
#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/Window.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <stdexcept>
#include <string>

struct InitDone
{
    InitDone(HINSTANCE instance)
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::wing::Init(instance);
    }
    ~InitDone()
    {
        cmajor::wing::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::unicode;
using namespace cmajor::wing;

class MainWindow : public Window
{
public:
    MainWindow();
private:
    MenuItem* newProjectMenuItem;
};

MainWindow::MainWindow() : Window(WindowCreateParams().Text("Cmajor Code")), newProjectMenuItem(nullptr)
{
    std::unique_ptr<MenuBar> menuBar(new MenuBar());
    std::unique_ptr<MenuItem> fileMenuItem(new MenuItem("&File"));
    std::unique_ptr<MenuItem> newProjectMenuItemPtr(new MenuItem("&New Project.."));
    newProjectMenuItem = newProjectMenuItemPtr.get();
    newProjectMenuItem->SetShortcut(Keys::controlModifier | Keys::n);
    fileMenuItem->AddMenuItem(newProjectMenuItemPtr.release());
    menuBar->AddMenuItem(fileMenuItem.release());
    AddChild(menuBar.release());
}

int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    InitDone initDone(instance);
    try
    {
        MainWindow mainWindow;
        Application::Run(mainWindow);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
        return 1;
    }
    return 0;
}
