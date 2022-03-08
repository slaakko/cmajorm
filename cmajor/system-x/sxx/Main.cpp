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
#include <system-x/kernel/OsApi.hpp>
#include <system-x/machine/Config.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>

using namespace soulng::util;
using namespace soulng::unicode;

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    cmsx::machine::Init();
    cmsx::kernel::Init(true);
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

std::string SearchBin(const std::string& fileName)
{
    if (boost::filesystem::exists(fileName))
    {
        return fileName;
    }
    std::string executableFileName = fileName;
    if (!Path::HasExtension(fileName))
    {
        executableFileName.append(".x");
    }
    std::string fullPath = GetFullPath(executableFileName);
    if (boost::filesystem::exists(fullPath))
    {
        return fullPath;
    }
    std::string binDirectory = GetFullPath(Path::Combine(Path::Combine(CmajorRoot(), "system-x"), "bin"));
    fullPath = GetFullPath(Path::Combine(binDirectory, executableFileName));
    if (boost::filesystem::exists(fullPath))
    {
        return fullPath;
    }
    else
    {
        throw std::runtime_error("executable '" + fileName + "' not found");
    }
}

int main(int argc, const char** argv)
{
    uint8_t exitCode = 0;
    sxx::Console console;
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
                std::string filePath = SearchBin(arg);
                args.push_back(filePath);
            }
            else
            {
                args.push_back(arg);
            }
        }
        if (!programFileSeen)
        {
            std::string filePath = SearchBin("sh");
            args.push_back(filePath);
        }
        cmsx::machine::Machine machine;
        cmsx::kernel::Kernel::Instance().SetMachine(&machine);
        cmsx::kernel::Kernel::Instance().Start();
        cmsx::kernel::Process* process = cmsx::kernel::ProcessManager::Instance().CreateProcess();
        process->SetFilePath(args[0]);
        process->SetUMask(cmsx::machine::UMask());
        if (process->FilePath() != "/mnt/sx/bin/sh.x")
        {
            process->SetUID(cmsx::machine::UID());
            process->SetGID(cmsx::machine::GID());
        }
        cmsx::kernel::Load(process, args, env, machine);
        if (verbose)
        {
            std::cout << "running '" << args[0] << "'..." << std::endl;;
        }
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
                exitCode = 255;
                std::cout << ex.what() << std::endl;
            }
        }
        if (verbose)
        {
            if (exitCode == 0)
            {
                exitCode = process->ExitCode();
            }
            std::cout << "'" << args[0] << "' exited with code " << static_cast<int>(exitCode) << std::endl;
            std::cout << std::endl;
            std::chrono::steady_clock::duration userTime = process->UserTime();
            std::chrono::steady_clock::duration sleepTime = process->SleepTime();
            std::chrono::steady_clock::duration systemTime = process->SystemTime();
            std::chrono::steady_clock::duration totalTime = userTime + sleepTime + systemTime;
            std::chrono::steady_clock::duration childUserTime = process->ChildUserTime();
            std::chrono::steady_clock::duration childSleepTime = process->ChildSleepTime();
            std::chrono::steady_clock::duration childSystemTime = process->ChildSystemTime();
            std::chrono::steady_clock::duration childTotalTime = childUserTime + childSleepTime + childSystemTime;
            double userTimePercent = 0.0; 
            double sleepTimePercent = 0.0;
            double systemTimePercent = 0.0;
            if (totalTime.count() > 0)
            {
                userTimePercent = (100.0 * userTime) / totalTime;
                sleepTimePercent = (100.0 * sleepTime) / totalTime;
                systemTimePercent = (100.0 * systemTime) / totalTime;
            }
            double childUserTimePercent = 0.0;
            double childSleepTimePercent = 0.0;
            double childSystemTimePercent = 0.0;
            if (childTotalTime.count() > 0)
            {
                childUserTimePercent = (100.0 * childUserTime) / childTotalTime;
                childSleepTimePercent = (100.0 * childSleepTime) / childTotalTime;
                childSystemTimePercent = (100.0 * childSystemTime) / childTotalTime;
            }
            std::cout << "user time:         " << DurationStr(userTime) << " (" << userTimePercent << "%)" << std::endl;
            std::cout << "sleep time:        " << DurationStr(sleepTime) << " (" << sleepTimePercent << "%)" << std::endl;
            std::cout << "system time:       " << DurationStr(systemTime) << " (" << systemTimePercent << "%)" << std::endl;
            std::cout << "total time:        " << DurationStr(totalTime) << std::endl;
            std::cout << "child user time:   " << DurationStr(childUserTime) << " (" << childUserTimePercent << "%)" << std::endl;
            std::cout << "child sleep time:  " << DurationStr(childSleepTime) << " (" << childSleepTimePercent << "%)" << std::endl;
            std::cout << "child system time: " << DurationStr(childSystemTime) << " (" << childSystemTimePercent << "%)" << std::endl;;
            std::cout << "child total time:  " << DurationStr(childTotalTime) << std::endl;
        }
        cmsx::kernel::ProcessManager::Instance().DeleteProcess(process->Id());
        cmsx::kernel::Kernel::Instance().Stop();
        machine.Exit();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return exitCode;
}
