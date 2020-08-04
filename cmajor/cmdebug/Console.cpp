// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/Console.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <string>
#include <iostream>

namespace cmajor { namespace debug {

Console::Console() :
    driver(nullptr), terminated(false), commandAvailable(false), commandReceived(false), targetRunning(false), canProceeed(false),
    needTargetInput(false), targetInputLineAvailable(false), targetInputEof(false)
{
}

void Console::SetDriver(GdbDriver* driver_)
{
    driver = driver_;
}

void Console::Run()
{
    std::string line;
    while (true)
    {
        std::istream& s = std::getline(std::cin, line);
        canProceeed = false;
        if (needTargetInput)
        {
            std::unique_lock<std::mutex> lock(mtx);
            targetInputLineReceived = false;
            if (!s)
            {
                targetInputEof = true;
            }
            else
            {
                targetInputEof = false;
            }
            targetInputLine = line;
            targetInputLineAvailable = true;
            targetInputLineAvailableVar.notify_one();
            targetInputLineReceivedVar.wait(lock, [this]{ return targetInputLineReceived; });
            needTargetInput = false;
        }
        else
        {
            if (!s)
            {
                break;
            }
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
        targetInputLineAvailable = false;
        commandAvailable = false;
        {
            std::unique_lock<std::mutex> lock(mtx);
            proceed.wait(lock, [this]{ return canProceeed; });
        }
        if (needTargetInput)
        {
            driver->TargetInputPrompt();
        }
        else
        {
            driver->Prompt();
        }
    }
    terminated = true;
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
    commandAvailableVar.wait(lock, [this]{ return commandAvailable; });
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

std::string Console::GetTargetInputLine()
{
    std::unique_lock<std::mutex> lock(mtx);
    targetInputLineAvailable = false;
    needTargetInput = true;
    canProceeed = true;
    proceed.notify_one();
    targetInputLineAvailableVar.wait(lock, [this]{ return targetInputLineAvailable; });
    std::string line = targetInputLine;
    targetInputLineReceived = true;
    targetInputLineReceivedVar.notify_one();
    return line;
}

void Console::Reset()
{
    std::unique_lock<std::mutex> lock(mtx);
    commandAvailable = false;
    commandReceived = false;
    targetRunning = true;
    targetRunningVar.notify_one();
}

void Console::Proceed()
{
    std::unique_lock<std::mutex> lock(mtx);
    canProceeed = true;
    proceed.notify_one();
}

} } // namespace cmajor::debug
