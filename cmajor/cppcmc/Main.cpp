// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#ifdef _WIN32
#include <cmajor/cmres/InitDone.hpp>
#include <Windows.h>
#endif
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <soulng/util/Util.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
        sngxml::xpath::Init();
#ifdef _WIN32
        cmajor::resources::Init();
#endif
    }
    ~InitDone()
    {
#ifdef _WIN32
        cmajor::resources::Done();
#endif
        sngxml::xpath::Done();
        cmajor::symbols::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

struct BackendInit
{
    BackendInit()
    {
        CmmInit(cmajor::mid::BackEndKind::cmcppBackEnd);
    }
    ~BackendInit()
    {
        CmmDone();
    }
};

const char* version = "3.7.1";

void PrintHelp()
{
#ifdef _WIN32
    std::cout << "Cmajor with C++ backend compiler version " << version << " for Windows x64" << std::endl;
#else
    std::cout << "Cmajor with C++ backend compiler version " << version << std::endl;
#endif
    std::cout << "Usage: cppcmc [options] { project.cmp | solution.cms }" << std::endl;
    std::cout << "Compiles given Cmajor solutions and projects to C++ libraries or executables." << std::endl;
    std::cout << "Options:\n" <<
        "--help (-h)\n" <<
        "   print this help message\n" <<
        "--config=CONFIG (-c=CONFIG)\n" <<
        "   set configuration to CONFIG (debug | release)\n" <<
        "   default is debug\n" <<
        "--optimization-level=LEVEL (-O=LEVEL)\n" <<
        "   set optimization level to LEVEL=0-3\n" <<
        "   defaults: debug=0, release=3\n" <<
        "--verbose (-v)\n" <<
        "   print verbose messages\n" <<
        "--quiet (-q)\n" <<
        "   print no messages\n" <<
        "--tool-chain=TOOL_CHAIN (-tc=TOOL_CHAIN)\n" <<
        "   use tool chain TOOL_CHAIN (default is 'gcc')\n" << 
        "--strict-nothrow (-s)\n" <<
        "   treat nothrow violation as an error\n" <<
        "--time (-t)\n" <<
        "   print duration of compilation\n" <<
        "--outdir=OUTDIR (-o=OUTDIR)\n" <<
        "   set output directory root to OUTDIR\n" <<
        "--rebuild (-u)\n" <<
        "   build although sources not changed\n" <<
        "--clean (-e)\n" <<
        "   clean given solutions and projects\n" <<
        "--debug-parse (-p)\n" <<
        "   debug parsing to stdout\n" <<
        "--define SYMBOL (-D SYMBOL)\n" <<
        "   define a conditional compilation symbol SYMBOL.\n" <<
        "--gen-debug-info (-g)\n" <<
        "   generate debug info (on by default in debug configuration)\n" <<
        "--no-debug-info (-n)\n" <<
        "   don't generate debug info even for debug build\n" <<
        "--build-threads=N (-bt=N)\n" <<
        "   set number of build threads to N\n" <<
        "--disable-module-cache (-dm)\n" <<
        "   do not cache recently built modules\n" <<
        "--single-threaded-compile (-st)\n" <<
        "   compile source files in a project using a single thread\n" <<
        "--debug-compile (-dc)\n" <<
        "   show debug messages from multithreaded compilation\n" <<
        "--disable-codegen (-dg)\n" <<
        "   disable code generation\n" <<
        "--emit-asm (-f)\n" <<
        "   emit assembly code into file.asm\n" <<
        "--just-my-code (-j)\n" <<
        "   enable Just My Code debugging\n" <<
        "--all (-a)\n" <<
        "   build all dependencies\n" <<
        "--repository (-rp)\n" <<
        "   build repository project\n" <<
        std::endl;
}

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;
using namespace cmajor::build;

int main(int argc, const char** argv)
{
    SetBackEnd(cmajor::symbols::BackEnd::cmcpp);
    SetToolChain("gcc");
    std::unique_ptr<Module> rootModule;
    std::vector<std::unique_ptr<Module>> rootModules;
    std::set<std::string> builtProjects;
    try
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        InitDone initDone;
        std::string projectName;
        std::string projectDirectory;
        std::string target = "program";
        std::vector<std::string> files;
        std::vector<std::string> sourceFiles;
        std::vector<std::string> referenceFiles;
        if (argc < 2)
        {
            PrintHelp();
        }
        else
        {
            SetCompilerVersion(version);
            bool prevWasDefine = false;
            bool noDebugInfo = false;
            bool useModuleCache = true;
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
                    else if (arg == "--verbose" || arg == "-v")
                    {
                        SetGlobalFlag(GlobalFlags::verbose);
                    }
                    else if (arg == "--quiet" || arg == "-q")
                    {
                        SetGlobalFlag(GlobalFlags::quiet);
                    }
                    else if (arg == "--clean" || arg == "-e")
                    {
                        SetGlobalFlag(GlobalFlags::clean);
                    }
                    else if (arg == "--debug-parse" || arg == "-p")
                    {
                        SetGlobalFlag(GlobalFlags::debugParsing);
                    }
                    else if (arg == "--strict-nothrow" || arg == "-s")
                    {
                        SetGlobalFlag(GlobalFlags::strictNothrow);
                    }
                    else if (arg == "--time" || arg == "-t")
                    {
                        SetGlobalFlag(GlobalFlags::time);
                    }
                    else if (arg == "--rebuild" || arg == "-u")
                    {
                        SetGlobalFlag(GlobalFlags::rebuild);
                    }
                    else if (arg == "--all" || arg == "-a")
                    {
                        SetGlobalFlag(GlobalFlags::buildAll);
                    }
                    else if (arg == "--repository" || arg == "-rp")
                    {
                        SetGlobalFlag(GlobalFlags::repository);
                    }
                    else if (arg == "--define" || arg == "-D")
                    {
                        prevWasDefine = true;
                    }
                    else if (arg == "--gen-debug-info" || arg == "-g")
                    {
                        SetGlobalFlag(GlobalFlags::generateDebugInfo);
                    }
                    else if (arg == "--no-debug-info" || arg == "-n")
                    {
                        noDebugInfo = true;
                    }
                    else if (arg == "--disable-module-cache" || arg == "-dm")
                    {
                        useModuleCache = false;
                    }
                    else if (arg == "--single-threaded-compile" || arg == "-st")
                    {
                        SetGlobalFlag(GlobalFlags::singleThreadedCompile);
                    }
                    else if (arg == "--debug-compile" || arg == "-dc")
                    {
                        SetGlobalFlag(GlobalFlags::debugCompile);
                    }
                    else if (arg == "--disable-codegen" || arg == "-dg")
                    {
                        SetGlobalFlag(GlobalFlags::disableCodeGen);
                    }
                    else if (arg == "--emit-asm" || arg == "-f")
                    {
                        SetGlobalFlag(GlobalFlags::emitLlvm);
                    }
                    else if (arg == "--just-my-code" || arg == "-j")
                    {
                        SetGlobalFlag(GlobalFlags::justMyCodeDebugging);
                    }
                    else if (arg.find('=') != std::string::npos)
                    {
                        std::vector<std::string> components = Split(arg, '=');
                        if (components.size() == 2)
                        {
                            if (components[0] == "--config" || components[0] == "-c")
                            {
                                if (components[1] == "release")
                                {
                                    SetGlobalFlag(GlobalFlags::release);
                                }
                                else if (components[1] != "debug")
                                {
                                    throw std::runtime_error("unknown configuration '" + components[1] + "'");
                                }
                            }
                            else if (components[0] == "--optimization-level" || components[0] == "-O")
                            {
                                int optimizationLevel = boost::lexical_cast<int>(components[1]);
                                if (optimizationLevel >= 0 && optimizationLevel <= 3)
                                {
                                    SetOptimizationLevel(optimizationLevel);
                                }
                                else
                                {
                                    throw std::runtime_error("unknown optimization level '" + components[1] + "'");
                                }
                            }
                            else if (components[0] == "--reference" || components[0] == "-r")
                            {
                                std::string file = components[1];
                                boost::filesystem::path fp(file);
                                if (!boost::filesystem::exists(fp))
                                {
                                    throw std::runtime_error("referenced project file '" + fp.generic_string() + "' not found");
                                }
                                referenceFiles.push_back(file);
                            }
                            else if (components[0] == "--target")
                            {
                                target = components[1];
                                if (target != "program" && target != "library" && target != "unitTest")
                                {
                                    throw std::runtime_error("unknown target '" + target + "': not 'program', 'library', or 'unitTest'");
                                }
                            }
                            else if (components[0] == "--name" || components[0] == "-N")
                            {
                                projectName = components[1];
                            }
                            else if (components[0] == "--dir" || components[0] == "-pd")
                            {
                                projectDirectory = components[1];
                            }
                            else if (components[0] == "--build-threads" || components[0] == "-bt")
                            {
                                int numBuildThreads = boost::lexical_cast<int>(components[1]);
                                SetNumBuildThreads(numBuildThreads);
                            }
                            else if (components[0] == "--outdir" || components[0] == "-o")
                            {
                                std::string outdir = components[1];
                                SetOutDir(outdir);
                            }
                            else if (components[0] == "--tool-chain" || components[0] == "-tc")
                            {
                                std::string toolChain = components[1];
                                SetToolChain(toolChain);
                            }
                            else
                            {
                                throw std::runtime_error("unknown option '" + arg + "'");
                            }
                        }
                        else
                        {
                            throw std::runtime_error("invalid argument '" + arg + "'");
                        }
                    }
                    else
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                }
                else if (prevWasDefine)
                {
                    prevWasDefine = false;
                    DefineCommandLineConditionalSymbol(ToUtf32(arg));
                }
                else
                {
                    files.push_back(arg);
                }
            }
            if (files.empty())
            {
                PrintHelp();
                return 0;
            }
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
#ifdef _WIN32
                std::cout << "Cmajor with C++ backend compiler version " << version << " for Windows x64" << std::endl;
#else
                std::cout << "Cmajor with C++ backend compiler version " << version << std::endl;
#endif
            }
#ifndef _WIN32
            noDebugInfo = true;
#endif
            SetUseModuleCache(useModuleCache);
            BackendInit backend;
            if (!GetGlobalFlag(GlobalFlags::release) && !noDebugInfo)
            {
                SetGlobalFlag(GlobalFlags::generateDebugInfo);
            }
#ifndef _WIN32
            SetNumBuildThreads(1);
            SetGlobalFlag(GlobalFlags::singleThreadedCompile);
#else
            int n = GetNumBuildThreads();
            if (n == -1)
            {
                SetNumBuildThreads(1);
            }
#endif
            ReadToolChains(GetGlobalFlag(GlobalFlags::verbose));
            if (GetGlobalFlag(GlobalFlags::verbose))
            {
                std::cout << "current platform is '" << GetPlatform() << "'" << std::endl;
                std::cout << "current tool chain is '" << GetToolChain() << "'" << std::endl;
            }
            if (GetToolChain() == "vs")
            {
                SetGlobalFlag(GlobalFlags::disableCodeGen);
                if (GetGlobalFlag(GlobalFlags::verbose))
                {
                    std::cout << "Note: Native code generation for 'vs' tool chain is disabled. Just generating C++ source files and Visual Studio C++ project and solution files. " <<
                        "You can generate native code by compiling those project and solution files using Visual Studio or msbuild." << std::endl;
                }
            }
            for (const std::string& file : files)
            {
                boost::filesystem::path fp(file);
                if (fp.extension() == ".cms")
                {
                    if (GetGlobalFlag(GlobalFlags::msbuild))
                    {
                        throw std::runtime_error("solution file '" + fp.generic_string() + "'  cannot be given in --msbuild mode");
                    }
                    else if (!boost::filesystem::exists(fp))
                    {
                        throw std::runtime_error("solution file '" + fp.generic_string() + "' not found");
                    }
                    else
                    {
                        BuildSolution(GetFullPath(fp.generic_string()), rootModules);
                    }
                }
                else if (fp.extension() == ".cmp")
                {
                    if (GetGlobalFlag(GlobalFlags::msbuild))
                    {
                        throw std::runtime_error("project file '" + fp.generic_string() + "'  cannot be given in --msbuild mode");
                    }
                    else if (!boost::filesystem::exists(fp))
                    {
                        throw std::runtime_error("project file '" + fp.generic_string() + "' not found");
                    }
                    else
                    {
                        BuildProject(GetFullPath(fp.generic_string()), rootModule, builtProjects);
                    }
                }
                else if (fp.extension() == ".cm")
                {
                    if (GetGlobalFlag(GlobalFlags::msbuild))
                    {
                        boost::filesystem::path f(projectDirectory);
                        f /= fp;
                        if (!boost::filesystem::exists(f))
                        {
                            throw std::runtime_error("source file '" + f.generic_string() + "' not found");
                        }
                        else
                        {
                            sourceFiles.push_back(GetFullPath(f.generic_string()));
                        }
                    }
                    else
                    {
                        throw std::runtime_error("single .cm source file '" + fp.generic_string() + "' cannot be given if not in --msbuild mode");
                    }
                }
                else
                {
                    if (GetGlobalFlag(GlobalFlags::msbuild))
                    {
                        throw std::runtime_error("Argument '" + fp.generic_string() + "' has invalid extension. Not Cmajor source (.cm) file.");
                    }
                    else
                    {
                        throw std::runtime_error("Argument '" + fp.generic_string() + "' has invalid extension. Not Cmajor solution (.cms) or project (.cmp) file.");
                    }
                }
            }
            if (rootModule && !rootModule->WarningCollection().Warnings().empty())
            {
                if (!GetGlobalFlag(GlobalFlags::quiet) && !GetGlobalFlag(GlobalFlags::ide) && !GetGlobalFlag(GlobalFlags::msbuild))
                {
                }
                if (GetGlobalFlag(GlobalFlags::ide))
                {
                    std::unique_ptr<JsonObject> compileResult(new JsonObject());
                    compileResult->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(true)));
                    std::cerr << compileResult->ToString() << std::endl;
                }
            }
            else if (GetGlobalFlag(GlobalFlags::ide))
            {
                std::unique_ptr<JsonObject> compileResult(new JsonObject());
                compileResult->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(true)));
                std::cerr << compileResult->ToString() << std::endl;
            }
            if (GetGlobalFlag(GlobalFlags::time))
            {
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                auto dur = end - start;
                long long totalSecs = std::chrono::duration_cast<std::chrono::seconds>(dur).count() + 1;
                int hours = static_cast<int>(totalSecs / 3600);
                int mins = static_cast<int>((totalSecs / 60) % 60);
                int secs = static_cast<int>(totalSecs % 60);
                std::cout <<
                    (hours > 0 ? std::to_string(hours) + " hour" + ((hours != 1) ? "s " : " ") : "") <<
                    (mins > 0 ? std::to_string(mins) + " minute" + ((mins != 1) ? "s " : " ") : "") <<
                    secs << " second" << ((secs != 1) ? "s" : "") << std::endl;
            }
        }
    }
    catch (const soulng::lexer::ParsingException& ex)
    {
        if (!GetGlobalFlag(GlobalFlags::quiet))
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    catch (const Exception& ex)
    {
        if (!GetGlobalFlag(GlobalFlags::quiet))
        {
            std::cerr << ex.What() << std::endl;
        }
        return 1;
    }
    catch (const std::exception& ex)
    {
        if (!GetGlobalFlag(GlobalFlags::quiet))
        {
            std::cerr << ex.what() << std::endl;
        }
        return 1;
    }
    return 0;
}
