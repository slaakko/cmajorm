// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Load.hpp>
#include <system-x/machine/Machine.hpp>
#include <soulng/util/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <iostream>
#include <stdexcept>

using namespace soulng::util;

void InitApplication()
{
    soulng::util::Init();
    cmsx::machine::Init();
}

void DoneApplication()
{
    cmsx::machine::Done();
    soulng::util::Done();
}

int main(int argc, const char** argv)
{
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
                if (arg == "--verbose")
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
                args.push_back(GetFullPath(arg));
            }
            else
            {
                args.push_back(arg);
            }
        }
        cmsx::machine::Machine machine;
        cmsx::object::Load(args[0], 0, args, env, machine);
        machine.GetProcessor().Run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}