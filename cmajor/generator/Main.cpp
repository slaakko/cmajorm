// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/syntax/Generator.hpp>
#include <cmajor/syntax/CodeGenerator.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/InitDone.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>

#if defined(_MSC_VER) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <stdlib.h>
    #include <crtdbg.h>
#endif

class InitDone
{
public:
    InitDone()
    {
        cmajor::parsing::Init();
        cmajor::util::Init();
    }
    ~InitDone()
    {
        cmajor::util::Done();
        cmajor::parsing::Done();
    }
};

using cmajor::util::Split;

void AddToLibraryDirectories(const std::string& libraryPath, std::vector<std::string>& libraryDirectories)
{
    std::vector<std::string> dirs = Split(libraryPath, ';');
    int n = int(dirs.size());
    for (int i = 0; i < n; ++i)
    {
        std::string dir = dirs[i];
        libraryDirectories.push_back(dir);
    }
}

const char* version = "3.0";

int main(int argc, const char** argv)
{
#if defined(_MSC_VER) && !defined(NDEBUG)
    int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
    _CrtSetDbgFlag(dbgFlags);
    //_CrtSetBreakAlloc(40183);
#endif // defined(_MSC_VER) && !defined(NDEBUG)
    try
    {
        std::cout << "Cmajor Parser Generator version " << version << std::endl;
        if (argc < 2)
        {
            std::cout << "Usage: cmpg [options] {<file>.pp}\n" << "Compile Cmajor parsing project file(s) <file>.pp..." << std::endl;
            std::cout << "options:\n";
            std::cout << "-L <dir1>;<dir2>;...: add <dir1>, <dir2>, ... to library reference directories" << std::endl;
            std::cout << "-F                  : force code generation" << std::endl;
            return 1;
        }
        std::vector<std::string> projectFilePaths;
        std::vector<std::string> libraryDirectories;
        std::string cmajor_root;
        const char* cmajor_root_env = getenv("CMAJOR_ROOT");
        if (cmajor_root_env)
        {
            cmajor_root = cmajor_root_env;
        }
        if (cmajor_root.empty())
        {
            std::cerr << "please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory." << std::endl;
            return 2;
        }
        std::string libraryPath = (boost::filesystem::path(cmajor_root) / boost::filesystem::path("parsing")).generic_string();
        AddToLibraryDirectories(libraryPath, libraryDirectories);
        bool prevWasL = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "-L")
            {
                prevWasL = true;
            }
            else if (arg == "-F")
            {
                cmajor::syntax::SetForceCodeGen(true);
            }
            else if (prevWasL)
            {
                prevWasL = false;
                AddToLibraryDirectories(arg, libraryDirectories);
            }
            else
            {
                projectFilePaths.push_back(arg);
            }
        }
        InitDone initDone;
        int n = int(projectFilePaths.size());
        for (int i = 0; i < n; ++i)
        {
            const std::string& projectFilePath = projectFilePaths[i];
            cmajor::syntax::Generate(projectFilePath, libraryDirectories);
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 3;
    }
    catch (...)
    {
        std::cerr << "unknown exception" << std::endl;
        return 4;
    }
    return 0;
}
