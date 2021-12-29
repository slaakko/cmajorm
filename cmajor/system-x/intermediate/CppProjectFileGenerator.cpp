// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/CppProjectFileGenerator.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace cmsx::intermediate {

using namespace soulng::util;
using namespace soulng::unicode;

void GenerateProjectFile(Project* project, Module* module, const std::string& mainObjectFilePath, bool verbose)
{
    boost::uuids::uuid projectGuid = boost::uuids::random_generator()();
    std::string projectGuidStr = "{" + boost::lexical_cast<std::string>(projectGuid) + "}";

    std::string projectFilePath = GetFullPath(Path::Combine(Path::GetDirectoryName(GetFullPath(project->LibraryFilePath())), "program.vcxproj"));
    std::string directoryPath = Path::GetDirectoryName(projectFilePath);
    boost::system::error_code ec;
    boost::filesystem::create_directories(directoryPath, ec);
    if (ec)
    {
        throw std::runtime_error("could not create directory '" + directoryPath + "': " + PlatformStringToUtf8(ec.message()));
    }
    std::ofstream projectFile(projectFilePath);
    CodeFormatter formatter(projectFile);
    sngxml::dom::Document projectDoc;
    projectDoc.SetXmlEncoding(U"utf-8");
    projectDoc.SetXmlVersion(U"1.0");
    sngxml::dom::Element* projectElement = new sngxml::dom::Element(U"Project");
    projectElement->SetAttribute(U"DefaultTargets", U"Build");
    projectElement->SetAttribute(U"xmlns", U"http://schemas.microsoft.com/developer/msbuild/2003");

    sngxml::dom::Element* itemGroupElement = new sngxml::dom::Element(U"ItemGroup");
    itemGroupElement->SetAttribute(U"Label", U"ProjectConfigurations");

    sngxml::dom::Element* debugX64ConfigElement = new sngxml::dom::Element(U"ProjectConfiguration");
    debugX64ConfigElement->SetAttribute(U"Include", U"Debug|x64");
    sngxml::dom::Element* debugConfigElement = new sngxml::dom::Element(U"Configuration");
    debugX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugConfigElement));
    sngxml::dom::Text* debugConfigText = new sngxml::dom::Text(U"Debug");
    debugConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugConfigText));
    sngxml::dom::Element* debugPlatformElement = new sngxml::dom::Element(U"Platform");
    debugX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPlatformElement));
    sngxml::dom::Text* debugPlatformText = new sngxml::dom::Text(U"x64");
    debugPlatformElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugPlatformText));

    sngxml::dom::Element* releaseX64ConfigElement = new sngxml::dom::Element(U"ProjectConfiguration");
    releaseX64ConfigElement->SetAttribute(U"Include", U"Release|x64");
    sngxml::dom::Element* releaseConfigElement = new sngxml::dom::Element(U"Configuration");
    releaseX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseConfigElement));
    sngxml::dom::Text* releaseConfigText = new sngxml::dom::Text(U"Release");
    releaseConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseConfigText));
    sngxml::dom::Element* releasePlatformElement = new sngxml::dom::Element(U"Platform");
    releaseX64ConfigElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePlatformElement));
    sngxml::dom::Text* releasePlatformText = new sngxml::dom::Text(U"x64");
    releasePlatformElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releasePlatformText));

    itemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigElement));
    itemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigElement));
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(itemGroupElement));

    // Globals PropertyGroup
    sngxml::dom::Element* globalsPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
    globalsPropertyGroup->SetAttribute(U"Label", U"Globals");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(globalsPropertyGroup));
    sngxml::dom::Element* vcProjectVersionElement = new sngxml::dom::Element(U"VCProjectVersion");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(vcProjectVersionElement));
    sngxml::dom::Text* projectVersionText = new sngxml::dom::Text(U"16.0");
    vcProjectVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectVersionText));
    sngxml::dom::Element* keywordElement = new sngxml::dom::Element(U"Keyword");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement));
    sngxml::dom::Text* keywordText = new sngxml::dom::Text(U"Win32Proj");
    keywordElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordText));

    sngxml::dom::Element* projectGuidElement = new sngxml::dom::Element(U"ProjectGuid");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidElement));
    sngxml::dom::Text* projectGuidText = new sngxml::dom::Text(ToUtf32(projectGuidStr));
    projectGuidElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidText));

    sngxml::dom::Element* rootNamespaceElement = new sngxml::dom::Element(U"RootNamespace");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootNamespaceElement));
    sngxml::dom::Text* rootNamespaceText = new sngxml::dom::Text(U"program");
    rootNamespaceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootNamespaceText));

    sngxml::dom::Element* windowsTargetPlatformVersionElement = new sngxml::dom::Element(U"WindowsTargetPlatformVersion");
    globalsPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionElement));
    sngxml::dom::Text* windowsTargetPlatformVersionText = new sngxml::dom::Text(U"10.0");
    windowsTargetPlatformVersionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(windowsTargetPlatformVersionText));

    // Import Project
    sngxml::dom::Element* importProjectElement = new sngxml::dom::Element(U"Import");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importProjectElement));
    importProjectElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.Default.props");

    // DebugX64Config PropertyGroup
    sngxml::dom::Element* debugX64ConfigPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigPropertyGroup));
    debugX64ConfigPropertyGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
    debugX64ConfigPropertyGroup->SetAttribute(U"Label", U"Configuration");

    sngxml::dom::Element* debugX64ConfigConfigurationTypeElement = new sngxml::dom::Element(U"ConfigurationType");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigConfigurationTypeElement));
    sngxml::dom::Text* debugX64ConfigConfigurationTypeText = new sngxml::dom::Text(U"Application");
    debugX64ConfigConfigurationTypeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigConfigurationTypeText));

    sngxml::dom::Element* debugX64ConfigUseDebugLibrariesElement = new sngxml::dom::Element(U"UseDebugLibraries");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigUseDebugLibrariesElement));
    sngxml::dom::Text* debugX64ConfigUseDebugLibrariesText = new sngxml::dom::Text(U"true");
    debugX64ConfigUseDebugLibrariesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigUseDebugLibrariesText));

    sngxml::dom::Element* debugX64ConfigPlatformToolsetElement = new sngxml::dom::Element(U"PlatformToolset");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigPlatformToolsetElement));
    sngxml::dom::Text* debugX64ConfigPlatformToolsetText = new sngxml::dom::Text(U"v143");
    debugX64ConfigPlatformToolsetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigPlatformToolsetText));

    sngxml::dom::Element* debugX64ConfigCharacterSetElement = new sngxml::dom::Element(U"CharacterSet");
    debugX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigCharacterSetElement));
    sngxml::dom::Text* debugX64ConfigCharacterSetText = new sngxml::dom::Text(U"Unicode");
    debugX64ConfigCharacterSetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConfigCharacterSetText));

    // ReleaseX64Config PropertyGroup
    sngxml::dom::Element* releaseX64ConfigPropertyGroup = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigPropertyGroup));
    releaseX64ConfigPropertyGroup->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
    releaseX64ConfigPropertyGroup->SetAttribute(U"Label", U"Configuration");

    sngxml::dom::Element* releaseX64ConfigConfigurationTypeElement = new sngxml::dom::Element(U"ConfigurationType");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigConfigurationTypeElement));
    sngxml::dom::Text* releaseX64ConfigConfigurationTypeText = new sngxml::dom::Text(U"Application");
    releaseX64ConfigConfigurationTypeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigConfigurationTypeText));

    sngxml::dom::Element* releaseX64ConfigUseDebugLibrariesElement = new sngxml::dom::Element(U"UseDebugLibraries");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigUseDebugLibrariesElement));
    sngxml::dom::Text* releaseX64ConfigUseDebugLibrariesText = new sngxml::dom::Text(U"false");
    releaseX64ConfigUseDebugLibrariesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigUseDebugLibrariesText));

    sngxml::dom::Element* releaseX64ConfigPlatformToolsetElement = new sngxml::dom::Element(U"PlatformToolset");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigPlatformToolsetElement));
    sngxml::dom::Text* releaseX64ConfigPlatformToolsetText = new sngxml::dom::Text(U"v143");
    releaseX64ConfigPlatformToolsetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigPlatformToolsetText));

    sngxml::dom::Element* releaseX64ConfigWholeProgramOptimizationElement = new sngxml::dom::Element(U"WholeProgramOptimization");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigWholeProgramOptimizationElement));
    sngxml::dom::Text* releaseX64WholeProgramOptimizationText = new sngxml::dom::Text(U"true");
    releaseX64ConfigWholeProgramOptimizationElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WholeProgramOptimizationText));

    sngxml::dom::Element* releaseX64ConfigCharacterSetElement = new sngxml::dom::Element(U"CharacterSet");
    releaseX64ConfigPropertyGroup->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigCharacterSetElement));
    sngxml::dom::Text* releaseX64ConfigCharacterSetText = new sngxml::dom::Text(U"Unicode");
    releaseX64ConfigCharacterSetElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConfigCharacterSetText));

    // Import Project2
    sngxml::dom::Element* importProject2Element = new sngxml::dom::Element(U"Import");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importProject2Element));
    importProject2Element->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.props");

    // ImportExtensionSettings
    sngxml::dom::Element* importExtensionSettingsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importExtensionSettingsElement));
    importExtensionSettingsElement->SetAttribute(U"Label", U"ExtensionSettings");

    // ImportShared
    sngxml::dom::Element* importSharedElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importSharedElement));
    importSharedElement->SetAttribute(U"Label", U"Shared");

    // Import x64/Debug PropertySheets
    sngxml::dom::Element* importX64DebugPropertySheetsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64DebugPropertySheetsElement));
    importX64DebugPropertySheetsElement->SetAttribute(U"Label", U"PropertySheets");
    importX64DebugPropertySheetsElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
    sngxml::dom::Element* importX64DebugPropertySheetsProjectElement = new sngxml::dom::Element(U"Import");
    importX64DebugPropertySheetsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64DebugPropertySheetsProjectElement));
    importX64DebugPropertySheetsProjectElement->SetAttribute(U"Project", U"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
    importX64DebugPropertySheetsProjectElement->SetAttribute(U"Condition", U"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
    importX64DebugPropertySheetsProjectElement->SetAttribute(U"Label", U"LocalAppDataPlatform");

    // Import x64/Release PropertySheets
    sngxml::dom::Element* importX64ReleasePropertySheetsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64ReleasePropertySheetsElement));
    importX64ReleasePropertySheetsElement->SetAttribute(U"Label", U"PropertySheets");
    importX64ReleasePropertySheetsElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
    sngxml::dom::Element* importX64ReleasePropertySheetsProjectElement = new sngxml::dom::Element(U"Import");
    importX64ReleasePropertySheetsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importX64ReleasePropertySheetsProjectElement));
    importX64ReleasePropertySheetsProjectElement->SetAttribute(U"Project", U"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
    importX64ReleasePropertySheetsProjectElement->SetAttribute(U"Condition", U"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')");
    importX64ReleasePropertySheetsProjectElement->SetAttribute(U"Label", U"LocalAppDataPlatform");

    // UserMacros
    sngxml::dom::Element* userMacrosElement = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(userMacrosElement));
    userMacrosElement->SetAttribute(U"Label", U"UserMacros");

    // LinkIncremental DebugX64
    sngxml::dom::Element* linkIncrementalDebugX64ConditionElement = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalDebugX64ConditionElement));
    linkIncrementalDebugX64ConditionElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");
    sngxml::dom::Element* linkIncrementalDebugX64Element = new sngxml::dom::Element(U"LinkIncremental");
    linkIncrementalDebugX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalDebugX64Element));
    sngxml::dom::Text* linkIncrementalDebugX64ConditionText = new sngxml::dom::Text(U"true");
    linkIncrementalDebugX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalDebugX64ConditionText));
    sngxml::dom::Element* targetNameDebugX64Element = new sngxml::dom::Element(U"TargetName");
    linkIncrementalDebugX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameDebugX64Element));
    sngxml::dom::Text* targetNameDebugX64Text = new sngxml::dom::Text(U"programd");
    targetNameDebugX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameDebugX64Text));

    // LinkIncremental ReleaseX64
    sngxml::dom::Element* linkIncrementalReleaseX64ConditionElement = new sngxml::dom::Element(U"PropertyGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalReleaseX64ConditionElement));
    linkIncrementalReleaseX64ConditionElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");
    sngxml::dom::Element* linkIncrementalReleaseX64Element = new sngxml::dom::Element(U"LinkIncremental");
    linkIncrementalReleaseX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalReleaseX64Element));
    sngxml::dom::Text* linkIncrementalReleaseX64ConditionText = new sngxml::dom::Text(U"false");
    linkIncrementalReleaseX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkIncrementalReleaseX64ConditionText));
    sngxml::dom::Element* targetNameReleaseX64Element = new sngxml::dom::Element(U"TargetName");
    linkIncrementalReleaseX64ConditionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameReleaseX64Element));
    sngxml::dom::Text* targetNameReleaseX64Text = new sngxml::dom::Text(U"program");
    targetNameReleaseX64Element->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetNameReleaseX64Text));

    // ItemDefinitionGroupClCompile DebugX64

    sngxml::dom::Element* debugX64ItemDefinitionGroupCompileElement = new sngxml::dom::Element(U"ItemDefinitionGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ItemDefinitionGroupCompileElement));
    debugX64ItemDefinitionGroupCompileElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Debug|x64'");

    sngxml::dom::Element* debugX64CompileElement = new sngxml::dom::Element(U"ClCompile");
    debugX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64CompileElement));

    sngxml::dom::Element* debugX64WarningLevelElement = new sngxml::dom::Element(U"WarningLevel");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningLevelElement));
    sngxml::dom::Text* debugX64WarningLevelText = new sngxml::dom::Text(U"Level3");
    debugX64WarningLevelElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningLevelText));

    sngxml::dom::Element* debugX64SDLCheckElement = new sngxml::dom::Element(U"SDLCheck");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SDLCheckElement));
    sngxml::dom::Text* debugX64SDLCheckText = new sngxml::dom::Text(U"true");
    debugX64SDLCheckElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SDLCheckText));

    sngxml::dom::Element* debugX64PreprocessorElement = new sngxml::dom::Element(U"PreprocessorDefinitions");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64PreprocessorElement));
    sngxml::dom::Text* debugX64PreprocessorText = new sngxml::dom::Text(U"_DEBUG;_WINDOWS;%(PreprocessorDefinitions)");
    debugX64PreprocessorElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64PreprocessorText));

    sngxml::dom::Element* debugX64ConformanceElement = new sngxml::dom::Element(U"ConformanceMode");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConformanceElement));
    sngxml::dom::Text* debugX64ConformanceText = new sngxml::dom::Text(U"true");
    debugX64ConformanceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64ConformanceText));

    sngxml::dom::Element* debugX64LanguageStandardElement = new sngxml::dom::Element(U"LanguageStandard");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64LanguageStandardElement));
    sngxml::dom::Text* debugX64LanguageStandardText = new sngxml::dom::Text(U"stdcpp20");
    debugX64LanguageStandardElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64LanguageStandardText));

    //sngxml::dom::Element* debugX64IncludeElement = new sngxml::dom::Element(U"AdditionalIncludeDirectories");
    //debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64IncludeElement));
    //sngxml::dom::Text* debugX64IncludeText = new sngxml::dom::Text(U"..;" + ToUtf32(wingstallIncludeDir) + U";" + ToUtf32(BoostIncludeDir()));
    //debugX64IncludeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64IncludeText));

    sngxml::dom::Element* debugX64FormatElement = new sngxml::dom::Element(U"DebugInformationFormat");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64FormatElement));
    sngxml::dom::Text* debugX64FormatText = new sngxml::dom::Text(U"ProgramDatabase");
    debugX64FormatElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64FormatText));

    sngxml::dom::Element* debugX64WarningsElement = new sngxml::dom::Element(U"DisableSpecificWarnings");
    debugX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningsElement));
    sngxml::dom::Text* debugX64WarningsText = new sngxml::dom::Text(U"4101;4102;4251;4275;4244;4267");
    debugX64WarningsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64WarningsText));

    sngxml::dom::Element* debugX64LinkElement = new sngxml::dom::Element(U"Link");
    debugX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64LinkElement));

    sngxml::dom::Element* debugX64SubSystemElement = new sngxml::dom::Element(U"SubSystem");
    debugX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SubSystemElement));
    sngxml::dom::Text* debugX64SubSystemText = new sngxml::dom::Text(U"Console");
    debugX64SubSystemElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64SubSystemText));

    sngxml::dom::Element* debugX64DebugInfoElement = new sngxml::dom::Element(U"GenerateDebugInformation");
    debugX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64DebugInfoElement));
    sngxml::dom::Text* debugX64DebugInfoText = new sngxml::dom::Text(U"true");
    debugX64DebugInfoElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(debugX64DebugInfoText));

    sngxml::dom::Element* releaseX64ItemDefinitionGroupCompileElement = new sngxml::dom::Element(U"ItemDefinitionGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ItemDefinitionGroupCompileElement));
    releaseX64ItemDefinitionGroupCompileElement->SetAttribute(U"Condition", U"'$(Configuration)|$(Platform)'=='Release|x64'");

    sngxml::dom::Element* releaseX64CompileElement = new sngxml::dom::Element(U"ClCompile");
    releaseX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64CompileElement));

    sngxml::dom::Element* releaseX64WarningLevelElement = new sngxml::dom::Element(U"WarningLevel");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningLevelElement));
    sngxml::dom::Text* releaseX64WarningLevelText = new sngxml::dom::Text(U"Level3");
    releaseX64WarningLevelElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningLevelText));

    sngxml::dom::Element* releaseX64FunctionLevelLinkingElement = new sngxml::dom::Element(U"FunctionLevelLinking");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64FunctionLevelLinkingElement));
    sngxml::dom::Text* releaseX64FunctionLevelLinkingText = new sngxml::dom::Text(U"true");
    releaseX64FunctionLevelLinkingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64FunctionLevelLinkingText));

    sngxml::dom::Element* releaseX64IntrinsicElement = new sngxml::dom::Element(U"IntrinsicFunctions");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IntrinsicElement));
    sngxml::dom::Text* releaseX64IntrinsicsText = new sngxml::dom::Text(U"true");
    releaseX64IntrinsicElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IntrinsicsText));

    sngxml::dom::Element* releaseX64SDLCheckElement = new sngxml::dom::Element(U"SDLCheck");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SDLCheckElement));
    sngxml::dom::Text* releaseX64SDLCheckText = new sngxml::dom::Text(U"true");
    releaseX64SDLCheckElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SDLCheckText));

    sngxml::dom::Element* releaseX64PreprocessorElement = new sngxml::dom::Element(U"PreprocessorDefinitions");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64PreprocessorElement));
    sngxml::dom::Text* releaseX64PreprocessorText = new sngxml::dom::Text(U"NDEBUG;_WINDOWS;%(PreprocessorDefinitions)");
    releaseX64PreprocessorElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64PreprocessorText));

    sngxml::dom::Element* releaseX64ConformanceElement = new sngxml::dom::Element(U"ConformanceMode");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConformanceElement));
    sngxml::dom::Text* releaseX64ConformanceText = new sngxml::dom::Text(U"true");
    releaseX64ConformanceElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64ConformanceText));

    sngxml::dom::Element* releaseX64LanguageStandardElement = new sngxml::dom::Element(U"LanguageStandard");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64LanguageStandardElement));
    sngxml::dom::Text* releaseX64LanguageStandardText = new sngxml::dom::Text(U"stdcpp20");
    releaseX64LanguageStandardElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64LanguageStandardText));

    //sngxml::dom::Element* releaseX64IncludeElement = new sngxml::dom::Element(U"AdditionalIncludeDirectories");
    //releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IncludeElement));
    //sngxml::dom::Text* releaseX64IncludeText = new sngxml::dom::Text(U"..;" + ToUtf32(wingstallIncludeDir) + U";" + ToUtf32(BoostIncludeDir()));
    //releaseX64IncludeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64IncludeText));

    sngxml::dom::Element* releaseX64WarningsElement = new sngxml::dom::Element(U"DisableSpecificWarnings");
    releaseX64CompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningsElement));
    sngxml::dom::Text* releaseX64WarningsText = new sngxml::dom::Text(U"4101;4102;4251;4275;4244;4267");
    releaseX64WarningsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64WarningsText));

    sngxml::dom::Element* releaseX64LinkElement = new sngxml::dom::Element(U"Link");
    releaseX64ItemDefinitionGroupCompileElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64LinkElement));

    sngxml::dom::Element* releaseX64SubSystemElement = new sngxml::dom::Element(U"SubSystem");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SubSystemElement));
    sngxml::dom::Text* releaseX64SubSystemText = new sngxml::dom::Text(U"Console");
    releaseX64SubSystemElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64SubSystemText));

    sngxml::dom::Element* releaseX64EnableComDataFoldingElement = new sngxml::dom::Element(U"EnableCOMDATFolding");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64EnableComDataFoldingElement));
    sngxml::dom::Text* releaseX64EnableComDataFoldingText = new sngxml::dom::Text(U"true");
    releaseX64EnableComDataFoldingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64EnableComDataFoldingText));

    sngxml::dom::Element* releaseX64OptimizeReferencesElement = new sngxml::dom::Element(U"OptimizeReferences");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64OptimizeReferencesElement));
    sngxml::dom::Text* releaseX64OptimizeReferencesText = new sngxml::dom::Text(U"true");
    releaseX64OptimizeReferencesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64OptimizeReferencesText));

    sngxml::dom::Element* releaseX64DebugInfoElement = new sngxml::dom::Element(U"GenerateDebugInformation");
    releaseX64LinkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64DebugInfoElement));
    sngxml::dom::Text* releaseX64DebugInfoText = new sngxml::dom::Text(U"true");
    releaseX64DebugInfoElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(releaseX64DebugInfoText));

    sngxml::dom::Element* clCompileItemGroupElement = new sngxml::dom::Element(U"ItemGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileItemGroupElement));

    for (const std::string& sourceFilePath : project->RelativeSourceFilePaths())
    {
        sngxml::dom::Element* clCompileDataEement = new sngxml::dom::Element(U"ClCompile");
        clCompileItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileDataEement));
        clCompileDataEement->SetAttribute(U"Include", ToUtf32(Path::ChangeExtension(sourceFilePath, ".cpp")));
    }
    sngxml::dom::Element* clCompileDataEement = new sngxml::dom::Element(U"ClCompile");
    clCompileItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(clCompileDataEement));
    clCompileDataEement->SetAttribute(U"Include", ToUtf32(Path::ChangeExtension(Path::GetFileName(mainObjectFilePath), ".cpp")));

    sngxml::dom::Element* importTargetsElement = new sngxml::dom::Element(U"Import");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importTargetsElement));
    importTargetsElement->SetAttribute(U"Project", U"$(VCTargetsPath)\\Microsoft.Cpp.targets");

    sngxml::dom::Element* importExtensionTargetsElement = new sngxml::dom::Element(U"ImportGroup");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importExtensionTargetsElement));
    importExtensionTargetsElement->SetAttribute(U"Label", U"ExtensionTargets");

    projectDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(projectElement));
    projectDoc.Write(formatter);

    if (verbose)
    {
        std::cout << "==> " << projectFilePath << std::endl;
    }
}

} // cmsx::intermediate
