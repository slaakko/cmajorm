// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmproj/Conversion.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <iostream>

namespace cmajor { namespace cmproj {

using namespace soulng::unicode;

void ConvertProject(Project* project, const std::string& cmprojFilePath, const std::string& projectGuid, bool verbose)
{
    sngxml::dom::Document cmprojDoc;
    cmprojDoc.SetXmlVersion(U"1.0");
    cmprojDoc.SetXmlEncoding(U"utf-8");
    std::unique_ptr<sngxml::dom::Element> projectElement(new sngxml::dom::Element(U"Project"));
    projectElement->SetAttribute(U"ToolsVersion", U"4.0");
    projectElement->SetAttribute(U"DefaultTargets", U"Build");
    projectElement->SetAttribute(U"xmlns", U"http://schemas.microsoft.com/developer/msbuild/2003");
    std::unique_ptr<sngxml::dom::Element> propertyGroupElement(new sngxml::dom::Element(U"PropertyGroup"));
    propertyGroupElement->SetAttribute(U"Label", U"Globals");
    std::unique_ptr<sngxml::dom::Element> customProjectExtensionsPathElement(new sngxml::dom::Element(U"CustomProjectExtensionsPath"));
    std::unique_ptr<sngxml::dom::Text> customProjectExtensionsPath(new sngxml::dom::Text(U"$(LocalAppData)\\CustomProjectSystems\\Cmajor\\"));
    customProjectExtensionsPathElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(customProjectExtensionsPath.release()));
    propertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(customProjectExtensionsPathElement.release()));
    std::unique_ptr<sngxml::dom::Element> projectGuidElement(new sngxml::dom::Element(U"ProjectGuid"));
    std::unique_ptr<sngxml::dom::Text> projectGuidText(new sngxml::dom::Text(ToUtf32(projectGuid)));
    projectGuidElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidText.release()));
    propertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectGuidElement.release()));
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(propertyGroupElement.release()));
    std::unique_ptr<sngxml::dom::Element> targetPropertyGroupElement(new sngxml::dom::Element(U"PropertyGroup"));
    std::unique_ptr<sngxml::dom::Element> targetTypeElement(new sngxml::dom::Element(U"TargetType"));
    std::u32string targetTypeStr = U"program";
    if (project->GetTarget() == Target::winapp)
    {
        targetTypeStr = U"winapp";
    }
    else if (project->GetTarget() == Target::library)
    {
        targetTypeStr = U"library";
    }
    else if (project->GetTarget() == Target::winlib)
    {
        targetTypeStr = U"winlib";
    }
    else if (project->GetTarget() == Target::unitTest)
    {
        targetTypeStr = U"unitTest";
    }
    std::unique_ptr<sngxml::dom::Text> targetType(new sngxml::dom::Text(targetTypeStr));
    targetTypeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetType.release()));
    targetPropertyGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetTypeElement.release()));
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(targetPropertyGroupElement.release()));
    std::unique_ptr<sngxml::dom::Element> compileItemGroupElement(new sngxml::dom::Element(U"ItemGroup"));
    for (const std::string& sourceFilePath : project->RelativeSourceFilePaths())
    {
        std::unique_ptr<sngxml::dom::Element> compileElement(new sngxml::dom::Element(U"CmCompile"));
        compileElement->SetAttribute(U"Include", ToUtf32(sourceFilePath));
        compileItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(compileElement.release()));
    }
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(compileItemGroupElement.release()));
    std::unique_ptr<sngxml::dom::Element> textItemGroupElement(new sngxml::dom::Element(U"ItemGroup"));
    for (const std::string& textFilePath : project->RelativeTextFilePaths())
    {
        std::unique_ptr<sngxml::dom::Element> textElement(new sngxml::dom::Element(U"None"));
        textElement->SetAttribute(U"Include", ToUtf32(textFilePath));
        textItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(textElement.release()));
    }
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(textItemGroupElement.release()));
    std::unique_ptr<sngxml::dom::Element> referenceItemGroupElement(new sngxml::dom::Element(U"ItemGroup"));
    for (const std::string& referenceFilePath : project->RelativeReferencedProjectFilePaths())
    {
        std::unique_ptr<sngxml::dom::Element> projectReferenceElement(new sngxml::dom::Element(U"ProjectReference"));
        projectReferenceElement->SetAttribute(U"Include", ToUtf32(Path::ChangeExtension(referenceFilePath, ".cmproj")));
        referenceItemGroupElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(projectReferenceElement.release()));
    }
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(referenceItemGroupElement.release()));
    std::unique_ptr<sngxml::dom::Element> importPropsElement(new sngxml::dom::Element(U"Import"));
    importPropsElement->SetAttribute(U"Project", U"$(CustomProjectExtensionsPath)Cmajor.props");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importPropsElement.release()));
    std::unique_ptr<sngxml::dom::Element> importTargetsElement(new sngxml::dom::Element(U"Import"));
    importTargetsElement->SetAttribute(U"Project", U"$(CustomProjectExtensionsPath)Cmajor.targets");
    projectElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(importTargetsElement.release()));
    cmprojDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(projectElement.release()));
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
