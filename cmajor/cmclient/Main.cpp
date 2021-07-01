// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/ClientCommand.hpp>
#include <cmajor/build/InitDone.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <iostream>
#include <stdexcept>

const char* version = "4.0.0";

// global options:
// --verbose: be verbose
// --debug: print debugging output to console

// cmserver add SERVER port 52360 [default toolchain (clang | gcc | vs)]
// cmserver remove SERVER
// cmserver run SERVER

// cmserver --force add ...

// cmclient push PROJECT.cmp 
// cmclient push PROJECT.cmp to server SERVER

// cmclient --only push ...

// cmclient remove PROJECT.cmp
// cmclient remove PROJECT.cmp from server SERVER

// cmclient --all remove ...

// cmclient build PROJECT.cmp 
// cmclient build PROJECT.cmp using server SERVER

// cmclient --push build ...
// cmclient --config=(debug | release) build ...
// cmclient --toolchain=(clang | gcc | vs) build ...

// cmclient debug PROJECT.cmp
// cmclient debug PROJECT.cmp using server SERVER

// cmclient --build [--push] debug ...
// cmclient --toolchain=(clang | gcc) debug ...

// cmclient install PROJECT.cmp to DIRECTORY [from server SERVER]

// cmclient --toolchain=(clang | gcc | vs) install ...
// cmclient --config=(debug | release) install ...

using namespace soulng::util;

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        sngxml::xpath::Init();
        cmajor::symbols::Init();
        cmajor::build::Init();
    }
    ~InitDone()
    {
        cmajor::build::Done();
        cmajor::symbols::Done();
        sngxml::xpath::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

void PrintHelp()
{
    std::cout << "Cmajor client version " << version << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "cmclient [options] (push | remove | build | debug | install) (PROJECT.cmp | SOLUTION.cms) ..." << std::endl;
    std::cout << "| cmclient [options] show configuration" << std::endl;
    std::cout << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--messages | -m" << std::endl;
    std::cout << "  Show build messages." << std::endl;
    std::cout << "--debug | -d" << std::endl;
    std::cout << "  Print debugging output to console." << std::endl;
    std::cout << std::endl;
    std::cout << "cmclient [options] push PROJECT.cmp [to server SERVER]" << std::endl;
    std::cout << "  Push project to local build repository, or to build repository of SERVER." << std::endl;
    std::cout << "options for push:" << std::endl;
    std::cout << "--only | -o" << std::endl;
    std::cout << "  Push only given project. By default pushes also dependent projects." << std::endl;
    std::cout << std::endl;
    std::cout << "cmclient [options] remove PROJECT.cmp [from server SERVER]" << std::endl;
    std::cout << "  Remove project from local build repository, or from build repository of SERVER." << std::endl;
    std::cout << "options for remove:" << std::endl;
    std::cout << "--all | -a" << std::endl;
    std::cout << "  Remove also dependent projects." << std::endl;
    std::cout << std::endl;
    std::cout << "cmclient [options] build PROJECT.cmp [using server SERVER]" << std::endl;
    std::cout << "  Build project using local build repository, or using build repository of SERVER." << std::endl;
    std::cout << "options for build:" << std::endl;
    std::cout << "--push | -p" << std::endl;
    std::cout << "  Push project to repository first." << std::endl;
    std::cout << "--config=(debug|release) | -c=(debug|release)" << std::endl;
    std::cout << "  Build given configuration. Default: debug." << std::endl;
    std::cout << "--toolchain=TOOLCHAIN | -t=TOOLCHAIN" << std::endl;
    std::cout << "  Build using tool chain TOOLCHAIN where TOOLCHAIN=(clang | gcc | vs)." << std::endl;
    std::cout << "--only | -o" << std::endl;
    std::cout << "  Build only given project. By default builds also dependent projects." << std::endl;

    std::cout << std::endl;
    std::cout << "cmclient [options] debug PROJECT.cmp [using server SERVER]" << std::endl;
    std::cout << "  Debug project using local build repository, or using build repository of SERVER." << std::endl;
    std::cout << "options for debug:" << std::endl;
    std::cout << "--push | -p" << std::endl;
    std::cout << "  Push project to repository first." << std::endl;
    std::cout << "--build | -b" << std::endl;
    std::cout << "  Build project first." << std::endl;
    std::cout << "--toolchain=TOOLCHAIN | -t=TOOLCHAIN" << std::endl;
    std::cout << "  Debug using tool chain TOOLCHAIN where TOOLCHAIN=(clang | gcc | vs)." << std::endl;
    std::cout << std::endl;
    std::cout << "cmclient [options] install PROJECT.cmp to DIRECTORY [from server SERVER]" << std::endl;
    std::cout << "  Install executable of project to PROJECT.cmp to DIRECTORY." << std::endl;
    std::cout << "options for install:" << std::endl;
    std::cout << "--config=(debug|release) | -c=(debug|release)" << std::endl;
    std::cout << "  Install executable of given configuration. Default: debug." << std::endl;
    std::cout << "--toolchain=TOOLCHAIN | -t=TOOLCHAIN" << std::endl;
    std::cout << "  Install executable of given tool chain TOOLCHAIN where TOOLCHAIN=(clang | gcc | vs)." << std::endl;
    std::cout << "cmclient [options] show configuration" << std::endl;
    std::cout << "  Show configuration." << std::endl;
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        cmajor::symbols::SetBackEnd(cmajor::symbols::BackEnd::cmcpp);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::repository);
        SetToolChain("gcc");
        bool verbose = false;
        std::string options;
        std::string tokens;
        int tokenIndex = 0;
        std::string commandName;
        bool removeServerCommand = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                    cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
                }
                else if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--debug")
                {
                    cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::printDebugMessages);
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
                if (ostr.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(ostr, '=');
                    if (components.size() == 2)
                    {
                        if (components[0] == "c")
                        {
                            options.append("config=").append(components[1]);
                        }
                        else if (components[0] == "t")
                        {
                            options.append("toolchain=").append(components[1]);
                        }
                        else
                        {
                            throw std::runtime_error("unknown option '" + arg + "'");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("invalid option '" + arg + "'");
                    }
                }
                else
                {
                    for (char o : ostr)
                    {
                        if (!options.empty())
                        {
                            options.append(1, ' ');
                        }
                        switch (o)
                        {
                            case 'v':
                            {
                                verbose = true;
                                options.append("verbose");
                                cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
                                break;
                            }
                            case 'm':
                            {
                                options.append("messages");
                                break;
                            }
                            case 'h':
                            {
                                PrintHelp();
                                return 1;
                            }
                            case 'd':
                            {
                                options.append("debug");
                                cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::printDebugMessages);
                                break;
                            }
                            case 'p':
                            {
                                options.append("push");
                                break;
                            }
                            case 'r':
                            {
                                options.append("rebuild");
                                break;
                            }
                            case 'b':
                            {
                                options.append("build");
                                break;
                            }
                            case 'f':
                            {
                                options.append("force");
                                break;
                            }
                            case 'o':
                            {
                                options.append("only");
                                break;
                            }
                            case 'a':
                            {
                                options.append("all");
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
                if (!tokens.empty())
                {
                    tokens.append(1, ' ');
                }
                if (tokenIndex == 0)
                {
                    commandName = arg;
                }
                else if (tokenIndex == 1 && arg == "server")
                {
                    removeServerCommand = true;
                }
                else if ((tokenIndex == 1 && (commandName == "push" || commandName == "remove" || commandName == "build" || commandName == "debug" || commandName == "install")) ||
                    (tokenIndex == 3 && commandName == "install"))
                {
                    arg = "<" + arg + ">";
                }
                tokens.append(arg);
                ++tokenIndex;
            }
        }
        if (verbose)
        {
            std::cout << "Cmajor client version " << version << std::endl;
        }
        std::string commandStr;
        if (!options.empty())
        {
            commandStr.append(1, '[').append(options).append(1, ']').append(1, ' ');
        }
        ReadToolChains(cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose));
        commandStr.append(tokens);
        std::unique_ptr<cmajor::build::ClientCommand> command = cmajor::build::ParseClientCommand(commandStr);
        command->Execute();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return  0;
}
