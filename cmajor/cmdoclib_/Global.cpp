// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/Global.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/xpath/XPathEvaluate.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <mutex>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::util;
using namespace cmajor::unicode;

class DerivedClassMap
{
public:
    void AddDerivedClass(const std::u32string& baseClassId, const std::u32string& derivedClassId);
    std::vector<std::u32string> GetDerivedClassIds(const std::u32string& baseClassId) const;
    std::unique_ptr<dom::Element> ToDomElement();
private:
    std::unordered_map<std::u32string, std::unordered_set<std::u32string>> derivedClassMap;
};

void DerivedClassMap::AddDerivedClass(const std::u32string& baseClassId, const std::u32string& derivedClassId)
{
    std::unordered_set<std::u32string>& derivedClasses = derivedClassMap[baseClassId];
    derivedClasses.insert(derivedClassId);
}

std::vector<std::u32string> DerivedClassMap::GetDerivedClassIds(const std::u32string& baseClassId) const
{
    std::vector<std::u32string> derivedClassIds;
    auto it = derivedClassMap.find(baseClassId);
    if (it != derivedClassMap.cend())
    {
        const std::unordered_set<std::u32string>& ids = it->second;
        for (const std::u32string& id : ids)
        {
            derivedClassIds.push_back(id);
        }
    }
    return derivedClassIds;
}

std::unique_ptr<dom::Element> DerivedClassMap::ToDomElement()
{
    std::unique_ptr<dom::Element> mapElement(new dom::Element(U"derivedClassMap"));
    for (const auto& p : derivedClassMap)
    {
        const std::u32string& baseClassId = p.first;
        const std::unordered_set<std::u32string>& derivedClassIds = p.second;
        std::unique_ptr<dom::Element> baseClassElement(new dom::Element(U"baseClass"));
        baseClassElement->SetAttribute(U"classId", baseClassId);
        for (const std::u32string& derivedClassId : derivedClassIds)
        {
            std::unique_ptr<dom::Element> derivedClassElement(new dom::Element(U"derivedClass"));
            derivedClassElement->SetAttribute(U"classId", derivedClassId);
            baseClassElement->AppendChild(std::unique_ptr<dom::Node>(derivedClassElement.release()));
        }
        mapElement->AppendChild(std::unique_ptr<dom::Node>(baseClassElement.release()));
    }
    return mapElement;
}

class Global
{
public:
    static Global& Instance() { return *instance; }
    static void Init();
    static void Done();
    void Read(const std::vector<std::u32string>& moduleNames);
    void Write(const std::vector<std::u32string>& moduleNames);
    DerivedClassMap* GetDerivedClassMap(const std::u32string& moduleName);
private:
    static std::unique_ptr<Global> instance;
    std::unordered_map<std::u32string, DerivedClassMap*> moduleDerivedClassMap;
    std::vector<std::unique_ptr<DerivedClassMap>> derivedClassMaps;
};

std::unique_ptr<Global> Global::instance;

void Global::Init()
{
    instance.reset(new Global());
}
void Global::Done()
{
    instance.reset();
}

void Global::Read(const std::vector<std::u32string>& moduleNames)
{
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    for (const std::u32string& moduleName : moduleNames)
    {
        std::string moduleDir = GetFullPath(Path::Combine(contentDir, ToUtf8(moduleName)));
        std::string globalXmlPath = GetFullPath(Path::Combine(moduleDir, "global.xml"));
        if (boost::filesystem::exists(globalXmlPath))
        {
            std::unique_ptr<dom::Document> globalDoc = dom::ReadDocument(globalXmlPath);
            std::unique_ptr<xpath::XPathObject> baseClasses = xpath::Evaluate(U"/global/derivedClassMap/baseClass", globalDoc.get());
            if (baseClasses->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* baseClassNodeSet = static_cast<xpath::XPathNodeSet*>(baseClasses.get());
                int n = baseClassNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* baseClassNode = (*baseClassNodeSet)[i];
                    if (baseClassNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* baseClassElement = static_cast<dom::Element*>(baseClassNode);
                        std::u32string baseClassId = baseClassElement->GetAttribute(U"classId");
                        std::unique_ptr<xpath::XPathObject> derivedClasses = xpath::Evaluate(U"derivedClass", baseClassElement);
                        if (derivedClasses->Type() == xpath::XPathObjectType::nodeSet)
                        {
                            xpath::XPathNodeSet* derivedClassNodeSet = static_cast<xpath::XPathNodeSet*>(derivedClasses.get());
                            int n = derivedClassNodeSet->Length();
                            for (int i = 0; i < n; ++i)
                            {
                                dom::Node* derivedClassNode = (*derivedClassNodeSet)[i];
                                if (derivedClassNode->GetNodeType() == dom::NodeType::elementNode)
                                {
                                    dom::Element* derivedClassElement = static_cast<dom::Element*>(derivedClassNode);
                                    std::u32string derivedClassId = derivedClassElement->GetAttribute(U"classId");
                                    GetDerivedClassMap(moduleName)->AddDerivedClass(baseClassId, derivedClassId);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Global::Write(const std::vector<std::u32string>& moduleNames)
{
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    for (const std::u32string& moduleName : moduleNames)
    {
        std::string moduleDir = GetFullPath(Path::Combine(contentDir, ToUtf8(moduleName)));
        std::string globalXmlPath = GetFullPath(Path::Combine(moduleDir, "global.xml"));
        dom::Document globalDoc;
        std::unique_ptr<dom::Element> globalElement(new dom::Element(U"global"));
        DerivedClassMap* derivedClassMap = moduleDerivedClassMap[moduleName];
        if (derivedClassMap)
        {
            std::unique_ptr<dom::Element> derivedClassMapElement = derivedClassMap->ToDomElement();
            globalElement->AppendChild(std::unique_ptr<dom::Node>(derivedClassMapElement.release()));
            globalDoc.AppendChild(std::unique_ptr<dom::Node>(globalElement.release()));
            std::ofstream globalFile(globalXmlPath);
            CodeFormatter formatter(globalFile);
            globalDoc.Write(formatter);
        }
    }
}

DerivedClassMap* Global::GetDerivedClassMap(const std::u32string& moduleName)
{
    DerivedClassMap* derivedClassMap = moduleDerivedClassMap[moduleName];
    if (!derivedClassMap)
    {
        derivedClassMap = new DerivedClassMap();
        moduleDerivedClassMap[moduleName] = derivedClassMap;
        derivedClassMaps.push_back(std::unique_ptr<DerivedClassMap>(derivedClassMap));
    }
    return derivedClassMap;
}

std::mutex globalMutex;

void ReadGlobals(const std::vector<std::u32string>& moduleNames)
{
    Global::Instance().Read(moduleNames);
}

void WriteGlobals(const std::vector<std::u32string>& moduleNames)
{
    Global::Instance().Write(moduleNames);
}

void AddDerivedClass(ClassTypeSymbol* baseClass, ClassTypeSymbol* derivedClass)
{
    std::lock_guard<std::mutex> lock(globalMutex);
    Module* originalModule = baseClass->GetModule();
    std::u32string moduleName = originalModule->Name();
    Global::Instance().GetDerivedClassMap(moduleName)->AddDerivedClass(baseClass->Id(), derivedClass->Id());
}

void AddConceptRefinement(ConceptSymbol* refinedConcept, ConceptSymbol* concept)
{
    std::lock_guard<std::mutex> lock(globalMutex);
    Module* originalModule = refinedConcept->GetModule();
    std::u32string moduleName = originalModule->Name();
    Global::Instance().GetDerivedClassMap(moduleName)->AddDerivedClass(refinedConcept->Id(), concept->Id());
}

std::vector<std::u32string> GetDerivedClassIds(const std::u32string& moduleName, const std::u32string& baseClassId)
{
    std::lock_guard<std::mutex> lock(globalMutex);
    return Global::Instance().GetDerivedClassMap(moduleName)->GetDerivedClassIds(baseClassId);
}

std::vector<std::u32string> GetDerivedConceptIds(const std::u32string& moduleName, const std::u32string& refinedConceptId)
{
    std::lock_guard<std::mutex> lock(globalMutex);
    return Global::Instance().GetDerivedClassMap(moduleName)->GetDerivedClassIds(refinedConceptId);
}

void WriteModuleXml(const std::string& targetDir, const std::vector<std::u32string>& moduleNames)
{
    std::unique_ptr<dom::Document> moduleXmlDoc(new dom::Document());
    std::unique_ptr<dom::Element> modulesElement(new dom::Element(U"modules"));
    for (const std::u32string& moduleName : moduleNames)
    {
        std::unique_ptr<dom::Element> moduleElement(new dom::Element(U"module"));
        moduleElement->SetAttribute(U"name", moduleName);
        modulesElement->AppendChild(std::unique_ptr<dom::Node>(moduleElement.release()));
    }
    moduleXmlDoc->AppendChild(std::unique_ptr<dom::Node>(modulesElement.release()));
    std::string moduleXmlFilePath = Path::Combine(targetDir, "modules.xml");
    std::ofstream moduleXmlFile(moduleXmlFilePath);
    CodeFormatter formatter(moduleXmlFile);
    formatter.SetIndentSize(1);
    moduleXmlDoc->Write(formatter);
}

void ReadGrammars(Input* input)
{
    for (const std::string& libraryPrefix : input->libraryDirs)
    {
        std::string globalGrammarFilePath = GetFullPath(Path::Combine(Path::Combine(input->targetDirPath, libraryPrefix), "grammars.xml"));
        std::unique_ptr<dom::Document> globalGrammarDoc = dom::ReadDocument(globalGrammarFilePath);
        std::unique_ptr<xpath::XPathObject> globalGrammarsObject = xpath::Evaluate(U"/grammars/grammar", globalGrammarDoc.get());
        if (globalGrammarsObject->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* globalGrammarNodeSet = static_cast<xpath::XPathNodeSet*>(globalGrammarsObject.get());
            int n = globalGrammarNodeSet->Length();
            for (int i = 0; i < n; ++i)
            {
                dom::Node* globalGrammarNode = (*globalGrammarNodeSet)[i];
                if (globalGrammarNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* globalGrammarElement = static_cast<dom::Element*>(globalGrammarNode);
                    std::string grammarFilePath = ToUtf8(globalGrammarElement->GetAttribute(U"filePath"));
                    std::unique_ptr<dom::Document> localGrammarDoc = dom::ReadDocument(Path::Combine(Path::Combine(input->targetDirPath, libraryPrefix), grammarFilePath));
                    std::unique_ptr<xpath::XPathObject> localGrammarsObject = xpath::Evaluate(U"/grammars/grammar", localGrammarDoc.get());
                    if (localGrammarsObject->Type() == xpath::XPathObjectType::nodeSet)
                    {
                        xpath::XPathNodeSet* localGrammarNodeSet = static_cast<xpath::XPathNodeSet*>(localGrammarsObject.get());
                        int nl = localGrammarNodeSet->Length();
                        for (int i = 0; i < nl; ++i)
                        {
                            dom::Node* localGrammarNode = (*localGrammarNodeSet)[i];
                            if (localGrammarNode->GetNodeType() == dom::NodeType::elementNode)
                            {
                                dom::Element* localGrammarElement = static_cast<dom::Element*>(localGrammarNode);
                                std::u32string localGrammarId = localGrammarElement->GetAttribute(U"id");
                                std::string localGrammarFilePath = Path::Combine(Path::Combine("../../..", libraryPrefix), grammarFilePath);
                                input->grammarMap[localGrammarId] = localGrammarFilePath;
                            }
                        }
                    }
                }
            }
        }
    }
}

void WriteGrammars(Input* input, const std::vector<std::string>& grammarFilePaths)
{
    std::string globalGrammarFilePath = GetFullPath(Path::Combine(input->targetDirPath, "grammars.xml"));
    std::ofstream globalGrammarFile(globalGrammarFilePath);
    CodeFormatter formatter(globalGrammarFile);
    formatter.SetIndentSize(1);
    dom::Document globalGrammarDoc;
    std::unique_ptr<dom::Element> grammarsElement(new dom::Element(U"grammars"));
    for (const std::string& grammarFilePath : grammarFilePaths)
    {
        std::unique_ptr<dom::Element> grammarElement(new dom::Element(U"grammar"));
        grammarElement->SetAttribute(U"filePath", ToUtf32(grammarFilePath));
        grammarsElement->AppendChild(std::unique_ptr<dom::Node>(grammarElement.release()));
    }
    globalGrammarDoc.AppendChild(std::unique_ptr<dom::Node>(grammarsElement.release()));
    globalGrammarDoc.Write(formatter);
}

void GlobalInit()
{
    Global::Init();
}

void GlobalDone()
{
    Global::Done();
}

} } // namespace cmajor::cmdoclib
