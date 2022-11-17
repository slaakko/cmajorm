// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Process.hpp>
#include <soulng/util/ProcessImpl.hpp>
#ifdef _WIN32
#include <Windows.h>
#undef min
#undef max
#else
#include <sys/types.h>
#include <unistd.h>
#endif

namespace soulng { namespace util {

Process::Process(const std::string& command, Redirections redirections) : impl(new ProcessImpl(command, redirections))
{
}

Process::~Process()
{
    delete impl;
}

bool Process::Running()
{
    return impl->Running();
}

void Process::WaitForExit()
{
    impl->WaitForExit();
}

int Process::ExitCode() const
{
    return impl->ExitCode();
}

void Process::Terminate()
{
    impl->Terminate();
}

bool Process::Eof(StdHandle handle)
{
    return impl->Eof(handle);
}

std::string Process::ReadLine(StdHandle handle)
{
    return impl->ReadLine(handle);
}

std::string Process::ReadToEnd(StdHandle handle)
{
    return impl->ReadToEnd(handle);
}

void Process::WriteLine(const std::string& line)
{
    impl->WriteLine(line);
}

int GetPid()
{
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

} } // namespace soulng::util
