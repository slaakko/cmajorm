// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Archive.hpp>
#include <soulng/util/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <system-x/machine/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <stdexcept>
#include <iostream>

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
    std::cout << "System X Archiver version " << Version() << std::endl;
    std::cout << "Usage: sxar [options] { OBJECTFILE.o }" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--archive ARCHIVEFILE.a | -a ARCHIVEFILE.a" << std::endl;
    std::cout << "  Set archive file name to ARCHIVEFILE.a." << std::endl;
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
        bool prevWasArchive = false;
        std::string archiveFilePath;
        std::vector<std::string> objectFileNames;
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
                else if (arg == "--archive")
                {
                    prevWasArchive = true;
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
                        case 'a':
                        {
                            prevWasArchive = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else if (prevWasArchive)
            {
                prevWasArchive = false;
                archiveFilePath = GetFullPath(arg);
            }
            else
            {
                objectFileNames.push_back(GetFullPath(arg));
            }
        }
        if (archiveFilePath.empty())
        {
            throw std::runtime_error("archive file not specified");
        }
        if (objectFileNames.empty())
        {
            throw std::runtime_error("object files not specified");
        }
        cmsx::object::CreateArchive(0, archiveFilePath, objectFileNames, verbose);
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}
