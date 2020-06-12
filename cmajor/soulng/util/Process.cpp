// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Process.hpp>
#include <boost/process.hpp>

namespace soulng { namespace util {

class ProcessImpl
{
public:
    ProcessImpl(const std::string& command) :
        process(command, boost::process::std_out > processStdOut, boost::process::std_err > processStdErr, boost::process::std_in < processStdIn)
    {
    }
    bool Running()
    {
        return process.running();
    }
    void WaitForExit()
    {
        process.wait();
    }
    int ExitCode() const
    {
        return process.exit_code();
    }
    bool Eof(Process::StdHandle handle)
    {
        switch (handle)
        {
            case Process::StdHandle::std_out:
            {
                if (processStdOut) return false;
                break;
            }
            case Process::StdHandle::std_err:
            {
                if (processStdErr) return false;
                break;
            }
        }
        return true;
    }
    std::string ReadLine(Process::StdHandle handle)
    {
        std::string line;
        switch (handle)
        {
            case Process::StdHandle::std_out:
            {
                if (processStdOut)
                {
                    std::getline(processStdOut, line);
                }
                break;
            }
            case Process::StdHandle::std_err:
            {
                if (processStdErr)
                {
                    std::getline(processStdErr, line);
                }
                break;
            }
        }
        return line;
    }
    std::string ReadToEnd(Process::StdHandle handle)
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
    void WriteLine(const std::string& line)
    {
        if (processStdIn)
        {
            processStdIn << line << std::endl;
        }
    }
private:
    boost::process::ipstream processStdOut;
    boost::process::ipstream processStdErr;
    boost::process::opstream processStdIn;
    boost::process::child process;
};

Process::Process(const std::string& command) : impl(new ProcessImpl(command))
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

} } // namespace soulng::util
