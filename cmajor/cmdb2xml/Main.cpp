// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DebugInfoToXml.hpp>
#include <cmajor/cmdebug/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <stdexcept>
#include <iostream>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        cmajor::debug::Init();
    }
    ~InitDone()
    {
        cmajor::debug::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;

const char* version = "4.1.0";

void PrintHelp()
{
    std::cout << "Cmajor debug information file conversion utility version " << version << std::endl;
    std::cout << "Convert FILE.cmdb to FILE.xml for inspection." << std::endl;
    std::cout << "Usage: cmdb2xml [options] FILE.cmdb" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help | -h" << std::endl;
    std::cout << "  Print this help and exit." << std::endl;
    std::cout << "--verbose | -v" << std::endl;
    std::cout << "  Be verbose." << std::endl;
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        bool verbose = false;
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
                if (EndsWith(arg, ".cmdb"))
                {
                    std::string cmdbFilePath = GetFullPath(arg);
                    if (verbose)
                    {
                        std::cout << "> " << cmdbFilePath << std::endl;
                    }
                    std::string xmlDebugInfoFilePath = Path::ChangeExtension(cmdbFilePath, ".xml");
                    std::unique_ptr<sngxml::dom::Document> xmlDoc = cmajor::debug::GetDebugInfoAsXml(cmdbFilePath);
                    cmajor::debug::WriteXmlDebugInfo(xmlDoc.get(), xmlDebugInfoFilePath);
                    if (verbose)
                    {
                        std::cout << "==> " << xmlDebugInfoFilePath << std::endl;
                    }
                }
                else
                {
                    throw std::runtime_error("unknown file extension (not .cmdb)");
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
