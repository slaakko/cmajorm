// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_MAIN_WINDOW_INCLUDED
#define CMCODE_MAIN_WINDOW_INCLUDED
#include <cmajor/cmcode/Solution.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>
#include <cmajor/cmview/CmajorEditor.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/SplitContainer.hpp>
#include <cmajor/wing/TreeView.hpp>
#include <cmajor/wing/TabControl.hpp>
#include <cmajor/wing/ToolBar.hpp>
#include <cmajor/wing/LogView.hpp>

namespace cmcode {

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
    void SetActiveProject(sngcm::ast::Project* project, TreeViewNode* newActiveProjectNode);
    void RemoveProject(sngcm::ast::Project* project);
    void OpenFileLocation(const std::string& filePath);
protected:
    bool ProcessMessage(Message& msg) override;
    void OnWindowClosing(CancelArgs& args) override;
    void OnKeyDown(KeyEventArgs& args) override;
    void MouseUpNotification(MouseEventArgs& args) override;
private:
    void HandleServiceMessage();
    void ClearOutput();
    void WriteOutput(const std::string& text);
    void OpenProject(const std::string& filePath);
    void HandleBuildReply(const BuildReply& buildReply);
    void HandleBuildError(const std::string& buildError);
    void HandleStopBuild();
    void SetState(MainWindowState state_);
    int VerticalSplitterDistance();
    int HorizontalSplitterDistance();
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
    CmajorEditor* AddCmajorEditor(const std::string& fileName, const std::string& key, const std::string& filePath, sngcm::ast::Project* project);
    void CodeTabPageSelected();
    void CodeTabPageRemoved(ControlEventArgs& args);
    void OutputTabControlTabPageRemoved(ControlEventArgs& args);
    void OutputTabControlTabPageSelected();
    LogView* GetOutputLogView();
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
    std::unique_ptr<SolutionData> solutionData;
    std::unordered_map<TabPage*, Editor*> tabPageEditorMap;
    MainWindowState state;
    std::string backend;
    std::string config;
    std::vector<std::unique_ptr<ClickAction>> clickActions;
};

} // namespace cmcode

#endif // CMCODE_MAIN_WINDOW_INCLUDED
