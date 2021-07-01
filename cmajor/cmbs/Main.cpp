// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/BuildServer.hpp>
#include <cmajor/cmbs/BuildClient.hpp>
#include <cmajor/cmbs/KeepAliveServer.hpp>
#include <cmajor/cmpm/PortMapClient.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#ifdef TRACE
#include <cmajor/cmbs_trace/TraceFunctions.hpp>
#include <soulng/util/Trace.hpp>
#endif
#include <cmajor/cmmid/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
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
#include <mutex>
#include <condition_variable>

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

const char* version = "4.0.0";

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
    std::cout << "--progress | -g" << std::endl;
    std::cout << "  Send progress messages." << std::endl;
    std::cout << std::endl;
    std::cout << "--port=PORT | -p=PORT" << std::endl;
    std::cout << "  Set server port number to PORT." << std::endl;
    std::cout << "  Default port number is 54325." << std::endl;
    std::cout << "--keepAliveServerPort=PORT | -k=PORT." << std::endl;
    std::cout << "  Set keep alive server port number to PORT." << std::endl;
    std::cout << "  Default port number is 54329." << std::endl;
    std::cout << "--portMapServicePort=PORT | -m=PORT" << std::endl;
    std::cout << "  Set port map service port number to PORT." << std::endl;
    std::cout << "  Optional. When set revises main port number and keep alive server port number using port map service every minute." << std::endl;
    std::cout << std::endl;
    std::cout << "--request=FILE | -r=FILE" << std::endl;
    std::cout << "  Read build request from file FILE and run it." << std::endl;
}

struct KeepAliveServerRun
{
    KeepAliveServerRun(int keepAliveServerPort, std::condition_variable* exitVar, bool* exiting)
    {
        cmbs::StartKeepAliveServer(keepAliveServerPort, exitVar, exiting);
    }
    ~KeepAliveServerRun()
    {
        cmbs::StopKeepAliveServer();
    }
};

struct BuildServerRun
{
    BuildServerRun(int port, const std::string& version, bool log, bool progress, std::condition_variable* exitVar, bool* exiting)
    {
        cmbs::StartBuildServer(port, version, log, progress, exitVar, exiting);
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

std::mutex mtx;

int main(int argc, const char** argv)
{
    InitDone initDone;
    #ifdef TRACE    
    soulng::util::BeginTracing();
    soulng::util::SetThreadId('M');
    soulng::util::Tracer mainTracer(main_f);
    #endif
    std::condition_variable exitVar;
    bool exiting = false;
    bool log = false;
    bool wait = false;
    bool progress = false;
    try
    {
        int port = 54325;
        int keepAliveServerPort = cmbs::defaultKeepAliveServerPort;
        int portMapServicePort = -1;
        std::string requestFilePath;
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
                else if (arg == "--wait")
                {
                    wait = true;
                }
                else if (arg == "--progress")
                {
                    progress = true;
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
                        else if (option == "--keepAliveServerPort")
                        {
                            keepAliveServerPort = boost::lexical_cast<int>(value);
                        }
                        else if (option == "--portMapServicePort")
                        {
                            portMapServicePort = boost::lexical_cast<int>(value);
                        }
                        else if (option == "--request")
                        {
                            requestFilePath = GetFullPath(value);
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
                    else if (option == "-k")
                    {
                        keepAliveServerPort = boost::lexical_cast<int>(value);
                    }
                    else if (option == "-r")
                    {
                        requestFilePath = GetFullPath(value);
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
                            case 'w':
                            {
                                wait = true;
                                break;
                            }
                            case 'g':
                            {
                                progress = true;
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
        if (wait)
        {
            std::this_thread::sleep_for(std::chrono::seconds{ 45 });
        }
        KeepAliveServerRun runKeepAliveServer(keepAliveServerPort, &exitVar, &exiting);
        BuildServerRun runBuildServer(port, version, log, progress, &exitVar, &exiting);
        std::vector<int> ports;
        if (portMapServicePort != -1)
        {
            ports.push_back(port);
            ports.push_back(keepAliveServerPort);
        }
        PortMapClientRun runPortMapClient(portMapServicePort, ports, "cmbs", soulng::util::GetPid());
        if (requestFilePath.empty())
        {
            bool stopReported = false;
            bool timeoutReported = false;
            while (!cmbs::BuildServerStopRequested() && !cmbs::Timeout())
            {
                std::unique_lock<std::mutex> lock(mtx);
                if (exitVar.wait_for(lock, std::chrono::seconds{ 3 }, [exiting] { return exiting; }))
                {
                    if (cmbs::BuildServerStopRequested())
                    {
                        std::cout << "Build server stop request received." << std::endl;
                        stopReported = true;
                    }
                    else if (cmbs::Timeout())
                    {
                        std::cout << "Build server timeout." << std::endl;
                        timeoutReported = true;
                    }
                    break;
                }
            }
            if (cmbs::BuildServerStopRequested() && !stopReported)
            {
                std::cout << "Build server stop request received." << std::endl;
            }
            else if (cmbs::Timeout() && !timeoutReported)
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
        #ifdef TRACE
        soulng::util::EndTracing();
        #endif
        return 1;
    }
    #ifdef TRACE
    soulng::util::EndTracing();
    #endif
    return 0;
}
