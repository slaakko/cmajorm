// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
    }
    ~InitDone()
    {
        cmajor::symbols::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

const char* version = "3.10.0";

void PrintHelp()
{
#ifdef _WIN32
    std::cout << "Cmajor Module Dump " << version << " for Windows x64" << std::endl;
#else
    std::cout << "Cmajor Module Dump " << version << std::endl;
#endif
}

using namespace cmajor::symbols;
using namespace cmajor::binder;

int main(int argc, const char** argv)
{
    std::unique_ptr<Module> rootModule;
    try
    {
        InitDone initDone;
        if (argc < 2)
        {
            PrintHelp();
            return 0;
        }
        SetGlobalFlag(GlobalFlags::info);
        std::vector<std::string> moduleFilePaths;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (!arg.empty() && arg[0] == '-')
            {
                if (arg == "--help" || arg == "-h")
                {
                    PrintHelp();
                    return 0;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }

            }
            else
            {
                moduleFilePaths.push_back(arg);
            }
        }
        if (moduleFilePaths.empty())
        {
            PrintHelp();
            return 0;
        }
        for (const std::string& moduleFilePath : moduleFilePaths)
        {
            boost::filesystem::path cmmPath(moduleFilePath);
            if (cmmPath.extension() != ".cmm")
            {
                throw std::runtime_error("Argument '" + cmmPath.generic_string() + "' has invalid extension. Not Cmajor module file (.cmm).");
            }
            if (!boost::filesystem::exists(cmmPath))
            {
                throw std::runtime_error("Cmajor module file '" + moduleFilePath + "' not found.");
            }
            rootModule.reset(new Module(moduleFilePath));
            rootModule->SetRootModule();
            SetRootModuleForCurrentThread(rootModule.get());
            if (rootModule->Name() == U"System.Base")
            {
                cmajor::symbols::MetaInit(rootModule->GetSymbolTable());
            }
            CompileUnitNode compileUnit(Span(), boost::uuids::nil_uuid(), "foo");
            AttributeBinder attributeBinder(rootModule.get());
            rootModule->Dump();
        }
    }
    catch (const Exception& ex)
    {
        std::cerr << ex.What() << std::endl;
        return 1;
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
