// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/InitDone.hpp>
#include <cmajor/build/InitDone.hpp>
#include <cmajor/build/ServerCommand.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/serialization/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <iostream>
#include <stdexcept>

using namespace soulng::util;

void InitApplication()
{
    soulng::util::Init();
    sngcm::ast::Init();
    sngxml::xpath::Init();
    sngxml::xmlser::Init();
    cmajor::symbols::Init();
    cmajor::build::Init();
}

const char* version = "4.3.0";

void PrintHelp()
{
    std::cout << "Cmajor server version " << version << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "cmserver [options] (add server | remove server | run server | show configuration) ..." << std::endl;
    std::cout << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--debug | -d" << std::endl;
    std::cout << "  Print debugging output to console." << std::endl;
    std::cout << std::endl;
    std::cout << "cmserver [options] add server SERVERNAME port PORTNUMBER" << std::endl;
    std::cout << "  Add server SERVERNAME that listens port PORTNUMBER." << std::endl;
    std::cout << "options for add server:" << std::endl;
    std::cout << "  --force" << std::endl;
    std::cout << "  If server with identical name already exists, " << std::endl;
    std::cout << "  change its port number to be the given port number." << std::endl;
    std::cout << std::endl;
    std::cout << "cmserver [options] remove server SERVERNAME" << std::endl;
    std::cout << "  Remove server SERVERNAME." << std::endl;
    std::cout << std::endl;
    std::cout << "cmserver [options] run server SERVERNAME" << std::endl;
    std::cout << "  Run server SERVERNAME." << std::endl;
    std::cout << "  The server begins listening configured port number," << std::endl;
    std::cout << "  accepts client connections to that port number and" << std::endl;
    std::cout << "  executes push and build commands issued by the client." << std::endl;
    std::cout << std::endl;
    std::cout << "cmserver [options] show configuration" << std::endl;
    std::cout << "  Show configuration." << std::endl;
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        cmajor::symbols::SetBackEnd(cmajor::symbols::BackEnd::cmcpp);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::repository);
        SetToolChain("gcc");
        bool verbose = false;
        std::string options;
        std::string tokens;
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
                    cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
                }
                if (!options.empty())
                {
                    options.append(1, ' ');
                }
                options.append(arg.substr(2));
            }
            else if (StartsWith(arg, "-"))
            {
                if (!options.empty())
                {
                    options.append(1, ' ');
                }
                std::string ostr = arg.substr(1);
                for (char o : ostr)
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
                            options.append("verbose");
                            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
                            break;
                        }
                        case 'd':
                        {
                            options.append("debug");
                            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::printDebugMessages);
                            break;
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
                if (!tokens.empty())
                {
                    tokens.append(1, ' ');
                }
                tokens.append(arg);
            }
        }
        if (verbose)
        {
            std::cout << "Cmajor server version " << version << std::endl;
        }
        std::string commandStr;
        if (!options.empty())
        {
            commandStr.append(1, '[').append(options).append(1, ']').append(1, ' ');
        }
        ReadToolChains(cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose));
        commandStr.append(tokens);
        std::unique_ptr<cmajor::build::ServerCommand> command = cmajor::build::ParseServerCommand(commandStr);
        command->Execute();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
