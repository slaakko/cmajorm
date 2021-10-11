// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/cmdoclib/Global.hpp>
#include <cmajor/cmdoclib/Style.hpp>
#include <cmajor/cmdoclib/InitDone.hpp>
#include <cmajor/cmdoclib/Doc.hpp>
#include <cmajor/cmdoclib/ParserDoc.hpp>
#include <cmajor/cmdoclib/SymbolTableXml.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Util.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>

void InitApplication()
{
    soulng::util::Init();
    cmajor::cmdoclib::Init();
    sngxml::xpath::Init();
    sngcm::ast::Init();
    cmajor::symbols::Init();
}

const char* version = "4.2.0";

void PrintHelp()
{
    std::cout << "Cmajor Documentation Generator version " << version << std::endl;
    std::cout << "Usage: cmdoc [options] { file.cmdoc.xml }" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "--verbose (-v)" << std::endl;
    std::cout << "  be verbose" << std::endl;
    std::cout << "--help (-h)" << std::endl;
    std::cout << "  print this help" << std::endl;
    std::cout << "--optimize (-o)" << std::endl;
    std::cout << "  optimize output generation" << std::endl;
    std::cout << "--build-threads=N (-bt=N)" << std::endl;
    std::cout << "  set number of build threads to N\n" << std::endl;
}

using namespace cmajor::cmdoclib;
using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;
using namespace cmajor::build;

int main(int argc, const char** argv)
{
    std::vector<std::unique_ptr<Module>> rootModules;
    try
    {
        InitApplication();
        SetGlobalFlag(GlobalFlags::cmdoc);
        SetGlobalFlag(GlobalFlags::singleThreadedCompile);
        std::vector<std::string> cmdocXmlFilePaths;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!arg.empty() && arg[0] == '-')
            {
                if (arg == "--verbose" || arg == "-v")
                {
                    SetGlobalFlag(GlobalFlags::verbose);
                }
                else if (arg == "--help" || arg == "-h")
                {
                    PrintHelp();
                }
                else if (arg == "--optimize" || arg == "-o")
                {
                    SetGlobalFlag(GlobalFlags::optimizeCmDoc);
                }
                else if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(arg, '=');
                    if (components.size() == 2)
                    {
                        if (components[0] == "--build-threads" || components[0] == "-bt")
                        {
                            int numBuildThreads = boost::lexical_cast<int>(components[1]);
                            SetNumBuildThreads(numBuildThreads);
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
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else
            {
                cmdocXmlFilePaths.push_back(arg);
            }
        }
        SetGlobalFlag(GlobalFlags::rebuild);
#ifndef _WIN32
        SetNumBuildThreads(1);
        SetGlobalFlag(GlobalFlags::singleThreadedCompile);
#endif
        for (const std::string& cmDocFilePath : cmdocXmlFilePaths)
        {
            Input input = ReadInputXml(cmDocFilePath); 
            SetInputPtr(&input);
            GenerateStyleFile();
            GenerateCodeStyleFile();
            rootModules.clear();
            std::u32string solutionName;
            std::vector<std::u32string> moduleNames;
            BuildSolution(input.solutionFilePath, rootModules, solutionName, moduleNames);
            rootModules.clear();
            WriteGlobals(moduleNames);
            WriteModuleXml(input.targetDirPath, moduleNames);
            ReadExternalModuleDocuments(&input);
            BuildDocs(solutionName, moduleNames);
            SetInputPtr(nullptr);
        }
    }
    catch (const soulng::lexer::ParsingException& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (const Exception& ex)
    {
        std::cerr << ex.What() << std::endl;
        return 1;
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
