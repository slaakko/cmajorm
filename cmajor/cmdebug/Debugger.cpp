// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/Debugger.hpp>
#include <cmajor/cmdebug/DebuggerCommandParser.hpp>
#include <cmajor/cmdebug/DebuggerCommandLexer.hpp>
#include <cmajor/cmdebug/DebugExprLexer.hpp>
#include <cmajor/cmdebug/DebugExprParser.hpp>
#include <cmajor/cmdebug/DIExpr.hpp>
#include <cmajor/cmdebug/DebugExprTranslatorVisitor.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/TokenValueParsers.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

std::unique_ptr<DebuggerCommand> ParseDebuggerCommand(std::string& currentSourceFilePath, const std::string& line)
{
    std::string commandLine = line;
    commandLine.append("\r\n");
    DebuggerCommandLexer lexer(ToUtf32(commandLine), "", 0);
    std::unique_ptr<DebuggerCommand> command = DebuggerCommandParser::Parse(lexer, &currentSourceFilePath);
    return command;
}

Console::Console() : driver(nullptr), terminated(false), commandAvailable(false), commandReceived(false), targetRunning(false), canProceeed(false)
{
}

void Console::SetDriver(GdbDriver* driver_)
{
    driver = driver_;
}

void Console::Run()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        canProceeed = false;
        bool commandIsRunningCommand = false;
        if (driver->TargetRunning())
        {
            WriteTargetInputLine(line);
        }
        else
        {
            std::unique_ptr<DebuggerCommand> command;
            try
            {
                command = ParseDebuggerCommand(driver->CurrentSourceFilePath(), line);
            }
            catch (const std::exception& ex)
            {
                driver->Error(ex.what());
                driver->Prompt();
                continue;
            }
            commandIsRunningCommand = command->IsRunningCommand(*driver);
            if (command->GetKind() == DebuggerCommand::Kind::exit)
            {
                break;
            }
            else
            {
                std::unique_lock<std::mutex> lock(mtx);
                commandReceived = false;
                targetRunning = false;
                commands.push_back(std::move(command));
                commandAvailableVar.notify_one();
                commandAvailable = true;
                commandReceivedVar.wait(lock, [this]{ return commandReceived; });
            }
        }
        if (commandIsRunningCommand)
        {
            std::unique_lock<std::mutex> lock(mtx);
            targetRunningVar.wait(lock, [this] { return targetRunning; });
        }
        commandAvailable = false;
        bool wait = true;
        while (wait)
        {
            std::unique_lock<std::mutex> lock(mtx);
            std::cv_status status = proceed.wait_for(lock, std::chrono::milliseconds{ 500 });
            if (status == std::cv_status::timeout)
            {
                if (driver->TargetRunning())
                {
                    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                    std::chrono::steady_clock::duration idlePeriod = now - activeTimeStamp;
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(idlePeriod).count() > 3000)
                    {
                        wait = false;
                    }
                }
            }
            else if (status == std::cv_status::no_timeout && canProceeed)
            {
                wait = false;
            }
        }
        if (!driver->TargetRunning())
        {
            driver->Prompt();
        }
    }
    terminated = true;
    CloseTargetHandles();
    TerminateGDB();
    std::unique_lock<std::mutex> lock(mtx);
    commands.push_back(std::unique_ptr<DebuggerCommand>(new DebuggerExitCommand()));
    commandAvailable = true;
    commandAvailableVar.notify_one();
}

void Console::SetActive()
{
    activeTimeStamp = std::chrono::steady_clock::now();
}

std::unique_ptr<DebuggerCommand> Console::GetCommand()
{
    if (terminated) return std::unique_ptr<DebuggerCommand>();
    std::unique_lock<std::mutex> lock(mtx);
    commandAvailableVar.wait(lock, [this] { return commandAvailable; });
    std::unique_ptr<DebuggerCommand> command = std::move(commands.front());
    commands.pop_front();
    commandReceived = true;
    commandReceivedVar.notify_one();
    return command;
}

void Console::SetTargetRunning()
{
    std::unique_lock<std::mutex> lock(mtx);
    targetRunning = true;
    targetRunningVar.notify_one();
}

void Console::Proceed()
{
    std::unique_lock<std::mutex> lock(mtx);
    canProceeed = true;
    proceed.notify_one();
}

void RunConsole(Console* console)
{
    console->Run();
}

GdbBreakpoint::GdbBreakpoint(Instruction* instruction_) : instruction(instruction_), number(-1)
{
}

DebuggerBreakpoint::DebuggerBreakpoint(const std::string& id_) : id(id_), frame()
{
}

void DebuggerBreakpoint::AddGdbBreakpointNumber(int gdbBreakpointNumber)
{
    gdbBreakpointNumbers.push_back(gdbBreakpointNumber);
}

void DebuggerBreakpoint::SetFrame(const Frame& frame_)
{
    frame = frame_;
}

Frame GetFrame(GdbTupleValue* frameValue)
{
    Frame frame;
    GdbValue* levelValue = frameValue->GetField("level");
    if (levelValue && levelValue->GetKind() == GdbValue::Kind::string)
    {
        try
        {
            frame.level = boost::lexical_cast<int>(static_cast<GdbStringValue*>(levelValue)->Value());
        }
        catch (const std::exception&)
        {
            frame.level = -1;
        }
    }
    GdbValue* funcValue = frameValue->GetField("func");
    if (funcValue && funcValue->GetKind() == GdbValue::Kind::string)
    {
        frame.func = static_cast<GdbStringValue*>(funcValue)->Value();
    }
    GdbValue* fileValue = frameValue->GetField("file");
    if (fileValue && fileValue->GetKind() == GdbValue::Kind::string)
    {
        frame.file = static_cast<GdbStringValue*>(fileValue)->Value();
    }
    GdbValue* lineValue = frameValue->GetField("line");
    if (lineValue && lineValue->GetKind() == GdbValue::Kind::string)
    {
        frame.line = boost::lexical_cast<int>(static_cast<GdbStringValue*>(lineValue)->Value());
    }
    return frame;
}

bool GetBreakpointResults(GdbResults* results, std::string& file, int& line, int& number, CodeFormatter& formatter)
{
    file.clear();
    line = 0;
    number = -1;
    try
    {
        GdbValue* bkptValue = results->GetField("bkpt");
        if (bkptValue)
        {
            if (bkptValue->GetKind() == GdbValue::Kind::tuple)
            {
                GdbTupleValue* tuple = static_cast<GdbTupleValue*>(bkptValue);
                GdbValue* fileValue = tuple->GetField("file");
                if (fileValue && fileValue->GetKind() == GdbValue::Kind::string)
                {
                    file = static_cast<GdbStringValue*>(fileValue)->Value();
                }
                GdbValue* lineValue = tuple->GetField("line");
                if (lineValue && lineValue->GetKind() == GdbValue::Kind::string)
                {
                    line = boost::lexical_cast<int>(static_cast<GdbStringValue*>(lineValue)->Value());
                }
                GdbValue* numberValue = tuple->GetField("number");
                if (numberValue && numberValue->GetKind() == GdbValue::Kind::string)
                {
                    number = boost::lexical_cast<int>(static_cast<GdbStringValue*>(numberValue)->Value());
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        formatter.WriteLine("error getting breakpoint results: " + std::string(ex.what()));
    }
    return !file.empty() && line != 0 && number != -1;
}

bool GetStoppedResults(GdbResults* results, std::unique_ptr<JsonObject>& stopObject, Frame& frame)
{
    stopObject.reset(new JsonObject());
    try
    {
        GdbValue* reasonValue = results->GetField("reason");
        stopObject->AddField(U"success", std::unique_ptr<JsonValue>(new JsonString(U"true")));
        if (reasonValue && reasonValue->GetKind() == GdbValue::Kind::string)
        {
            std::string reason = static_cast<GdbStringValue*>(reasonValue)->Value();
            stopObject->AddField(U"reason", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(reason))));
            if (reason == "breakpoint-hit")
            {
                GdbValue* breakpointNumberValue = results->GetField("bkptno");
                if (breakpointNumberValue && breakpointNumberValue->GetKind() == GdbValue::Kind::string)
                {
                    stopObject->AddField(U"breakpointNumber",
                        std::unique_ptr<JsonValue>(new JsonString(ToUtf32(static_cast<GdbStringValue*>(breakpointNumberValue)->Value()))));
                }
            }
        }
        GdbValue* threadIdValue = results->GetField("thread-id");
        if (threadIdValue && threadIdValue->GetKind() == GdbValue::Kind::string)
        {
            stopObject->AddField(U"threadId", 
                std::unique_ptr<JsonValue>(new JsonString(ToUtf32(static_cast<GdbStringValue*>(threadIdValue)->Value()))));
        }
        GdbValue* frameValue = results->GetField("frame");
        if (frameValue && frameValue->GetKind() == GdbValue::Kind::tuple)
        {
            frame = GetFrame(static_cast<GdbTupleValue*>(frameValue));
            stopObject->AddField(U"cppFrame", frame.ToJson(false));
        }
    }
    catch (const std::exception& ex)
    {
        stopObject->AddField(U"success", std::unique_ptr<JsonValue>(new JsonString(U"false")));
        stopObject->AddField(U"error", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ex.what()))));
        return false;
    }
    return true;
}

bool GetStackDepthResult(GdbResults* results, std::unique_ptr<JsonValue>& depthResult)
{
    JsonObject* jsonObject = new JsonObject();
    try
    {
        GdbValue* depthValue = results->GetField("depth");
        if (depthValue && depthValue->GetKind() == GdbValue::Kind::string)
        {
            jsonObject->AddField(U"depth", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(static_cast<GdbStringValue*>(depthValue)->Value()))));
            jsonObject->AddField(U"success", std::unique_ptr<JsonValue>(new JsonString(U"true")));
        }
        return true;
    }
    catch (const std::exception& ex)
    {
        jsonObject->AddField(U"success", std::unique_ptr<JsonValue>(new JsonString(U"false")));
        jsonObject->AddField(U"error", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ex.what()))));
    }
    depthResult.reset(jsonObject);
    return false;
}

bool GetStackListFramesResult(GdbResults* results, std::unique_ptr<JsonValue>& cppStackResult, std::vector<Frame>& cppFrames)
{
    JsonObject* jsonObject = new JsonObject();
    try
    {
        JsonArray* framesArray = new JsonArray();
        GdbValue* stackValue = results->GetField("stack");
        if (stackValue && stackValue->GetKind() == GdbValue::Kind::list)
        {
            GdbListValue* list = static_cast<GdbListValue*>(stackValue);
            int n = list->Count();
            for (int i = 0; i < n; ++i)
            {
                GdbValue* frameValue = list->GetValue(i);
                if (frameValue && frameValue->GetKind() == GdbValue::Kind::result)
                {
                    GdbResult* result = static_cast<GdbResult*>(frameValue);
                    GdbValue* value = result->Value();
                    if (value && value->GetKind() == GdbValue::Kind::tuple)
                    {
                        Frame frame = GetFrame(static_cast<GdbTupleValue*>(value));
                        framesArray->AddItem(frame.ToJson(true));
                        cppFrames.push_back(frame);
                    }
                }
            }
        }
        jsonObject->AddField(U"success", std::unique_ptr<JsonValue>(new JsonString(U"true")));
        jsonObject->AddField(U"cppFrames", std::unique_ptr<JsonValue>(framesArray));
        cppStackResult.reset(jsonObject);
        return true;
    }
    catch (const std::exception& ex)
    {
        jsonObject->AddField(U"success", std::unique_ptr<JsonValue>(new JsonString(U"false")));
        jsonObject->AddField(U"error", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ex.what()))));
    }
    cppStackResult.reset(jsonObject);
    return false;
}

bool GetVarCreateResult(GdbResults* results, std::unique_ptr<JsonValue>& result, CodeFormatter& formatter)
{
    try
    {
        result.reset(results->ToJson());
        return true;
    }
    catch (const std::exception& ex)
    {
        formatter.WriteLine("error getting var create results: " + std::string(ex.what()));
    }
    return false;
}

bool GetVarEvaluateResult(GdbResults* results, std::unique_ptr<JsonValue>& result, CodeFormatter& formatter)
{
    try
    {
        result.reset(results->ToJson());
        return true;
    }
    catch (const std::exception& ex)
    {
        formatter.WriteLine("error getting var evaluate results: " + std::string(ex.what()));
    }
    return false;
}

bool GetVarListChildrenResult(GdbResults* results, std::unique_ptr<JsonValue>& result, CodeFormatter& formatter)
{
    try
    {
        result.reset(results->ToJson());
        return true;
    }
    catch (const std::exception& ex)
    {
        formatter.WriteLine("error getting var list children results: " + std::string(ex.what()));
    }
    return false;
}


Debugger::Debugger(const std::string& executable, const std::vector<std::string>& args, bool verbose_, CodeFormatter& formatter_, Console& console_) :
    verbose(verbose_), formatter(formatter_), state(State::initializing), wasRunning(false), targetOutput(false), nextBreakpointNumber(1), nextTempBreakpointNumber(1),
    nextGdbVariableIndex(1), console(console_)
{
    std::string cmdbFilePath = Path::ChangeExtension(executable, ".cmdb");
    if (boost::filesystem::exists(cmdbFilePath))
    {
        if (verbose)
        {
            formatter.WriteLine("> " + cmdbFilePath);
        }
        debugInfo = ReadDebugInfo(cmdbFilePath);
    }
    else
    {
        throw std::runtime_error("error: debug information '" + cmdbFilePath + "' file for executable '" + executable + "' not found");
    }
    if (verbose)
    {
        formatter.WriteLine("starting GDB...");
    }
    StartGDB(executable, args, *this);
    if (verbose)
    {
        GdbReply* startReply = GetGDBStartReply();
        startReply->Print(formatter);
    }
    StartProgram();
}

Debugger::~Debugger()
{
    try
    {
        if (!console.Terminated())
        {
            if (verbose)
            {
                std::lock_guard<std::recursive_mutex> lock(outputMutex);
                formatter.WriteLine("stopping GDB...");
            }
            StopGDB(*this);
        }
    }
    catch (...)
    {
    }
}

void Debugger::Exit()
{
    SetState(State::exitingDebugger);
}

void Debugger::Prompt()
{
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    formatter.Write("cmdb> ");
}

void Debugger::Error(const std::string& msg)
{
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    formatter.WriteLine(msg);
}

void Debugger::StartProgram()
{
    result.reset(new JsonObject());
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    Instruction* mainFunctionEntryInstruction = debugInfo->GetMainFunctionEntryInstruction();
    GdbBreakpoint* bp = SetBreakpoint(mainFunctionEntryInstruction);
    if (bp != nullptr)
    {
        bool succeeded = Run();
        if (succeeded)
        {
            succeeded = DeleteBreakpoint(bp->GetInstruction());
            if (succeeded)
            {
                if (verbose)
                {
                    formatter.WriteLine("program started.");
                }
                SetState(State::programStarted);
                AddStopResultToResult();
                result->Write(formatter);
                if (stoppedInstruction != nullptr)
                {
                    stoppedInstruction->PrintSource(formatter);
                }
            }
        }
    }
    Prompt();
}

GdbBreakpoint* Debugger::SetBreakpoint(Instruction* instruction)
{
    GdbBreakpoint* breakpoint = new GdbBreakpoint(instruction);
    gdbBreakpointsByInstruction[instruction] = breakpoint;
    GdbBreakInsertCommand breakInsertCommand(instruction->GetExplicitCppLocationArgs());
    bool succeeded = ExecuteGDBCommand(breakInsertCommand);
    if (succeeded)
    {
        return breakpoint;
    }
    else
    {
        DeleteBreakpoint(instruction);
        return nullptr;
    }
}

bool Debugger::DeleteBreakpoint(Instruction* instruction)
{
    bool succeeded = false;
    GdbBreakpoint* breakpoint = GetBreakpoint(instruction);
    if (breakpoint)
    {
        if (breakpoint->Number() != -1)
        {
            GdbBreakDeleteCommand breakDeleteComand(std::to_string(breakpoint->Number()));
            succeeded = ExecuteGDBCommand(breakDeleteComand);
            gdbBreakpointsByNumber.erase(breakpoint->Number());
        }
        auto it = gdbBreakpointsByInstruction.find(instruction);
        if (it != gdbBreakpointsByInstruction.cend())
        {
            GdbBreakpoint* bp = it->second;
            delete bp;
            gdbBreakpointsByInstruction.erase(instruction);
        }
    }
    return succeeded;
}

GdbBreakpoint* Debugger::GetBreakpoint(Instruction* instruction) const
{
    auto it = gdbBreakpointsByInstruction.find(instruction);
    if (it != gdbBreakpointsByInstruction.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

GdbBreakpoint* Debugger::GetBreakpoint(int number) const
{
    auto it = gdbBreakpointsByNumber.find(number);
    if (it != gdbBreakpointsByNumber.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Instruction* Debugger::GetInstructionForCppLocation(const std::string& cppFile, int cppLine)
{
    try
    {
        std::string projectDir = Path::GetDirectoryName(cppFile);
        Project* project = debugInfo->GetProjectByPath(projectDir);
        if (project)
        {
            std::string compileUnitBaseName = Path::GetFileNameWithoutExtension(cppFile);
            CompileUnit* compileUnit = project->GetCompileUnit(compileUnitBaseName);
            if (compileUnit)
            {
                Instruction* instruction = compileUnit->GetInstruction(cppLine);
                return instruction;
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.WriteLine("error getting C++ instruction for frame : cppFile=" + cppFile + ", cppLine=" + std::to_string(cppLine) + ": " + ex.what());
    }
    return nullptr;
}

void Debugger::SetLatestCommand(DebuggerCommand* latestCommand_)
{
    latestCommand.reset(latestCommand_);
}

bool Debugger::LatestCommandWasRunningCommand() 
{
    if (!latestCommand) return false;
    return latestCommand->IsRunningCommand(*this);
}

void Debugger::SetState(State state_)
{
    state = state_;
}

std::string Debugger::StateStr(State state) const
{
    switch (state)
    {
        case State::initializing: return "initializing";
        case State::programStarted: return "program started";
        case State::running: return "running";
        case State::stopped: return "stopped";
        case State::programExitedNormally: return "program exited normally";
        case State::exitingDebugger: return "exiting debugger";
    }
    return std::string();
}

std::string Debugger::GetNextTemporaryBreakpointId()
{
    int tempBpNumber = nextTempBreakpointNumber++;
    return "t" + std::to_string(tempBpNumber);
}

std::string Debugger::GetNextGdbVariableName()
{
    int index = nextGdbVariableIndex++;
    return "v" + std::to_string(index);
}

void Debugger::AddStopResultToResult()
{
    if (stopResult)
    {
        if (result->Type() == JsonValueType::object)
        {
            JsonObject* resultObject = static_cast<JsonObject*>(result.get());
            resultObject->AddField(U"state", std::move(stopResult));
        }
    }
}

bool Debugger::Run()
{
    GdbExecRunCommand execRunCommand;
    bool succeeded = ExecuteGDBCommand(execRunCommand);
    return succeeded;
}

void Debugger::Help()
{
    // todo
}

void Debugger::Next()
{
    result.reset(new JsonObject());
    if (state != State::stopped && state != State::programStarted)
    {
        throw std::runtime_error("error: state is '" + StateStr(state) + "'");
    }
    Instruction* prevStoppedInstruction = stoppedInstruction;
    bool stop = false;
    while (!stop)
    {
        stoppedInstruction = nullptr;
        GdbExecNextCommand execNextCommand;
        bool succeeded = ExecuteGDBCommand(execNextCommand);
        if (succeeded)
        {
            if (state == State::stopped)
            {
                if (stoppedInstruction != nullptr)
                {
                    if (stoppedInstruction->CppLineIndex() == 0)
                    {
                        if (prevStoppedInstruction == nullptr ||
                            prevStoppedInstruction->GetCompileUnitFunction() != stoppedInstruction->GetCompileUnitFunction() ||
                            stoppedInstruction->SourceLineNumber() > prevStoppedInstruction->SourceLineNumber())
                        {
                            if (IsStopInstruction(stoppedInstruction))
                            {
                                std::lock_guard<std::recursive_mutex> lock(outputMutex);
                                AddStopResultToResult();
                                result->Write(formatter);
                                stoppedInstruction->PrintSource(formatter);
                                stop = true;
                            }
                        }
                    }
                    else
                    {
                        if (prevStoppedInstruction == nullptr ||
                            prevStoppedInstruction->GetCompileUnitFunction() != stoppedInstruction->GetCompileUnitFunction())
                        {
                            if (IsStopInstruction(stoppedInstruction))
                            {
                                std::lock_guard<std::recursive_mutex> lock(outputMutex);
                                AddStopResultToResult();
                                result->Write(formatter);
                                stoppedInstruction->PrintSource(formatter);
                                stop = true;
                            }
                        }
                    }
                }
            }
            else
            {
                std::lock_guard<std::recursive_mutex> lock(outputMutex);
                AddStopResultToResult();
                result->Write(formatter);
                if (stoppedInstruction != nullptr)
                {
                    stoppedInstruction->PrintSource(formatter);
                }
                stop = true;
            }
        }
        else
        {
            if (result)
            {
                result->Write(formatter);
            }
        }
    }
}

void Debugger::Step()
{
    result.reset(new JsonObject());
    if (state != State::stopped && state != State::programStarted)
    {
        throw std::runtime_error("error: state is '" + StateStr(state) + "'");
    }
    Instruction* prevStoppedInstruction = stoppedInstruction;
    Instruction* prevSingleStepInstruction = nullptr;
    bool stop = false;
    bool skipNext = false;
    while (!stop)
    {
        GdbExecStepCommand execStepCommand;
        bool succeeded = true;
        if (skipNext)
        {
            skipNext = false;
        }
        else
        {
            stoppedInstruction = nullptr;
            succeeded = ExecuteGDBCommand(execStepCommand);
        }
        if (succeeded)
        {
            if (state == State::stopped)
            {
                if (stoppedInstruction != nullptr)
                {
                    prevSingleStepInstruction = stoppedInstruction;
                    if (stoppedInstruction->CppLineIndex() == 0)
                    {
                        if (prevStoppedInstruction == nullptr ||
                            prevStoppedInstruction->GetCompileUnitFunction() != stoppedInstruction->GetCompileUnitFunction() ||
                            stoppedInstruction->SourceLineNumber() > prevStoppedInstruction->SourceLineNumber())
                        {
                            if (IsStopInstruction(stoppedInstruction))
                            {
                                std::lock_guard<std::recursive_mutex> lock(outputMutex);
                                AddStopResultToResult();
                                result->Write(formatter);
                                stoppedInstruction->PrintSource(formatter);
                                stop = true;
                            }
                        }
                    }
                    else
                    {
                        if (prevStoppedInstruction == nullptr ||
                            prevStoppedInstruction->GetCompileUnitFunction() != stoppedInstruction->GetCompileUnitFunction())
                        {
                            if (IsStopInstruction(stoppedInstruction))
                            {
                                std::lock_guard<std::recursive_mutex> lock(outputMutex);
                                AddStopResultToResult();
                                result->Write(formatter);
                                stoppedInstruction->PrintSource(formatter);
                                stop = true;
                            }
                        }
                    }
                }
                else
                {
                    if (prevSingleStepInstruction != nullptr && prevSingleStepInstruction->Next() != nullptr)
                    {
                        GdbExecUntilCommand execUntilCommand(prevSingleStepInstruction->Next()->GetFileLineCppLocationArgs());
                        succeeded = ExecuteGDBCommand(execUntilCommand);
                        skipNext = true;
                    }
                }
            }
            else
            {
                std::lock_guard<std::recursive_mutex> lock(outputMutex);
                AddStopResultToResult();
                result->Write(formatter);
                if (stoppedInstruction != nullptr)
                {
                    stoppedInstruction->PrintSource(formatter);
                }
                stop = true;
            }
        }
        else if (result)
        {
            result->Write(formatter);
        }
    }
}

void Debugger::Continue()
{
    result.reset(new JsonObject());
    if (state != State::stopped && state != State::programStarted)
    {
        throw std::runtime_error("error: state is '" + StateStr(state) + "'");
    }
    GdbExecContinueCommand exeContinueCommand;
    bool succeeded = ExecuteGDBCommand(exeContinueCommand);
    if (succeeded)
    {
        if (state == State::stopped)
        {
            if (stoppedInstruction != nullptr)
            {
                if (stoppedInstruction->CppLineIndex() == 0 && IsStopInstruction(stoppedInstruction))
                {
                    std::lock_guard<std::recursive_mutex> lock(outputMutex);
                    AddStopResultToResult();
                    result->Write(formatter);
                    stoppedInstruction->PrintSource(formatter);
                }
            }
        }
    }
    else if (result)
    {
        result->Write(formatter);
    }
}

void Debugger::Finish()
{
    result.reset(new JsonObject());
    if (state != State::stopped && state != State::programStarted)
    {
        throw std::runtime_error("error: state is '" + StateStr(state) + "'");
    }
    GdbExecFinishCommand execFinishCommand;
    bool succeeded = ExecuteGDBCommand(execFinishCommand);
    if (succeeded)
    {
        if (state == State::stopped)
        {
            if (stoppedInstruction != nullptr)
            {
                if (stoppedInstruction->CppLineIndex() == 0 && IsStopInstruction(stoppedInstruction))
                {
                    std::lock_guard<std::recursive_mutex> lock(outputMutex);
                    AddStopResultToResult();
                    result->Write(formatter);
                    stoppedInstruction->PrintSource(formatter);
                }
                else
                {
                    Next();
                }
            }
            else
            {
                Next();
            }
        }
    }
    else if (result)
    {
        result->Write(formatter);
    }
}

void Debugger::Until(const SourceLocation& location)
{
    result.reset(new JsonObject());
    if (state != State::stopped && state != State::programStarted)
    {
        throw std::runtime_error("error: state is '" + StateStr(state) + "'");
    }
    std::vector<Instruction*> instructions = debugInfo->GetSourceFileMap().GetInstructions(location);
    if (instructions.size() == 1)
    {
        Instruction* instruction = instructions[0];
        GdbExecUntilCommand execUntilCommand(instruction->GetFileLineCppLocationArgs());
        bool succeeded = ExecuteGDBCommand(execUntilCommand);
        if (succeeded)
        {
            if (state == State::stopped)
            {
                if (stoppedInstruction != nullptr)
                {
                    if (stoppedInstruction->CppLineIndex() == 0 && IsStopInstruction(stoppedInstruction))
                    {
                        std::lock_guard<std::recursive_mutex> lock(outputMutex);
                        AddStopResultToResult();
                        result->Write(formatter);
                        stoppedInstruction->PrintSource(formatter);
                    }
                }
            }
        }
    }
    else if (instructions.size() > 1)
    {
        std::unique_ptr<DebuggerBreakpoint> bp(new DebuggerBreakpoint(GetNextTemporaryBreakpointId()));
        std::string bpId = bp->Id();
        if (Break(std::move(bp), instructions))
        {
            Continue();
            Delete(bpId);
        }
        else
        {
            throw std::runtime_error("error: could not set temporary breakpoint to location '" + location.ToString() + "'");
        }
    }
}

bool Debugger::Break(std::unique_ptr<DebuggerBreakpoint>&& bp, const std::vector<Instruction*>& instructions)
{
    std::unique_ptr<DebuggerBreakpoint> breakpoint(std::move(bp));
    for (Instruction* inst : instructions)
    {
        GdbBreakpoint* gdbbp = SetBreakpoint(inst);
        if (gdbbp != nullptr)
        {
            breakpoint->AddGdbBreakpointNumber(gdbbp->Number());
        }
        else
        {
            for (int gdbBreakpointNumber : breakpoint->GdbBreakpointNumbers())
            {
                GdbBreakpoint* gdbbp = GetBreakpoint(gdbBreakpointNumber);
                DeleteBreakpoint(gdbbp->GetInstruction());
            }
            return false;
        }
    }
    DebuggerBreakpoint* b = breakpoint.release();
    if (!instructions.empty())
    {
        b->SetFrame(instructions[0]->GetCmajorFrame());
    }
    debuggerBreakpointMap[b->Id()] = b;
    return true;
}

void Debugger::Break(const SourceLocation& location) 
{
    int breakpointNumber = GetNextBreakpointNumber();
    std::unique_ptr<DebuggerBreakpoint> bp(new DebuggerBreakpoint(std::to_string(breakpointNumber)));
    std::vector<Instruction*> instructions = debugInfo->GetSourceFileMap().GetInstructions(location);
    if (Break(std::move(bp), instructions))
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.WriteLine("breakpoint " + std::to_string(breakpointNumber) + " set to " + std::to_string(instructions.size()) +
            " instructions at '" + instructions[0]->GetCmajorFrame().ToString(false) + "'");
    }
}

void Debugger::Delete(const std::string& breakpointId)
{
    auto it = debuggerBreakpointMap.find(breakpointId);
    if (it != debuggerBreakpointMap.cend())
    {
        DebuggerBreakpoint* bp = it->second;
        std::string gdbBreakpoints;
        bool first = true;
        for (int gdbBreakpointNumber : bp->GdbBreakpointNumbers())
        {
            if (first)
            {
                first = false;
            }
            else
            {
                gdbBreakpoints.append(1, ' ');
            }
            gdbBreakpoints.append(std::to_string(gdbBreakpointNumber));
        }
        GdbBreakDeleteCommand breakDeleteCommand(gdbBreakpoints);
        bool succeeded = ExecuteGDBCommand(breakDeleteCommand);
        if (succeeded)
        {
            delete bp;
            debuggerBreakpointMap.erase(breakpointId);
            std::lock_guard<std::recursive_mutex> lock(outputMutex);
            formatter.WriteLine("breakpoint " + breakpointId + " deleted");
        }
        else
        {
            throw std::runtime_error("error: could not delete breakpoint " + breakpointId);
        }
    }
    else
    {
        throw std::runtime_error("error: breakpoint " + breakpointId + " not found");
    }
}

void Debugger::Depth()
{
    result.reset(new JsonObject());
    GdbStackInfoDepthCommand stackInfoDepthCommand;
    bool succeeded = ExecuteGDBCommand(stackInfoDepthCommand);
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    if (result)
    {
        result->Write(formatter);
    }
}

void Debugger::Frames(int low, int high)
{
    result.reset(new JsonObject());
    GdbStackListFramesCommand stackListFrames(low, high);
    bool succeeded = ExecuteGDBCommand(stackListFrames);
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    if (result)
    {
        result->Write(formatter);
    }
}

void Debugger::ShowBreakpoints()
{
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    for (const auto& p : debuggerBreakpointMap)
    {
        DebuggerBreakpoint* bp = p.second;
        formatter.WriteLine("breakpoint " + bp->Id() + ": " + std::to_string(bp->GdbBreakpointNumbers().size()) + " instructions at " + bp->GetFrame().ToString(false));
    }
}

void Debugger::List(const SourceLocation& location)
{
    if (!location.IsEmpty() && (location.path != listLocation.path || location.line != 0))
    {
        if (!location.path.empty())
        {
            if (location.path != listLocation.path)
            {
                listLocation = location;
            }
        }
        else if (location.line != 0)
        {
            listLocation.line = location.line;
        }
    }
    std::string sourceFilePath = debugInfo->GetSourceFileMap().GetSourceFilePath(listLocation);
    SourceFile& sourceFile = debugInfo->GetSourceFileCache().GetSourceFile(sourceFilePath);
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    sourceFile.Print(formatter, listLocation.line, stoppedInstruction, false);
    listLocation.line = std::min(listLocation.line + 2 * debugInfo->GetSourceFileWindowSize(), int(sourceFile.Lines().size() + 1));
    currentSourceFilePath = listLocation.path;
}

void Debugger::Print(const std::string& expression)
{
    if (!stoppedInstruction)
    {
        throw std::runtime_error("error: not stopped");
    }
    DebugExprLexer lexer(ToUtf32(expression), "", 0);
    std::unique_ptr<DebugExprNode> node = DebugExprParser::Parse(lexer);
    DebugExprTranslatorVisitor translatorVisitor(debugInfo.get(), stoppedInstruction->GetScope());
    node->Accept(translatorVisitor);
    DINode* translatedNode = translatorVisitor.TranslatedNode();
    if (translatedNode->IsTypeIdNode())
    {
        DIType* type = translatedNode->Type();
        JsonObject* object = new JsonObject();
        object->AddField(U"type", type->ToJson());
        result.reset(object);
    }
    else
    {
        std::string gdbVariableName = GetNextGdbVariableName();
        GdbVarCreateCommand varCreateCommand(gdbVariableName, "*", translatedNode->GdbExprString());
        bool succeeded = ExecuteGDBCommand(varCreateCommand);
        if (succeeded)
        {
            if (result)
            {
                DIType* type = translatedNode->Type();
                if (result->Type() == JsonValueType::object)
                {
                    JsonObject* object = static_cast<JsonObject*>(result.get());
                    DITypeRef typeRef(type);
                    object->AddField(U"static_type", typeRef.ToJson());
                }
                if (type->GetKind() == DIType::Kind::pointerType || type->GetKind() == DIType::Kind::referenceType)
                {
                    if (result->Type() == JsonValueType::object)
                    {
                        DIType* dynamicType = GetDynamicType(type, translatedNode);
                        if (dynamicType)
                        {
                            DITypeRef typeRef(dynamicType);
                            JsonObject* object = static_cast<JsonObject*>(result.get());
                            object->AddField(U"dynamic_type", typeRef.ToJson());
                        }
                    }
                }
            }
        }
    }
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    if (result)
    {
        result->Write(formatter);
    }
}

DIType* Debugger::GetDynamicType(DIType* diType, DINode* diNode)
{
    if (diType->GetKind() == DIType::Kind::pointerType)
    {
        DIPointerType* pointerType = static_cast<DIPointerType*>(diType);
        DIType* pointedType = pointerType->PointedToType();
        DIDerefNode derefNode(pointedType, diNode->Clone());
        DIType* dynamicType = GetDynamicType(pointedType, &derefNode);
        if (dynamicType)
        {
            DIType* ptrType = MakePointerType(dynamicType);
            return ptrType;
        }
    }
    else if (diType->GetKind() == DIType::Kind::referenceType)
    {
        DIReferenceType* referenceType = static_cast<DIReferenceType*>(diType);
        DIType* referredType = referenceType->BaseType();
        DIDerefNode derefNode(referredType, diNode->Clone());
        DIType* dynamicType = GetDynamicType(referredType, &derefNode);
        if (dynamicType)
        {
            DIType* refType = MakeReferenceType(dynamicType);
            return refType;
        }
    }
    else if (diType->GetKind() == DIType::Kind::constType)
    {
        DIConstType* constType = static_cast<DIConstType*>(diType);
        DIType* baseType = constType->BaseType();
        DIType* dynamicType = GetDynamicType(baseType, diNode);
        if (dynamicType)
        {
            DIType* cnstType = MakeConstType(dynamicType);
            return cnstType;
        }
    }
    else
    {
        if (diType->GetKind() == DIType::Kind::classType || diType->GetKind() == DIType::Kind::specializationType)
        {
            DIClassType* classType = static_cast<DIClassType*>(diType);
            if (classType->IsPolymorphic())
            {
                if (classType->VmtPtrIndex() != -1)
                {
                    DIDotNode vmtPtrMember(classType, diNode->Clone(), "m" + std::to_string(classType->VmtPtrIndex()), false);
                    DIAddrOfNode vmtPtrAddr(classType, vmtPtrMember.Clone());
                    std::string gdbVariableName = GetNextGdbVariableName();
                    GdbVarCreateCommand varCreateCommand(gdbVariableName, "*", vmtPtrAddr.GdbExprString());
                    std::unique_ptr<JsonValue> mainResult(result.release());
                    bool succeeded = ExecuteGDBCommand(varCreateCommand);
                    if (succeeded)
                    {
                        if (result->Type() == JsonValueType::object)
                        {
                            JsonObject* resultObject = static_cast<JsonObject*>(result.get());
                            JsonValue* value = resultObject->GetField(U"value");
                            if (value && value->Type() == JsonValueType::string)
                            {
                                std::string vmtVarFieldStr = ToUtf8(static_cast<JsonString*>(value)->Value());
                                std::string vmtVarName = ParseVmtVariableName(vmtVarFieldStr);
                                if (!vmtVarName.empty())
                                {
                                    DIType* dynamicType = debugInfo->GetPolymorphicType(vmtVarName);
                                    result.reset(mainResult.release());
                                    return dynamicType;
                                }
                            }
                        }
                    }
                    result.reset(mainResult.release());
                }
                else if (!classType->BaseClassId().is_nil())
                {
                    return GetDynamicType(classType->BaseClassType(), diNode);
                }
            }
        }
    }
    return nullptr;
}

void Debugger::RepeatLatestCommand()
{
    if (latestCommand)
    {
        latestCommand->Execute(*this);
    }
    else
    {
        throw std::runtime_error("no latest command: try 'help'");
    }
}

bool Debugger::IsStopInstruction(Instruction* instruction) const
{
    InstructionFlags flags = instruction->GetFlags();
    if ((flags & (InstructionFlags::beginBrace | InstructionFlags::endBrace)) != InstructionFlags::none)
    {
        return true;
    }
    if ((flags & (InstructionFlags::entryCode | InstructionFlags::exitCode)) != InstructionFlags::none)
    {
        return false;
    }
    return true;
}

bool Debugger::ExecuteGDBCommand(const GdbCommand& command)
{
    std::unique_ptr<GdbReply> reply = cmajor::debug::ExecuteGDBCommand(command, *this);
    if (!reply) return false;
    GdbReplyRecord* stoppedRecord = reply->GetStoppedRecord();
    while (stoppedRecord == nullptr)
    {
        ProcessReply(command.GetKind(), reply.get());
        ResetTargetOutputFlag();
        reply = ReadGDBReply(*this);
        if (!reply) return false;
        stoppedRecord = reply->GetStoppedRecord();
    }
    ProcessReply(command.GetKind(), reply.get());
    bool succeeded = true;
    GdbReplyRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord != nullptr)
    {
        succeeded = resultRecord->CommandSucceeded();
    }
    if (!succeeded)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("error executing GDB command '" + command.Str() + "': ");
        resultRecord->Print(formatter);
    }
    return succeeded;
}

void Debugger::ProcessReply(GdbCommand::Kind commandKind, GdbReply* reply)
{
    switch (commandKind)
    {
        case GdbCommand::Kind::breakInsert:
        {
            ProcessBreakInsertReply(reply);
            break;
        }
        case GdbCommand::Kind::breakDelete:
        {
            ProcessBreakDeleteReply(reply);
            break;
        }
        case GdbCommand::Kind::execRun:
        {
            ProcessExecRunReply(reply);
            break;
        }
        case GdbCommand::Kind::execNext:
        {
            ProcessExecNextReply(reply);
            break;
        }
        case GdbCommand::Kind::execStep:
        {
            ProcessExecStepReply(reply);
            break;
        }
        case GdbCommand::Kind::execContinue:
        {
            ProcessExecContinueReply(reply);
            break;
        }
        case GdbCommand::Kind::execFinish:
        {
            ProcessExecFinishReply(reply);
            break;
        }
        case GdbCommand::Kind::execUntil:
        {
            ProcessExecUntilReply(reply);
            break;
        }
        case GdbCommand::Kind::stackInfoDepth:
        {
            ProcessStackInfoDepthReply(reply);
            break;
        }
        case GdbCommand::Kind::stackListFrames:
        {
            ProcessStackListFramesReply(reply);
            break;
        }
        case GdbCommand::Kind::varCreate:
        {
            ProcessVarCreateReply(reply);
            break;
        }
        case GdbCommand::Kind::varEvaluateExpression:
        {
            ProcessVarEvaluateReply(reply);
            break;
        }
        case GdbCommand::Kind::varListChildren:
        {
            ProcessVarListChildrenReply(reply);
            break;
        }
    }
}

void Debugger::ProcessReplyRecord(GdbReplyRecord* record)
{
    switch (record->GetKind())
    {
        case GdbReplyRecord::Kind::execRunning:
        {
            SetState(State::running);
            wasRunning = true;
            console.SetActive();
            console.SetTargetRunning();
            break;
        }
        case GdbReplyRecord::Kind::execStopped:
        {
            SetState(State::stopped);
            break;
        }
        case GdbReplyRecord::Kind::notification:
        {
            // todo
            break;
        }
        case GdbReplyRecord::Kind::consoleOutput:
        {
            ProcessConsoleOutput(static_cast<GdbConsoleOutputRecord*>(record));
            break;
        }
        case GdbReplyRecord::Kind::targetOutput:
        {
            ProcessTargetOutput(static_cast<GdbTargetOutputRecord*>(record));
            targetOutput = true;
            break;
        }
        case GdbReplyRecord::Kind::logOutput:
        {
            ProcessLogOutput(static_cast<GdbLogOutputRecord*>(record));
        }
    }
}

void Debugger::ProcessConsoleOutput(GdbConsoleOutputRecord* record)
{
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    formatter.WriteLine("console output: " + record->Text());
    console.SetActive();
}

void Debugger::ProcessTargetOutput(GdbTargetOutputRecord* record)
{
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    formatter.WriteLine("target output: " + record->Text());
    console.SetActive();
}

void Debugger::ProcessLogOutput(GdbLogOutputRecord* record)
{
    std::lock_guard<std::recursive_mutex> lock(outputMutex);
    formatter.WriteLine("log output: " + record->Text());
    console.SetActive();
}

void Debugger::ProcessBreakInsertReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::done)
        {
            GdbResults* results = resultRecord->Results();
            if (results)
            {
                std::string breakpointFile;
                int breakpointLine = 0;
                int breakpointNumber = -1;
                if (GetBreakpointResults(results, breakpointFile, breakpointLine, breakpointNumber, formatter))
                {
                    Instruction* instruction = GetInstructionForCppLocation(breakpointFile, breakpointLine);
                    if (instruction)
                    {
                        GdbBreakpoint* breakpoint = GetBreakpoint(instruction);
                        if (breakpoint)
                        {
                            breakpoint->SetNumber(breakpointNumber);
                            gdbBreakpointsByNumber[breakpointNumber] = breakpoint;
                            success = true;
                        }
                    }
                }
            }
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful break-insert command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessBreakDeleteReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::done)
        {
            success = true;
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful break-delete command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessExecRunReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::running)
        {
            success = true;
        }
    }
    else
    {
        cmajor::debug::GdbReplyRecord* stoppedRecord = reply->GetStoppedRecord();
        if (stoppedRecord)
        {
            if (stoppedRecord->GetKind() == cmajor::debug::GdbReplyRecord::Kind::execStopped)
            {
                GdbExecStoppedRecord* execStoppedRecord = static_cast<GdbExecStoppedRecord*>(stoppedRecord);
                success = ProcessExecStoppedRecord(execStoppedRecord);
            }
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful exec-run command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessExecContinueReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::running)
        {
            success = true;
        }
    }
    else
    {
        cmajor::debug::GdbReplyRecord* stoppedRecord = reply->GetStoppedRecord();
        if (stoppedRecord)
        {
            if (stoppedRecord->GetKind() == cmajor::debug::GdbReplyRecord::Kind::execStopped)
            {
                GdbExecStoppedRecord* execStoppedRecord = static_cast<GdbExecStoppedRecord*>(stoppedRecord);
                success = ProcessExecStoppedRecord(execStoppedRecord);
            }
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful exec-continue command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessExecNextReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::running)
        {
            success = true;
        }
    }
    else
    {
        cmajor::debug::GdbReplyRecord* stoppedRecord = reply->GetStoppedRecord();
        if (stoppedRecord)
        {
            if (stoppedRecord->GetKind() == cmajor::debug::GdbReplyRecord::Kind::execStopped)
            {
                GdbExecStoppedRecord* execStoppedRecord = static_cast<GdbExecStoppedRecord*>(stoppedRecord);
                success = ProcessExecStoppedRecord(execStoppedRecord);
            }
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful exec-next command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessExecStepReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::running)
        {
            success = true;
        }
    }
    else
    {
        cmajor::debug::GdbReplyRecord* stoppedRecord = reply->GetStoppedRecord();
        if (stoppedRecord)
        {
            if (stoppedRecord->GetKind() == cmajor::debug::GdbReplyRecord::Kind::execStopped)
            {
                GdbExecStoppedRecord* execStoppedRecord = static_cast<GdbExecStoppedRecord*>(stoppedRecord);
                success = ProcessExecStoppedRecord(execStoppedRecord);
            }
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful exec-step command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessExecFinishReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::running)
        {
            success = true;
        }
    }
    else
    {
        cmajor::debug::GdbReplyRecord* stoppedRecord = reply->GetStoppedRecord();
        if (stoppedRecord)
        {
            if (stoppedRecord->GetKind() == cmajor::debug::GdbReplyRecord::Kind::execStopped)
            {
                GdbExecStoppedRecord* execStoppedRecord = static_cast<GdbExecStoppedRecord*>(stoppedRecord);
                success = ProcessExecStoppedRecord(execStoppedRecord);
            }
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful exec-finish command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessExecUntilReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::running)
        {
            success = true;
        }
    }
    else
    {
        cmajor::debug::GdbReplyRecord* stoppedRecord = reply->GetStoppedRecord();
        if (stoppedRecord)
        {
            if (stoppedRecord->GetKind() == cmajor::debug::GdbReplyRecord::Kind::execStopped)
            {
                GdbExecStoppedRecord* execStoppedRecord = static_cast<GdbExecStoppedRecord*>(stoppedRecord);
                success = ProcessExecStoppedRecord(execStoppedRecord);
            }
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful exec-until command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessStackInfoDepthReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::done)
        {
            success = GetStackDepthResult(resultRecord->Results(), result);
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful stack-info-depth command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessStackListFramesReply(GdbReply* reply)
{
    std::unique_ptr<JsonValue> cppStackResult(new JsonObject());
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::done)
        {
            std::unique_ptr<JsonObject> stackResult(new JsonObject());
            std::unique_ptr<JsonArray> framesArray(new JsonArray());
            std::vector<Frame> cppFrames;
            success = GetStackListFramesResult(resultRecord->Results(), cppStackResult, cppFrames);
            if (success)
            {
                for (const Frame& cppFrame : cppFrames)
                {
                    try
                    {
                        Instruction* instruction = debugInfo->GetInstruction(cppFrame, formatter);
                        if (instruction)
                        {
                            Frame cmajorFrame = instruction->GetCmajorFrame();
                            cmajorFrame.level = cppFrame.level;
                            framesArray->AddItem(cmajorFrame.ToJson(true));
                        }
                        else
                        {
                            Frame frame;
                            frame.level = cppFrame.level;
                            framesArray->AddItem(frame.ToJson(true));
                        }
                    }
                    catch (const std::exception&)
                    {
                        Frame frame;
                        frame.level = cppFrame.level;
                        framesArray->AddItem(frame.ToJson(true));
                    }
                }
                if (cppStackResult->Type() == JsonValueType::object)
                {
                    JsonObject* cppStackObject = static_cast<JsonObject*>(cppStackResult.get());
                    JsonValue* cppFramesValue = cppStackObject->GetField(U"cppFrames");
                    if (cppFramesValue)
                    {
                        stackResult->AddField(U"cppFrames", std::unique_ptr<JsonValue>(cppFramesValue->Clone()));
                    }
                }
                stackResult->AddField(U"frames", std::unique_ptr<JsonValue>(framesArray.release()));
                result.reset(stackResult.release());
            }
        }
    }
    if (!success)
    {
        result.reset(cppStackResult.release());
    }
}
void Debugger::ProcessVarCreateReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::done)
        {
            success = GetVarCreateResult(resultRecord->Results(), result, formatter);
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful var-create command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessVarEvaluateReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::done)
        {
            success = GetVarEvaluateResult(resultRecord->Results(), result, formatter);
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful var-evaluate command, reply: ");
        reply->Print(formatter);
    }
}

void Debugger::ProcessVarListChildrenReply(GdbReply* reply)
{
    bool success = false;
    cmajor::debug::GdbResultRecord* resultRecord = reply->GetResultRecord();
    if (resultRecord)
    {
        if (resultRecord->GetClass() == GdbResultRecord::Class::done)
        {
            success = GetVarListChildrenResult(resultRecord->Results(), result, formatter);
        }
    }
    if (!success)
    {
        std::lock_guard<std::recursive_mutex> lock(outputMutex);
        formatter.Write("unsuccessful var-list-children command, reply: ");
        reply->Print(formatter);
    }
}

bool Debugger::ProcessExecStoppedRecord(GdbExecStoppedRecord* execStoppedRecord)
{
    std::unique_ptr<JsonObject> stopObject;
    bool success = false;
    GdbResults* results = execStoppedRecord->Results();
    if (results)
    {
        Frame cppFrame;
        success = GetStoppedResults(results, stopObject, cppFrame);
        if (success)
        {
            JsonValue* reasonField = stopObject->GetField(U"reason");
            std::u32string reason;
            if (reasonField->Type() == JsonValueType::string)
            {
                reason = static_cast<JsonString*>(reasonField)->Value();
            }
            if (reason == U"exited-normally")
            {
                SetState(State::programExitedNormally);
            }
            else
            {
                Instruction* instruction = debugInfo->GetInstruction(cppFrame, formatter);
                if (instruction)
                {
                    stopObject->AddField(U"frame", instruction->GetCmajorFrame().ToJson(false));
                    stoppedInstruction = instruction;
                    SourceFile& sourceFile = stoppedInstruction->GetCompileUnitFunction()->GetSourceFile();
                    int sourceLine = std::min(int(sourceFile.Lines().size()), instruction->SourceLineNumber() + debugInfo->GetSourceFileWindowSize() + 1);
                    listLocation = SourceLocation(sourceFile.FilePath(), sourceLine);
                    currentSourceFilePath = sourceFile.FilePath();
                }
                else
                {
                    stoppedInstruction = nullptr;
                }
            }
        }
    }
    stopResult.reset(stopObject.release());
    return success;
}

void Debugger::Proceed()
{
    console.Proceed();
}

DebuggerCommand::DebuggerCommand(Kind kind_) : kind(kind_)
{
}

DebuggerCommand::~DebuggerCommand()
{
}

DebuggerExitCommand::DebuggerExitCommand() : DebuggerCommand(Kind::exit)
{
}

void DebuggerExitCommand::Execute(Debugger& debugger)
{
    debugger.Exit();
}

DebuggerCommand* DebuggerExitCommand::Clone()
{
    return new DebuggerExitCommand();
}

DebuggerHelpCommand::DebuggerHelpCommand() : DebuggerCommand(Kind::help)
{
}

void DebuggerHelpCommand::Execute(Debugger& debugger)
{
    debugger.Help();
}

DebuggerCommand* DebuggerHelpCommand::Clone()
{
    return new DebuggerHelpCommand();
}

DebuggerNextCommand::DebuggerNextCommand() : DebuggerCommand(Kind::next)
{
}

void DebuggerNextCommand::Execute(Debugger& debugger)
{
    debugger.Next();
}

DebuggerCommand* DebuggerNextCommand::Clone()
{
    return new DebuggerNextCommand();
}

DebuggerStepCommand::DebuggerStepCommand() : DebuggerCommand(Kind::step)
{
}

void DebuggerStepCommand::Execute(Debugger& debugger)
{
    debugger.Step();
}

DebuggerCommand* DebuggerStepCommand::Clone()
{
    return new DebuggerStepCommand();
}

DebuggerContinueCommand::DebuggerContinueCommand() : DebuggerCommand(Kind::continue_)
{
}

void DebuggerContinueCommand::Execute(Debugger& debugger)
{
    debugger.Continue();
}

DebuggerCommand* DebuggerContinueCommand::Clone()
{
    return new DebuggerContinueCommand();
}

DebuggerFinishCommand::DebuggerFinishCommand() : DebuggerCommand(Kind::finish)
{
}

void DebuggerFinishCommand::Execute(Debugger& debugger)
{
    debugger.Finish();
}

DebuggerCommand* DebuggerFinishCommand::Clone()
{
    return new DebuggerFinishCommand();
}

DebuggerUntilCommand::DebuggerUntilCommand(const SourceLocation& location_) : DebuggerCommand(Kind::until), location(location_)
{
}

void DebuggerUntilCommand::Execute(Debugger& debugger)
{
    debugger.Until(location);
}

DebuggerCommand* DebuggerUntilCommand::Clone()
{
    return new DebuggerUntilCommand(location);
}

DebuggerBreakCommand::DebuggerBreakCommand(const SourceLocation& location_) : DebuggerCommand(Kind::break_), location(location_)
{
}

void DebuggerBreakCommand::Execute(Debugger& debugger)
{
    debugger.Break(location);
}

DebuggerCommand* DebuggerBreakCommand::Clone()
{
    return new DebuggerBreakCommand(location);
}

DebuggerDeleteCommand::DebuggerDeleteCommand(int breakpointNumber_) : DebuggerCommand(Kind::delete_), breakpointNumber(breakpointNumber_)
{
}

void DebuggerDeleteCommand::Execute(Debugger& debugger)
{
    debugger.Delete(std::to_string(breakpointNumber));
}

DebuggerCommand* DebuggerDeleteCommand::Clone()
{
    return new DebuggerDeleteCommand(breakpointNumber);
}

DebuggerDepthCommand::DebuggerDepthCommand::DebuggerDepthCommand() : DebuggerCommand(Kind::depth)
{
}

void DebuggerDepthCommand::Execute(Debugger& debugger)
{
    debugger.Depth();
}

DebuggerFramesCommand::DebuggerFramesCommand(int low_, int high_) : DebuggerCommand(Kind::frames), low(low_), high(high_)
{
}

void DebuggerFramesCommand::Execute(Debugger& debugger)
{
    debugger.Frames(low, high);
}

DebuggerCommand* DebuggerFramesCommand::Clone()
{
    return new DebuggerFramesCommand(low, high);
}

DebuggerCommand* DebuggerDepthCommand::Clone()
{
    return new DebuggerDepthCommand();
}

DebuggerShowBreakpointsCommand::DebuggerShowBreakpointsCommand() : DebuggerCommand(Kind::showBreakpoints)
{
}

void DebuggerShowBreakpointsCommand::Execute(Debugger& debugger)
{
    debugger.ShowBreakpoints();
}

DebuggerCommand* DebuggerShowBreakpointsCommand::Clone()
{
    return new DebuggerShowBreakpointsCommand();
}

DebuggerListCommand::DebuggerListCommand(const SourceLocation& location_) : DebuggerCommand(Kind::list), location(location_)
{
}

void DebuggerListCommand::Execute(Debugger& debugger)
{
    debugger.List(location);
}

DebuggerCommand* DebuggerListCommand::Clone()
{
    return new DebuggerListCommand(location);
}

DebuggerPrintCommand::DebuggerPrintCommand(const std::string& expression_) : DebuggerCommand(Kind::print), expression(expression_)
{
}

void DebuggerPrintCommand::Execute(Debugger& debugger)
{
    debugger.Print(expression);
}

DebuggerCommand* DebuggerPrintCommand::Clone()
{
    return new DebuggerPrintCommand(expression);
}

DebuggerRepeatLatestCommand::DebuggerRepeatLatestCommand() : DebuggerCommand(Kind::repeatLatest)
{
}

bool DebuggerRepeatLatestCommand::IsRunningCommand(GdbDriver& driver) const
{
    return driver.LatestCommandWasRunningCommand();
}

void DebuggerRepeatLatestCommand::Execute(Debugger& debugger)
{
    debugger.RepeatLatestCommand();
}

DebuggerCommand* DebuggerRepeatLatestCommand::Clone()
{
    return new DebuggerRepeatLatestCommand();
}

void RunDebuggerInteractive(const std::string& executable, const std::vector<std::string>& args, bool verbose)
{
    CodeFormatter formatter(std::cout);
    Console console;
    Debugger debugger(executable, args, verbose, formatter, console);
    console.SetDriver(&debugger);
    std::thread consoleThread{ RunConsole, &console };
    try
    {
        while (!debugger.Exiting())
        {
            try
            {
                debugger.ResetRunningFlag();
                debugger.ResetTargetOutputFlag();
                std::unique_ptr<DebuggerCommand> command = console.GetCommand();
                if (!command || console.Terminated())
                {
                    break;
                }
                command->Execute(debugger);
                debugger.Proceed();
                if (command->GetKind() != DebuggerCommand::Kind::repeatLatest)
                {
                    debugger.SetLatestCommand(command->Clone());
                }
            }
            catch (const std::exception& ex)
            {
                formatter.WriteLine(ex.what());
                debugger.Proceed();
            }
        }
        consoleThread.join();
    }
    catch (const std::exception&)
    {
        consoleThread.join();
        throw;
    }
}

} } // namespace cmajor::debug
