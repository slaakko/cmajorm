// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/BuildServer.hpp>
#include <cmajor/cmbs/BuildClient.hpp>
#include <soulng/util/Path.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#ifdef _WIN32
#include <cmajor/cmres/InitDone.hpp>
#endif
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
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
    }
    ~InitDone()
    {
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
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << std::endl;
    std::cout << "--machine | -m" << std::endl;
    std::cout << "  Machine interface." << std::endl;
    std::cout << std::endl;
    std::cout << "--port=PORT | -p=PORT" << std::endl;
    std::cout << "  Set server port number to PORT." << std::endl;
    std::cout << "  Default port number is 54325." << std::endl;
    std::cout << std::endl;
    std::cout << "--request=FILE | -r=FILE" << std::endl;
    std::cout << "  Read build request from file FILE and run it." << std::endl;
}

struct BuildServerRun
{
    BuildServerRun(int port, bool verbose_, const std::string& version) : verbose(verbose_)
    {
        if (verbose)
        {
            std::cout << "Build server starting..." << std::endl;
        }
        cmbs::StartBuildServer(port, verbose, version);
        if (verbose)
        {
            std::cout << "Build server started." << std::endl;
        }
    }
    ~BuildServerRun()
    {
        if (verbose)
        {
            std::cout << "Build server stopping..." << std::endl;
        }
        cmbs::StopBuildServer();
        if (verbose)
        {
            std::cout << "Build server stopped." << std::endl;
        }
    }
    bool verbose;
};

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        bool verbose = false;
        bool machine = false;
        int port = 54325;
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
                else if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--machine")
                {
                    machine = true;
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
                            case 'v':
                            {
                                verbose = true;
                                break;
                            }
                            case 'm':
                            {
                                machine = true;
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
        BuildServerRun runBuildServer(port, verbose, version);
        if (requestFilePath.empty())
        {
            std::string line;
            if (!machine)
            {
                std::cout << "Enter 'exit' or 'quit' to exit.." << std::endl;
                std::cout << "> ";
            }
            while (std::getline(std::cin, line))
            {
                if (line == "quit" || line == "exit")
                {
                    std::cout << "bye" << std::endl;
                    break;
                }
                else
                {
                    std::cout << "Build server: unknown command: " << line << std::endl;
                    std::cout << "Enter 'exit' or 'quit' to exit.." << std::endl;
                }
            }
        }
        else
        {
            cmbs::RunRequest(requestFilePath, port);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
