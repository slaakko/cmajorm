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
        std::string projectName;
        std::string projectFilePath;
        std::string projectDir;
        std::string projectGuidStr;
        std::string projectTargetStr;
        std::string projectConfigStr;
        std::vector<std::string> sourceFiles;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
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
                    std::string target = arg;
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
                else
                {
                    sourceFiles.push_back(Path::MakeCanonical(arg));
                }
            }
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
            sngxml::dom::Element* projectGuidElement = new sngxml::dom::Element(U"ProjectGuid");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(globalsProjectGroupElement));
            globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidElement));
            sngxml::dom::Text* projectGuidText = new sngxml::dom::Text(ToUtf32(projectGuidStr));
            projectGuidElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidText));
            sngxml::dom::Element* projectRootNamespaceElement = new sngxml::dom::Element(U"RootNamespace");
            globalsProjectGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectRootNamespaceElement));
            sngxml::dom::Text* rootNamespaceText = new sngxml::dom::Text(ToUtf32(projectName));
            projectRootNamespaceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootNamespaceText));

            sngxml::dom::Element* importCppDefaultPropsElement = new sngxml::dom::Element(U"Import");
            importCppDefaultPropsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppDefaultPropsElement));

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
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPropertyGroupElement));

            sngxml::dom::Element* importCppPropsElement = new sngxml::dom::Element(U"Import");
            importCppPropsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.props");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importCppPropsElement));

            sngxml::dom::Element* debugItemDefinitionGroup = new sngxml::dom::Element(U"ItemDefinitionGroup");
            debugItemDefinitionGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugItemDefinitionGroup));
            sngxml::dom::Element* debugClCompileItemDefinition = new sngxml::dom::Element(U"ClCompile");
            debugItemDefinitionGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugClCompileItemDefinition));
            sngxml::dom::Element* debugPrecompiledHeaderElement = new sngxml::dom::Element(U"PrecompiledHeader");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPrecompiledHeaderElement));
            sngxml::dom::Text* debugNotUsingPrecompiledHeaderText = new sngxml::dom::Text(U"NotUsing");
            debugPrecompiledHeaderElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugNotUsingPrecompiledHeaderText));
            sngxml::dom::Element* debugDebugInformationFormatElement = new sngxml::dom::Element(U"DebugInformationFormat");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugDebugInformationFormatElement));
            sngxml::dom::Text* debugProgramDatabaseText = new sngxml::dom::Text(U"ProgramDatabase");
            debugDebugInformationFormatElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugProgramDatabaseText));
            sngxml::dom::Element* debugOptimizationElement = new sngxml::dom::Element(U"Optimization");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugOptimizationElement));
            sngxml::dom::Text* debugOptimizationText = new sngxml::dom::Text(U"Disabled");
            debugOptimizationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugOptimizationText));
            sngxml::dom::Element* debugRuntimeLibraryElement = new sngxml::dom::Element(U"RuntimeLibrary");
            debugClCompileItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugRuntimeLibraryElement));
            sngxml::dom::Text* debugRuntimeLibraryText = new sngxml::dom::Text(U"MultiThreadedDebugDLL");
            debugRuntimeLibraryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugRuntimeLibraryText));

            if (projectTargetStr == "program" || projectTargetStr == "winapp" || projectTargetStr == "winguiapp")
            {
                sngxml::dom::Element* debugLinkItemDefinition = new sngxml::dom::Element(U"Link");
                debugItemDefinitionGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugLinkItemDefinition));
                sngxml::dom::Element* subSystemElement = new sngxml::dom::Element(U"SubSystem");
                debugLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(subSystemElement));
                std::u32string subsystem = U"Console";
                if (projectTargetStr == "winapp" || projectTargetStr == "winguiapp")
                {
                    subsystem = U"Windows";
                }
                sngxml::dom::Text* subSystemTextStr = new sngxml::dom::Text(subsystem);
                subSystemElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subSystemTextStr));
                sngxml::dom::Element* generateDebugInformationElement = new sngxml::dom::Element(U"GenerateDebugInformation");
                debugLinkItemDefinition->AppendChild(std::unique_ptr<sngxml::dom::Node>(generateDebugInformationElement));
                sngxml::dom::Text* debugInfoText = new sngxml::dom::Text(U"true");
                generateDebugInformationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugInfoText));
            }

            sngxml::dom::Element* debugSetDirectoriesPropertyGroup = new sngxml::dom::Element(U"PropertyGroup"); // overriding default directories:
            debugSetDirectoriesPropertyGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'"); // can override for example TargetName, BaseIntermediateOutputPath, OutDir etc.
            projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugSetDirectoriesPropertyGroup));
            sngxml::dom::Element* debugIntermediateDirectoryElement = new sngxml::dom::Element(U"BaseIntermediateOutputPath");
            sngxml::dom::Text* debugIntermediateDirectoryText = new sngxml::dom::Text(U".");
            debugIntermediateDirectoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugIntermediateDirectoryText));
            debugSetDirectoriesPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugIntermediateDirectoryElement));
            sngxml::dom::Element* debugBaseOutputPathDirectoryElement = new sngxml::dom::Element(U"BaseOutputPath");
            sngxml::dom::Text* debugBaseOutputPathDirectoryText = new sngxml::dom::Text(U".");
            debugBaseOutputPathDirectoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugBaseOutputPathDirectoryText));
            debugSetDirectoriesPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugBaseOutputPathDirectoryElement));
            sngxml::dom::Element* debugOutputPathDirectoryElement = new sngxml::dom::Element(U"OutputPath");
            sngxml::dom::Text* debugOutputPathDirectoryText = new sngxml::dom::Text(U".");
            debugOutputPathDirectoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugOutputPathDirectoryText));
            debugSetDirectoriesPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugOutputPathDirectoryElement));
            sngxml::dom::Element* debugIntermediateOutputPathDirectoryElement = new sngxml::dom::Element(U"IntermediateOutputPath");
            sngxml::dom::Text* debugIntermediateOutputPathDirectoryText = new sngxml::dom::Text(U".");
            debugIntermediateOutputPathDirectoryElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugIntermediateOutputPathDirectoryText));
            debugSetDirectoriesPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugIntermediateOutputPathDirectoryElement));
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
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
