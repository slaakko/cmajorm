// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Unicode.hpp>
#include <stdexcept>
#include <iostream>
#include <fstream>

void PrintHelp()
{
    // todo
}

using namespace soulng::util;
using namespace soulng::unicode;

void InitApplication()
{
    soulng::util::Init();
    sngxml::xpath::Init();
}

void DoneApplication()
{
    sngxml::xpath::Done();
    soulng::util::Done();
}

void ConvertNodeToDigraphElement(sngxml::dom::Element* element, CodeFormatter& formatter)
{
    std::u32string idAttr = element->GetAttribute(U"id");
    if (idAttr.empty())
    {
        throw std::runtime_error("element " + ToUtf8(element->Name()) + " has no 'id' attribute");
    }
    std::string id = ToUtf8(idAttr);
    std::string dominanceFrontierIdSet("df=[");
    std::unique_ptr<sngxml::xpath::XPathObject> frontierObject = sngxml::xpath::Evaluate(U"dominance_frontier_node", element);
    if (frontierObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(frontierObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                std::u32string frontierNodeIdAttr = element->GetAttribute(U"id");
                if (frontierNodeIdAttr.empty())
                {
                    throw std::runtime_error("frontier element " + std::to_string(i) + " has no 'id' attribute");
                }
                std::string frontierNodeId = ToUtf8(frontierNodeIdAttr);
                if (i > 0)
                {
                    dominanceFrontierIdSet.append(", ");
                }
                dominanceFrontierIdSet.append(frontierNodeId);
            }
        }
    }
    dominanceFrontierIdSet.append("]");
    formatter.WriteLine(id + " [label=\"" + id + " " + dominanceFrontierIdSet + "\", shape=box];");
    std::unique_ptr<sngxml::xpath::XPathObject> childNodeObject = sngxml::xpath::Evaluate(U"node", element);
    if (childNodeObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(childNodeObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                std::u32string childIdAttr = element->GetAttribute(U"id");
                if (childIdAttr.empty())
                {
                    throw std::runtime_error("child element " + std::to_string(i) + " has no 'id' attribute");
                }
                std::string childId = ToUtf8(childIdAttr);
                formatter.WriteLine(id + " -> " + childId + ";");
                ConvertNodeToDigraphElement(element, formatter);
            }
        }
    }

}

void ConvertXmlFileToGraphFile(const std::string& xmlFilePath, const std::string& graphFilePath)
{
    std::unique_ptr<sngxml::dom::Document> doc = sngxml::dom::ReadDocument(xmlFilePath);
    sngxml::dom::Element* root = doc->DocumentElement();
    std::ofstream graphFile(graphFilePath);
    CodeFormatter formatter(graphFile);
    formatter.WriteLine("digraph G");
    formatter.WriteLine("{");
    formatter.IncIndent();
    std::unique_ptr<sngxml::xpath::XPathObject> rootObject = sngxml::xpath::Evaluate(U"/dominator_tree/node", doc.get());
    if (rootObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(rootObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                ConvertNodeToDigraphElement(element, formatter);
            }
        }
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void ConvertGraphFileToPng(const std::string& graphFilePath, const std::string& pngFilePath)
{
    std::string commandLine = "dot ";
    commandLine.append("-Tpng ").append("\"").append(graphFilePath).append("\" -o\"").append(pngFilePath).append("\"");
    Process process(commandLine, Process::Redirections::none);
    process.WaitForExit();
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        bool verbose = false;
        bool dot = false;
        std::vector<std::string> xmlFileNames;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg.starts_with("--"))
            {
                if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--verbose")
                {
                    verbose = true;
                }
                else if (arg == "--dot")
                {
                    dot = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (arg.starts_with("-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                    case 'h':
                    {
                        PrintHelp();
                        return 1;
                    }
                    case 'v':
                    {
                        verbose = true;
                        break;
                    }
                    case 'd':
                    {
                        dot = true;
                        break;
                    }
                    default:
                    {
                        throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                    }
                    }
                }
            }
            else
            {
                xmlFileNames.push_back(GetFullPath(arg));
            }
        }
        for (const std::string& xmlFileName : xmlFileNames)
        {
            if (verbose)
            {
                std::cout << "> " << xmlFileName << std::endl;
            }
            std::string graphFileName = Path::ChangeExtension(xmlFileName, ".graph");
            ConvertXmlFileToGraphFile(xmlFileName, graphFileName);
            if (verbose)
            {
                std::cout << "==> " << graphFileName << std::endl;
            }
            if (dot)
            {
                std::string pngFileName = Path::ChangeExtension(graphFileName, ".png");
                ConvertGraphFileToPng(graphFileName, pngFileName);
                if (verbose)
                {
                    std::cout << "==> " << pngFileName << std::endl;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    DoneApplication();
    return 0;
}
