// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmproj/Conversion.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <iostream>
#include <fstream>

namespace cmajor { namespace cmproj {

using namespace soulng::unicode;

void ConvertProjectToCmProject(Project* project, const std::string& cmprojFilePath, const std::string& projectGuid, bool verbose)
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
    else if (project->GetTarget() == Target::winguiapp)
    {
        targetTypeStr = U"winguiapp";
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
    for (const std::string& resourceFilePath : project->RelativeResourceFilePaths())
    {
        std::unique_ptr<sngxml::dom::Element> compileElement(new sngxml::dom::Element(U"CmCompile"));
        compileElement->SetAttribute(U"Include", ToUtf32(resourceFilePath));
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

void ConvertCmProjectToProject(const std::string& cmprojFilePath, const std::string& projectFilePath, bool verbose)
{
    if (verbose)
    {
        std::cout << "> " << cmprojFilePath << std::endl;
    }
    std::unique_ptr<sngxml::dom::Document> cmprojectDoc = sngxml::dom::ReadDocument(cmprojFilePath);
    std::string projectName = Path::GetFileNameWithoutExtension(cmprojFilePath);
    std::ofstream cmpFile(projectFilePath);
    CodeFormatter formatter(cmpFile);
    formatter.WriteLine("project " + projectName + ";");
    std::unique_ptr<sngxml::xpath::XPathObject> targetResult = sngxml::xpath::Evaluate(U"/Project/PropertyGroup/TargetType", cmprojectDoc.get());
    if (targetResult)
    {
        if (targetResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* targetTypes = static_cast<sngxml::xpath::XPathNodeSet*>(targetResult.get());
            if (targetTypes->Length() == 1)
            {
                sngxml::dom::Node* targetTypeNode = (*targetTypes)[0];
                if (targetTypeNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* targetTypeElement = static_cast<sngxml::dom::Element*>(targetTypeNode);
                    sngxml::dom::Node* targetNode = targetTypeElement->FirstChild();
                    if (targetNode)
                    {
                        if (targetNode->GetNodeType() == sngxml::dom::NodeType::textNode)
                        {
                            sngxml::dom::Text* targetText = static_cast<sngxml::dom::Text*>(targetNode);
                            const std::u32string& target = targetText->Data();
                            if (target == U"program")
                            {
                                formatter.WriteLine("target=program;");
                            }
                            else if (target == U"winapp")
                            {
                                formatter.WriteLine("target=winapp;");
                            }
                            else if (target == U"winguiapp")
                            {
                                formatter.WriteLine("target=winguiapp;");
                            }
                            else if (target == U"library")
                            {
                                formatter.WriteLine("target=library;");
                            }
                            else if (target == U"winlib")
                            {
                                formatter.WriteLine("target=winlib;");
                            }
                            else if (target == U"unitTest")
                            {
                                formatter.WriteLine("target=unitTest;");
                            }
                        }
                    }
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> referenceFileResult = sngxml::xpath::Evaluate(U"/Project/ItemGroup/ProjectReference", cmprojectDoc.get());
    if (referenceFileResult)
    {
        if (referenceFileResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* references = static_cast<sngxml::xpath::XPathNodeSet*>(referenceFileResult.get());
            int n = references->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* referenceNode = (*references)[i];
                if (referenceNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* referenceElement = static_cast<sngxml::dom::Element*>(referenceNode);
                    std::u32string referencePath = referenceElement->GetAttribute(U"Include");
                    if (!referencePath.empty())
                    {
                        std::string referenceProjectPath = Path::ChangeExtension(ToUtf8(referencePath), ".cmp");
                        formatter.WriteLine("reference <" + referenceProjectPath + ">;");
                    }
                }
            }

        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> sourceFileResult = sngxml::xpath::Evaluate(U"/Project/ItemGroup/CmCompile", cmprojectDoc.get());
    if (sourceFileResult)
    {
        if (sourceFileResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* sourceFiles = static_cast<sngxml::xpath::XPathNodeSet*>(sourceFileResult.get());
            int n = sourceFiles->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* sourceFileNode = (*sourceFiles)[i];
                if (sourceFileNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* sourceFileElement = static_cast<sngxml::dom::Element*>(sourceFileNode);
                    std::string sourceFilePath = ToUtf8(sourceFileElement->GetAttribute(U"Include"));
                    if (Path::GetExtension(sourceFilePath) == ".cm")
                    {
                        formatter.WriteLine("source <" + sourceFilePath + ">;");
                    }
                    else if (Path::GetExtension(sourceFilePath) == ".xml")
                    {
                        formatter.WriteLine("resource <" + sourceFilePath + ">;");
                    }
                }
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> textFileResult = sngxml::xpath::Evaluate(U"/Project/ItemGroup/None", cmprojectDoc.get());
    if (textFileResult)
    {
        if (textFileResult->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* textFiles = static_cast<sngxml::xpath::XPathNodeSet*>(textFileResult.get());
            int n = textFiles->Length();
            for (int i = 0; i < n; ++i)
            {
                sngxml::dom::Node* textFileNode = (*textFiles)[i];
                if (textFileNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* textFileElement = static_cast<sngxml::dom::Element*>(textFileNode);
                    std::string textFilePath = ToUtf8(textFileElement->GetAttribute(U"Include"));
                    formatter.WriteLine("text <" + textFilePath + ">;");
                }
            }
        }
    }
    if (verbose)
    {
        std::cout << "==> " << projectFilePath << std::endl;
    }
}

} } // namespace cmajor::cmproj
