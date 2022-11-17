// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Process.hpp>
#include <cmajor/rts/String.hpp>
#include <soulng/util/Process.hpp>

void* RtCreateProcess(const char* command, int32_t redirections, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return nullptr;
}

void RtDeleteProcess(void* process)
{
#ifndef __MINGW32__
    delete static_cast<soulng::util::Process*>(process);
#endif
}

const char* RtGetString(int32_t stringHandle)
{
#ifndef __MINGW32__
    return cmajor::rt::GetString(stringHandle);
#endif
    return nullptr;
}

void RtDisposeString(int32_t stringHandle)
{
#ifndef __MINGW32__
    cmajor::rt::DisposeString(stringHandle);
#endif
}

bool RtProcessRunning(void* process, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return false;
}

bool RtProcessWaitForExit(void* process, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return false;
}

int RtProcessExitCode(void* process, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return 0;
}

bool RtProcessTerminate(void* process, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return false;
}

bool RtProcessEof(void* process, int handle, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return false;
}

int32_t RtProcessReadLine(void* process, int handle, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return -1;
}

int32_t RtProcessReadToEnd(void* process, int handle, int32_t& errorStringHandle)
{
#ifndef __MINGW32__
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
#endif
    return -1;
}

bool RtProcessWriteLine(void* process, const char* line, int32_t& errorStrHandle)
{
#ifndef __MINGW32__
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
#endif
    return false;
}
