// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_MAIN_WINDOW_INCLUDED
#define CMCODE_MAIN_WINDOW_INCLUDED
#include <cmajor/cmcode/Solution.hpp>
#include <cmajor/cmcode/LocationList.hpp>
#include <cmajor/cmcode/SearchDialog.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/cmsvc/DebugService.hpp>
#include <cmajor/cmsvc/DebugServiceRequest.hpp>
#include <cmajor/cmsvc/CodeCompletionService.hpp>
#include <cmajor/cmsvc/CodeCompletionServiceRequest.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>
#include <cmajor/cmview/CmajorEditor.hpp>
#include <cmajor/cmview/ResourceFileEditor.hpp>
#include <cmajor/cmview/TextFileEditor.hpp>
#include <cmajor/cmview/CallStackView.hpp>
#include <cmajor/cmview/LocalsView.hpp>
#include <cmajor/cmview/ErrorView.hpp>
#include <cmajor/cmview/SearchResultsView.hpp>
#include <cmajor/cmview/CodeCompletionView.hpp>
#include <cmajor/wing/Clipboard.hpp>
#include <cmajor/wing/Console.hpp>
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
#include <soulng/rex/Nfa.hpp>

namespace cmcode {

extern const char* cmajorCodeVersion;

const int buildProgressTimerId = 10;
const int buildProgressTimerPeriod = 100;
const int startupDialogTimer = 15;
const int startupDialogTimerDelay = 10;
const int toolTipTimerId = 12;
const int toolTipShowPeriod = 3000;
const int ccTimerId = 20;

using namespace cmajor::view;
using namespace cmajor::wing;
using namespace soulng::rex;

enum class MainWindowState : int
{
    idle, building, debugging, running
};

enum class CCState : int
{
    idle, editModuleLoaded, error
};

struct ExpressionEvaluateRequest
{
    ExpressionEvaluateRequest(const std::string& expression_, const Point& screenLoc_);
    std::string expression;
    Point screenLoc;
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
    void GotoCursor(const SourceLoc& sourceLocation);
    void GotoLocation(const DefinitionSourceLocation& location);
    void SetProjectReferences(sngcm::ast::Project* project);
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
    void ShowStartupDialog();
    void SaveProjectData();
    void SaveSolutionData();
    void AddClipboardListener();
    void RemoveClipboardListener();
    void StartBuilding();
    void StopBuilding();
    void ShowBuildProgress();
    void StartDebugging();
    void StopDebugging();
    void StartRunning();
    void StopRunning();
    void StartCodeCompletion();
    void StopCodeCompletion(bool log);
    void LoadEditModule();
    void LoadEditModule(sngcm::ast::Project* project);
    void LoadEditModuleForCurrentFile();
    void ResetEditModuleCache();
    void ParseSource();
    void GetParamHelpList(int symbolIndex);
    void HandleServiceMessage();
    void ClearOutput();
    void WriteOutput(const std::string& text);
    void OpenProject(const std::string& filePath);
    void SetIDEState();
    void AddEditor(const std::string& filePath);
    void HandleBuildReply(BuildReply& buildReply);
    void HandleBuildError(const std::string& buildError);
    void HandleGetDefinitionReply(GetDefinitionReply& getDefinitionReply);
    void HandleGetDefinitionError(const std::string& getDefinitionError);
    void HandleStopBuild();
    void HandleStartDebugReply(const StartDebugReply& startDebugReply);
    void HandleStartDebugError(const std::string& error);
    void HandleContinueReply(const ContinueReply& continueReply);
    void HandleNextReply(const NextReply& nextReply);
    void HandleStepReply(const StepReply& stepReply);
    void HandleFinishReply(const FinishReply& finishReply);
    void HandleUntilReply(const UntilReply& untilReply);
    void HandleBreakReply(const BreakReply& breakReply);
    void HandleDeleteReply(const DeleteReply& deleteReply);
    void HandleDepthReply(const DepthReply& depthReply);
    void HandleFramesReply(const FramesReply& framesReply);
    void HandleEvaluateReply(const EvaluateReply& evaluateReply, int requestId);
    void HandleCountReply(const CountReply& countReply);
    void HandleEvaluateChildReply(const EvaluateChildReply& evaluateChildReply);
    void HandleLocation(const ::Location& location, bool saveLocation, bool setSelection);
    void HandleTargetState(TargetState state);
    void HandleTargetRunning();
    void HandleTargetInput();
    void HandleTargetOutputRequest(const TargetOutputRequest& targetOutputRequest);
    void ConsoleInputReady();
    void HandleDebugServiceStopped();
    void HandleProcessTerminated();
    void HandleRunServiceStopped();
    void HandleLoadEditModuleReply(const LoadEditModuleReply& loadEditModuleReply);
    void HandleLoadEditModuleError(const std::string& error);
    void HandleResetEditModuleCacheReply(const ResetEditModuleCacheReply& resetEditModuleCacherReply);
    void HandleResetEditModuleCacheError(const std::string& error);
    void HandleParseSourceReply(const ParseSourceReply& parseSourceReply);
    void HandleParseSourceError(const std::string& error);
    void HandleGetCCListReply(const GetCCListReply& getCCListReply);
    void HandleGetCCListError(const std::string& error);
    void HandleGetParamHelpListReply(const GetParamHelpListReply& getParamHelpListReply);
    void HandleGetParamHelpListError(const std::string& error);
    void SetState(MainWindowState state_);
    void SetEditorState();
    void ResetDebugLocations();
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
    void ChangeBreakpoints(CancelArgs& args);
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
    void Search(SearchExtent extent, const std::vector<std::string>& files, const std::vector<std::u32string>& lines, const std::string& searchText, 
        bool wholeWords, bool caseInsensitive, bool regularExpression);
    bool Search(const std::u32string& line, const std::u32string& text, bool wholeWords, Nfa* re);
    void ViewSearchResult(ViewSearchResultEventArgs& args);
    void OptionsClick();
    void CallStackClick();
    void LocalsClick();
    void ErrorsClick();
    void SearchResultsClick();
    void PortMapClick();
    void BuildActiveProjectClick();
    bool BuildActiveProject();
    void RebuildActiveProjectClick();
    void CleanActiveProjectClick();
    void BuildSettingsClick();
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
    void ToggleCodeCompletionClick();
    void TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args);
    void TreeViewNodeClick(TreeViewNodeClickEventArgs& args);
    void TreeViewNodeExpanded(TreeViewNodeEventArgs& args);
    void TreeViewNodeCollapsed(TreeViewNodeEventArgs& args);
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
    SearchResultsView* GetSearchResultsView();
    CallStackView* GetCallStackView();
    void ClearCallStack();
    void UpdateCallStack();
    void CallStackFrameSelected(FrameSelectedEventArgs& args);
    LocalsView* GetLocalsView();
    void ClearLocals();
    void UpdateLocals();
    void LocalsViewNodeExpanded(TreeViewNodeEventArgs& args);
    void LocalsViewUpdateNeeded();
    Console* GetConsole();
    void UpdateCurrentDebugStrip();
    void ResetSelections();
    void ExpressionHover(ExpressionHoverEventArgs& args);
    void CCEscape();
    void CCTextChanged();
    void CCShow();
    void CCHide();
    void CCNext();
    void CCPrev();
    void CCNextPage();
    void CCPrevPage();
    void CCSelect(SelectedEventArgs& args);
    void CCStart();
    void CCStop();
    void ParamHelpNext();
    void ParamHelpPrev();
    void ParamHelpClose();
    void CCViewLocationChanged();
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
    MenuItem* buildSettingsMenuItem;
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
    TabPage* logTabPage;
    LogView* log;
    TabPage* consoleTabPage;
    Console* console;
    TabPage* debugTabPage;
    StatusBar* statusBar;
    TabPage* searchResultsTabPage;
    SearchResultsView* searchResultsView;
    TabPage* callStackTabPage;
    CallStackView* callStackView;
    TabPage* localsTabPage;
    LocalsView* localsView;
    StatusBarTextItem* buildIndicatorStatuBarItem;
    StatusBarTextItem* editorReadWriteIndicatorStatusBarItem;
    StatusBarTextItem* editorDirtyIndicatorStatusBarItem;
    StatusBarTextItem* sourceFilePathStatusBarItem;
    StatusBarTextItem* codeCompletionStatusBarItem;
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
    bool programRunning;
    bool startDebugging;
    bool signalReceived;
    int callStackDepth;
    bool callStackOpen;
    bool localsViewOpen;
    std::string backend;
    std::string config;
    std::unique_ptr<ClipboardListener> clipboardListener;
    int pid;
    CCState ccState;
    ClipboardFormat cmajorCodeFormat;
    std::u32string clipboardData;
    std::vector<std::unique_ptr<ClickAction>> clickActions;
    LocationList locations;
    std::unique_ptr<Request> debugRequest;
    ::Location savedLocation;
    std::vector<std::string> buildIndicatorTexts;
    std::vector<ExpressionEvaluateRequest> expressionEvaluateRequests;
    ToolTip* toolTipWindow;
    CodeCompletionListView* codeCompletionListView;
    CodeCompletionParamHelpView* paramHelpView;
    bool ccTimerRunning;
    bool editModuleLoaded;
    bool ccCanSelect;
    CCEntry currentCCEntry;
    std::vector<int> ruleContext;
};

} // namespace cmcode

#endif // CMCODE_MAIN_WINDOW_INCLUDED
