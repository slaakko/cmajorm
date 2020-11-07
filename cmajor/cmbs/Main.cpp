// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/BuildServer.hpp>
#include <cmajor/cmbs/BuildClient.hpp>
#include <cmajor/cmpm/PortMapClient.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#ifdef _WIN32
#include <cmajor/cmres/InitDone.hpp>
#endif
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/LogFileWriter.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <thread>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
        sngxml::xpath::Init();
#ifdef _WIN32
        cmajor::resources::Init();
#endif
        cmajor::cmpm::InitPortMapClient();
    }
    ~InitDone()
    {
        cmajor::cmpm::DonePortMapClient();
#ifdef _WIN32
        cmajor::resources::Done();
#endif
        sngxml::xpath::Done();
        cmajor::symbols::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;
using namespace soulng::unicode;

const char* version = "3.7.0";

void PrintHelp()
{
    std::cout << "Cmajor Build Server version " << version << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: cmbs [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << std::endl;
    std::cout << "--log | -l" << std::endl;
    std::cout << "  Write log to %CMAJOR_ROOT%/log/cmbs.log (by default C:\\cmajor\\log\\cmbs.log)." << std::endl;
    std::cout << std::endl;
    std::cout << "--timeoutSecs=SECS | -t=SECS" << std::endl;
    std::cout << "  Set timeout in seconds." << std::endl;
    std::cout << "  Default timeout is 15 seconds." << std::endl;
    std::cout << std::endl;
    std::cout << "--port=PORT | -p=PORT" << std::endl;
    std::cout << "  Set server port number to PORT." << std::endl;
    std::cout << "  Default port number is 54325." << std::endl;
    std::cout << "--portMapServicePort=PORT | -m=PORT" << std::endl;
    std::cout << "  Set port map service port number to PORT." << std::endl;
    std::cout << "  Optional. When set revises main port number using port map service every minute." << std::endl;
    std::cout << std::endl;
    std::cout << "--request=FILE | -r=FILE" << std::endl;
    std::cout << "  Read build request from file FILE and run it." << std::endl;
}

struct BuildServerRun
{
    BuildServerRun(int port, const std::string& version, int timeoutSecs, bool log)
    {
        cmbs::StartBuildServer(port, version, timeoutSecs, log);
    }
    ~BuildServerRun()
    {
        cmbs::StopBuildServer();
    }
};

struct PortMapClientRun
{
    PortMapClientRun(int portMapServicePort_, const std::vector<int>& portNumbers, const std::string& programName, int pid) : portMapServicePort(portMapServicePort_)
    {
        if (portMapServicePort != -1)
        {
            cmajor::cmpm::StartPortMapClient(portMapServicePort, portNumbers, programName, pid);
        }
    }
    ~PortMapClientRun()
    {
        if (portMapServicePort != -1)
        {
            cmajor::cmpm::StopPortMapClient();
        }
    }
    int portMapServicePort;
};

int main(int argc, const char** argv)
{
    InitDone initDone;
    bool log = false;
    try
    {
        int port = 54325;
        int portMapServicePort = -1;
        std::string requestFilePath;
        int timeoutSecs = 15;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--log")
                {
                    log = true;
                }
                else if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(arg, '=');
                    if (components.size() == 2)
                    {
                        std::string option = components[0];
                        std::string value = components[1];
                        if (option == "--port")
                        {
                            port = boost::lexical_cast<int>(value);
                        }
                        else if (option == "--portMapServicePort")
                        {
                            portMapServicePort = boost::lexical_cast<int>(value);
                        }
                        else if (option == "--request")
                        {
                            requestFilePath = GetFullPath(value);
                        }
                        else if (option == "--timeoutSecs")
                        {
                            timeoutSecs = boost::lexical_cast<int>(value);
                        }
                        else
                        {
                            throw std::runtime_error("unknown option '" + arg + "'");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (StartsWith(arg, "-"))
            {
                if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(arg, '=');
                    std::string option = components[0];
                    std::string value = components[1];
                    if (option == "-p")
                    {
                        port = boost::lexical_cast<int>(value);
                    }
                    else if (option == "-m")
                    {
                        portMapServicePort = boost::lexical_cast<int>(value);
                    }
                    else if (option == "-r")
                    {
                        requestFilePath = GetFullPath(value);
                    }
                    else if (option == "-t")
                    {
                        timeoutSecs = boost::lexical_cast<int>(value);
                    }
                    else
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                }
                else
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
                            case 'l':
                            {
                                log = true;
                                break;
                            }
                            default:
                            {
                                throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                            }
                        }
                    }
                }
            }
            else
            {
                throw std::runtime_error("unknown argument '" + arg + "'");
            }
        }
        BuildServerRun runBuildServer(port, version, timeoutSecs, log);
        std::vector<int> ports;
        if (portMapServicePort != -1)
        {
            ports.push_back(port);
        }
        PortMapClientRun runPortMapClient(portMapServicePort, ports, "cmbs", soulng::util::GetPid());
        if (requestFilePath.empty())
        {
            while (!cmbs::BuildServerStopRequested() && !cmbs::BuildServerTimeOut())
            {
                std::this_thread::sleep_for(std::chrono::duration{ std::chrono::seconds{ 1 } });
            }
            if (cmbs::BuildServerStopRequested())
            {
                std::cout << "Build server stop request received." << std::endl;
            }
            else if (cmbs::BuildServerTimeOut())
            {
                std::cout << "Build server timeout." << std::endl;
            }
        }
        else
        {
            cmbs::RunRequest(requestFilePath, port);
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "build-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << ex.what() << std::endl;
        if (log)
        {
            std::string logFilePath = cmbs::CmbsLogFilePath();
            soulng::util::LogFileWriter writer(logFilePath);
            writer.WriteLine("================================================================================");
            writer.WriteCurrentDateTime();
            writer << "main got exception: " << ex.what() << std::endl;
        }
        return 1;
    }
    return 0;
}
