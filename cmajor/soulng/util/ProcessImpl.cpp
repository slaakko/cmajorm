// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/ProcessImpl.hpp>

namespace soulng { namespace util {

ProcessImpl::ProcessImpl(const std::string& command, Process::Redirections redirections_) : redirections(redirections_), stdOutEof(false), stdErrEof(false)
{
    switch (redirections)
    {
        case Process::Redirections::none:
        {
            process = boost::process::child(command);
            break;
        }
        case Process::Redirections::processStdIn:
        {
            process = boost::process::child(command, boost::process::std_in < processStdIn);
            break;
        }
        case Process::Redirections::processStdOut:
        {
            process = boost::process::child(command, boost::process::std_out > processStdOut);
            break;
        }
        case Process::Redirections::processStdErr:
        {
            process = boost::process::child(command, boost::process::std_err > processStdErr);
            break;
        }
        case Process::Redirections::processStdIn | Process::Redirections::processStdOut:
        {
            process = boost::process::child(command, boost::process::std_in < processStdIn, boost::process::std_out > processStdOut);
            break;
        }
        case Process::Redirections::processStdIn | Process::Redirections::processStdErr:
        {
            process = boost::process::child(command, boost::process::std_in < processStdIn, boost::process::std_err > processStdErr);
            break;
        }
        case Process::Redirections::processStdOut | Process::Redirections::processStdErr:
        {
            process = boost::process::child(command, boost::process::std_out > processStdOut, boost::process::std_err > processStdErr);
            break;
        }
        case Process::Redirections::processStdIn | Process::Redirections::processStdOut | Process::Redirections::processStdErr:
        {
            process = boost::process::child(command, boost::process::std_in < processStdIn, boost::process::std_out > processStdOut, boost::process::std_err > processStdErr);
            break;
        }
    }
}

bool ProcessImpl::Running()
{
    return process.running();
}

void ProcessImpl::WaitForExit()
{
    process.wait();
}

int ProcessImpl::ExitCode() const
{
    return process.exit_code();
}

void ProcessImpl::Terminate()
{
    process.terminate();
}

bool ProcessImpl::Eof(Process::StdHandle handle)
{
    switch (handle)
    {
        case Process::StdHandle::stdOut:
        {
            if ((redirections & Process::Redirections::processStdOut) != Process::Redirections::none)
            {
                if (!stdOutEof && processStdOut) return false;
            }
            break;
        }
        case Process::StdHandle::stdErr:
        {
            if ((redirections & Process::Redirections::processStdErr) != Process::Redirections::none)
            {
                if (!stdErrEof && processStdErr) return false;
            }
            break;
        }
    }
    return true;
}

std::string ProcessImpl::ReadLine(Process::StdHandle handle)
{
    std::string line;
    switch (handle)
    {
        case Process::StdHandle::stdOut:
        {
            if ((redirections & Process::Redirections::processStdOut) != Process::Redirections::none)
            {
                if (processStdOut)
                {
                    if (!std::getline(processStdOut, line))
                    {
                        stdOutEof = true;
                    }
                }
            }
            break;
        }
        case Process::StdHandle::stdErr:
        {
            if ((redirections & Process::Redirections::processStdErr) != Process::Redirections::none)
            {
                if (processStdErr)
                {
                    if (!std::getline(processStdErr, line))
                    {
                        stdErrEof = true;
                    }
                }
            }
            break;
        }
    }
    return line;
}

std::string ProcessImpl::ReadToEnd(Process::StdHandle handle)
{
    std::string lines;
    bool first = true;
    while (!Eof(handle))
    {
        if (first)
        {
            first = false;
        }
        else
        {
            lines.append(1, '\n');
        }
        std::string line = ReadLine(handle);
        if (!line.empty())
        {
            lines.append(line);
        }
    }
    return lines;
}

void ProcessImpl::WriteLine(const std::string& line)
{
    if ((redirections & Process::Redirections::processStdIn) != Process::Redirections::none)
    {
        if (processStdIn)
        {
            processStdIn << line << std::endl;
        }
    }
    else
    {
        throw std::runtime_error("process stdin not redirected");
    }
}

} } // namespace soulng::util
