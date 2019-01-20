// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmproj/Conversion.hpp>
#include <cmajor/dom/Document.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Path.hpp>
#include <iostream>

namespace cmajor { namespace cmproj {

using namespace cmajor::unicode;

void ConvertProject(Project* project, const std::string& cmprojFilePath, const std::string& projectGuid, bool verbose)
{
    cmajor::dom::Document cmprojDoc;
    cmprojDoc.SetXmlVersion(U"1.0");
    cmprojDoc.SetXmlEncoding(U"utf-8");
    std::unique_ptr<cmajor::dom::Element> projectElement(new cmajor::dom::Element(U"Project"));
    projectElement->SetAttribute(U"ToolsVersion", U"4.0");
    projectElement->SetAttribute(U"DefaultTargets", U"Build");
    projectElement->SetAttribute(U"xmlns", U"http://schemas.microsoft.com/developer/msbuild/2003");
    std::unique_ptr<cmajor::dom::Element> propertyGroupElement(new cmajor::dom::Element(U"PropertyGroup"));
    propertyGroupElement->SetAttribute(U"Label", U"Globals");
    std::unique_ptr<cmajor::dom::Element> customProjectExtensionsPathElement(new cmajor::dom::Element(U"CustomProjectExtensionsPath"));
    std::unique_ptr<cmajor::dom::Text> customProjectExtensionsPath(new cmajor::dom::Text(U"$(LocalAppData)\\CustomProjectSystems\\Cmajor\\"));
    customProjectExtensionsPathElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(customProjectExtensionsPath.release()));
    propertyGroupElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(customProjectExtensionsPathElement.release()));
    std::unique_ptr<cmajor::dom::Element> projectGuidElement(new cmajor::dom::Element(U"ProjectGuid"));
    std::unique_ptr<cmajor::dom::Text> projectGuidText(new cmajor::dom::Text(ToUtf32(projectGuid)));
    projectGuidElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(projectGuidText.release()));
    propertyGroupElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(projectGuidElement.release()));
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(propertyGroupElement.release()));
    std::unique_ptr<cmajor::dom::Element> targetPropertyGroupElement(new cmajor::dom::Element(U"PropertyGroup"));
    std::unique_ptr<cmajor::dom::Element> targetTypeElement(new cmajor::dom::Element(U"TargetType"));
    std::u32string targetTypeStr = U"program";
    if (project->GetTarget() == Target::library)
    {
        targetTypeStr = U"library";
    }
    else if (project->GetTarget() == Target::unitTest)
    {
        targetTypeStr = U"unitTest";
    }
    std::unique_ptr<cmajor::dom::Text> targetType(new cmajor::dom::Text(targetTypeStr));
    targetTypeElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(targetType.release()));
    targetPropertyGroupElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(targetTypeElement.release()));
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(targetPropertyGroupElement.release()));
    std::unique_ptr<cmajor::dom::Element> compileItemGroupElement(new cmajor::dom::Element(U"ItemGroup"));
    for (const std::string& sourceFilePath : project->RelativeSourceFilePaths())
    {
        std::unique_ptr<cmajor::dom::Element> compileElement(new cmajor::dom::Element(U"CmCompile"));
        compileElement->SetAttribute(U"Include", ToUtf32(sourceFilePath));
        compileItemGroupElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(compileElement.release()));
    }
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(compileItemGroupElement.release()));
    std::unique_ptr<cmajor::dom::Element> textItemGroupElement(new cmajor::dom::Element(U"ItemGroup"));
    for (const std::string& textFilePath : project->RelativeTextFilePaths())
    {
        std::unique_ptr<cmajor::dom::Element> textElement(new cmajor::dom::Element(U"None"));
        textElement->SetAttribute(U"Include", ToUtf32(textFilePath));
        textItemGroupElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(textElement.release()));
    }
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(textItemGroupElement.release()));
    std::unique_ptr<cmajor::dom::Element> referenceItemGroupElement(new cmajor::dom::Element(U"ItemGroup"));
    for (const std::string& referenceFilePath : project->RelativeReferencedProjectFilePaths())
    {
        std::unique_ptr<cmajor::dom::Element> projectReferenceElement(new cmajor::dom::Element(U"ProjectReference"));
        projectReferenceElement->SetAttribute(U"Include", ToUtf32(Path::ChangeExtension(referenceFilePath, ".cmproj")));
        referenceItemGroupElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(projectReferenceElement.release()));
    }
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(referenceItemGroupElement.release()));
    std::unique_ptr<cmajor::dom::Element> importPropsElement(new cmajor::dom::Element(U"Import"));
    importPropsElement->SetAttribute(U"Project", U"$(CustomProjectExtensionsPath)Cmajor.props");
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(importPropsElement.release()));
    std::unique_ptr<cmajor::dom::Element> importTargetsElement(new cmajor::dom::Element(U"Import"));
    importTargetsElement->SetAttribute(U"Project", U"$(CustomProjectExtensionsPath)Cmajor.targets");
    projectElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(importTargetsElement.release()));
    cmprojDoc.AppendChild(std::unique_ptr<cmajor::dom::Node>(projectElement.release()));
    std::ofstream cmprojFile(cmprojFilePath);
    CodeFormatter formatter(cmprojFile);
    formatter.SetIndentSize(1);
    cmprojDoc.Write(formatter);
    if (verbose)
    {
        std::cout << "==> " << cmprojFilePath << std::endl;
    }
}

} } // namespace cmajor::cmproj
