// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <iostream>

namespace cmajor { namespace cmdoclib {

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;

std::mutex mtx;

std::mutex& GetInputMutex()
{
    return mtx;
}

Input ReadInputXml(const std::string& cmDocFilePath)
{
    Input input;
    input.baseDir = GetFullPath(Path::GetDirectoryName(Path::MakeCanonical(cmDocFilePath)));
    bool verbose = GetGlobalFlag(GlobalFlags::verbose);
    if (verbose)
    {
        std::cout << "> " << cmDocFilePath << std::endl;
    }
    std::unique_ptr<sngxml::dom::Document> inputDoc = sngxml::dom::ReadDocument(cmDocFilePath);
    std::unique_ptr<sngxml::xpath::XPathObject> solutionObject = sngxml::xpath::Evaluate(U"/cmdoc/solution", inputDoc.get());
    if (solutionObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* solutionNodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(solutionObject.get());
        if (solutionNodeSet->Length() == 1)
        {
            sngxml::dom::Node* solutionNode = (*solutionNodeSet)[0];
            if (solutionNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* solutionElement = static_cast<sngxml::dom::Element*>(solutionNode);
                std::string relativeSolutionFilePath = ToUtf8(solutionElement->GetAttribute(U"filePath"));
                input.relativeSolutionFilePath = Path::MakeCanonical(relativeSolutionFilePath);
                std::string solutionFilePath = GetFullPath(Path::Combine(input.baseDir, relativeSolutionFilePath));
                input.solutionFilePath = solutionFilePath;
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> docObject = sngxml::xpath::Evaluate(U"/cmdoc/doc", inputDoc.get());
    if (docObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* docNodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(docObject.get());
        if (docNodeSet->Length() == 1)
        {
            sngxml::dom::Node* docNode = (*docNodeSet)[0];
            if (docNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* docElement = static_cast<sngxml::dom::Element*>(docNode);
                std::string docFilePath = GetFullPath(Path::Combine(input.baseDir, ToUtf8(docElement->GetAttribute(U"filePath"))));
                input.docs = sngxml::dom::ReadDocument(docFilePath);
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> targetObject = sngxml::xpath::Evaluate(U"/cmdoc/target", inputDoc.get());
    if (targetObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* targetNodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(targetObject.get());
        if (targetNodeSet->Length() == 1)
        {
            sngxml::dom::Node* targetNode = (*targetNodeSet)[0];
            if (targetNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* targetElement = static_cast<sngxml::dom::Element*>(targetNode);
                std::string relativeTargetDirPath = ToUtf8(targetElement->GetAttribute(U"dir"));
                std::string targetDirPath = GetFullPath(Path::Combine(input.baseDir, relativeTargetDirPath));
                input.targetDirPath = targetDirPath;
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> scm2htmlObject = sngxml::xpath::Evaluate(U"/cmdoc/scm2html", inputDoc.get());
    if (scm2htmlObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(scm2htmlObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                std::string filePath = ToUtf8(element->GetAttribute(U"filePath"));
                input.scm2htmlFilePath = GetFullPath(Path::Combine(input.baseDir, filePath));
            }
        }
    }
    std::unique_ptr<sngxml::xpath::XPathObject> libraryObjects = sngxml::xpath::Evaluate(U"/cmdoc/libraries/library", inputDoc.get());
    if (libraryObjects->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* libraryNodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(libraryObjects.get());
        int  n = libraryNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* libraryNode = (*libraryNodeSet)[i];
            if (libraryNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* libraryElement = static_cast<sngxml::dom::Element*>(libraryNode);
                std::string libraryDir = ToUtf8(libraryElement->GetAttribute(U"dir"));
                if (!libraryDir.empty())
                {
                    input.libraryDirs.push_back(libraryDir);
                    std::string libraryPrefix = Path::Combine(libraryDir, "content");
                    std::string libDir = Path::Combine(input.baseDir, libraryDir);
                    std::unique_ptr<sngxml::dom::Document> modulesDoc = sngxml::dom::ReadDocument(Path::Combine(libDir, "modules.xml"));
                    std::unique_ptr<sngxml::xpath::XPathObject> moduleObjects = sngxml::xpath::Evaluate(U"/modules/module", modulesDoc.get());
                    if (moduleObjects->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                    {
                        sngxml::xpath::XPathNodeSet* moduleNodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(moduleObjects.get());
                        int nm = moduleNodeSet->Length();
                        for (int i = 0; i < nm; ++i)
                        {
                            sngxml::dom::Node* moduleNode = (*moduleNodeSet)[i];
                            if (moduleNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                            {
                                sngxml::dom::Element* moduleElement = static_cast<sngxml::dom::Element*>(moduleNode);
                                std::u32string moduleName = moduleElement->GetAttribute(U"name");
                                input.libraryPrefixMap[moduleName] = libraryPrefix;
                            }
                        }
                    }
                }
            }
        }
    }
    return input;
}

Input* input = nullptr;

void SetInputPtr(Input* inputPtr)
{
    input = inputPtr;
}

Input* GetInputPtr()
{
    if (!input)
    {
        throw std::runtime_error("input not set");
    }
    return input;
}

void SetEmptyLibraryPrefix(const std::u32string& moduleName)
{
    Input* input = GetInputPtr();
    auto it = input->libraryPrefixMap.find(moduleName);
    if (it != input->libraryPrefixMap.cend())
    {
        throw std::runtime_error("library prefix for module '" + ToUtf8(moduleName) + "' already set");
    }
    else
    {
        input->libraryPrefixMap[moduleName] = "";
    }
}

} } // namespace cmajor::cmdoclib
