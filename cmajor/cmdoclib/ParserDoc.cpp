// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/ParserDoc.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Parser.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::symbols;
using namespace soulng::util;
using namespace soulng::unicode;

void GeneratePPXml(sngcm::ast::Project* project)
{
    std::string styleFilePath = "../../../style/style.css";
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    boost::filesystem::create_directories(targetDir);
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    boost::filesystem::create_directories(contentDir);
    std::string projectDir = GetFullPath(Path::Combine(contentDir, ToUtf8(project->Name())));
    boost::filesystem::create_directories(projectDir);
    std::string ppXmlFilePath = GetFullPath(Path::Combine(projectDir, "pp.xml"));
    std::ofstream ppXmlFile(ppXmlFilePath);
    CodeFormatter formatter(ppXmlFile);
    formatter.SetIndentSize(1);
    sngxml::dom::Document ppDoc;
    std::unique_ptr<sngxml::dom::Element> parserProjectsElement(new sngxml::dom::Element(U"parserProjects"));
    for (const std::string& textFilePath : project->RelativeTextFilePaths())
    {
        if (Path::GetExtension(textFilePath) == ".pp")
        {
            std::unique_ptr<sngxml::dom::Element> parserProjectElement(new sngxml::dom::Element(U"parserProject"));
            std::string ppFilePath = Path::Combine(Path::Combine(
                Path::Combine("../..", Path::GetDirectoryName(input->relativeSolutionFilePath)), Path::GetDirectoryName(project->RelativeFilePath())), textFilePath);
            parserProjectElement->SetAttribute(U"filePath", ToUtf32(ppFilePath));
            parserProjectElement->SetAttribute(U"styleFilePath", ToUtf32(styleFilePath));
            parserProjectsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parserProjectElement.release()));
        }
    }
    ppDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(parserProjectsElement.release()));
    ppDoc.Write(formatter);
}

void GenerateGmXml(Input* input, const std::string& moduleDir)
{
    std::string gmXmlFilePath = GetFullPath(Path::Combine(moduleDir, "gm.xml"));
    std::ofstream gmXmlFile(gmXmlFilePath);
    CodeFormatter formatter(gmXmlFile);
    formatter.SetIndentSize(1);
    sngxml::dom::Document gmDoc;
    std::unique_ptr<sngxml::dom::Element> grammarsElement(new sngxml::dom::Element(U"grammars"));
    for (const auto& p : input->grammarMap)
    {
        std::unique_ptr<sngxml::dom::Element> grammarElement(new sngxml::dom::Element(U"grammar"));
        grammarElement->SetAttribute(U"id", p.first);
        grammarElement->SetAttribute(U"docPath", ToUtf32(p.second));
        grammarsElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(grammarElement.release()));
    }
    gmDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(grammarsElement.release()));
    gmDoc.Write(formatter);
}

void BuildParserDocs(Input* input, const std::string& moduleDir, const std::string& grammarXmlFilePath, const std::string& relativeModuleDir, 
    std::vector<std::string>& grammarFilePaths, const std::u32string& moduleName, std::vector<GrammarInfo>& grammars)
{
    std::unique_ptr<sngxml::dom::Document> parserProjectsDoc = sngxml::dom::ReadDocument(Path::Combine(moduleDir, "pp.xml"));
    std::unique_ptr<sngxml::xpath::XPathObject> parserProjectObject = sngxml::xpath::Evaluate(U"/parserProjects/parserProject", parserProjectsDoc.get());
    if (parserProjectObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
    {
        sngxml::xpath::XPathNodeSet* parserProjectNodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(parserProjectObject.get());
        int n = parserProjectNodeSet->Length();
        if (n > 0)
        {
            std::string command;
            bool verbose = GetGlobalFlag(GlobalFlags::verbose);
            command.append("cmparser2html ");
            if (verbose)
            {
                command.append("--verbose ");
            }
            command.append("\"" + moduleDir + "\"");
            GenerateGmXml(input, moduleDir);
            soulng::util::System(command);
            std::string grammarFilePath = Path::Combine(relativeModuleDir, Path::GetFileName(grammarXmlFilePath));
            grammarFilePaths.push_back(grammarFilePath);
            std::unique_ptr<sngxml::dom::Document> grammarDoc = sngxml::dom::ReadDocument(grammarXmlFilePath);
            std::unique_ptr<sngxml::xpath::XPathObject> grammarsObject = sngxml::xpath::Evaluate(U"/grammars/grammar", grammarDoc.get());
            if (grammarsObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
            {
                sngxml::xpath::XPathNodeSet* grammarsNodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(grammarsObject.get());
                int ng = grammarsNodeSet->Length();
                for (int i = 0; i < ng; ++i)
                {
                    sngxml::dom::Node* grammarNode = (*grammarsNodeSet)[i];
                    if (grammarNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                    {
                        sngxml::dom::Element* grammarElement = static_cast<sngxml::dom::Element*>(grammarNode);
                        std::u32string grammarName = grammarElement->GetAttribute(U"name");
                        std::u32string grammarId = grammarElement->GetAttribute(U"id");
                        std::string grammarDocPath = ToUtf8(grammarElement->GetAttribute(U"docPath"));
                        GrammarInfo grammarInfo(grammarName, grammarId, grammarDocPath);
                        grammars.push_back(grammarInfo);
                        std::sort(grammars.begin(), grammars.end(), ByGrammarName());
                        input->grammarMap[grammarId] = Path::Combine(Path::Combine("../..", ToUtf8(moduleName)), grammarDocPath);
                    }
                }
            }

        }
    }
}

} } // namespace cmajor::cmdoclib
