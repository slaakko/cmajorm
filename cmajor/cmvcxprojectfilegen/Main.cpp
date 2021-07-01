#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>

using namespace soulng::util;
using namespace soulng::unicode;

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        sngxml::xpath::Init();
    }
    ~InitDone()
    {
        sngxml::xpath::Done();
        sngcm::ast::Done();
        soulng::util::Done();
    }
};

const char* version = "4.0.0";

void PrintHelp()
{
    std::cout << "Cmajor Visual Studio C++ .vcxproj project file generator version " << version << std::endl;
    std::cout << "Usage: cmvcxprojectfilegen [options] { SOURCEFILE.cpp }" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "--help (-h):" << std::endl;
    std::cout << "  Print help and exit." << std::endl;
    std::cout << "--verbose (-v):" << std::endl;
    std::cout << "  Be verbose." << std::endl;
    std::cout << "--name (-n) $PROJECT_NAME$" << std::endl;
    std::cout << "  Set project name to $PROJECT_NAME$." << std::endl;
    std::cout << "--file (-f) $PROJECT_FILE_PATH$" << std::endl;
    std::cout << "  Generate project file $PROJECT_FILE_PATH$." << std::endl;
    std::cout << "--target (-t) $PROJECT_TARGET$" << std::endl;
    std::cout << "  Set project target to $PROJECT_TARGET$," << std::endl;
    std::cout << "  where target can be 'program', 'library', 'winapp', 'winguiapp', 'winlib'" << std::endl;
    std::cout << "--config (-c) $PROJECT_CONFIG$" << std::endl;
    std::cout << "  Set project configuration to $PROJECT_CONFIG$," << std::endl;
    std::cout << "  where config can be 'debug' or 'release'" << std::endl;
    std::cout << "--librarydirs (-L) $LIBRARY_DIRECTORIES$" << std::endl;
    std::cout << "  Set library directories to semicolon-separated list of directories." << std::endl;
    std::cout << "--libs (-l) $LIBRARY_FILE_NAMES$" << std::endl;
    std::cout << "  Set libraries to semicolon-separated list of library file names." << std::endl;
    std::cout << "--options (-o) $OPTIONS$" << std::endl;
    std::cout << "  Set compilation options to $OPTIONS$," << std::endl;
    std::cout << "  where options can contain /FAs for generating assembly listings," << std::endl;
    std::cout << "  /JMC for enabling Just My Code debugging, and" << std::endl;
    std::cout << "  /GR for enabling C++ runtime type-information." << std::endl;
}

int main(int argc, const char** argv)
{
    InitDone initDone;
    try
    {
        bool verbose = false;
        bool prevWasName = false;
        bool prevWasFile = false;
        bool prevWasTarget = false;
        bool prevWasConfig = false;
        bool prevWasLibraryDirs = false;
        bool prevWasLibs = false;
        bool prevWasOptions = false;
        std::string projectName;
        std::string projectFilePath;
        std::string projectDir;
        std::string projectGuidStr;
        std::string projectTargetStr;
        std::string projectConfigStr;
        std::string libraryDirectoriesStr;
        std::string librariesStr;
        std::string optionsStr;
        std::vector<std::string> sourceFiles;
        std::string target;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--name")
                {
                    prevWasName = true;
                }
                else if (arg == "--file")
                {
                    prevWasFile = true;
                }
                else if (arg == "--target")
                {
                    prevWasTarget = true;
                }
                else if (arg == "--config")
                {
                    prevWasConfig = true;
                }
                else if (arg == "--librarydirs")
                {
                    prevWasLibraryDirs = true;
                }
                else if (arg == "--libs")
                {
                    prevWasLibs = true;
                }
                else if (arg == "--options")
                {
                    prevWasOptions = true;
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
                        case 'n':
                        {
                            prevWasName = true;
                            break;
                        }
                        case 'f':
                        { 
                            prevWasFile = true;
                            break;
                        }
                        case 't':
                        {
                            prevWasTarget = true;
                            break;
                        }
                        case 'c':
                        {
                            prevWasConfig = true;
                            break;
                        }
                        case 'L':
                        {
                            prevWasLibraryDirs = true;
                            break;
                        }
                        case 'l':
                        {
                            prevWasLibs = true;
                            break;
                        }
                        case 'o':
                        {
                            prevWasOptions = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + arg + "'");
                        }
                    }
                }
            }
            else
            {
                if (prevWasName)
                {
                    projectName = arg;
                    prevWasName = false;
                }
                else if (prevWasFile)
                {
                    projectFilePath = GetFullPath(arg);
                    projectDir = Path::GetDirectoryName(projectFilePath);
                    prevWasFile = false;
                }
                else if (prevWasTarget)
                {
                    target = arg;
                    if (target == "library" || target == "winlib")
                    {
                        projectTargetStr = "StaticLibrary";
                    }
                    else if (target == "program" || target == "winapp" || target == "winguiapp")
                    {
                        projectTargetStr = "Application";
                    }
                    prevWasTarget = false;
                }
                else if (prevWasConfig)
                {
                    projectConfigStr = arg;
                    prevWasConfig = false;
                }
                else if (prevWasLibraryDirs)
                {
                    libraryDirectoriesStr = arg;
                    prevWasLibraryDirs = false;
                }
                else if (prevWasLibs)
                {
                    librariesStr = arg;
                    prevWasLibs = false;
                }
                else if (prevWasOptions)
                {
                    optionsStr = arg;
                    prevWasOptions = false;
                }
                else
                {
                    sourceFiles.push_back(Path::MakeCanonical(arg));
                }
            }
        }
        if (verbose)
        {
            std::cout << "Cmajor Visual Studio C++ .vcxproj project file generator 'cmvcxprojectfilegen' version " << version << std::endl;
        }
        if (projectConfigStr == "debug")
        {
            boost::uuids::uuid projectGuid = boost::uuids::random_generator()();
            projectGuidStr = "{" + boost::uuids::to_string(projectGuid) + "}";
            sngxml::dom::Document projectFileDoc;
            sngxml::dom::Element* projectElement = new sngxml::dom::Element(U"Project");
            projectFileDoc.SetXmlVersion(U"1.0");
            projectFileDoc.SetXmlEncoding(U"utf-8");
            projectFileDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(projectElement));
            projectElement->SetAttribute(U"DefaultTargets", U"Build");
            projectElement->SetAttribute(U"xmlns", U"http://schemas.microsoft.com/developer/msbuild/2003");
            sngxml::dom::Element* projectConfigurationsElement = new sngxml::dom::Element(U"ItemGroup");
            projectConfigurationsElement->SetAttribute(U"Label", U"ProjectConfigurations");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectConfigurationsElement));
            sngxml::dom::Element* debugProjectConfigurationElement = new sngxml::dom::Element(U"ProjectConfiguration");
            debugProjectConfigurationElement->SetAttribute(U"Include", U"Debug|x64");
            projectConfigurationsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugProjectConfigurationElement));
            sngxml::dom::Element* debugConfigurationElement = new sngxml::dom::Element(U"Configuration");
            sngxml::dom::Text* debugText = new sngxml::dom::Text(U"Debug");
            debugConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugText));
            debugProjectConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugConfigurationElement));
            sngxml::dom::Element* platformConfigurationElement = new sngxml::dom::Element(U"Platform");
            sngxml::dom::Text* platformText = new sngxml::dom::Text(U"x64");
            platformConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(platformText));
            debugProjectConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(platformConfigurationElement));

            sngxml::dom::Element* globalsProjectGroupElement = new sngxml::dom::Element(U"PropertyGroup");
            globalsProjectGroupElement->SetAttribute(U"Label", U"Globals");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(globalsProjectGroupElement));
            sngxml::dom::Element* vcProjectVersionElement = new sngxml::dom::Element(U"VCProjectVersion");
            vcProjectVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"16.0")));
            globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(vcProjectVersionElement));
            sngxml::dom::Element* projectGuidElement = new sngxml::dom::Element(U"ProjectGuid");
            globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidElement));
            sngxml::dom::Text* projectGuidText = new sngxml::dom::Text(ToUtf32(projectGuidStr));
            projectGuidElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidText));
            sngxml::dom::Element* keywordElement = new sngxml::dom::Element(U"Keyword");
            keywordElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Win32Proj")));
            globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement));
            sngxml::dom::Element* projectRootNamespaceElement = new sngxml::dom::Element(U"RootNamespace");
            globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectRootNamespaceElement));
            sngxml::dom::Text* rootNamespaceText = new sngxml::dom::Text(ToUtf32(projectName));
            projectRootNamespaceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootNamespaceText));
            sngxml::dom::Element* windowsTargetPlatformVersionElement = new sngxml::dom::Element(U"WindowsTargetPlatformVersion");
            globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionElement));
            sngxml::dom::Text* windowsTargetPlatformVersionText = new sngxml::dom::Text(U"10.0");
            windowsTargetPlatformVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionText));

            sngxml::dom::Element* importCppDefaultPropsElement = new sngxml::dom::Element(U"Import");
            importCppDefaultPropsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppDefaultPropsElement));

            sngxml::dom::Element* configurationPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
            configurationPropertyGroup->SetAttribute(U"Label", U"Configuration");
            configurationPropertyGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
            sngxml::dom::Element* preferredToolArchitecture = new sngxml::dom::Element(U"PreferredToolArchitecture");
            preferredToolArchitecture->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"x64")));
            configurationPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(preferredToolArchitecture));
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(configurationPropertyGroup));

            sngxml::dom::Element* debugPropertyGroupElement = new sngxml::dom::Element(U"PropertyGroup");
            debugPropertyGroupElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
            sngxml::dom::Element* debugConfigurationTypeElement = new sngxml::dom::Element(U"ConfigurationType");
            sngxml::dom::Text* debugConfigurationTypeText = new sngxml::dom::Text(ToUtf32(projectTargetStr));
            debugConfigurationTypeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugConfigurationTypeText));
            debugPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugConfigurationTypeElement));
            sngxml::dom::Element* debugUseDebugLibrariesElement = new sngxml::dom::Element(U"UseDebugLibraries");
            sngxml::dom::Text* debugUseDebugLibrariesText = new sngxml::dom::Text(U"true");
            debugUseDebugLibrariesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugUseDebugLibrariesText));
            debugPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugUseDebugLibrariesElement));
            sngxml::dom::Element* debugPlatformToolsetElement = new sngxml::dom::Element(U"PlatformToolset");
            sngxml::dom::Text* debugPlatformToolsetText = new sngxml::dom::Text(U"v142");
            debugPlatformToolsetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPlatformToolsetText));
            debugPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPlatformToolsetElement));
            sngxml::dom::Element* debugCharacterElement = new sngxml::dom::Element(U"CharacterSet");
            sngxml::dom::Text* debugCharacterText = new sngxml::dom::Text(U"Unicode");
            debugCharacterElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugCharacterText));
            debugPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugCharacterElement));
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPropertyGroupElement));

            sngxml::dom::Element* importCppPropsElement = new sngxml::dom::Element(U"Import");
            importCppPropsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.props");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppPropsElement));

            sngxml::dom::Element* debugTargetPropertyGroupElement = new sngxml::dom::Element(U"PropertyGroup");
            debugTargetPropertyGroupElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
            sngxml::dom::Element* debugTargetLinkElement = new sngxml::dom::Element(U"LinkIncremental");
            debugTargetLinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
            debugTargetPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugTargetLinkElement));
            sngxml::dom::Element* debugTargetNameElement = new sngxml::dom::Element(U"TargetName");
            debugTargetNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(projectName))));
            debugTargetPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugTargetNameElement));

            sngxml::dom::Element* debugItemDefinitionGroup = new sngxml::dom::Element(U"ItemDefinitionGroup");
            debugItemDefinitionGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugItemDefinitionGroup));
            sngxml::dom::Element* debugClCompileItemDefinition = new sngxml::dom::Element(U"ClCompile");
            debugItemDefinitionGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugClCompileItemDefinition));
            sngxml::dom::Element* warningLevelElement = new sngxml::dom::Element(U"WarningLevel");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(warningLevelElement));
            warningLevelElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Level3")));
            sngxml::dom::Element* sdlCheckElement = new sngxml::dom::Element(U"SDLCheck");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(sdlCheckElement));
            sdlCheckElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
            sngxml::dom::Element* exceptionHandlingElement = new sngxml::dom::Element(U"ExceptionHandling");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(exceptionHandlingElement));
            exceptionHandlingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"SyncCThrow")));
            sngxml::dom::Element* debugPrecompiledHeaderElement = new sngxml::dom::Element(U"PrecompiledHeader");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPrecompiledHeaderElement));
            sngxml::dom::Text* debugNotUsingPrecompiledHeaderText = new sngxml::dom::Text(U"NotUsing");
            debugPrecompiledHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugNotUsingPrecompiledHeaderText));
            sngxml::dom::Element* debugDebugInformationFormatElement = new sngxml::dom::Element(U"DebugInformationFormat");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugDebugInformationFormatElement));
            sngxml::dom::Text* debugProgramDatabaseText = new sngxml::dom::Text(U"ProgramDatabase");
            debugDebugInformationFormatElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugProgramDatabaseText));
            sngxml::dom::Element* languageStandardElement = new sngxml::dom::Element(U"LanguageStandard");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(languageStandardElement));
            sngxml::dom::Text* languageStandardText = new sngxml::dom::Text(U"stdcpp17");
            languageStandardElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(languageStandardText));
            sngxml::dom::Element* debugOptimizationElement = new sngxml::dom::Element(U"Optimization");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugOptimizationElement));
            sngxml::dom::Text* debugOptimizationText = new sngxml::dom::Text(U"Disabled");
            debugOptimizationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugOptimizationText));
            sngxml::dom::Element* debugRuntimeLibraryElement = new sngxml::dom::Element(U"RuntimeLibrary");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugRuntimeLibraryElement));
            sngxml::dom::Text* debugRuntimeLibraryText = new sngxml::dom::Text(U"MultiThreadedDebugDLL");
            debugRuntimeLibraryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugRuntimeLibraryText));
            sngxml::dom::Element* debugDisableWarningsElement = new sngxml::dom::Element(U"DisableSpecificWarnings");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugDisableWarningsElement));
            debugDisableWarningsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"4102;4146;4244")));
            if (optionsStr.find("/FAs") != std::string::npos)
            {
                sngxml::dom::Element* debugAssemblerOutputElement = new sngxml::dom::Element(U"AssemblerOutput");
                debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugAssemblerOutputElement));
                debugAssemblerOutputElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"AssemblyAndSourceCode")));
            }
            if (optionsStr.find("/JMC") != std::string::npos)
            {
                sngxml::dom::Element* debugJustMyCodeElement = new sngxml::dom::Element(U"SupportJustMyCode");
                debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugJustMyCodeElement));
                debugJustMyCodeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
            }
            else
            {
                sngxml::dom::Element* debugJustMyCodeElement = new sngxml::dom::Element(U"SupportJustMyCode");
                debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugJustMyCodeElement));
                debugJustMyCodeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"false")));
            }
            if (optionsStr.find("/GR") != std::string::npos)
            {
                sngxml::dom::Element* debugRTTI = new sngxml::dom::Element(U"RuntimeTypeInfo");
                debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugRTTI));
                debugRTTI->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
            }
            if (projectTargetStr == "Application")
            {
                sngxml::dom::Element* debugLinkItemDefinition = new sngxml::dom::Element(U"Link");
                debugItemDefinitionGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugLinkItemDefinition));
                sngxml::dom::Element* subSystemElement = new sngxml::dom::Element(U"SubSystem");
                debugLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(subSystemElement));
                std::u32string subsystem = U"Console";
                if (target == "winapp" || target == "winguiapp")
                {
                    subsystem = U"Windows";
                }
                sngxml::dom::Element* libraryDirectoriesElement = new sngxml::dom::Element(U"AdditionalLibraryDirectories");
                libraryDirectoriesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(libraryDirectoriesStr))));
                debugLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(libraryDirectoriesElement));
                sngxml::dom::Element* dependenciesElement = new sngxml::dom::Element(U"AdditionalDependencies");
                dependenciesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(librariesStr))));
                debugLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(dependenciesElement));
                sngxml::dom::Text* subSystemTextStr = new sngxml::dom::Text(subsystem);
                subSystemElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subSystemTextStr));
                sngxml::dom::Element* generateDebugInformationElement = new sngxml::dom::Element(U"GenerateDebugInformation");
                debugLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(generateDebugInformationElement));
                sngxml::dom::Text* debugInfoText = new sngxml::dom::Text(U"true");
                generateDebugInformationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugInfoText));
                sngxml::dom::Element* stackSizeElement = new sngxml::dom::Element(U"StackReserveSize");
                stackSizeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"16777216")));
                debugLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(stackSizeElement));
            }

            sngxml::dom::Element* debugSources = new sngxml::dom::Element(U"ItemGroup");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugSources));
            for (const std::string& sourceFile : sourceFiles)
            {
                sngxml::dom::Element* clCompileElement = new sngxml::dom::Element(U"ClCompile");
                clCompileElement->SetAttribute(U"Include", ToUtf32(sourceFile));
                debugSources->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileElement));
            }

            sngxml::dom::Element* importCppTargetsElement = new sngxml::dom::Element(U"Import");
            importCppTargetsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.targets");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppTargetsElement));
            std::ofstream projectFile(projectFilePath);
            CodeFormatter formatter(projectFile);
            projectFileDoc.Write(formatter);
        }
        else
        {
            if (projectConfigStr == "release")
            {
                boost::uuids::uuid projectGuid = boost::uuids::random_generator()();
                projectGuidStr = "{" + boost::uuids::to_string(projectGuid) + "}";
                sngxml::dom::Document projectFileDoc;
                sngxml::dom::Element* projectElement = new sngxml::dom::Element(U"Project");
                projectFileDoc.SetXmlVersion(U"1.0");
                projectFileDoc.SetXmlEncoding(U"utf-8");
                projectFileDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(projectElement));
                projectElement->SetAttribute(U"DefaultTargets", U"Build");
                projectElement->SetAttribute(U"xmlns", U"http://schemas.microsoft.com/developer/msbuild/2003");
                sngxml::dom::Element* projectConfigurationsElement = new sngxml::dom::Element(U"ItemGroup");
                projectConfigurationsElement->SetAttribute(U"Label", U"ProjectConfigurations");
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectConfigurationsElement));
                sngxml::dom::Element* releaseProjectConfigurationElement = new sngxml::dom::Element(U"ProjectConfiguration");
                releaseProjectConfigurationElement->SetAttribute(U"Include", U"Release|x64");
                projectConfigurationsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseProjectConfigurationElement));
                sngxml::dom::Element* releaseConfigurationElement = new sngxml::dom::Element(U"Configuration");
                sngxml::dom::Text* releaseText = new sngxml::dom::Text(U"Release");
                releaseConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseText));
                releaseProjectConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseConfigurationElement));
                sngxml::dom::Element* platformConfigurationElement = new sngxml::dom::Element(U"Platform");
                sngxml::dom::Text* platformText = new sngxml::dom::Text(U"x64");
                platformConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(platformText));
                releaseProjectConfigurationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(platformConfigurationElement));

                sngxml::dom::Element* globalsProjectGroupElement = new sngxml::dom::Element(U"PropertyGroup");
                globalsProjectGroupElement->SetAttribute(U"Label", U"Globals");
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(globalsProjectGroupElement));
                sngxml::dom::Element* vcProjectVersionElement = new sngxml::dom::Element(U"VCProjectVersion");
                vcProjectVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"16.0")));
                globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(vcProjectVersionElement));
                sngxml::dom::Element* projectGuidElement = new sngxml::dom::Element(U"ProjectGuid");
                globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidElement));
                sngxml::dom::Text* projectGuidText = new sngxml::dom::Text(ToUtf32(projectGuidStr));
                projectGuidElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidText));
                sngxml::dom::Element* keywordElement = new sngxml::dom::Element(U"Keyword");
                keywordElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Win32Proj")));
                globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement));
                sngxml::dom::Element* projectRootNamespaceElement = new sngxml::dom::Element(U"RootNamespace");
                globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectRootNamespaceElement));
                sngxml::dom::Text* rootNamespaceText = new sngxml::dom::Text(ToUtf32(projectName));
                projectRootNamespaceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootNamespaceText));
                sngxml::dom::Element* windowsTargetPlatformVersionElement = new sngxml::dom::Element(U"WindowsTargetPlatformVersion");
                globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionElement));
                sngxml::dom::Text* windowsTargetPlatformVersionText = new sngxml::dom::Text(U"10.0");
                windowsTargetPlatformVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionText));

                sngxml::dom::Element* importCppDefaultPropsElement = new sngxml::dom::Element(U"Import");
                importCppDefaultPropsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppDefaultPropsElement));

                sngxml::dom::Element* releasePropertyGroupElement = new sngxml::dom::Element(U"PropertyGroup");
                releasePropertyGroupElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
                sngxml::dom::Element* releaseConfigurationTypeElement = new sngxml::dom::Element(U"ConfigurationType");
                sngxml::dom::Text* releaseConfigurationTypeText = new sngxml::dom::Text(ToUtf32(projectTargetStr));
                releaseConfigurationTypeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseConfigurationTypeText));
                releasePropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseConfigurationTypeElement));
                sngxml::dom::Element* releaseUseDebugLibrariesElement = new sngxml::dom::Element(U"UseDebugLibraries");
                sngxml::dom::Text* releaseUseDebugLibrariesText = new sngxml::dom::Text(U"false");
                releaseUseDebugLibrariesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseUseDebugLibrariesText));
                releasePropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseUseDebugLibrariesElement));
                sngxml::dom::Element* releasePlatformToolsetElement = new sngxml::dom::Element(U"PlatformToolset");
                sngxml::dom::Text* releasePlatformToolsetText = new sngxml::dom::Text(U"v142");
                releasePlatformToolsetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePlatformToolsetText));
                releasePropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePlatformToolsetElement));
                sngxml::dom::Element* wholeProgramOptimizationElement = new sngxml::dom::Element(U"WholeProgramOptimization");
                wholeProgramOptimizationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"false")));
                releasePropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(wholeProgramOptimizationElement));
                sngxml::dom::Element* releaseCharacterElement = new sngxml::dom::Element(U"CharacterSet");
                sngxml::dom::Text* releaseCharacterText = new sngxml::dom::Text(U"Unicode");
                releaseCharacterElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseCharacterText));
                releasePropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseCharacterElement));
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePropertyGroupElement));

                sngxml::dom::Element* importCppPropsElement = new sngxml::dom::Element(U"Import");
                importCppPropsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.props");
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppPropsElement));

                sngxml::dom::Element* configurationPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
                configurationPropertyGroup->SetAttribute(U"Label", U"Configuration");
                configurationPropertyGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
                sngxml::dom::Element* preferredToolArchitecture = new sngxml::dom::Element(U"PreferredToolArchitecture");
                preferredToolArchitecture->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"x64")));
                configurationPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(preferredToolArchitecture));
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(configurationPropertyGroup));

                sngxml::dom::Element* releaseTargetPropertyGroupElement = new sngxml::dom::Element(U"PropertyGroup");
                releaseTargetPropertyGroupElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
                sngxml::dom::Element* releaseTargetLinkElement = new sngxml::dom::Element(U"LinkIncremental");
                releaseTargetLinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"false")));
                releaseTargetPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseTargetLinkElement));
                sngxml::dom::Element* releaseTargetNameElement = new sngxml::dom::Element(U"TargetName");
                releaseTargetNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(projectName))));
                releaseTargetPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseTargetNameElement));

                sngxml::dom::Element* releaseItemDefinitionGroup = new sngxml::dom::Element(U"ItemDefinitionGroup");
                releaseItemDefinitionGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseItemDefinitionGroup));
                sngxml::dom::Element* releaseClCompileItemDefinition = new sngxml::dom::Element(U"ClCompile");
                releaseItemDefinitionGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseClCompileItemDefinition));
                sngxml::dom::Element* warningLevelElement = new sngxml::dom::Element(U"WarningLevel");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(warningLevelElement));
                warningLevelElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"Level3")));
                sngxml::dom::Element* sdlCheckElement = new sngxml::dom::Element(U"SDLCheck");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(sdlCheckElement));
                sdlCheckElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
                sngxml::dom::Element* exceptionHandlingElement = new sngxml::dom::Element(U"ExceptionHandling");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(exceptionHandlingElement));
                exceptionHandlingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"SyncCThrow")));
                sngxml::dom::Element* releasePrecompiledHeaderElement = new sngxml::dom::Element(U"PrecompiledHeader");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePrecompiledHeaderElement));
                sngxml::dom::Text* releaseNotUsingPrecompiledHeaderText = new sngxml::dom::Text(U"NotUsing");
                releasePrecompiledHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseNotUsingPrecompiledHeaderText));
                sngxml::dom::Element* releaseDebugInformationFormatElement = new sngxml::dom::Element(U"DebugInformationFormat");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseDebugInformationFormatElement));
                sngxml::dom::Text* releaseProgramDatabaseText = new sngxml::dom::Text(U"ProgramDatabase");
                releaseDebugInformationFormatElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseProgramDatabaseText));
                sngxml::dom::Element* languageStandardElement = new sngxml::dom::Element(U"LanguageStandard");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(languageStandardElement));
                sngxml::dom::Text* languageStandardText = new sngxml::dom::Text(U"stdcpp17");
                languageStandardElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(languageStandardText));
                sngxml::dom::Element* releaseOptimizationElement = new sngxml::dom::Element(U"Optimization");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseOptimizationElement));
                sngxml::dom::Text* releaseOptimizationText = new sngxml::dom::Text(U"MaxSpeed");
                releaseOptimizationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseOptimizationText));
                sngxml::dom::Element* functionLevelLinkingElement = new sngxml::dom::Element(U"FunctionLevelLinking");
                functionLevelLinkingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionLevelLinkingElement));
                sngxml::dom::Element* intrinsicFunctionsElement = new sngxml::dom::Element(U"IntrinsicFunctions");
                intrinsicFunctionsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(intrinsicFunctionsElement));
                sngxml::dom::Element* releaseRuntimeLibraryElement = new sngxml::dom::Element(U"RuntimeLibrary");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseRuntimeLibraryElement));
                sngxml::dom::Text* releaseRuntimeLibraryText = new sngxml::dom::Text(U"MultiThreadedDLL");
                releaseRuntimeLibraryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseRuntimeLibraryText));
                sngxml::dom::Element* releaseDisableWarningsElement = new sngxml::dom::Element(U"DisableSpecificWarnings");
                releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseDisableWarningsElement));
                releaseDisableWarningsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"4102;4146;4244")));
                if (optionsStr.find("/FAs") != std::string::npos)
                {
                    sngxml::dom::Element* releaseAssemblerOutputElement = new sngxml::dom::Element(U"AssemblerOutput");
                    releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseAssemblerOutputElement));
                    releaseAssemblerOutputElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"AssemblyAndSourceCode")));
                }
                if (optionsStr.find("/JMC") != std::string::npos)
                {
                    sngxml::dom::Element* releaseJustMyCodeElement = new sngxml::dom::Element(U"SupportJustMyCode");
                    releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseJustMyCodeElement));
                    releaseJustMyCodeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
                }
                else
                {
                    sngxml::dom::Element* releaseJustMyCodeElement = new sngxml::dom::Element(U"SupportJustMyCode");
                    releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseJustMyCodeElement));
                    releaseJustMyCodeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"false")));
                }
                if (optionsStr.find("/GR") != std::string::npos)
                {
                    sngxml::dom::Element* releaseRTTI = new sngxml::dom::Element(U"RuntimeTypeInfo");
                    releaseClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseRTTI));
                    releaseRTTI->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
                }
                if (projectTargetStr == "Application")
                {
                    sngxml::dom::Element* releaseLinkItemDefinition = new sngxml::dom::Element(U"Link");
                    releaseItemDefinitionGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseLinkItemDefinition));
                    sngxml::dom::Element* subSystemElement = new sngxml::dom::Element(U"SubSystem");
                    releaseLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(subSystemElement));
                    std::u32string subsystem = U"Console";
                    if (target == "winapp" || target == "winguiapp")
                    {
                        subsystem = U"Windows";
                    }
                    sngxml::dom::Element* libraryDirectoriesElement = new sngxml::dom::Element(U"AdditionalLibraryDirectories");
                    libraryDirectoriesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(libraryDirectoriesStr))));
                    releaseLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(libraryDirectoriesElement));
                    sngxml::dom::Element* dependenciesElement = new sngxml::dom::Element(U"AdditionalDependencies");
                    dependenciesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(ToUtf32(librariesStr))));
                    releaseLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(dependenciesElement));
                    sngxml::dom::Text* subSystemTextStr = new sngxml::dom::Text(subsystem);
                    subSystemElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subSystemTextStr));
                    sngxml::dom::Element* enableComdataFoldingElement = new sngxml::dom::Element(U"EnableCOMDATFolding");
                    enableComdataFoldingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
                    releaseLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(enableComdataFoldingElement));
                    sngxml::dom::Element* optimizeReferencesElement = new sngxml::dom::Element(U"OptimizeReferences");
                    optimizeReferencesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"true")));
                    releaseLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(optimizeReferencesElement));
                    sngxml::dom::Element* generateDebugInformationElement = new sngxml::dom::Element(U"GenerateDebugInformation");
                    releaseLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(generateDebugInformationElement));
                    sngxml::dom::Text* debugInfoText = new sngxml::dom::Text(U"true");
                    generateDebugInformationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugInfoText));
                    sngxml::dom::Element* stackSizeElement = new sngxml::dom::Element(U"StackReserveSize");
                    stackSizeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(U"16777216")));
                    releaseLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(stackSizeElement));
                }

                sngxml::dom::Element* releaseSources = new sngxml::dom::Element(U"ItemGroup");
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseSources));
                for (const std::string& sourceFile : sourceFiles)
                {
                    sngxml::dom::Element* clCompileElement = new sngxml::dom::Element(U"ClCompile");
                    clCompileElement->SetAttribute(U"Include", ToUtf32(sourceFile));
                    releaseSources->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileElement));
                }

                sngxml::dom::Element* importCppTargetsElement = new sngxml::dom::Element(U"Import");
                importCppTargetsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.targets");
                projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppTargetsElement));
                std::ofstream projectFile(projectFilePath);
                CodeFormatter formatter(projectFile);
                projectFileDoc.Write(formatter);
            }
        }
        if (verbose)
        {
            std::cout << "==> " << projectFilePath << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
