// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Process.hpp>
#include <cmajor/rt/String.hpp>
#include <soulng/util/Process.hpp>

void* RtCreateProcess(const char* command, int32_t redirections, int32_t& errorStringHandle)
{
    std::string cmd(command);
    errorStringHandle = -1;
    try
    {
        soulng::util::Process* process = new soulng::util::Process(cmd, static_cast<soulng::util::Process::Redirections>(redirections));
        return process;
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString("error creating process '" + cmd + "': " + ex.what());
    }
    return nullptr;
}

void RtDeleteProcess(void* process)
{
    delete static_cast<soulng::util::Process*>(process);
}

const char* RtGetString(int32_t stringHandle)
{
    return cmajor::rt::GetString(stringHandle);
}

void RtDisposeString(int32_t stringHandle)
{
    cmajor::rt::DisposeString(stringHandle);
}

bool RtProcessRunning(void* process, int32_t& errorStringHandle)
{
    try
    {
        errorStringHandle = -1;
        return static_cast<soulng::util::Process*>(process)->Running();
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtProcessWaitForExit(void* process, int32_t& errorStringHandle)
{
    try
    {
        errorStringHandle = -1;
        static_cast<soulng::util::Process*>(process)->WaitForExit();
        return true;
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

int RtProcessExitCode(void* process, int32_t& errorStringHandle)
{
    try
    {
        errorStringHandle = -1;
        return static_cast<soulng::util::Process*>(process)->ExitCode();
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString(ex.what());
        return 1;
    }
}

bool RtProcessTerminate(void* process, int32_t& errorStringHandle)
{
    try
    {
        errorStringHandle = -1;
        static_cast<soulng::util::Process*>(process)->Terminate();
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
    return true;
}

bool RtProcessEof(void* process, int handle, int32_t& errorStringHandle)
{
    try
    {
        errorStringHandle = -1;
        return static_cast<soulng::util::Process*>(process)->Eof(static_cast<soulng::util::Process::StdHandle>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString(ex.what());
        return true;
    }
}

int32_t RtProcessReadLine(void* process, int handle, int32_t& errorStringHandle)
{
    try
    {
        errorStringHandle = -1;
        std::string line = static_cast<soulng::util::Process*>(process)->ReadLine(static_cast<soulng::util::Process::StdHandle>(handle));
        return cmajor::rt::InstallString(line);
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString(ex.what());
        return -1;
    }
}

int32_t RtProcessReadToEnd(void* process, int handle, int32_t& errorStringHandle)
{
    try
    {
        errorStringHandle = -1;
        std::string str = static_cast<soulng::util::Process*>(process)->ReadToEnd(static_cast<soulng::util::Process::StdHandle>(handle));
        return cmajor::rt::InstallString(str);
    }
    catch (const std::exception& ex)
    {
        errorStringHandle = cmajor::rt::InstallString(ex.what());
        return -1;
    }
}

bool RtProcessWriteLine(void* process, const char* line, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        std::string lineStr(line);
        static_cast<soulng::util::Process*>(process)->WriteLine(lineStr);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
    return true;
}
