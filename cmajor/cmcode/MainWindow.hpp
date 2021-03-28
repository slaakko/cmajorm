// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_MAIN_WINDOW_INCLUDED
#define CMCODE_MAIN_WINDOW_INCLUDED
#include <cmajor/cmcode/Solution.hpp>
#include <cmajor/cmcode/LocationList.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>
#include <cmajor/cmview/CmajorEditor.hpp>
#include <cmajor/cmview/ResourceFileEditor.hpp>
#include <cmajor/cmview/TextFileEditor.hpp>
#include <cmajor/cmview/ErrorView.hpp>
#include <cmajor/wing/Clipboard.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/SplitContainer.hpp>
#include <cmajor/wing/TreeView.hpp>
#include <cmajor/wing/TabControl.hpp>
#include <cmajor/wing/ToolBar.hpp>
#include <cmajor/wing/StatusBar.hpp>
#include <cmajor/wing/LogView.hpp>

namespace cmcode {

const int buildProgressTimerId = 10;
const int buildProgressTimerPeriod = 100;

using namespace cmajor::view;
using namespace cmajor::wing;

enum class MainWindowState : int
{
    idle, building, debugging, running
};

class MainWindow : public Window
{
public:
    MainWindow(const std::string& filePath);
    ~MainWindow();
    void BuildSolutionClick();
    void RebuildSolutionClick();
    void CleanSolutionClick();
    void BuildProject(sngcm::ast::Project* project);
    void RebuildProject(sngcm::ast::Project* project);
    void CleanProject(sngcm::ast::Project* project);
    void AddNewProject();
    void AddExistingProject();
    void SetActiveProject(sngcm::ast::Project* project, TreeViewNode* newActiveProjectNode);
    void RemoveProject(sngcm::ast::Project* project);
    void OpenFileLocation(const std::string& filePath);
    void AddNewSourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode);
    void AddExistingSourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode);
    void AddNewResourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode);
    void AddExistingResourceFile(sngcm::ast::Project* project, TreeViewNode* projectNode);
    void AddNewTextFile(sngcm::ast::Project* project, TreeViewNode* projectNode);
    void AddExistingTextFile(sngcm::ast::Project* project, TreeViewNode* projectNode);
    void RemoveFile(sngcm::ast::Project* project, const std::string& filePath, const std::string& fileName, TreeViewNode* fileNode);
    void GotoDefinition(sngcm::ast::Project* project, const std::string& identifier, const DefinitionSourceLocation& sourceLocation);
    void GotoLocation(const DefinitionSourceLocation& location);
protected:
    bool ProcessMessage(Message& msg) override;
    void OnWindowStateChanged() override;
    void OnSizeChanged() override;
    void OnWindowClosing(CancelArgs& args) override;
    void OnKeyDown(KeyEventArgs& args) override;
    void MouseUpNotification(MouseEventArgs& args) override;
    void OnTimer(TimerEventArgs& args) override;
    void OnGotFocus() override;
    void OnClipboardUpdate() override;
private:
    void SaveConfigurationSettings();
    void LoadConfigurationSettings();
    void AddClipboardListener();
    void RemoveClipboardListener();
    void StartBuilding();
    void StopBuilding();
    void ShowBuildProgress();
    void HandleServiceMessage();
    void ClearOutput();
    void WriteOutput(const std::string& text);
    void OpenProject(const std::string& filePath);
    void HandleBuildReply(BuildReply& buildReply);
    void HandleBuildError(const std::string& buildError);
    void HandleGetDefinitionReply(GetDefinitionReply& getDefinitionReply);
    void HandleGetDefinitionError(const std::string& getDefinitionError);
    void HandleStopBuild();
    void SetState(MainWindowState state_);
    void SetEditorState();
    void SetEditorsReadOnly();
    void SetEditorsReadWrite();
    void SetFocusToEditor();
    void EditorReadOnlyChanged();
    void EditorDirtyChanged();
    void EditorCCDirtyChanged();
    void EditorCaretPosChanged();
    void EditorSelectionChanged();
    void EditorRightClick(RightClickEventArgs& args);
    bool GetDefinitionSourceLocationAt(const Point& loc, TextView* textView, std::string& identifier, DefinitionSourceLocation& sourceLocation);
    sngcm::ast::Project* CurrentProject();
    DefinitionSourceLocation CurrentLocation() const;
    void BreakpointAdded(AddBreakpointEventArgs& args);
    void BreakpointRemoved(RemoveBreakpointEventArgs& args);
    void VerticalSplitContainerSplitterDistanceChanged();
    void HorizontalSplitContainerSplitterDistanceChanged();
    void NewProjectClick();
    void OpenProjectClick();
    bool CloseSolution();
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
    void CallStackClick();
    void LocalsClick();
    void ErrorsClick();
    void SearchResultsClick();
    void PortMapClick();
    void BuildActiveProjectClick();
    void RebuildActiveProjectClick();
    void CleanActiveProjectClick();
    void StartDebuggingClick();
    void StartWithoutDebuggingClick();
    void TerminateProcessClick();
    void StopDebuggingClick();
    void ShowNextStatementClick();
    void StepOverClick();
    void StepIntoClick();
    void StepOutClick();
    void ToggleBreakpointClick();
    void ProgramArgumentsClick();
    void CloseAllTabsClick();
    void CloseExternalTabsClick();
    void HomepageClick();
    void LocalDocumentationClick();
    void AboutClick();
    void GotoPreviousLocationClick();
    void GotoNextLocationClick();
    void CppButtonClick();
    void LlvmButtonClick();
    void DebugButtonClick();
    void ReleaseButtonClick();
    void StopBuildServerClick();
    void TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args);
    void TreeViewNodeClick(TreeViewNodeClickEventArgs& args);
    Editor* GetEditorByTabPage(TabPage* tabPage) const;
    CmajorEditor* AddCmajorEditor(const std::string& fileName, const std::string& key, const std::string& filePath, sngcm::ast::Project* project);
    ResourceFileEditor* AddResourceFileEditor(const std::string& fileName, const std::string& key, const std::string& filePath, sngcm::ast::Project* project);
    TextFileEditor* AddTextFileEditor(const std::string& fileName, const std::string& key, const std::string& filePath, sngcm::ast::Project* project);
    void AddFilePathsToProject(const std::string& newSourceFilePath, const std::string& newResourceFilePath, const std::string& newTextFilePath, sngcm::ast::Project* project, TreeViewNode* projectNode);
    void CodeTabPageSelected();
    void CodeTabPageRemoved(ControlEventArgs& args);
    void OutputTabControlTabPageRemoved(ControlEventArgs& args);
    void OutputTabControlTabPageSelected();
    LogView* GetOutputLogView();
    ErrorView* GetErrorView();
    void ViewError(ViewErrorArgs& args);
    Editor* CurrentEditor() const;
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
    MenuItem* callStackMenuItem;
    MenuItem* localsMenuItem;
    MenuItem* errorsMenuItem;
    MenuItem* searchResultsMenuItem;
    MenuItem* portMapMenuItem;
    MenuItem* buildSolutionMenuItem;
    MenuItem* rebuildSolutionMenuItem;
    MenuItem* cleanSolutionMenuItem;
    MenuItem* buildActiveProjectMenuItem;
    MenuItem* rebuildActiveProjectMenuItem;
    MenuItem* cleanActiveProjectMenuItem;
    MenuItem* startDebuggingMenuItem;
    MenuItem* startWithoutDebuggingMenuItem;
    MenuItem* terminateProcessMenuItem;
    MenuItem* stopDebuggingMenuItem;
    MenuItem* showNextStatementMenuItem;
    MenuItem* stepOverMenuItem;
    MenuItem* stepIntoMenuItem;
    MenuItem* stepOutMenuItem;
    MenuItem* toggleBreakpointMenuItem;
    MenuItem* programArgumentsMenuItem;
    MenuItem* closeAllTabsMenuItem;
    MenuItem* closeExternalTabsMenuItem;
    MenuItem* homepageMenuItem;
    MenuItem* localDocumentationMenuItem;
    MenuItem* aboutMenuItem;
    ToolButton* prevToolButton;
    ToolButton* nextToolButton;
    ToolButton* saveToolButton;
    ToolButton* saveAllToolButton;
    ToolButton* cppToolButton;
    ToolButton* llvmToolButton;
    ToolButton* debugToolButton;
    ToolButton* releaseToolButton;
    ToolButton* buildSolutionToolButton;
    ToolButton* buildActiveProjectToolButton;
    ToolButton* stopBuildServerToolButton;
    ToolButton* startDebuggingToolButton;
    ToolButton* stopDebuggingToolButton;
    ToolButton* showNextStatementToolButton;
    ToolButton* stepOverToolButton;
    ToolButton* stepIntoToolButton;
    ToolButton* stepOutToolButton;
    SplitContainer* verticalSplitContainer;
    SplitContainer* horizontalSplitContainer;
    TabControl* codeTabControl;
    TreeView* solutionTreeView;
    TabControl* outputTabControl;
    TabPage* outputTabPage;
    LogView* outputLogView;
    TabPage* errorTabPage;
    ErrorView* errorView;
    StatusBar* statusBar;
    StatusBarTextItem* buildIndicatorStatuBarItem;
    StatusBarTextItem* editorReadWriteIndicatorStatusBarItem;
    StatusBarTextItem* editorDirtyIndicatorStatusBarItem;
    StatusBarTextItem* sourceFilePathStatusBarItem;
    StatusBarTextItem* lineStatusBarItem;
    StatusBarTextItem* columnStatusBarItem;
    int buildProgressCounter;
    bool buildProgressTimerRunning;
    bool setMaximizedSplitterDistance;
    bool sizeChanged;
    float verticalSplitContainerFactor;
    float horizontalSplitContainerFactor;
    bool showingDialog;
    std::unique_ptr<SolutionData> solutionData;
    std::unordered_map<TabPage*, Editor*> tabPageEditorMap;
    MainWindowState state;
    std::string backend;
    std::string config;
    std::unique_ptr<ClipboardListener> clipboardListener;
    ClipboardFormat cmajorCodeFormat;
    std::u32string clipboardData;
    std::vector<std::unique_ptr<ClickAction>> clickActions;
    LocationList locations;
};

} // namespace cmcode

#endif // CMCODE_MAIN_WINDOW_INCLUDED
