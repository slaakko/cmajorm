// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/RunService.hpp>
#include <cmajor/cmsvc/DebugService.hpp>
#include <cmajor/cmsvc/PortMapService.hpp>
#include <cmajor/cmdebug/CmdbSession.hpp>
#include <cmajor/cmdebug/DebuggerOutputWriter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/TextUtils.hpp>
#include <thread>
#include <condition_variable>
#include <mutex>

namespace cmajor { namespace service {

using namespace cmajor::debug;
using namespace soulng::util;

class RunService;

class RunOutputWriter : public DebuggerOutputWriter
{
public:
    RunOutputWriter(RunService* runService_);
    void WriteLogMessage(const std::string& message) override;
    void WriteJsonValue(soulng::util::JsonValue* jsonValue) override;
    void WriteWarning(const std::string& warning) override;
private:
    RunService* runService;
};

RunOutputWriter::RunOutputWriter(RunService* runService_) : runService(runService_)
{
}

void RunOutputWriter::WriteLogMessage(const std::string& message)
{
    PutOutputServiceMessage(message);
}

void RunOutputWriter::WriteJsonValue(soulng::util::JsonValue* jsonValue)
{
}

void RunOutputWriter::WriteWarning(const std::string& warning)
{
    PutOutputServiceMessage("warning: " + warning);
}

class RunService : public CmdbSessionClient
{
public:
    static void Init();
    static void Done();
    static RunService& Instance() { return *instance; }
    void Start(const std::string& executableName, const std::string& programArguments, const std::string& processName);
    void Run();
    void TerminateProcess();
    void Stop();
    std::string GetTargetInputBytes() override;
    void WriteTargetOuput(int handle, const std::string& s) override;
    void SetInputEof();
    void PutInputLine(const std::string& inputLine);
private:
    RunService();
    static std::unique_ptr<RunService> instance;
    RunOutputWriter outputWriter;
    std::unique_ptr<soulng::util::Process> process;
    std::thread runThread;
    std::string startCommand;
    std::string cmdbSessionFilePath;
    int sessionPort;
    bool runThreadStarted;
    bool inputEof;
    bool waitingForInput;
    bool exiting;
    std::mutex mtx;
    std::condition_variable waitingForInputVar;
    std::list<std::string> inputLines;
};

std::unique_ptr<RunService> RunService::instance;

void RunService::Init()
{
    instance.reset(new RunService());
}

void RunService::Done()
{
    instance.reset();
}

RunService::RunService() : sessionPort(0), outputWriter(this), runThreadStarted(false), inputEof(false), waitingForInput(false), exiting(false)
{
}

void DoRun(RunService* service)
{
    service->Run();
}

void RunService::Start(const std::string& executableName, const std::string& programArguments, const std::string& processName)
{
    exiting = false;
    inputEof = false;
    cmdbSessionFilePath = Path::ChangeExtension(executableName, ".cmdbs");
    sessionPort = GetFreePortNumber(processName);
    if (sessionPort == -1)
    {
        sessionPort = 54322;
    }
    startCommand = executableName;
    startCommand.append(1, ' ').append(programArguments);
    runThreadStarted = false;
    runThread = std::thread(DoRun, this);
    runThreadStarted = true;
}

void RunService::Run()
{
    try
    {
        SetCmdbSessionPort(sessionPort);
        StartCmdbSession(cmdbSessionFilePath, &outputWriter, this, true);
        RemoveCmdbSessionFileGuard removeFileGuard(cmdbSessionFilePath);
        process.reset(new Process(startCommand, Process::Redirections::none));
        process->WaitForExit();
        int exitCode = process->ExitCode();
        process.reset();
        PutServiceMessage(new ProcessTerminatedServiceMessage());
        PutOutputServiceMessage("process exited with code " + std::to_string(exitCode));
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("run service: error: " + std::string(ex.what()));
    }
}

void RunService::TerminateProcess()
{
    if (process)
    {
        if (process->Running())
        {
            process->Terminate();
        }
    }
}

void RunService::Stop()
{
    try
    {
        exiting = true;
        if (waitingForInput)
        {
            waitingForInputVar.notify_one();
        }
        if (process)
        {
            if (process->Running())
            {
                process->Terminate();
            }
        }
        if (runThreadStarted)
        {
            runThread.join(); 
            runThreadStarted = false;
        }
        StopCmdbSession();
        PutServiceMessage(new RunServiceStoppedServiceMessage());
    }
    catch (const std::exception& ex)
    {
        PutOutputServiceMessage("runservice stop: error: " + std::string(ex.what()));
    }
}

void RunService::SetInputEof()
{
    inputEof = true;
    if (waitingForInput)
    {
        waitingForInputVar.notify_one();
    }
}

void RunService::PutInputLine(const std::string& inputLine)
{
    std::lock_guard<std::mutex> lock(mtx);
    inputLines.push_back(inputLine);
    if (waitingForInput)
    {
        waitingForInputVar.notify_one();
    }
}

std::string RunService::GetTargetInputBytes()
{
    std::unique_lock<std::mutex> lock(mtx);
    if (inputEof)
    {
        return std::string();
    }
    std::string targetInputLine;
    if (!inputLines.empty())
    {
        targetInputLine = std::move(inputLines.front());
        inputLines.pop_front();
    }
    else
    {
        waitingForInput = true;
        waitingForInputVar.wait(lock, [this]{ return !inputLines.empty() || inputEof || exiting; });
        if (exiting || inputEof)
        {
            return std::string();
        }
        targetInputLine = std::move(inputLines.front());
        inputLines.pop_front();
    }
    std::string targetInputBytes;
    for (char c : targetInputLine)
    {
        uint8_t x = static_cast<uint8_t>(c);
        targetInputBytes.append(ToHexString(x)); 
    }
    targetInputBytes.append(ToHexString(static_cast<uint8_t>('\n')));
    return targetInputBytes;
}

void RunService::WriteTargetOuput(int handle, const std::string& s)
{
    TargetOutputRequest outputRequest;
    outputRequest.handle = handle;
    outputRequest.output = s;
    TargetOutputServiceMessage outputMessage(outputRequest);
    PutServiceMessage(new TargetOutputServiceMessage(outputMessage));
}

void StartProgram(const std::string& executableName, const std::string& programArguments, const std::string& processName)
{
    RunService::Instance().Start(executableName, programArguments, processName);
}

void TerminateProcess()
{
    RunService::Instance().TerminateProcess();
}

void StopRunService()
{
    RunService::Instance().Stop();
}

ProcessTerminatedServiceMessage::ProcessTerminatedServiceMessage() : ServiceMessage(ServiceMessageKind::processTerminated)
{
}

RunServiceStoppedServiceMessage::RunServiceStoppedServiceMessage() : ServiceMessage(ServiceMessageKind::runServiceStopped)
{
}

void SetProgramTargetInputEof()
{
    RunService::Instance().SetInputEof();
}

void PutProgramTargetInputLine(const std::string& targetInputLine)
{
    RunService::Instance().PutInputLine(targetInputLine);
}

void InitRunService()
{
    RunService::Init();
}

void DoneRunService()
{
    RunService::Done();
}

} } // namespace cmajor::service
