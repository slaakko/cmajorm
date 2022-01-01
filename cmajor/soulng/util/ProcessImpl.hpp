// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_PROCESS_IMPL_INCLUDED
#define SOULNG_UTIL_PROCESS_IMPL_INCLUDED
#include <algorithm>
#include <soulng/util/Process.hpp>
#if defined(_WIN32) && defined(__MINGW32__)
#ifndef __kernel_entry
#define __kernel_entry
#endif
#endif
#include <boost/process.hpp>
#include <string>

namespace soulng { namespace util {

class UTIL_API ProcessImpl
{
public:
    ProcessImpl(const std::string& command, Process::Redirections redirections_);
    ProcessImpl(const ProcessImpl&) = delete;
    ProcessImpl& operator=(const ProcessImpl&) = delete;
    ProcessImpl(ProcessImpl&&) = delete;
    ProcessImpl& operator=(ProcessImpl&&) = delete;
    bool Running();
    void WaitForExit();
    int ExitCode() const;
    void Terminate();
    bool Eof(Process::StdHandle handle);
    std::string ReadLine(Process::StdHandle handle);
    std::string ReadToEnd(Process::StdHandle handle);
    void WriteLine(const std::string& line);
private:
    Process::Redirections redirections;
    boost::process::ipstream processStdOut;
    boost::process::ipstream processStdErr;
    boost::process::opstream processStdIn;
    boost::process::child process;
    bool stdOutEof;
    bool stdErrEof;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_PROCESS_IMPL_INCLUDED
