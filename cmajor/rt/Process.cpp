// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Process.hpp>
#include <cmajor/rt/String.hpp>
#include <soulng/util/Process.hpp>

void* RtCreateProcess(const char* command, int32_t& errorStringHandle)
{
    std::string cmd(command);
    errorStringHandle = -1;
    try
    {
        soulng::util::Process* process = new soulng::util::Process(cmd);
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

bool RtProcessRunning(void* process)
{
    return static_cast<soulng::util::Process*>(process)->Running();
}

void RtProcessWaitForExit(void* process)
{
    static_cast<soulng::util::Process*>(process)->WaitForExit();
}

int RtProcessExitCode(void* process)
{
    return static_cast<soulng::util::Process*>(process)->ExitCode();
}

bool RtProcessEof(void* process, int handle)
{
    return static_cast<soulng::util::Process*>(process)->Eof(static_cast<soulng::util::Process::StdHandle>(handle));
}

int32_t RtProcessReadLine(void* process, int handle)
{
    std::string line = static_cast<soulng::util::Process*>(process)->ReadLine(static_cast<soulng::util::Process::StdHandle>(handle));
    return cmajor::rt::InstallString(line);
}

int32_t RtProcessReadToEnd(void* process, int handle)
{
    std::string str = static_cast<soulng::util::Process*>(process)->ReadToEnd(static_cast<soulng::util::Process::StdHandle>(handle));
    return cmajor::rt::InstallString(str);
}

void RtProcessWriteLine(void* process, const char* line)
{
    std::string lineStr(line);
    static_cast<soulng::util::Process*>(process)->WriteLine(lineStr);
}
