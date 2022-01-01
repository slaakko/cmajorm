// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Environment.hpp>
#include <cmajor/rts/Error.hpp>
#include <cmajor/rts/String.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/System.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <process.h>
#else
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#endif

namespace cmajor { namespace rt {

struct Exec
{
    std::string command;
    std::vector<std::pair<int, std::string>> redirections;
};

struct Handle
{
    Handle() : handle(-1) { }
    Handle(int handle_) : handle(handle_) { }
    Handle(Handle&& that) : handle(that.handle)
    {
        that.handle = -1;
    }
    operator int() const
    {
        return handle;
    }
    ~Handle()
    {
        if (handle != -1)
        {
            close(handle);
        }
    }
    int handle;
};

class Executor
{
public:
    static void Init();
    static void Done();
    static Executor& Instance() { return *instance; }
    int32_t BeginExecute(const char* command);
    void AddRedirection(int32_t execHandle, int handle, const char* file);
    int32_t EndExecute(int32_t execHandle);
private:
    static std::unique_ptr<Executor> instance;
    Executor();
    std::mutex mtx;
    int32_t nextExecHandle;
    std::unordered_map<int32_t, std::unique_ptr<Exec>> execMap;
    int32_t Execute(Exec* exec);
};

std::unique_ptr<Executor> Executor::instance;

void Executor::Init()
{
    instance.reset(new Executor());
}

void Executor::Done()
{
    instance.reset();
}

Executor::Executor() : nextExecHandle(1)
{
}

int32_t Executor::BeginExecute(const char* command)
{
    std::lock_guard<std::mutex> lock(mtx);
    int32_t execHandle = nextExecHandle++;
    std::unique_ptr<Exec> exec(new Exec());
    exec->command = command;
    execMap[execHandle] = std::move(exec);
    return execHandle;
}

void Executor::AddRedirection(int32_t execHandle, int handle, const char* file)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = execMap.find(execHandle);
    if (it != execMap.cend())
    {
        Exec* exec = it->second.get();
        exec->redirections.push_back(std::make_pair(handle, std::string(file)));
    }
}

int32_t Executor::EndExecute(int32_t execHandle)
{
    Exec* exec = nullptr;
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = execMap.find(execHandle);
        if (it != execMap.cend())
        {
            exec = it->second.get();
        }
    }
    if (exec)
    {
        int32_t exitCode = Execute(exec);
        std::lock_guard<std::mutex> lock(mtx);
        execMap.erase(execHandle);
        return exitCode;
    }
    else
    {
        return 1;
    }
}

#ifdef _WIN32

int32_t Executor::Execute(Exec* exec)
{
    std::vector<std::pair<int, Handle>> toRestore;
    for (const std::pair<int, std::string>& p : exec->redirections)
    {
        int handle = p.first;
        std::string file = p.second;
        Handle oldHandle = _dup(handle);
        if (oldHandle != -1)
        {
            toRestore.push_back(std::make_pair(handle, std::move(oldHandle)));
#if defined(__MINGW32__)
            int pmode = 0x0100 | 0x0080;
#else
            int pmode = _S_IREAD | _S_IWRITE;
#endif
            Handle fd = _creat(file.c_str(), pmode);
            if (fd != -1)
            {
                _dup2(fd, handle);
            }
        }
    }
    int32_t exitCode = system(exec->command.c_str());
    for (std::pair<int, Handle>& p : toRestore)
    {
        int handle = p.first;
        Handle old = std::move(p.second);
        _dup2(old, handle);
    }
    return exitCode;
}

#else

int32_t Executor::Execute(Exec* exec)
{
    std::vector<std::pair<int, Handle>> toRestore;
    for (const std::pair<int, std::string>& p : exec->redirections)
    {
        int handle = p.first;
        std::string file = p.second;
        Handle oldHandle = dup(handle);
        if (oldHandle != -1)
        {
            toRestore.push_back(std::make_pair(handle, std::move(oldHandle)));
            int pmode = S_IRUSR | S_IWUSR | S_IRGRP;
            Handle fd = creat(file.c_str(), pmode);
            if (fd != -1)
            {
                dup2(fd, handle);
            }
        }
    }
    int32_t exitCode = system(exec->command.c_str());
    for (std::pair<int, Handle>& p : toRestore)
    {
        int handle = p.first;
        Handle old = std::move(p.second);
        dup2(old, handle);
    }
    return exitCode;
}

#endif

void InitEnvironment()
{
    Executor::Init();
}

void DoneEnvironment()
{
    Executor::Done();
}

} } // namespace cmajor::rt

extern "C" RT_API const char* RtGetEnvironmentVariable(const char* environmentVariableName)
{
    const char* envVar = std::getenv(environmentVariableName);
    if (envVar)
    {
        return envVar;
    }
    return "";
}

std::mutex mtx;

extern "C" RT_API int32_t RtGetCurrentWorkingDirectoryHandle()
{
    std::lock_guard<std::mutex> lock(mtx);
    std::unique_ptr<char[]> buffer(new char[8192]);
    if (getcwd(buffer.get(), 8192))
    {
        return cmajor::rt::InstallString(buffer.get());
    }
    else
    {
        return cmajor::rt::InstallError(std::string("could not get current working directory: ") + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
    }
}

extern "C" RT_API const char* RtGetCurrentWorkingDirectory(int32_t currentWorkingDirectoryHandle)
{
    return cmajor::rt::GetString(currentWorkingDirectoryHandle);
}

extern "C" RT_API void RtFreeCurrentWorkingDirectoryHandle(int32_t currentWorkingDirectoryHandle)
{
    cmajor::rt::DisposeString(currentWorkingDirectoryHandle);
}

extern "C" RT_API int32_t RtExecute(const char* command)
{
    return system(command);
}

extern "C" RT_API int32_t RtBeginExec(const char* command)
{
    return cmajor::rt::Executor::Instance().BeginExecute(command);
}

extern "C" RT_API void RtAddRedirection(int32_t execHandle, int handle, const char* file)
{
    cmajor::rt::Executor::Instance().AddRedirection(execHandle, handle, file);
}

extern "C" RT_API int32_t RtEndExec(int32_t execHandle)
{
    return cmajor::rt::Executor::Instance().EndExecute(execHandle);
}

extern "C" RT_API int32_t RtBeginGetPathToExecutable()
{
    std::string pathToExecutable = soulng::util::GetPathToExecutable();
    return cmajor::rt::InstallString(pathToExecutable);
}

extern "C" RT_API const char* RtGetPathToExecutable(int32_t pathHandle)
{
    return cmajor::rt::GetString(pathHandle);
}

extern "C" RT_API void RtEndGetPathToExecutable(int32_t pathHandle)
{
    cmajor::rt::DisposeString(pathHandle);
}

extern "C" RT_API const char* RtGetOsInfo()
{
#ifdef _WIN32
    return "windows";
#else
    static char buf[1024];
    std::memset(buf, 0, 1024);
    FILE* fp = fopen("/proc/version", "r");
    if (fp != NULL)
    {
        const char* s = fgets(buf, 1023, fp);
        fclose(fp);
        if (s == NULL)
        {
            return "ERROR";
        }
    }
    else
    {
        return "ERROR";
    }
    return buf;
#endif
}
