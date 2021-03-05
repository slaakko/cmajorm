// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_MAIN_WINDOW_INCLUDED
#define CMCODE_MAIN_WINDOW_INCLUDED
#include <cmajor/cmcode/Solution.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/wing/Window.hpp>
#include <cmajor/wing/Wing.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/Menu.hpp>
#include <cmajor/wing/SplitContainer.hpp>
#include <cmajor/wing/TreeView.hpp>

namespace cmcode {

using namespace cmajor::wing;

enum class MainWindowState : int
{
    idle
};

class MainWindow : public Window
{
public:
    MainWindow(const std::string& filePath);
    ~MainWindow();
protected:
    bool ProcessMessage(Message& msg) override;
    void OnWindowClosing(CancelArgs& args) override;
private:
    void HandleServiceMessage();
    void ClearOutput();
    void WriteOutput(const std::string& text);
    void OpenProject(const std::string& filePath);
    void SetState(MainWindowState state_);
    int VerticalSplitterDistance();
    int HorizontalSplitterDistance();
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
    void CallStackClick();
    void LocalsClick();
    void ErrorsClick();
    void SearchResultsClick();
    void PortMapClick();
    void BuildSolutionClick();
    void RebuildSolutionClick();
    void CleanSolutionClick();
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
    SplitContainer* verticalSplitContainer;
    SplitContainer* horizontalSplitContainer;
    TreeView* solutionTreeView;
    std::unique_ptr<SolutionData> solutionData;
    MainWindowState state;
};

} // namespace cmcode

#endif // CMCODE_MAIN_WINDOW_INCLUDED