#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/InitDone.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <cmajor/cmdebug/CmdbSession.hpp>
#include <soulng/util/Ansi.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#endif

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::debug::Init();
    }
    ~InitDone()
    {
        cmajor::debug::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;

const char* version = "3.6.0";

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
    std::cout << "--sessionPort=PORT_NUMBER | -s=PORT_NUMBER" << std::endl;
    std::cout << "  Set the port number of the CMDB session that cmdb and the program being debugged will use for exchanging console I/O messages. Default port is 54322." << std::endl;
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    std::cout << "Cmajor debugger version " << version << std::endl;
    try
    {
        bool verbose = false;
        std::string executable;
        std::vector<std::string> args;
        bool executableSeen = false;
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
                    else if (arg == "--debug")
                    {
                        cmajor::debug::SetDebugFlag();
                    }
                    else if (arg == "--help")
                    {
                        PrintHelp();
                        return 1;
                    }
                    else if (arg.find('=') != std::string::npos)
                    {
                        std::vector<std::string> components = Split(arg, '=');
                        if (components.size() == 2)
                        {
                            if (components[0] == "--sessionPort")
                            {
                                int port = boost::lexical_cast<int>(components[1]);
                                cmajor::debug::SetCmdbSessionPort(port);
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
                            if (components[0] == "s")
                            {
                                int port = boost::lexical_cast<int>(components[1]);
                                cmajor::debug::SetCmdbSessionPort(port);
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
        cmajor::debug::RunDebuggerInteractive(executable, args, verbose);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
