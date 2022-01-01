// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlSerGen.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace soulng::util;

void PrintHelp()
{
    std::cout << "xmlsergen [options] <file>" << std::endl;
    std::cout << "Generate XML serialization classes from given XML schema file." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h:" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose | -v:" << std::endl;
    std::cout << "  Be verbose." << std::endl;
}

void InitApplication()
{
    soulng::util::Init();
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        bool verbose = false;
        std::vector<std::string> filePaths;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
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
            else if (StartsWith(arg, "-"))
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
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                filePaths.push_back(arg);
            }
        }
        if (filePaths.empty())
        {
            throw std::runtime_error("no file paths given");
        }
        for (const std::string& filePath : filePaths)
        {
            if (verbose)
            {
                std::cout << "> " << filePath << std::endl;
            }
            sngxml::xmlser::GenerateXmlSerializationCode(filePath, verbose);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}