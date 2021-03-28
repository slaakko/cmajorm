// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/MainWindow.hpp>
#include <cmajor/cmcode/Config.hpp>
#include <cmajor/cmcode/Action.hpp>
#include <cmajor/cmcode/Build.hpp>
#include <cmajor/cmcode/ToolBar.hpp>
#include <cmajor/cmcode/AddNewProjectDialog.hpp>
#include <cmajor/cmcode/NewProjectDialog.hpp>
#include <cmajor/cmcode/AddNewSourceFileDialog.hpp>
#include <cmajor/cmcode/AddNewResourceFileDialog.hpp>
#include <cmajor/cmcode/AddNewTextFileDialog.hpp>
#include <cmajor/cmcode/SelectProjectTypeDialog.hpp>
#include <cmajor/wing/Dialog.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <cmajor/wing/ToolBar.hpp>
#include <cmajor/wing/MessageBox.hpp>
#include <cmajor/cmsvc/Config.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/RequestDispatcher.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>
#include <cmajor/cmsvc/BuildService.hpp>
#include <cmajor/cmsvc/BuildServiceRequest.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/rex/Context.hpp>
#include <soulng/rex/Match.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmcode {

using namespace cmajor::service;
using namespace sngcm::ast;
using namespace soulng::unicode;
using namespace soulng::util;

struct ShowDialogGuard
{
    ShowDialogGuard(bool& showingDialog_) : showingDialog(showingDialog_)
    {
        showingDialog = true;
    }
    ~ShowDialogGuard()
    {
        showingDialog = false;
    }
    bool& showingDialog;
};

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
    prevToolButton(nullptr),
    nextToolButton(nullptr),
    saveToolButton(nullptr), 
    saveAllToolButton(nullptr),
    cppToolButton(nullptr),
    llvmToolButton(nullptr),
    debugToolButton(nullptr),
    releaseToolButton(nullptr),
    buildSolutionToolButton(nullptr),
    buildActiveProjectToolButton(nullptr),
    stopBuildServerToolButton(nullptr),
    startDebuggingToolButton(nullptr),
    stopDebuggingToolButton(nullptr),
    showNextStatementToolButton(nullptr),
    stepOverToolButton(nullptr),
    stepIntoToolButton(nullptr),
    stepOutToolButton(nullptr),
    verticalSplitContainer(nullptr),
    horizontalSplitContainer(nullptr),
    codeTabControl(nullptr),
    solutionTreeView(nullptr),
    outputTabControl(nullptr),
    outputTabPage(nullptr),
    outputLogView(nullptr),
    errorTabPage(nullptr),
    errorView(nullptr),
    logTabPage(nullptr),
    log(nullptr),
    statusBar(nullptr),
    searchResultsTabPage(nullptr),
    searchResultsView(nullptr),
    buildIndicatorStatuBarItem(nullptr), 
    editorReadWriteIndicatorStatusBarItem(nullptr),
    editorDirtyIndicatorStatusBarItem(nullptr),
    sourceFilePathStatusBarItem(nullptr),
    lineStatusBarItem(nullptr),
    columnStatusBarItem(nullptr),
    buildProgressCounter(0),
    buildProgressTimerRunning(false),
    setMaximizedSplitterDistance(false),
    sizeChanged(false),
    verticalSplitContainerFactor(0),
    horizontalSplitContainerFactor(0),
    showingDialog(false),
    state(MainWindowState::idle),
    backend("cpp"),
    config("debug"),
    cmajorCodeFormat("cmajor.code"),
    locations(this)
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
    std::unique_ptr<MenuItem> gotoMenuItemPtr(new MenuItem("&Go To Line..."));
    gotoMenuItem = gotoMenuItemPtr.get();
    gotoMenuItem->SetShortcut(Keys::controlModifier | Keys::g);
    gotoMenuItem->Click().AddHandler(this, &MainWindow::GotoClick);
    editMenuItem->AddMenuItem(gotoMenuItemPtr.release());
    std::unique_ptr<MenuItem> searchMenuItemPtr(new MenuItem("&Search..."));
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
    buildMenuItem->AddMenuItem(cleanSolutionMenuItemPtr.release());
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

    std::unique_ptr<ToolBar> toolBarPtr(MakeToolBar());
    ToolBar* toolBar = toolBarPtr.get();
    std::unique_ptr<Control> borderedToolBar(new BorderedControl(BorderedControlCreateParams(toolBarPtr.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(DefaultToolBarBorderColor()).FocusedSingleBorderColor(DefaultToolBarBorderColor()).SetSize(toolBar->GetSize()).SetDock(Dock::top)));

    std::unique_ptr<ToolButton> prevToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.prev.bitmap").SetPadding(Padding(8, 8, 8, 8)).SetToolTip("Go To Previous Location")));
    prevToolButton = prevToolButtonPtr.get();
    prevToolButton->Click().AddHandler(this, &MainWindow::GotoPreviousLocationClick);
    prevToolButton->Disable();
    toolBar->AddToolButton(prevToolButtonPtr.release());

    std::unique_ptr<ToolButton> nextToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.next.bitmap").SetPadding(Padding(8, 8, 8, 8)).SetToolTip("Go To Next Location")));
    nextToolButton = nextToolButtonPtr.get();
    nextToolButton->Click().AddHandler(this, &MainWindow::GotoNextLocationClick);
    nextToolButton->Disable();
    toolBar->AddToolButton(nextToolButtonPtr.release());

    toolBar->AddToolButton(new ToolButtonSeparator());

    std::unique_ptr<ToolButton> saveToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.save.bitmap").SetPadding(Padding(8, 8, 8, 8)).SetToolTip("Save (Ctrl+S)")));
    saveToolButton = saveToolButtonPtr.get();
    saveToolButton->Click().AddHandler(this, &MainWindow::SaveClick);
    saveToolButton->Disable();
    toolBar->AddToolButton(saveToolButtonPtr.release());

    std::unique_ptr<ToolButton> saveAllToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.save.all.bitmap").SetPadding(Padding(6, 6, 6, 6)).SetToolTip("Save All (Ctrl+Shift+S)")));
    saveAllToolButton = saveAllToolButtonPtr.get();
    saveAllToolButton->Click().AddHandler(this, &MainWindow::SaveAllClick);
    saveAllToolButton->Disable();
    toolBar->AddToolButton(saveAllToolButtonPtr.release());

    toolBar->AddToolButton(new ToolButtonSeparator());

    Size textButtonSize(36, 12);

    std::unique_ptr<ToolButton> cppToolButtonPtr(new TextToolButton(TextToolButtonCreateParams("C++").Style(ToolButtonStyle::manual).SetSize(textButtonSize).SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Compile using C++ Backend")));
    cppToolButton = cppToolButtonPtr.get();
    cppToolButton->Click().AddHandler(this, &MainWindow::CppButtonClick);
    cppToolButton->Disable();
    toolBar->AddToolButton(cppToolButtonPtr.release());

    std::unique_ptr<ToolButton> llvmToolButtonPtr(new TextToolButton(TextToolButtonCreateParams("LLVM").Style(ToolButtonStyle::manual).SetSize(textButtonSize).SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Compile using LLVM Backend")));
    llvmToolButton = llvmToolButtonPtr.get();
    llvmToolButton->Click().AddHandler(this, &MainWindow::LlvmButtonClick);
    llvmToolButton->Disable();
    toolBar->AddToolButton(llvmToolButtonPtr.release());

    toolBar->AddToolButton(new ToolButtonSeparator());

    std::unique_ptr<ToolButton> debugToolButtonPtr(new TextToolButton(TextToolButtonCreateParams("Debug").Style(ToolButtonStyle::manual).SetSize(textButtonSize).SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Compile using Debug Configuration")));
    debugToolButton = debugToolButtonPtr.get();
    debugToolButton->Click().AddHandler(this, &MainWindow::DebugButtonClick);
    debugToolButton->Disable();
    toolBar->AddToolButton(debugToolButtonPtr.release());

    std::unique_ptr<ToolButton> releaseToolButtonPtr(new TextToolButton(TextToolButtonCreateParams("Release").Style(ToolButtonStyle::manual).SetSize(textButtonSize).SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Compile using Release Configuration")));
    releaseToolButton = releaseToolButtonPtr.get();
    releaseToolButton->Click().AddHandler(this, &MainWindow::ReleaseButtonClick);
    releaseToolButton->Disable();
    toolBar->AddToolButton(releaseToolButtonPtr.release());

    toolBar->AddToolButton(new ToolButtonSeparator());

    std::unique_ptr<ToolButton> buildSolutionToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.build.solution.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Build Solution (F7)")));
    buildSolutionToolButton = buildSolutionToolButtonPtr.get();
    buildSolutionToolButton->Click().AddHandler(this, &MainWindow::BuildSolutionClick);
    buildSolutionToolButton->Disable();
    toolBar->AddToolButton(buildSolutionToolButtonPtr.release());

    std::unique_ptr<ToolButton> buildActiveProjectToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.build.project.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Build Active Project (Ctrl+B)")));
    buildActiveProjectToolButton = buildActiveProjectToolButtonPtr.get();
    buildActiveProjectToolButton->Click().AddHandler(this, &MainWindow::BuildActiveProjectClick);
    buildActiveProjectToolButton->Disable();
    toolBar->AddToolButton(buildActiveProjectToolButtonPtr.release());

    std::unique_ptr<ToolButton> stopBuildServerToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.cancel.build.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Stop Build Server")));
    stopBuildServerToolButton = stopBuildServerToolButtonPtr.get();
    stopBuildServerToolButton->Click().AddHandler(this, &MainWindow::StopBuildServerClick);
    stopBuildServerToolButton->Disable();
    toolBar->AddToolButton(stopBuildServerToolButtonPtr.release());

    toolBar->AddToolButton(new ToolButtonSeparator());

    std::unique_ptr<ToolButton> startDebuggingToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.debug.start.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Start Debugging (F5)")));
    startDebuggingToolButton = startDebuggingToolButtonPtr.get();
    startDebuggingToolButton->Click().AddHandler(this, &MainWindow::StartDebuggingClick);
    startDebuggingToolButton->Disable();
    toolBar->AddToolButton(startDebuggingToolButtonPtr.release());

    std::unique_ptr<ToolButton> stopDebuggingToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.debug.stop.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Stop Debugging (Shift+F5)")));
    stopDebuggingToolButton = stopDebuggingToolButtonPtr.get();
    stopDebuggingToolButton->Click().AddHandler(this, &MainWindow::StopDebuggingClick);
    stopDebuggingToolButton->Disable();
    toolBar->AddToolButton(stopDebuggingToolButtonPtr.release());

    toolBar->AddToolButton(new ToolButtonSeparator());

    std::unique_ptr<ToolButton> showNextStatementToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.debug.show.next.statement.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Show Next Statement (Ctrl+J)")));
    showNextStatementToolButton = showNextStatementToolButtonPtr.get();
    showNextStatementToolButton->Click().AddHandler(this, &MainWindow::ShowNextStatementClick);
    showNextStatementToolButton->Disable();
    toolBar->AddToolButton(showNextStatementToolButtonPtr.release());

    std::unique_ptr<ToolButton> stepOverToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.debug.step.over.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Step Over (F12)")));
    stepOverToolButton = stepOverToolButtonPtr.get();
    stepOverToolButton->Click().AddHandler(this, &MainWindow::StepOverClick);
    stepOverToolButton->Disable();
    toolBar->AddToolButton(stepOverToolButtonPtr.release());

    std::unique_ptr<ToolButton> stepIntoToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.debug.step.into.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Step Into (F11)")));
    stepIntoToolButton = stepIntoToolButtonPtr.get();
    stepIntoToolButton->Click().AddHandler(this, &MainWindow::StepIntoClick);
    stepIntoToolButton->Disable();
    toolBar->AddToolButton(stepIntoToolButtonPtr.release());

    std::unique_ptr<ToolButton> stepOutToolButtonPtr(new ToolButton(ToolButtonCreateParams().ToolBitMapName("cmcode.debug.step.out.bitmap").SetPadding(Padding(8, 8, 8, 8)).
        SetToolTip("Step Out (Shift+F11)")));
    stepOutToolButton = stepOutToolButtonPtr.get();
    stepOutToolButton->Click().AddHandler(this, &MainWindow::StepOutClick);
    stepOutToolButton->Disable();
    toolBar->AddToolButton(stepOutToolButtonPtr.release());

    AddChild(borderedToolBar.release());

    std::unique_ptr<SplitContainer> verticalSplitContainerPtr(
        new SplitContainer(SplitContainerCreateParams(SplitterOrientation::vertical).SplitterDistance(0).SetDock(Dock::fill)));
    verticalSplitContainer = verticalSplitContainerPtr.get();
    verticalSplitContainer->SplitterDistanceChanged().AddHandler(this, &MainWindow::VerticalSplitContainerSplitterDistanceChanged);
    std::unique_ptr<SplitContainer> horizontalSplitContainerPtr(
        new SplitContainer(SplitContainerCreateParams(SplitterOrientation::horizontal).SplitterDistance(0).SetDock(Dock::fill)));
    horizontalSplitContainer = horizontalSplitContainerPtr.get();
    horizontalSplitContainer->SplitterDistanceChanged().AddHandler(this, &MainWindow::HorizontalSplitContainerSplitterDistanceChanged);
    std::unique_ptr<TabControl> codeTabControlPtr(new TabControl(TabControlCreateParams().SetDock(Dock::fill)));
    codeTabControl = codeTabControlPtr.get();
    codeTabControl->TabPageSelected().AddHandler(this, &MainWindow::CodeTabPageSelected);
    codeTabControl->ControlRemoved().AddHandler(this, &MainWindow::CodeTabPageRemoved);
    std::unique_ptr<Control> paddedCodeTabControl(new PaddedControl(PaddedControlCreateParams(codeTabControlPtr.release()).Defaults()));
    std::unique_ptr<Control> borderedCodeTabControl(new BorderedControl(BorderedControlCreateParams(paddedCodeTabControl.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(DefaultTabControlFrameColor()).FocusedSingleBorderColor(DefaultTabControlFrameColor()).SetDock(Dock::fill)));
    std::unique_ptr<TreeView> solutionTreeViewPtr(new TreeView(TreeViewCreateParams().Defaults()));
    solutionTreeView = solutionTreeViewPtr.get();
    solutionTreeView->NodeDoubleClick().AddHandler(this, &MainWindow::TreeViewNodeDoubleClick);
    solutionTreeView->NodeClick().AddHandler(this, &MainWindow::TreeViewNodeClick);
    solutionTreeView->SetDoubleBuffered();
    std::unique_ptr<PaddedControl> paddedTreeViewPtr(new PaddedControl(PaddedControlCreateParams(solutionTreeViewPtr.release()).Defaults()));
    std::unique_ptr<ScrollableControl> scrollableTreeViewPtr(new ScrollableControl(ScrollableControlCreateParams(paddedTreeViewPtr.release()).SetDock(Dock::fill)));
    horizontalSplitContainer->Pane1Container()->AddChild(borderedCodeTabControl.release());
    horizontalSplitContainer->Pane2Container()->AddChild(scrollableTreeViewPtr.release());
    verticalSplitContainer->Pane1Container()->AddChild(horizontalSplitContainerPtr.release());
    std::unique_ptr<TabControl> outputTabControlPtr(new TabControl(TabControlCreateParams().Defaults()));
    outputTabControl = outputTabControlPtr.get();
    outputTabControl->ControlRemoved().AddHandler(this, &MainWindow::OutputTabControlTabPageRemoved);
    outputTabControl->TabPageSelected().AddHandler(this, &MainWindow::OutputTabControlTabPageSelected);
    std::unique_ptr<Control> paddedOutputTabControl(new PaddedControl(PaddedControlCreateParams(outputTabControlPtr.release()).Defaults()));
    std::unique_ptr<Control> borderedOutptTabControl(new BorderedControl(BorderedControlCreateParams(paddedOutputTabControl.release()).SetBorderStyle(BorderStyle::single).
        NormalSingleBorderColor(DefaultTabControlFrameColor()).FocusedSingleBorderColor(DefaultTabControlFrameColor()).SetDock(Dock::fill)));
    std::unique_ptr<TabPage> outputTabPagePtr(new TabPage("Output", "output"));
    outputTabPage = outputTabPagePtr.get();
    std::unique_ptr<LogView> outputLogViewPtr(new LogView(TextViewCreateParams().Defaults()));
    outputLogView = outputLogViewPtr.get();
    outputLogView->SetDoubleBuffered();
    std::unique_ptr<Control> scrollableOutputLogView(new ScrollableControl(ScrollableControlCreateParams(outputLogViewPtr.release()).SetDock(Dock::fill)));
    outputTabPage->AddChild(scrollableOutputLogView.release());
    outputTabControl->AddTabPage(outputTabPagePtr.release());
    verticalSplitContainer->Pane2Container()->AddChild(borderedOutptTabControl.release());
    AddChild(verticalSplitContainerPtr.release());

    std::unique_ptr<StatusBar> statusBarPtr(new StatusBar(StatusBarCreateParams().Defaults()));
    statusBar = statusBarPtr.get();
    std::unique_ptr<StatusBarTextItem> buildIndicatorStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(1).BorderStyle(StatusBarItemBorderStyle::sunken)));
    buildIndicatorStatuBarItem = buildIndicatorStatusBarItemPtr.get();
    statusBar->AddItem(buildIndicatorStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> editorReadWriteIndicatorStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(3).BorderStyle(StatusBarItemBorderStyle::sunken)));
    editorReadWriteIndicatorStatusBarItem = editorReadWriteIndicatorStatusBarItemPtr.get();
    statusBar->AddItem(editorReadWriteIndicatorStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> editorDirtyIndicatorStausBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(1).BorderStyle(StatusBarItemBorderStyle::sunken)));
    editorDirtyIndicatorStatusBarItem = editorDirtyIndicatorStausBarItemPtr.get();
    statusBar->AddItem(editorDirtyIndicatorStausBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> sourceFilePathStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(0).BorderStyle(StatusBarItemBorderStyle::sunken)));
    sourceFilePathStatusBarItem = sourceFilePathStatusBarItemPtr.get();
    statusBar->AddItem(sourceFilePathStatusBarItemPtr.release());
    statusBar->AddItem(new StatusBarSpringItem());
    std::unique_ptr<StatusBarTextItem> lineLabelStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(5).Text("Line:").BorderStyle(StatusBarItemBorderStyle::flat)));
    statusBar->AddItem(lineLabelStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> lineStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(5).BorderStyle(StatusBarItemBorderStyle::sunken)));
    lineStatusBarItem = lineStatusBarItemPtr.get();
    statusBar->AddItem(lineStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> columnLabelStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(4).Text("Col:").BorderStyle(StatusBarItemBorderStyle::flat)));
    statusBar->AddItem(columnLabelStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> columnStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(5).BorderStyle(StatusBarItemBorderStyle::sunken)));
    columnStatusBarItem = columnStatusBarItemPtr.get();
    statusBar->AddItem(columnStatusBarItemPtr.release());
    AddChild(statusBarPtr.release());

    locations.SetToolButtons(prevToolButton, nextToolButton);

    SetServiceMessageHandlerView(this);
    StartRequestDispatcher();

    if (!filePath.empty())
    {
        OpenProject(filePath);
    }

    SetState(MainWindowState::idle);
    LoadConfigurationSettings();

    AddClipboardListener();

    SetTimer(configurationSaveTimerId, configurationSavePeriod);
}

MainWindow::~MainWindow()
{
    SetServiceMessageHandlerView(nullptr);
    StopRequestDispatcher();
}

void MainWindow::OnWindowClosing(CancelArgs& args)
{
    try
    {
        Window::OnWindowClosing(args);
        if (!CloseSolution())
        {
            args.cancelClose = true;
        }
        else
        {
            SaveConfigurationSettings();
            this->RemoveClipboardListener();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::OnKeyDown(KeyEventArgs& args)
{
    try
    {
        Window::OnKeyDown(args);
        if (!args.handled)
        {
            switch (args.key)
            {
                case Keys::escape:
                {
                    RemoveContextMenu();
                    args.handled = true;
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::MouseUpNotification(MouseEventArgs& args)
{
    try
    {
        if (args.buttons == MouseButtons::lbutton)
        {
            if (!showingDialog)
            {
                RemoveContextMenu();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::OnTimer(TimerEventArgs& args)
{
    try
    {
        Window::OnTimer(args);
        if (args.timerId == buildProgressTimerId)
        {
            ShowBuildProgress();
        }
        else if (args.timerId == configurationSaveTimerId)
        {
            SaveConfigurationSettings();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::OnGotFocus()
{
    Window::OnGotFocus();
    SetFocusToEditor();
}

void MainWindow::OnClipboardUpdate()
{
    try
    {
        Window::OnClipboardUpdate();
        Clipboard clipboard(Handle());
        if (clipboard.IsClipboardFormatAvailable(cmajorCodeFormat))
        {
            clipboardData = clipboard.GetStringData(cmajorCodeFormat);
            pasteMenuItem->Enable();
        }
        else
        {
            ClipboardFormat unicodeText(unicodeTextClipboardFormat);
            if (clipboard.IsClipboardFormatAvailable(unicodeText))
            {
                clipboardData = clipboard.GetStringData(unicodeText);
                pasteMenuItem->Enable();
            }
            else
            {
                ClipboardFormat ansiText(textClipboardFormat);
                if (clipboard.IsClipboardFormatAvailable(ansiText))
                {
                    clipboardData = clipboard.GetStringData(ansiText);
                    pasteMenuItem->Enable();
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), "Could not open clipboard, another application window may have clipboard open: " + std::string(ex.what()));
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

void MainWindow::OnWindowStateChanged()
{
    Window::OnWindowStateChanged();
    const WindowSettings& windowSettings = GetWindowSettings();
    if (GetWindowState() == WindowState::normal)
    {
        SetLocation(Point(windowSettings.location.x, windowSettings.location.y));
        SetSize(Size(windowSettings.size.width, windowSettings.size.height));
        horizontalSplitContainer->SetSplitterDistance(windowSettings.normalHorizontalSplitterDistance);
        verticalSplitContainer->SetSplitterDistance(windowSettings.normalVerticalSplitterDistance);
    }
    else if (GetWindowState() == WindowState::maximized)
    {
        setMaximizedSplitterDistance = true;
    }
}

void MainWindow::OnSizeChanged()
{
    Window::OnSizeChanged();
    if (GetWindowState() == WindowState::maximized && setMaximizedSplitterDistance)
    {
        setMaximizedSplitterDistance = false;
        const WindowSettings& windowSettings = GetWindowSettings();
        horizontalSplitContainer->SetSplitterDistance(windowSettings.maximizedHorizontalSplitterDistance);
        verticalSplitContainer->SetSplitterDistance(windowSettings.maximizedVerticalSplitterDistance);
    }
    statusBar->SetChanged();
    statusBar->Invalidate();
    if (GetWindowState() == WindowState::normal)
    {
        sizeChanged = true;
        if (verticalSplitContainerFactor != 0)
        {
            int splitterDistance = static_cast<int>(verticalSplitContainerFactor * verticalSplitContainer->GetSize().Height);
            verticalSplitContainer->SetSplitterDistance(splitterDistance);
        }
        if (horizontalSplitContainerFactor != 0)
        {
            int splitterDistance = static_cast<int>(horizontalSplitContainerFactor * horizontalSplitContainer->GetSize().Width);
            horizontalSplitContainer->SetSplitterDistance(splitterDistance);
        }
        sizeChanged = false;
    }
}

void MainWindow::LoadConfigurationSettings()
{
    LoadConfiguration();
    const WindowSettings& windowSettings = GetWindowSettings();
    SetWindowState(WindowState(windowSettings.windowState));
    if (GetWindowState() == WindowState::normal)
    {
        SetLocation(Point(windowSettings.location.x, windowSettings.location.y));
        SetSize(Size(windowSettings.size.width, windowSettings.size.height));
        horizontalSplitContainer->SetSplitterDistance(windowSettings.normalHorizontalSplitterDistance);
        verticalSplitContainer->SetSplitterDistance(windowSettings.normalVerticalSplitterDistance);
    }
    else if (GetWindowState() == WindowState::maximized)
    {
        setMaximizedSplitterDistance = true;
    }
}

void MainWindow::SaveConfigurationSettings()
{
    WindowState windowState = GetWindowState();
    WindowSettings& windowSettings = GetWindowSettings();
    windowSettings.windowState = static_cast<int>(windowState);
    if (windowState == WindowState::normal)
    {
        Point location = Location();
        WindowLocation windowLocation;
        windowLocation.x = location.X;
        windowLocation.y = location.Y;
        windowSettings.location = windowLocation;
        Size size = GetSize();
        WindowSize windowSize;
        windowSize.width = size.Width;
        windowSize.height = size.Height;
        windowSettings.size = windowSize;
        windowSettings.normalHorizontalSplitterDistance = horizontalSplitContainer->SplitterDistance();
        windowSettings.normalVerticalSplitterDistance = verticalSplitContainer->SplitterDistance();
    }
    else if (windowState == WindowState::maximized)
    {
        windowSettings.maximizedHorizontalSplitterDistance = horizontalSplitContainer->SplitterDistance();
        windowSettings.maximizedVerticalSplitterDistance = verticalSplitContainer->SplitterDistance();
    }
    cmcode::SaveConfiguration();
}

void MainWindow::AddClipboardListener()
{
    try
    {
        clipboardListener.reset(new ClipboardListener(Handle()));
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::RemoveClipboardListener()
{
    clipboardListener.reset();
}

void MainWindow::StartBuilding()
{
    SaveAllClick();
    SetEditorsReadOnly();
    SetState(MainWindowState::building);
    ClearOutput();
    buildProgressCounter = 0;
    buildProgressTimerRunning = true;
    SetTimer(buildProgressTimerId, buildProgressTimerPeriod);
}

void MainWindow::StopBuilding()
{
    if (buildProgressTimerRunning)
    {
        buildProgressTimerRunning = false;
        KillTimer(buildProgressTimerId);
    }
    buildIndicatorStatuBarItem->SetText(std::string());
    SetState(MainWindowState::idle);
    SetEditorsReadWrite();
}

void MainWindow::ShowBuildProgress()
{
    if (state == MainWindowState::building)
    {
        std::string buildIndicatorText;
        switch (buildProgressCounter % 4)
        {
            case 0:
            {
                buildIndicatorText = "|";
                break;
            }
            case 1:
            {
                buildIndicatorText = "/";
                break;
            }
            case 2:
            {
                buildIndicatorText = "-";
                break;
            }
            case 3:
            {
                buildIndicatorText = "\\";
                break;
            }
        }
        buildIndicatorStatuBarItem->SetText(buildIndicatorText);
        ++buildProgressCounter;
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
            case ServiceMessageKind::buildReply:
            {
                BuildReplyServiceMessage* message = static_cast<BuildReplyServiceMessage*>(serviceMessage.get());
                HandleBuildReply(message->GetBuildReply());
                break;
            }
            case ServiceMessageKind::buildError:
            {
                BuildErrorServiceMessage* message = static_cast<BuildErrorServiceMessage*>(serviceMessage.get());
                HandleBuildError(message->Error());
                break;
            }
            case ServiceMessageKind::getDefinitionReply:
            {
                GetDefinitionReplyServiceMessage* message = static_cast<GetDefinitionReplyServiceMessage*>(serviceMessage.get());
                HandleGetDefinitionReply(message->GetGetDefinitionReply());
                break;
            }
            case ServiceMessageKind::getDefinitionError:
            {
                GetDefinitionErrorServiceMessage* message = static_cast<GetDefinitionErrorServiceMessage*>(serviceMessage.get());
                HandleGetDefinitionError(message->Error());
            }
            case ServiceMessageKind::stopBuild:
            {
                HandleStopBuild();
                break;
            }
        }
    }
}

void MainWindow::ClearOutput()
{
    GetOutputLogView()->Clear();
}

void MainWindow::WriteOutput(const std::string& text)
{
    GetOutputLogView()->WriteLine(text);
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
        codeTabControl->CloseAllTabPages();
        solutionData.reset(new SolutionData(std::move(solution), solutionTreeView));
        SetState(MainWindowState::idle);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::HandleBuildReply(BuildReply& buildReply)
{
    StopBuilding();
    if (buildReply.requestValid)
    {
        if (buildReply.success)
        {
            PutOutputServiceMessage("build successful, time=" + buildReply.time);
        }
        else
        {
            PutOutputServiceMessage("build unsuccessful");
            if (!buildReply.errors.empty())
            {
                ErrorView* errorView = GetErrorView();
                errorView->Clear();
                errorView->SetErrors(std::move(buildReply.errors));
            }
        }
    }
    else
    {
        PutOutputServiceMessage("invalid build request: " + buildReply.requestErrorMessage);
    }
}

void MainWindow::HandleBuildError(const std::string& buildError)
{
    StopBuilding();
    PutOutputServiceMessage("build unsuccessful");
}

void MainWindow::HandleStopBuild()
{
    StopBuilding();
}

void MainWindow::HandleGetDefinitionReply(GetDefinitionReply& getDefinitionReply)
{
    StopBuilding();
    if (getDefinitionReply.ok)
    {
        DefinitionSourceLocation currentLocation = CurrentLocation();
        locations.AddLocation(currentLocation);
        locations.AddLocation(getDefinitionReply.definitionLocation);
        locations.GotoPreviousLocation(currentLocation);
    }
    else
    {
        PutOutputServiceMessage("goto definition command unsuccessful: " + getDefinitionReply.error);
    }
}

void MainWindow::HandleGetDefinitionError(const std::string& getDefinitionError)
{
    StopBuilding();
    PutOutputServiceMessage("goto definition command unsuccessful");
}

void MainWindow::SetState(MainWindowState state_)
{
    state = state_;
    newProjectMenuItem->Disable();
    openProjectMenuItem->Disable();
    closeSolutionMenuItem->Disable();
    saveMenuItem->Disable();
    saveAllMenuItem->Disable();
    exitMenuItem->Disable();
    copyMenuItem->Disable();
    cutMenuItem->Disable();
    pasteMenuItem->Disable();
    undoMenuItem->Disable();
    redoMenuItem->Disable();
    gotoMenuItem->Disable();
    searchMenuItem->Disable();
    optionsMenuItem->Disable();
    callStackMenuItem->Disable();
    localsMenuItem->Disable();
    errorsMenuItem->Disable();
    searchResultsMenuItem->Disable();
    portMapMenuItem->Disable();
    buildSolutionMenuItem->Disable();
    rebuildSolutionMenuItem->Disable();
    cleanSolutionMenuItem->Disable();
    buildActiveProjectMenuItem->Disable();
    rebuildActiveProjectMenuItem->Disable();
    cleanActiveProjectMenuItem->Disable();
    startDebuggingMenuItem->Disable();
    startWithoutDebuggingMenuItem->Disable();
    terminateProcessMenuItem->Disable();
    stopDebuggingMenuItem->Disable();
    showNextStatementMenuItem->Disable();
    stepOverMenuItem->Disable();
    stepIntoMenuItem->Disable();
    stepOutMenuItem->Disable();
    toggleBreakpointMenuItem->Disable();
    programArgumentsMenuItem->Disable();
    closeAllTabsMenuItem->Disable();
    closeExternalTabsMenuItem->Disable();
    homepageMenuItem->Disable();
    localDocumentationMenuItem->Disable();
    aboutMenuItem->Disable();
    prevToolButton->Disable();
    nextToolButton->Disable();
    saveToolButton->Disable();
    saveAllToolButton->Disable();
    cppToolButton->Disable();
    llvmToolButton->Disable();
    debugToolButton->Disable();
    releaseToolButton->Disable();
    buildSolutionToolButton->Disable();
    buildActiveProjectToolButton->Disable();
    stopBuildServerToolButton->Disable();
    startDebuggingToolButton->Disable();
    stopDebuggingToolButton->Disable();
    showNextStatementToolButton->Disable();
    stepOverToolButton->Disable();
    stepIntoToolButton->Disable();
    stepOutToolButton->Disable();

    if (backend == "cpp")
    {
        cppToolButton->SetState(ToolButtonState::pressed);
        llvmToolButton->SetState(ToolButtonState::normal);
    }
    if (backend == "llvm")
    {
        llvmToolButton->SetState(ToolButtonState::pressed);
        cppToolButton->SetState(ToolButtonState::normal);
    }
    if (config == "debug")
    {
        debugToolButton->SetState(ToolButtonState::pressed);
        releaseToolButton->SetState(ToolButtonState::normal);
    }
    if (config == "release")
    {
        releaseToolButton->SetState(ToolButtonState::pressed);
        debugToolButton->SetState(ToolButtonState::normal);
    }

    bool solutionOpen = solutionData.get() != nullptr;

    // todo:
    // gotoMenuItem
    // callStackMenuItem
    // localsMenuItem
    // errorsMenuItem
    // terminateProcessMenuItem
    // stopDebuggingMenuItem
    // showNextStatementMenuItem
    // stepOverMenuItem
    // stepIntoMenuItem
    // stepOutMenuItem
    // toggleBreakpointMenuItem
    // prevToolButton
    // nextToolButton
    // saveToolButton
    // stopBuildServerToolButton
    // stopDebuggingToolButton
    // showNextStatementToolButton
    // stepOverToolButton
    // stepIntoToolButton
    // stepOutToolButton

    // always on:

    exitMenuItem->Enable();
    optionsMenuItem->Enable();
    searchResultsMenuItem->Enable();
    portMapMenuItem->Enable();
    closeAllTabsMenuItem->Enable();
    closeExternalTabsMenuItem->Enable();
    homepageMenuItem->Enable();
    localDocumentationMenuItem->Enable();
    aboutMenuItem->Enable();

    switch (state)
    {
        case MainWindowState::idle:
        {
            newProjectMenuItem->Enable();
            openProjectMenuItem->Enable();

            bool buildServiceRunning = BuildServiceRunning();

            if (solutionOpen)
            {
                closeSolutionMenuItem->Enable();
                saveAllMenuItem->Enable();
                searchMenuItem->Enable();
                errorsMenuItem->Enable();
                buildSolutionMenuItem->Enable();
                rebuildSolutionMenuItem->Enable();
                cleanSolutionMenuItem->Enable();
                buildActiveProjectMenuItem->Enable();
                rebuildActiveProjectMenuItem->Enable();
                cleanActiveProjectMenuItem->Enable();
                startDebuggingMenuItem->Enable();
                startWithoutDebuggingMenuItem->Enable();
                programArgumentsMenuItem->Enable();
                saveAllToolButton->Enable();
                cppToolButton->Enable();
                llvmToolButton->Enable();
                debugToolButton->Enable();
                releaseToolButton->Enable();
                buildSolutionToolButton->Enable();
                buildActiveProjectToolButton->Enable();
                startDebuggingToolButton->Enable();
                if (buildServiceRunning)
                {
                    stopBuildServerToolButton->Enable();
                }
            }
            break;
        }
        case MainWindowState::building:
        {
            stopBuildServerToolButton->Enable();
            break;
        }
    }
    SetEditorState();
}

void MainWindow::SetEditorState()
{
    bool editorOpen = !codeTabControl->TabPages().IsEmpty();
    if (editorOpen)
    {
        gotoMenuItem->Enable();
        Editor* currentEditor = CurrentEditor();
        if (currentEditor)
        {
            TextView* textView = currentEditor->GetTextView();
            if (textView)
            {
                std::string editorReadWriteText;
                if (textView->IsReadOnly())
                {
                    editorReadWriteText = "R/O";
                }
                else
                {
                    editorReadWriteText = "R/W";
                }
                editorReadWriteIndicatorStatusBarItem->SetText(editorReadWriteText);
                std::string editorDirtyText;
                if (textView->IsDirty())
                {
                    editorDirtyText = "*";
                }
                else
                {
                    editorDirtyText = std::string();
                }
                editorDirtyIndicatorStatusBarItem->SetText(editorDirtyText);
                sourceFilePathStatusBarItem->SetText(textView->FilePath());
                if (!textView->IsSelectionEmpty())
                {
                    copyMenuItem->Enable();
                    cutMenuItem->Enable();
                }
                else
                {
                    copyMenuItem->Disable();
                    cutMenuItem->Disable();
                }
                if (textView->CaretLine() != 0)
                {
                    lineStatusBarItem->SetText(std::to_string(textView->CaretLine()));
                }
                else
                {
                    lineStatusBarItem->SetText(std::string());
                }
                if (textView->CaretColumn() != 0)
                {
                    columnStatusBarItem->SetText(std::to_string(textView->CaretColumn()));
                }
                else
                {
                    columnStatusBarItem->SetText(std::string());
                }
            }
        }
    }
    else
    {
        editorReadWriteIndicatorStatusBarItem->SetText(std::string());
        editorDirtyIndicatorStatusBarItem->SetText(std::string());
        sourceFilePathStatusBarItem->SetText(std::string());
        lineStatusBarItem->SetText(std::string());
        columnStatusBarItem->SetText(std::string());
    }
}

void MainWindow::SetEditorsReadOnly()
{
    Component* child = codeTabControl->TabPages().FirstChild();
    while (child)
    {
        if (child->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(child);
            Editor* editor = GetEditorByTabPage(tabPage);
            if (editor)
            {
                TextView* textView = editor->GetTextView();
                if (textView)
                {
                    textView->SetReadOnly();
                }
            }
        }
        child = child->NextSibling();
    }
}

void MainWindow::SetEditorsReadWrite()
{
    Component* child = codeTabControl->TabPages().FirstChild();
    while (child)
    {
        if (child->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(child);
            Editor* editor = GetEditorByTabPage(tabPage);
            if (editor)
            {
                TextView* textView = editor->GetTextView();
                if (textView)
                {
                    textView->SetReadWrite();
                }
            }
        }
        child = child->NextSibling();
    }
}

void MainWindow::SetFocusToEditor()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        TextView* textView = editor->GetTextView();
        if (textView)
        {
            textView->SetFocus();
        }
    }
}

void MainWindow::EditorReadOnlyChanged()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        TextView* textView = editor->GetTextView();
        if (textView)
        {
            std::string editorReadWriteText;
            if (textView->IsReadOnly())
            {
                editorReadWriteText = "R/O";
            }
            else
            {
                editorReadWriteText = "R/W";
            }
            editorReadWriteIndicatorStatusBarItem->SetText(editorReadWriteText);
        }
    }
}

void MainWindow::EditorDirtyChanged()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        TextView* textView = editor->GetTextView();
        if (textView)
        {
            std::string editorDirtyText;
            if (textView->IsDirty())
            {
                saveToolButton->Enable();
                saveMenuItem->Enable();
                editorDirtyText = "*";
            }
            else
            {
                saveToolButton->Disable();
                saveMenuItem->Disable();
                editorDirtyText = std::string();
            }
            editorDirtyIndicatorStatusBarItem->SetText(editorDirtyText);
        }
    }
}

void MainWindow::EditorCCDirtyChanged()
{
    // todo
}

void MainWindow::EditorCaretPosChanged()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        TextView* textView = editor->GetTextView();
        if (textView)
        {
            if (textView->CaretLine() != 0)
            {
                lineStatusBarItem->SetText(std::to_string(textView->CaretLine()));
            }
            else
            {
                lineStatusBarItem->SetText(std::string());
            }
            if (textView->CaretColumn() != 0)
            {
                columnStatusBarItem->SetText(std::to_string(textView->CaretColumn()));
            }
            else
            {
                columnStatusBarItem->SetText(std::string());
            }
        }
    }
}

void MainWindow::EditorSelectionChanged()
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            if (textView)
            {
                if (!textView->IsSelectionEmpty())
                {
                    copyMenuItem->Enable();
                    cutMenuItem->Enable();
                }
                else
                {
                    copyMenuItem->Disable();
                    cutMenuItem->Disable();
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::EditorRightClick(RightClickEventArgs& args)
{
    try
    {
        Control* control = args.control;
        if (control->IsTextView())
        {
            TextView* textView = static_cast<TextView*>(control);
            std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
            std::string identifier;
            DefinitionSourceLocation sourceLocation;
            if (GetDefinitionSourceLocationAt(args.location, textView, identifier, sourceLocation))
            {
                std::unique_ptr<MenuItem> gotoDefinitionMenuItem(new MenuItem("Go To Definition"));
                sngcm::ast::Project* project = CurrentProject();
                if (!project)
                {
                    throw std::runtime_error("current project not deduced");
                }
                clickActions.clear();
                clickActions.push_back(std::unique_ptr<ClickAction>(new GotoDefinitionAction(gotoDefinitionMenuItem.get(), this, project, identifier, sourceLocation)));
                contextMenu->AddMenuItem(gotoDefinitionMenuItem.release());
            }
            if (state == MainWindowState::debugging)
            {
                // todo: run to cursor
            }
            if (contextMenu->HasMenuItems())
            {
                Point loc = args.location;
                textView->TranslateContentLocationInternal(loc);
                Point screenLoc = textView->ClientToScreen(loc);
                ShowContextMenu(contextMenu.release(), screenLoc);
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

bool MainWindow::GetDefinitionSourceLocationAt(const Point& loc, TextView* textView, std::string& identifier, DefinitionSourceLocation& sourceLocation)
{
    int line = 0;
    int column = 0;
    textView->GetLineColumn(loc, line, column);
    if (line >= 1 && line <= textView->Lines().size())
    {
        if (column >= 1 && column <= textView->GetLineLength(line))
        {
            char32_t c = textView->GetCharAt(line, column);
            if (IsIdStart(c) || IsIdCont(c))
            {
                while (column > 1 && (IsIdStart(c) || IsIdCont(c)))
                {
                    --column;
                    c = textView->GetCharAt(line, column);
                }
                if (IsWhiteSpace(c) || IsWordSeparator(c))
                {
                    ++column;
                }
                c = textView->GetCharAt(line, column);
                std::u32string id;
                int scol = column;
                while (column <= textView->GetLineLength(line) && (IsIdStart(c) || IsIdCont(c)))
                {
                    id.append(1, c);
                    ++column;
                    if (column <= textView->GetLineLength(line))
                    {
                        c = textView->GetCharAt(line, column);
                    }
                }
                int ecol = column;
                identifier = ToUtf8(id);
                sourceLocation.file = textView->FilePath();
                sourceLocation.line = line;
                sourceLocation.scol = scol;
                sourceLocation.ecol = ecol;
                return true;
            }
        }
    }
    return false;
}

sngcm::ast::Project* MainWindow::CurrentProject()
{
    if (!solutionData) return nullptr;
    sngcm::ast::Project* project = nullptr;
    Editor* editor = CurrentEditor();
    if (editor)
    {
        SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(editor->FilePath());
        if (data)
        {
            project = data->project;
        }
    }
    if (!project)
    {
        project = solutionData->GetSolution()->ActiveProject();
    }
    return project;
}

void MainWindow::GotoDefinition(sngcm::ast::Project* project, const std::string& identifier, const DefinitionSourceLocation& sourceLocation)
{
    try
    {
        GetDefinitionRequest request;
        request.backend = backend;
        request.config = config;
        request.projectName = ToUtf8(project->Name());
        request.projectFilePath = project->FilePath();
        request.identifier = identifier;
        request.identifierLocation = sourceLocation;
        StartBuilding();
        StartGetDefinitionRequest(request);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::GotoLocation(const DefinitionSourceLocation& location)
{
    TabPage* tabPage = codeTabControl->GetTabPageByKey(location.file);
    CmajorEditor* cmajorEditor = nullptr;
    if (tabPage)
    {
        Editor* editor = GetEditorByTabPage(tabPage);
        if (editor->IsCmajorEditor())
        {
            cmajorEditor = static_cast<CmajorEditor*>(editor);
        }
        tabPage->Select();
    }
    if (!cmajorEditor)
    {
        SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(location.file);
        if (data)
        {
            cmajorEditor = AddCmajorEditor(data->fileName, data->filePath, data->filePath, data->project);
        }
        else
        {
            cmajorEditor = AddCmajorEditor(Path::GetFileName(location.file), location.file, location.file, nullptr);
        }
    }
    if (cmajorEditor)
    {
        TextView* textView = cmajorEditor->GetTextView();
        if (textView)
        {
            int line = location.line;
            int scol = location.scol;
            int ecol = location.ecol;
            Selection selection;
            selection.start.line = line;
            selection.start.column = scol;
            selection.end.line = line;
            selection.end.column = ecol;
            textView->EnsureLineVisible(line);
            textView->SetSelection(selection);
            textView->SetCaretLineCol(line, scol);
        }
    }
}

DefinitionSourceLocation MainWindow::CurrentLocation() const
{
    DefinitionSourceLocation currentLocation;
    if (!solutionData) return currentLocation;
    Editor* editor = CurrentEditor();
    if (editor)
    {
        TextView* textView = editor->GetTextView();
        if (textView)
        {
            currentLocation.file = textView->FilePath();
            int line = textView->CaretLine();
            int column = textView->CaretColumn();
            currentLocation.line = line;
            currentLocation.scol = column;
            currentLocation.ecol = column;
        }
    }
    return currentLocation;
}

void MainWindow::BreakpointAdded(AddBreakpointEventArgs& args)
{
    ShowInfoMessageBox(Handle(), "breakpoint added");
}

void MainWindow::BreakpointRemoved(RemoveBreakpointEventArgs& args)
{
    ShowInfoMessageBox(Handle(), "breakpoint removed");
}

void MainWindow::VerticalSplitContainerSplitterDistanceChanged()
{
    if (GetWindowState() == WindowState::normal && !sizeChanged)
    {
        verticalSplitContainerFactor = (verticalSplitContainer->GetSize().Height - verticalSplitContainer->Pane2Container()->GetSize().Height) / (1.0f * verticalSplitContainer->GetSize().Height);
    }
}

void MainWindow::HorizontalSplitContainerSplitterDistanceChanged()
{
    if (GetWindowState() == WindowState::normal && !sizeChanged)
    {
        horizontalSplitContainerFactor = (horizontalSplitContainer->GetSize().Width - horizontalSplitContainer->Pane2Container()->GetSize().Width) / (1.0f * horizontalSplitContainer->GetSize().Width);
    }
}

void MainWindow::NewProjectClick()
{
    try
    {
        NewProjectDialog dialog;
        dialog.SetProjectLocation(CmajorProjectsDir());
        ShowDialogGuard guard(showingDialog);
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            bool sameDir = dialog.PlaceSolutionInSameDirectory();
            std::string solutionFilePath;
            std::string projectFilePath;
            if (!sameDir)
            {
                solutionFilePath = Path::Combine(dialog.GetProjectLocation(), Path::Combine(dialog.GetSolutionName(), dialog.GetSolutionName() + ".cms"));
                projectFilePath = Path::Combine(dialog.GetProjectLocation(), Path::Combine(dialog.GetSolutionName(), Path::Combine(dialog.GetProjectName(), dialog.GetProjectName() + ".cmp")));
                std::string solutionDir = Path::GetDirectoryName(solutionFilePath);
                boost::filesystem::create_directories(solutionDir);
                std::string projectDir = Path::GetDirectoryName(projectFilePath);
                boost::filesystem::create_directories(projectDir);
            }
            else
            {
                projectFilePath = Path::Combine(dialog.GetProjectLocation(), Path::Combine(dialog.GetProjectName(), dialog.GetProjectName() + ".cmp"));
                solutionFilePath = Path::ChangeExtension(projectFilePath, ".cms");
                std::string projectDir = Path::GetDirectoryName(projectFilePath);
                boost::filesystem::create_directories(projectDir);
            }
            if (boost::filesystem::exists(solutionFilePath))
            {
                MessageBoxResult result = MessageBox::Show("Solution file '" + solutionFilePath + "' already exists. Do you want to overwrite it?", "Question", this, MB_YESNO);
                if (result == MessageBoxResult::no)
                {
                    return;
                }
            }
            if (boost::filesystem::exists(projectFilePath))
            {
                MessageBoxResult result = MessageBox::Show("Project file '" + projectFilePath + "' already exists. Do you want to overwrite it?", "Question", this, MB_YESNO);
                if (result == MessageBoxResult::no)
                {
                    return;
                }
            }
            std::unique_ptr<sngcm::ast::Solution> solution(new sngcm::ast::Solution(ToUtf32(dialog.GetSolutionName()), solutionFilePath));
            std::unique_ptr<sngcm::ast::Project> project(new sngcm::ast::Project(ToUtf32(dialog.GetProjectName()), projectFilePath, "debug", sngcm::ast::BackEnd::llvm, "gcc", sngcm::ast::SystemDirKind::regular));
            project->SetTarget(dialog.GetProjectType());
            solution->SetActiveProject(project.get());
            solution->AddProject(std::move(project));
            solution->Save();
            if (!CloseSolution())
            { 
                return;
            }
            OpenProject(solutionFilePath);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::OpenProjectClick()
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Cmajor Solution Files (*.cms)", "*.cms"));
        descriptionFilterPairs.push_back(std::make_pair("Cmajor Project Files (*.cmp)", "*.cmp"));
        std::string initialDirectory = CmajorProjectsDir();
        std::string filePath;
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "cms", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            if (CloseSolution())
            {
                OpenProject(GetFullPath(filePath));
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

bool MainWindow::CloseSolution()
{
    try
    {
        if (!solutionData) return true;
        Component* child = codeTabControl->TabPages().FirstChild();
        while (child)
        {
            if (child->IsTabPage())
            {
                TabPage* tabPage = static_cast<TabPage*>(child);
                Editor* editor = GetEditorByTabPage(tabPage);
                if (editor)
                {
                    if (editor->IsDirty())
                    {
                        MessageBoxResult result = MessageBox::Show(editor->FilePath() + " is modified. Save changes?", "Question", this, MB_YESNOCANCEL);
                        if (result == MessageBoxResult::cancel) return false;
                        else if (result == MessageBoxResult::yes)
                        {
                            editor->Save();
                        }
                    }
                }
            }
            child = child->NextSibling();
        }
        codeTabControl->CloseAllTabPages();
        solutionData.reset();
        ResetFocusedControl();
        SetEditorState();
        return true;
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
    return false;
}

void MainWindow::CloseSolutionClick()
{
    if (CloseSolution())
    {
        solutionTreeView->SetRoot(nullptr);
    }
}

void MainWindow::SaveClick()
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            editor->Save();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::SaveAllClick()
{
    try
    {
        if (!solutionData) return;
        sngcm::ast::Solution* solution = solutionData->GetSolution();
        solution->Save();
        sngcm::ast::Project* activeProject = solution->ActiveProject();
        if (activeProject)
        {
            // todo: save project settings
        }
        Component* child = codeTabControl->TabPages().FirstChild();
        while (child)
        {
            if (child->IsTabPage())
            {
                TabPage* tabPage = static_cast<TabPage*>(child);
                Editor* editor = GetEditorByTabPage(tabPage);
                if (editor)
                {
                    if (editor->IsDirty())
                    {
                        editor->Save();
                    }
                }
            }
            child = child->NextSibling();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ExitClick()
{
    Close();
}

void MainWindow::CopyClick()
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            if (textView)
            {
                if (!textView->IsSelectionEmpty())
                {
                    SelectionData selection = textView->GetSelection();
                    if (!selection.selectedText.empty())
                    {
                        try
                        {
                            Clipboard clipboard(Handle());
                            clipboard.SetEmpty();
                            clipboard.SetStringData(cmajorCodeFormat, selection.selectedText);
                            std::u32string crlfText = selection.selectedText;
                            std::u32string::size_type lfPos = crlfText.find('\n');
                            while (lfPos != std::u32string::npos)
                            {
                                crlfText.replace(lfPos, 1, U"\r\n");
                                lfPos = crlfText.find('\n', lfPos + 2);
                            }
                            ClipboardFormat unicodeText(unicodeTextClipboardFormat);
                            clipboard.SetStringData(unicodeText, crlfText);
                        }
                        catch (const std::exception& ex)
                        {
                            ShowErrorMessageBox(Handle(), ex.what());
                            clipboardData = selection.selectedText;
                            pasteMenuItem->Enable();
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::CutClick()
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            if (textView)
            {
                if (!textView->IsSelectionEmpty())
                {
                    SelectionData selection = textView->GetSelection();
                    if (!selection.selectedText.empty())
                    {
                        try
                        {
                            Clipboard clipboard(Handle());
                            clipboard.SetEmpty();
                            clipboard.SetStringData(cmajorCodeFormat, selection.selectedText);
                            std::u32string crlfText = selection.selectedText;
                            std::u32string::size_type lfPos = crlfText.find('\n');
                            while (lfPos != std::u32string::npos)
                            {
                                crlfText.replace(lfPos, 1, U"\r\n");
                                lfPos = crlfText.find('\n', lfPos + 2);
                            }
                            ClipboardFormat unicodeText(unicodeTextClipboardFormat);
                            clipboard.SetStringData(unicodeText, crlfText);
                            textView->AddRemoveSelectionCommand();
                            textView->RemoveSelection();
                        }
                        catch (const std::exception& ex)
                        {
                            ShowErrorMessageBox(Handle(), ex.what());
                            textView->AddRemoveSelectionCommand();
                            textView->RemoveSelection();
                            clipboardData = selection.selectedText;
                            pasteMenuItem->Enable();
                        }
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::PasteClick()
{
    try
    {
        if (!clipboardData.empty())
        {
            Editor* editor = CurrentEditor();
            if (editor)
            {
                TextView* textView = editor->GetTextView();
                if (textView)
                {
                    int lineIndex = textView->CaretLine() - 1;
                    int columnIndex = textView->CaretColumn() - 1;
                    textView->SetFocus();
                    textView->InsertText(lineIndex, columnIndex, clipboardData);
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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
    try
    {
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        sngcm::ast::Solution* solution = solutionData->GetSolution();
        if (searchResultsTabPage)
        {
            searchResultsTabPage->Close();
            searchResultsTabPage = nullptr;
        }
        bool hasCurrentFile = false;
        Editor* editor = CurrentEditor();
        if (editor)
        {
            hasCurrentFile = true;
        }
        ShowDialogGuard dialogGuard(showingDialog);
        SearchDialog dialog(hasCurrentFile);
        DialogResult result = dialog.ShowDialog(*this);
        if (result == DialogResult::ok)
        {
            std::vector<std::string> files;
            switch (dialog.GetSearchExtent())
            {
                case SearchExtent::currentFile:
                {
                    files.push_back(editor->FilePath());
                    std::vector<std::u32string> lines;
                    for (const auto& line : editor->GetTextView()->Lines())
                    {
                        lines.push_back(*line);
                    }
                    Search(SearchExtent::currentFile, files, lines, dialog.SearchText(), dialog.WholeWords(), dialog.CaseInsensitive(), dialog.RegularExpression());
                    break;
                }
                case SearchExtent::activeProject:
                {
                    sngcm::ast::Project* activeProject = solution->ActiveProject();
                    files = activeProject->SourceFilePaths();
                    Search(SearchExtent::activeProject, files, std::vector<std::u32string>(), dialog.SearchText(), dialog.WholeWords(), dialog.CaseInsensitive(), dialog.RegularExpression());
                    break;
                }
                case SearchExtent::entireSolution:
                {
                    for (const auto& project : solution->Projects())
                    {
                        for (const auto& sourceFilePath : project->SourceFilePaths())
                        {
                            files.push_back(sourceFilePath);
                        }
                    }
                    Search(SearchExtent::entireSolution, files, std::vector<std::u32string>(), dialog.SearchText(), dialog.WholeWords(), dialog.CaseInsensitive(), dialog.RegularExpression());
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::Search(SearchExtent extent, const std::vector<std::string>& files, const std::vector<std::u32string>& lines, const std::string& searchText, 
    bool wholeWords, bool caseInsensitive, bool regularExpression)
{
    SearchResultsView* searchResults = GetSearchResultsView();
    searchResults->Clear();
    std::u32string text = ToUtf32(searchText);
    if (caseInsensitive)
    {
        text = ToLower(text);
    }
    Nfa* re = nullptr;
    Context context;
    Nfa nfa;
    if (regularExpression)
    {
        nfa = CompileRegularExpressionPattern(context, U".*(" + text + U").*");
        re = &nfa;
    }
    for (const std::string& fileName : files)
    {
        std::vector<std::u32string> readLines;
        const std::vector<std::u32string>* lns = &readLines;
        if (extent == SearchExtent::currentFile)
        {
            lns = &lines;
        }
        else
        {
            std::string content = ReadFile(fileName);
            readLines = SplitTextIntoLines(ToUtf32(content));
        }
        int n = lns->size();
        for (int i = 0; i < n; ++i)
        {
            const std::u32string* line = &(*lns)[i];
            std::u32string lowerLine;
            if (caseInsensitive)
            {
                lowerLine = ToLower(*line);
                line = &lowerLine;
            }
            if (Search(*line, text, wholeWords, re))
            {
                searchResults->Add(new SearchResult(fileName, i + 1, ToUtf8((*lns)[i])));
            }
        }
    }
    searchResults->Invalidate();
}

bool MainWindow::Search(const std::u32string& line, const std::u32string& text, bool wholeWords, Nfa* re)
{
    if (re)
    {
        return PatternMatch(line, *re);
    }
    else
    {
        std::u32string::size_type pos = line.find(text, 0);
        while (pos != std::u32string::npos)
        {
            bool match = true;
            if (wholeWords)
            {
                if (pos > 0)
                {
                    char32_t prev = line[pos - 1];
                    if (IsAlphabetic(prev) || IsNumber(prev))
                    {
                        match = false;
                    }
                }
                if (pos + text.length() < line.length())
                {
                    char32_t next = line[pos + text.length()];
                    if (IsAlphabetic(next) || IsNumber(next))
                    {
                        match = false;
                    }
                }
            }
            if (match)
            {
                return true;
            }
            pos = line.find(text, pos + 1);
        }
    }
    return false;
}

void MainWindow::ViewSearchResult(ViewSearchResultEventArgs& args)
{
    try
    {
        SearchResult* searchResult = args.searchResult;
        if (searchResult)
        {
            TabPage* tabPage = codeTabControl->GetTabPageByKey(searchResult->fileName);
            Editor* editor = nullptr;
            if (tabPage)
            {
                editor = GetEditorByTabPage(tabPage);
                tabPage->Select();
            }
            if (!editor)
            {
                SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(searchResult->fileName);
                if (data)
                {
                    editor = AddCmajorEditor(data->fileName, data->key, data->filePath, data->project);
                }
                else
                {
                    editor = AddCmajorEditor(Path::GetFileName(searchResult->fileName), searchResult->fileName, searchResult->fileName, nullptr);
                }
            }
            if (editor)
            {
                TextView* textView = editor->GetTextView();
                if (textView)
                {
                    textView->EnsureLineVisible(searchResult->lineNumber);
                    textView->SetCaretLineCol(std::min(searchResult->lineNumber, static_cast<int>(textView->Lines().size())), 1 + textView->LineNumberFieldLength());
                    textView->ScrollToCaret();
                    textView->SetFocus();
                    textView->Invalidate();
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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
    try
    {
        searchResultsView = GetSearchResultsView();
        if (searchResultsView)
        {
            searchResultsView->Invalidate();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }

}

void MainWindow::PortMapClick()
{
    ShowInfoMessageBox(Handle(), "Port Map");
}

void MainWindow::BuildSolutionClick()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        StartBuilding();
        StartBuild(backend, config, solutionData->GetSolution()->FilePath(), BuildRequestKind::build | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::RebuildSolutionClick()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        StartBuilding();
        StartBuild(backend, config, solutionData->GetSolution()->FilePath(), BuildRequestKind::rebuild | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::CleanSolutionClick()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        StartBuilding();
        StartBuild(backend, config, solutionData->GetSolution()->FilePath(), BuildRequestKind::clean);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::BuildProject(sngcm::ast::Project* project)
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        StartBuilding();
        StartBuild(backend, config, project->FilePath(), BuildRequestKind::build | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::RebuildProject(sngcm::ast::Project* project)
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        StartBuilding();
        StartBuild(backend, config, project->FilePath(), BuildRequestKind::rebuild | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::CleanProject(sngcm::ast::Project* project)
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        StartBuilding();
        StartBuild(backend, config, project->FilePath(), BuildRequestKind::clean | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddNewProject()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        AddNewProjectDialog dialog; 
        sngcm::ast::Solution* solution = solutionData->GetSolution();
        std::string solutionDir = Path::GetDirectoryName(solution->FilePath());
        dialog.SetProjectLocation(solutionDir);
        ShowDialogGuard guard(showingDialog);
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            std::string projectFilePath = Path::Combine(dialog.GetProjectLocation(), Path::Combine(dialog.GetProjectName(), dialog.GetProjectName() + ".cmp"));
            std::string projectDir = Path::GetDirectoryName(projectFilePath);
            boost::filesystem::create_directories(projectDir);
            if (boost::filesystem::exists(projectFilePath))
            {
                MessageBoxResult result = MessageBox::Show("Project file '" + projectFilePath + "' already exists. Do you want to overwrite it?", "Question", this, MB_YESNO);
                if (result == MessageBoxResult::no)
                {
                    return;
                }
            }
            std::unique_ptr<sngcm::ast::Project> project(new sngcm::ast::Project(ToUtf32(dialog.GetProjectName()), projectFilePath, "debug", sngcm::ast::BackEnd::llvm, "gcc", 
                sngcm::ast::SystemDirKind::regular));
            project->SetTarget(dialog.GetProjectType());
            solution->AddProject(std::move(project));
            solution->SortByProjectName();
            solution->Save();
            std::string solutionFilePath = solution->FilePath();
            if (!CloseSolution())
            {
                return;
            }
            OpenProject(solutionFilePath);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddExistingProject()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::pair<std::string, std::string>("Cmajor Project Files (*.cmp)", "*.cmp"));
        std::string initialDirectory = CmajorProjectsDir();
        std::string filePath;
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        std::string defaultFilePath;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "cmp", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            std::vector<std::unique_ptr<std::u32string>> contents;
            std::unique_ptr<sngcm::ast::Project> project = ReadProject(GetFullPath(filePath), contents);
            sngcm::ast::Solution* solution = solutionData->GetSolution();
            if (solution->HasProject(project->Name()))
            {
                throw std::runtime_error("solution already has a project with name '" + ToUtf8(project->Name()) + "'");
            }
            solution->AddProject(std::move(project));
            solution->SortByProjectName();
            solution->Save();
            std::string solutionFilePath = solution->FilePath();
            if (!CloseSolution())
            {
                return;
            }
            OpenProject(solutionFilePath);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::SetActiveProject(sngcm::ast::Project* project, TreeViewNode* newActiveProjectNode)
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        solutionData->ActiveProjectNode()->ResetActive();
        newActiveProjectNode->SetActive();
        solutionData->SetActiveProjectNode(newActiveProjectNode);
        solutionData->GetSolution()->SetActiveProject(project);
        // todo: load project settings
        solutionTreeView->Invalidate();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::RemoveProject(sngcm::ast::Project* project)
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        sngcm::ast::Solution* solution = solutionData->GetSolution();
        solution->RemoveProject(project);
        solution->Save();
        std::string solutionFilePath = solution->FilePath();
        if (!CloseSolution())
        {
            return;
        }
        OpenProject(solutionFilePath);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::OpenFileLocation(const std::string& filePath)
{
    try
    {
        std::string directoryLocation = Path::GetDirectoryName(filePath);
#pragma warning(disable:4311)
#pragma warning(disable:4302)
        if (reinterpret_cast<int>(ShellExecuteA(Handle(), "open", directoryLocation.c_str(), nullptr, nullptr, SW_SHOWNORMAL)) < 32)
        {
            throw std::runtime_error("shell execute failed");
        }
#pragma warning(default:4311)
#pragma warning(default:4302)
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddNewSourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode)
{
    try
    {
        AddNewSourceFileDialog dialog;
        ShowDialogGuard guard(showingDialog);
        DialogResult result = dialog.ShowDialog(*this);
        if (result == DialogResult::ok)
        {
            std::string sourceFileName = dialog.SourceFileName();
            if (!EndsWith(sourceFileName, ".cm"))
            {
                sourceFileName.append(".cm");
            }
            std::string newSourceFilePath = GetFullPath(Path::Combine(project->SourceBasePath().generic_string(), sourceFileName));
            if (project->HasSourceFile(newSourceFilePath))
            {
                throw std::runtime_error("source file '" + newSourceFilePath + "' already exists in project '" + ToUtf8(project->Name()) + "'");
            }
            project->AddSourceFileName(sourceFileName, newSourceFilePath);
            project->Save();
            // todo load edit module
            AddFilePathsToProject(newSourceFilePath, "", "", project, projectNode);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddExistingSourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode)
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Cmajor Source Files (*.cm)", "*.cm"));
        std::string initialDirectory = Path::GetDirectoryName(project->FilePath());
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        std::string filePath;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "cm", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            filePath = GetFullPath(filePath);
            if (project->HasSourceFile(filePath))
            {
                throw std::runtime_error("project '" + ToUtf8(project->Name()) + "' already has source file '" + filePath + "'");
            }
            std::string fileName = Path::GetFileName(filePath);
            project->AddSourceFileName(fileName, filePath);
            project->Save();
            // todo: load edit module
            AddFilePathsToProject("", "", "", project, projectNode);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddNewResourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode)
{
    try
    {
        AddNewResourceFileDialog dialog;
        ShowDialogGuard guard(showingDialog);
        DialogResult result = dialog.ShowDialog(*this);
        if (result == DialogResult::ok)
        {
            std::string resourceFileName = dialog.ResourceFileName();
            if (!EndsWith(resourceFileName, ".xml"))
            {
                resourceFileName.append(".xml");
            }
            std::string newResourceFilePath = GetFullPath(Path::Combine(project->SourceBasePath().generic_string(), resourceFileName));
            if (project->HasResourceFile(newResourceFilePath))
            {
                throw std::runtime_error("resource file '" + newResourceFilePath + "' already exists in project '" + ToUtf8(project->Name()) + "'");
            }
            project->AddResourceFileName(resourceFileName, newResourceFilePath);
            project->Save();
            AddFilePathsToProject("", newResourceFilePath, "", project, projectNode);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddExistingResourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode)
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Cmajor Resource Files (*.xml)", "*.xml"));
        std::string initialDirectory = Path::GetDirectoryName(project->FilePath());
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        std::string filePath;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "xml", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            filePath = GetFullPath(filePath);
            if (project->HasResourceFile(filePath))
            {
                throw std::runtime_error("project '" + ToUtf8(project->Name()) + "' already has resource file '" + filePath + "'");
            }
            std::string fileName = Path::GetFileName(filePath);
            project->AddResourceFileName(fileName, filePath);
            project->Save();
            AddFilePathsToProject("", "", "", project, projectNode);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddNewTextFile(sngcm::ast::Project* project, TreeViewNode* projectNode)
{
    try
    {
        AddNewTextFileDialog dialog;
        ShowDialogGuard guard(showingDialog);
        DialogResult result = dialog.ShowDialog(*this);
        if (result == DialogResult::ok)
        {
            std::string fileName = dialog.FileName();
            std::string filePath = GetFullPath(Path::Combine(project->SourceBasePath().generic_string(), fileName));
            if (project->HasTextFile(filePath))
            {
                throw std::runtime_error("text file '" + filePath + "' already exists in project '" + ToUtf8(project->Name()) + "'");
            }
            project->AddTextFileName(fileName, filePath);
            project->Save();
            AddFilePathsToProject("", "", filePath, project, projectNode);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::AddExistingTextFile(sngcm::ast::Project* project, TreeViewNode* projectNode)
{
    try
    {
        std::vector<std::pair<std::string, std::string>> descriptionFilterPairs;
        descriptionFilterPairs.push_back(std::make_pair("Text Files (*.*)", "*.*"));
        std::string initialDirectory = Path::GetDirectoryName(project->FilePath());
        std::string currentDirectory;
        std::vector<std::string> fileNames;
        std::string filePath;
        bool selected = OpenFileName(Handle(), descriptionFilterPairs, initialDirectory, std::string(), "txt", OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, filePath, currentDirectory, fileNames);
        if (selected)
        {
            filePath = GetFullPath(filePath);
            if (project->HasTextFile(filePath))
            {
                throw std::runtime_error("project '" + ToUtf8(project->Name()) + "' already has text file '" + filePath + "'");
            }
            std::string fileName = Path::GetFileName(filePath);
            project->AddTextFileName(fileName, filePath);
            project->Save();
            AddFilePathsToProject("", "", "", project, projectNode);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::RemoveFile(sngcm::ast::Project* project, const std::string& filePath, const std::string& fileName, TreeViewNode* fileNode)
{
    try
    {
        TabPage* tabPage = codeTabControl->GetTabPageByKey(filePath);
        if (tabPage)
        {
            Editor* editor = GetEditorByTabPage(tabPage);
            if (editor)
            {
                if (editor->IsDirty())
                {
                    editor->Save();
                }
            }
            tabPage->Close();
        }
        TreeViewNode* projectNode = fileNode->Parent();
        project->RemoveFile(filePath, fileName);
        project->Save();
        if (EndsWith(filePath, ".cm"))
        {
            // todo: load edit module
        }
        projectNode->RemoveChild(fileNode);
        Invalidate();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::BuildActiveProjectClick()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        sngcm::ast::Project* activeProject = solutionData->GetSolution()->ActiveProject();
        if (!activeProject)
        {
            throw std::runtime_error("no active project set for the solution");
        }
        StartBuilding();
        StartBuild(backend, config, activeProject->FilePath(), BuildRequestKind::build | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::RebuildActiveProjectClick()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        sngcm::ast::Project* activeProject = solutionData->GetSolution()->ActiveProject();
        if (!activeProject)
        {
            throw std::runtime_error("no active project set for the solution");
        }
        StartBuilding();
        StartBuild(backend, config, activeProject->FilePath(), BuildRequestKind::rebuild | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::CleanActiveProjectClick()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        if (!solutionData)
        {
            throw std::runtime_error("no solution open");
        }
        sngcm::ast::Project* activeProject = solutionData->GetSolution()->ActiveProject();
        if (!activeProject)
        {
            throw std::runtime_error("no active project set for the solution");
        }
        StartBuilding();
        StartBuild(backend, config, activeProject->FilePath(), BuildRequestKind::clean | BuildRequestKind::buildDependencies);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
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
    try
    {
        codeTabControl->CloseAllTabPages();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::CloseExternalTabsClick()
{
    try
    {
        std::vector<TabPage*> tabgPagesToClose;
        Component* child = codeTabControl->TabPages().FirstChild();
        while (child)
        {
            if (child->IsTabPage())
            {
                TabPage* tabPage = static_cast<TabPage*>(child);
                Editor* editor = GetEditorByTabPage(tabPage);
                if (editor)
                {
                    SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(editor->FilePath());
                    if (!data || !data->project)
                    {
                        tabgPagesToClose.push_back(tabPage);
                    }
                }
            }
            child = child->NextSibling();
        }
        for (TabPage* tabPage : tabgPagesToClose)
        {
            tabPage->Hide();
            tabPage->Close();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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

void MainWindow::GotoPreviousLocationClick()
{
    try
    {
        locations.GotoPreviousLocation(CurrentLocation());
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::GotoNextLocationClick()
{
    try
    {
        locations.GotoNextLocation(CurrentLocation());
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::CppButtonClick()
{
    backend = "cpp";
    llvmToolButton->SetState(ToolButtonState::normal);
}

void MainWindow::LlvmButtonClick()
{
    backend = "llvm";
    cppToolButton->SetState(ToolButtonState::normal);
}

void MainWindow::DebugButtonClick()
{
    config = "debug";
    releaseToolButton->SetState(ToolButtonState::normal);
}

void MainWindow::ReleaseButtonClick()
{
    config = "release";
    debugToolButton->SetState(ToolButtonState::normal);
}

void MainWindow::StopBuildServerClick()
{
    PutRequest(new cmajor::service::StopBuildRequest());
}

void MainWindow::TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args)
{
    try
    {
        TreeViewNode* node = args.node;
        if (node->Data())
        {
            SolutionTreeViewNodeData* data = static_cast<SolutionTreeViewNodeData*>(node->Data());
            if (data->kind == SolutionTreeViewNodeDataKind::solution || data->kind == SolutionTreeViewNodeDataKind::project)
            {
                switch (node->State())
                {
                    case TreeViewNodeState::collapsed:
                    {
                        node->ExpandAll();
                        break;
                    }
                    case TreeViewNodeState::expanded:
                    {
                        node->CollapseAll();
                        break;
                    }
                }
            }
            else if (data->kind == SolutionTreeViewNodeDataKind::file)
            {
                std::string ext = Path::GetExtension(data->fileName);
                if (ext == ".cm")
                {
                    TabPage* prevTabPage = codeTabControl->GetTabPageByKey(data->key);
                    if (prevTabPage)
                    {
                        codeTabControl->SetSelectedTabPage(prevTabPage);
                    }
                    else
                    {
                        AddCmajorEditor(data->fileName, data->key, data->filePath, data->project);
                        // LoadEditModule(data->project->FilePath()); todo
                    }
                }
                else if (ext == ".xml")
                {
                    TabPage* prevTabPage = codeTabControl->GetTabPageByKey(data->key);
                    if (prevTabPage)
                    {
                        codeTabControl->SetSelectedTabPage(prevTabPage);
                    }
                    else
                    {
                        AddResourceFileEditor(data->fileName, data->key, data->filePath, data->project);
                    }
                }
                else 
                {
                    TabPage* prevTabPage = codeTabControl->GetTabPageByKey(data->key);
                    if (prevTabPage)
                    {
                        codeTabControl->SetSelectedTabPage(prevTabPage);
                    }
                    else
                    {
                        AddTextFileEditor(data->fileName, data->key, data->filePath, data->project);
                    }
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::TreeViewNodeClick(TreeViewNodeClickEventArgs& args)
{
    try
    {
        TreeViewNode* node = args.node;
        if (args.buttons == MouseButtons::lbutton)
        {
            node->Select();
        }
        else if (args.buttons == MouseButtons::rbutton)
        {
            clickActions.clear();
            SolutionTreeViewNodeData* data = static_cast<SolutionTreeViewNodeData*>(node->Data());
            switch (data->kind)
            {
                case SolutionTreeViewNodeDataKind::solution:
                {
                    std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
                    std::unique_ptr<MenuItem> buildMenuItem(new MenuItem("Build"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new BuildSolutionAction(buildMenuItem.get(), this)));
                    contextMenu->AddMenuItem(buildMenuItem.release());
                    std::unique_ptr<MenuItem> rebuildMenuItem(new MenuItem("Rebuild"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new RebuildSolutionAction(rebuildMenuItem.get(), this)));
                    contextMenu->AddMenuItem(rebuildMenuItem.release());
                    std::unique_ptr<MenuItem> cleanMenuItem(new MenuItem("Clean"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new CleanSolutionAction(cleanMenuItem.get(), this)));
                    contextMenu->AddMenuItem(cleanMenuItem.release());
                    std::unique_ptr<MenuItem> addNewProjectMenuItem(new MenuItem("Add New Project..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddNewProjectAction(addNewProjectMenuItem.get(), this)));
                    contextMenu->AddMenuItem(addNewProjectMenuItem.release());
                    std::unique_ptr<MenuItem> addExistingProjectMenuItem(new MenuItem("Add Existing Project..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddExistingProjectAction(addExistingProjectMenuItem.get(), this)));
                    contextMenu->AddMenuItem(addExistingProjectMenuItem.release());
                    std::unique_ptr<MenuItem> openFileLocationMenuItem(new MenuItem("Open File Location"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new OpenFileLocationAction(openFileLocationMenuItem.get(), this, data->solution->FilePath())));
                    contextMenu->AddMenuItem(openFileLocationMenuItem.release());
                    if (contextMenu->HasMenuItems())
                    {
                        Point contentLoc = args.location;
                        TreeView* treeView = node->GetTreeView();
                        if (treeView)
                        {
                            Point treeViewContentLocation = treeView->ContentLocation();
                            Point loc(contentLoc.X, contentLoc.Y - treeViewContentLocation.Y);
                            Point screenLoc = solutionTreeView->ClientToScreen(loc);
                            ShowContextMenu(contextMenu.release(), screenLoc);
                        }
                    }
                    break;
                }
                case SolutionTreeViewNodeDataKind::project:
                {
                    sngcm::ast::Project* project = data->project;
                    std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
                    std::unique_ptr<MenuItem> buildMenuItem(new MenuItem("Build"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new BuildProjectAction(buildMenuItem.get(), this, project)));
                    contextMenu->AddMenuItem(buildMenuItem.release());
                    std::unique_ptr<MenuItem> rebuildMenuItem(new MenuItem("Rebuild"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new RebuildProjectAction(rebuildMenuItem.get(), this, project)));
                    contextMenu->AddMenuItem(rebuildMenuItem.release());
                    std::unique_ptr<MenuItem> cleanMenuItem(new MenuItem("Clean"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new CleanProjectAction(cleanMenuItem.get(), this, project)));
                    contextMenu->AddMenuItem(cleanMenuItem.release());
                    std::unique_ptr<MenuItem> addNewSourceFileMenuItem(new MenuItem("Add New Source File..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddNewSourceFileAction(addNewSourceFileMenuItem.get(), this, project, node)));
                    contextMenu->AddMenuItem(addNewSourceFileMenuItem.release());
                    std::unique_ptr<MenuItem> addExistingSourceFileMenuItem(new MenuItem("Add Existing Source File..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddExistingSourceFileAction(addExistingSourceFileMenuItem.get(), this, project, node)));
                    contextMenu->AddMenuItem(addExistingSourceFileMenuItem.release());
                    std::unique_ptr<MenuItem> addNewResourceFileMenuItem(new MenuItem("Add New Resource File..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddNewResourceFileAction(addNewResourceFileMenuItem.get(), this, project, node)));
                    contextMenu->AddMenuItem(addNewResourceFileMenuItem.release());
                    std::unique_ptr<MenuItem> addExistingResouceFileMenuItem(new MenuItem("Add Existing Resource File..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddExistingResourceFileAction(addExistingResouceFileMenuItem.get(), this, project, node)));
                    contextMenu->AddMenuItem(addExistingResouceFileMenuItem.release());
                    std::unique_ptr<MenuItem> addNewTextFileMenuItem(new MenuItem("Add New Text File..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddNewTextFileAction(addNewTextFileMenuItem.get(), this, project, node)));
                    contextMenu->AddMenuItem(addNewTextFileMenuItem.release());
                    std::unique_ptr<MenuItem> addExistingTextFileMenuItem(new MenuItem("Add Existing Text File..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new AddExistingTextFileAction(addExistingTextFileMenuItem.get(), this, project, node)));
                    contextMenu->AddMenuItem(addExistingTextFileMenuItem.release());
                    if (solutionData->GetSolution()->ActiveProject() != project)
                    {
                        std::unique_ptr<MenuItem> setActiveMenuItem(new MenuItem("Set Active"));
                        clickActions.push_back(std::unique_ptr<ClickAction>(new SetActiveProjectAction(setActiveMenuItem.get(), this, project, node)));
                        contextMenu->AddMenuItem(setActiveMenuItem.release());
                    }
                    std::unique_ptr<MenuItem> openFileLocationMenuItem(new MenuItem("Open File Location"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new OpenFileLocationAction(openFileLocationMenuItem.get(), this, project->FilePath())));
                    contextMenu->AddMenuItem(openFileLocationMenuItem.release());
                    std::unique_ptr<MenuItem> removeMenuItem(new MenuItem("Remove"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new RemoveProjectAction(removeMenuItem.get(), this, project)));
                    contextMenu->AddMenuItem(removeMenuItem.release());
                    if (contextMenu->HasMenuItems())
                    {
                        Point contentLoc = args.location;
                        TreeView* treeView = node->GetTreeView();
                        if (treeView)
                        {
                            Point treeViewContentLocation = treeView->ContentLocation();
                            Point loc(contentLoc.X, contentLoc.Y - treeViewContentLocation.Y);
                            Point screenLoc = solutionTreeView->ClientToScreen(loc);
                            ShowContextMenu(contextMenu.release(), screenLoc);
                        }
                    }
                    break;
                }
                case SolutionTreeViewNodeDataKind::file:
                {
                    sngcm::ast::Project* project = data->project;
                    std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
                    std::unique_ptr<MenuItem> openFileLocationMenuItem(new MenuItem("Open File Location"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new OpenFileLocationAction(openFileLocationMenuItem.get(), this, data->filePath)));
                    contextMenu->AddMenuItem(openFileLocationMenuItem.release());
                    std::unique_ptr<MenuItem> removeMenuItem(new MenuItem("Remove"));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new RemoveFileAction(removeMenuItem.get(), this, data->project, data->filePath, data->fileName, node)));
                    contextMenu->AddMenuItem(removeMenuItem.release());
                    if (contextMenu->HasMenuItems())
                    {
                        Point contentLoc = args.location;
                        TreeView* treeView = node->GetTreeView();
                        if (treeView)
                        {
                            Point treeViewContentLocation = treeView->ContentLocation();
                            Point loc(contentLoc.X, contentLoc.Y - treeViewContentLocation.Y);
                            Point screenLoc = solutionTreeView->ClientToScreen(loc);
                            ShowContextMenu(contextMenu.release(), screenLoc);
                        }
                    }
                    break;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

Editor* MainWindow::GetEditorByTabPage(TabPage* tabPage) const
{
    auto it = tabPageEditorMap.find(tabPage);
    if (it != tabPageEditorMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

CmajorEditor* MainWindow::AddCmajorEditor(const std::string& fileName, const std::string& key, const std::string& filePath, sngcm::ast::Project* project)
{
    std::unique_ptr<TabPage> tabPage(new TabPage(fileName, key));
    BreakpointList* breakpointList = nullptr;
    if (project)
    {
        ProjectData* projectData = solutionData->GetProjectDataByProject(project);
        if (projectData)
        {
            breakpointList = &projectData->GetBreakpointCollection().GetBreakpointList(filePath);
        }
    }
    if (!breakpointList)
    {
        breakpointList = &solutionData->GetSolutionBreakpointCollection().GetBreakpointList(filePath);
    }
    std::unique_ptr<CmajorEditor> editorPtr(new CmajorEditor(CmajorEditorCreateParams(filePath, CmajorSourceCodeViewCreateParams().Defaults(), DebugStripCreateParams(breakpointList).Defaults()).Defaults()));
    CmajorEditor* editor = editorPtr.get();
    CmajorSourceCodeView* sourceCodeView = editor->SourceCodeView();
    if (sourceCodeView)
    {
        sourceCodeView->DirtyChanged().AddHandler(this, &MainWindow::EditorDirtyChanged);
        sourceCodeView->CCDirtyChanged().AddHandler(this, &MainWindow::EditorCCDirtyChanged);
        sourceCodeView->ReadOnlyChanged().AddHandler(this, &MainWindow::EditorReadOnlyChanged);
        sourceCodeView->CaretPosChanged().AddHandler(this, &MainWindow::EditorCaretPosChanged);
        sourceCodeView->SelectionChanged().AddHandler(this, &MainWindow::EditorSelectionChanged);
        sourceCodeView->RightClick().AddHandler(this, &MainWindow::EditorRightClick);
        sourceCodeView->Copy().AddHandler(this, &MainWindow::CopyClick);
        sourceCodeView->Cut().AddHandler(this, &MainWindow::CutClick);
        sourceCodeView->Paste().AddHandler(this, &MainWindow::PasteClick);
        if (state != MainWindowState::idle)
        {
            sourceCodeView->SetReadOnly();
        }
        DebugStrip* debugStrip = sourceCodeView->GetDebugStrip();
        if (debugStrip)
        {
            debugStrip->BreakpointAdded().AddHandler(this, &MainWindow::BreakpointAdded);
            debugStrip->BreakpointRemoved().AddHandler(this, &MainWindow::BreakpointRemoved);
        }
    }
    tabPage->AddChild(editorPtr.release());
    tabPageEditorMap[tabPage.get()] = editor;
    codeTabControl->AddTabPage(tabPage.release());
    SetEditorState();
    SetFocusToEditor();
    return editor;
}

ResourceFileEditor* MainWindow::AddResourceFileEditor(const std::string& fileName, const std::string& key, const std::string& filePath, sngcm::ast::Project* project)
{
    std::unique_ptr<TabPage> tabPage(new TabPage(fileName, key));
    std::unique_ptr<ResourceFileEditor> editorPtr(new ResourceFileEditor(ResourceFileEditorCreateParams(filePath).Defaults()));
    ResourceFileEditor* editor = editorPtr.get();
    TextView* textView = editor->GetTextView();
    if (textView)
    {
        textView->DirtyChanged().AddHandler(this, &MainWindow::EditorDirtyChanged);
        textView->ReadOnlyChanged().AddHandler(this, &MainWindow::EditorReadOnlyChanged);
        textView->CaretPosChanged().AddHandler(this, &MainWindow::EditorCaretPosChanged);
        textView->SelectionChanged().AddHandler(this, &MainWindow::EditorSelectionChanged);
        textView->Copy().AddHandler(this, &MainWindow::CopyClick);
        textView->Cut().AddHandler(this, &MainWindow::CutClick);
        textView->Paste().AddHandler(this, &MainWindow::PasteClick);
        if (state != MainWindowState::idle)
        {
            textView->SetReadOnly();
        }
    }
    tabPage->AddChild(editorPtr.release());
    tabPageEditorMap[tabPage.get()] = editor;
    codeTabControl->AddTabPage(tabPage.release());
    SetEditorState();
    SetFocusToEditor();
    return editor;
}

TextFileEditor* MainWindow::AddTextFileEditor(const std::string& fileName, const std::string& key, const std::string& filePath, sngcm::ast::Project* project)
{
    std::unique_ptr<TabPage> tabPage(new TabPage(fileName, key));
    std::unique_ptr<TextFileEditor> editorPtr(new TextFileEditor(TextFileEditorCreateParams(filePath).Defaults()));
    TextFileEditor* editor = editorPtr.get();
    TextView* textView = editor->GetTextView();
    if (textView)
    {
        textView->DirtyChanged().AddHandler(this, &MainWindow::EditorDirtyChanged);
        textView->ReadOnlyChanged().AddHandler(this, &MainWindow::EditorReadOnlyChanged);
        textView->CaretPosChanged().AddHandler(this, &MainWindow::EditorCaretPosChanged);
        textView->SelectionChanged().AddHandler(this, &MainWindow::EditorSelectionChanged);
        textView->Copy().AddHandler(this, &MainWindow::CopyClick);
        textView->Cut().AddHandler(this, &MainWindow::CutClick);
        textView->Paste().AddHandler(this, &MainWindow::PasteClick);
        if (state != MainWindowState::idle)
        {
            textView->SetReadOnly();
        }
    }
    tabPage->AddChild(editorPtr.release());
    tabPageEditorMap[tabPage.get()] = editor;
    codeTabControl->AddTabPage(tabPage.release());
    SetEditorState();
    SetFocusToEditor();
    return editor;
}

void MainWindow::AddFilePathsToProject(const std::string& newSourceFilePath, const std::string& newResourceFilePath, const std::string& newTextFilePath, sngcm::ast::Project* project, TreeViewNode* projectNode)
{
    projectNode->RemoveChildren();
    sngcm::ast::Solution* solution = solutionData->GetSolution();
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(sourceFilePath);
        if (data)
        {
            std::unique_ptr<TreeViewNode> fileNode(new TreeViewNode(data->fileName));
            fileNode->SetData(data);
            projectNode->AddChild(fileNode.release());
        }
        else
        {
            std::unique_ptr<SolutionTreeViewNodeData> newData(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::file, solution, project, sourceFilePath, Path::GetFileName(sourceFilePath)));
            data = newData.get();
            std::unique_ptr<TreeViewNode> fileNode(new TreeViewNode(newData->fileName));
            fileNode->SetData(newData.get());
            projectNode->AddChild(fileNode.release());
            solutionData->AddTreeViewNodeData(newData.release());
        }
        if (sourceFilePath == newSourceFilePath)
        {
            std::ofstream file(sourceFilePath);
            AddCmajorEditor(data->fileName, data->key, data->filePath, project);
        }
    }
    for (const std::string& resourceFilePath : project->ResourceFilePaths())
    {
        SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(resourceFilePath);
        if (data)
        {
            std::unique_ptr<TreeViewNode> fileNode(new TreeViewNode(data->fileName));
            fileNode->SetData(data);
            projectNode->AddChild(fileNode.release());
        }
        else
        {
            std::unique_ptr<SolutionTreeViewNodeData> newData(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::file, solution, project, resourceFilePath, Path::GetFileName(resourceFilePath)));
            data = newData.get();
            std::unique_ptr<TreeViewNode> fileNode(new TreeViewNode(newData->fileName));
            fileNode->SetData(newData.get());
            projectNode->AddChild(fileNode.release());
            solutionData->AddTreeViewNodeData(newData.release());
        }
        if (resourceFilePath == newResourceFilePath)
        {
            std::ofstream file(resourceFilePath);
            AddResourceFileEditor(data->fileName, data->key, data->filePath, project);
        }
    }
    for (const std::string& textFilePath : project->TextFilePaths())
    {
        SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(textFilePath);
        if (data)
        {
            std::unique_ptr<TreeViewNode> fileNode(new TreeViewNode(data->fileName));
            fileNode->SetData(data);
            projectNode->AddChild(fileNode.release());
        }
        else
        {
            std::unique_ptr<SolutionTreeViewNodeData> newData(new SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind::file, solution, project, textFilePath, Path::GetFileName(textFilePath)));
            data = newData.get();
            std::unique_ptr<TreeViewNode> fileNode(new TreeViewNode(data->fileName));
            fileNode->SetData(data);
            projectNode->AddChild(fileNode.release());
            solutionData->AddTreeViewNodeData(newData.release());
        }
        if (textFilePath == newTextFilePath)
        {
            std::ofstream textFile(textFilePath);
            AddTextFileEditor(data->fileName, data->key, data->filePath, project);
        }
    }
    projectNode->Expand();
    solutionTreeView->SetChanged();
    solutionTreeView->Invalidate();
}

void MainWindow::CodeTabPageSelected()
{
    TabPage* selectedTabPage = codeTabControl->SelectedTabPage();
    Editor* editor = GetEditorByTabPage(selectedTabPage);
    if (editor)
    {
        editor->Select();
        SetEditorState();
    }
}

void MainWindow::CodeTabPageRemoved(ControlEventArgs& args)
{
    SetEditorState();
/*  TODO:
    HideCodeCompletionList();
    Control* removedControl = args.control;
    if (removedControl is TabPage*)
    {
        TabPage* removedTabPage = cast<TabPage*>(removedControl);
        Cm.Views.Editor* editor = GetEditorByTabPage(removedTabPage);
        if (editor != null)
        {
            if (editor->IsDirty())
            {
                MessageBoxResult result = MessageBox.Show(editor->FilePath() + " is modified. Save changes?", "Question", this, MessageBoxType.MB_YESNO);
                if (result == MessageBoxResult.yes)
                {
                    editor->Save();
                }
            }
        }
        editorByTabPageMap.Remove(removedTabPage);
        if (removedTabPage == debugTabPage)
        {
            debugTabPage = null;
        }
    }
    if (codeTabControl->TabPages().IsEmpty())
    {
        sourceFilePathStatusBarItem->SetText("");
        ResetFocusedControl();
    }
*/
}

void MainWindow::OutputTabControlTabPageRemoved(ControlEventArgs& args)
{
    if (args.control == outputTabPage)
    {
        outputTabPage = nullptr;
        outputLogView = nullptr;
    }
    else if (args.control == errorTabPage)
    {
        errorTabPage = nullptr;
        errorView = nullptr;
    }
    else if (args.control == searchResultsTabPage)
    {
        searchResultsTabPage = nullptr;
        searchResultsView = nullptr;
    }
    else if (args.control == logTabPage)
    {
        logTabPage = nullptr;
        log = nullptr;
    }
}

void MainWindow::OutputTabControlTabPageSelected()
{
    // todo
}

LogView* MainWindow::GetOutputLogView()
{
    if (!outputLogView)
    {
        std::unique_ptr<TabPage> outputTabPagePtr(new TabPage("Output", "output"));
        outputTabPage = outputTabPagePtr.get();
        std::unique_ptr<LogView> outputLogViewPtr(new LogView(TextViewCreateParams().Defaults()));
        outputLogView = outputLogViewPtr.get();
        outputLogView->SetDoubleBuffered();
        std::unique_ptr<Control> scrollableOutputLogView(new ScrollableControl(ScrollableControlCreateParams(outputLogViewPtr.release()).SetDock(Dock::fill)));
        outputTabPage->AddChild(scrollableOutputLogView.release());
        outputTabControl->AddTabPage(outputTabPagePtr.release());
    }
    outputTabPage->Select();
    return outputLogView;
}

ErrorView* MainWindow::GetErrorView()
{
    if (!errorView)
    {
        std::unique_ptr<TabPage> errorTabPagePtr(new TabPage("Errors", "errors"));
        errorTabPage = errorTabPagePtr.get();
        std::unique_ptr<ErrorView> errorViewPtr(new ErrorView(ErrorViewCreateParams().Defaults()));
        errorView = errorViewPtr.get();
        errorView->ViewError().AddHandler(this, &MainWindow::ViewError);
        errorTabPage->AddChild(errorViewPtr.release());
        outputTabControl->AddTabPage(errorTabPagePtr.release());
    }
    errorTabPage->Select();
    return errorView;
}

void MainWindow::ViewError(ViewErrorArgs& args)
{
    try
    {
        CompileError* error = args.error;
        if (error)
        {
            TabPage* tabPage = codeTabControl->GetTabPageByKey(error->file);
            Editor* editor = nullptr;
            if (tabPage)
            {
                editor = GetEditorByTabPage(tabPage);
                tabPage->Select();
            }
            if (!editor)
            {
                SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(error->file);
                if (data)
                {
                    editor = AddCmajorEditor(data->fileName, data->key, data->filePath, data->project);
                }
                else
                {
                    editor = AddCmajorEditor(Path::GetFileName(error->file), error->file, error->file, nullptr);
                }
            }
            TextView* textView = editor->GetTextView();
            if (textView)
            {
                int line = error->line;
                int scol = error->scol;
                int ecol = error->ecol;
                textView->EnsureLineVisible(line);
                textView->SetCaretLineCol(std::min(line, static_cast<int>(textView->Lines().size())), 1 + textView->LineNumberFieldLength());
                textView->ScrollToCaret();
                textView->SetFocus();
                textView->Invalidate();
                if (scol != 0 && ecol != 0 && scol != ecol)
                {
                    textView->ResetSelection();
                    SourcePos start(line, scol);
                    SourcePos end(line, ecol);
                    textView->ExtendSelection(start, end);
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

Editor* MainWindow::CurrentEditor() const
{
    if (codeTabControl->TabPages().IsEmpty()) return nullptr;
    TabPage* selectedTabPage = codeTabControl->SelectedTabPage();
    if (selectedTabPage)
    {
        Editor* editor = GetEditorByTabPage(selectedTabPage);
        if (editor)
        {
            return editor;
        }
    }
    return nullptr;
}

SearchResultsView* MainWindow::GetSearchResultsView()
{
    if (!searchResultsView)
    {
        searchResultsView = new SearchResultsView();
        searchResultsView->ViewSearchResult().AddHandler(this, &MainWindow::ViewSearchResult);
        searchResultsTabPage = new TabPage("Search Results", "searchResults");
        searchResultsTabPage->AddChild(searchResultsView);
        outputTabControl->AddTabPage(searchResultsTabPage);
    }
    searchResultsTabPage->Select();
    return searchResultsView;
}

} // namespace cmcode
