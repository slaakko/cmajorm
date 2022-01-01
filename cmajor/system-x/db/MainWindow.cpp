// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/MainWindow.hpp>
#include <system-x/db/AboutDialog.hpp>
#include <system-x/os/Load.hpp>
#include <system-x/os/ProcessManager.hpp>
#include <wing/Theme.hpp>
#include <wing/PaddedControl.hpp>
#include <wing/BorderedControl.hpp>
#include <soulng/util/Path.hpp>

#undef min
#undef max

namespace cmsx::db {

const char* cmsxDBVersion = "4.3.0";

using namespace soulng::util;

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
    SendMessage(mainWindow->Handle(), DEBUGGER_ERROR, 0, 0);
}

void MainDebuggerObserver::DebuggerProcessExit()
{
    SendMessage(mainWindow->Handle(), DEBUGGER_PROCESS_EXIT, 0, 0);
}

MainWindow::MainWindow(const std::string& filePath_, const std::vector<std::string>& args_, const std::vector<std::string>& env_) : 
    Window(WindowCreateParams().Text("System X Debugger").BackgroundColor(GetColor("window.background")).WindowClassName("system.x.db.MainWindow")),
    observer(this), filePath(GetFullPath(filePath_)), args(args_), env(env_), machine(nullptr), process(nullptr), codeView(nullptr), registerView(nullptr), 
    dataView(nullptr), argsView(nullptr), envView(nullptr), heapView(nullptr), stackView(nullptr), currentTopView(nullptr), currentBottomView(nullptr)
{
    std::unique_ptr<MenuBar> menuBar(new MenuBar());
    std::unique_ptr<MenuItem> fileMenuItem(new MenuItem("&File"));
    std::unique_ptr<MenuItem> openFileMenuItemPtr(new MenuItem("&Open..."));
    openFileMenuItem = openFileMenuItemPtr.get();
    openFileMenuItem->SetShortcut(Keys::controlModifier | Keys::o);
    openFileMenuItem->Click().AddHandler(this, &MainWindow::OpenFileClick);
    fileMenuItem->AddMenuItem(openFileMenuItemPtr.release());
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

    if (!filePath.empty())
    {
        LoadProcess();
    }
}

MainWindow::~MainWindow()
{
    StopDebugging();
}

bool MainWindow::ProcessMessage(Message& msg)
{
    switch (msg.message)
    {
        case DEBUGGER_READY:
        {
            UpdateViews();
            msg.result = 0;
            return true;
        }
        case DEBUGGER_ERROR:
        {
            // todo
            msg.result = 0;
            return true;
        }
        case DEBUGGER_PROCESS_EXIT:
        {
            // todo
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
    // todo
}

void MainWindow::ExitClick()
{
    Close();
}

void MainWindow::ViewCodeClick()
{

    if (codeView)
    {
        TabPage* tabPage = topTabControl->GetTabPageByKey("code");
        if (tabPage)
        {
            tabPage->Select();
        }
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
        TabPage* tabPage = bottomTabControl->GetTabPageByKey("regs");
        if (tabPage)
        {
            tabPage->Select();
        }
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
        TabPage* tabPage = topTabControl->GetTabPageByKey("data");
        if (tabPage)
        {
            tabPage->Select();
        }
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
        TabPage* tabPage = topTabControl->GetTabPageByKey("args");
        if (tabPage)
        {
            tabPage->Select();
        }
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
        TabPage* tabPage = topTabControl->GetTabPageByKey("env");
        if (tabPage)
        {
            tabPage->Select();
        }
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
        TabPage* tabPage = topTabControl->GetTabPageByKey("heap");
        if (tabPage)
        {
            tabPage->Select();
        }
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
        TabPage* tabPage = topTabControl->GetTabPageByKey("stack");
        if (tabPage)
        {
            tabPage->Select();
        }
    }
    else
    {
        CreateStackView();
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
    currentTopView->UpdateView();
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
    currentBottomView->SetVisible(false);
    TabPage* selectedTabPage = bottomTabControl->SelectedTabPage();
    if (selectedTabPage->Key() == "regs")
    {
        currentBottomView = registerView;
    }
    currentBottomView->SetVisible(true);
    currentBottomView->UpdateView();
}

void MainWindow::BottomTabPageRemoved(ControlEventArgs& controlEventArgs)
{
    TabPage* tabPage = static_cast<TabPage*>(controlEventArgs.control);
    if (tabPage->Key() == "regs")
    {
        RemoveView(registerView);
        registerView = nullptr;
    }
    else
    {

    }
}

void MainWindow::LoadProcess()
{
    machine.reset(new cmsx::machine::Machine());
    cmsx::os::ProcessManager::Instance().SetMachine(machine.get());
    process = cmsx::os::ProcessManager::Instance().CreateProcess();
    process->SetFilePath(filePath);
    cmsx::os::Load(process, args, env, *machine);
    cmsx::os::ProcessManager::Instance().SetCurrentProcess(process);
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
    }
    ViewCodeClick();
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
    debugger.reset(new Debugger(*machine));
    debugger->SetObserver(&observer);
    debuggerThread = std::thread(RunDebugger, debugger.get(), machine.get());
}

void MainWindow::StopDebugging()
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
    if (process)
    {
        cmsx::os::ProcessManager::Instance().DeleteProcess(process->Id());
        process = nullptr;
    }
    machine.reset();
    cmsx::os::ProcessManager::Instance().SetMachine(nullptr);
}

void MainWindow::UpdateViews()
{
    for (DebugView* view : views)
    {
        if (view->IsVisible())
        {
            view->UpdateView();
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
    codeView->UpdateView();
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
    registerView->UpdateView();
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
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage("code"));
    }
    dataView->UpdateView();
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
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage("data"));
    }
    argsView->UpdateView();
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
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage("args"));
    }
    envView->UpdateView();
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
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage("env"));
    }
    heapView->UpdateView();
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
        topTabControl->InsertTabPageAfter(tabPage.release(), GetTabPageByNameOrFirstTabPage("heap"));
    }
    stackView->UpdateView();
}

TabPage* MainWindow::GetTabPageByNameOrFirstTabPage(const std::string& tabName) const
{
    TabPage* tabPage = topTabControl->GetTabPageByKey(tabName);
    if (tabPage)
    {
        return tabPage;
    }
    else if (!topTabControl->TabPages().IsEmpty())
    {
        return static_cast<TabPage*>(topTabControl->TabPages().FirstChild());
    }
    else
    {
        return nullptr;
    }
}

} // namespace cmsx::db
