// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <sngjson/json/JsonImport.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <fstream>

using namespace soulng::util;
using namespace soulng::unicode;

ToolChains toolChains;

ToolChains& GetToolChains()
{
    return toolChains;
}

void CreateVSToolChain(bool verbose)
{
    ToolChain vs;
    vs.name = "vs";
    Tool compiler;
    compiler.name = "compiler";
    compiler.commandName = "cl";
    compiler.outputFileExtension = ".obj";
    compiler.assemblyFileExtension = ".asm";
    compiler.debugInformationFileExtension = ".pdb";
    compiler.args.push_back("/I\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\MSVC\\14.25.28610\\include\"");
    compiler.args.push_back("/c");
    compiler.args.push_back("$SOURCE_FILE$");
    compiler.args.push_back("/Od");
    compiler.args.push_back("/EHs");
    compiler.args.push_back("/Fo:$OBJECT_FILE$");
    compiler.args.push_back("/Fa:$ASSEMBLY_FILE$");
    compiler.args.push_back("/Fd:$DEBUG_INFORMATION_FILE$");
    compiler.args.push_back("/Zi");
    compiler.args.push_back("/FS");
    compiler.args.push_back("/std:c++latest");
    vs.tools.push_back(compiler);
    Tool libraryManager;
    libraryManager.name = "library-manager";
    libraryManager.commandName = "lib";
    libraryManager.outputFileExtension = ".lib";
    libraryManager.args.push_back("/VERBOSE");
    libraryManager.args.push_back("/MACHINE:X64");
    libraryManager.args.push_back("/OUT:$LIBRARY_FILE$");
    libraryManager.args.push_back("$OBJECT_FILES$");
    vs.tools.push_back(libraryManager);
    Tool linker;
    linker.name = "linker";
    linker.commandName = "cl";
    linker.outputFileExtension = ".exe";
    linker.debugInformationFileExtension = ".pdb";
    linker.args.push_back("$MAIN_OBJECT_FILE$");
    linker.args.push_back("$LIBRARY_FILES$");
    linker.args.push_back("/Fe:$EXECUTABLE_FILE$");
    linker.args.push_back("/Fd:$DEBUG_INFORMATION_FILE$");
    linker.args.push_back("/FS");
    linker.args.push_back("/link");
    linker.args.push_back("/MACHINE:X64");
    linker.args.push_back("/ENTRY:$ENTRY$");
    linker.args.push_back("/DEBUG");
    linker.args.push_back("/SUBSYSTEM:$SUBSYSTEM$");
    linker.args.push_back("/STACK:16777216");
    linker.args.push_back("/NODEFAULTLIB");
    vs.tools.push_back(linker);
    Tool projectFileGenerator;
    projectFileGenerator.name = "project-file-generator";
    projectFileGenerator.commandName = "cmvcxprojectfilegen";
    projectFileGenerator.outputFileExtension = ".vcxproj";
    projectFileGenerator.args.push_back("--verbose");
    projectFileGenerator.args.push_back("--name");
    projectFileGenerator.args.push_back("$PROJECT_NAME$");
    projectFileGenerator.args.push_back("--file");
    projectFileGenerator.args.push_back("$PROJECT_FILE_PATH$");
    projectFileGenerator.args.push_back("--target");
    projectFileGenerator.args.push_back("$PROJECT_TARGET$");
    projectFileGenerator.args.push_back("--config");
    projectFileGenerator.args.push_back("$PROJECT_CONFIG$");
    projectFileGenerator.args.push_back("$SOURCE_FILES$");
    vs.tools.push_back(projectFileGenerator);
    Tool solutionFileGenerator;
    solutionFileGenerator.name = "solution-file-generator";
    solutionFileGenerator.commandName = "cmslnfilegen";
    solutionFileGenerator.outputFileExtension = ".sln";
    solutionFileGenerator.args.push_back("$SOLUTION_NAME$");
    solutionFileGenerator.args.push_back("$SOLUTION_FILE_PATH$");
    solutionFileGenerator.args.push_back("$PROJECT_FILES$");
    vs.tools.push_back(solutionFileGenerator);
    toolChains.toolChains.push_back(vs);
    if (verbose)
    {
        std::cout << "vs tool chain created" << std::endl;
    }
}

void CreateToolChains(bool verbose)
{
    CreateVSToolChain(verbose);
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
        std::cout << "> " << toolChainConfigFilePath << std::endl;
    }
    std::u32string jsonStr = ToUtf32(ReadFile(toolChainConfigFilePath));
    JsonLexer lexer(jsonStr, toolChainConfigFilePath, 0);
    std::unique_ptr<JsonValue> jsonValue = JsonParser::Parse(lexer);
    sngjson::json::FromJson(jsonValue.get(), toolChains);
}

void ShowToolChains()
{
    CodeFormatter formatter(std::cout);
    formatter.WriteLine("tool chains:");
    for (const ToolChain& toolChain : toolChains.toolChains)
    {
        formatter.WriteLine(toolChain.name + ":");
        formatter.IncIndent();
        for (const Tool& tool : toolChain.tools)
        {
            formatter.WriteLine("name: " + tool.name);
            formatter.WriteLine("commandName: " + tool.commandName);
            formatter.WriteLine("outputFileExtension: " + tool.outputFileExtension);
            formatter.WriteLine("debugInformationFileExtension: " + tool.debugInformationFileExtension);
            formatter.WriteLine("args:");
            formatter.IncIndent();
            for (const std::string& arg : tool.args)
            {
                formatter.WriteLine(arg);
            }
            formatter.DecIndent();
        }
        formatter.DecIndent();
    }
}

void WriteToolChains(bool verbose)
{
    std::string toolChainConfigFilePath = GetFullPath(ToolChainConfigFilePath());
    std::ofstream toolChainFile(toolChainConfigFilePath);
    CodeFormatter formatter(toolChainFile);
    std::unique_ptr<JsonValue> jsonValue = toolChains.ToJson();
    std::string jsonStr = jsonValue->ToString();
    formatter.WriteLine(jsonStr);
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

const Tool& GetCompilerTool()
{
    ToolChains& toolChains = GetToolChains();
    for (const ToolChain& tc : toolChains.toolChains)
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
            throw std::runtime_error("'compiler' tool not found from tool chain '" + toolChain + "'");
        }
    }
    throw std::runtime_error("tool chain '" + toolChain + "' not found");
}

const Tool& GetLibraryManagerTool()
{
    const ToolChains& toolChains = GetToolChains();
    for (const ToolChain& tc : toolChains.toolChains)
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
            throw std::runtime_error("'library-manager' tool not found from tool chain '" + toolChain + "'");
        }
    }
    throw std::runtime_error("'" + toolChain + "' tool chain not found");
}

const Tool& GetLinkerTool()
{
    const ToolChains& toolChains = GetToolChains();
    for (const ToolChain& tc : toolChains.toolChains)
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
            throw std::runtime_error("'linker' tool not found from tool chain '" + toolChain + "'");
        }
    }
    throw std::runtime_error("'" + toolChain + "' tool chain not found");
}

const Tool& GetProjectFileGeneratorTool()
{
    const ToolChains& toolChains = GetToolChains();
    for (const ToolChain& tc : toolChains.toolChains)
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
            throw std::runtime_error("'project-file-generator' tool not found from tool chain '" + toolChain + "'");
        }
    }
    throw std::runtime_error("'" + toolChain + "' tool chain not found");
}
