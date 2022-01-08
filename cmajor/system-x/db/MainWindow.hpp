// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_MAIN_WINDOW_INCLUDED
#define CMSX_DB_MAIN_WINDOW_INCLUDED
#include <system-x/db/DebugView.hpp>
#include <system-x/db/CodeView.hpp>
#include <system-x/db/RegisterView.hpp>
#include <system-x/db/DataView.hpp>
#include <wing/LogView.hpp>
#include <wing/Window.hpp>
#include <wing/Menu.hpp>
#include <wing/SplitContainer.hpp>
#include <wing/TabControl.hpp>
#include <machine/Machine.hpp>
#include <kernel/Process.hpp>
#include <db/Debugger.hpp>
#include <thread>
#include <condition_variable>

namespace cmsx::db {

extern const char* cmsxDBVersion;

using namespace cmajor::wing;

const int DEBUGGER_READY = WM_USER + 1;
const int DEBUGGER_ERROR = WM_USER + 2;
const int DEBUGGER_PROCESS_EXIT = WM_USER + 3;

class MainWindow;

class MainDebuggerObserver : public DebuggerObserver
{
public:
    MainDebuggerObserver(MainWindow* mainWindow_);
    void DebuggerReady() override;
    void DebuggerError(const std::string& message) override;
    void DebuggerProcessExit() override;
    const std::string& ErrorMessage() const { return errorMessage; }
private:
    MainWindow* mainWindow;
    std::string errorMessage;
};

enum class DebuggingState
{
    debuggerIdle, debuggerWaitingForCommand, debuggerBusy, debuggerExit, debuggerError
};

class MainWindow : public Window
{
public:
    MainWindow(const std::string& filePath_);
    ~MainWindow();
    bool WaitingDebugger() const { return waitingDebugger; }
    void NotifyDebuggingStopped();
    void NotifyDebuggerError();
protected:
    bool ProcessMessage(Message& msg) override;
    void OnMouseWheel(MouseWheelEventArgs& args) override;
private:
    void ToggleBreakpointClick();
    void SingleStepClick();
    void StepOverClick();
    void ContinueClick();
    void AboutClick();
    void OpenFileClick();
    void CloseFileClick();
    void ResetClick();
    void StopClick();
    void ExitClick();
    void ViewCodeClick();
    void ViewRegsClick();
    void ViewDataClick();
    void ViewArgsClick();
    void ViewEnvClick();
    void ViewHeapClick();
    void ViewStackClick();
    void ViewLogClick();
    void NextLineClick();
    void PrevLineClick();
    void NextPageClick();
    void PrevPageClick();
    void NextQuarterClick();
    void PrevQuarterClick();
    void HomeClick();
    void EndClick();
    void PrevAddressClick();
    void TopTabPageSelected();
    void TopTabPageRemoved(ControlEventArgs& controlEventArgs);
    void BottomTabPageSelected();
    void BottomTabPageRemoved(ControlEventArgs& controlEventArgs);
    void LoadProcess();
    void StartDebugging();
    void StopDebugging(bool unloadProcess);
    void UpdateViews();
    void RemoveView(DebugView* debugView);
    void CreateCodeView();
    void CreateRegisterView();
    void CreateDataView();
    void CreateArgsView();
    void CreateEnvView();
    void CreateHeapView();
    void CreateStackView();
    void CreateLogView();
    TabPage* GetTabPageByNameOrFirstTabPage(TabControl* tabControl, const std::string& tabName) const;
    void WaitUntilDebuggingStoppedOrError();
    void PrintError(const std::string& errorMessage);
    void PrintExit();
    void SetState(DebuggingState state_);
    MainDebuggerObserver observer;
    std::unique_ptr<cmsx::machine::Machine> machine;
    std::unique_ptr<Debugger> debugger;
    CodeView* codeView;
    RegisterView* registerView;
    DataView* dataView;
    DataView* argsView;
    DataView* envView;
    DataView* heapView;
    DataView* stackView;
    LogView* logView;
    std::string filePath;
    std::vector<std::string> args;
    std::vector<std::string> env;
    std::thread debuggerThread;
    cmsx::kernel::Process* process;
    cmajor::wing::MenuItem* openFileMenuItem;
    cmajor::wing::MenuItem* closeFileMenuItem;
    cmajor::wing::MenuItem* exitMenuItem;
    cmajor::wing::SplitContainer* verticalSplitContainer;
    cmajor::wing::TabControl* topTabControl;
    cmajor::wing::TabControl* bottomTabControl;
    DebugView* currentTopView;
    DebugView* currentBottomView;
    std::vector<DebugView*> views;
    DataRanges dataRanges;
    bool waitingDebugger;
    std::mutex mtx;
    std::condition_variable debuggingStoppedOrErrorVar;
    MenuItem* startContinueMenuItem;
    MenuItem* stopMenuItem;
    MenuItem* resetMenuItem;
    MenuItem* singleStepMenuItem;
    MenuItem* stepOverMenuItem;
    MenuItem* toggleBreakpointMenuItem;
    MenuItem* gotoPrevAddressMenuItem;
    DebuggingState state;
    bool fileOpen;
};

} // namespace cmsx::db

#endif // CMSX_DB_MAIN_WINDOW_INCLUDED
