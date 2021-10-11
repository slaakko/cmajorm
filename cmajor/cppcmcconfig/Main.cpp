#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngjson/json/JsonLexer.hpp>
#include <sngjson/json/JsonParser.hpp>
#include <soulng/util/Ansi.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

using namespace soulng::util;
using namespace soulng::unicode;

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return std::string(e);
}

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
}

void PrintHelp(CodeFormatter& formatter)
{
    formatter << fgDarkCyan << "Usage" << reset << ": " << fgWhite << " cppcmcconfig [options]" << reset << std::endl;
    formatter << "Print and/or change MinGW Boost configuration on Windows for cppcmc. " << std::endl;
    formatter << "If the program is started without options, it prints current values without making changes." << std::endl;
    formatter << "Options:" << std::endl;
    formatter << fgWhite << "--boost-lib-dir=" << reset << fgGreen << "BOOST_LIB_DIR" << reset << std::endl;
    formatter << "  where BOOST_LIB_DIR is an absolute path to the subdirectory whose name is " << fgWhite << "lib" << reset <<
        " that is under the Boost installation directory. It contains *.a files." << std::endl;
    formatter << "  Effect: configures the Boost lib-directory for cppcmc." << std::endl;
    formatter << fgWhite << "--boost-include-dir=" << reset << fgGreen << "BOOST_INCLUDE_DIR" << reset << std::endl;
    formatter << "  where BOOST_INCLUDE_DIR is an absolute path to the subdirectory whose name is " << fgWhite << "boost-1_XX" << reset <<
        " that is under the subdirectory whose name is " << fgWhite << "include" << reset << " that is under the Boost installation directory. "
        " It contains a subdirectory whose name is " << fgWhite << "boost" << reset << "." << std::endl;
    formatter << "  Effect: configures the Boost include-directory for cppcmc." << std::endl;
    formatter << fgWhite << "--boost-version=" << reset << fgGreen << "BOOST_VERSION_NUMBER" << reset << std::endl;
    formatter << "  where BOOST_VERSION_NUMBER is the version number of Boost in format \"1.XX\" or \"1_XX\"" << std::endl;
    formatter << "  Effect: configures the Boost version number for cppcmc." << std::endl;
}

void BoostLibDir(CodeFormatter& formatter, CodeFormatter& errorFormatter, const std::string& cmajorRootDir, const std::string& boostLibDir)
{
    std::string compilerConfigFilePath = GetFullPath(Path::Combine(Path::Combine(cmajorRootDir, "config"), "compiler-config.xml"));
    if (boost::filesystem::exists(compilerConfigFilePath))
    {
        formatter << fgDarkCyan << "info" << reset << ": " << fgWhite << compilerConfigFilePath << reset << " found." << std::endl;
        std::unique_ptr<sngxml::dom::Document> doc = sngxml::dom::ReadDocument(compilerConfigFilePath);
        std::unique_ptr<sngxml::xpath::XPathObject> object = sngxml::xpath::Evaluate(U"/compiler/windows/gcc/boost", doc.get());
        if (object)
        {
            if (object->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(object.get());
                if (nodeSet->Length() == 1)
                {
                    sngxml::dom::Node* node = (*nodeSet)[0];
                    if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                        std::u32string libraryDirAttr = element->GetAttribute(U"libraryDir");
                        if (!libraryDirAttr.empty())
                        {
                            std::string libraryDir = ToUtf8(libraryDirAttr);
                            formatter << fgDarkCyan << "info" << reset << ": boost lib-directory (1/1) path is " << fgWhite << libraryDir << reset << std::endl;
                            if (!boostLibDir.empty())
                            {
                                element->SetAttribute(U"libraryDir", ToUtf32(boostLibDir));
                                std::ofstream compileConfigFile(compilerConfigFilePath);
                                CodeFormatter compilerConfigFormatter(compileConfigFile);
                                doc->Write(compilerConfigFormatter);
                                formatter << fgGreen << "success" << reset << ": boost lib-directory path set to " << fgWhite << boostLibDir << reset << std::endl;
                            }
                        }
                        else
                        {
                            errorFormatter << fgRed << "error" << reset << ": " << fgWhite << "/compiler/windows/gcc/boost" << reset << " has no 'libraryDir' attribute in " <<
                                fgWhite << compilerConfigFilePath << reset << std::endl;
                        }
                    }
                    else
                    {
                        errorFormatter << fgRed << "error" << reset << ": " << fgWhite << "/compiler/windows/gcc/boost" << reset << " element not found from " <<
                            fgWhite << compilerConfigFilePath << reset << std::endl;
                    }
                }
                else
                {
                    errorFormatter << fgRed << "error" << reset << ": " << fgWhite << "/compiler/windows/gcc/boost" << reset << " element not found from " <<
                        fgWhite << compilerConfigFilePath << reset << std::endl;
                }
            }
            else
            {
                errorFormatter << fgRed << "error" << reset << ": " << fgWhite << "/compiler/windows/gcc/boost" << reset << " element not found from " <<
                    fgWhite << compilerConfigFilePath << reset << std::endl;
            }
        }
        else
        {
            errorFormatter << fgRed << "error" << reset << ": " << fgWhite << "/compiler/windows/gcc/boost" << reset << " element not found from " <<
                fgWhite << compilerConfigFilePath << reset << std::endl;
        }
    }
    else
    {
        errorFormatter << fgRed << "error" << reset << ": " << fgWhite << compilerConfigFilePath << reset << " not found" << std::endl;
    }
}

void BoostIncludeDirCommon(CodeFormatter& formatter, CodeFormatter& errorFormatter, const std::string& cmajorRootDir,
    const std::string& commonFilePath, const std::string& boostIncludeDir, int index, int count)
{
    std::vector<std::string> lines;
    {
        std::ifstream commonFile(commonFilePath);
        std::string line;
        while (std::getline(commonFile, line))
        {
            lines.push_back(line);
        }
        if (!lines.empty())
        {
            for (std::string& line : lines)
            {
                if (line.find("BOOST_INCLUDE_DIR=") != std::string::npos)
                {
                    formatter << fgDarkCyan << "info" << reset << ": boost include-directory (" << index << "/" << count << ") path is " << fgWhite << line.substr(std::string("BOOST_INCLUDE_DIR=").length()) << reset << std::endl;
                    if (!boostIncludeDir.empty())
                    {
                        line = line.substr(0, std::string("BOOST_INCLUDE_DIR=").length()) + boostIncludeDir;
                    }
                }
            }
        }
        else
        {
            errorFormatter << fgRed << "error" << reset << ": " << fgWhite << commonFilePath << reset << " is empty" << std::endl;
            return;
        }
    }
    if (!boostIncludeDir.empty())
    {
        std::ofstream commonFile(commonFilePath);
        for (const std::string& line : lines)
        {
            commonFile << line << std::endl;
            if (!commonFile)
            {
                errorFormatter << fgRed << "error" << reset << ": could not write to " << fgWhite << commonFilePath << reset << std::endl;
                return;
            }
        }
        if (commonFile)
        {
            formatter << fgGreen << "success" << reset << ": boost include-directory (" << index << "/" << count << ") path set to " << fgWhite << boostIncludeDir << reset << std::endl;
        }
    }
}

void BoostIncludeDir(CodeFormatter& formatter, CodeFormatter& errorFormatter, const std::string& cmajorRootDir, const std::string& boostIncludeDir)
{
    std::string gccCommonFilePath = GetFullPath(Path::Combine(Path::Combine(Path::Combine(Path::Combine(cmajorRootDir, "rts"), "build"), "gcc"), "Makefile.common"));
    if (boost::filesystem::exists(gccCommonFilePath))
    {
        formatter << fgDarkCyan << "info" << reset << ": " << fgWhite << gccCommonFilePath << reset << " found." << std::endl;
        BoostIncludeDirCommon(formatter, errorFormatter, cmajorRootDir, gccCommonFilePath, boostIncludeDir, 1, 2);
    }
    else
    {
        errorFormatter << fgRed << "error" << reset << ": " << fgWhite << gccCommonFilePath << reset << " not found" << std::endl;
    }
    formatter.WriteLine();
    std::string clangCommonFilePath = GetFullPath(Path::Combine(Path::Combine(Path::Combine(Path::Combine(cmajorRootDir, "rts"), "build"), "clang"), "Makefile.common"));
    if (boost::filesystem::exists(clangCommonFilePath))
    {
        formatter << fgDarkCyan << "info" << reset << ": " << fgWhite << clangCommonFilePath << reset << " found." << std::endl;
        BoostIncludeDirCommon(formatter, errorFormatter, cmajorRootDir, clangCommonFilePath, boostIncludeDir, 2, 2);
    }
    else
    {
        errorFormatter << fgRed << "error" << reset << ": " << fgWhite << clangCommonFilePath << reset << " not found" << std::endl;
    }
}

bool ChangeBoostVersion(CodeFormatter& formatter, CodeFormatter& errorFormatter, std::u32string& runtimeLibsLine, const std::u32string& libFileName,
    const std::string& boostVersion, int& index, int count)
{
    bool changed = false;
    std::u32string::size_type libFilePos = runtimeLibsLine.find(libFileName);
    std::u32string tail;
    if (libFilePos != std::u32string::npos)
    {
        std::u32string::size_type semicolonPos = runtimeLibsLine.find(U";", libFilePos + libFileName.length());
        if (semicolonPos != std::u32string::npos)
        {
            std::u32string currentVersionNumber = runtimeLibsLine.substr(libFilePos + libFileName.length(), semicolonPos - (libFilePos + libFileName.length()));
            if (!currentVersionNumber.empty())
            {
                formatter << fgDarkCyan << "info" << reset << ": boost version (" << index << "/" << count << ") is " << fgWhite << ToUtf8(currentVersionNumber) << reset << std::endl;
                ++index;
            }
            else
            {
                formatter << fgRed << "error" << reset << ": current boost version not found from line: " << ToUtf8(runtimeLibsLine) << std::endl;
                ++index;
                return false;
            }
            tail = runtimeLibsLine.substr(semicolonPos);
        }
        else
        {
            std::u32string currentVersionNumber = runtimeLibsLine.substr(libFilePos + libFileName.length());
            if (!currentVersionNumber.empty())
            {
                formatter << fgDarkCyan << "info" << reset << ": boost version (" << index << "/" << count << ") is " << fgWhite << ToUtf8(currentVersionNumber) << reset << std::endl;
                ++index;
            }
            else
            {
                formatter << fgRed << "error" << reset << ": current boost version not found from line: " << ToUtf8(runtimeLibsLine) << std::endl;
                ++index;
                return false;
            }
        }
        if (!boostVersion.empty())
        {
            runtimeLibsLine = runtimeLibsLine.substr(0, libFilePos) + libFileName + ToUtf32(boostVersion) + tail;
            changed = true;
        }
    }
    else
    {
        formatter << fgRed << "error" << reset << ": library file name '" + ToUtf8(libFileName) + "' not found from line: " << ToUtf8(runtimeLibsLine) << std::endl;
        return false;
    }
    return changed;
}

bool ChangeBoostVersion(CodeFormatter& formatter, CodeFormatter& errorFormatter, JsonString* argStr, const std::string& boostVersion, bool debug, int& index, int count)
{
    bool allChanged = true;
    std::u32string runtimeLibsLine = argStr->Value();
    if (debug)
    {
        if (!ChangeBoostVersion(formatter, errorFormatter, runtimeLibsLine, U"-lboost_filesystem-mgw8-mt-sd-x64-", boostVersion, index, count))
        {
            allChanged = false;
        }
    }
    else
    {
        if (!ChangeBoostVersion(formatter, errorFormatter, runtimeLibsLine, U"-lboost_filesystem-mgw8-mt-s-x64-", boostVersion, index, count))
        {
            allChanged = false;
        }
    }
    if (debug)
    {
        if (!ChangeBoostVersion(formatter, errorFormatter, runtimeLibsLine, U"-lboost_iostreams-mgw8-mt-sd-x64-", boostVersion, index, count))
        {
            allChanged = false;
        }
    }
    else
    {
        if (!ChangeBoostVersion(formatter, errorFormatter, runtimeLibsLine, U"-lboost_iostreams-mgw8-mt-s-x64-", boostVersion, index, count))
        {
            allChanged = false;
        }
    }
    if (debug)
    {
        if (!ChangeBoostVersion(formatter, errorFormatter, runtimeLibsLine, U"-lboost_system-mgw8-mt-sd-x64-", boostVersion, index, count))
        {
            allChanged = false;
        }
    }
    else
    {
        if (!ChangeBoostVersion(formatter, errorFormatter, runtimeLibsLine, U"-lboost_system-mgw8-mt-s-x64-", boostVersion, index, count))
        {
            allChanged = false;
        }
    }
    if (allChanged)
    {
        argStr->SetValue(runtimeLibsLine);
    }
    return allChanged;
}

void BoostVersion(CodeFormatter& formatter, CodeFormatter& errorFormatter, const std::string& cmajorRootDir, const std::string& boostVersion)
{
    int versionIndex = 1;
    int versionCount = 6;
    std::string version = boostVersion;
    if (!version.empty())
    {
        for (char& c : version)
        {
            if (c == '.')
            {
                c = '_';
            }
        }
    }
    bool changed = false;
    std::string toolChainsJsonFilePath = GetFullPath(Path::Combine(Path::Combine(cmajorRootDir, "config"), "tool-chains.json"));
    if (boost::filesystem::exists(toolChainsJsonFilePath))
    {
        formatter << fgDarkCyan << "info" << reset << ": " << fgWhite << toolChainsJsonFilePath << reset << " found." << std::endl;
        try
        {
            std::string toolChainsJsonFileContent = ReadFile(toolChainsJsonFilePath);
            std::u32string content = ToUtf32(toolChainsJsonFileContent);
            JsonLexer lexer(content, toolChainsJsonFilePath, 0);
            std::unique_ptr<soulng::util::JsonValue> jsonValue = JsonParser::Parse(lexer);
            if (jsonValue)
            {
                if (jsonValue->Type() == JsonValueType::object)
                {
                    JsonObject* jsonObject = static_cast<JsonObject*>(jsonValue.get());
                    JsonValue* platforms = jsonObject->GetField(U"platforms");
                    if (platforms && platforms->Type() == JsonValueType::array)
                    {
                        JsonArray* platformsArray = static_cast<JsonArray*>(platforms);
                        int n = platformsArray->Count();
                        if (n == 0)
                        {
                            throw std::runtime_error("'platforms' JSON array is empty");
                        }
                        for (int i = 0; i < n; ++i)
                        {
                            JsonValue* platform = (*platformsArray)[i];
                            if (platform && platform->Type() == JsonValueType::object)
                            {
                                JsonObject* platformObject = static_cast<JsonObject*>(platform);
                                JsonValue* name = platformObject->GetField(U"name");
                                if (name && name->Type() == JsonValueType::string)
                                {
                                    JsonString* nameStr = static_cast<JsonString*>(name);
                                    if (nameStr->Value() == U"windows")
                                    {
                                        JsonValue* toolChains = platformObject->GetField(U"toolChains");
                                        if (toolChains && toolChains->Type() == JsonValueType::array)
                                        {
                                            JsonArray* toolChainsArray = static_cast<JsonArray*>(toolChains);
                                            int n = toolChainsArray->Count();
                                            if (n == 0)
                                            {
                                                throw std::runtime_error("'platforms/*/windows/*/toolChains' JSON array is empty");
                                            }
                                            for (int i = 0; i < n; ++i)
                                            {
                                                JsonValue* toolChain = (*toolChainsArray)[i];
                                                if (toolChain && toolChain->Type() == JsonValueType::object)
                                                {
                                                    JsonObject* toolChainObject = static_cast<JsonObject*>(toolChain);
                                                    JsonValue* name = toolChainObject->GetField(U"name");
                                                    if (name && name->Type() == JsonValueType::string)
                                                    {
                                                        JsonString* nameStr = static_cast<JsonString*>(name);
                                                        if (nameStr->Value() == U"gcc")
                                                        {
                                                            JsonValue* tools = toolChainObject->GetField(U"tools");
                                                            if (tools && tools->Type() == JsonValueType::array)
                                                            {
                                                                JsonArray* toolsArray = static_cast<JsonArray*>(tools);
                                                                int n = toolsArray->Count();
                                                                if (n == 0)
                                                                {
                                                                    throw std::runtime_error("'platforms/*/windows/toolChains/*/gcc/tools': JSON array is empty");
                                                                }
                                                                for (int i = 0; i < n; ++i)
                                                                {
                                                                    JsonValue* tool = (*toolsArray)[i];
                                                                    if (tool->Type() == JsonValueType::object)
                                                                    {
                                                                        JsonObject* toolObject = static_cast<JsonObject*>(tool);
                                                                        JsonValue* name = toolObject->GetField(U"name");
                                                                        if (name && name->Type() == JsonValueType::string)
                                                                        {
                                                                            JsonString* toolName = static_cast<JsonString*>(name);
                                                                            if (toolName->Value() == U"linker")
                                                                            {
                                                                                JsonValue* configurations = toolObject->GetField(U"configurations");
                                                                                if (configurations && configurations->Type() == JsonValueType::array)
                                                                                {
                                                                                    JsonArray* configurationsArray = static_cast<JsonArray*>(configurations);
                                                                                    int n = configurationsArray->Count();
                                                                                    for (int i = 0; i < n; ++i)
                                                                                    {
                                                                                        JsonValue* configuration = (*configurationsArray)[i];
                                                                                        if (configuration && configuration->Type() == JsonValueType::object)
                                                                                        {
                                                                                            JsonObject* configurationObject = static_cast<JsonObject*>(configuration);
                                                                                            JsonValue* name = configurationObject->GetField(U"name");
                                                                                            if (name && name->Type() == JsonValueType::string)
                                                                                            {
                                                                                                bool debug = false;
                                                                                                JsonString* configName = static_cast<JsonString*>(name);
                                                                                                if (configName->Value() == U"debug")
                                                                                                {
                                                                                                    debug = true;
                                                                                                }
                                                                                                else if (configName->Value() == U"release")
                                                                                                {
                                                                                                    debug = false;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    throw std::runtime_error("unknown configuration '" + ToUtf8(configName->Value()));
                                                                                                }
                                                                                                JsonValue* args = configurationObject->GetField(U"args");
                                                                                                if (args && args->Type() == JsonValueType::array)
                                                                                                {
                                                                                                    JsonArray* argsArray = static_cast<JsonArray*>(args);
                                                                                                    int n = argsArray->Count();
                                                                                                    for (int i = 0; i < n; ++i)
                                                                                                    {
                                                                                                        JsonValue* arg = (*argsArray)[i];
                                                                                                        if (arg->Type() == JsonValueType::string)
                                                                                                        {
                                                                                                            JsonString* argStr = static_cast<JsonString*>(arg);
                                                                                                            std::string argS = ToUtf8(argStr->Value());
                                                                                                            if (StartsWith(argS, "$RUNTIME_LIBS$"))
                                                                                                            {
                                                                                                                if (ChangeBoostVersion(formatter, errorFormatter, argStr, version, debug, versionIndex, versionCount))
                                                                                                                {
                                                                                                                    changed = true;
                                                                                                                }
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    throw std::runtime_error("'platforms/*/windows/toolChains/*/gcc/tools/*/linker/configurations/*/args': JSON array expected");
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            throw std::runtime_error("'platforms/*/windows/toolChains/*/gcc/tools/*/linker/configurations/*': JSON object expected");
                                                                                        }
                                                                                    }
                                                                                }
                                                                                else
                                                                                {
                                                                                    throw std::runtime_error("'platforms/*/windows/toolChains/*/gcc/tools/*/linker/configurations': JSON array not found");
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            throw std::runtime_error("'platforms/*/windows/toolChains/*/gcc/tools/*/name': JSON string expected");
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        throw std::runtime_error("'platforms/*/windows/toolChains/*/gcc/tools/*': JSON object expected");
                                                                    }
                                                                }
                                                            }
                                                            else
                                                            {
                                                                throw std::runtime_error("'platforms/*/windows/toolChains/*/gcc/tools': JSON array not found");
                                                            }
                                                        }
                                                    }
                                                    else
                                                    {
                                                        throw std::runtime_error("'platforms/*/windows/toolChains/*/name': JSON string expected");
                                                    }
                                                }
                                                else
                                                {
                                                    throw std::runtime_error("'platforms/*/windows/toolChains/*': JSON object expected");
                                                }
                                            }
                                        }
                                        else
                                        {
                                            throw std::runtime_error("'platforms/*/windows/toolChains': JSON array not found");
                                        }
                                    }
                                }
                                else
                                {
                                    throw std::runtime_error("'platforms/*/name': JSON string expected");
                                }
                            }
                            else
                            {
                                throw std::runtime_error("'platforms/*': JSON object expected");
                            }
                        }
                        if (changed)
                        {
                            std::ofstream toolChainsJsonFile(toolChainsJsonFilePath);
                            CodeFormatter toolChainsFormatter(toolChainsJsonFile);
                            jsonValue->Write(toolChainsFormatter);
                            formatter << fgGreen << "success" << reset << ": boost version set to " << fgWhite << version << reset << std::endl;
                        }
                    }
                    else
                    {
                        throw std::runtime_error("'platforms': JSON array not found");
                    }
                }
                else
                {
                    throw std::runtime_error("'platforms': JSON object expected");
                }
            }
        }
        catch (const std::exception& ex)
        {
            errorFormatter << fgRed << "error" << reset << ": parsing " << fgWhite << toolChainsJsonFilePath << reset << " failed: " << ex.what() << std::endl;
        }
    }
    else
    {
        errorFormatter << fgRed << "error" << reset << ": " << fgWhite << toolChainsJsonFilePath << reset << " not found" << std::endl;
    }
}

int main(int argc, const char** argv)
{
    CodeFormatter errorFormatter(std::cerr);
    try
    {
        InitApplication();
        CodeFormatter formatter(std::cout);
        std::string boostLibDir;
        std::string boostIncludeDir;
        std::string boostVersion;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--help")
                {
                    PrintHelp(formatter);
                    return 1;
                }
                else if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(arg, '=');
                    if (components.size() == 2)
                    {
                        if (components[0] == "--boost-lib-dir")
                        {
                            boostLibDir = components[1];
                        }
                        else if (components[0] == "--boost-include-dir")
                        {
                            boostIncludeDir = components[1];
                        }
                        else if (components[0] == "--boost-version")
                        {
                            boostVersion = components[1];
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
            else if (StartsWith(arg, "-"))
            {
                if (arg.find('=') != std::string::npos)
                {
                    std::vector<std::string> components = Split(arg, '=');
                    if (components.size() == 2)
                    {
                        if (components[0] == "-L")
                        {
                            boostLibDir = components[1];
                        }
                        else if (components[0] == "-I")
                        {
                            boostIncludeDir = components[1];
                        }
                        else if (components[0] == "-V")
                        {
                            boostVersion = components[1];
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
                    if (options.empty())
                    {
                        throw std::runtime_error("unknown option '" + arg + "'");
                    }
                    for (char o : options)
                    {
                        switch (o)
                        {
                            case 'h':
                            {
                                PrintHelp(formatter);
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
        }
        std::string cmajorRootDir = GetFullPath(CmajorRootDir());
        formatter << fgDarkCyan << "info" << reset << ": CMAJOR_ROOT=" << fgWhite << cmajorRootDir << reset << std::endl;
        formatter.WriteLine();
        formatter << fgYellow << "BOOST_LIB_DIR:" << reset << std::endl;
        BoostLibDir(formatter, errorFormatter, cmajorRootDir, boostLibDir);
        formatter.WriteLine();
        formatter << fgYellow << "BOOST_INCLUDE_DIR:" << reset << std::endl;
        BoostIncludeDir(formatter, errorFormatter, cmajorRootDir, boostIncludeDir);
        formatter.WriteLine();
        formatter << fgYellow << "BOOST_VERSION_NUMBER:" << reset << std::endl;
        BoostVersion(formatter, errorFormatter, cmajorRootDir, boostVersion);
    }
    catch (const std::exception& ex)
    {
        errorFormatter << fgRed << "error" << reset << ": " << fgWhite << ex.what() << reset << std::endl;
        return 1;
    }
    return 0;
}
