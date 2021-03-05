// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/MainWindow.hpp>
#include <sngcm/ast/Project.hpp>
#include <cmajor/wing/Dialog.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/RequestDispatcher.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmcode {

using namespace cmajor::service;
using namespace sngcm::ast;
using namespace soulng::unicode;
using namespace soulng::util;

std::string CmajorProjectDirectory()
{
    return GetFullPath(Path::Combine(CmajorRootDir(), "projects"));
}

MainWindow::MainWindow(const std::string& filePath) : Window(WindowCreateParams().Text("Cmajor Code")),
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
    optionsMenuItem(nullptr),
    callStackMenuItem(nullptr),
    localsMenuItem(nullptr),
    errorsMenuItem(nullptr),
    searchResultsMenuItem(nullptr),
    portMapMenuItem(nullptr),
    buildSolutionMenuItem(nullptr),
    rebuildSolutionMenuItem(nullptr),
    cleanSolutionMenuItem(nullptr),
    buildActiveProjectMenuItem(nullptr),
    rebuildActiveProjectMenuItem(nullptr),
    cleanActiveProjectMenuItem(nullptr),
    startDebuggingMenuItem(nullptr),
    startWithoutDebuggingMenuItem(nullptr),
    terminateProcessMenuItem(nullptr),
    stopDebuggingMenuItem(nullptr),
    showNextStatementMenuItem(nullptr),
    stepOverMenuItem(nullptr),
    stepIntoMenuItem(nullptr),
    stepOutMenuItem(nullptr),
    toggleBreakpointMenuItem(nullptr),
    programArgumentsMenuItem(nullptr),
    closeAllTabsMenuItem(nullptr),
    closeExternalTabsMenuItem(nullptr),
    homepageMenuItem(nullptr),
    localDocumentationMenuItem(nullptr),
    aboutMenuItem(nullptr),
    verticalSplitContainer(nullptr),
    horizontalSplitContainer(nullptr),
    solutionTreeView(nullptr),
    state(MainWindowState::idle)
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
    std::unique_ptr<MenuItem> viewMenuItem(new MenuItem("&View"));
    std::unique_ptr<MenuItem> debugWindowsMenuItem(new MenuItem("&Debug Windows"));
    std::unique_ptr<MenuItem> callStackMenuItemPtr(new MenuItem("&Call Stack"));
    callStackMenuItem = callStackMenuItemPtr.get();
    callStackMenuItem->Click().AddHandler(this, &MainWindow::CallStackClick);
    debugWindowsMenuItem->AddMenuItem(callStackMenuItemPtr.release());
    std::unique_ptr<MenuItem> localsMenuItemPtr(new MenuItem("&Locals"));
    localsMenuItem = localsMenuItemPtr.get();
    localsMenuItem->Click().AddHandler(this, &MainWindow::LocalsClick);
    debugWindowsMenuItem->AddMenuItem(localsMenuItemPtr.release());
    viewMenuItem->AddMenuItem(debugWindowsMenuItem.release());
    std::unique_ptr<MenuItem> errorsMenuItemPtr(new MenuItem("&Errors"));
    errorsMenuItem = errorsMenuItemPtr.get();
    errorsMenuItem->Click().AddHandler(this, &MainWindow::ErrorsClick);
    viewMenuItem->AddMenuItem(errorsMenuItemPtr.release());
    std::unique_ptr<MenuItem> searchResultsMenuItemPtr(new MenuItem("&Search Results"));
    searchResultsMenuItem = searchResultsMenuItemPtr.get();
    searchResultsMenuItem->Click().AddHandler(this, &MainWindow::SearchResultsClick);
    viewMenuItem->AddMenuItem(searchResultsMenuItemPtr.release());
    std::unique_ptr<MenuItem> portMapMenuItemPtr(new MenuItem("&Port Map"));
    portMapMenuItem = portMapMenuItemPtr.get();
    portMapMenuItem->Click().AddHandler(this, &MainWindow::PortMapClick);
    viewMenuItem->AddMenuItem(portMapMenuItemPtr.release());
    menuBar->AddMenuItem(viewMenuItem.release());
    std::unique_ptr<MenuItem> buildMenuItem(new MenuItem("&Build"));
    std::unique_ptr<MenuItem> buildSolutionMenuItemPtr(new MenuItem("&Build Solution"));
    buildSolutionMenuItem = buildSolutionMenuItemPtr.get();
    buildSolutionMenuItem->SetShortcut(Keys::f7);
    buildSolutionMenuItem->Click().AddHandler(this, &MainWindow::BuildSolutionClick);
    buildMenuItem->AddMenuItem(buildSolutionMenuItemPtr.release());
    std::unique_ptr<MenuItem> rebuildSolutionMenuItemPtr(new MenuItem("&Rebuild Solution"));
    rebuildSolutionMenuItem = rebuildSolutionMenuItemPtr.get();
    rebuildSolutionMenuItem->SetShortcut(Keys::altModifier | Keys::f7);
    rebuildSolutionMenuItem->Click().AddHandler(this, &MainWindow::RebuildSolutionClick);
    buildMenuItem->AddMenuItem(rebuildSolutionMenuItemPtr.release());
    std::unique_ptr<MenuItem> cleanSolutionMenuItemPtr(new MenuItem("&Clean Solution"));
    cleanSolutionMenuItem = cleanSolutionMenuItemPtr.get();
    cleanSolutionMenuItem->Click().AddHandler(this, &MainWindow::CleanSolutionClick);
    buildMenuItem->AddMenuItem(new MenuItemSeparator());
    std::unique_ptr<MenuItem> buildActiveProjectMenuItemPtr(new MenuItem("Build &Active Project"));
    buildActiveProjectMenuItem = buildActiveProjectMenuItemPtr.get();
    buildActiveProjectMenuItem->SetShortcut(Keys::controlModifier | Keys::b);
    buildActiveProjectMenuItem->Click().AddHandler(this, &MainWindow::BuildActiveProjectClick);
    buildMenuItem->AddMenuItem(buildActiveProjectMenuItemPtr.release());
    std::unique_ptr<MenuItem> rebuildActiveProjectMenuItemPtr(new MenuItem("Reb&uild Active Project"));
    rebuildActiveProjectMenuItem = rebuildActiveProjectMenuItemPtr.get();
    rebuildActiveProjectMenuItem->Click().AddHandler(this, &MainWindow::RebuildActiveProjectClick);
    buildMenuItem->AddMenuItem(rebuildActiveProjectMenuItemPtr.release());
    std::unique_ptr<MenuItem> cleanActiveProjectMenuItemPtr(new MenuItem("C&lean Active Project"));
    cleanActiveProjectMenuItem = cleanActiveProjectMenuItemPtr.get();
    cleanActiveProjectMenuItem->Click().AddHandler(this, &MainWindow::CleanActiveProjectClick);
    buildMenuItem->AddMenuItem(cleanActiveProjectMenuItemPtr.release());
    menuBar->AddMenuItem(buildMenuItem.release());
    std::unique_ptr<MenuItem> debugMenuItem(new MenuItem("&Debug"));
    std::unique_ptr<MenuItem> startDebuggingMenuItemPtr(new MenuItem("&Start Debugging"));
    startDebuggingMenuItem = startDebuggingMenuItemPtr.get();
    startDebuggingMenuItem->SetShortcut(Keys::f5);
    startDebuggingMenuItem->Click().AddHandler(this, &MainWindow::StartDebuggingClick);
    debugMenuItem->AddMenuItem(startDebuggingMenuItemPtr.release());
    std::unique_ptr<MenuItem> startWithoutDebuggingMenuItemPtr(new MenuItem("Start &Without Debugging"));
    startWithoutDebuggingMenuItem = startWithoutDebuggingMenuItemPtr.get();
    startWithoutDebuggingMenuItem->SetShortcut(Keys::controlModifier | Keys::f5);
    startWithoutDebuggingMenuItem->Click().AddHandler(this, &MainWindow::StartWithoutDebuggingClick);
    debugMenuItem->AddMenuItem(startWithoutDebuggingMenuItemPtr.release());
    std::unique_ptr<MenuItem> terminateProcessMenuItemPtr(new MenuItem("&Terminate Process"));
    terminateProcessMenuItem = terminateProcessMenuItemPtr.get();
    terminateProcessMenuItem->Click().AddHandler(this, &MainWindow::TerminateProcessClick);
    debugMenuItem->AddMenuItem(terminateProcessMenuItemPtr.release());
    std::unique_ptr<MenuItem> stopDebuggingMenuItemPtr(new MenuItem("Sto&p Debugging"));
    stopDebuggingMenuItem = stopDebuggingMenuItemPtr.get();
    stopDebuggingMenuItem->SetShortcut(Keys::shiftModifier | Keys::f5);
    stopDebuggingMenuItem->Click().AddHandler(this, &MainWindow::StopDebuggingClick);
    debugMenuItem->AddMenuItem(stopDebuggingMenuItemPtr.release());
    debugMenuItem->AddMenuItem(new MenuItemSeparator());
    std::unique_ptr<MenuItem> showNextStatementMenuItemPtr(new MenuItem("S&how Next Statement"));
    showNextStatementMenuItem = showNextStatementMenuItemPtr.get();
    showNextStatementMenuItem->SetShortcut(Keys::controlModifier | Keys::j);
    showNextStatementMenuItem->Click().AddHandler(this, &MainWindow::ShowNextStatementClick);
    debugMenuItem->AddMenuItem(showNextStatementMenuItemPtr.release());
    std::unique_ptr<MenuItem> stepOverMenuItemPtr(new MenuItem("Step &Over"));
    stepOverMenuItem = stepOverMenuItemPtr.get();
    stepOverMenuItem->SetShortcut(Keys::f12);
    stepOverMenuItem->Click().AddHandler(this, &MainWindow::StepOverClick);
    debugMenuItem->AddMenuItem(stepOverMenuItemPtr.release());
    std::unique_ptr<MenuItem> stepIntoMenuItemPtr(new MenuItem("Step &Into"));
    stepIntoMenuItem = stepIntoMenuItemPtr.get();
    stepIntoMenuItem->SetShortcut(Keys::f11);
    stepIntoMenuItem->Click().AddHandler(this, &MainWindow::StepIntoClick);
    debugMenuItem->AddMenuItem(stepIntoMenuItemPtr.release());
    std::unique_ptr<MenuItem> stepOutMenuItemPtr(new MenuItem("Step O&ut"));
    stepOutMenuItem = stepOutMenuItemPtr.get();
    stepOutMenuItem->SetShortcut(Keys::shiftModifier | Keys::f11);
    stepOutMenuItem->Click().AddHandler(this, &MainWindow::StepOutClick);
    debugMenuItem->AddMenuItem(stepOutMenuItemPtr.release());
    debugMenuItem->AddMenuItem(new MenuItemSeparator());
    std::unique_ptr<MenuItem> toggleBreakpointMenuItemPtr(new MenuItem("&Toggle Breakpoint"));
    toggleBreakpointMenuItem = toggleBreakpointMenuItemPtr.get();
    toggleBreakpointMenuItem->SetShortcut(Keys::f9);
    toggleBreakpointMenuItem->Click().AddHandler(this, &MainWindow::ToggleBreakpointClick);
    debugMenuItem->AddMenuItem(toggleBreakpointMenuItemPtr.release());
    debugMenuItem->AddMenuItem(new MenuItemSeparator());
    std::unique_ptr<MenuItem> programArgumentsMenuItemPtr(new MenuItem("Program &Arguments..."));
    programArgumentsMenuItem = programArgumentsMenuItemPtr.get();
    programArgumentsMenuItem->Click().AddHandler(this, &MainWindow::ProgramArgumentsClick);
    debugMenuItem->AddMenuItem(programArgumentsMenuItemPtr.release());
    menuBar->AddMenuItem(debugMenuItem.release());
    std::unique_ptr<MenuItem> windowMenuItem(new MenuItem("&Window"));
    std::unique_ptr<MenuItem> closeAllTabsMenuItemPtr(new MenuItem("&Close All Tabs"));
    closeAllTabsMenuItem = closeAllTabsMenuItemPtr.get();
    closeAllTabsMenuItem->Click().AddHandler(this, &MainWindow::CloseAllTabsClick);
    windowMenuItem->AddMenuItem(closeAllTabsMenuItemPtr.release());
    std::unique_ptr<MenuItem> closeExternalTabsMenuItemPtr(new MenuItem("Close &External Tabs"));
    closeExternalTabsMenuItem = closeExternalTabsMenuItemPtr.get();
    closeExternalTabsMenuItem->SetShortcut(Keys::controlModifier | Keys::w);
    closeExternalTabsMenuItem->Click().AddHandler(this, &MainWindow::CloseExternalTabsClick);
    windowMenuItem->AddMenuItem(closeExternalTabsMenuItemPtr.release());
    menuBar->AddMenuItem(windowMenuItem.release());
    std::unique_ptr<MenuItem> helpMenuItem(new MenuItem("&Help"));
    std::unique_ptr<MenuItem> homepageMenuItemPtr(new MenuItem("&Homepage"));
    homepageMenuItem = homepageMenuItemPtr.get();
    homepageMenuItem->Click().AddHandler(this, &MainWindow::HomepageClick);
    helpMenuItem->AddMenuItem(homepageMenuItemPtr.release());
    std::unique_ptr<MenuItem> localDocumentationMenuItemPtr(new MenuItem("&Local Documentation"));
    localDocumentationMenuItem = localDocumentationMenuItemPtr.get();
    localDocumentationMenuItem->Click().AddHandler(this, &MainWindow::LocalDocumentationClick);
    helpMenuItem->AddMenuItem(localDocumentationMenuItemPtr.release());
    std::unique_ptr<MenuItem> aboutMenuItemPtr(new MenuItem("&About..."));
    aboutMenuItem = aboutMenuItemPtr.get();
    aboutMenuItem->Click().AddHandler(this, &MainWindow::AboutClick);
    helpMenuItem->AddMenuItem(aboutMenuItemPtr.release());
    menuBar->AddMenuItem(helpMenuItem.release());
    AddChild(menuBar.release());

    std::unique_ptr<SplitContainer> verticalSplitContainerPtr(
        new SplitContainer(SplitContainerCreateParams(SplitterOrientation::vertical).SplitterDistance(VerticalSplitterDistance()).SetDock(Dock::fill)));
    verticalSplitContainer = verticalSplitContainerPtr.get();
    std::unique_ptr<SplitContainer> horizontalSplitContainerPtr(
        new SplitContainer(SplitContainerCreateParams(SplitterOrientation::horizontal).SplitterDistance(HorizontalSplitterDistance()).SetDock(Dock::fill)));
    horizontalSplitContainer = horizontalSplitContainerPtr.get();
    std::unique_ptr<TreeView> solutionTreeViewPtr(new TreeView(TreeViewCreateParams().Defaults()));
    solutionTreeView = solutionTreeViewPtr.get();
    solutionTreeView->SetDoubleBuffered();
    std::unique_ptr<PaddedControl> paddedTreeViewPtr(new PaddedControl(PaddedControlCreateParams(solutionTreeViewPtr.release()).Defaults()));
    std::unique_ptr<ScrollableControl> scrollableTreeViewPtr(new ScrollableControl(ScrollableControlCreateParams(paddedTreeViewPtr.release()).SetDock(Dock::fill)));
    horizontalSplitContainer->Pane2Container()->AddChild(scrollableTreeViewPtr.release());
    verticalSplitContainer->Pane1Container()->AddChild(horizontalSplitContainerPtr.release());
    AddChild(verticalSplitContainerPtr.release());

    SetServiceMessageHandlerView(this);
    StartRequestDispatcher();

    if (!filePath.empty())
    {
        OpenProject(filePath);
    }
}

MainWindow::~MainWindow()
{
    StopRequestDispatcher();
}

void MainWindow::OnWindowClosing(CancelArgs& args)
{
    try
    {
        Window::OnWindowClosing(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

bool MainWindow::ProcessMessage(Message& msg)
{
    switch (msg.message)
    {
        case SM_SERVICE_MESSAGE_AVAILABLE:
        {
            HandleServiceMessage();
            msg.result = 0;
            return true;
        }
        default:
        {
            return Window::ProcessMessage(msg);
        }
    }
}

void MainWindow::HandleServiceMessage()
{
    std::unique_ptr<ServiceMessage> serviceMessage = GetServiceMessage();
    if (serviceMessage)
    {
        switch (serviceMessage->Kind())
        {
            case ServiceMessageKind::clearOutput:
            {
                ClearOutput();
                break;
            }
            case ServiceMessageKind::outputMessage:
            {
                OutputServiceMessage* message = static_cast<OutputServiceMessage*>(serviceMessage.get());
                WriteOutput(message->Text());
                break;
            }
        }
    }
}

void MainWindow::ClearOutput()
{
    // todo
}

void MainWindow::WriteOutput(const std::string& text)
{
    // todo
}

void MainWindow::OpenProject(const std::string& filePath)
{
    try
    {
        std::vector<std::unique_ptr<std::u32string>> contents;
        std::unique_ptr<sngcm::ast::Solution> solution;
        if (!filePath.empty() && boost::filesystem::exists(filePath))
        {
            std::string ext = Path::GetExtension(filePath); 
            if (ext == ".cms")
            {
                 solution = ReadSolution(filePath, contents);
            }
            else if (ext == ".cmp")
            {
                std::string solutionFilePath = Path::ChangeExtension(filePath, ".cms");
                if (boost::filesystem::exists(solutionFilePath))
                {
                    solution = ReadSolution(solutionFilePath, contents);
                }
                else
                {
                    std::unique_ptr<sngcm::ast::Project> project = ReadProject(filePath, contents);
                    solution.reset(new sngcm::ast::Solution(project->Name(), solutionFilePath));
                    solution->SetActiveProject(project.get());
                    solution->AddProject(std::move(project));
                    solution->Save();
                }
            }
            else
            {
                throw std::runtime_error("file path '" + filePath + "' has invalid extension (not .cmp or .cms)");
            }
        }
        else
        {
            throw std::runtime_error("file path '" + filePath + "' is empty or does not exist");
        }
        // todo close code tab control files
        solutionData.reset(new SolutionData(std::move(solution), solutionTreeView));
        SetState(MainWindowState::idle);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::SetState(MainWindowState state_)
{
    state = state_;
    // todo
}

int MainWindow::VerticalSplitterDistance()
{
    return 0;
}

int MainWindow::HorizontalSplitterDistance()
{
    return 0;
}

void MainWindow::NewProjectClick()
{
    ShowInfoMessageBox(Handle(), "New Project");
}

void MainWindow::OpenProjectClick()
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Cmajor Solution Files (*.cms)", "*.cms"));
        descriptionFilterPairs.push_back(std::make_pair("Cmajor Project Files (*.cmp)", "*.cmp"));
        std::string initialDirectory = CmajorProjectDirectory();
        std::string filePath;
        std::string currentDiretory;
        std::vector<std::string> fileNames;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "cms", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDiretory, fileNames);
        if (selected)
        {
            OpenProject(GetFullPath(filePath));
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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

void MainWindow::CallStackClick()
{
    ShowInfoMessageBox(Handle(), "Call Stack");
}

void MainWindow::LocalsClick()
{
    ShowInfoMessageBox(Handle(), "Locals");
}

void MainWindow::ErrorsClick()
{
    ShowInfoMessageBox(Handle(), "Errors");
}

void MainWindow::SearchResultsClick()
{
    ShowInfoMessageBox(Handle(), "Search Results");
}

void MainWindow::PortMapClick()
{
    ShowInfoMessageBox(Handle(), "Port Map");
}

void MainWindow::BuildSolutionClick()
{
    ShowInfoMessageBox(Handle(), "Build Solution");
}

void MainWindow::RebuildSolutionClick()
{
    ShowInfoMessageBox(Handle(), "Rebuild Solution");
}

void MainWindow::CleanSolutionClick()
{
    ShowInfoMessageBox(Handle(), "Clean Solution");
}

void MainWindow::BuildActiveProjectClick()
{
    ShowInfoMessageBox(Handle(), "Build Active Project");
}

void MainWindow::RebuildActiveProjectClick()
{
    ShowInfoMessageBox(Handle(), "Rebuild Active Project");
}

void MainWindow::CleanActiveProjectClick()
{
    ShowInfoMessageBox(Handle(), "Clean Active Project");
}

void MainWindow::StartDebuggingClick()
{
    ShowInfoMessageBox(Handle(), "Start Debugging");
}

void MainWindow::StartWithoutDebuggingClick()
{
    ShowInfoMessageBox(Handle(), "Start Without Debugging");
}

void MainWindow::TerminateProcessClick()
{
    ShowInfoMessageBox(Handle(), "Terminate Process");
}

void MainWindow::StopDebuggingClick()
{
    ShowInfoMessageBox(Handle(), "Stop Debugging");
}

void MainWindow::ShowNextStatementClick()
{
    ShowInfoMessageBox(Handle(), "Show Next Statement");
}

void MainWindow::StepOverClick()
{
    ShowInfoMessageBox(Handle(), "Step Over");
}

void MainWindow::StepIntoClick()
{
    ShowInfoMessageBox(Handle(), "Step Into");
}

void MainWindow::StepOutClick()
{
    ShowInfoMessageBox(Handle(), "Step Out");
}

void MainWindow::ToggleBreakpointClick()
{
    ShowInfoMessageBox(Handle(), "Toggle Breakpoint");
}

void MainWindow::ProgramArgumentsClick()
{
    ShowInfoMessageBox(Handle(), "Program Arguments");
}

void MainWindow::CloseAllTabsClick()
{
    ShowInfoMessageBox(Handle(), "Close All Tabs");
}

void MainWindow::CloseExternalTabsClick()
{
    ShowInfoMessageBox(Handle(), "Close External Tabs");
}

void MainWindow::HomepageClick()
{
    ShowInfoMessageBox(Handle(), "Homepage");
}

void MainWindow::LocalDocumentationClick()
{
    ShowInfoMessageBox(Handle(), "Local Documentation");
}

void MainWindow::AboutClick()
{
    ShowInfoMessageBox(Handle(), "About");
}

} // namespace cmcode
