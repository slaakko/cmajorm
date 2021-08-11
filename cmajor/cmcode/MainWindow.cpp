// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/MainWindow.hpp>
#include <cmajor/cmcode/AboutDialog.hpp>
#include <cmajor/cmcode/BuildSettingsDialog.hpp>
#include <cmajor/cmcode/OptionsDialog.hpp>
#include <cmajor/cmcode/StartupDialog.hpp>
#include <cmajor/cmcode/ProjectReferencesDialog.hpp>
#include <cmajor/cmcode/Config.hpp>
#include <cmajor/cmcode/Action.hpp>
#include <cmajor/cmcode/Build.hpp>
#include <cmajor/cmcode/Debug.hpp>
#include <cmajor/cmcode/Run.hpp>
#include <cmajor/cmcode/CodeCompletion.hpp>
#include <cmajor/cmcode/ToolBar.hpp>
#include <cmajor/cmcode/AddNewProjectDialog.hpp>
#include <cmajor/cmcode/NewProjectDialog.hpp>
#include <cmajor/cmcode/AddNewSourceFileDialog.hpp>
#include <cmajor/cmcode/AddNewResourceFileDialog.hpp>
#include <cmajor/cmcode/AddNewTextFileDialog.hpp>
#include <cmajor/cmcode/GotoLineDialog.hpp>
#include <cmajor/cmcode/ProgramArgumentsDialog.hpp>
#include <cmajor/cmcode/SelectProjectTypeDialog.hpp>
#include <cmajor/wing/Ansi.hpp>
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
#include <cmajor/cmsvc/RunService.hpp>
#include <cmajor/cmsvc/RunServiceRequest.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/rex/Context.hpp>
#include <soulng/rex/Match.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmcode {

const char* cmajorCodeVersion = "4.1.0";

bool CmCodeLogFlagExists()
{
    std::string cmcodeLogFlagFilePath = Path::Combine(cmajor::service::CmajorConfigDir(), "cmcode.log.flag");
    return boost::filesystem::exists(cmcodeLogFlagFilePath);
}

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

bool IsProgramTarget(Target target)
{
    return target == Target::program || target == Target::winapp || target == Target::winguiapp;
}

ExpressionEvaluateRequest::ExpressionEvaluateRequest(const std::string& expression_, const Point& screenLoc_) : expression(expression_), screenLoc(screenLoc_)
{
}

MainWindow::MainWindow(const std::string& filePath) : Window(WindowCreateParams().Text("Cmajor Code").WindowClassName("cmajor.code." + std::to_string(GetPid()))),
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
    buildSettingsMenuItem(nullptr),
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
    consoleTabPage(nullptr),
    console(nullptr),
    debugTabPage(nullptr),
    statusBar(nullptr),
    searchResultsTabPage(nullptr),
    searchResultsView(nullptr),
    callStackTabPage(nullptr),
    callStackView(nullptr),
    localsTabPage(nullptr),
    localsView(nullptr),
    buildIndicatorStatuBarItem(nullptr), 
    editorReadWriteIndicatorStatusBarItem(nullptr),
    editorDirtyIndicatorStatusBarItem(nullptr),
    sourceFilePathStatusBarItem(nullptr),
    codeCompletionStatusBarItem(nullptr),
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
    programRunning(false),
    startDebugging(false),
    signalReceived(false),
    callStackDepth(-1),
    callStackOpen(false),
    localsViewOpen(false),
    backend("cpp"),
    config("debug"),
    pid(GetPid()),
    ccState(CCState::idle),
    cmajorCodeFormat("cmajor.code"),
    locations(this),
    toolTipWindow(new ToolTip(ToolTipCreateParams().Defaults())),
    codeCompletionListView(nullptr),
    paramHelpView(nullptr),
    ccTimerRunning(false),
    editModuleLoaded(false),
    ccCanSelect(false)
{
    buildIndicatorTexts.push_back("|");
    buildIndicatorTexts.push_back("/");
    buildIndicatorTexts.push_back(ToUtf8(std::u32string(1, char32_t(0x2015))));
    buildIndicatorTexts.push_back("\\");
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
    std::unique_ptr<MenuItem> toggleCodeCompletionMenuItemPtr(new MenuItem("&Toggle Code Completion On/Off"));
    toggleCodeCompletionMenuItemPtr->SetShortcut(Keys::controlModifier | Keys::t);
    toggleCodeCompletionMenuItemPtr->Click().AddHandler(this, &MainWindow::ToggleCodeCompletionClick);
    editMenuItem->AddMenuItem(toggleCodeCompletionMenuItemPtr.release());
    editMenuItem->AddMenuItem(new MenuItemSeparator());
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
    buildMenuItem->AddMenuItem(new MenuItemSeparator());
    std::unique_ptr<MenuItem> buildSettingsMenuItemPtr(new MenuItem("&Settings..."));
    buildSettingsMenuItem = buildSettingsMenuItemPtr.get();
    buildSettingsMenuItem->Click().AddHandler(this, &MainWindow::BuildSettingsClick);
    buildMenuItem->AddMenuItem(buildSettingsMenuItemPtr.release());
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
    solutionTreeView->NodeExpanded().AddHandler(this, &MainWindow::TreeViewNodeExpanded);
    solutionTreeView->NodeCollapsed().AddHandler(this, &MainWindow::TreeViewNodeCollapsed);
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
    if (CmCodeLogFlagExists())
    {
        std::unique_ptr<TabPage> logTabPagePtr(new TabPage("Log", "log"));
        logTabPage = logTabPagePtr.get();
        std::unique_ptr<LogView> logViewPtr(new LogView(TextViewCreateParams().Defaults()));
        log = logViewPtr.get();
        log->SetDoubleBuffered();
        std::unique_ptr<Control> scrollableLogView(new ScrollableControl(ScrollableControlCreateParams(logViewPtr.release()).SetDock(Dock::fill)));
        logTabPage->AddChild(scrollableLogView.release());
        Application::SetLogView(log);
        outputTabControl->AddTabPage(logTabPagePtr.release());
    }
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
    std::unique_ptr<StatusBarTextItem> codeCompletionLabelStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(11).Text("Code Completion Status:").BorderStyle(StatusBarItemBorderStyle::flat)));
    statusBar->AddItem(codeCompletionLabelStatusBarItemPtr.release());
    std::unique_ptr<StatusBarTextItem> codeCompletionStatusBarItemPtr(new StatusBarTextItem(StatusBarTextItemCreateParams().MaxTextLength(5).BorderStyle(StatusBarItemBorderStyle::sunken)));
    codeCompletionStatusBarItem = codeCompletionStatusBarItemPtr.get();
    statusBar->AddItem(codeCompletionStatusBarItemPtr.release());
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

    AddChild(toolTipWindow);

    CodeCompletionHelpView* codeCompletionHelpView = new CodeCompletionHelpView(CCHelpCreateParams().Defaults(), CodeCompletionHelpViewCreateParams().Defaults());
    codeCompletionHelpView->Hide();
    AddChild(codeCompletionHelpView);
    codeCompletionListView = new CodeCompletionListView(CCListCreateParams().Defaults(), CodeCompletionListViewCreateParams().Defaults(), codeCompletionHelpView);
    codeCompletionListView->Hide();
    AddChild(codeCompletionListView);
    paramHelpView = new CodeCompletionParamHelpView(ParamHelpCreateParams().Defaults(), CodeCompletionParamHelpViewCreateParams().Defaults());
    paramHelpView->Hide();
    AddChild(paramHelpView);

    locations.SetToolButtons(prevToolButton, nextToolButton);

    SetServiceMessageHandlerView(this);
    StartRequestDispatcher();

    SetState(MainWindowState::idle);
    LoadConfigurationSettings();

    AddClipboardListener();

    const Options& options = GetOptions();
    if (filePath.empty() && options.showStartupDialog)
    {
        const std::vector<RecentSolution>& recentSolutions = GetRecentSolutions();
        if (!recentSolutions.empty())
        {
            SetTimer(startupDialogTimer, startupDialogTimerDelay);
        }
    }

    SetTimer(configurationSaveTimerId, configurationSavePeriod);

    if (options.codeCompletion)
    {
        codeCompletionStatusBarItem->SetText("On");
        StartCodeCompletion();
    }
    else
    {
        codeCompletionStatusBarItem->SetText("Off");
    }

    if (!filePath.empty())
    {
        OpenProject(filePath);
    }
}

void MainWindow::ShowStartupDialog()
{
    StartupDialog dialog;
    if (dialog.ShowDialog(*this) == DialogResult::ok)
    {
        const RecentSolution& selectedSolution = dialog.GetSelectedSolution();
        OpenProject(selectedSolution.filePath);
    }
}

MainWindow::~MainWindow()
{
    SetServiceMessageHandlerView(nullptr);
    if (state == MainWindowState::debugging)
    {
        StopDebugService();
    }
    else if (state == MainWindowState::running)
    {
        StopRunService();
    }
    StopCodeCompletion(false);
    StopRequestDispatcher();
}

void MainWindow::OnWindowClosing(CancelArgs& args)
{
    try
    {
        Window::OnWindowClosing(args);
        if (!CloseSolution())
        {
            args.cancel = true;
        }
        else
        {
            SaveConfigurationSettings();
            SaveProjectData();
            SaveSolutionData();
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
        if (args.timerId == startupDialogTimer)
        {
            KillTimer(startupDialogTimer);
            ShowStartupDialog();
        }
        else if (args.timerId == buildProgressTimerId)
        {
            ShowBuildProgress();
        }
        else if (args.timerId == configurationSaveTimerId)
        {
            SaveConfigurationSettings();
            SaveProjectData();
            SaveSolutionData();
        }
        else if (args.timerId == toolTipTimerId)
        {
            KillTimer(toolTipTimerId);
            toolTipWindow->Hide();
        }
        else if (args.timerId == ccTimerId)
        {
            ParseSource();
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
    if (windowSettings.defined)
    {
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
}

void MainWindow::OnSizeChanged()
{
    Window::OnSizeChanged();
    if (GetWindowState() == WindowState::maximized && setMaximizedSplitterDistance)
    {
        setMaximizedSplitterDistance = false;
        const WindowSettings& windowSettings = GetWindowSettings();
        if (windowSettings.defined)
        {
            horizontalSplitContainer->SetSplitterDistance(windowSettings.maximizedHorizontalSplitterDistance);
            verticalSplitContainer->SetSplitterDistance(windowSettings.maximizedVerticalSplitterDistance);
        }
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
    if (windowSettings.defined)
    {
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
}

void MainWindow::SaveConfigurationSettings()
{
    WindowState windowState = GetWindowState();
    WindowSettings& windowSettings = GetWindowSettings();
    windowSettings.defined = true;
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

void MainWindow::SaveProjectData()
{
    if (!solutionData) return;
    for (const auto& projectData : solutionData->Projects())
    {
        if (projectData->Changed())
        {
            std::string projectSettingsFilePath = projectData->GetProject()->FilePath();
            projectSettingsFilePath.append(".settings.xml");
            projectData->Save(projectSettingsFilePath);
        }
    }
}

void MainWindow::SaveSolutionData()
{
    if (!solutionData) return;
    Editor* editor = CurrentEditor();
    if (editor)
    {
        solutionData->SetCurrentOpenFile(editor->FilePath());
        TextView* textView = editor->GetTextView();
        if (textView)
        {
            solutionData->SetCurrentCursorLine(textView->CaretLine());
        }
    }
    solutionData->SetCallStackOpen(callStackOpen);
    solutionData->SetLocalsViewOpen(localsViewOpen);
    if (solutionData->Changed())
    {
        std::string solutionSettingsFilePath = solutionData->GetSolution()->FilePath();
        solutionSettingsFilePath.append(".settings.xml");
        solutionData->Save(solutionSettingsFilePath);
    }
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
    GetErrorView()->Clear();
    ClearOutput();
    ResetEditModuleCache();
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

void MainWindow::StartDebugging()
{
    ResetDebugLocations();
    if (state == MainWindowState::debugging) return;
    expressionEvaluateRequests.clear();
    savedLocation = ::Location();
    SetEditorsReadOnly();
    startDebugging = true;
    signalReceived = false;
    SetState(MainWindowState::debugging);
    ClearOutput();
    GetConsole()->Clear();
    ClearCallStack();
    ClearLocals();
    sngcm::ast::Solution* solution = solutionData->GetSolution();
    sngcm::ast::Project* activeProject = solution->ActiveProject();
    ProjectData* projectData = solutionData->GetProjectDataByProject(activeProject);
    if (!projectData)
    {
        throw std::runtime_error("active project has no data");
    }
    const std::string& programArguments = projectData->ProgramArguments();
    std::vector<Breakpoint*> breakpoints = solutionData->GetBreakpoints();
    StartDebugService(pid, backend, config, activeProject, programArguments, breakpoints);
}

void MainWindow::StopDebugging()
{
    ResetDebugLocations();
    savedLocation = ::Location();
    startDebuggingMenuItem->SetText("Start Debugging");
    startDebuggingToolButton->SetToolTip("Start Debugging (F5)");
    SetState(MainWindowState::idle);
    SetEditorsReadWrite();
    PutOutputServiceMessage("debugging stopped");
}

void MainWindow::StartRunning()
{
    sngcm::ast::Solution* solution = solutionData->GetSolution();
    sngcm::ast::Project* activeProject = solution->ActiveProject();
    if (activeProject->GetTarget() != sngcm::ast::Target::program && activeProject->GetTarget() != sngcm::ast::Target::winapp && activeProject->GetTarget() != sngcm::ast::Target::winguiapp)
    {
        throw std::runtime_error("project '" + ToUtf8(activeProject->Name()) + "' is a library project");
    }
    ProjectData* projectData = solutionData->GetProjectDataByProject(activeProject);
    if (!projectData)
    {
        throw std::runtime_error("active project has no data");
    }
    SetEditorsReadOnly();
    SetState(MainWindowState::running);
    ClearOutput();
    GetConsole()->Clear();
    const std::string& programArguments = projectData->ProgramArguments();
    RunProgram(backend, config, activeProject, programArguments);
}

void MainWindow::StopRunning()
{
    SetState(MainWindowState::idle);
    SetEditorsReadWrite();
    PutOutputServiceMessage("run service stopped");
}

void MainWindow::StartCodeCompletion()
{
    try
    {
        StartCodeCompletionService(pid);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::StopCodeCompletion(bool log)
{
    editModuleLoaded = false;
    StopCodeCompletionService(log);
}

void MainWindow::LoadEditModule()
{
    if (!solutionData) return;
    sngcm::ast::Solution* sln = solutionData->GetSolution();
    if (!sln) return;
    editModuleLoaded = true;
    LoadEditModule(sln->ActiveProject());
}

void MainWindow::LoadEditModule(Project* project)
{
    if (!project) return;
    cmcode::LoadEditModule(project->FilePath(), backend, config);
}

void MainWindow::LoadEditModuleForCurrentFile()
{
    LoadEditModule(CurrentProject());
}

void MainWindow::ResetEditModuleCache()
{
    editModuleLoaded = false;
    ccState = CCState::idle;
    cmcode::ResetEditModuleCache();
}

void MainWindow::ParseSource()
{
    Project* project = CurrentProject();
    if (!project) return;
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    if (ccState != CCState::editModuleLoaded) return;
    Editor* editor = CurrentEditor();
    if (!editor) return;
    if (!editor->IsCmajorEditor()) return;
    if (!editor->IsCCDirty()) return;
    CmajorEditor* cmajorEditor = static_cast<CmajorEditor*>(editor);
    CmajorSourceCodeView* cmajorSourceCodeView = cmajorEditor->SourceCodeView();
    if (cmajorSourceCodeView->CCLineNumber() == cmajorSourceCodeView->CaretLine()) return;
    cmajorSourceCodeView->SetCCLineNumber(cmajorSourceCodeView->CaretLine());
    cmajorSourceCodeView->ResetCCDirty();
    if (!editModuleLoaded)
    {
        LoadEditModule();
    } 
    cmcode::ParseSource(project->FilePath(), backend, config, cmajorSourceCodeView->FilePath(), cmajorSourceCodeView->GetCursorText());
}

void MainWindow::GetParamHelpList(int symbolIndex)
{
    Project* project = CurrentProject();
    if (!project) return;
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    if (ccState != CCState::editModuleLoaded) return;
    Editor* editor = CurrentEditor();
    if (!editor) return;
    if (!editor->IsCmajorEditor()) return;
    cmcode::GetParamHelpList(project->FilePath(), backend, config, editor->FilePath(), symbolIndex);
}

void MainWindow::ShowBuildProgress()
{
    if (state == MainWindowState::building)
    {
        buildIndicatorStatuBarItem->SetText(buildIndicatorTexts[buildProgressCounter % 4]);
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
                break;
            }
            case ServiceMessageKind::stopBuild:
            {
                HandleStopBuild();
                break;
            }
            case ServiceMessageKind::startDebugReply:
            {
                StartReplyServiceMessage* message = static_cast<StartReplyServiceMessage*>(serviceMessage.get());
                HandleStartDebugReply(message->GetStartDebugReply());
                break;
            }
            case ServiceMessageKind::startError:
            {
                StartErrorServiceMessage* message = static_cast<StartErrorServiceMessage*>(serviceMessage.get());
                HandleStartDebugError(message->Error());
                break;
            }
            case ServiceMessageKind::continueReply:
            {
                ContinueReplyServiceMessage* message = static_cast<ContinueReplyServiceMessage*>(serviceMessage.get());
                HandleContinueReply(message->GetContinueReply());
                break;
            }
            case ServiceMessageKind::nextReply:
            {
                NextReplyServiceMessage* message = static_cast<NextReplyServiceMessage*>(serviceMessage.get());
                HandleNextReply(message->GetNextReply());
                break;
            }
            case ServiceMessageKind::stepReply:
            {
                StepReplyServiceMessage* message = static_cast<StepReplyServiceMessage*>(serviceMessage.get());
                HandleStepReply(message->GetStepReply());
                break;
            }
            case ServiceMessageKind::finishReply:
            {
                FinishReplyServiceMessage* message = static_cast<FinishReplyServiceMessage*>(serviceMessage.get());
                HandleFinishReply(message->GetFinishReply());
                break;
            }
            case ServiceMessageKind::untilReply:
            {
                UntilReplyServiceMessage* message = static_cast<UntilReplyServiceMessage*>(serviceMessage.get());
                HandleUntilReply(message->GetUntilReply());
                break;
            }
            case ServiceMessageKind::breakReply:
            {
                BreakReplyServiceMessage* message = static_cast<BreakReplyServiceMessage*>(serviceMessage.get());
                HandleBreakReply(message->GetBreakReply());
                break;
            }
            case ServiceMessageKind::deleteReply:
            {
                DeleteReplyServiceMessage* message = static_cast<DeleteReplyServiceMessage*>(serviceMessage.get());
                HandleDeleteReply(message->GetDeleteReply());
                break;
            }
            case ServiceMessageKind::depthReply:
            {
                DepthReplyServiceMessage* message = static_cast<DepthReplyServiceMessage*>(serviceMessage.get());
                HandleDepthReply(message->GetDepthReply());
                break;
            }
            case ServiceMessageKind::framesReply:
            {
                FramesReplyServiceMessage* message = static_cast<FramesReplyServiceMessage*>(serviceMessage.get());
                HandleFramesReply(message->GetFramesReply());
                break;
            }
            case ServiceMessageKind::evaluateReply:
            {
                EvaluateReplyServiceMessage* message = static_cast<EvaluateReplyServiceMessage*>(serviceMessage.get());
                HandleEvaluateReply(message->GetEvaluateReply(), message->RequestId());
                break;
            }
            case ServiceMessageKind::countReply:
            {
                CountReplyServiceMessage* message = static_cast<CountReplyServiceMessage*>(serviceMessage.get());
                HandleCountReply(message->GetCountReply());
                break;
            }
            case ServiceMessageKind::evaluateChildReply:
            {
                EvaluateChildReplyServiceMessage* message = static_cast<EvaluateChildReplyServiceMessage*>(serviceMessage.get());
                HandleEvaluateChildReply(message->GetEvaluateChildReply());
                break;
            }
            case ServiceMessageKind::targetRunning:
            {
                TargetRunningServiceMessage* message = static_cast<TargetRunningServiceMessage*>(serviceMessage.get());
                HandleTargetRunning();
                break;
            }
            case ServiceMessageKind::targetInput:
            {
                TargetInputServiceMessage* message = static_cast<TargetInputServiceMessage*>(serviceMessage.get());
                HandleTargetInput();
                break;
            }
            case ServiceMessageKind::targetOutput:
            {
                TargetOutputServiceMessage* message = static_cast<TargetOutputServiceMessage*>(serviceMessage.get());
                HandleTargetOutputRequest(message->GetTargetOutputRequest());
                break;
            }
            case ServiceMessageKind::debugServiceStopped:
            {
                DebugServiceStoppedServiceMessage* message = static_cast<DebugServiceStoppedServiceMessage*>(serviceMessage.get()); 
                HandleDebugServiceStopped();
                break;
            }
            case ServiceMessageKind::processTerminated:
            {
                HandleProcessTerminated();
                break;
            }
            case ServiceMessageKind::runServiceStopped:
            {
                HandleRunServiceStopped();
                break;
            }
            case ServiceMessageKind::loadEditModuleReply:
            {
                LoadEditModuleReplyServiceMessage* message = static_cast<LoadEditModuleReplyServiceMessage*>(serviceMessage.get());
                HandleLoadEditModuleReply(message->Reply());
                break;
            }
            case ServiceMessageKind::loadEditModuleError:
            {
                LoadEditModuleErrorServiceMessage* message = static_cast<LoadEditModuleErrorServiceMessage*>(serviceMessage.get());
                HandleLoadEditModuleError(message->Error());
                break;
            }
            case ServiceMessageKind::resetEditModuleCacheReply:
            {
                ResetEditModuleCacheReplyServiceMessage* message = static_cast<ResetEditModuleCacheReplyServiceMessage*>(serviceMessage.get());
                HandleResetEditModuleCacheReply(message->Reply());
                break;
            }
            case ServiceMessageKind::resetEditModuleCacheError:
            {
                ResetEditModuleCacheErrorServiceMessage* message = static_cast<ResetEditModuleCacheErrorServiceMessage*>(serviceMessage.get());
                HandleResetEditModuleCacheError(message->Error());
                break;
            }
            case ServiceMessageKind::parseSourceReply:
            {
                ParseSourceReplyServiceMessage* message = static_cast<ParseSourceReplyServiceMessage*>(serviceMessage.get());
                HandleParseSourceReply(message->Reply());
                break;
            }
            case ServiceMessageKind::parseSourceError:
            {
                ParseSourceErrorServiceMessage* message = static_cast<ParseSourceErrorServiceMessage*>(serviceMessage.get());
                HandleParseSourceError(message->Error());
                break;
            }
            case ServiceMessageKind::getCCListReply:
            {
                GetCCListReplyServiceMessage* message = static_cast<GetCCListReplyServiceMessage*>(serviceMessage.get());
                HandleGetCCListReply(message->Reply());
                break;
            }
            case ServiceMessageKind::getCCListError:
            {
                GetCCListErrorServiceMessage* message = static_cast<GetCCListErrorServiceMessage*>(serviceMessage.get());
                HandleGetCCListError(message->Error());
                break;
            }
            case ServiceMessageKind::getParamHelpListReply:
            {
                GetParamHelpListReplyServiceMessage* message = static_cast<GetParamHelpListReplyServiceMessage*>(serviceMessage.get());
                HandleGetParamHelpListReply(message->Reply());
                break;
            }
            case ServiceMessageKind::getParamHelpListError:
            {
                GetParamHelpListErrorServiceMessage* message = static_cast<GetParamHelpListErrorServiceMessage*>(serviceMessage.get());
                HandleGetParamHelpListError(message->Error());
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
        SetIDEState();
        sngcm::ast::Solution* sln = solutionData->GetSolution();
        if (sln)
        { 
            AddRecentSolution(ToUtf8(sln->Name()), sln->FilePath());
            SaveConfiguration();
            LoadEditModule();
        }
        SetState(MainWindowState::idle);
    }
    catch (const std::exception& ex)
    {
        SetState(MainWindowState::idle);
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::SetIDEState()
{
    try
    {
        for (const std::string& openFile : solutionData->OpenFiles())
        {
            AddEditor(openFile);
        }
        if (!solutionData->CurrentOpenFile().empty())
        {
            TabPage* tabPage = codeTabControl->GetTabPageByKey(solutionData->CurrentOpenFile());
            if (tabPage)
            {
                tabPage->Select();
            }
            else
            {
                AddEditor(solutionData->CurrentOpenFile());
            }
        }
        if (solutionData->CurrentCursorLine() > 0)
        {
            TabPage* tabPage = codeTabControl->GetTabPageByKey(solutionData->CurrentOpenFile());
            if (tabPage)
            {
                Editor* editor = GetEditorByTabPage(tabPage);
                if (editor)
                {
                    TextView* textView = editor->GetTextView();
                    if (textView)
                    {
                        int line = solutionData->CurrentCursorLine();
                        if (line > 0 && line <= textView->Lines().size())
                        {
                            textView->SetCaretLineCol(line, 1 + textView->LineNumberFieldLength());
                            textView->ScrollToCaret();
                            textView->EnsureLineVisible(line);
                        }
                    }
                }
            }
        }
        for (const std::string& expandedProject : solutionData->ExpandedProjects())
        {
            TreeViewNode* projectNode = solutionData->GetProjectNodeByName(expandedProject);
            if (projectNode)
            {
                projectNode->Expand();
            }
        }
        callStackOpen = solutionData->CallStackOpen();
        if (callStackOpen)
        {
            GetCallStackView();
        }
        localsViewOpen = solutionData->LocalsViewOpen();
        if (localsViewOpen)
        {
            GetLocalsView();
        }
        outputTabPage->Select();
        SetFocusToEditor();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::AddEditor(const std::string& filePath)
{
    if (!boost::filesystem::exists(filePath)) return;
    SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(filePath);
    if (data)
    {
        std::string ext = Path::GetExtension(data->fileName);
        if (ext == ".cm")
        {
            AddCmajorEditor(data->fileName, data->key, data->filePath, data->project);
        }
        else if (ext == ".xml")
        {
            AddResourceFileEditor(data->fileName, data->key, data->filePath, data->project);
        }
        else
        {
            AddTextFileEditor(data->fileName, data->key, data->filePath, data->project);
        }
    }
    else
    {
        std::string ext = Path::GetExtension(filePath);
        if (ext == ".cm")
        {
            AddCmajorEditor(Path::GetFileName(filePath), filePath, filePath, nullptr);
        }
        else if (ext == ".xml")
        {
            AddResourceFileEditor(Path::GetFileName(filePath), filePath, filePath, nullptr);
        }
        else
        {
            AddTextFileEditor(Path::GetFileName(filePath), filePath, filePath, nullptr);
        }
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
            if (debugRequest)
            {
                StartDebugging();
                PutRequest(debugRequest.release());
            }
        }
        else
        {
            debugRequest.reset();
            PutOutputServiceMessage("build unsuccessful");
            if (!buildReply.errors.empty())
            {
                ErrorView* view = GetErrorView();
                view->Clear();
                view->SetErrors(std::move(buildReply.errors));
            }
        }
    }
    else
    {
        debugRequest.reset();
        PutOutputServiceMessage("invalid build request: " + buildReply.requestErrorMessage);
    }
    SetFocusToEditor();
}

void MainWindow::HandleBuildError(const std::string& buildError)
{
    debugRequest.reset();
    StopBuilding();
    PutOutputServiceMessage("build unsuccessful");
    SetFocusToEditor();
}

void MainWindow::HandleStopBuild()
{
    StopBuilding();
    SetFocusToEditor();
}

void MainWindow::HandleStartDebugReply(const StartDebugReply& startDebugReply)
{
    UpdateCurrentDebugStrip();
    startDebugging = false;
    if (startDebugReply.success)
    {
        HandleTargetState(startDebugReply.state);
        HandleLocation(startDebugReply.location, true, false);
        startDebuggingMenuItem->SetText("Continue");
        startDebuggingToolButton->SetToolTip("Continue (F5)");
        PutOutputServiceMessage("debugging started");
        if (localsView)
        {
            ClearLocals();
            UpdateLocals();
        }
    }
    else
    {
        PutOutputServiceMessage("start debugging request failed: " + startDebugReply.error);
    }
}

void MainWindow::HandleStartDebugError(const std::string& error)
{
    UpdateCurrentDebugStrip();
    startDebugging = false;
    PutRequest(new StopDebugServiceRequest());
}

void MainWindow::HandleContinueReply(const ContinueReply& continueReply)
{
    if (continueReply.success)
    {
        HandleTargetState(continueReply.state);
        HandleLocation(continueReply.location, true, false);
        UpdateLocals();
    }
    else
    {
        PutOutputServiceMessage("continue request failed: " + continueReply.error);
    }
}

void MainWindow::HandleNextReply(const NextReply& nextReply)
{
    if (nextReply.success)
    {
        HandleTargetState(nextReply.state);
        HandleLocation(nextReply.location, true, false);
        UpdateLocals();
    }
    else
    {
        PutOutputServiceMessage("next request failed: " + nextReply.error);
    }
}

void MainWindow::HandleStepReply(const StepReply& stepReply)
{
    if (stepReply.success)
    {
        HandleTargetState(stepReply.state);
        HandleLocation(stepReply.location, true, false);
        UpdateLocals();
    }
    else
    {
        PutOutputServiceMessage("step request failed: " + stepReply.error);
    }
}

void MainWindow::HandleFinishReply(const FinishReply& finishReply)
{
    if (finishReply.success)
    {
        HandleTargetState(finishReply.state);
        HandleLocation(finishReply.location, true, false);
        UpdateLocals();
    }
    else
    {
        PutOutputServiceMessage("finish request failed: " + finishReply.error);
    }
}

void MainWindow::HandleUntilReply(const UntilReply& untilReply)
{
    if (untilReply.success)
    {
        HandleTargetState(untilReply.state);
        HandleLocation(untilReply.location, true, false);
        UpdateLocals();
    }
    else
    {
        PutOutputServiceMessage("until request failed: " + untilReply.error);
    }
}

void MainWindow::HandleBreakReply(const BreakReply& breakReply)
{
    UpdateCurrentDebugStrip();
    if (breakReply.breakpointInfo.success)
    {
        PutOutputServiceMessage("breakpoint added");
    }
    else
    {
        PutOutputServiceMessage("break request failed: " + breakReply.breakpointInfo.error);
    }
}

void MainWindow::HandleDeleteReply(const DeleteReply& deleteReply)
{
    UpdateCurrentDebugStrip();
    if (deleteReply.success)
    {
        PutOutputServiceMessage("breakpoint removed");
    }
    else
    {
        PutOutputServiceMessage("delete request failed: " + deleteReply.error);
    }
}

void MainWindow::HandleDepthReply(const DepthReply& depthReply)
{
    if (depthReply.success)
    {
        callStackDepth = depthReply.depth;
        UpdateCallStack();
    }
    else
    {
        PutOutputServiceMessage("depth request failed: " + depthReply.error);
    }
}

void MainWindow::HandleFramesReply(const FramesReply& framesReply)
{
    if (framesReply.success)
    {
        GetCallStackView()->SetFrameRange(framesReply.frames);
    }
    else
    {
        PutOutputServiceMessage("frames request failed: " + framesReply.error);
    }
    if (signalReceived)
    {
        GetOutputLogView()->Select();
    }
}

void MainWindow::HandleEvaluateReply(const EvaluateReply& evaluateReply, int requestId)
{
    if (evaluateReply.success)
    {
        if (requestId >= 0 && requestId < expressionEvaluateRequests.size())
        {
            const ExpressionEvaluateRequest& evaluateRequest = expressionEvaluateRequests[requestId];
            toolTipWindow->Hide();
            toolTipWindow->SetText(evaluateRequest.expression + " = " + evaluateReply.result.value);
            toolTipWindow->MeasureExtent();
            Point loc = evaluateRequest.screenLoc;
            loc = ScreenToClient(loc);
            loc.Y = loc.Y + toolTipWindow->GetSize().Height;
            toolTipWindow->SetLocation(loc);
            toolTipWindow->BringToFront();
            toolTipWindow->Show();
            toolTipWindow->Invalidate();
            toolTipWindow->Update();
            SetTimer(toolTipTimerId, toolTipShowPeriod);
        }
        else
        {
            PutOutputServiceMessage("invalid evaluate request id");
        }
    }
    else
    {
        PutOutputServiceMessage("evaluate request failed: " + evaluateReply.error);
    }
}

void MainWindow::HandleCountReply(const CountReply& countReply)
{
    if (countReply.success)
    {
        LocalsView* view = GetLocalsView();
        if (view)
        {
            view->SetLocalCount(countReply.count);
            UpdateLocals();
        }
    }
    else
    {
        PutOutputServiceMessage("count request failed: " + countReply.error);
    }
}

void MainWindow::HandleEvaluateChildReply(const EvaluateChildReply& evaluateChildReply)
{
    if (evaluateChildReply.success)
    {
        LocalsView* view = GetLocalsView();
        if (view)
        {
            view->SetChildResults(evaluateChildReply.results);
            UpdateLocals();
        }
    }
    else
    {
        PutOutputServiceMessage("evaluate child request failed: " + evaluateChildReply.error);
    }
}

void MainWindow::HandleLocation(const ::Location& location, bool saveLocation, bool setSelection)
{
    try
    {
        if (location.line > 0)
        {
            lineStatusBarItem->SetText(std::to_string(location.line));
        }
        else
        {
            lineStatusBarItem->SetText(std::string());
        }
        columnStatusBarItem->SetText(std::string());
        if (saveLocation)
        {
            savedLocation = location;
        }
        if (location.file.empty()) return;
        Editor* editor = nullptr;
        const std::string& filePath = location.file;
        TabPage* tabPage = codeTabControl->GetTabPageByKey(filePath);
        if (tabPage)
        {
            tabPage->Select();
            editor = GetEditorByTabPage(tabPage);
        }
        else
        {
            SolutionTreeViewNodeData* data = solutionData->GetSolutionTreeViewNodeDataByKey(filePath);
            if (data)
            {
                editor = AddCmajorEditor(data->fileName, data->key, data->filePath, data->project);
            }
        }
        if (!editor)
        {
            editor = AddCmajorEditor(Path::GetFileName(filePath), filePath, filePath, nullptr);
        }
        if (editor->IsCmajorEditor())
        {
            CmajorEditor* cmajorEditor = static_cast<CmajorEditor*>(editor);
            DebugStrip* debugStrip = cmajorEditor->GetDebugStrip();
            SourceSpan debugLocation;
            debugLocation.line = location.line;
            debugLocation.scol = location.scol;
            debugLocation.ecol = location.ecol;
            if (!setSelection)
            {
                debugStrip->SetDebugLocation(debugLocation);
            }
        }
        TextView* textView = editor->GetTextView();
        if (textView)
        {
            textView->EnsureLineVisible(location.line);
            textView->SetCaretLineCol(location.line, location.scol);
            if (setSelection)
            {
                ResetSelections();
                SourcePos start(location.line, location.scol);
                SourcePos end(location.line, location.ecol);
                Selection selection;
                selection.start = start;
                selection.end = end;
                textView->SetSelection(selection);
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::HandleTargetState(TargetState state)
{
    programRunning = false;
    SetState(MainWindowState::debugging);
    if (state.stopReason == "exited-normally")
    {
        PutRequest(new StopDebugServiceRequest());
        PutOutputServiceMessage("program exited normally");
    }
    else if (state.stopReason == "exited")
    {
        std::string message = "program exited";
        message.append(", exit code=" + state.exitCode);
        PutRequest(new StopDebugServiceRequest());
        PutOutputServiceMessage(message);
    }
    else if (state.stopReason == "signal-received")
    {
        std::string message = "program received signal";
        message.append(", signal=" + state.signalName + ", meaning=" + state.signalMeaning);
        signalReceived = true;
        PutOutputServiceMessage(message);
        callStackDepth = -1;
        GetCallStackView();
        UpdateCallStack();
    }
    else if (state.stopReason == "breakpoint-hit")
    {
        UpdateCallStack();
        if (!state.breakpointId.empty())
        {
            std::string message = "breakpoint hit";
            message.append(", breakpoint=" + state.breakpointId);
            PutOutputServiceMessage(message);
        }
    }
    else if (state.stopReason == "end-stepping-range")
    {
        UpdateCallStack();
    }
}

void MainWindow::HandleTargetRunning()
{
    ResetDebugLocations();
    programRunning = true;
    SetState(MainWindowState::debugging);
}

void MainWindow::HandleTargetInput()
{
    GetConsole()->StartReadLine();
}

void MainWindow::HandleTargetOutputRequest(const TargetOutputRequest& targetOutputRequest)
{
    GetConsole()->Write(targetOutputRequest.handle, targetOutputRequest.output);
}

void MainWindow::ConsoleInputReady()
{
    Console* console = GetConsole();
    if (console->Eof())
    {
        if (state == MainWindowState::debugging)
        {
            PutRequest(new SetTargetInputEofRequest());
        }
        else if (state == MainWindowState::running)
        {
            PutRequest(new SetProgramEofRequest());
        }
    }
    else
    {
        if (state == MainWindowState::debugging)
        {
            PutRequest(new PutTargetInputLineRequest(ToUtf8(console->InputLine())));
        }
        else if (state == MainWindowState::running)
        {
            PutRequest(new PutProgramInputLineRequest(ToUtf8(console->InputLine())));
        }
    }
}

void MainWindow::HandleDebugServiceStopped()
{
    StopDebugging();
    SetFocusToEditor();
}

void MainWindow::HandleProcessTerminated()
{
    PutRequest(new StopRunServiceRequest());
}

void MainWindow::HandleRunServiceStopped()
{
    StopRunning();
    SetFocusToEditor();
}

void MainWindow::HandleLoadEditModuleReply(const LoadEditModuleReply& loadEditModuleReply)
{
    try
    {
        if (loadEditModuleReply.result == "error")
        {
            PutOutputServiceMessage("edit module loading error: " + loadEditModuleReply.error);
            ccState = CCState::error;
            if (ccTimerRunning)
            {
                KillTimer(ccTimerId);
            }
        }
        else
        {
            PutOutputServiceMessage("edit module " + loadEditModuleReply.key + " " + loadEditModuleReply.result);
            ccState = CCState::editModuleLoaded;
            const Options& options = GetOptions();
            if (options.codeCompletion)
            {
                ccTimerRunning = true;
                SetTimer(ccTimerId, options.parsingFrequency);
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::HandleLoadEditModuleError(const std::string& error)
{
    ccState = CCState::error;
}

void MainWindow::HandleResetEditModuleCacheReply(const ResetEditModuleCacheReply& resetEditModuleCacherReply)
{
    if (resetEditModuleCacherReply.result == "error")
    {
        PutOutputServiceMessage("error resetting edit module cache: " + resetEditModuleCacherReply.error);
    }
    else
    {
        PutOutputServiceMessage("edit module cache reset");
    }
}

void MainWindow::HandleResetEditModuleCacheError(const std::string& error)
{
    PutOutputServiceMessage("error resetting edit module cache: " + error);
}

void MainWindow::HandleParseSourceReply(const ParseSourceReply& parseSourceReply)
{
    if (parseSourceReply.ok)
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            textView->SetCCCat(std::string());
            ruleContext = parseSourceReply.ruleContext;
        }
    }
    else
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            textView->ResetCCActive();
            textView->ResetCCOpen();
        }
        codeCompletionListView->Hide();
    }
}

void MainWindow::HandleParseSourceError(const std::string& error)
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        TextView* textView = editor->GetTextView();
        textView->ResetCCActive();
        textView->ResetCCOpen();
    }
    codeCompletionListView->Hide();
}

void MainWindow::HandleGetCCListReply(const GetCCListReply& getCCListReply)
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            if (getCCListReply.ok)
            {
                std::vector<CCEntry> entries = ParseCCList(getCCListReply.ccList);
                if (entries.empty())
                {
                    codeCompletionListView->Hide();
                    ccCanSelect = false;
                }
                else
                {
                    Point ccPos = textView->CCPos();
                    Point screenPos = textView->ClientToScreen(ccPos);
                    Point ccListPos = ScreenToClient(screenPos);
                    codeCompletionListView->SetLocation(ccListPos);
                    codeCompletionListView->SetContent(entries);
                    codeCompletionListView->Show();
                    codeCompletionListView->BringToFront();
                    codeCompletionListView->Invalidate();
                    codeCompletionListView->Update();
                    codeCompletionListView->SetDefaultSize();
                    textView->SetCCOpen();
                    ccCanSelect = true;
                }
            }
            else
            {
                ccCanSelect = false;
                PutOutputServiceMessage("code completion error: " + getCCListReply.error);
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::HandleGetCCListError(const std::string& error)
{
    PutOutputServiceMessage("code completion error: " + error);
}

void MainWindow::HandleGetParamHelpListReply(const GetParamHelpListReply& getParamHelpListReply)
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            if (getParamHelpListReply.ok)
            {
                std::vector<std::string> paramHelpList = ParseParamHelpList(getParamHelpListReply.list);
                if (paramHelpList.empty())
                {
                    paramHelpView->Hide();
                }
                else
                {
                    Point ccPos = textView->CCPos();
                    Point screenPos = textView->ClientToScreen(ccPos);
                    Point paramHelpListPos = ScreenToClient(screenPos);
                    paramHelpView->SetLocation(paramHelpListPos);
                    paramHelpView->SetContent(paramHelpList);
                    paramHelpView->Show();
                    paramHelpView->BringToFront();
                    paramHelpView->Invalidate();
                    paramHelpView->Update();
                    textView->SetParamHelpOpen();
                }
            }
            else
            {
                PutOutputServiceMessage("code completion error: " + getParamHelpListReply.error);
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::HandleGetParamHelpListError(const std::string& error)
{
    PutOutputServiceMessage("code completion error: " + error);
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
    sngcm::ast::Project* activeProject = nullptr;
    if (solutionOpen)
    {
        activeProject = solutionData->GetSolution()->ActiveProject();
    }

    // always on:

    exitMenuItem->Enable();
    optionsMenuItem->Enable();
    searchResultsMenuItem->Enable();
    callStackMenuItem->Enable();
    localsMenuItem->Enable();
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
                startWithoutDebuggingMenuItem->Enable();
                if (activeProject && IsProgramTarget(activeProject->GetTarget()))
                {
                    programArgumentsMenuItem->Enable();
                }
                saveAllToolButton->Enable();
                cppToolButton->Enable();
                llvmToolButton->Enable();
                debugToolButton->Enable();
                releaseToolButton->Enable();
                buildSolutionToolButton->Enable();
                buildActiveProjectToolButton->Enable();
                toggleBreakpointMenuItem->Enable();
                if (backend == "cpp" && config == "debug")
                {
                    startDebuggingMenuItem->Enable();
                    startDebuggingToolButton->Enable();
                    stepOverMenuItem->Enable();
                    stepOverToolButton->Enable();
                    stepIntoMenuItem->Enable();
                    stepIntoToolButton->Enable();
                }
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
        case MainWindowState::debugging:
        {
            stopDebuggingMenuItem->Enable();
            stopDebuggingToolButton->Enable();
            if (programRunning || startDebugging)
            {
                startDebuggingMenuItem->Disable();
                startDebuggingToolButton->Disable();
                stepOverMenuItem->Disable();
                stepOverToolButton->Disable();
                stepIntoMenuItem->Disable();
                stepIntoToolButton->Disable();
                stepOutMenuItem->Disable();
                stepOutToolButton->Disable();
                toggleBreakpointMenuItem->Disable();
            }
            else
            {
                startDebuggingMenuItem->Enable();
                startDebuggingToolButton->Enable();
                stepOverMenuItem->Enable();
                stepOverToolButton->Enable();
                stepIntoMenuItem->Enable();
                stepIntoToolButton->Enable();
                stepOutMenuItem->Enable();
                stepOutToolButton->Enable();
                toggleBreakpointMenuItem->Enable();
            }
            if (!savedLocation.file.empty())
            {
                showNextStatementMenuItem->Enable();
                showNextStatementToolButton->Enable();
            }
            else
            {
                showNextStatementMenuItem->Disable();
                showNextStatementToolButton->Disable();
            }
            break;
        }
        case MainWindowState::running:
        {
            terminateProcessMenuItem->Enable();
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
                if (textView->CaretLine() > 0)
                {
                    lineStatusBarItem->SetText(std::to_string(textView->CaretLine()));
                }
                else
                {
                    lineStatusBarItem->SetText(std::string());
                }
                if (textView->CaretColumn() > 0)
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

void MainWindow::ResetDebugLocations()
{
    Component* child = codeTabControl->TabPages().FirstChild();
    while (child)
    {
        if (child->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(child);
            Editor* editor = GetEditorByTabPage(tabPage);
            if (editor->IsCmajorEditor())
            {
                CmajorEditor* cmajorEditor = static_cast<CmajorEditor*>(editor);
                DebugStrip* debugStrip = cmajorEditor->GetDebugStrip();
                if (debugStrip)
                {
                    debugStrip->ResetDebugLocation();
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
            if (textView->CaretLine() > 0)
            {
                lineStatusBarItem->SetText(std::to_string(textView->CaretLine()));
            }
            else
            {
                lineStatusBarItem->SetText(std::string());
            }
            if (textView->CaretColumn() > 0)
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
        clickActions.clear();
        Control* control = args.control;
        if (control->IsTextView())
        {
            TextView* textView = static_cast<TextView*>(control);
            std::unique_ptr<ContextMenu> contextMenu(new ContextMenu());
            if (state == MainWindowState::idle)
            {
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
                    clickActions.push_back(std::unique_ptr<ClickAction>(new GotoDefinitionAction(gotoDefinitionMenuItem.get(), this, project, identifier, sourceLocation)));
                    contextMenu->AddMenuItem(gotoDefinitionMenuItem.release());
                }
            }
            else if (state == MainWindowState::debugging)
            {
                SourceLoc sourceLocation;
                sourceLocation.path = textView->FilePath();
                sourceLocation.line = textView->CaretLine();
                std::unique_ptr<MenuItem> gotoCursorMenuItem(new MenuItem("Go To Cursor"));
                clickActions.push_back(std::unique_ptr<ClickAction>(new GotoCursorAction(gotoCursorMenuItem.get(), this, sourceLocation)));
                contextMenu->AddMenuItem(gotoCursorMenuItem.release());
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

void MainWindow::GotoCursor(const SourceLoc& sourceLocation)
{
    PutRequest(new UntilDebugServiceRequest(sourceLocation));
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
            selection.start.col = scol;
            selection.end.line = line;
            selection.end.col = ecol;
            textView->EnsureLineVisible(line);
            textView->SetSelection(selection);
            textView->SetCaretLineCol(line, scol);
        }
    }
}

void MainWindow::SetProjectReferences(sngcm::ast::Project* project)
{
    try
    {
        if (!solutionData) return;
        Solution* solution = solutionData->GetSolution();
        ProjectReferencesDialog dialog(project, solution);
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            std::vector<Project*> referencedProjects = dialog.ReferencedProjects();
            project->SetReferencedProjects(referencedProjects);
            project->Save();
            LoadEditModule(project);
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
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

void MainWindow::ChangeBreakpoints(CancelArgs& args)
{
    try
    {
        std::string requestName;
        if (DebugRequestInProgress(requestName))
        {
            throw std::runtime_error("cannot change breakpoints while debug request is running (request=" + requestName + ")");
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::BreakpointAdded(AddBreakpointEventArgs& args)
{
    try
    {
        if (state != MainWindowState::debugging)
        {
            args.breakpoint->info.success = true;
            return;
        }
        PutRequest(new BreakDebugServiceRequest(args.breakpoint));
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::BreakpointRemoved(RemoveBreakpointEventArgs& args)
{
    try
    {
        if (state != MainWindowState::debugging)
        {
            return;
        }
        PutRequest(new DeleteDebugServiceRequest(args.breakpointId));
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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
            solutionTreeView->SetRoot(nullptr);
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
                solutionTreeView->SetRoot(nullptr);
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
        ccState = CCState::idle;
        callStackOpen = callStackView != nullptr;
        localsViewOpen = localsView != nullptr;
        SaveSolutionData();
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
        if (callStackTabPage)
        {
            callStackTabPage->Close();
        }
        if (localsTabPage)
        {
            localsTabPage->Close();
        }
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
        SaveSolutionData();
        SaveProjectData();
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
                if (textView->IsReadOnly()) return;
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
                if (textView->IsReadOnly()) return;
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
                    if (textView->IsReadOnly()) return;
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
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            if (textView)
            {
                if (textView->IsReadOnly()) return;
                textView->Undo();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::RedoClick()
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            TextView* textView = editor->GetTextView();
            if (textView)
            {
                if (textView->IsReadOnly()) return;
                textView->Redo();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::GotoClick()
{
    try
    {
        GotoLineDialog dialog;
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            Editor* editor = CurrentEditor();
            if (editor)
            {
                TextView* textView = editor->GetTextView();
                if (textView)
                {
                    int lineNumber = dialog.LineNumber();
                    textView->SetCaretLineCol(std::min(lineNumber, static_cast<int>(textView->Lines().size())), 1 + textView->LineNumberFieldLength());
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
    try
    {
        const Options& prevOptions = GetOptions();
        bool prevCodeCompletion = prevOptions.codeCompletion;
        OptionsDialog dialog; 
        dialog.SetOptionsFrom(GetOptions());
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            const Options& options = dialog.GetOptions();
            SetOptions(options);
            SaveConfiguration();
            if (prevCodeCompletion != options.codeCompletion)
            {
                if (options.codeCompletion)
                {
                    codeCompletionStatusBarItem->SetText("On");
                    StartCodeCompletion();
                }
                else
                {
                    codeCompletionStatusBarItem->SetText("Off");
                    StopCodeCompletion(true);
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::CallStackClick()
{
    try
    {
        ClearCallStack();
        GetCallStackView()->Invalidate();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::LocalsClick()
{
    try
    {
        GetLocalsView();
        UpdateLocals();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ErrorsClick()
{
    try
    {
        ErrorView* view = GetErrorView();
        if (view)
        {
            view->Invalidate();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::SearchResultsClick()
{
    try
    {
        SearchResultsView* view = GetSearchResultsView();
        if (view)
        {
            view->Invalidate();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::PortMapClick()
{
    try
    {
        Process* process = new Process("cmpmv", Process::Redirections::none);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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
            solutionTreeView->SetRoot(nullptr);
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
            solutionTreeView->SetRoot(nullptr);
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
        sngcm::ast::Solution* solution = solutionData->GetSolution();
        solution->SetActiveProject(project);
        solution->Save();
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
        std::string projectName = ToUtf8(project->Name());
        solutionData->RemoveProjectNode(projectName);
        solutionData->RemoveExpandedProject(projectName);
        sngcm::ast::Solution* solution = solutionData->GetSolution();
        solution->RemoveProject(project);
        solution->Save();
        std::string solutionFilePath = solution->FilePath();
        if (!CloseSolution())
        {
            return;
        }
        solutionTreeView->SetRoot(nullptr);
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
            LoadEditModule(project);
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
            LoadEditModule(project);
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
        solutionData->RemoveOpenFile(filePath);
        if (solutionData->CurrentOpenFile() == filePath)
        {
            solutionData->SetCurrentOpenFile(std::string());
        }
        project->RemoveFile(filePath, fileName);
        project->Save();
        if (EndsWith(filePath, ".cm"))
        {
            LoadEditModule(project);
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

bool MainWindow::BuildActiveProject()
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
        if (!IsProgramTarget(activeProject->GetTarget()))
        {
            throw std::runtime_error("active project is a library project");
        }
        StartBuilding();
        StartBuild(backend, config, activeProject->FilePath(), BuildRequestKind::build | BuildRequestKind::buildDependencies);
        return true;
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
    return false;
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

void MainWindow::BuildSettingsClick()
{
    try
    {
        if (state != MainWindowState::idle)
        {
            throw std::runtime_error("wrong state");
        }
        BuildSettingsDialog dialog;
        dialog.SetValuesFrom(GetBuildSettings());
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            SetBuildSettings(dialog.GetValues());
            SaveConfiguration();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), std::string(ex.what()));
    }
}

void MainWindow::StartDebuggingClick()
{
    try
    {
        ClearCallStack();
        ClearLocals();
        ResetSelections();
        if (state == MainWindowState::debugging)
        {
            StartDebugging();
            PutRequest(new ContinueDebugServiceRequest());
        }
        else
        {
            debugRequest.reset(new ContinueDebugServiceRequest());
            if (!BuildActiveProject())
            {
                debugRequest.reset();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::StartWithoutDebuggingClick()
{
    try
    {
        StartRunning();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::TerminateProcessClick()
{
    try
    {
        PutRequest(new TerminateProcessRequest());
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::StopDebuggingClick()
{
    if (state != MainWindowState::debugging) return;
    PutRequest(new StopDebugServiceRequest());
}

void MainWindow::ShowNextStatementClick()
{
    try
    {
        HandleLocation(savedLocation, false, false);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::StepOverClick()
{
    try
    {
        ClearCallStack();
        ClearLocals();
        ResetSelections();
        if (state == MainWindowState::debugging)
        {
            StartDebugging();
            PutRequest(new NextDebugServiceRequest());
        }
        else
        {
            debugRequest.reset(new NextDebugServiceRequest());
            if (!BuildActiveProject())
            {
                debugRequest.reset();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::StepIntoClick()
{
    try
    {
        ClearCallStack();
        ClearLocals();
        ResetSelections();
        if (state == MainWindowState::debugging)
        {
            StartDebugging();
            PutRequest(new StepDebugServiceRequest());
        }
        else
        {
            debugRequest.reset(new StepDebugServiceRequest());
            if (!BuildActiveProject())
            {
                debugRequest.reset();
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::StepOutClick()
{
    try
    {
        ClearCallStack();
        ClearLocals();
        ResetSelections();
        StartDebugging();
        PutRequest(new FinishDebugServiceRequest());
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ToggleBreakpointClick()
{
    try
    {
        Editor* editor = CurrentEditor();
        if (editor)
        {
            if (editor->IsCmajorEditor())
            {
                CmajorEditor* cmajorEditor = static_cast<CmajorEditor*>(editor);
                CmajorSourceCodeView* cmajorSourceCodeView = cmajorEditor->SourceCodeView();
                if (cmajorSourceCodeView)
                {
                    cmajorSourceCodeView->ToggleBreakpoint();
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::ProgramArgumentsClick()
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
        if (!IsProgramTarget(activeProject->GetTarget()))
        {
            throw std::runtime_error("active project is a library project");
        }
        ProjectData* projectData = solutionData->GetProjectDataByProject(activeProject);
        if (!projectData)
        { 
            throw std::runtime_error("active project has no data");
        }
        ProgramArgumentsDialog dialog;
        dialog.SetProgramArguments(projectData->ProgramArguments());
        if (dialog.ShowDialog(*this) == DialogResult::ok)
        {
            projectData->SetProgramArguments(dialog.ProgramArguments());
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
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
    try
    {
        std::string homePage = "http://slaakko.github.io/cmajor/";
        ShellExecuteA(Handle(), "open", homePage.c_str(), nullptr, nullptr, SW_SHOW);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::LocalDocumentationClick()
{
    try
    {
        std::string indexFilePath = Path::Combine(sngcm::ast::CmajorRootDir(), "doc/index.html");
        ShellExecuteA(Handle(), "open", indexFilePath.c_str(), nullptr, nullptr, SW_SHOW);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
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
    SetState(state);
    LoadEditModuleForCurrentFile();
}

void MainWindow::LlvmButtonClick()
{
    backend = "llvm";
    cppToolButton->SetState(ToolButtonState::normal);
    SetState(state);
    LoadEditModuleForCurrentFile();
}

void MainWindow::DebugButtonClick()
{
    config = "debug";
    releaseToolButton->SetState(ToolButtonState::normal);
    SetState(state);
    LoadEditModuleForCurrentFile();
}

void MainWindow::ReleaseButtonClick()
{
    config = "release";
    debugToolButton->SetState(ToolButtonState::normal);
    SetState(state);
    LoadEditModuleForCurrentFile();
}

void MainWindow::StopBuildServerClick()
{
    PutRequest(new cmajor::service::StopBuildRequest());
}

void MainWindow::ToggleCodeCompletionClick()
{
    const Options& oldOptions = GetOptions();
    Options newOptions(oldOptions);
    if (oldOptions.codeCompletion)
    {
        newOptions.codeCompletion = false;
        StopCodeCompletion(true);
        codeCompletionStatusBarItem->SetText("Off");
    }
    else
    {
        newOptions.codeCompletion = true;
        StartCodeCompletion();
        LoadEditModule();
        codeCompletionStatusBarItem->SetText("On");
    }
    SetOptions(newOptions);
    SaveConfiguration();
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
                    std::unique_ptr<MenuItem> referencesMenuItem(new MenuItem("Rererences..."));
                    clickActions.push_back(std::unique_ptr<ClickAction>(new ProjectReferencesAction(referencesMenuItem.get(), this, project)));
                    contextMenu->AddMenuItem(referencesMenuItem.release());
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

void MainWindow::TreeViewNodeExpanded(TreeViewNodeEventArgs& args)
{
    try
    {
        TreeViewNode* node = args.node;
        SolutionTreeViewNodeData* data = static_cast<SolutionTreeViewNodeData*>(node->Data());
        if (data->kind == SolutionTreeViewNodeDataKind::project)
        {
            solutionData->AddExpandedProject(ToUtf8(data->project->Name()));
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::TreeViewNodeCollapsed(TreeViewNodeEventArgs& args)
{
    try
    {
        TreeViewNode* node = args.node;
        SolutionTreeViewNodeData* data = static_cast<SolutionTreeViewNodeData*>(node->Data());
        if (data->kind == SolutionTreeViewNodeDataKind::project)
        {
            solutionData->RemoveExpandedProject(ToUtf8(data->project->Name()));
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
        sourceCodeView->ExpressionHover().AddHandler(this, &MainWindow::ExpressionHover);
        sourceCodeView->SetUndoRedoMenuItems(undoMenuItem, redoMenuItem);
        sourceCodeView->CCTextChanged().AddHandler(this, &MainWindow::CCTextChanged);
        sourceCodeView->CC().AddHandler(this, &MainWindow::CCShow);
        sourceCodeView->EscapePressed().AddHandler(this, &MainWindow::CCEscape);
        sourceCodeView->CCNext().AddHandler(this, &MainWindow::CCNext);
        sourceCodeView->CCPrev().AddHandler(this, &MainWindow::CCPrev);
        sourceCodeView->CCNextPage().AddHandler(this, &MainWindow::CCNextPage);
        sourceCodeView->CCPrevPage().AddHandler(this, &MainWindow::CCPrevPage);
        sourceCodeView->CCSelect().AddHandler(this, &MainWindow::CCSelect);
        sourceCodeView->CCStart().AddHandler(this, &MainWindow::CCStart);
        sourceCodeView->CCStop().AddHandler(this, &MainWindow::CCStop);
        sourceCodeView->ParamHelpNext().AddHandler(this, &MainWindow::ParamHelpNext);
        sourceCodeView->ParamHelpPrev().AddHandler(this, &MainWindow::ParamHelpPrev);
        sourceCodeView->ParamHelpClose().AddHandler(this, &MainWindow::ParamHelpClose);
        sourceCodeView->ContentLocationChanged().AddHandler(this, &MainWindow::CCViewLocationChanged);
        if (state != MainWindowState::idle)
        {
            sourceCodeView->SetReadOnly();
        }
        DebugStrip* debugStrip = sourceCodeView->GetDebugStrip();
        if (debugStrip)
        {
            debugStrip->ChangeBreakpoints().AddHandler(this, &MainWindow::ChangeBreakpoints);
            debugStrip->BreakpointAdded().AddHandler(this, &MainWindow::BreakpointAdded);
            debugStrip->BreakpointRemoved().AddHandler(this, &MainWindow::BreakpointRemoved);
        }
    }
    tabPage->AddChild(editorPtr.release());
    tabPageEditorMap[tabPage.get()] = editor;
    codeTabControl->AddTabPage(tabPage.release());
    SetEditorState();
    SetFocusToEditor();
    solutionData->AddOpenFile(filePath);
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
        textView->SetUndoRedoMenuItems(undoMenuItem, redoMenuItem);
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
    solutionData->AddOpenFile(filePath);
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
        textView->SetUndoRedoMenuItems(undoMenuItem, redoMenuItem);
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
    solutionData->AddOpenFile(filePath);
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
        LoadEditModuleForCurrentFile();
    }
}

void MainWindow::CodeTabPageRemoved(ControlEventArgs& args)
{
    SetEditorState();
    CCHide();
    ParamHelpClose();
    Control* removedControl = args.control;
    if (removedControl->IsTabPage())
    {
        TabPage* removedTabPage = static_cast<TabPage*>(removedControl);
        Editor* editor = GetEditorByTabPage(removedTabPage);
        if (editor)
        {
            if (editor->IsDirty())
            {
                MessageBoxResult result = MessageBox::Show(editor->FilePath() + " is modified. Save changes?", "Question", this, MB_YESNO);
                if (result == MessageBoxResult::yes)
                {
                    editor->Save();
                }
            }
            solutionData->RemoveOpenFile(editor->FilePath());
            if (editor->FilePath() == solutionData->CurrentOpenFile())
            {
                solutionData->SetCurrentOpenFile(std::string());
            }
        }
        tabPageEditorMap.erase(removedTabPage);
        if (removedTabPage == debugTabPage)
        {
            debugTabPage = nullptr;
        }
    }
    if (codeTabControl->TabPages().IsEmpty())
    {
        sourceFilePathStatusBarItem->SetText("");
        ResetFocusedControl();
    }
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
    else if (args.control == callStackTabPage)
    {
        callStackTabPage = nullptr;
        callStackView = nullptr;
    }
    else if (args.control == localsTabPage)
    {
        localsTabPage = nullptr;
        localsView = nullptr;
    }
    else if (args.control == logTabPage)
    {
        logTabPage = nullptr;
        log = nullptr;
    }
    else if (args.control == consoleTabPage)
    {
        consoleTabPage = nullptr;
        console = nullptr;
    }
}

void MainWindow::OutputTabControlTabPageSelected()
{
    // todo?
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

CallStackView* MainWindow::GetCallStackView()
{
    if (!callStackView)
    {
        callStackView = new CallStackView();
        callStackView->FrameSelected().AddHandler(this, &MainWindow::CallStackFrameSelected);
        if (state == MainWindowState::debugging)
        {
            UpdateCallStack();
        }
        callStackTabPage = new TabPage("Call Stack", "callStack");
        callStackTabPage->AddChild(callStackView);
        outputTabControl->AddTabPage(callStackTabPage);
    }
    callStackTabPage->Select();
    callStackOpen = true;
    return callStackView;
}

void MainWindow::ClearCallStack()
{
    if (signalReceived) return;
    callStackDepth = -1;
    if (callStackView)
    {
        callStackView->Clear();
    }
}

void MainWindow::UpdateCallStack()
{
    if (!callStackView) return;
    if (callStackDepth == -1)
    {
        PutRequest(new DepthDebugServiceRequest());
    }
    else if (callStackDepth >= 0)
    {
        callStackView->SetDepth(callStackDepth);
        std::pair<int, int> frameRange = callStackView->GetFrameRange();
        if (frameRange.first != -1 && frameRange.second != -1)
        {
            PutRequest(new FramesDebugServiceRequest(frameRange.first, frameRange.second));
        }
    }
}

void MainWindow::CallStackFrameSelected(FrameSelectedEventArgs& args)
{
    try
    {
        HandleLocation(*args.frame, false, true);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

LocalsView* MainWindow::GetLocalsView()
{
    if (!localsView)
    {
        localsView = new LocalsView();
        localsView->LocalNodeExpanded().AddHandler(this, &MainWindow::LocalsViewNodeExpanded);
        localsView->UpdateNeeded().AddHandler(this, &MainWindow::LocalsViewUpdateNeeded);
        localsTabPage = new TabPage("Locals", "locals");
        localsTabPage->AddChild(localsView);
        outputTabControl->AddTabPage(localsTabPage);
    }
    localsTabPage->Select();
    localsViewOpen = true;
    return localsView;
}

void MainWindow::ClearLocals()
{
    if (localsView)
    {
        localsView->Clear();
    }
}

void MainWindow::UpdateLocals()
{
    if (!localsView) return;
    int localCount = localsView->LocalCount();
    if (localCount == -1)
    {
        if (!localsView->LocalCountRequested())
        {
            localsView->SetLocalCountRequested();
            PutRequest(new CountDebugServiceRequest("@locals"));
        }
    }
    else 
    {
        if (!localsView->ChildExtentRequested())
        {
            ChildExtent childExtent = localsView->GetChildExtent();
            if (!childExtent.IsEmpty())
            {
                localsView->SetChildExtentRequested();
                PutRequest(new EvaluateChildDebugServiceRequest(localsView->FetchExpression(), childExtent.start, childExtent.count));
            }
        }
        else
        {
            localsView->ResetChildExtentRequested();
            if (!localsView->IsFetchSetEmpty())
            {
                localsView->Fetch();
                ChildExtent childExtent = localsView->GetChildExtent();
                if (!childExtent.IsEmpty())
                {
                    localsView->SetChildExtentRequested();
                    PutRequest(new EvaluateChildDebugServiceRequest(localsView->FetchExpression(), childExtent.start, childExtent.count));
                }
            }
        }
        localsView->Update();
    }
}

void MainWindow::LocalsViewNodeExpanded(TreeViewNodeEventArgs& args)
{
    try
    {
        if (!localsView) return;
        UpdateLocals();
        localsView->UpdateFetchSet();
        if (!localsView->IsFetchSetEmpty())
        {
            localsView->Fetch();
        }
        UpdateLocals();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::LocalsViewUpdateNeeded()
{
    try
    {
        if (!localsView) return;
        UpdateLocals();
        localsView->UpdateFetchSet();
        if (!localsView->IsFetchSetEmpty())
        {
            localsView->Fetch();
        }
        UpdateLocals();
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

Console* MainWindow::GetConsole()
{
    if (!console)
    {
        console = new Console(ConsoleCreateParams().Defaults());
        console->SetDoubleBuffered();
        console->ConsoleInputReady().AddHandler(this, &MainWindow::ConsoleInputReady);
        ScrollableControl* scrollableConsole = new ScrollableControl(ScrollableControlCreateParams(console).SetDock(Dock::fill));
        consoleTabPage = new TabPage("Console", "console");
        consoleTabPage->AddChild(scrollableConsole);
        outputTabControl->AddTabPage(consoleTabPage);
    }
    consoleTabPage->Select();
    return console;
}

void MainWindow::UpdateCurrentDebugStrip()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        if (editor->IsCmajorEditor())
        {
            CmajorEditor* cmajorEditor = static_cast<CmajorEditor*>(editor);
            DebugStrip* debugStrip = cmajorEditor->GetDebugStrip();
            if (debugStrip)
            {
                debugStrip->Update();
            }
        }
    }
}

void MainWindow::ResetSelections()
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
                    textView->ResetSelection();
                }
            }
        }
        child = child->NextSibling();
    }
}

void MainWindow::ExpressionHover(ExpressionHoverEventArgs& args)
{
    if (state != MainWindowState::debugging) return;
    int requestId = expressionEvaluateRequests.size();
    ExpressionEvaluateRequest request(args.expression, args.screenLoc);
    expressionEvaluateRequests.push_back(request);
    PutRequest(new EvaluateDebugServiceRequest(args.expression, requestId));
}

void MainWindow::CCEscape()
{
    CCHide();
    ParamHelpClose();
}

void MainWindow::CCTextChanged()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    Editor* editor = CurrentEditor();
    if (editor)
    {
        if (!editor->GetTextView()->IsCCActive()) return;
        if (!editModuleLoaded)
        {
            PutOutputServiceMessage("loading edit module...");
            LoadEditModule();
        }
        Project* project = CurrentProject();
        if (project)
        {
            ccCanSelect = false;
            GetCCList(project->FilePath(), backend, config, editor->FilePath(), editor->GetTextView()->GetCCText(), ruleContext, editor->GetTextView()->CursorLine());
        }
    }
}

void MainWindow::CCShow()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    Editor* editor = CurrentEditor();
    if (editor)
    {
        editor->GetTextView()->SetCCActive();
        CCTextChanged();
    }
}

void MainWindow::CCHide()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        editor->GetTextView()->ResetCCActive();
        editor->GetTextView()->ResetCCOpen();
    }
    codeCompletionListView->Hide();
}

void MainWindow::CCNext()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    codeCompletionListView->Next();
}

void MainWindow::CCPrev()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    codeCompletionListView->Prev();
}

void MainWindow::CCNextPage()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    codeCompletionListView->NextPage();
}

void MainWindow::CCPrevPage()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    codeCompletionListView->PrevPage();
}

void MainWindow::CCSelect(SelectedEventArgs& args)
{
    try
    {
        const Options& options = GetOptions();
        if (!options.codeCompletion) return;
        if (ccCanSelect)
        {
            currentCCEntry = codeCompletionListView->GetSelectedEntry();
            Editor* editor = CurrentEditor();
            if (editor)
            {
                editor->GetTextView()->ReplaceCCText(ToUtf32(currentCCEntry.replacement));
                editor->GetTextView()->ResetCCActive();
                editor->GetTextView()->ResetCCOpen();
                editor->GetTextView()->SetCCCat(currentCCEntry.category);
            }
            codeCompletionListView->Hide();
            if (currentCCEntry.category == "FN" || currentCCEntry.category == "CL" || currentCCEntry.category == "CO")
            {
                GetParamHelpList(currentCCEntry.symbolIndex);
            }
            args.selected = true;
        }
        else
        {
            Editor* editor = CurrentEditor();
            if (editor)
            {
                editor->GetTextView()->ResetCCOpen();
                editor->GetTextView()->ResetCCActive();
            }
            codeCompletionListView->Hide();
        }
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void MainWindow::CCStart()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    Editor* editor = CurrentEditor();
    if (editor)
    {
        editor->GetTextView()->SetCCActive();
    }
}

void MainWindow::CCStop()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        editor->GetTextView()->ResetCCOpen();
        editor->GetTextView()->ResetCCActive();
    }
    codeCompletionListView->Hide();
}

void MainWindow::ParamHelpNext()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    paramHelpView->Next();
}

void MainWindow::ParamHelpPrev()
{
    const Options& options = GetOptions();
    if (!options.codeCompletion) return;
    paramHelpView->Prev();
}

void MainWindow::ParamHelpClose()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        editor->GetTextView()->ResetParamHelpOpen();
    }
    paramHelpView->Hide();
}

void MainWindow::CCViewLocationChanged()
{
    Editor* editor = CurrentEditor();
    if (editor)
    {
        editor->GetTextView()->ResetCCOpen();
        editor->GetTextView()->ResetCCActive();
    }
    codeCompletionListView->Hide();
}

} // namespace cmcode
