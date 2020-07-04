// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/GdbReplyLexer.hpp>
#include <cmajor/cmdebug/GdbReplyLineParser.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <thread>
#include <memory>

namespace cmajor { namespace debug {

using namespace soulng::util;
using namespace soulng::unicode;

GdbCommand::GdbCommand(Kind kind_, const std::string& str_) : kind(kind_), str(str_)
{
}

GdbCommand::~GdbCommand()
{
}

GdbBreakInsertCommand::GdbBreakInsertCommand(const std::string& location) : GdbCommand(Kind::breakInsert, "-break-insert " + location)
{
}

GdbBreakDeleteCommand::GdbBreakDeleteCommand(const std::string& breakpoints) : GdbCommand(Kind::breakDelete, "-break-delete " + breakpoints)
{
}

GdbExecContinueCommand::GdbExecContinueCommand() : GdbCommand(Kind::execContinue, "-exec-continue")
{
}

GdbExecFinishCommand::GdbExecFinishCommand() : GdbCommand(Kind::execFinish, "-exec-finish")
{
}

GdbExecNextCommand::GdbExecNextCommand() : GdbCommand(Kind::execNext, "-exec-next")
{
}

GdbExecStepCommand::GdbExecStepCommand() : GdbCommand(Kind::execStep, "-exec-step")
{
}

GdbExecUntilCommand::GdbExecUntilCommand(const std::string& location) : GdbCommand(Kind::execUntil, "-exec-until " + location)
{
}

GdbExecRunCommand::GdbExecRunCommand() : GdbCommand(Kind::execRun, "-exec-run")
{
}

GdbExitCommand::GdbExitCommand() : GdbCommand(Kind::exit, "-gdb-exit")
{
}

GdbStackInfoDepthCommand::GdbStackInfoDepthCommand() : GdbCommand(Kind::stackInfoDepth, "-stack-info-depth")
{
}

std::string FrameStr(int lowFrame, int highFrame)
{
    if (lowFrame == -1 && highFrame == -1)
    {
        return std::string();
    }
    std::string s(1, ' ');
    s.append(std::to_string(lowFrame)).append(1, ' ').append(std::to_string(highFrame));
    return s;
}

GdbStackListFramesCommand::GdbStackListFramesCommand(int lowFrame, int highFrame) :
    GdbCommand(Kind::stackListFrames, "-stack-list-frames" + FrameStr(lowFrame, highFrame))
{
}

GdbValue::GdbValue(Kind kind_) : kind(kind_)
{
}

GdbValue::~GdbValue()
{
}

GdbStringValue::GdbStringValue(const std::string& value_) : GdbValue(Kind::string), value(value_)
{
}

void GdbStringValue::Print(CodeFormatter& formatter)
{
    formatter.Write("\"" + soulng::util::StringStr(value) + "\"");
}

std::string GdbStringValue::ToString() const
{
    return "\"" + soulng::util::StringStr(value) + "\"";
}

GdbTupleValue::GdbTupleValue() : GdbValue(Kind::tuple), results()
{
}

void GdbTupleValue::AddResult(GdbResult* result)
{
    fieldMap[result->Name()] = result->Value();
    results.push_back(std::unique_ptr<GdbResult>(result));
}

GdbValue* GdbTupleValue::GetField(const std::string& fieldName) const
{
    auto it = fieldMap.find(fieldName);
    if (it != fieldMap.cend())
    {
        GdbValue* value = it->second;
        return value;
    }
    else
    {
        return nullptr;
    }
}

void GdbTupleValue::Print(CodeFormatter& formatter)
{
    formatter.Write("{ ");
    bool first = true;
    for (const auto& result : results)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        result->Print(formatter);
    }
    formatter.Write(" }");
}

std::string GdbTupleValue::ToString() const
{
    std::string s = "{ ";
    bool first = true;
    for (const auto& result : results)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(result->ToString());
    }
    s.append(" }");
    return s;
}

GdbListValue::GdbListValue() : GdbValue(Kind::list)
{
}

void GdbListValue::AddValue(GdbValue* value)
{
    values.push_back(std::unique_ptr<GdbValue>(value));
}

void GdbListValue::Print(CodeFormatter& formatter)
{
    formatter.Write("[ ");
    bool first = true;
    for (const auto& value : values)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        value->Print(formatter);
    }
    formatter.Write(" ]");
}

std::string GdbListValue::ToString() const
{
    std::string s = "[ ";
    bool first = true;
    for (const auto& result : values)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            s.append(", ");
        }
        s.append(result->ToString());
    }
    s.append(" ]");
    return s;
}

GdbValue* GdbListValue::GetValue(int index) const
{
    if (index >= 0 && index < Count())
    {
        return values[index].get();
    }
    else
    {
        return nullptr;
    }
}

GdbResult::GdbResult(const std::string& name_, GdbValue* value_) : GdbValue(Kind::result), name(name_), value(value_)
{
}

void GdbResult::Print(CodeFormatter& formatter)
{
    formatter.Write(name);
    formatter.Write("=");
    value->Print(formatter);
}

std::string GdbResult::ToString() const
{
    std::string s = name;
    s.append("=").append(value->ToString());
    return s;
}

GdbResults::GdbResults()
{
}

void GdbResults::Print(CodeFormatter& formatter)
{
    int n = Count();
    if (n == 0) return;
    formatter.Write(",results=[");
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            formatter.Write(", ");
        }
        results[i]->Print(formatter);
    }
    formatter.Write("]");
}

void GdbResults::Add(GdbResult* result)
{
    results.push_back(std::unique_ptr<GdbResult>(result));
    fieldMap[result->Name()] = result->Value();
}

GdbValue* GdbResults::GetField(const std::string& fieldName) const
{
    auto it = fieldMap.find(fieldName);
    if (it != fieldMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

GdbReplyRecord::GdbReplyRecord(Kind kind_, GdbResults* results_) : kind(kind_), results(results_)
{
}

GdbReplyRecord::~GdbReplyRecord()
{
}

const char* GdbReplyRecord::KindStr() const
{
    switch (kind)
    {
        case Kind::result: return "result: ";
        case Kind::execRunning: return "execRunning";
        case Kind::execStopped: return "execStopped";
        case Kind::notification: return "notification";
        case Kind::consoleOutput: return "console output: ";
        case Kind::targetOutput: return "target output: ";
        case Kind::logOutput: return "log output: ";
        case Kind::prompt: return "prompt";
        case Kind::parsingError: return "parsing error: ";
    }
    return "";
}

void GdbReplyRecord::Print(CodeFormatter& formatter)
{
    formatter.Write(KindStr());
    results->Print(formatter);
    formatter.WriteLine();
}

GdbResultRecord::GdbResultRecord(Class cls_, GdbResults* results) : GdbReplyRecord(Kind::result, results), cls(cls_)
{
}

const char* GdbResultRecord::ClassStr() const
{
    switch (cls)
    {
        case Class::done: return "done";
        case Class::running: return "running";
        case Class::connected: return "connected";
        case Class::error: return "error";
        case Class::exit: return "exit";
    }
    return "";
}

void GdbResultRecord::Print(CodeFormatter& formatter)
{
    GdbReplyRecord::Print(formatter);
    formatter.Write(ClassStr());
}

GdbDoneRecord::GdbDoneRecord(GdbResults* results) : GdbResultRecord(Class::done, results)
{
}

GdbRunningRecord::GdbRunningRecord(GdbResults* results) : GdbResultRecord(Class::running, results)
{
}

GdbConnectedRecord::GdbConnectedRecord(GdbResults* results) : GdbResultRecord(Class::connected, results)
{
}

GdbErrorRecord::GdbErrorRecord(GdbResults* results) : GdbResultRecord(Class::error, results)
{
}

std::string GdbErrorRecord::Msg() const
{
    GdbValue* value = Results()->GetField("msg");
    if (value != nullptr)
    {
        return value->ToString();
    }
    return std::string();
}

std::string GdbErrorRecord::Code() const
{
    GdbValue* value = Results()->GetField("code");
    if (value != nullptr)
    {
        return value->ToString();
    }
    return std::string();
}

GdbExitRecord::GdbExitRecord(GdbResults* results) : GdbResultRecord(Class::exit, results)
{
}

GdbAsyncRecord::GdbAsyncRecord(Kind kind, GdbResults* results) : GdbReplyRecord(kind, results)
{
}

GdbExecRecord::GdbExecRecord(Kind kind, GdbResults* results) : GdbAsyncRecord(kind, results)
{
}

GdbExecRunningRecord::GdbExecRunningRecord(GdbResults* results) : GdbExecRecord(Kind::execRunning, results)
{
}

GdbExecStoppedRecord::GdbExecStoppedRecord(GdbResults* results) : GdbExecRecord(Kind::execStopped, results)
{
}

GdbNotifyAsyncRecord::GdbNotifyAsyncRecord(const std::string& notification_, GdbResults* results) : GdbAsyncRecord(Kind::notification, results)
{
}

void GdbNotifyAsyncRecord::Print(CodeFormatter& formatter)
{
    GdbAsyncRecord::Print(formatter);
    formatter.WriteLine(notification);
}

GdbStreamRecord::GdbStreamRecord(Kind kind, const std::string& text_) : GdbReplyRecord(kind, new GdbResults()), text(text_)
{
}

void GdbStreamRecord::Print(CodeFormatter& formatter)
{
    GdbReplyRecord::Print(formatter);
    formatter.WriteLine("\"" + StringStr(text) + "\"");
}

GdbConsoleOutputRecord::GdbConsoleOutputRecord(const std::string& text) : GdbStreamRecord(Kind::consoleOutput, text)
{
}

GdbTargetOutputRecord::GdbTargetOutputRecord(const std::string& text) : GdbStreamRecord(Kind::targetOutput, text)
{
}

GdbLogOutputRecord::GdbLogOutputRecord(const std::string& text) : GdbStreamRecord(Kind::logOutput, text)
{
}

GdbPrompt::GdbPrompt() : GdbReplyRecord(Kind::prompt, new GdbResults())
{
}

GdbParsingError::GdbParsingError(const std::string& parsingError_) : GdbReplyRecord(Kind::parsingError, new GdbResults()), parsingError(parsingError_)
{
}

void GdbParsingError::Print(CodeFormatter& formatter)
{
    GdbReplyRecord::Print(formatter);
    formatter.WriteLine(parsingError);
}

GdbReply::GdbReply() : resultRecord(nullptr), stoppedRecord(nullptr)
{
}

void GdbReply::SetTextLines(const std::vector<std::string>& textLines_)
{
    textLines = textLines_;
}

void GdbReply::AddReplyRecord(std::unique_ptr<GdbReplyRecord>&& replyRecord)
{
    if (replyRecord->GetKind() == GdbReplyRecord::Kind::result)
    {
        resultRecord = static_cast<GdbResultRecord*>(replyRecord.get());
    }
    if (replyRecord->Stopped())
    {
        stoppedRecord = replyRecord.get();
    }
    replyRecords.push_back(std::move(replyRecord));
}

void GdbReply::Print(CodeFormatter& formatter)
{
    for (const auto& replyRecord : replyRecords)
    {
        replyRecord->Print(formatter);
    }
}

GdbReply::~GdbReply()
{
}

class Gdb
{
public:
    static void Init();
    static void Done();
    static Gdb& Instance() { return *instance; }
    void SetDebugFlag() { debug = true; }
    void Start(const std::string& executable, const std::vector<std::string>& args);
    void Run(const std::string& startCommand);
    void Stop();
    std::unique_ptr<GdbReply> Execute(const GdbCommand& commmand);
    std::unique_ptr<GdbReply> ReadReply();
    GdbReply* GetStartReply() { return startReply.get(); }
private:
    Gdb();
    bool debug;
    static std::unique_ptr<Gdb> instance;
    std::unique_ptr<soulng::util::Process> gdb;
    std::unique_ptr<GdbReply> startReply;
    std::thread gdbThread;
    std::exception_ptr gdbException;
    int gdbExitCode;
};

void RunGDB(Gdb* gdb, const std::string& startCommand)
{
    gdb->Run(startCommand);
}

std::unique_ptr<Gdb> Gdb::instance;

void Gdb::Init()
{
    instance.reset(new Gdb());
}

void Gdb::Done()
{
    instance.reset();
}

Gdb::Gdb() : debug(false), gdbExitCode(-1)
{
}

void Gdb::Start(const std::string& executable, const std::vector<std::string>& args)
{
    std::string startCommand;
    startCommand.append("gdb");
    startCommand.append(" --interpreter=mi");
    if (!args.empty())
    {
        startCommand.append(" --args");
        startCommand.append(" \"").append(executable).append("\"");
        for (const std::string& arg : args)
        {
            startCommand.append(" \"").append(arg).append("\"");
        }
    }
    else
    {
        startCommand.append(" \"").append(executable).append("\"");
    }
    gdbThread = std::thread{ RunGDB, this, startCommand };
    std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    startReply = ReadReply();
}

void Gdb::Run(const std::string& startCommand)
{
    try
    {
        gdb.reset(new soulng::util::Process(startCommand,
            soulng::util::Process::Redirections::processStdIn |
            soulng::util::Process::Redirections::processStdOut |
            soulng::util::Process::Redirections::processStdErr));
    }
    catch (...)
    {
        gdbException = std::current_exception();
    }
}

void Gdb::Stop()
{
    GdbExitCommand exitCommand;
    Execute(exitCommand);
    gdb->WaitForExit();
    gdbThread.join();
    gdbExitCode = gdb->ExitCode();
}

std::unique_ptr<GdbReply> Gdb::Execute(const GdbCommand& command)
{
    if (gdbException)
    {
        std::rethrow_exception(gdbException);
    }
    if (debug)
    {
        LogMessage(-1, "-> " + command.Str());
    }
    gdb->WriteLine(command.Str());
    std::unique_ptr<GdbReply> reply;
    if (command.GetKind() != GdbCommand::Kind::exit)
    {
        reply = ReadReply();
    }
    if (gdbException)
    {
        std::rethrow_exception(gdbException);
    }
    return reply;
}

std::unique_ptr<GdbReplyRecord> ParseGdbReplyRecord(const std::string& line)
{
    std::string lineNL = line;
    lineNL.append("\r\n");
    GdbReplyLexer lexer(ToUtf32(lineNL), "", 0);
    try
    {
        std::unique_ptr<GdbReplyRecord> replyRecord = GdbReplyLineParser::Parse(lexer);
        return replyRecord;
    }
    catch (const std::exception& ex)
    {
        return std::unique_ptr<GdbParsingError>(new GdbParsingError(ex.what()));
    }
}

std::unique_ptr<GdbReply> Gdb::ReadReply()
{
    std::unique_ptr<GdbReply> reply(new GdbReply());
    std::vector<std::string> textLines;
    std::string line = gdb->ReadLine(soulng::util::Process::StdHandle::stdOut);
    while (line.empty())
    {
        line = gdb->ReadLine(soulng::util::Process::StdHandle::stdOut);
    }
    if (debug)
    {
        LogMessage(-1, "<- " + line);
    }
    if (!line.empty())
    {
        switch (line[0])
        {
            case '=': case '~': case '^': case '*': case '&':
            {
                break;
            }
            default:
            {
                line = "@\"" + soulng::util::StringStr(line) + "\"";
                break;
            }
        }
    }
    std::unique_ptr<GdbReplyRecord> replyRecord = ParseGdbReplyRecord(line);
    while (replyRecord->GetKind() != GdbReplyRecord::Kind::prompt)
    {
        if (replyRecord->GetKind() == GdbReplyRecord::Kind::parsingError)
        {
            GdbParsingError* parsingError = static_cast<GdbParsingError*>(replyRecord.get());
            LogMessage(-1, "error parsing GDB reply '" + line + "': " + parsingError->ParsingError());
        }
        textLines.push_back(line);
        reply->AddReplyRecord(std::move(replyRecord));
        line = gdb->ReadLine(soulng::util::Process::StdHandle::stdOut);
        if (debug)
        {
            LogMessage(-1, "<- " + line);
        }
        replyRecord = ParseGdbReplyRecord(line);
    }
    reply->SetTextLines(textLines);
    return reply;
}

void SetDebugFlag()
{
    Gdb::Instance().SetDebugFlag();
}

void StartGDB(const std::string& executable, const std::vector<std::string>& args)
{
    Gdb::Instance().Start(executable, args);
}

GdbReply* GetGDBStartReply()
{
    return Gdb::Instance().GetStartReply();
}

std::unique_ptr<GdbReply> ExecuteGDBCommand(const GdbCommand& command)
{
    return Gdb::Instance().Execute(command);
}

std::unique_ptr<GdbReply> ReadGDBReply()
{
    return Gdb::Instance().ReadReply();
}

void StopGDB()
{
    Gdb::Instance().Stop();
}

void InitGDB()
{
    Gdb::Init();
}

void DoneGDB()
{
    Gdb::Done();
}

} } // namespace cmajor::debug
