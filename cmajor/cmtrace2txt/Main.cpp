// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmtrace/Trace.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Trace.hpp>
#include <soulng/util/Unicode.hpp>
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
    std::cout << "--filter=\"<filter_expression>\" | -f=\"<filter_expression>\"" << std::endl;
    std::cout << "  Filter trace using given filter expression." << std::endl;
}

bool CheckCmajorRootEnv()
{
    try
    {
        soulng::unicode::CmajorRoot();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return false;
    }
    return true;
}

int main(int argc, const char** argv)
{
    if (!CheckCmajorRootEnv()) return 1;
    InitDone initDone;
    try
    {
        std::string traceFilePath;
        bool verbose = false;
        std::string filterExpr;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (soulng::util::StartsWith(arg, "--"))
            {
                if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = soulng::util::Split(arg, '=');
                    if (components.size() >= 2)
                    {
                        std::string option = components[0];
                        std::string param = components[1];
                        for (int i = 2; i < components.size(); ++i)
                        {
                            param.append(std::string(1, '=')).append(components[i]);
                        }
                        if (option == "--filter")
                        {
                            filterExpr = param;
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
            }
            else if (soulng::util::StartsWith(arg, "-"))
            {
                if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = soulng::util::Split(arg, '=');
                    if (components.size() >= 2)
                    {
                        std::string option = components[0];
                        std::string param = components[1];
                        for (int i = 2; i < components.size(); ++i)
                        {
                            param.append(std::string(1, '=')).append(components[i]);
                        }
                        if (option == "-f")
                        {
                            filterExpr = param;
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
        std::unique_ptr<cmtrace::ContainerNode> root = cmtrace::ReadTrace(traceFilePath);
        if (!filterExpr.empty())
        {
            root = cmtrace::FilterTrace(root.get(), filterExpr);
        }
        std::string textFilePath = soulng::util::Path::ChangeExtension(traceFilePath, ".txt");
        cmtrace::WriteTextFile(root.get(), textFilePath, filterExpr);
        if (verbose)
        {
            std::cout << "==> " << textFilePath << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
