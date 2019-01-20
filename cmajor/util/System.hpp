// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_SYSTEM_INCLUDED
#define CMAJOR_UTIL_SYSTEM_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace cmajor { namespace util {

class ProcessFailure : public std::runtime_error
{
public:
    ProcessFailure(const std::string& errorMessage_, int exitCode_);
    int ExitCode() const { return exitCode; }
private:
    int exitCode;
};

void DisableConsoleWindow();
void System(const std::string& command);
void System(const std::string& command, bool ignoreReturnValue);
void System(const std::string& command, int redirectFd, const std::string& toFile);
void System(const std::string& command, int redirectFd, const std::string& toFile, bool ignoreReturnValue);
void System(const std::string& command, const std::vector<std::pair<int, std::string>>& redirections);
unsigned long long Spawn(const std::string& filename, const std::vector<std::string>& args);
int Wait(unsigned long long processHandle);
int ReadFromPipe(int pipeHandle, void* buffer, unsigned int count);
int WriteToPipe(int pipeHandle, void* buffer, unsigned int count);
void RedirectStdHandlesToPipes(std::vector<int>& oldHandles, std::vector<int>& pipeHandles);
void RestoreStdHandles(const std::vector<int>& oldHandles);
std::string GetPathToExecutable();

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_SYSTEM_INCLUDED
