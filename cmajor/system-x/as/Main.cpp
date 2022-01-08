// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Interface.hpp>
#include <system-x/assembler/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <string>
#include <iostream>
#include <vector>

using namespace soulng::util;

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    cmsx::machine::Init();
    cmsx::assembler::Init();
}

void DoneApplication()
{
    cmsx::assembler::Done();
    cmsx::machine::Done();
    sngxml::xpath::Done();
    soulng::util::Done();
}

std::string Version()
{
    return "4.3.0";
}

void PrintHelp()
{
    std::cout << "System X Assembler version " << Version() << std::endl;
    std::cout << "Usage: sxas [options] { FILE.s }" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose" << std::endl;
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        bool verbose = false;
        std::vector<std::string> fileNames;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg.starts_with("--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
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
            else if (arg.starts_with("-"))
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
                        case 'h':
                        {
                            PrintHelp();
                            return 1;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) +"'");
                        }
                    }
                }
            }
            else
            {
                fileNames.push_back(GetFullPath(arg));
            }
        }
        for (const std::string& fileName : fileNames)
        {
            cmsx::assembler::Assemble(-1, fileName, Path::ChangeExtension(fileName, ".o"), verbose);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}