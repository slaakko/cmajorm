#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/InitDone.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <iostream>

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

void PrintHelp()
{

}

const char* version = "3.6.0";

int main(int argc, const char** argv)
{
    std::cout << "Cmajor debugger version " << version << std::endl;
    InitDone initDone;
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
                    else
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                }
                else if (StartsWith(arg, "-"))
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
