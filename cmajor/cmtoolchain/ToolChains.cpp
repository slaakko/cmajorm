// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <sngjson/json/JsonImport.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <fstream>

using namespace soulng::util;
using namespace soulng::unicode;

Platforms platforms;

Platforms& GetPlatforms()
{
    return platforms;
}

std::string GetPlatform()
{
#ifdef _WIN32
    return "windows";
#else
    return "linux";
#endif
}

Platform& GetOrInsertPlatform(const std::string& name)
{
    Platforms& platforms = GetPlatforms();
    for (Platform& pl : platforms.platforms)
    {
        if (pl.name == name)
        {
            return pl;
        }
    }
    Platform pl;
    pl.name = name;
    platforms.platforms.push_back(pl);
    return GetOrInsertPlatform(name);
}

void CreateVSToolChain(bool verbose)
{
    Platform& windows = GetOrInsertPlatform("windows");

    ToolChain vs;
    vs.name = "vs";

    Tool compiler;
    compiler.name = "compiler";
    compiler.commandName = "cl";
    compiler.outputFileExtension = ".obj";

    Configuration compilerDebugConfig;
    compilerDebugConfig.name = "debug";
    compilerDebugConfig.outputDirectory = "x64/Debug";
    compiler.debugInformationFileExtension = ".pdb";
    compiler.assemblyFileExtension = ".asm";
    compiler.configurations.push_back(compilerDebugConfig);

    Configuration compilerReleaseConfig;
    compilerReleaseConfig.name = "release";
    compilerReleaseConfig.outputDirectory = "x64/Release";
    compiler.configurations.push_back(compilerReleaseConfig);
    vs.tools.push_back(compiler);

    Tool libraryManager;
    libraryManager.name = "library-manager";
    libraryManager.commandName = "lib";
    libraryManager.outputFileExtension = ".lib";

    Configuration libraryManagerDebugConfig;
    libraryManagerDebugConfig.name = "debug";
    libraryManagerDebugConfig.outputDirectory = "x64/Debug";
    libraryManager.configurations.push_back(libraryManagerDebugConfig);

    Configuration libraryManagerReleaseConfig;
    libraryManagerReleaseConfig.name = "release";
    libraryManagerReleaseConfig.outputDirectory = "x64/Release";
    libraryManager.configurations.push_back(libraryManagerReleaseConfig);

    vs.tools.push_back(libraryManager);

    Tool linker;
    linker.name = "linker";
    linker.commandName = "cl";
    linker.outputFileExtension = ".exe";
    linker.debugInformationFileExtension = ".pdb";

    Configuration linkerDebugConfig;
    linkerDebugConfig.name = "debug";
    linker.configurations.push_back(linkerDebugConfig);

    Configuration linkerReleaseConfig;
    linkerReleaseConfig.name = "release";
    linker.configurations.push_back(linkerReleaseConfig);

    vs.tools.push_back(linker);

    Tool projectFileGenerator;
    projectFileGenerator.name = "project-file-generator";
    projectFileGenerator.commandName = "cmvcxprojectfilegen";
    projectFileGenerator.outputFileExtension = ".vcxproj";

    Configuration projectFileGeneratorDebugConfig;
    projectFileGeneratorDebugConfig.name = "debug";
    projectFileGeneratorDebugConfig.outputDirectory = "x64/Debug";
    projectFileGeneratorDebugConfig.args.push_back("$RUNTIME_LIBS$pdcurses.lib;libbz2.lib;zlibstat.lib;ws2_32.lib;User32.lib;Advapi32.lib;cmrtsd.lib");
    projectFileGeneratorDebugConfig.args.push_back("--verbose");
    projectFileGeneratorDebugConfig.args.push_back("--name");
    projectFileGeneratorDebugConfig.args.push_back("$PROJECT_NAME$");
    projectFileGeneratorDebugConfig.args.push_back("--file");
    projectFileGeneratorDebugConfig.args.push_back("$PROJECT_FILE_PATH$");
    projectFileGeneratorDebugConfig.args.push_back("--target");
    projectFileGeneratorDebugConfig.args.push_back("$PROJECT_TARGET$");
    projectFileGeneratorDebugConfig.args.push_back("--config");
    projectFileGeneratorDebugConfig.args.push_back("$PROJECT_CONFIG$");
    projectFileGeneratorDebugConfig.args.push_back("--librarydirs");
    projectFileGeneratorDebugConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    projectFileGeneratorDebugConfig.args.push_back("--libs");
    projectFileGeneratorDebugConfig.args.push_back("$LIBRARY_FILE_NAMES$");
    projectFileGeneratorDebugConfig.args.push_back("$GENERATE_ASSEMBLY_FILE_OPTION$/FAs");
    projectFileGeneratorDebugConfig.args.push_back("$GENERATE_JUST_MY_CODE_OPTION$/JMC");
    projectFileGeneratorDebugConfig.args.push_back("$ENABLE_RUNTIME_TYPE_INFORMATION_OPTION$/GR");
    projectFileGeneratorDebugConfig.args.push_back("--options");
    projectFileGeneratorDebugConfig.args.push_back("$OPTIONS$");
    projectFileGeneratorDebugConfig.args.push_back("$SOURCE_FILES$");
    projectFileGenerator.configurations.push_back(projectFileGeneratorDebugConfig);

    Configuration projectFileGeneratorReleaseConfig;
    projectFileGeneratorReleaseConfig.name = "release";
    projectFileGeneratorReleaseConfig.outputDirectory = "x64/Release";
    projectFileGeneratorReleaseConfig.args.push_back("$RUNTIME_LIBS$pdcurses.lib;libbz2.lib;zlibstat.lib;ws2_32.lib;User32.lib;Advapi32.lib;cmrts.lib");
    projectFileGeneratorReleaseConfig.args.push_back("--verbose");
    projectFileGeneratorReleaseConfig.args.push_back("--name");
    projectFileGeneratorReleaseConfig.args.push_back("$PROJECT_NAME$");
    projectFileGeneratorReleaseConfig.args.push_back("--file");
    projectFileGeneratorReleaseConfig.args.push_back("$PROJECT_FILE_PATH$");
    projectFileGeneratorReleaseConfig.args.push_back("--target");
    projectFileGeneratorReleaseConfig.args.push_back("$PROJECT_TARGET$");
    projectFileGeneratorReleaseConfig.args.push_back("--config");
    projectFileGeneratorReleaseConfig.args.push_back("$PROJECT_CONFIG$");
    projectFileGeneratorReleaseConfig.args.push_back("--librarydirs");
    projectFileGeneratorReleaseConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    projectFileGeneratorReleaseConfig.args.push_back("--libs");
    projectFileGeneratorReleaseConfig.args.push_back("$LIBRARY_FILE_NAMES$");
    projectFileGeneratorReleaseConfig.args.push_back("$GENERATE_ASSEMBLY_FILE_OPTION$/FAs");
    projectFileGeneratorReleaseConfig.args.push_back("$GENERATE_JUST_MY_CODE_OPTION$/JMC");
    projectFileGeneratorReleaseConfig.args.push_back("$ENABLE_RUNTIME_TYPE_INFORMATION_OPTION$/GR");
    projectFileGeneratorReleaseConfig.args.push_back("--options");
    projectFileGeneratorReleaseConfig.args.push_back("$OPTIONS$");
    projectFileGeneratorReleaseConfig.args.push_back("$SOURCE_FILES$");
    projectFileGenerator.configurations.push_back(projectFileGeneratorReleaseConfig);

    vs.tools.push_back(projectFileGenerator);
    Tool solutionFileGenerator;
    solutionFileGenerator.name = "solution-file-generator";
    solutionFileGenerator.commandName = "cmslnfilegen";
    solutionFileGenerator.outputFileExtension = ".sln";

    Configuration solutionFileGeneratorDebugConfig;
    solutionFileGeneratorDebugConfig.name = "debug";
    solutionFileGeneratorDebugConfig.args.push_back("--verbose");
    solutionFileGeneratorDebugConfig.args.push_back("--name");
    solutionFileGeneratorDebugConfig.args.push_back("$SOLUTION_NAME$");
    solutionFileGeneratorDebugConfig.args.push_back("--file");
    solutionFileGeneratorDebugConfig.args.push_back("$SOLUTION_FILE_PATH$");
    solutionFileGeneratorDebugConfig.args.push_back("$PROJECT_FILE_PATHS$");
    solutionFileGenerator.configurations.push_back(solutionFileGeneratorDebugConfig);

    Configuration solutionFileGeneratorReleaseConfig;
    solutionFileGeneratorReleaseConfig.name = "release";
    solutionFileGeneratorReleaseConfig.args.push_back("--verbose");
    solutionFileGeneratorReleaseConfig.args.push_back("--name");
    solutionFileGeneratorReleaseConfig.args.push_back("$SOLUTION_NAME$");
    solutionFileGeneratorReleaseConfig.args.push_back("--file");
    solutionFileGeneratorReleaseConfig.args.push_back("$SOLUTION_FILE_PATH$");
    solutionFileGeneratorReleaseConfig.args.push_back("$PROJECT_FILE_PATHS$");
    solutionFileGenerator.configurations.push_back(solutionFileGeneratorReleaseConfig);

    vs.tools.push_back(solutionFileGenerator);

    windows.toolChains.push_back(vs);

    if (verbose)
    {
        std::cout << "vs tool chain created" << std::endl;
    }
}

void CreateWindowsGccToolChain(bool verbose)
{
    Platform& windows = GetOrInsertPlatform("windows");

    ToolChain gcc;
    gcc.name = "gcc";

    Tool compiler;
    compiler.name = "compiler";
    compiler.commandName = "g++";
    compiler.outputFileExtension = ".o";

    Configuration compilerDebugConfig;
    compilerDebugConfig.name = "debug";
    compilerDebugConfig.outputDirectory = ".";
    compilerDebugConfig.args.push_back("-std=c++17");
    compilerDebugConfig.args.push_back("-c");
    compilerDebugConfig.args.push_back("$SOURCE_FILE$");
    compilerDebugConfig.args.push_back("-o");
    compilerDebugConfig.args.push_back("$OBJECT_FILE$");
    compilerDebugConfig.args.push_back("-g");
    compilerDebugConfig.args.push_back("-O0");
    compiler.configurations.push_back(compilerDebugConfig);

    Configuration compilerReleaseConfig;
    compilerReleaseConfig.name = "release";
    compilerReleaseConfig.outputDirectory = ".";
    compilerReleaseConfig.args.push_back("-std=c++17");
    compilerReleaseConfig.args.push_back("-c");
    compilerReleaseConfig.args.push_back("$SOURCE_FILE$");
    compilerReleaseConfig.args.push_back("-o");
    compilerReleaseConfig.args.push_back("$OBJECT_FILE$");
    compilerReleaseConfig.args.push_back("-g");
    compilerReleaseConfig.args.push_back("-O2");
    compiler.configurations.push_back(compilerReleaseConfig);

    gcc.tools.push_back(compiler);

    Tool libraryManager;
    libraryManager.name = "library-manager";
    libraryManager.commandName = "ar";
    libraryManager.outputFileExtension = ".a";

    Configuration libraryManagerDebugConfig;
    libraryManagerDebugConfig.name = "debug";
    libraryManagerDebugConfig.outputDirectory = ".";
    libraryManagerDebugConfig.args.push_back("rv");
    libraryManagerDebugConfig.args.push_back("$LIBRARY_FILE$");
    libraryManagerDebugConfig.args.push_back("$OBJECT_FILES$");
    libraryManager.configurations.push_back(libraryManagerDebugConfig);

    Configuration libraryManagerReleaseConfig;
    libraryManagerReleaseConfig.name = "release";
    libraryManagerReleaseConfig.outputDirectory = ".";
    libraryManagerReleaseConfig.args.push_back("rv");
    libraryManagerReleaseConfig.args.push_back("$LIBRARY_FILE$");
    libraryManagerReleaseConfig.args.push_back("$OBJECT_FILES$");
    libraryManager.configurations.push_back(libraryManagerReleaseConfig);

    gcc.tools.push_back(libraryManager);

    Tool linker;
    linker.name = "linker";
    linker.commandName = "g++";
    linker.outputFileExtension = ".exe";

    Configuration linkerDebugConfig;
    linkerDebugConfig.name = "debug";
    linkerDebugConfig.args.push_back("$LIBRARY_PATH_FLAG$-L");
    linkerDebugConfig.args.push_back(std::string("$RUNTIME_LIBS$") +
        "-lcmrtsd;-lcmsnglexerd;-lcmsngparserd;-lcmsngxmldomd;-lcmsngxmlxmld;-lcmsngxmlxpathd;-lcmehd;-lcmsngutild;-lpdcursesd;" +
        "-lbz2d;-lzd;-lwsock32;-lws2_32;-lboost_filesystem-mgw8-mt-sd-x64-1_73;-lboost_iostreams-mgw8-mt-sd-x64-1_73;-lboost_system-mgw8-mt-sd-x64-1_73");
    linkerDebugConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    linkerDebugConfig.args.push_back("$MAIN_OBJECT_FILE$");
    linkerDebugConfig.args.push_back("-Xlinker --start-group");
    linkerDebugConfig.args.push_back("$LIBRARY_FILES$");
    linkerDebugConfig.args.push_back("-Xlinker --end-group");
    linkerDebugConfig.args.push_back("-o");
    linkerDebugConfig.args.push_back("$EXECUTABLE_FILE$");
    linker.configurations.push_back(linkerDebugConfig);

    Configuration linkerReleaseConfig;
    linkerReleaseConfig.name = "release";
    linkerReleaseConfig.args.push_back("$LIBRARY_PATH_FLAG$-L");
    linkerReleaseConfig.args.push_back(std::string("$RUNTIME_LIBS$") +
        "-lcmrts;-lcmsnglexer;-lcmsngparser;-lcmsngxmldom;-lcmsngxmlxml;-lcmsngxmlxpath;-lcmeh;-lcmsngutil;-lpdcurses;" +
        "-lbz2;-lz;-lwsock32;-lws2_32;-lboost_filesystem-mgw8-mt-s-x64-1_73;-lboost_iostreams-mgw8-mt-s-x64-1_73;-lboost_system-mgw8-mt-s-x64-1_73");
    linkerReleaseConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    linkerReleaseConfig.args.push_back("$MAIN_OBJECT_FILE$");
    linkerReleaseConfig.args.push_back("-Xlinker --start-group");
    linkerReleaseConfig.args.push_back("$LIBRARY_FILES$");
    linkerReleaseConfig.args.push_back("-Xlinker --end-group");
    linkerReleaseConfig.args.push_back("-o");
    linkerReleaseConfig.args.push_back("$EXECUTABLE_FILE$");
    linker.configurations.push_back(linkerReleaseConfig);

    gcc.tools.push_back(linker);

    windows.toolChains.push_back(gcc);

    if (verbose)
    {
        std::cout << "Windows gcc tool chain created" << std::endl;
    }
}

void CreateLinuxClangToolChain(bool verbose)
{
    Platform& linux = GetOrInsertPlatform("linux");

    ToolChain clang;
    clang.name = "clang";

    Tool compiler;
    compiler.name = "compiler";
    compiler.commandName = "clang++";
    compiler.outputFileExtension = ".o";

    Configuration compilerDebugConfig;
    compilerDebugConfig.name = "debug";
    compilerDebugConfig.outputDirectory = ".";
    compilerDebugConfig.args.push_back("-std=c++17");
    compilerDebugConfig.args.push_back("-c");
    compilerDebugConfig.args.push_back("$SOURCE_FILE$");
    compilerDebugConfig.args.push_back("-o");
    compilerDebugConfig.args.push_back("$OBJECT_FILE$");
    compilerDebugConfig.args.push_back("-g");
    compilerDebugConfig.args.push_back("-O0");
    compiler.configurations.push_back(compilerDebugConfig);

    Configuration compilerReleaseConfig;
    compilerReleaseConfig.name = "release";
    compilerReleaseConfig.outputDirectory = ".";
    compilerReleaseConfig.args.push_back("-std=c++17");
    compilerReleaseConfig.args.push_back("-c");
    compilerReleaseConfig.args.push_back("$SOURCE_FILE$");
    compilerReleaseConfig.args.push_back("-o");
    compilerReleaseConfig.args.push_back("$OBJECT_FILE$");
    compilerReleaseConfig.args.push_back("-g");
    compilerReleaseConfig.args.push_back("-O2");
    compiler.configurations.push_back(compilerReleaseConfig);

    clang.tools.push_back(compiler);

    Tool libraryManager;
    libraryManager.name = "library-manager";
    libraryManager.commandName = "llvm-ar";
    libraryManager.outputFileExtension = ".a";

    Configuration libraryManagerDebugConfig;
    libraryManagerDebugConfig.name = "debug";
    libraryManagerDebugConfig.outputDirectory = ".";
    libraryManagerDebugConfig.args.push_back("rv");
    libraryManagerDebugConfig.args.push_back("$LIBRARY_FILE$");
    libraryManagerDebugConfig.args.push_back("$OBJECT_FILES$");
    libraryManager.configurations.push_back(libraryManagerDebugConfig);

    Configuration libraryManagerReleaseConfig;
    libraryManagerReleaseConfig.name = "release";
    libraryManagerReleaseConfig.outputDirectory = ".";
    libraryManagerReleaseConfig.args.push_back("rv");
    libraryManagerReleaseConfig.args.push_back("$LIBRARY_FILE$");
    libraryManagerReleaseConfig.args.push_back("$OBJECT_FILES$");
    libraryManager.configurations.push_back(libraryManagerReleaseConfig);

    clang.tools.push_back(libraryManager);

    Tool linker;
    linker.name = "linker";
    linker.commandName = "clang++";

    Configuration linkerDebugConfig;
    linkerDebugConfig.name = "debug";
    linkerDebugConfig.args.push_back("$LIBRARY_PATH_FLAG$-L");
    linkerDebugConfig.args.push_back(std::string("$RUNTIME_LIBS$") +
        "-lcmrtsd;-lcmsnglexerd;-lcmsngparserd;-lcmsngxmldomd;-lcmsngxmlxmld;-lcmsngxmlxpathd;-lcmehd;-lcmsngutild;" +
        "-lbz2;-lz;-lboost_filesystem;-lboost_iostreams;-lboost_system;-lncurses");
    linkerDebugConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    linkerDebugConfig.args.push_back("$MAIN_OBJECT_FILE$");
    linkerDebugConfig.args.push_back("-pthread");
    linkerDebugConfig.args.push_back("-Xlinker --start-group");
    linkerDebugConfig.args.push_back("$LIBRARY_FILES$");
    linkerDebugConfig.args.push_back("-Xlinker --end-group");
    linkerDebugConfig.args.push_back("-o");
    linkerDebugConfig.args.push_back("$EXECUTABLE_FILE$");
    linker.configurations.push_back(linkerDebugConfig);

    Configuration linkerReleaseConfig;
    linkerReleaseConfig.name = "release";
    linkerReleaseConfig.args.push_back("$LIBRARY_PATH_FLAG$-L");
    linkerReleaseConfig.args.push_back(std::string("$RUNTIME_LIBS$") +
        "-lcmrts;-lcmsnglexer;-lcmsngparser;-lcmsngxmldom;-lcmsngxmlxml;-lcmsngxmlxpath;-lcmeh;-lcmsngutil;" +
        "-lbz2;-lz;-lboost_filesystem;-lboost_iostreams;-lboost_system;-lncurses");
    linkerReleaseConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    linkerReleaseConfig.args.push_back("$MAIN_OBJECT_FILE$");
    linkerReleaseConfig.args.push_back("-pthread");
    linkerReleaseConfig.args.push_back("-Xlinker --start-group");
    linkerReleaseConfig.args.push_back("$LIBRARY_FILES$");
    linkerReleaseConfig.args.push_back("-Xlinker --end-group");
    linkerReleaseConfig.args.push_back("-o");
    linkerReleaseConfig.args.push_back("$EXECUTABLE_FILE$");
    linker.configurations.push_back(linkerReleaseConfig);

    clang.tools.push_back(linker);

    linux.toolChains.push_back(clang);

    if (verbose)
    {
        std::cout << "Linux clang tool chain created" << std::endl;
    }
}

void CreateLinuxGccToolChain(bool verbose)
{
    Platform& linux = GetOrInsertPlatform("linux");

    ToolChain gcc;
    gcc.name = "gcc";

    Tool compiler;
    compiler.name = "compiler";
    compiler.commandName = "g++";
    compiler.outputFileExtension = ".o";

    Configuration compilerDebugConfig;
    compilerDebugConfig.name = "debug";
    compilerDebugConfig.outputDirectory = ".";
    compilerDebugConfig.args.push_back("-std=c++17");
    compilerDebugConfig.args.push_back("-c");
    compilerDebugConfig.args.push_back("$SOURCE_FILE$");
    compilerDebugConfig.args.push_back("-o");
    compilerDebugConfig.args.push_back("$OBJECT_FILE$");
    compilerDebugConfig.args.push_back("-g");
    compilerDebugConfig.args.push_back("-O0");
    compiler.configurations.push_back(compilerDebugConfig);

    Configuration compilerReleaseConfig;
    compilerReleaseConfig.name = "release";
    compilerReleaseConfig.outputDirectory = ".";
    compilerReleaseConfig.args.push_back("-std=c++17");
    compilerReleaseConfig.args.push_back("-c");
    compilerReleaseConfig.args.push_back("$SOURCE_FILE$");
    compilerReleaseConfig.args.push_back("-o");
    compilerReleaseConfig.args.push_back("$OBJECT_FILE$");
    compilerReleaseConfig.args.push_back("-g");
    compilerReleaseConfig.args.push_back("-O2");
    compiler.configurations.push_back(compilerReleaseConfig);

    gcc.tools.push_back(compiler);

    Tool libraryManager;
    libraryManager.name = "library-manager";
    libraryManager.commandName = "ar";
    libraryManager.outputFileExtension = ".a";

    Configuration libraryManagerDebugConfig;
    libraryManagerDebugConfig.name = "debug";
    libraryManagerDebugConfig.outputDirectory = ".";
    libraryManagerDebugConfig.args.push_back("rv");
    libraryManagerDebugConfig.args.push_back("$LIBRARY_FILE$");
    libraryManagerDebugConfig.args.push_back("$OBJECT_FILES$");
    libraryManager.configurations.push_back(libraryManagerDebugConfig);

    Configuration libraryManagerReleaseConfig;
    libraryManagerReleaseConfig.name = "release";
    libraryManagerReleaseConfig.outputDirectory = ".";
    libraryManagerReleaseConfig.args.push_back("rv");
    libraryManagerReleaseConfig.args.push_back("$LIBRARY_FILE$");
    libraryManagerReleaseConfig.args.push_back("$OBJECT_FILES$");
    libraryManager.configurations.push_back(libraryManagerReleaseConfig);

    gcc.tools.push_back(libraryManager);

    Tool linker;
    linker.name = "linker";
    linker.commandName = "g++";

    Configuration linkerDebugConfig;
    linkerDebugConfig.name = "debug";
    linkerDebugConfig.args.push_back("$LIBRARY_PATH_FLAG$-L");
    linkerDebugConfig.args.push_back(std::string("$RUNTIME_LIBS$") +
        "-lcmrtsd;-lcmsnglexerd;-lcmsngparserd;-lcmsngxmldomd;-lcmsngxmlxmld;-lcmsngxmlxpathd;-lcmehd;-lcmsngutild;" +
        "-lbz2;-lz;-lboost_filesystem;-lboost_iostreams;-lboost_system;-lncurses");
    linkerDebugConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    linkerDebugConfig.args.push_back("$MAIN_OBJECT_FILE$");
    linkerDebugConfig.args.push_back("-pthread");
    linkerDebugConfig.args.push_back("-Xlinker --start-group");
    linkerDebugConfig.args.push_back("$LIBRARY_FILES$");
    linkerDebugConfig.args.push_back("-Xlinker --end-group");
    linkerDebugConfig.args.push_back("-o");
    linkerDebugConfig.args.push_back("$EXECUTABLE_FILE$");
    linker.configurations.push_back(linkerDebugConfig);

    Configuration linkerReleaseConfig;
    linkerReleaseConfig.name = "release";
    linkerReleaseConfig.args.push_back("$LIBRARY_PATH_FLAG$-L");
    linkerReleaseConfig.args.push_back(std::string("$RUNTIME_LIBS$") +
        "-lcmrts;-lcmsnglexer;-lcmsngparser;-lcmsngxmldom;-lcmsngxmlxml;-lcmsngxmlxpath;-lcmeh;-lcmsngutil;" +
        "-lbz2;-lz;-lboost_filesystem;-lboost_iostreams;-lboost_system;-lncurses");
    linkerReleaseConfig.args.push_back("$LIBRARY_DIRECTORIES$");
    linkerReleaseConfig.args.push_back("$MAIN_OBJECT_FILE$");
    linkerReleaseConfig.args.push_back("-pthread");
    linkerReleaseConfig.args.push_back("-Xlinker --start-group");
    linkerReleaseConfig.args.push_back("$LIBRARY_FILES$");
    linkerReleaseConfig.args.push_back("-Xlinker --end-group");
    linkerReleaseConfig.args.push_back("-o");
    linkerReleaseConfig.args.push_back("$EXECUTABLE_FILE$");
    linker.configurations.push_back(linkerReleaseConfig);

    gcc.tools.push_back(linker);

    linux.toolChains.push_back(gcc);

    if (verbose)
    {
        std::cout << "Linux gcc tool chain created" << std::endl;
    }
}

void CreateToolChains(bool verbose)
{
    CreateVSToolChain(verbose);
    CreateWindowsGccToolChain(verbose);
    CreateLinuxClangToolChain(verbose);
    CreateLinuxGccToolChain(verbose);
}

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return std::string(e);
}

std::string CmajorConfigDir()
{
    std::string rootDir = CmajorRootDir();
    return Path::Combine(rootDir, "config");
}

std::string ToolChainConfigFilePath()
{
    std::string configDir = CmajorConfigDir();
    return Path::Combine(configDir, "tool-chains.json");
}

void ReadToolChains(bool verbose)
{
    std::string toolChainConfigFilePath = GetFullPath(ToolChainConfigFilePath());
    if (verbose)
    {
        LogMessage(-1, "> " + toolChainConfigFilePath);
    }
    std::u32string jsonStr = ToUtf32(ReadFile(toolChainConfigFilePath));
    JsonLexer lexer(jsonStr, toolChainConfigFilePath, 0);
    std::unique_ptr<JsonValue> jsonValue = JsonParser::Parse(lexer);
    sngjson::json::FromJson(jsonValue.get(), platforms);
}

void ShowToolChains()
{
    CodeFormatter formatter(std::cout);
    std::unique_ptr<JsonValue> jsonValue = platforms.ToJson();
    jsonValue->Write(formatter);
}

void WriteToolChains(bool verbose)
{
    std::string toolChainConfigFilePath = GetFullPath(ToolChainConfigFilePath());
    std::ofstream toolChainFile(toolChainConfigFilePath);
    CodeFormatter formatter(toolChainFile);
    std::unique_ptr<JsonValue> jsonValue = platforms.ToJson();
    jsonValue->Write(formatter);
    if (verbose)
    {
        std::cout << "==> " << toolChainConfigFilePath << std::endl;
    }
}

std::string toolChain;

void SetToolChain(const std::string& toolChain_)
{
    toolChain = toolChain_;
}

std::string GetToolChain()
{
    return toolChain;
}

void ResetToolChain()
{
    toolChain = "gcc";
}

const Configuration& GetToolConfiguration(const Tool& tool, const std::string& config)
{
    for (const Configuration& configuration : tool.configurations)
    {
        if (configuration.name == config)
        {
            return configuration;
        }
    }
    throw std::runtime_error("'" + config + "' configuration not found from tool '" + tool.name + "'");
}

const Tool& GetCompilerTool(const std::string& platform, const std::string& toolChain)
{
    const Platforms& platforms = GetPlatforms();
    for (const Platform& pl : platforms.platforms)
    {
        if (pl.name == platform)
        {
            for (const ToolChain& tc : pl.toolChains)
            {
                if (tc.name == toolChain)
                {
                    for (const Tool& tool : tc.tools)
                    {
                        if (tool.name == "compiler")
                        {
                            return tool;
                        }
                    }
                    throw std::runtime_error("'compiler' tool not found from tool chain '" + toolChain + "' for platform '" + platform + "'");
                }
            }
            throw std::runtime_error("tool chain '" + toolChain + "' not found for platform '" + platform + "'");
        }
    }
    throw std::runtime_error("platform '" + platform + "' not found");
}

const Tool& GetLibraryManagerTool(const std::string& platform, const std::string& toolChain)
{
    const Platforms& platforms = GetPlatforms();
    for (const Platform& pl : platforms.platforms)
    {
        if (pl.name == platform)
        {
            for (const ToolChain& tc : pl.toolChains)
            {
                if (tc.name == toolChain)
                {
                    for (const Tool& tool : tc.tools)
                    {
                        if (tool.name == "library-manager")
                        {
                            return tool;
                        }
                    }
                    throw std::runtime_error("'library-manager' tool not found from tool chain '" + toolChain + "' for platform '" + platform + "'");
                }
            }
            throw std::runtime_error("tool chain '" + toolChain + "' not found for platform '" + platform + "'");
        }
    }
    throw std::runtime_error("platform '" + platform + "' not found");
}

const Tool& GetLinkerTool(const std::string& platform, const std::string& toolChain)
{
    const Platforms& platforms = GetPlatforms();
    for (const Platform& pl : platforms.platforms)
    {
        if (pl.name == platform)
        {
            for (const ToolChain& tc : pl.toolChains)
            {
                if (tc.name == toolChain)
                {
                    for (const Tool& tool : tc.tools)
                    {
                        if (tool.name == "linker")
                        {
                            return tool;
                        }
                    }
                    throw std::runtime_error("'linker' tool not found from tool chain '" + toolChain + "' for platform '" + platform + "'");
                }
            }
            throw std::runtime_error("tool chain '" + toolChain + "' not found for platform '" + platform + "'");
        }
    }
    throw std::runtime_error("platform '" + platform + "' not found");
}

const Tool& GetProjectFileGeneratorTool(const std::string& platform, const std::string& toolChain)
{
    const Platforms& platforms = GetPlatforms();
    for (const Platform& pl : platforms.platforms)
    {
        if (pl.name == platform)
        {
            for (const ToolChain& tc : pl.toolChains)
            {
                if (tc.name == toolChain)
                {
                    for (const Tool& tool : tc.tools)
                    {
                        if (tool.name == "project-file-generator")
                        {
                            return tool;
                        }
                    }
                    throw std::runtime_error("'project-file-generator' tool not found from tool chain '" + toolChain + "' for platform '" + platform + "'");
                }
            }
            throw std::runtime_error("tool chain '" + toolChain + "' not found for platform '" + platform + "'");
        }
    }
    throw std::runtime_error("platform '" + platform + "' not found");
}

const Tool& GetSolutionFileGeneratorTool(const std::string& platform, const std::string& toolChain)
{
    const Platforms& platforms = GetPlatforms();
    for (const Platform& pl : platforms.platforms)
    {
        if (pl.name == platform)
        {
            for (const ToolChain& tc : pl.toolChains)
            {
                if (tc.name == toolChain)
                {
                    for (const Tool& tool : tc.tools)
                    {
                        if (tool.name == "solution-file-generator")
                        {
                            return tool;
                        }
                    }
                    throw std::runtime_error("'solution-file-generator' tool not found from tool chain '" + toolChain + "' for platform '" + platform + "'");
                }
            }
            throw std::runtime_error("tool chain '" + toolChain + "' not found for platform '" + platform + "'");
        }
    }
    throw std::runtime_error("platform '" + platform + "' not found");
}
