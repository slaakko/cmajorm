// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/cmmid/InitDone.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Warning.hpp>
#ifdef _WIN32
#include <cmajor/cmres/InitDone.hpp>
#endif
#include <soulng/lexer/ParsingException.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/xpath/InitDone.hpp>
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

struct BackendSelector
{
    BackendSelector(cmajor::symbols::BackEnd backend)
    {
        switch (backend)
        {
            case cmajor::symbols::BackEnd::llvm:
            {
                CmmInit(cmajor::mid::BackEndKind::llvmBackEnd);
                break;
            }
            case cmajor::symbols::BackEnd::cmsx:
            {
                CmmInit(cmajor::mid::BackEndKind::cmsxBackEnd);
                break;
            }
        }
    }
    ~BackendSelector()
    {
        CmmDone();
    }
};

const char* version = "3.5.0";

void PrintHelp()
{
#ifdef _WIN32
    std::cout << "Cmajor compiler version " << version << " for Windows x64" << std::endl; 
#else
    std::cout << "Cmajor compiler version " << version << std::endl;
#endif
    std::cout << "Usage: cmc [options] { project.cmp | solution.cms }" << std::endl;
    std::cout << "Compiles given Cmajor solutions and projects." << std::endl;
    std::cout << "Options:\n" <<
        "--help (-h)\n" <<
        "   print this help message\n" <<
        "--config=CONFIG (-c=CONFIG)\n" <<
        "   set configuration to CONFIG (debug | release)\n" <<
        "   default is debug\n" <<
        "--backend=BACKEND (-be=BACKEND)\n" <<
        "   set compiler back-end to BACKEND : (llvm | cmsx)\n" <<
        "   default is llvm\n" <<
        "--optimization-level=LEVEL (-O=LEVEL)\n" <<
        "   set optimization level to LEVEL=0-3\n" <<
        "   defaults: debug=0, release=2\n" <<
        "--verbose (-v)\n" <<
        "   print verbose messages\n" <<
        "--quiet (-q)\n" <<
        "   print no messages\n" <<
        "--strict-nothrow (-s)\n" <<
        "   treat nothrow violation as an error\n" <<
        "--time (-t)\n" <<
        "   print duration of compilation\n" <<
        "--outdir=OUTDIR (-o=OUTDIR)\n" <<
        "   set output directory root to OUTDIR\n" <<
        "--rebuild (-u)\n" <<
        "   build although sources not changed\n" <<
        "--emit-llvm (-l)\n" <<
        "   emit intermediate LLVM code to file.ll files\n" <<
        "--emit-opt-llvm (-o)\n" <<
        "   emit optimized intermediate LLVM code to file.opt.ll files\n" <<
        "--clean (-e)\n" <<
        "   clean given solutions and projects\n" <<
        "--debug-parse (-p)\n" <<
        "   debug parsing to stdout\n" <<
        "--ast2xml (-x)\n" <<
        "   output AST of each source file as xml\n" <<
        "--sym2xml (-y)\n" <<
        "   output symbol table as xml\n" <<
        "--bdt2xml (-bd)\n" <<
        "   output bound tree as xml\n" <<
        "--link-with-debug-runtime (-d)\n" <<
        "   link with the debug version of the runtime library cmrt350(d).dll\n" <<
        "--link-using-ms-link (-m)\n" <<
        "   use Microsoft's link.exe as the linker\n" << 
        "--define SYMBOL (-D SYMBOL)\n" <<
        "   define a conditional compilation symbol SYMBOL.\n" <<
        "--gen-debug-info (-g)\n" <<
        "   generate debug info (on by default in debug configuration)\n" <<
        "--no-debug-info (-n)\n" <<
        "   don't generate debug info even for debug build\n" <<
        "--ide (-i)\n" <<
        "   set IDE mode: this mode is for Cmajor Development Environment.\n" <<
        "--msbuild (-b)\n" <<
        "   set MSBuild mode: this mode is for Visual Studio and MSBuild.\n" <<
        "--build-threads=N (-bt=N)\n" <<
        "   set number of build threads to N\n" <<
        "--disable-module-cache (-dm)\n" <<
        "   do not cache recently built modules\n" <<
        "--single-threaded-compile (-st)\n" <<
        "   compile source files in a project using a single thread\n" <<
        "--debug-compile (-dc)\n" <<
        "   show debug messages from multithreaded compilation\n" <<
        std::endl;
}

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;
using namespace cmajor::build;

#if(0)
void AddWarningsTo(sngxml::dom::Element* diagnosticsElement, Module* module)
{
    if (!module->WarningCollection().Warnings().empty())
    {
        for (const Warning& warning : module->WarningCollection().Warnings())
        {
            std::unique_ptr<sngxml::dom::Element> diagnosticElement(new sngxml::dom::Element(U"diagnostic"));
            std::unique_ptr<sngxml::dom::Element> categoryElement(new sngxml::dom::Element(U"category"));
            std::unique_ptr<sngxml::dom::Text> categoryText(new sngxml::dom::Text(U"warning"));
            categoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryText.release()));
            std::unique_ptr<sngxml::dom::Element> messageElement(new sngxml::dom::Element(U"message"));
            std::unique_ptr<sngxml::dom::Text> messageText(new sngxml::dom::Text(ToUtf32(warning.Message())));
            messageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageText.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryElement.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement.release()));
            std::unique_ptr<sngxml::dom::Element> spanElement = SpanToDomElement(module, warning.Defined());
            if (spanElement)
            {
                diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(spanElement.release()));
            }
            diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
            for (const Span& span : warning.References())
            {
                if (!span.Valid()) continue;
                std::unique_ptr<sngxml::dom::Element> diagnosticElement(new sngxml::dom::Element(U"diagnostic"));
                std::unique_ptr<sngxml::dom::Element> categoryElement(new sngxml::dom::Element(U"category"));
                std::unique_ptr<sngxml::dom::Text> categoryText(new sngxml::dom::Text(U"info"));
                categoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryText.release()));
                std::unique_ptr<sngxml::dom::Element> messageElement(new sngxml::dom::Element(U"message"));
                std::unique_ptr<sngxml::dom::Text> messageText(new sngxml::dom::Text(ToUtf32("see reference to")));
                messageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageText.release()));
                diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryElement.release()));
                diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement.release()));
                std::unique_ptr<sngxml::dom::Element> spanElement = SpanToDomElement(module, span);
                if (spanElement)
                {
                    diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(spanElement.release()));
                    diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
                }
            }
        }
    }
}
#endif 

int main(int argc, const char** argv)
{
    std::unique_ptr<Module> rootModule;
    std::vector<std::unique_ptr<Module>> rootModules;
    try
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        InitDone initDone;
        std::string projectName;
        std::string projectDirectory;
        std::string target = "program";
        std::vector<std::string> files;
        std::vector<std::string> sourceFiles;
        std::vector<std::string> resourceFiles;
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
                    else if (arg == "--ide" || arg == "-i")
                    {
                        SetGlobalFlag(GlobalFlags::ide);
                    }
                    else if (arg == "--msbuild" || arg == "-b")
                    {
                        SetGlobalFlag(GlobalFlags::msbuild);
                    }
                    else if (arg == "--debug-parse" || arg == "-p")
                    {
                        SetGlobalFlag(GlobalFlags::debugParsing);
                    }
                    else if (arg == "--ast2xml" || arg == "-x")
                    {
                        SetGlobalFlag(GlobalFlags::ast2xml);
                        SetGlobalFlag(GlobalFlags::singleThreadedCompile);
                    }
                    else if (arg == "--sym2xml" || arg == "-y")
                    {
                        SetGlobalFlag(GlobalFlags::sym2xml);
                        SetGlobalFlag(GlobalFlags::singleThreadedCompile);
                    }
                    else if (arg == "--bdt2xml" || arg == "-bd")
                    {
                        SetGlobalFlag(GlobalFlags::bdt2xml);
                        SetGlobalFlag(GlobalFlags::singleThreadedCompile);
                    }
                    else if (arg == "--strict-nothrow" || arg == "-s")
                    {
                        SetGlobalFlag(GlobalFlags::strictNothrow);
                    }
                    else if (arg == "--emit-llvm" || arg == "-l")
                    {
                        SetGlobalFlag(GlobalFlags::emitLlvm);
                    }
                    else if (arg == "--emit-opt-llvm" || arg == "-o")
                    {
                        SetGlobalFlag(GlobalFlags::emitLlvm);
                        SetGlobalFlag(GlobalFlags::emitOptLlvm);
                    }
                    else if (arg == "--link-with-debug-runtime" || arg == "-d")
                    {
                        SetGlobalFlag(GlobalFlags::linkWithDebugRuntime);
                    }
                    else if (arg == "--link-using-ms-link" || arg == "-m")
                    {
                        SetGlobalFlag(GlobalFlags::linkUsingMsLink);
                    }
                    else if (arg == "--time" || arg == "-t")
                    {
                        SetGlobalFlag(GlobalFlags::time);
                    }
                    else if (arg == "--rebuild" || arg == "-u")
                    {
                        SetGlobalFlag(GlobalFlags::rebuild);
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
                            else if (components[0] == "--backend" || components[0] == "-be")
                            {
                                if (components[1] == "llvm")
                                {
                                    SetBackEnd(cmajor::symbols::BackEnd::llvm);
                                }
                                else if (components[1] == "cmsx")
                                {
                                    SetBackEnd(cmajor::symbols::BackEnd::cmsx);
                                }
                                else
                                {
                                    throw std::runtime_error("unknown compile target '" + components[1] + "'");
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
                            else if (components[0] == "--resource")
                            {
                                std::string file = components[1];
                                boost::filesystem::path fp(file);
                                if (!boost::filesystem::exists(fp))
                                {
                                    throw std::runtime_error("resource XML file '" + fp.generic_string() + "' not found");
                                }
                                resourceFiles.push_back(file);
                            }
                            else if (components[0] == "--target" || components[0] == "-a")
                            {
                                target = components[1];
                                if (target != "program" && target != "winguiapp" && target != "winapp" && target != "library" && target != "winlib" && target != "unitTest")
                                {
                                    throw std::runtime_error("unknown target '" + target + "': not 'program', 'winguiapp', 'winapp', 'library', 'winlib' or 'unitTest'");
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
                std::cout << "Cmajor compiler version " << version << " for Windows x64" << std::endl;
#else
                std::cout << "Cmajor compiler version " << version << std::endl;
#endif
            }
#ifndef _WIN32
            noDebugInfo = true;
#endif
            SetUseModuleCache(useModuleCache);
            BackendSelector backend(GetBackEnd());
            if (!GetGlobalFlag(GlobalFlags::release) && !noDebugInfo)
            {
                SetGlobalFlag(GlobalFlags::generateDebugInfo);
            }
#ifndef _WIN32
            SetNumBuildThreads(1);
            SetGlobalFlag(GlobalFlags::singleThreadedCompile); 
#endif
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
                        BuildProject(GetFullPath(fp.generic_string()), rootModule);
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
            if (GetGlobalFlag(GlobalFlags::msbuild))
            {
                SetGlobalFlag(GlobalFlags::rebuild);
                BuildMsBuildProject(projectName, projectDirectory, target, sourceFiles, resourceFiles, referenceFiles, rootModule);
            }
            if (rootModule && !rootModule->WarningCollection().Warnings().empty())
            {
                if (!GetGlobalFlag(GlobalFlags::quiet) && !GetGlobalFlag(GlobalFlags::ide) && !GetGlobalFlag(GlobalFlags::msbuild))
                {
#if(0)
                    for (const Warning& warning : rootModule->WarningCollection().Warnings())
                    {
                        std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                        std::cerr << what << std::endl;
                    }
#endif
                }
                if (GetGlobalFlag(GlobalFlags::ide))
                {
#if(0)
                    for (const Warning& warning : rootModule->WarningCollection().Warnings())
                    {
                        std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                        std::cout << what << std::endl;
                    }
#endif
                    std::unique_ptr<JsonObject> compileResult(new JsonObject());
                    compileResult->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(true)));
#if(0)
                    if (!rootModule->WarningCollection().Warnings().empty())
                    {
                        JsonArray* warningsArray = new JsonArray();
                        for (const Warning& warning : rootModule->WarningCollection().Warnings())
                        {
                            warningsArray->AddItem(std::move(warning.ToJson(rootModule.get())));
                        }
                        compileResult->AddField(U"warnings", std::unique_ptr<JsonValue>(warningsArray));
                    }
#endif
                    std::cerr << compileResult->ToString() << std::endl;
                }
                else if (GetGlobalFlag(GlobalFlags::msbuild))
                {
                    sngxml::dom::Document compileResultDoc;
                    std::unique_ptr<sngxml::dom::Element> compileResultElement(new sngxml::dom::Element(U"compileResult"));
                    std::unique_ptr<sngxml::dom::Element> successElement(new sngxml::dom::Element(U"success"));
                    std::unique_ptr<sngxml::dom::Text> trueValue(new sngxml::dom::Text(U"true"));
                    successElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trueValue.release()));
                    compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(successElement.release()));
                    compileResultDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(compileResultElement.release()));
                    CodeFormatter formatter(std::cerr);
                    formatter.SetIndentSize(1);
                    compileResultDoc.Write(formatter);
                }
            }
            else if (GetGlobalFlag(GlobalFlags::ide))
            {
                std::unique_ptr<JsonObject> compileResult(new JsonObject());
                compileResult->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(true)));
                std::cerr << compileResult->ToString() << std::endl;
            }
            else if (GetGlobalFlag(GlobalFlags::msbuild))
            {
                sngxml::dom::Document compileResultDoc;
                std::unique_ptr<sngxml::dom::Element> compileResultElement(new sngxml::dom::Element(U"compileResult"));
                std::unique_ptr<sngxml::dom::Element> successElement(new sngxml::dom::Element(U"success"));
                std::unique_ptr<sngxml::dom::Text> trueValue(new sngxml::dom::Text(U"true"));
                successElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(trueValue.release()));
                compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(successElement.release()));
                compileResultDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(compileResultElement.release()));
                CodeFormatter formatter(std::cerr);
                formatter.SetIndentSize(1);
                compileResultDoc.Write(formatter);
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
        if (!GetGlobalFlag(GlobalFlags::quiet) && !GetGlobalFlag(GlobalFlags::ide) && !GetGlobalFlag(GlobalFlags::msbuild))
        {
            std::cerr << ex.what() << std::endl;
            if (rootModule)
            {
#if(0)
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                    std::cerr << what << std::endl;
                }
#endif
            }
        }
        if (GetGlobalFlag(GlobalFlags::ide))
        {
            std::cout << ex.what() << std::endl;
            if (rootModule)
            {
#if(0)
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                    std::cout << what << std::endl;
                }
#endif
            }
            std::unique_ptr<JsonObject> compileResult(new JsonObject());
            compileResult->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(false)));
            std::unique_ptr<JsonObject> json(new JsonObject());
            json->AddField(U"tool", std::unique_ptr<JsonValue>(new JsonString(U"cmc")));
            json->AddField(U"kind", std::unique_ptr<JsonValue>(new JsonString(U"error")));
            json->AddField(U"project", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ex.Project()))));
            json->AddField(U"message", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ex.Message()))));
            std::unique_ptr<JsonArray> refs(new JsonArray());
            std::unique_ptr<JsonObject> ref = SpanToJson(rootModule.get(), ex.GetSpan()); 
            if (ref)
            {
                refs->AddItem(std::move(ref));
            }
            json->AddField(U"references", std::move(refs));
            compileResult->AddField(U"diagnostics", std::move(json));
#if(0)
            if (rootModule && !rootModule->WarningCollection().Warnings().empty())
            {
                JsonArray* warningsArray = new JsonArray();
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    warningsArray->AddItem(std::move(warning.ToJson(rootModule.get())));
                }
                compileResult->AddField(U"warnings", std::unique_ptr<JsonValue>(warningsArray));
            }
#endif
            std::cerr << compileResult->ToString() << std::endl;
        }
        else if (GetGlobalFlag(GlobalFlags::msbuild))
        {
            std::cout << ex.what() << std::endl;
            if (rootModule)
            {
#if(0)
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                    std::cout << what << std::endl;
                }
#endif
            }
            sngxml::dom::Document compileResultDoc;
            std::unique_ptr<sngxml::dom::Element> compileResultElement(new sngxml::dom::Element(U"compileResult"));
            std::unique_ptr<sngxml::dom::Element> successElement(new sngxml::dom::Element(U"success"));
            std::unique_ptr<sngxml::dom::Text> falseValue(new sngxml::dom::Text(U"false"));
            successElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(falseValue.release()));
            compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(successElement.release()));
            std::unique_ptr<sngxml::dom::Element> diagnosticsElement(new sngxml::dom::Element(U"diagnostics"));
            std::unique_ptr<sngxml::dom::Element> diagnosticElement(new sngxml::dom::Element(U"diagnostic"));
            std::unique_ptr<sngxml::dom::Element> categoryElement(new sngxml::dom::Element(U"category"));
            std::unique_ptr<sngxml::dom::Text> categoryText(new sngxml::dom::Text(U"error"));
            categoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryText.release()));
            std::unique_ptr<sngxml::dom::Element> subcategoryElement(new sngxml::dom::Element(U"subcategory"));
            std::unique_ptr<sngxml::dom::Text> subcategoryText(new sngxml::dom::Text(U"error"));
            subcategoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subcategoryText.release()));
            std::unique_ptr<sngxml::dom::Element> messageElement(new sngxml::dom::Element(U"message"));
            std::unique_ptr<sngxml::dom::Text> messageText(new sngxml::dom::Text(ToUtf32(ex.Message())));
            messageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageText.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryElement.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subcategoryElement.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement.release()));
            std::unique_ptr<sngxml::dom::Element> spanElement = SpanToDomElement(rootModule.get(), ex.GetSpan());
            if (spanElement)
            {
                diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(spanElement.release()));
            }
            diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
#if(0)
            AddWarningsTo(diagnosticsElement.get(), rootModule.get());
#endif
            compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticsElement.release()));
            compileResultDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(compileResultElement.release()));
            CodeFormatter formatter(std::cerr);
            formatter.SetIndentSize(1);
            compileResultDoc.Write(formatter);
        }
        return 1;
    }
    catch (const Exception& ex)
    {
        if (!GetGlobalFlag(GlobalFlags::quiet) && !GetGlobalFlag(GlobalFlags::ide) && !GetGlobalFlag(GlobalFlags::msbuild))
        {
            std::cerr << ex.What() << std::endl;
#if(0)
            for (const Warning& warning : ex.GetModule()->WarningCollection().Warnings())
            {
                std::string what = Expand(ex.GetModule(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                std::cerr << what << std::endl;
            }
#endif
        }
        if (GetGlobalFlag(GlobalFlags::ide))
        {
            std::cout << ex.What() << std::endl;
#if(0)
            for (const Warning& warning : ex.GetModule()->WarningCollection().Warnings())
            {
                std::string what = Expand(ex.GetModule(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                std::cout << what << std::endl;
            }
#endif
            std::unique_ptr<JsonObject> compileResult(new JsonObject());
            compileResult->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(false)));
            compileResult->AddField(U"diagnostics", std::move(ex.ToJson()));
#if(0)
            if (!ex.GetModule()->WarningCollection().Warnings().empty())
            {
                JsonArray* warningsArray = new JsonArray();
                for (const Warning& warning : ex.GetModule()->WarningCollection().Warnings())
                {
                    warningsArray->AddItem(std::move(warning.ToJson(rootModule.get())));
                }
                compileResult->AddField(U"warnings", std::unique_ptr<JsonValue>(warningsArray));
            }
#endif
            std::cerr << compileResult->ToString() << std::endl;
        }
        else if (GetGlobalFlag(GlobalFlags::msbuild))
        {
            std::cout << ex.What() << std::endl;
#if(0)
            for (const Warning& warning : ex.GetModule()->WarningCollection().Warnings())
            {
                std::string what = Expand(ex.GetModule(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                std::cout << what << std::endl;
            }
#endif
            sngxml::dom::Document compileResultDoc;
            std::unique_ptr<sngxml::dom::Element> compileResultElement(new sngxml::dom::Element(U"compileResult"));
            std::unique_ptr<sngxml::dom::Element> successElement(new sngxml::dom::Element(U"success"));
            std::unique_ptr<sngxml::dom::Text> falseValue(new sngxml::dom::Text(U"false"));
            successElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(falseValue.release()));
            compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(successElement.release()));
            std::unique_ptr<sngxml::dom::Element> diagnosticsElement(new sngxml::dom::Element(U"diagnostics"));
            ex.AddToDiagnosticsElement(diagnosticsElement.get());
#if(0)
            AddWarningsTo(diagnosticsElement.get(), ex.GetModule());
#endif
            compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticsElement.release()));
            compileResultDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(compileResultElement.release()));
            CodeFormatter formatter(std::cerr);
            formatter.SetIndentSize(1);
            compileResultDoc.Write(formatter);
        }
        return 1;
    }
    catch (const std::exception& ex)
    {
        if (!GetGlobalFlag(GlobalFlags::quiet) && !GetGlobalFlag(GlobalFlags::ide) && !GetGlobalFlag(GlobalFlags::msbuild))
        {
            std::cerr << ex.what() << std::endl;
            if (rootModule)
            {
#if(0)
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                    std::cerr << what << std::endl;
                }
#endif
            }
        }
        if (GetGlobalFlag(GlobalFlags::ide))
        {
            std::cout << ex.what() << std::endl;
            if (rootModule)
            {
#if(0)
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                    std::cout << what << std::endl;
                }
#endif
            }
            std::unique_ptr<JsonObject> compileResult(new JsonObject());
            compileResult->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(false)));
            std::unique_ptr<JsonObject> diagnostics(new JsonObject());
            diagnostics->AddField(U"tool", std::unique_ptr<JsonValue>(new JsonString(U"cmc")));
            diagnostics->AddField(U"kind", std::unique_ptr<JsonValue>(new JsonString(U"error")));
            diagnostics->AddField(U"project", std::unique_ptr<JsonValue>(new JsonString(U"")));
            diagnostics->AddField(U"message", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(ex.what()))));
            compileResult->AddField(U"diagnostics", std::move(diagnostics));
#if(0)
            if (rootModule && !rootModule->WarningCollection().Warnings().empty())
            {
                JsonArray* warningsArray = new JsonArray();
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    warningsArray->AddItem(std::move(warning.ToJson(rootModule.get())));
                }
                compileResult->AddField(U"warnings", std::unique_ptr<JsonValue>(warningsArray));
            }
#endif
            std::cerr << compileResult->ToString() << std::endl;
        }
        else if (GetGlobalFlag(GlobalFlags::msbuild))
        {
            std::cout << ex.what() << std::endl;
            if (rootModule)
            {
#if(0)
                for (const Warning& warning : rootModule->WarningCollection().Warnings())
                {
                    std::string what = Expand(rootModule.get(), warning.Message(), warning.Defined(), warning.References(), "Warning");
                    std::cout << what << std::endl;
                }
#endif
            }
            sngxml::dom::Document compileResultDoc;
            std::unique_ptr<sngxml::dom::Element> compileResultElement(new sngxml::dom::Element(U"compileResult"));
            std::unique_ptr<sngxml::dom::Element> successElement(new sngxml::dom::Element(U"success"));
            std::unique_ptr<sngxml::dom::Text> falseValue(new sngxml::dom::Text(U"false"));
            successElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(falseValue.release()));
            compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(successElement.release()));
            std::unique_ptr<sngxml::dom::Element> diagnosticsElement(new sngxml::dom::Element(U"diagnostics"));
            std::unique_ptr<sngxml::dom::Element> diagnosticElement(new sngxml::dom::Element(U"diagnostic"));
            std::unique_ptr<sngxml::dom::Element> categoryElement(new sngxml::dom::Element(U"category"));
            std::unique_ptr<sngxml::dom::Text> categoryText(new sngxml::dom::Text(U"error"));
            std::unique_ptr<sngxml::dom::Element> subcategoryElement(new sngxml::dom::Element(U"subcategory"));
            std::unique_ptr<sngxml::dom::Text> subcategoryText(new sngxml::dom::Text(U"general"));
            subcategoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subcategoryText.release()));
            categoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryText.release()));
            std::unique_ptr<sngxml::dom::Element> messageElement(new sngxml::dom::Element(U"message"));
            std::unique_ptr<sngxml::dom::Text> messageText(new sngxml::dom::Text(ToUtf32(ex.what())));
            messageElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageText.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(categoryElement.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subcategoryElement.release()));
            diagnosticElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(messageElement.release()));
            diagnosticsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticElement.release()));
#if(0)
            AddWarningsTo(diagnosticsElement.get(), rootModule.get());
#endif
            compileResultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(diagnosticsElement.release()));
            compileResultDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(compileResultElement.release()));
            CodeFormatter formatter(std::cerr);
            formatter.SetIndentSize(1);
            compileResultDoc.Write(formatter);
        }
        return 1;
    }
    return 0;
}
