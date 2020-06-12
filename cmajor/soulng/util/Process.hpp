// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_PROCESS_INCLUDED
#define SOULNG_UTIL_PROCESS_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

namespace soulng { namespace util {

class ProcessImpl;

class UTIL_API Process
{
public:
    enum class StdHandle : int
    {
        std_out = 1, std_err = 2
    };
    Process(const std::string& command);
    ~Process();
    bool Running();
    void WaitForExit();
    int ExitCode() const;
    bool Eof(StdHandle handle);
    std::string ReadLine(StdHandle handle);
    std::string ReadToEnd(StdHandle handle);
    void WriteLine(const std::string& line);
private:
    ProcessImpl* impl;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_PROCESS_INCLUDED
