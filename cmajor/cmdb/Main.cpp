#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/InitDone.hpp>
#include <cmajor/cmdebug/ConsoleDebugger.hpp>
#include <cmajor/cmdebug/ServerDebugger.hpp>
#include <cmajor/cmdebug/DebuggerClient.hpp>
#include <cmajor/cmdebug/CmdbSession.hpp>
#include <cmajor/cmdebug/KillChannel.hpp>
#ifdef _WIN32
#include <cmajor/cmpm/PortMapClient.hpp>
#endif
#include <soulng/util/Ansi.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/TextUtils.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <thread>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::debug::Init();
#ifdef _WIN32
        cmajor::cmpm::InitPortMapClient();
#endif
    }
    ~InitDone()
    {
#ifdef _WIN32
        cmajor::cmpm::DonePortMapClient();
#endif
        cmajor::debug::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;

const char* version = "4.2.0";

void PrintHelp()
{
    std::cout << "Usage: cmdb [options] EXECUTABLE[.exe] [ ARGS... ]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--debug | -d" << std::endl;
    std::cout << "  Debug the debugger." << std::endl;
    std::cout << "--dontBreakOnThrow | -t" << std::endl;
    std::cout << "  Do not break on throw instructions." << std::endl;
    std::cout << "--server | -e" << std::endl;
    std::cout << "  Start debugger in server mode. Default is console mode." << std::endl;
    std::cout << "--client | -c" << std::endl;
    std::cout << "  Used in combination with --server. Starts test client along with server." << std::endl;
    std::cout << "--port=PORT_NUMBER | -p=PORT_NUMBER" << std::endl;
    std::cout << "  Set debug server port number. Default port is 54326." << std::endl;
    std::cout << "--idle | -i" << std::endl;
    std::cout << "  Send idle messages." << std::endl;
    std::cout << "--log | -l" << std::endl;
    std::cout << "  Write log to %CMAJOR_ROOT%/log/cmdb.log (by default C:\\cmajor\\log\\cmdb.log)." << std::endl;
    std::cout << "--sessionPort=PORT_NUMBER | -s=PORT_NUMBER" << std::endl;
    std::cout << "  Set the port number of the CMDB session that cmdb and the program being debugged will use for exchanging console I/O messages. Default port is 54322." << std::endl;
    std::cout << "--portMapServicePort=PORT_NUMBER | -m=PORT_NUMBER" << std::endl;
    std::cout << "  Set port map service port number to PORT_NUMBER." << std::endl;
    std::cout << "  Optional. When set revises main port number and session port number using port map service every minute." << std::endl;
    std::cout << "--killPort=PORT_NUMBER | -k=PORT_NUMBER" << std::endl;
    std::cout << "  Optional. Set kill channel port number to PORT_NUMBER." << std::endl;
}

int main(int argc, const char** argv)
{
    bool server = false;
    bool client = false;
    InitDone initDone;
    int portMapServicePort = -1;
    try
    {
        bool verbose = false;
        std::string executable;
        std::vector<std::string> args;
        bool executableSeen = false;
        bool breakOnThrow = true;
        bool log = false;
        bool idle = false;
        int port = 54326;
        int sessionPort = 54322;
        int killPort = -1;
        bool wait = false;
        std::vector<int> portNumbers;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!executableSeen)
            {
                if (StartsWith(arg, "--"))
                {
                    if (arg == "--verbose")
                    {
                        verbose = true;
                    }
                    else if (arg == "--wait")
                    {
                        wait = true;
                    }
                    else if (arg == "--debug")
                    {
                        cmajor::debug::SetDebugFlag();
                    }
                    else if (arg == "--help")
                    {
                        PrintHelp();
                        return 1;
                    }
                    else if (arg == "--dontBreakOnThrow")
                    {
                        breakOnThrow = false;
                    }
                    else if (arg == "--server")
                    {
                        server = true;
                    }
                    else if (arg == "--client")
                    {
                        client = true;
                    }
                    else if (arg == "--log")
                    {
                        log = true;
                    }
                    else if (arg == "--idle")
                    {
                        idle = true;
                    }
                    else if (arg.find('=') != std::string::npos)
                    {
                        std::vector<std::string> components = Split(arg, '=');
                        if (components.size() == 2)
                        {
                            if (components[0] == "--port")
                            {
                                port = boost::lexical_cast<int>(components[1]);
                            }
                            else if (components[0] == "--sessionPort")
                            {
                                sessionPort = boost::lexical_cast<int>(components[1]);
                                cmajor::debug::SetCmdbSessionPort(sessionPort);
                            }
                            else if (components[0] == "--portMapServicePort")
                            {
                                portMapServicePort = boost::lexical_cast<int>(components[1]);
                            }
                            else if (components[0] == "--killPort")
                            {
                                killPort = boost::lexical_cast<int>(components[1]);
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
                    std::string options = arg.substr(1);
                    if (options.find('=') != std::string::npos)
                    {
                        std::vector<std::string> components = Split(arg, '=');
                        if (components.size() == 2)
                        {
                            if (components[0] == "p")
                            {
                                port = boost::lexical_cast<int>(components[1]);
                            }
                            else if (components[0] == "s")
                            {
                                sessionPort = boost::lexical_cast<int>(components[1]);
                                cmajor::debug::SetCmdbSessionPort(sessionPort);
                            }
                            else if (components[0] == "m")
                            {
                                portMapServicePort = boost::lexical_cast<int>(components[1]);
                            }
                            else if (components[0] == "k")
                            {
                                killPort = boost::lexical_cast<int>(components[1]);
                            }
                        }
                        else
                        {
                            throw std::runtime_error("unknown option '" + arg + "'");
                        }
                    }
                    else
                    {
                        for (char o : options)
                        {
                            switch (o)
                            {
                                case 'v':
                                {
                                    verbose = true;
                                    break;
                                }
                                case 'd':
                                {
                                    cmajor::debug::SetDebugFlag();
                                    break;
                                }
                                case 'h':
                                {
                                    PrintHelp();
                                    return 1;
                                }
                                case 't':
                                {
                                    breakOnThrow = false;
                                    break;
                                }
                                case 'e':
                                {
                                    server = true;
                                    break;
                                }
                                case 'c':
                                {
                                    client = true;
                                    break;
                                }
                                case 'l':
                                {
                                    log = true;
                                    break;
                                }
                                case 'i':
                                {
                                    idle = true;
                                    break;
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
                    executable = arg;
                    executableSeen = true;
                }
            }
            else
            {
                args.push_back(arg);
            }
        }
        if (executable.empty())
        {
            throw std::runtime_error("no executable specified");
        }
        if (wait)
        {
            std::this_thread::sleep_for(std::chrono::seconds{ 45 });
        }
        if (server)
        {
            if (killPort != -1)
            {
                cmajor::debug::StartKillChannel(killPort);
            }
#ifdef _WIN32
            if (portMapServicePort != -1)
            {
                std::vector<int> portNumbers;
                portNumbers.push_back(port);
                portNumbers.push_back(sessionPort);
                cmajor::cmpm::StartPortMapClient(portMapServicePort, portNumbers, "cmdb", GetPid());
            }
#endif
            if (client)
            {
                cmajor::debug::StartDebuggerServer(executable, args, verbose, breakOnThrow, version, port, log, idle);
                cmajor::debug::RunClient(port);
                cmajor::debug::StopDebuggerServer();
            }
            else
            {
                cmajor::debug::RunDebuggerServer(executable, args, verbose, breakOnThrow, version, port, log, idle);
            }
#ifdef _WIN32
            if (portMapServicePort != -1)
            {
                cmajor::cmpm::StopPortMapClient();
                portMapServicePort = -1;
            }
#endif
            if (killPort != -1)
            {
                cmajor::debug::StopKillChannel();
            }
        }
        else
        {
            cmajor::debug::RunDebuggerInteractive(executable, args, verbose, breakOnThrow, version);
        }
    }
    catch (const std::exception& ex)
    {
#ifdef _WIN32
        if (portMapServicePort != -1)
        {
            cmajor::cmpm::StopPortMapClient();
        }
#endif
        if (server)
        {
            std::cout << "debug-server-error" << std::endl;
            std::cout << ex.what() << std::endl;
        }
        else
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    return 0;
}
