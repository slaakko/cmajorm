// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_INFO_DEBUGGER_INCLUDED
#define CMAJOR_DEBUG_INFO_DEBUGGER_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <string>
#include <vector>
#include <memory>

namespace cmajor { namespace debug {

using namespace soulng::util;

class DebugInfo;
class Instruction;
class DebuggerCommand;
struct SourceLocation;

class DEBUG_API GdbBreakpoint
{
public:
    GdbBreakpoint(Instruction* instruction_);
    void SetNumber(int number_) { number = number_; }
    int Number() const { return number; }
    Instruction* GetInstruction() const { return instruction; }
private:
    Instruction* instruction;
    int number;
};

class DEBUG_API DebuggerBreakpoint
{
public:
    DebuggerBreakpoint(const std::string& id_);
    const std::string& Id() const { return id; }
    const std::vector<int>& GdbBreakpointNumbers() const { return gdbBreakpointNumbers; }
    void AddGdbBreakpointNumber(int gdbBreakpointNumber);
    void SetFrame(const Frame& frame_);
    const Frame& GetFrame() const { return frame; }
private:
    std::string id;
    std::vector<int> gdbBreakpointNumbers;
    Frame frame;
};

struct DEBUG_API StopRecord
{
    StopRecord();
    void Print(CodeFormatter& formatter);
    std::string reason;
    int breakpointNumber;
    int threadId;
    Frame cppFrame;
    Frame cmajorFrame;
};

class DEBUG_API Debugger
{
public:
    Debugger(const std::string& executable, const std::vector<std::string>& args, bool verbose_, CodeFormatter& formatter_);
    ~Debugger();
    enum class State
    {
        initializing, programStarted, running, stopped, programExitedNormally, exitingDebugger
    };
    void Exit();
    bool Exiting() const { return state == State::exitingDebugger; }
    bool Run();
    void Help();
    void Next();
    void Step();
    void Continue();
    void Finish();
    void Until(const SourceLocation& location);
    bool Break(std::unique_ptr<DebuggerBreakpoint>&& bp, const std::vector<Instruction*>& instructions);
    void Break(const SourceLocation& location);
    void Delete(const std::string& breakpointId);
    void Depth();
    void Frames(int low, int high);
    void ShowBreakpoints();
    void List(const SourceLocation& location);
    void RepeatLatestCommand();
    bool IsStopInstruction(Instruction* instruction) const;
    bool ExecuteGDBCommand(const GdbCommand& command);
    void ProcessReply(GdbCommand::Kind commandKind, GdbReply* reply);
    void ProcessReplyRecord(GdbReplyRecord* record);
    void ProcessConsoleOutput(GdbConsoleOutputRecord* record);
    void ProcessTargetOutput(GdbTargetOutputRecord* record);
    void ProcessLogOutput(GdbLogOutputRecord* record);
    void ProcessBreakInsertReply(GdbReply* reply);
    void ProcessBreakDeleteReply(GdbReply* reply);
    void ProcessExecRunReply(GdbReply* reply);
    void ProcessExecNextReply(GdbReply* reply);
    void ProcessExecStepReply(GdbReply* reply);
    void ProcessExecContinueReply(GdbReply* reply);
    void ProcessExecFinishReply(GdbReply* reply);
    void ProcessExecUntilReply(GdbReply* reply);
    void ProcessStackInfoDepthReply(GdbReply* reply);
    void ProcessStackListFramesReply(GdbReply* reply);
    bool ProcessExecStoppedRecord(GdbExecStoppedRecord* execStoppedRecord);
    void StartProgram();
    GdbBreakpoint* SetBreakpoint(Instruction* instruction);
    bool DeleteBreakpoint(Instruction* instruction);
    GdbBreakpoint* GetBreakpoint(Instruction* instruction) const;
    GdbBreakpoint* GetBreakpoint(int number) const;
    Instruction* GetInstructionForCppLocation(const std::string& cppFile, int cppLine);
    void SetLatestCommand(DebuggerCommand* latestCommand_);
    void SetState(State state_);
    std::string StateStr(State state) const;
    int GetNextBreakpointNumber() { return nextBreakpointNumber++; }
    std::string GetNextTemporaryBreakpointId();
    std::string& CurrentSourceFilePath() { return currentSourceFilePath; }
private:
    State state;
    Instruction* stoppedInstruction;
    StopRecord stopRecord;
    int stackDepth;
    std::vector<Frame> cppFrames;
    std::vector<Frame> cmajorFrames;
    bool verbose;
    std::unique_ptr<DebugInfo> debugInfo;
    CodeFormatter& formatter;
    std::unordered_map<Instruction*, GdbBreakpoint*> gdbBreakpointsByInstruction;
    std::unordered_map<int, GdbBreakpoint*> gdbBreakpointsByNumber;
    std::map<std::string, DebuggerBreakpoint*> debuggerBreakpointMap;
    std::unique_ptr<DebuggerCommand> latestCommand;
    int nextBreakpointNumber;
    int nextTempBreakpointNumber;
    SourceLocation listLocation;
    std::string currentSourceFilePath;
};

class DEBUG_API DebuggerCommand
{
public:
    enum class Kind
    {
        exit, help, next, step, continue_, finish, until, break_, delete_, depth, frames, showBreakpoints, list, repeatLatest
    };
    DebuggerCommand(Kind kind_);
    virtual ~DebuggerCommand();
    virtual void Execute(Debugger& debugger) = 0;
    virtual DebuggerCommand* Clone() = 0;
    Kind GetKind() const { return kind; }
private:
    Kind kind;
};

class DEBUG_API DebuggerExitCommand : public DebuggerCommand
{
public:
    DebuggerExitCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerHelpCommand : public DebuggerCommand
{
public:
    DebuggerHelpCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerNextCommand : public DebuggerCommand
{
public:
    DebuggerNextCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerStepCommand : public DebuggerCommand
{
public:
    DebuggerStepCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerContinueCommand : public DebuggerCommand
{
public:
    DebuggerContinueCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerFinishCommand : public DebuggerCommand
{
public:
    DebuggerFinishCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerUntilCommand : public DebuggerCommand
{
public:
    DebuggerUntilCommand(const SourceLocation& location_);
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
private:
    SourceLocation location;
};

class DEBUG_API DebuggerBreakCommand : public DebuggerCommand
{
public:
    DebuggerBreakCommand(const SourceLocation& location_);
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
private:
    SourceLocation location;
};

class DEBUG_API DebuggerDeleteCommand : public DebuggerCommand
{
public:
    DebuggerDeleteCommand(int breakpointNumber_);
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
private:
    int breakpointNumber;
};

class DEBUG_API DebuggerDepthCommand : public DebuggerCommand
{
public:
    DebuggerDepthCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerFramesCommand : public DebuggerCommand
{
public:
    DebuggerFramesCommand(int low_, int high_);
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
private:
    int low;
    int high;
};

class DEBUG_API DebuggerShowBreakpointsCommand : public DebuggerCommand
{
public:
    DebuggerShowBreakpointsCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

class DEBUG_API DebuggerListCommand : public DebuggerCommand
{
public:
    DebuggerListCommand(const SourceLocation& location_);
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
private:
    SourceLocation location;
};

class DEBUG_API DebuggerRepeatLatestCommand : public DebuggerCommand
{
public:
    DebuggerRepeatLatestCommand();
    void Execute(Debugger& debugger) override;
    DebuggerCommand* Clone() override;
};

DEBUG_API void RunDebuggerInteractive(const std::string& executable, const std::vector<std::string>& args, bool verbose);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_INFO_DEBUGGER_INCLUDED

