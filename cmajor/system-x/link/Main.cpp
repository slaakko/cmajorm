// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/BinaryFile.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>

using namespace soulng::util;

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
    cmsx::machine::Init();
}

void DoneApplication()
{
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
    std::cout << "System X Linker version " << Version() << std::endl;
    std::cout << "Usage: sxlink [options] { FILE.o | FILE.a }" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--executable FILE.x | -x FILE.x" << std::endl;
    std::cout << "  Set executable output file." << std::endl;
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        bool verbose = false;
        bool prevWasExecutable = false;
        std::string executableFilePath;
        std::string clsIdFileName;
        std::vector<std::string> binaryFileNames;
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
                else if (arg == "--executable")
                {
                    prevWasExecutable = true;
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
                        case 'x':
                        {
                            prevWasExecutable = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else if (prevWasExecutable)
            {
                prevWasExecutable = false;
                executableFilePath = GetFullPath(arg);
            }
            else
            {
                binaryFileNames.push_back(GetFullPath(arg));
            }
        }
        if (executableFilePath.empty())
        {
            throw std::runtime_error("executable file not set");
        }
        cmsx::object::Link(0, executableFilePath, binaryFileNames, clsIdFileName, verbose);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}
