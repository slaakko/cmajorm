// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_SYSTEM_INCLUDED
#define SOULNG_UTIL_SYSTEM_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace soulng { namespace util {

class ProcessFailure : public std::runtime_error
{
public:
    ProcessFailure(const std::string& errorMessage_, int exitCode_);
    int ExitCode() const { return exitCode; }
private:
    int exitCode;
};

UTIL_API void DisableConsoleWindow();
UTIL_API void System(const std::string& command);
UTIL_API void System(const std::string& command, bool ignoreReturnValue);
UTIL_API void System(const std::string& command, int redirectFd, const std::string& toFile);
UTIL_API void System(const std::string& command, int redirectFd, const std::string& toFile, bool ignoreReturnValue);
UTIL_API void System(const std::string& command, const std::vector<std::pair<int, std::string>>& redirections);
UTIL_API unsigned long long Spawn(const std::string& filename, const std::vector<std::string>& args);
UTIL_API int Wait(unsigned long long processHandle);
UTIL_API int ReadFromPipe(int pipeHandle, void* buffer, unsigned int count);
UTIL_API int WriteToPipe(int pipeHandle, void* buffer, unsigned int count);
UTIL_API void RedirectStdHandlesToPipes(std::vector<int>& oldHandles, std::vector<int>& pipeHandles);
UTIL_API void RestoreStdHandles(const std::vector<int>& oldHandles);
UTIL_API std::string GetPathToExecutable();

} } // namespace soulng::util

#endif // SOULNG_UTIL_SYSTEM_INCLUDED
