// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Machine.hpp>
#include <system-x/sxx/Console.hpp>
#include <system-x/kernel/Load.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <system-x/kernel/InitDone.hpp>
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
    cmsx::kernel::Init();
}

void DoneApplication()
{
    cmsx::kernel::Done();
    cmsx::machine::Done();
    sngxml::xpath::Done();
    soulng::util::Done();
}

const char* version = "4.3.0";

void PrintHelp()
{
    std::cout << "System X Executor version " << version << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: sxx [options] FILE.x [ arguments for FILE.x ]" << std::endl;
    std::cout << std::endl;
    std::cout << "Run FILE.x in the System X virtual machine." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
}

int main(int argc, const char** argv)
{
    sxx::Console console;
    try
    {
        InitApplication();
        cmsx::kernel::SetConsoleFiles(&console, &console);
        std::vector<std::string> args;
        std::vector<std::string> env;
        bool programFileSeen = false;
        bool verbose = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!programFileSeen && arg.starts_with("--"))
            {
                if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--verbose")
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
                        case 'h':
                        {
                            PrintHelp();
                            return 1;
                        }
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
        cmsx::kernel::Kernel::Instance().SetMachine(&machine);
        cmsx::kernel::Kernel::Instance().Start();
        cmsx::kernel::Process* process = cmsx::kernel::ProcessManager::Instance().CreateProcess();
        process->SetFilePath(args[0]);
        cmsx::kernel::Load(process, args, env, machine);
        if (verbose)
        {
            std::cout << "running '" << args[0] << "'..." << std::endl;;
        }
        console.SetToUtf16Mode();
        machine.Start();
        cmsx::kernel::ProcessManager::Instance().WaitForProcessesExit();
        if (machine.HasException())
        {
            try
            {
                machine.CheckExceptions();
            }
            catch (const std::exception& ex)
            {
                console.SetToTextMode();
                std::cout << ex.what() << std::endl;
            }
        }
        if (verbose)
        {
            uint8_t exitCode = 255;
            if (process)
            {
                exitCode = process->ExitCode();
            }
            console.SetToTextMode();
            std::cout << "'" << args[0] << "' exited with code " << static_cast<int>(exitCode) << std::endl;
            std::cout << std::endl;
            std::chrono::steady_clock::duration userTime = process->UserTime();
            std::chrono::steady_clock::duration sleepTime = process->SleepTime();
            std::chrono::steady_clock::duration systemTime = process->SystemTime();
            std::chrono::steady_clock::duration totalTime = userTime + sleepTime + systemTime;
            double userTimePercent = (100.0 * userTime) / totalTime;
            double sleepTimePercent = (100.0 * sleepTime) / totalTime;
            double systemTimePercent = (100.0 * systemTime) / totalTime;
            std::cout << "user time:   " << DurationStr(userTime) << " (" << userTimePercent << "%)" << std::endl;
            std::cout << "sleep time:  " << DurationStr(sleepTime) << " (" << sleepTimePercent << "%)" << std::endl;
            std::cout << "system time: " << DurationStr(systemTime) << " (" << systemTimePercent << "%)" << std::endl;
            std::cout << "total time:  " << DurationStr(totalTime) << std::endl;
        }
        cmsx::kernel::ProcessManager::Instance().DeleteProcess(process->Id());
        cmsx::kernel::Kernel::Instance().Stop();
        machine.Exit();
    }
    catch (const std::exception& ex)
    {
        console.SetToTextMode();
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}
