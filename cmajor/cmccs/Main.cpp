#include <cmajor/cmccs/CodeCompletionServer.hpp>
#include <cmajor/cmccs/KeepAliveServer.hpp>
#include <cmajor/cmmsg/Register.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <sngxml/serialization/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <cmajor/cmpm/Register.hpp>
#include <cmajor/cmpm/PortMapClient.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <mutex>

struct KeepAliveServerRun
{
    KeepAliveServerRun(int keepAliveServerPort, std::condition_variable* exitVar, bool* exiting)
    {
        cmccs::StartKeepAliveServer(keepAliveServerPort, exitVar, exiting);
    }
    ~KeepAliveServerRun()
    {
        cmccs::StopKeepAliveServer();
    }
};

struct CodeCompletionServerRun
{
    CodeCompletionServerRun(int port, const std::string& version, std::condition_variable* exitVar, bool* exiting)
    {
        cmccs::StartCodeCompletionServer(port, version, exitVar, exiting);
    }
    ~CodeCompletionServerRun()
    {
        cmccs::StopCodeCompletionServer();
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

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
        sngxml::xpath::Init();
        sngxml::xmlser::Init();
        cmpm::Register();
        cmmsg::Register();
        cmajor::cmpm::InitPortMapClient();
    }
    ~InitDone()
    {
        cmajor::cmpm::DonePortMapClient();
        sngxml::xmlser::Done();
        sngxml::xpath::Done();
        cmajor::symbols::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;

const char* version = "4.2.0";

void PrintHelp()
{
    std::cout << "Cmajor Code Completion Server version " << version << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: cmccs [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << std::endl;
    std::cout << "--port=PORT | -p=PORT" << std::endl;
    std::cout << "  Set server port number to PORT." << std::endl;
    std::cout << "  Default port number is 54327." << std::endl;
    std::cout << "--keepAliveServerPort=PORT | -k=PORT." << std::endl;
    std::cout << "  Set keep alive server port number to PORT." << std::endl;
    std::cout << "  Default port number is 54328." << std::endl;
    std::cout << "--portMapServicePort=PORT | -m=PORT" << std::endl;
    std::cout << "  Set port map service port number to PORT." << std::endl;
    std::cout << "  Optional. When set revises main port number and keep alive server port number using port map service every minute." << std::endl;
    std::cout << std::endl;
}

bool CheckCmajorRootEnv()
{
    try
    {
        soulng::unicode::CmajorRoot();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return false;
    }
    return true;
}

int main(int argc, const char** argv)
{
    if (!CheckCmajorRootEnv()) return 1;
    InitDone initDone;
    std::condition_variable exitVar;
    bool exiting = false;
    try
    {
        int port = 54327;
        int keepAliveServerPort = cmccs::defaultKeepAliveServerPort;
        int portMapServicePort = -1;
        bool wait = false;
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
                else if (arg == "--wait")
                {
                    wait = true;
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
                            case 'w':
                            {
                                wait = true;
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
                throw std::runtime_error("unknown option '" + arg + "'");
            }
        }
        if (wait)
        {
            std::this_thread::sleep_for(std::chrono::seconds{ 60 });
        }
        KeepAliveServerRun runKeepAliveServer(keepAliveServerPort, &exitVar, &exiting);
        CodeCompletionServerRun runCodeCompletionServer(port, version, &exitVar, &exiting);
        std::vector<int> ports;
        if (portMapServicePort != -1)
        {
            ports.push_back(port);
            ports.push_back(keepAliveServerPort);
        }
        PortMapClientRun runPortMapClient(portMapServicePort, ports, "cmccs", soulng::util::GetPid());
        bool timeoutReported = false;
        while (!cmccs::Timeout())
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (exitVar.wait_for(lock, std::chrono::seconds{ 3 }, [exiting] { return exiting; }))
            {
                if (cmccs::Timeout())
                {
                    std::cout << "Code completion server timeout." << std::endl;
                    timeoutReported = true;
                }
                break;
            }
        }
        if (cmccs::Timeout() && !timeoutReported)
        {
            std::cout << "Code completion server timeout." << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "code-completion-server-error" << std::endl;
        std::cout << ex.what() << std::endl;
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}