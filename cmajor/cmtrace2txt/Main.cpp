// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Trace.hpp>
#include <stdexcept>
#include <iostream>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
    }
    ~InitDone()
    {
        soulng::util::Done();
    }
};

void PrintHelp()
{
    std::cout << "Trace file conversion utility" << std::endl;
    std::cout << "Usage: cmtrace2txt [options] <trace.file>" << std::endl;
    std::cout << "Converts a binary trace file to a text file for inspection." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        std::string traceFilePath;
        bool verbose = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (soulng::util::StartsWith(arg, "--"))
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
            else if (soulng::util::StartsWith(arg, "-"))
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
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                traceFilePath = argv[i];
            }
        }
        if (traceFilePath.empty())
        {
            throw std::runtime_error("trace file name not given");
        }
        if (verbose)
        {
            std::cout << "> " << traceFilePath << std::endl;
        }
        soulng::util::ConvertTraceToText(traceFilePath);
        if (verbose)
        {
            std::cout << "==> " << soulng::util::Path::ChangeExtension(traceFilePath, ".txt") << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}