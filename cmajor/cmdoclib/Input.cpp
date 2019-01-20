// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/xpath/XPathEvaluate.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Path.hpp>
#include <iostream>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::util;
using namespace cmajor::unicode;
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
    std::unique_ptr<dom::Document> inputDoc = dom::ReadDocument(cmDocFilePath);
    std::unique_ptr<xpath::XPathObject> solutionObject = xpath::Evaluate(U"/cmdoc/solution", inputDoc.get());
    if (solutionObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* solutionNodeSet = static_cast<xpath::XPathNodeSet*>(solutionObject.get());
        if (solutionNodeSet->Length() == 1)
        {
            dom::Node* solutionNode = (*solutionNodeSet)[0];
            if (solutionNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* solutionElement = static_cast<dom::Element*>(solutionNode);
                std::string relativeSolutionFilePath = ToUtf8(solutionElement->GetAttribute(U"filePath"));
                input.relativeSolutionFilePath = Path::MakeCanonical(relativeSolutionFilePath);
                std::string solutionFilePath = GetFullPath(Path::Combine(input.baseDir, relativeSolutionFilePath));
                input.solutionFilePath = solutionFilePath;
            }
        }
    }
    std::unique_ptr<xpath::XPathObject> docObject = xpath::Evaluate(U"/cmdoc/doc", inputDoc.get());
    if (docObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* docNodeSet = static_cast<xpath::XPathNodeSet*>(docObject.get());
        if (docNodeSet->Length() == 1)
        {
            dom::Node* docNode = (*docNodeSet)[0];
            if (docNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* docElement = static_cast<dom::Element*>(docNode);
                std::string docFilePath = GetFullPath(Path::Combine(input.baseDir, ToUtf8(docElement->GetAttribute(U"filePath"))));
                input.docs = dom::ReadDocument(docFilePath);
            }
        }
    }
    std::unique_ptr<xpath::XPathObject> targetObject = xpath::Evaluate(U"/cmdoc/target", inputDoc.get());
    if (targetObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* targetNodeSet = static_cast<xpath::XPathNodeSet*>(targetObject.get());
        if (targetNodeSet->Length() == 1)
        {
            dom::Node* targetNode = (*targetNodeSet)[0];
            if (targetNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* targetElement = static_cast<dom::Element*>(targetNode);
                std::string relativeTargetDirPath = ToUtf8(targetElement->GetAttribute(U"dir"));
                std::string targetDirPath = GetFullPath(Path::Combine(input.baseDir, relativeTargetDirPath));
                input.targetDirPath = targetDirPath;
            }
        }
    }
    std::unique_ptr<xpath::XPathObject> libraryObjects = xpath::Evaluate(U"/cmdoc/libraries/library", inputDoc.get());
    if (libraryObjects->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* libraryNodeSet = static_cast<xpath::XPathNodeSet*>(libraryObjects.get());
        int  n = libraryNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* libraryNode = (*libraryNodeSet)[i];
            if (libraryNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* libraryElement = static_cast<dom::Element*>(libraryNode);
                std::string libraryDir = ToUtf8(libraryElement->GetAttribute(U"dir"));
                if (!libraryDir.empty())
                {
                    input.libraryDirs.push_back(libraryDir);
                    std::string libraryPrefix = Path::Combine(libraryDir, "content");
                    std::string libDir = Path::Combine(input.baseDir, libraryDir);
                    std::unique_ptr<dom::Document> modulesDoc = dom::ReadDocument(Path::Combine(libDir, "modules.xml"));
                    std::unique_ptr<xpath::XPathObject> moduleObjects = xpath::Evaluate(U"/modules/module", modulesDoc.get());
                    if (moduleObjects->Type() == xpath::XPathObjectType::nodeSet)
                    {
                        xpath::XPathNodeSet* moduleNodeSet = static_cast<xpath::XPathNodeSet*>(moduleObjects.get());
                        int nm = moduleNodeSet->Length();
                        for (int i = 0; i < nm; ++i)
                        {
                            dom::Node* moduleNode = (*moduleNodeSet)[i];
                            if (moduleNode->GetNodeType() == dom::NodeType::elementNode)
                            {
                                dom::Element* moduleElement = static_cast<dom::Element*>(moduleNode);
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
