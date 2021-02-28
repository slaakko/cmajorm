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
    void NewProjectClick();
    void OpenProjectClick();
    void CloseSolutionClick();
    void SaveClick();
    void SaveAllClick();
    void ExitClick();
    void CopyClick();
    void CutClick();
    void PasteClick();
    void UndoClick();
    void RedoClick();
    void GotoClick();
    void SearchClick();
    void OptionsClick();
    MenuItem* newProjectMenuItem;
    MenuItem* openProjectMenuItem;
    MenuItem* closeSolutionMenuItem;
    MenuItem* saveMenuItem;
    MenuItem* saveAllMenuItem;
    MenuItem* exitMenuItem;
    MenuItem* copyMenuItem;
    MenuItem* cutMenuItem;
    MenuItem* pasteMenuItem;
    MenuItem* undoMenuItem;
    MenuItem* redoMenuItem;
    MenuItem* gotoMenuItem;
    MenuItem* searchMenuItem;
    MenuItem* optionsMenuItem;
};

MainWindow::MainWindow() : Window(WindowCreateParams().Text("Cmajor Code")), 
    newProjectMenuItem(nullptr),
    openProjectMenuItem(nullptr),
    closeSolutionMenuItem(nullptr),
    saveMenuItem(nullptr),
    saveAllMenuItem(nullptr),
    exitMenuItem(nullptr),
    copyMenuItem(nullptr),
    cutMenuItem(nullptr),
    pasteMenuItem(nullptr),
    undoMenuItem(nullptr),
    redoMenuItem(nullptr),
    gotoMenuItem(nullptr),
    searchMenuItem(nullptr),
    optionsMenuItem(nullptr)
{
    std::unique_ptr<MenuBar> menuBar(new MenuBar());
    std::unique_ptr<MenuItem> fileMenuItem(new MenuItem("&File"));
    std::unique_ptr<MenuItem> newProjectMenuItemPtr(new MenuItem("&New Project.."));
    newProjectMenuItem = newProjectMenuItemPtr.get();
    newProjectMenuItem->SetShortcut(Keys::controlModifier | Keys::n);
    newProjectMenuItem->Click().AddHandler(this, &MainWindow::NewProjectClick);
    fileMenuItem->AddMenuItem(newProjectMenuItemPtr.release());
    std::unique_ptr<MenuItem> openProjectMenuItemPtr(new MenuItem("&Open Project/Solution.."));
    openProjectMenuItem = openProjectMenuItemPtr.get();
    openProjectMenuItem->SetShortcut(Keys::controlModifier | Keys::o);
    openProjectMenuItem->Click().AddHandler(this, &MainWindow::OpenProjectClick);
    fileMenuItem->AddMenuItem(openProjectMenuItemPtr.release());
    std::unique_ptr<MenuItem> closeSolutionMenuItemPtr(new MenuItem("Close Solution"));
    closeSolutionMenuItem = closeSolutionMenuItemPtr.get();
    closeSolutionMenuItem->Click().AddHandler(this, &MainWindow::CloseSolutionClick);
    fileMenuItem->AddMenuItem(closeSolutionMenuItemPtr.release());
    fileMenuItem->AddMenuItem(new MenuItemSeparator());
    std::unique_ptr<MenuItem> saveMenuItemPtr(new MenuItem("&Save"));
    saveMenuItem = saveMenuItemPtr.get();
    saveMenuItem->SetShortcut(Keys::controlModifier | Keys::s);
    saveMenuItem->Click().AddHandler(this, &MainWindow::SaveClick);
    fileMenuItem->AddMenuItem(saveMenuItemPtr.release());
    std::unique_ptr<MenuItem> saveAllMenuItemPtr(new MenuItem("Save A&ll"));
    saveAllMenuItem = saveAllMenuItemPtr.get();
    saveAllMenuItem->SetShortcut(Keys::controlModifier | Keys::shiftModifier | Keys::s);
    saveAllMenuItem->Click().AddHandler(this, &MainWindow::SaveAllClick);
    fileMenuItem->AddMenuItem(saveAllMenuItemPtr.release());
    fileMenuItem->AddMenuItem(new MenuItemSeparator());
    std::unique_ptr<MenuItem> exitMenuItemPtr(new MenuItem("E&xit"));
    exitMenuItem = exitMenuItemPtr.get();
    exitMenuItem->SetShortcut(Keys::altModifier | Keys::f4);
    exitMenuItem->Click().AddHandler(this, &MainWindow::ExitClick);
    fileMenuItem->AddMenuItem(exitMenuItemPtr.release());
    menuBar->AddMenuItem(fileMenuItem.release());
    std::unique_ptr<MenuItem> editMenuItem(new MenuItem("&Edit"));
    std::unique_ptr<MenuItem> copyMenuItemPtr(new MenuItem("&Copy"));
    copyMenuItem = copyMenuItemPtr.get();
    copyMenuItem->SetShortcut(Keys::controlModifier | Keys::c);
    copyMenuItem->Click().AddHandler(this, &MainWindow::CopyClick);
    editMenuItem->AddMenuItem(copyMenuItemPtr.release());
    std::unique_ptr<MenuItem> cutMenuItemPtr(new MenuItem("C&ut"));
    cutMenuItem = cutMenuItemPtr.get();
    cutMenuItem->SetShortcut(Keys::controlModifier | Keys::x);
    cutMenuItem->Click().AddHandler(this, &MainWindow::CutClick);
    editMenuItem->AddMenuItem(cutMenuItemPtr.release());
    std::unique_ptr<MenuItem> pasteMenuItemPtr(new MenuItem("&Paste"));
    pasteMenuItem = pasteMenuItemPtr.get();
    pasteMenuItem->SetShortcut(Keys::controlModifier | Keys::v);
    pasteMenuItem->Click().AddHandler(this, &MainWindow::PasteClick);
    editMenuItem->AddMenuItem(pasteMenuItemPtr.release());
    std::unique_ptr<MenuItem> undoMenuItemPtr(new MenuItem("U&ndo"));
    undoMenuItem = undoMenuItemPtr.get();
    undoMenuItem->SetShortcut(Keys::controlModifier | Keys::z);
    undoMenuItem->Click().AddHandler(this, &MainWindow::UndoClick);
    editMenuItem->AddMenuItem(undoMenuItemPtr.release());
    std::unique_ptr<MenuItem> redoMenuItemPtr(new MenuItem("&Redo"));
    redoMenuItem = redoMenuItemPtr.get();
    redoMenuItem->SetShortcut(Keys::controlModifier | Keys::y);
    redoMenuItem->Click().AddHandler(this, &MainWindow::RedoClick);
    editMenuItem->AddMenuItem(redoMenuItemPtr.release());
    std::unique_ptr<MenuItem> gotoMenuItemPtr(new MenuItem("&Go To Line"));
    gotoMenuItem = gotoMenuItemPtr.get();
    gotoMenuItem->SetShortcut(Keys::controlModifier | Keys::g);
    gotoMenuItem->Click().AddHandler(this, &MainWindow::GotoClick);
    editMenuItem->AddMenuItem(gotoMenuItemPtr.release());
    std::unique_ptr<MenuItem> searchMenuItemPtr(new MenuItem("&Search"));
    searchMenuItem = searchMenuItemPtr.get();
    searchMenuItem->SetShortcut(Keys::controlModifier | Keys::f);
    searchMenuItem->Click().AddHandler(this, &MainWindow::SearchClick);
    editMenuItem->AddMenuItem(searchMenuItemPtr.release());
    std::unique_ptr<MenuItem> optionsMenuItemPtr(new MenuItem("&Options..."));
    optionsMenuItem = optionsMenuItemPtr.get();
    optionsMenuItem->Click().AddHandler(this, &MainWindow::OptionsClick);
    editMenuItem->AddMenuItem(optionsMenuItemPtr.release());
    menuBar->AddMenuItem(editMenuItem.release());
    AddChild(menuBar.release());
}

void MainWindow::NewProjectClick()
{
    ShowInfoMessageBox(Handle(), "New Project");
}

void MainWindow::OpenProjectClick()
{
    ShowInfoMessageBox(Handle(), "Open Project");
}

void MainWindow::CloseSolutionClick()
{
    ShowInfoMessageBox(Handle(), "Close Solution");
}

void MainWindow::SaveClick()
{
    ShowInfoMessageBox(Handle(), "Save");
}

void MainWindow::SaveAllClick()
{
    ShowInfoMessageBox(Handle(), "Save All");
}

void MainWindow::ExitClick()
{
    Application::Exit();
}

void MainWindow::CopyClick()
{
    ShowInfoMessageBox(Handle(), "Copy");
}

void MainWindow::CutClick()
{
    ShowInfoMessageBox(Handle(), "Cut");
}

void MainWindow::PasteClick()
{
    ShowInfoMessageBox(Handle(), "Paste");
}

void MainWindow::UndoClick()
{
    ShowInfoMessageBox(Handle(), "Undo");
}

void MainWindow::RedoClick()
{
    ShowInfoMessageBox(Handle(), "Redo");
}

void MainWindow::GotoClick()
{
    ShowInfoMessageBox(Handle(), "Goto");
}

void MainWindow::SearchClick()
{
    ShowInfoMessageBox(Handle(), "Search");
}

void MainWindow::OptionsClick()
{
    ShowInfoMessageBox(Handle(), "Options");
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
