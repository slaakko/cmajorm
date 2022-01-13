// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/MainWindow.hpp>
#include <system-x/db/AboutDialog.hpp>
#include <system-x/kernel/Load.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <wing/Theme.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <wing/ScrollableControl.hpp>
#include <wing/Dialog.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>

#undef min
#undef max

namespace cmsx::db {

const char* cmsxDBVersion = "4.3.0";

using namespace soulng::util;
using namespace soulng::unicode;

std::string CmajorProjectsDir()
{
    std::string projectsDir = Path::Combine(CmajorRoot(), "projects");
    boost::filesystem::create_directories(projectsDir);
    return projectsDir;
}

MainDebuggerObserver::MainDebuggerObserver(MainWindow* mainWindow_) : mainWindow(mainWindow_)
{
}

void MainDebuggerObserver::DebuggerReady()
{
    SendMessage(mainWindow->Handle(), DEBUGGER_READY, 0, 0);
}

void MainDebuggerObserver::DebuggerError(const std::string& message)
{
    errorMessage = message;
    if (mainWindow->WaitingDebugger())
    {
        mainWindow->NotifyDebuggerError();
    }
    else
    {
        SendMessage(mainWindow->Handle(), DEBUGGER_ERROR, 0, 0);
    }
}

void MainDebuggerObserver::DebuggerProcessExit()
{
    if (mainWindow->WaitingDebugger())
    {
        mainWindow->NotifyDebuggingStopped();
    }
    else
    {
        SendMessage(mainWindow->Handle(), DEBUGGER_PROCESS_EXIT, 0, 0);
    }
}

MainWindow::MainWindow(const std::string& filePath_) : 
    Window(WindowCreateParams().Text("System X Debugger").BackgroundColor(GetColor("window.background")).WindowClassName("system.x.db.MainWindow")),
    observer(this), filePath(filePath_), args(), env(), machine(nullptr), process(nullptr), openFileMenuItem(nullptr), closeFileMenuItem(nullptr), exitMenuItem(nullptr), 
    codeView(nullptr), registerView(nullptr), dataView(nullptr), argsView(nullptr), envView(nullptr), heapView(nullptr), stackView(nullptr), logView(nullptr), 
    currentTopView(nullptr), currentBottomView(nullptr), waitingDebugger(false), startContinueMenuItem(nullptr), stopMenuItem(nullptr), resetMenuItem(nullptr), 
    singleStepMenuItem(nullptr), stepOverMenuItem(nullptr), toggleBreakpointMenuItem(nullptr), fileOpen(false), console(nullptr)
{
    std::unique_ptr<MenuBar> menuBar(new MenuBar());
    std::unique_ptr<MenuItem> fileMenuItem(new MenuItem("&File"));
    std::unique_ptr<MenuItem> openFileMenuItemPtr(new MenuItem("&Open..."));
    openFileMenuItem = openFileMenuItemPtr.get();
    openFileMenuItem->SetShortcut(Keys::controlModifier | Keys::o);
    openFileMenuItem->Click().AddHandler(this, &MainWindow::OpenFileClick);
    fileMenuItem->AddMenuItem(openFileMenuItemPtr.release());
    std::unique_ptr<MenuItem> closeFileMenuItemPtr(new MenuItem("&Close"));
    closeFileMenuItem = closeFileMenuItemPtr.get();
    closeFileMenuItem->SetShortcut(Keys::controlModifier | Keys::f4);
    closeFileMenuItem->Click().AddHandler(this, &MainWindow::CloseFileClick);
    fileMenuItem->AddMenuItem(closeFileMenuItemPtr.release());
    std::unique_ptr<MenuItem> exitMenuItemPtr(new MenuItem("E&xit"));
    exitMenuItem = exitMenuItemPtr.get();
    exitMenuItem->SetShortcut(Keys::altModifier | Keys::f4);
    exitMenuItem->Click().AddHandler(this, &MainWindow::ExitClick);
    fileMenuItem->AddMenuItem(exitMenuItemPtr.release());
    menuBar->AddMenuItem(fileMenuItem.release());
    std::unique_ptr<MenuItem> viewMenuItem(new MenuItem("&View"));
    std::unique_ptr<MenuItem> codeMenuItemPtr(new MenuItem("&Code"));
    codeMenuItemPtr->Click().AddHandler(this, &MainWindow::ViewCodeClick);
    viewMenuItem->AddMenuItem(codeMenuItemPtr.release());
    std::unique_ptr<MenuItem> dataMenuItemPtr(new MenuItem("&Data"));
    dataMenuItemPtr->Click().AddHandler(this, &MainWindow::ViewDataClick);
    viewMenuItem->AddMenuItem(dataMenuItemPtr.release());
    std::unique_ptr<MenuItem> argsMenuItemPtr(new MenuItem("&Arguments"));
    argsMenuItemPtr->Click().AddHandler(this, &MainWindow::ViewArgsClick);
    viewMenuItem->AddMenuItem(argsMenuItemPtr.release());
    std::unique_ptr<MenuItem> envMenuItemPtr(new MenuItem("&Environment"));
    envMenuItemPtr->Click().AddHandler(this, &MainWindow::ViewEnvClick);
    viewMenuItem->AddMenuItem(envMenuItemPtr.release());
    std::unique_ptr<MenuItem> heapMenuItemPtr(new MenuItem("&Heap"));
    heapMenuItemPtr->Click().AddHandler(this, &MainWindow::ViewHeapClick);
    viewMenuItem->AddMenuItem(heapMenuItemPtr.release());
    std::unique_ptr<MenuItem> stackMenuItemPtr(new MenuItem("&Stack"));
    stackMenuItemPtr->Click().AddHandler(this, &MainWindow::ViewStackClick);
    viewMenuItem->AddMenuItem(stackMenuItemPtr.release());
    std::unique_ptr<MenuItem> regsMenuItemPtr(new MenuItem("&Registers"));
    regsMenuItemPtr->Click().AddHandler(this, &MainWindow::ViewRegsClick);
    viewMenuItem->AddMenuItem(regsMenuItemPtr.release());
    std::unique_ptr<MenuItem> startMenuItemPtr(new MenuItem("&Start"));
    startMenuItemPtr->SetShortcut(Keys::home);
    startMenuItemPtr->Click().AddHandler(this, &MainWindow::HomeClick);
    viewMenuItem->AddMenuItem(startMenuItemPtr.release());
    std::unique_ptr<MenuItem> endMenuItemPtr(new MenuItem("&End"));
    endMenuItemPtr->SetShortcut(Keys::end);
    endMenuItemPtr->Click().AddHandler(this, &MainWindow::EndClick);
    viewMenuItem->AddMenuItem(endMenuItemPtr.release());
    std::unique_ptr<MenuItem> nextLineMenuItemPtr(new MenuItem("&Next Line"));
    nextLineMenuItemPtr->SetShortcut(Keys::down);
    nextLineMenuItemPtr->Click().AddHandler(this, &MainWindow::NextLineClick);
    viewMenuItem->AddMenuItem(nextLineMenuItemPtr.release());
    std::unique_ptr<MenuItem> prevLineMenuItemPtr(new MenuItem("&Previous Line"));
    prevLineMenuItemPtr->SetShortcut(Keys::up);
    prevLineMenuItemPtr->Click().AddHandler(this, &MainWindow::PrevLineClick);
    viewMenuItem->AddMenuItem(prevLineMenuItemPtr.release());
    std::unique_ptr<MenuItem> nextQuarterMenuItemPtr(new MenuItem("Next &Quarter"));
    nextQuarterMenuItemPtr->SetShortcut(Keys::f8);
    nextQuarterMenuItemPtr->Click().AddHandler(this, &MainWindow::NextQuarterClick);
    viewMenuItem->AddMenuItem(nextQuarterMenuItemPtr.release());
    std::unique_ptr<MenuItem> prevQuarterMenuItemPtr(new MenuItem("&Previous Quarter"));
    prevQuarterMenuItemPtr->SetShortcut(Keys::f7);
    prevQuarterMenuItemPtr->Click().AddHandler(this, &MainWindow::PrevQuarterClick);
    viewMenuItem->AddMenuItem(prevQuarterMenuItemPtr.release());
    std::unique_ptr<MenuItem> nextPageMenuItemPtr(new MenuItem("Ne&xt Page"));
    nextPageMenuItemPtr->SetShortcut(Keys::pageDown);
    nextPageMenuItemPtr->Click().AddHandler(this, &MainWindow::NextPageClick);
    viewMenuItem->AddMenuItem(nextPageMenuItemPtr.release());
    std::unique_ptr<MenuItem> prevPageMenuItemPtr(new MenuItem("P&revious Page"));
    prevPageMenuItemPtr->SetShortcut(Keys::pageUp);
    prevPageMenuItemPtr->Click().AddHandler(this, &MainWindow::PrevPageClick);
    viewMenuItem->AddMenuItem(prevPageMenuItemPtr.release());
    menuBar->AddMenuItem(viewMenuItem.release());

    std::unique_ptr<MenuItem> debugMenuItem(new MenuItem("&Debug"));

    std::unique_ptr<MenuItem> continueMenuItemPtr(new MenuItem("&Start/Continue Debugging"));
    startContinueMenuItem = continueMenuItemPtr.get();
    startContinueMenuItem->SetShortcut(Keys::f5);
    startContinueMenuItem->Click().AddHandler(this, &MainWindow::ContinueClick);
    debugMenuItem->AddMenuItem(continueMenuItemPtr.release());

    std::unique_ptr<MenuItem> stopMenuItemPtr(new MenuItem("S&top Debugging"));
    stopMenuItem = stopMenuItemPtr.get();
    stopMenuItem->SetShortcut(Keys::shiftModifier | Keys::f5);
    stopMenuItem->Click().AddHandler(this, &MainWindow::StopClick);
    debugMenuItem->AddMenuItem(stopMenuItemPtr.release());

    std::unique_ptr<MenuItem> resetMenuItemPtr(new MenuItem("&Reset Program To Start"));
    resetMenuItem = resetMenuItemPtr.get();
    resetMenuItem->SetShortcut(Keys::f4);
    resetMenuItem->Click().AddHandler(this, &MainWindow::ResetClick);
    debugMenuItem->AddMenuItem(resetMenuItemPtr.release());

    std::unique_ptr<MenuItem> singleStepMenuItemPtr(new MenuItem("S&ingle Step"));;
    singleStepMenuItem = singleStepMenuItemPtr.get();
    singleStepMenuItem->SetShortcut(Keys::f11);
    singleStepMenuItem->Click().AddHandler(this, &MainWindow::SingleStepClick);
    debugMenuItem->AddMenuItem(singleStepMenuItemPtr.release());

    std::unique_ptr<MenuItem> stepOverMenuItemPtr(new MenuItem("Step &Over"));;
    stepOverMenuItem = stepOverMenuItemPtr.get();
    stepOverMenuItem->SetShortcut(Keys::f12);
    stepOverMenuItem->Click().AddHandler(this, &MainWindow::StepOverClick);
    debugMenuItem->AddMenuItem(stepOverMenuItemPtr.release());

    std::unique_ptr<MenuItem> toggleBreakpointMenuItemPtr(new MenuItem("&Toggle Breakpoint"));;
    toggleBreakpointMenuItem = toggleBreakpointMenuItemPtr.get();
    toggleBreakpointMenuItem->SetShortcut(Keys::f9);
    toggleBreakpointMenuItem->Click().AddHandler(this, &MainWindow::ToggleBreakpointClick);
    debugMenuItem->AddMenuItem(toggleBreakpointMenuItemPtr.release());

    std::unique_ptr<MenuItem> gotoPrevAddressMenuItemPtr(new MenuItem("&Goto Previous Address"));
    gotoPrevAddressMenuItem = gotoPrevAddressMenuItemPtr.get();
    gotoPrevAddressMenuItem->SetShortcut(Keys::left);
    gotoPrevAddressMenuItem->Click().AddHandler(this, &MainWindow::PrevAddressClick);
    debugMenuItem->AddMenuItem(gotoPrevAddressMenuItemPtr.release());

    menuBar->AddMenuItem(debugMenuItem.release());


    std::unique_ptr<MenuItem> helpMenuItem(new MenuItem("&Help"));
    std::unique_ptr<MenuItem> aboutMenuItemPtr(new MenuItem("&About..."));
    aboutMenuItemPtr->Click().AddHandler(this, &MainWindow::AboutClick);
    helpMenuItem->AddMenuItem(aboutMenuItemPtr.release());
    menuBar->AddMenuItem(helpMenuItem.release());
    AddChild(menuBar.release());

    SplitContainerCreateParams verticalSplitContainerCreateParams(SplitterOrientation::vertical);
    verticalSplitContainerCreateParams.BackgroundColor(GetColor("splitContainer.background"));
    verticalSplitContainerCreateParams.Pane1BackgroundColor(GetColor("splitContainer.background")).Pane2BackgroundColor(GetColor("splitContainer.background")).
        BackgroundColor(GetColor("splitContainer.background")).SplitterBackgroundColor(GetColor("splitter.background")).SplitterEdgeColor(GetColor("splitter.edge")).SplitterDistance(0).SetDock(Dock::fill);
    std::unique_ptr<SplitContainer> verticalSplitContainerPtr(new SplitContainer(verticalSplitContainerCreateParams));
    verticalSplitContainer = verticalSplitContainerPtr.get();

    std::unique_ptr<TabControl> topTabControlPtr(
        new TabControl(TabControlCreateParams().
            BackgroundColor(GetColor("code.tab.control.background")).
            FrameColor(GetColor("code.tab.control.frame")).
            TextColor(GetColor("code.tab.control.text")).
            NormalBackgroundColor(GetColor("code.tab.control.tab.normal.background")).
            SelectedBackgroundColor(GetColor("code.tab.control.tab.selected.background")).
            CloseBoxSelectedColor(GetColor("code.tab.control.close.box.selected.background")).
            SetDock(Dock::fill)));
    topTabControl = topTabControlPtr.get();
    topTabControl->SetBackgroundItemName("code.tab.control.background");
    topTabControl->SetFrameItemName("code.tab.control.frame");
    topTabControl->SetTextItemName("code.tab.control.text");
    topTabControl->SetTabNormalBackgroundItemName("code.tab.control.tab.normal.background");
    topTabControl->SetTabSelectedBackgroundItemName("code.tab.control.tab.selected.background");
    topTabControl->SetCloseBoxSelectedBackgroundItemName("code.tab.control.close.box.selected.background");
    topTabControl->TabPageSelected().AddHandler(this, &MainWindow::TopTabPageSelected);
    topTabControl->ControlRemoved().AddHandler(this, &MainWindow::TopTabPageRemoved);
    std::unique_ptr<Control> paddedTopTabControl(new PaddedControl(PaddedControlCreateParams(topTabControlPtr.release()).Defaults()));
    std::unique_ptr<Control> borderedTopTabControl(new BorderedControl(BorderedControlCreateParams(paddedTopTabControl.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(GetColor("code.tab.control.frame")).FocusedSingleBorderColor(GetColor("code.tab.control.frame")).SetDock(Dock::fill)));
    borderedTopTabControl->SetFrameItemName("code.tab.control.frame");
    verticalSplitContainer->Pane1Container()->AddChild(borderedTopTabControl.release());

    std::unique_ptr<TabControl> bottomTabControlPtr(
        new TabControl(TabControlCreateParams().
            BackgroundColor(GetColor("code.tab.control.background")).
            FrameColor(GetColor("code.tab.control.frame")).
            TextColor(GetColor("code.tab.control.text")).
            NormalBackgroundColor(GetColor("code.tab.control.tab.normal.background")).
            SelectedBackgroundColor(GetColor("code.tab.control.tab.selected.background")).
            CloseBoxSelectedColor(GetColor("code.tab.control.close.box.selected.background")).
            SetDock(Dock::fill)));
    bottomTabControl = bottomTabControlPtr.get();
    bottomTabControl->SetBackgroundItemName("code.tab.control.background");
    bottomTabControl->SetFrameItemName("code.tab.control.frame");
    bottomTabControl->SetTextItemName("code.tab.control.text");
    bottomTabControl->SetTabNormalBackgroundItemName("code.tab.control.tab.normal.background");
    bottomTabControl->SetTabSelectedBackgroundItemName("code.tab.control.tab.selected.background");
    bottomTabControl->SetCloseBoxSelectedBackgroundItemName("code.tab.control.close.box.selected.background");
    bottomTabControl->TabPageSelected().AddHandler(this, &MainWindow::BottomTabPageSelected);
    bottomTabControl->ControlRemoved().AddHandler(this, &MainWindow::BottomTabPageRemoved);
    std::unique_ptr<Control> paddedBottomTabControl(new PaddedControl(PaddedControlCreateParams(bottomTabControlPtr.release()).Defaults()));
    std::unique_ptr<Control> borderedBottomTabControl(new BorderedControl(BorderedControlCreateParams(paddedBottomTabControl.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(GetColor("code.tab.control.frame")).FocusedSingleBorderColor(GetColor("code.tab.control.frame")).SetDock(Dock::fill)));
    borderedBottomTabControl->SetFrameItemName("code.tab.control.frame");
    verticalSplitContainer->Pane2Container()->AddChild(borderedBottomTabControl.release());

    AddChild(verticalSplitContainerPtr.release());

    SetState(DebuggingState::debuggerIdle);

    consoleFile.reset(new cmsx::guicon::ConsoleFile());
    cmsx::kernel::SetConsoleFiles(consoleFile.get(), consoleFile.get());

    if (!filePath.empty())
    {
        LoadProcess();
    }
}

MainWindow::~MainWindow()
{
    StopDebugging(true);
}

void MainWindow::NotifyDebuggingStopped()
{
    std::unique_lock<std::mutex> lock(mtx);
    SetState(DebuggingState::debuggerExit);
    debuggingStoppedOrErrorVar.notify_one();
}

void MainWindow::NotifyDebuggerError()
{
    std::unique_lock<std::mutex> lock(mtx);
    SetState(DebuggingState::debuggerError);
    debuggingStoppedOrErrorVar.notify_one();
}

bool MainWindow::ProcessMessage(Message& msg)
{
    switch (msg.message)
    {
        case DEBUGGER_READY:
        {
            SetState(DebuggingState::debuggerWaitingForCommand);
            UpdateViews();
            msg.result = 0;
            return true;
        }
        case DEBUGGER_ERROR:
        {
            SetState(DebuggingState::debuggerError);
            UpdateViews();
            PrintError(observer.ErrorMessage());
            msg.result = 0;
            return true;
        }
        case DEBUGGER_PROCESS_EXIT:
        {
            SetState(DebuggingState::debuggerExit);
            UpdateViews();
            PrintExit();
            msg.result = 0;
            return true;
        }
        default:
        {
            return Window::ProcessMessage(msg);
        }
    }
}

void MainWindow::OnMouseWheel(MouseWheelEventArgs& args)
{
    if (args.value > 0)
    {
        PrevQuarterClick();
    }
    else
    {
        NextQuarterClick();
    }
    args.handled = true;
}

void MainWindow::ToggleBreakpointClick()
{
    try
    {
        debugger->ToggleBreakpoint(codeView->CurrentAddress());
        codeView->Reset();
        codeView->UpdateView(false);
    }
    catch (const std::exception& ex)
    {
        PrintError(ex.what());
    }
}

void MainWindow::SingleStepClick()
{
    try
    {
        if (state == DebuggingState::debuggerWaitingForCommand)
        {
            SetState(DebuggingState::debuggerBusy);
            debugger->SingleStep();
        }
        else
        {
            throw std::runtime_error("debugger not waiting for command");
        }
    }
    catch (const std::exception& ex)
    {
        PrintError(ex.what());
    }
}

void MainWindow::StepOverClick()
{
    try
    {
        if (state == DebuggingState::debuggerWaitingForCommand)
        {
            SetState(DebuggingState::debuggerBusy);
            debugger->StepOver();
        }
        else
        {
            throw std::runtime_error("debugger not waiting for command");
        }
    }
    catch (const std::exception& ex)
    {
        PrintError(ex.what());
    }
}

void MainWindow::ContinueClick()
{
    try
    {
        if (state == DebuggingState::debuggerWaitingForCommand)
        {
            SetState(DebuggingState::debuggerBusy);
            debugger->Continue();
        }
        else
        {
            throw std::runtime_error("debugger not waiting for command");
        }
    }
    catch (const std::exception& ex)
    {
        PrintError(ex.what());
    }
}

void MainWindow::AboutClick()
{
    try
    {
        AboutDialog dialog;
        dialog.ShowDialog(*this);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::OpenFileClick()
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("System X executable files (*.x)", "*.x"));
        std::string initialDirectory = CmajorProjectsDir();
        std::string filePath;
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "x", 
            OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            waitingDebugger = true;
            if (state == DebuggingState::debuggerWaitingForCommand)
            {
                StopDebugging(true);
            }
            this->filePath = GetFullPath(filePath);
            waitingDebugger = false;
            LoadProcess();
            fileOpen = true;
        }
    }
    catch (const std::exception& ex)
    {
        waitingDebugger = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::CloseFileClick()
{
    try
    {
        waitingDebugger = true;
        StopDebugging(true);
        waitingDebugger = false;
        fileOpen = false;
        SetState(DebuggingState::debuggerIdle);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ResetClick()
{
    try
    {
        if (filePath.empty())
        {
            throw std::runtime_error("no executable file open");
        }
        waitingDebugger = true;
        if (state == DebuggingState::debuggerWaitingForCommand || state == DebuggingState::debuggerExit)
        {
            StopDebugging(true);
        }
        waitingDebugger = false;
        LoadProcess();
    }
    catch (const std::exception& ex)
    {
        waitingDebugger = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::StopClick()
{
    try
    {
        waitingDebugger = true;
        if (state == DebuggingState::debuggerWaitingForCommand)
        {
            StopDebugging(true);
        }
        waitingDebugger = false;
    }
    catch (const std::exception& ex)
    {
        waitingDebugger = false;
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ExitClick()
{
    Close();
}

void MainWindow::ViewCodeClick()
{
    if (codeView)
    {
        codeView->SetVisible(true);
        TabPage* tabPage = topTabControl->GetTabPageByKey("code");
        if (tabPage)
        {
            tabPage->Select();
        }
        codeView->UpdateView(true);
    }
    else
    {
        CreateCodeView();
    }
}

void MainWindow::ViewRegsClick()
{
    if (registerView)
    {
        registerView->SetVisible(true);
        TabPage* tabPage = bottomTabControl->GetTabPageByKey("regs");
        if (tabPage)
        {
            tabPage->Select();
        }
        registerView->UpdateView(true);
    }
    else
    {
        CreateRegisterView();
    }
}

void MainWindow::ViewDataClick()
{
    if (dataView)
    {
        dataView->SetVisible(true);
        TabPage* tabPage = topTabControl->GetTabPageByKey("data");
        if (tabPage)
        {
            tabPage->Select();
        }
        dataView->UpdateView(true);
    }
    else
    {
        CreateDataView();
    }
}

void MainWindow::ViewArgsClick()
{
    if (argsView)
    {
        argsView->SetVisible(true);
        TabPage* tabPage = topTabControl->GetTabPageByKey("args");
        if (tabPage)
        {
            tabPage->Select();
        }
        argsView->UpdateView(true);
    }
    else
    {
        CreateArgsView();
    }
}

void MainWindow::ViewEnvClick()
{
    if (envView)
    {
        envView->SetVisible(true);
        TabPage* tabPage = topTabControl->GetTabPageByKey("env");
        if (tabPage)
        {
            tabPage->Select();
        }
        envView->UpdateView(true);
    }
    else
    {
        CreateEnvView();
    }
}

void MainWindow::ViewHeapClick()
{
    if (heapView)
    {
        heapView->SetVisible(true);
        TabPage* tabPage = topTabControl->GetTabPageByKey("heap");
        if (tabPage)
        {
            tabPage->Select();
        }
        heapView->UpdateView(true);
    }
    else
    {
        CreateHeapView();
    }
}

void MainWindow::ViewStackClick()
{
    if (stackView)
    {
        stackView->SetVisible(true);
        TabPage* tabPage = topTabControl->GetTabPageByKey("stack");
        if (tabPage)
        {
            tabPage->Select();
        }
        stackView->UpdateView(true);
    }
    else
    {
        CreateStackView();
    }
}

void MainWindow::ViewLogClick()
{
    if (logView)
    {
        TabPage* tabPage = bottomTabControl->GetTabPageByKey("log");
        if (tabPage)
        {
            tabPage->Select();
        }
    }
    else
    {
        CreateLogView();
    }
}

void MainWindow::NextLineClick()
{
    if (currentTopView)
    {
        currentTopView->NextLine();
    }
}

void MainWindow::PrevLineClick()
{
    if (currentTopView)
    {
        currentTopView->PrevLine();
    }
}

void MainWindow::NextQuarterClick()
{
    if (currentTopView)
    {
        currentTopView->NextQuarter();
    }
}

void MainWindow::PrevQuarterClick()
{
    if (currentTopView)
    {
        currentTopView->PrevQuarter();
    }
}

void MainWindow::NextPageClick()
{
    if (currentTopView)
    {
        currentTopView->NextPage();
    }
}

void MainWindow::PrevPageClick()
{
    if (currentTopView)
    {
        currentTopView->PrevPage();
    }
}

void MainWindow::HomeClick()
{
    if (currentTopView)
    {
        currentTopView->ToStart();
    }
}

void MainWindow::EndClick()
{
    if (currentTopView)
    {
        currentTopView->ToEnd();
    }
}

void MainWindow::PrevAddressClick()
{
    if (currentTopView == codeView)
    {
        codeView->GotoPrevAddress();
    }
}

void MainWindow::TopTabPageSelected()
{
    currentTopView->SetVisible(false);
    TabPage* selectedTabPage = topTabControl->SelectedTabPage();
    if (selectedTabPage->Key() == "code")
    {
        currentTopView = codeView;
    }
    else if (selectedTabPage->Key() == "data")
    {
        currentTopView = dataView;
    }
    else if (selectedTabPage->Key() == "args")
    {
        currentTopView = argsView;
    }
    else if (selectedTabPage->Key() == "env")
    {
        currentTopView = envView;
    }
    else if (selectedTabPage->Key() == "heap")
    {
        currentTopView = heapView;
    }
    else if (selectedTabPage->Key() == "stack")
    {
        currentTopView = stackView;
    }
    currentTopView->SetVisible(true);
    currentTopView->UpdateView(true);
}

void MainWindow::TopTabPageRemoved(ControlEventArgs& controlEventArgs)
{
    TabPage* tabPage = static_cast<TabPage*>(controlEventArgs.control);
    if (tabPage->Key() == "code")
    {
        RemoveView(codeView);
        codeView = nullptr;
    }
    else if (tabPage->Key() == "data")
    {
        RemoveView(dataView);
        dataView = nullptr;
    }
    else if (tabPage->Key() == "args")
    {
        RemoveView(argsView);
        argsView = nullptr;
    }
    else if (tabPage->Key() == "env")
    {
        RemoveView(envView);
        envView = nullptr;
    }
    else if (tabPage->Key() == "heap")
    {
        RemoveView(heapView);
        heapView = nullptr;
    }
    else if (tabPage->Key() == "stack")
    {
        RemoveView(stackView);
        stackView = nullptr;
    }
}

void MainWindow::BottomTabPageSelected()
{
    TabPage* selectedTabPage = bottomTabControl->SelectedTabPage();
    if (selectedTabPage->Key() == "regs")
    {
        currentBottomView->SetVisible(false);
        currentBottomView = registerView;
        currentBottomView->SetVisible(true);
        currentBottomView->UpdateView(true);
    }
}

void MainWindow::BottomTabPageRemoved(ControlEventArgs& controlEventArgs)
{
    TabPage* tabPage = static_cast<TabPage*>(controlEventArgs.control);
    if (tabPage->Key() == "regs")
    {
        RemoveView(registerView);
        registerView = nullptr;
    }
    else if (tabPage->Key() == "log")
    {
        logView = nullptr;
    }
}

void MainWindow::LoadProcess()
{
    SetState(DebuggingState::debuggerBusy);
    machine.reset(new cmsx::machine::Machine());
    cmsx::kernel::Kernel::Instance().SetMachine(machine.get()); 
    cmsx::kernel::Kernel::Instance().Start();
    process = cmsx::kernel::ProcessManager::Instance().CreateProcess();
    process->SetFilePath(filePath);
    args.clear();
    args.push_back(filePath);
    cmsx::kernel::Load(process, args, env, *machine);
    dataRanges.SetMachine(machine.get());
    dataRanges.SetProcess(process);
    if (codeView)
    {
        codeView->Reset();
    }
    else
    {
        CreateCodeView();
    }
    if (!registerView)
    {
        CreateRegisterView();
    }
    if (!logView)
    {
        CreateLogView();
    }
    logView->Clear();
    if (!dataView)
    {
        CreateDataView();
    }
    if (!argsView)
    {
        CreateArgsView();
    }
    if (!envView)
    {
        CreateEnvView();
    }
    if (!heapView)
    {
        CreateHeapView();
    }
    if (!stackView)
    {
        CreateStackView();
    }
    for (DebugView* view : views)
    {
        view->SetMachine(machine.get());
        view->SetProcess(process);
        view->SetVisible(false);
    }
    ViewCodeClick();
    ViewRegsClick();
    if (!console)
    {
        console = new cmajor::wing::Console(cmajor::wing::ConsoleCreateParams().Defaults());
        console->SetFlag(ControlFlags::scrollSubject);
        console->SetDoubleBuffered();
        console->ConsoleInputReady().AddHandler(this, &MainWindow::ConsoleInputReady);
        consoleFile->SetConsole(console);
        cmajor::wing::ScrollableControl* scrollableConsole = new ScrollableControl(ScrollableControlCreateParams(console).SetDock(Dock::fill));
        cmajor::wing::TabPage* consoleTabPage = new cmajor::wing::TabPage("Console", "console");
        consoleTabPage->AddChild(scrollableConsole);
        bottomTabControl->AddTabPage(consoleTabPage);
    }
    ViewRegsClick();
    StartDebugging();
}

void RunDebugger(Debugger* debugger, cmsx::machine::Machine* machine)
{
    debugger->Run();
}

void MainWindow::StartDebugging()
{
    if (!machine)
    {
        return;
    }
    if (debuggerThread.joinable())
    {
        debuggerThread.join();
    }
    debugger.reset(new Debugger(machine.get(), process));
    debugger->SetObserver(&observer);
    for (auto view : views)
    {
        view->SetDebugger(debugger.get());
    }
    debuggerThread = std::thread(RunDebugger, debugger.get(), machine.get());
}

void MainWindow::StopDebugging(bool unloadProcess)
{
    if (debugger)
    {
        debugger->Stop();
    }
    if (debuggerThread.joinable())
    {
        debuggerThread.join();
    }
    debugger.reset();
    if (unloadProcess)
    {
        if (process)
        {
            cmsx::kernel::ProcessManager::Instance().DeleteProcess(process->Id());
            process = nullptr;
        }
        if (machine)
        {
            machine->Exit();
        }
        cmsx::kernel::Kernel::Instance().Stop();
        if (machine)
        {
            machine.reset();
        }
        cmsx::kernel::Kernel::Instance().SetMachine(nullptr);
        dataRanges.SetMachine(nullptr);
        dataRanges.SetProcess(nullptr);
        for (auto& view : views)
        {
            view->SetMachine(nullptr);
            view->SetProcess(nullptr);
        }
        UpdateViews();
    }
    SetState(DebuggingState::debuggerExit);
}

void MainWindow::UpdateViews()
{
    for (DebugView* view : views)
    {
        if (view->IsVisible())
        {
            view->UpdateView(true);
        }
    }
}

void MainWindow::RemoveView(DebugView* debugView)
{
    auto it = std::find(views.begin(), views.end(), debugView);
    if (it != views.end())
    {
        views.erase(it);
    }
}

void MainWindow::CreateCodeView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Code", "code"));
    codeView = new CodeView(CodeViewCreateParams().Defaults());
    codeView->SetMachine(machine.get());
    codeView->SetProcess(process);
    codeView->SetVisible(true);
    views.push_back(codeView);
    currentTopView = codeView;
    tabPage->AddChild(codeView);
    if (topTabControl->TabPages().IsEmpty())
    {
        topTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        topTabControl->InsertTabPageBefore(tabPage.release(), static_cast<TabPage*>(topTabControl->TabPages().FirstChild()));
    }
    codeView->UpdateView(true);
}

void MainWindow::CreateRegisterView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Registers", "regs"));
    registerView = new RegisterView(RegisterViewCreateParams().Defaults());
    registerView->SetMachine(machine.get());
    registerView->SetProcess(process);
    registerView->SetVisible(true);
    views.push_back(registerView);
    currentBottomView = registerView;
    tabPage->AddChild(registerView);
    if (bottomTabControl->TabPages().IsEmpty())
    {
        bottomTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        bottomTabControl->InsertTabPageBefore(tabPage.release(), static_cast<TabPage*>(bottomTabControl->TabPages().FirstChild()));
    }
    registerView->UpdateView(true);
}

void MainWindow::CreateDataView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Data", "data"));
    dataView = new DataView(DataViewCreateParams().Defaults(), dataRanges.GetDataRange("data"));
    dataView->SetMachine(machine.get());
    dataView->SetProcess(process);
    dataView->SetVisible(true);
    views.push_back(dataView);
    currentTopView = dataView;
    tabPage->AddChild(dataView);
    if (topTabControl->TabPages().IsEmpty())
    {
        topTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage(topTabControl, "code"));
    }
    dataView->UpdateView(true);
}

void MainWindow::CreateArgsView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Arguments", "args"));
    argsView = new DataView(DataViewCreateParams().Defaults(), dataRanges.GetDataRange("args"));
    argsView->SetMachine(machine.get());
    argsView->SetProcess(process);
    argsView->SetVisible(true);
    views.push_back(argsView);
    currentTopView = argsView;
    tabPage->AddChild(argsView);
    if (topTabControl->TabPages().IsEmpty())
    {
        topTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage(topTabControl, "data"));
    }
    argsView->UpdateView(true);
}

void MainWindow::CreateEnvView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Environment", "env"));
    envView = new DataView(DataViewCreateParams().Defaults(), dataRanges.GetDataRange("env"));
    envView->SetMachine(machine.get());
    envView->SetProcess(process);
    envView->SetVisible(true);
    views.push_back(envView);
    currentTopView = envView;
    tabPage->AddChild(envView);
    if (topTabControl->TabPages().IsEmpty())
    {
        topTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage(topTabControl, "args"));
    }
    envView->UpdateView(true);
}

void MainWindow::CreateHeapView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Heap", "heap"));
    heapView = new DataView(DataViewCreateParams().Defaults(), dataRanges.GetDataRange("heap"));
    heapView->SetMachine(machine.get());
    heapView->SetProcess(process);
    heapView->SetVisible(true);
    views.push_back(heapView);
    currentTopView = heapView;
    tabPage->AddChild(heapView);
    if (topTabControl->TabPages().IsEmpty())
    {
        topTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage(topTabControl, "env"));
    }
    heapView->UpdateView(true);
}

void MainWindow::CreateStackView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Stack", "stack"));
    stackView = new DataView(DataViewCreateParams().Defaults(), dataRanges.GetDataRange("stack"));
    stackView->SetMachine(machine.get());
    stackView->SetProcess(process);
    stackView->SetVisible(true);
    views.push_back(stackView);
    currentTopView = stackView;
    tabPage->AddChild(stackView);
    if (topTabControl->TabPages().IsEmpty())
    {
        topTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage(topTabControl, "heap"));
    }
    stackView->UpdateView(true);
}

void MainWindow::CreateLogView()
{
    std::unique_ptr<TabPage> tabPage(new TabPage("Log", "log"));
    logView = new LogView(TextViewCreateParams().SetDock(Dock::fill));
    tabPage->AddChild(logView);
    if (bottomTabControl->TabPages().IsEmpty())
    {
        bottomTabControl->AddTabPage(tabPage.release());
    }
    else
    {
        bottomTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage(bottomTabControl, "regs"));
    }
}

TabPage* MainWindow::GetTabPageByNameOrFirstTabPage(TabControl* tabControl, const std::string& tabName) const
{
    TabPage* tabPage = tabControl->GetTabPageByKey(tabName);
    if (tabPage)
    {
        return tabPage;
    }
    else if (!tabControl->TabPages().IsEmpty())
    {
        return static_cast<TabPage*>(tabControl->TabPages().FirstChild());
    }
    else
    {
        return nullptr;
    }
}

void MainWindow::WaitUntilDebuggingStoppedOrError()
{
    std::unique_lock<std::mutex> lock(mtx);
    debuggingStoppedOrErrorVar.wait(lock, [this] { return state == DebuggingState::debuggerExit || state == DebuggingState::debuggerError; });
    waitingDebugger = false;
    switch (state)
    {
        case DebuggingState::debuggerError:
        {
            std::string errorMessage = observer.ErrorMessage();
            PrintError(errorMessage);
            break;
        }
        case DebuggingState::debuggerExit:
        {
            PrintExit();
            break;
        }
    }
}

void MainWindow::PrintError(const std::string& errorMessage)
{
    ViewLogClick();
    logView->WriteLine(errorMessage);
}

void MainWindow::PrintExit()
{
    uint8_t exitCode = 255;
    if (process)
    {
        exitCode = process->ExitCode();
    }
    ViewLogClick();
    logView->WriteLine("process exited with code " + std::to_string(static_cast<int>(exitCode)));
}

void MainWindow::SetState(DebuggingState state_)
{
    state = state_;
    startContinueMenuItem->Disable();
    stopMenuItem->Disable();
    resetMenuItem->Disable();
    singleStepMenuItem->Disable();
    stepOverMenuItem->Disable();
    toggleBreakpointMenuItem->Disable();
    gotoPrevAddressMenuItem->Disable();
    closeFileMenuItem->Disable();
    switch (state)
    {
        case DebuggingState::debuggerBusy:
        {
            stopMenuItem->Enable();
            break;
        }
        case DebuggingState::debuggerWaitingForCommand:
        {
            startContinueMenuItem->Enable();
            stopMenuItem->Enable();
            resetMenuItem->Enable();
            singleStepMenuItem->Enable();
            stepOverMenuItem->Enable();
            toggleBreakpointMenuItem->Enable();
            gotoPrevAddressMenuItem->Enable();
            if (fileOpen)
            {
                closeFileMenuItem->Enable();
            }
            break;
        }
        case DebuggingState::debuggerError:
        {
            resetMenuItem->Enable();
            if (fileOpen)
            {
                closeFileMenuItem->Enable();
            }
            break;
        }
        case DebuggingState::debuggerExit:
        {
            resetMenuItem->Enable();
            if (fileOpen)
            {
                closeFileMenuItem->Enable();
            }
            break;
        }
    }
}

void MainWindow::ConsoleInputReady()
{
}

} // namespace cmsx::db
