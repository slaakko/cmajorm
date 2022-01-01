// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Machine.hpp>
#include <system-x/os/Load.hpp>
#include <system-x/os/ProcessManager.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <system-x/os/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Time.hpp>
#include <iostream>
#include <stdexcept>

using namespace soulng::util;

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    cmsx::machine::Init();
    cmsx::os::Init();
}

void DoneApplication()
{
    cmsx::os::Done();
    cmsx::machine::Done();
    sngxml::xpath::Done();
    soulng::util::Done();
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        std::vector<std::string> args;
        std::vector<std::string> env;
        bool programFileSeen = false;
        bool verbose = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!programFileSeen && arg.starts_with("--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (!programFileSeen && arg.starts_with("-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'v':
                        {
                            verbose = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else if (!programFileSeen)
            {
                programFileSeen = true;
                args.push_back(GetFullPath(arg));
            }
            else
            {
                args.push_back(arg);
            }
        }
        if (!programFileSeen)
        {
            throw std::runtime_error("no program set");
        }
        cmsx::machine::Machine machine;
        cmsx::os::Process* process = cmsx::os::ProcessManager::Instance().CreateProcess();
        process->SetFilePath(args[0]);
        cmsx::os::Load(process, args, env, machine);
        cmsx::os::ProcessManager::Instance().SetCurrentProcess(process);
        if (verbose)
        {
            std::cout << "running '" << args[0] << "'..." << std::endl;;
        }
        machine.GetProcessor().Run();
        if (verbose)
        {
            std::cout << "'" << args[0] << "' exited with code " << machine.GetProcessor().GetExitCode() << std::endl;
            std::cout << std::endl;
            std::chrono::steady_clock::duration userTime = process->UserTime();
            std::chrono::steady_clock::duration systemTime = process->SystemTime();
            std::chrono::steady_clock::duration totalTime = userTime + systemTime;
            double userTimePercent = (100.0 * userTime) / totalTime;
            double systemTimePercent = (100.0 * systemTime) / totalTime;
            std::cout << "user time:   " << DurationStr(userTime) << " (" << userTimePercent << "%)" << std::endl;
            std::cout << "system time: " << DurationStr(systemTime) << " (" << systemTimePercent << "%)" << std::endl;
            std::cout << "total time:  " << DurationStr(totalTime) << std::endl;
        }
        cmsx::os::ProcessManager::Instance().DeleteProcess(process->Id());
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}