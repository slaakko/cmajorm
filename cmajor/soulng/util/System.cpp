// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/System.hpp>
#include <soulng/util/Handle.hpp>
#include <soulng/util/TextUtils.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <memory>
#ifdef _WIN32
#include <process.h>
#include <windows.h>
#include <io.h>
#include <Psapi.h>
#elif defined(__linux) || defined(__posix) || defined(__unix)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/select.h>
#endif

namespace soulng { namespace util {

bool disableConsoleWindow = false;

void DisableConsoleWindow()
{
    disableConsoleWindow = true;
}

ProcessFailure::ProcessFailure(const std::string& errorMessage_, int exitCode_) : std::runtime_error(errorMessage_), exitCode(exitCode_)
{
}

#ifdef _WIN32

int dup2(int oldFd, int newFd)
{
    return _dup2(oldFd, newFd);
}

int pipe(int pipefd[2])
{
    unsigned int psize = 4096;
    int textmode = _O_BINARY;
    return _pipe(pipefd, psize, textmode);
}

#endif

int get_default_pmode()
{
#if defined(_WIN32)

    return S_IREAD | S_IWRITE;

#elif defined(__linux) || defined(__unix) || defined(__posix)

    return S_IRUSR | S_IWUSR | S_IRGRP;

#else

#error unknown platform

#endif
}

std::vector<std::string> ParseCommand(const std::string& command)
{
    std::vector<std::string> args;
    int state = 0;
    std::string arg;
    for (char c : command)
    {
        switch (state)
        {
            case 0:
            {
                if (c == '"')
                {
                    arg.append(1, '"');
                    state = 1;
                }
                else if (c == ' ')
                {
                    args.push_back(arg);
                    arg.clear();
                    state = 2;
                }
                else
                {
                    arg.append(1, c);
                }
                break;
            }
            case 1:
            {
                if (c == '"')
                {
                    arg.append(1, '"');
                    state = 0;
                }
                else
                {
                    arg.append(1, c);
                }
                break;
            }
            case 2:
            {
                if (c != ' ')
                {
                    arg.append(1, c);
                    state = 0;
                }
                break;
            }
        }
    }
    if (!arg.empty())
    {
        args.push_back(arg);
    }
    return args;
}

void System(const std::string& command, bool ignoreReturnValue)
{
    int retVal = 0;
    if (!disableConsoleWindow)
    {
        retVal = system(command.c_str());
    }
    else
    {
#ifdef _WIN32
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        int n = command.length();
        std::unique_ptr<char> c(new char[n + 1]);
        for (int i = 0; i < n; ++i)
        {
            c.get()[i] = command[i];
        }
        c.get()[n] = '\0';
        if (CreateProcess(NULL, c.get(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD exitCode;
            if (GetExitCodeProcess(pi.hProcess, &exitCode))
            {
                retVal = exitCode;
            }
            else
            {
                retVal = 1;
            }
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else
        {
            retVal = 1;
        }

#elif defined(__linux) || defined(__unix) || defined(__posix)
        retVal = system(command.c_str());
#else

#error unknown platform

#endif
    }
    if (!ignoreReturnValue)
    {
        if (retVal != 0)
        {
#ifdef _WIN32

            throw ProcessFailure("'" + command + "' returned exit code " + std::to_string(retVal), retVal);

#elif defined(__linux) || defined(__unix) || defined(__posix)

            if (WIFEXITED(retVal))
            {
                throw ProcessFailure("'" + command + "' returned exit code " + std::to_string(WEXITSTATUS(retVal)), WEXITSTATUS(retVal));
            }
            else
            {
                throw ProcessFailure("'" + command + "' terminated abnormally, status = " + std::to_string(retVal), retVal);
            }
#else

#error unknown platform

#endif
        }
    }
}

void System(const std::string& command)
{
    return System(command, false);
}

void System(const std::string& command, int redirectFd, const std::string& toFile, bool ignoreReturnValue)
{
    Handle old = dup(redirectFd);
    if (old == -1)
    {
        throw std::runtime_error("System redirect: could not duplicate handle " + std::to_string(redirectFd));
    }
    Handle fd = creat(toFile.c_str(), get_default_pmode());
    if (fd == -1)
    {
        throw std::runtime_error("System: could not create file '" + toFile + "'");
    }
    if (dup2(fd, redirectFd) == -1)
    {
        throw std::runtime_error("System redirect: dup2 failed");
    }
    try
    {
        System(command, ignoreReturnValue);
        dup2(old, redirectFd);
    }
    catch (...)
    {
        dup2(old, redirectFd);
        throw;
    }
}

void System(const std::string& command, int redirectFd, const std::string& toFile)
{
    System(command, redirectFd, toFile, false);
}

void System(const std::string& command, const std::vector<std::pair<int, std::string>>& redirections)
{
    std::vector<std::pair<int, Handle>> toRestore;
    for (const std::pair<int, std::string>& redirection : redirections)
    {
        int handle = redirection.first;
        std::string toFile = redirection.second;
        Handle oldHandle = dup(handle);
        if (oldHandle == -1)
        {
            throw std::runtime_error("System redirect: could not duplicate handle " + std::to_string(handle));
        }
        toRestore.push_back(std::make_pair(handle, std::move(oldHandle)));
        Handle fd = creat(toFile.c_str(), get_default_pmode());
        if (fd == -1)
        {
            throw std::runtime_error("System: could not create file '" + toFile + "'");
        }
        if (dup2(fd, handle) == -1)
        {
            throw std::runtime_error("System redirect: dup2 failed");
        }
    }
    try
    {
        System(command);
        for (std::pair<int, Handle>& r : toRestore)
        {
            int handle = r.first;
            Handle old = std::move(r.second);
            dup2(old, handle);
        }
    }
    catch (...)
    {
        for (std::pair<int, Handle>& r : toRestore)
        {
            int handle = r.first;
            Handle old = std::move(r.second);
            dup2(old, handle);
        }
        throw;
    }
}

#ifdef _WIN32

unsigned long long Spawn(const std::string& filename, const std::vector<std::string>& args)
{
    if (args.size() > 1023)
    {
        throw std::runtime_error("spawn: too many args");
    }
    const char* argList[1024];
    int a = 0;
    argList[a++] = filename.c_str();
    for (int i = 0; i < args.size(); ++i)
    {
        argList[a++] = args[i].c_str();
    }
    argList[a] = NULL;
    intptr_t handle = _spawnvp(_P_NOWAIT, filename.c_str(), argList);
    if (handle == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("spawn: " + error);
    }
    return handle;
}

int Wait(unsigned long long processHandle)
{
    int exitCode = 0;
    intptr_t result = _cwait(&exitCode, processHandle, 0);
    if (result == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("wait: " + error);
    }
    return exitCode;
}

#elif defined(__linux) || defined(__posix) || defined(__unix)

unsigned long long Spawn(const std::string& filename, const std::vector<std::string>& args)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("fork: " + error);
    }
    if (pid == 0)   // in child...
    {
        std::string command = filename;
        for (const std::string& arg : args)
        {
            command.append(" ").append(arg);
        }
        System(command);
        exit(0);
    }
    else // in parent...
    {
        return pid;
    }
}

int Wait(unsigned long long)
{
    int exitCode = 0;
    pid_t pid = wait(&exitCode);
    if (pid == -1)
    {
        std::string error = strerror(errno);
        throw std::runtime_error("wait: " + error);
    }
    return exitCode;
}

#endif 

int ReadFromPipe(int pipeHandle, void* buffer, unsigned int count)
{
    return read(pipeHandle, buffer, count);
}

int WriteToPipe(int pipeHandle, void* buffer, unsigned int count)
{
    return write(pipeHandle, buffer, count);
}

void RedirectStdHandlesToPipes(std::vector<int>& oldHandles, std::vector<int>& pipeHandles)
{
    int phRead[2];
    if (pipe(phRead) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: pipe failed");
    }
    int old0 = dup(0);
    if (old0 == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup(0) failed");
    }
    oldHandles.push_back(old0);
    if (dup2(phRead[0], 0) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup2(p0, 0) failed");
    }
    pipeHandles.push_back(phRead[1]);
    close(phRead[0]);
    int phWrite[2];
    if (pipe(phWrite) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: pipe failed");
    }
    int old1 = dup(1);
    if (old1 == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup(1) failed");
    }
    oldHandles.push_back(old1);
    if (dup2(phWrite[1], 1) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup2(p1, 1) failed");
    }
    pipeHandles.push_back(phWrite[0]);
    close(phWrite[1]);
    int phError[2];
    if (pipe(phError) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: pipe failed");
    }
    int old2 = dup(2);
    if (old2 == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup(2) failed");
    }
    oldHandles.push_back(old2);
    if (dup2(phError[1], 2) == -1)
    {
        throw std::runtime_error("RedirectStdHandlesToPipes: dup2(p2, 2) failed");
    }
    pipeHandles.push_back(phError[0]);
    close(phError[1]);
}

void RestoreStdHandles(const std::vector<int>& oldHandles)
{
    if (oldHandles.size() != 3)
    {
        throw std::runtime_error("3 old handles expected ");
    }
    if (dup2(oldHandles[0], 0) == -1)
    {
        throw std::runtime_error("RestoreStdHandles: dup2 0, old0 failed");
    }
    if (dup2(oldHandles[1], 1) == -1)
    {
        throw std::runtime_error("RestoreStdHandles: dup2 1, old1 failed");
    }
    if (dup2(oldHandles[2], 2) == -1)
    {
        throw std::runtime_error("RestoreStdHandles: dup2 2, old2 failed");
    }
}

std::string GetPathToExecutable()
{
    char buf[4096];
#ifdef _WIN32
    DWORD result = GetModuleFileName(NULL, buf, sizeof(buf));
    if (result == 0)
    {
        throw std::runtime_error("could not get path to current executable: GetModuleFileName failed");
    }
    return std::string(buf);

#elif defined(__linux) || defined(__unix) || defined(__posix)

    int result = readlink("/proc/self/exe", buf, sizeof(buf));
    if (result == -1)
    {
        throw std::runtime_error("could not get path to current executable: " + std::string(strerror(errno)));
    }
    std::string s;
    for (int i = 0; i < result; ++i)
    {
        s.append(1, buf[i]);
    }
    return s;
#else

#error unknown platform

#endif
}

} } // namespace soulng::util
