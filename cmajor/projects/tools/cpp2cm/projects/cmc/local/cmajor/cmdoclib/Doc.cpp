// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/Doc.hpp>
#include <cmajor/cmdoclib/ParserDoc.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/cmdoclib/Global.hpp>
#include <cmajor/cmdoclib/Diagram.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/xpath/XPathEvaluate.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Util.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::symbols;
using namespace cmajor::util;
using namespace cmajor::unicode;

class ContentVisitor : public dom::Visitor
{
public:
    ContentVisitor(Input* input_, dom::Element* parentElement_, dom::Document* moduleXmlDoc_, const std::vector<dom::Document*>& otherModuleXmlDocs_, const std::u32string& prefix_);
    void BeginVisit(dom::Element* element) override;
    void EndVisit(dom::Element* element) override;
    void Visit(dom::Text* text) override;
    void Visit(dom::CDataSection* cdataSection)  override;
    void Visit(dom::EntityReference* entityReference) override;
private:
    Input* input;
    dom::Element* parentElement;
    std::stack<dom::Element*> elementStack;
    std::stack<std::unique_ptr<dom::Element>> currentElementStack;
    std::unique_ptr<dom::Element> currentElement;
    dom::Document* moduleXmlDoc;
    std::vector<dom::Document*> otherModuleXmlDocs;
    bool hasContent;
    std::u32string prefix;
    std::stack<bool> inRefStack;
    bool inRef;
    bool space;
};

ContentVisitor::ContentVisitor(Input* input_, dom::Element* parentElement_, dom::Document* moduleXmlDoc_, const std::vector<dom::Document*>& otherModuleXmlDocs_, 
    const std::u32string& prefix_) :
    input(input_), parentElement(parentElement_), moduleXmlDoc(moduleXmlDoc_), otherModuleXmlDocs(otherModuleXmlDocs_), hasContent(false), prefix(prefix_), 
    inRef(false), space(false)
{
}

void ContentVisitor::BeginVisit(dom::Element* element)
{
    currentElementStack.push(std::move(currentElement));
    inRefStack.push(inRef);
    inRef = false;
    if (element->Name() == U"ref")
    {
        if (element->ChildNodes().Length() != 1)
        {
            throw std::runtime_error("ref: one text child node with nonempty id expected");
        }
        else
        {
            dom::Node* child = element->ChildNodes()[0];
            if (child->GetNodeType() == dom::NodeType::textNode)
            {
                dom::Text* text = static_cast<dom::Text*>(child);
                std::u32string id = text->Data();
                if (!id.empty())
                {
                    std::string libraryPrefix;
                    std::u32string link = prefix;
                    dom::Element* element = nullptr;
                    if (moduleXmlDoc)
                    {
                        element = moduleXmlDoc->GetElementById(id);
                    }
                    if (!element)
                    {
                        int n = otherModuleXmlDocs.size();
                        for (int i = 0; i < n; ++i)
                        {
                            dom::Document* otherModuleXmlDoc = otherModuleXmlDocs[i];
                            element = otherModuleXmlDoc->GetElementById(id);
                            if (element)
                            {
                                std::u32string moduleName = otherModuleXmlDoc->DocumentElement()->GetAttribute(U"module");
                                auto it = input->libraryPrefixMap.find(moduleName);
                                if (it != input->libraryPrefixMap.cend())
                                {
                                    libraryPrefix = it->second;
                                    if (!libraryPrefix.empty())
                                    {
                                        libraryPrefix = Path::Combine("../../..", libraryPrefix);
                                    }
                                }
                                break;
                            }
                        }
                    }
                    if (!element)
                    {
                        throw std::runtime_error("ref: id '" + ToUtf8(id) + "' not found");
                    }
                    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                    std::u32string text;
                    if (element->Name() == U"symbolTable")
                    {
                        link.append(U"../../").append(id).append(U"/index.html");
                        text = id;
                    }
                    else
                    {
                        if (libraryPrefix.empty())
                        {
                            link.append(element->GetAttribute(U"docPath"));
                        }
                        else
                        {
                            link.append(ToUtf32(Path::Combine(libraryPrefix, ToUtf8(element->GetAttribute(U"extPath")))));
                        }
                        text = element->GetAttribute(U"name");
                    }
                    linkElement->SetAttribute(U"href", link);
                    linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(text)));
                    if (hasContent)
                    {
                        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
                    }
                    parentElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                    hasContent = true;
                    inRef = true;
                }
                else
                {
                    throw std::runtime_error("ref: one text child node with nonempty id expected");
                }
            }
            else
            {
                throw std::runtime_error("ref: one text child node with nonempty id expected");
            }
        }
    }
    else if (element->Name() != U"desc" && element->Name() != U"details")
    {
        if (space)
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        }
        currentElement.reset(static_cast<dom::Element*>(element->CloneNode(false).release()));
        elementStack.push(parentElement);
        parentElement = currentElement.get();
        hasContent = true;
        space = false;
    }
}

void ContentVisitor::EndVisit(dom::Element* element)
{
    inRef = inRefStack.top();
    inRefStack.pop();
    if (currentElement)
    {
        parentElement = elementStack.top();
        elementStack.pop();
        parentElement->AppendChild(std::unique_ptr<dom::Node>(currentElement.release()));
    }
    currentElement = std::move(currentElementStack.top());
    currentElementStack.pop();
}

void ContentVisitor::Visit(dom::Text* text)
{
    space = false;
    if (inRef) return;
    if (!text->Data().empty())
    {
        if (text->Data()[text->Data().size() - 1] == ' ')
        {
            space = true;
        }
    }
    std::u32string textContent = Trim(text->Data());
    if (!textContent.empty())
    {
        std::u32string s;
        if (hasContent)
        {
            s.append(U" ");
        }
        s.append(textContent);
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(s)));
        hasContent = true;
    }
}

void ContentVisitor::Visit(dom::CDataSection* cdataSection)
{
    if (space)
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
    }
    parentElement->AppendChild(cdataSection->CloneNode(true));
    hasContent = true;
    space = false;
}

void ContentVisitor::Visit(dom::EntityReference* entityReference)
{
    if (space)
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
    }
    parentElement->AppendChild(entityReference->CloneNode(true));
    hasContent = true;
    space = entityReference->Data() == U"nbsp";
}

bool AppendDescription(dom::Element* parentElement, Input* input, const std::u32string& docId, dom::Document* docs,
    dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, const std::u32string& prefix)
{
    if (!docs) return false;
    bool appended = false;
    std::lock_guard<std::mutex> lock(GetInputMutex());
    dom::Element* docElement = docs->GetElementById(docId);
    if (docElement)
    {
        std::unique_ptr<xpath::XPathObject> descriptionObject = xpath::Evaluate(U"desc", docElement);
        if (descriptionObject->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* descriptionNodeSet = static_cast<xpath::XPathNodeSet*>(descriptionObject.get());
            if (descriptionNodeSet->Length() == 1)
            {
                dom::Node* descriptionNode = (*descriptionNodeSet)[0];
                if (descriptionNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* descriptionElement = static_cast<dom::Element*>(descriptionNode);
                    ContentVisitor visitor(input, parentElement, moduleXmlDoc, otherModuleXmlDocs, prefix);
                    descriptionElement->Accept(visitor);
                    appended = true;
                }
            }
        }
    }
    return appended;
}

std::unique_ptr<dom::Element> GetDescription(Input* input, const std::u32string& docId, dom::Document* docs, 
    dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, const std::u32string& prefix)
{
    if (!docs) return std::unique_ptr<dom::Element>();
    std::lock_guard<std::mutex> lock(GetInputMutex());
    std::unique_ptr<dom::Element> descriptionParagraph;
    dom::Element* docElement = docs->GetElementById(docId);
    if (docElement)
    {
        std::unique_ptr<xpath::XPathObject> descriptionObject = xpath::Evaluate(U"desc", docElement);
        if (descriptionObject->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* descriptionNodeSet = static_cast<xpath::XPathNodeSet*>(descriptionObject.get());
            if (descriptionNodeSet->Length() == 1)
            {
                dom::Node* descriptionNode = (*descriptionNodeSet)[0];
                if (descriptionNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* descriptionElement = static_cast<dom::Element*>(descriptionNode);
                    descriptionParagraph.reset(new dom::Element(U"p"));
                    descriptionParagraph->SetAttribute(U"xml:space", U"preserve");
                    descriptionParagraph->SetAttribute(U"class", U"description");
                    ContentVisitor visitor(input, descriptionParagraph.get(), moduleXmlDoc, otherModuleXmlDocs, prefix);
                    descriptionElement->Accept(visitor);
                }
            }
        }
    }
    return descriptionParagraph;
}

std::unique_ptr<dom::Element> GetDetails(Input* input, const std::u32string& docId, dom::Document* docs,
    dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, const std::u32string& prefix)
{
    if (!docs) return std::unique_ptr<dom::Element>();
    std::lock_guard<std::mutex> lock(GetInputMutex());
    std::unique_ptr<dom::Element> detailParagraphs;
    dom::Element* docElement = docs->GetElementById(docId);
    if (docElement)
    {
        std::unique_ptr<xpath::XPathObject> detailsObject = xpath::Evaluate(U"details", docElement);
        if (detailsObject->Type() == xpath::XPathObjectType::nodeSet)
        {
            xpath::XPathNodeSet* detailsNodeSet = static_cast<xpath::XPathNodeSet*>(detailsObject.get());
            int n = detailsNodeSet->Length();
            if (n > 0)
            {
                detailParagraphs.reset(new dom::Element(U"span"));
                detailParagraphs->SetAttribute(U"xml:space", U"preserve");
                for (int i = 0; i < n; ++i)
                {
                    dom::Node* detailNode = (*detailsNodeSet)[i];
                    if (detailNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* detailElement = static_cast<dom::Element*>(detailNode);
                        std::u32string title = detailElement->GetAttribute(U"title");
                        if (!title.empty())
                        {
                            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
                            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
                            detailParagraphs->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
                        }
                        std::unique_ptr<dom::Element> detailParagraph(new dom::Element(U"p"));
                        detailParagraph->SetAttribute(U"xml:space", U"preserve");
                        detailParagraph->SetAttribute(U"class", U"description");
                        ContentVisitor visitor(input, detailParagraph.get(), moduleXmlDoc, otherModuleXmlDocs, prefix);
                        detailElement->Accept(visitor);
                        detailParagraphs->AppendChild(std::unique_ptr<dom::Node>(detailParagraph.release()));
                    }
                }
            }
        }
    }
    return detailParagraphs;
}

std::unique_ptr<dom::Element> GetDescriptionAndDetails(Input* input, const std::u32string& docId, dom::Document* docs,
    dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, const std::u32string& prefix)
{
    if (!docs) return std::unique_ptr<dom::Element>();
    std::unique_ptr<dom::Element> descriptionParagraph = GetDescription(input, docId, docs, moduleXmlDoc, otherModuleXmlDocs, prefix);
    if (!descriptionParagraph) return std::unique_ptr<dom::Element>();
    std::unique_ptr<dom::Element> detailParagraphs = GetDetails(input, docId, docs, moduleXmlDoc, otherModuleXmlDocs, prefix);
    if (!detailParagraphs) return descriptionParagraph;
    std::unique_ptr<dom::Element> allParagraphs(new dom::Element(U"span"));
    allParagraphs->SetAttribute(U"xml:space", U"preserve");
    allParagraphs->AppendChild(std::unique_ptr<dom::Node>(descriptionParagraph.release()));
    allParagraphs->AppendChild(std::unique_ptr<dom::Node>(detailParagraphs.release()));
    return allParagraphs;
}

void GenerateRootIndexHtml(Input* input, const std::string& targetDir, const std::u32string& solutionName, const std::vector<std::u32string>& moduleNames, 
    const std::vector<std::string>& moduleLinks, const std::vector<std::unique_ptr<dom::Document>>& moduleXmlDocs)
{
    dom::Document* moduleXmlDoc = nullptr;
    std::vector<dom::Document*> otherModuleXmlDocs;
    for (const auto& doc : moduleXmlDocs)
    {
        otherModuleXmlDocs.push_back(doc.get());
    }
    dom::Document* docs = input->docs.get();
    std::u32string title = solutionName + U" Reference";
    std::string styleFilePath = "style/style.css";
    dom::Document indexDoc;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    if (docs)
    {
        std::unique_ptr<dom::Element> descriptionAndDetails = GetDescriptionAndDetails(input, U"solution", docs, moduleXmlDoc, otherModuleXmlDocs, U"content/System/doc");
        if (descriptionAndDetails)
        {
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(descriptionAndDetails.release()));
        }
    }
    std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
    h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Modules")));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
    std::unique_ptr<dom::Element> moduleTableElement(new dom::Element(U"table"));
    int n = moduleNames.size();
    for (int i = n - 1; i >= 0; --i)
    {
        std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
        const std::u32string& moduleName = moduleNames[i];
        std::u32string moduleLink = ToUtf32(moduleLinks[i]);
        std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
        std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
        linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(moduleName)));
        linkElement->SetAttribute(U"href", moduleLink);
        tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
        std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
        if (AppendDescription(td2Element.get(), input, moduleName, docs, moduleXmlDoc, otherModuleXmlDocs, U"content/System/doc/"))
        {
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
        }
        moduleTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
    }
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(moduleTableElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    indexDoc.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    std::string indexFilePath = GetFullPath(Path::Combine(targetDir, "index.html"));
    std::ofstream indexFile(indexFilePath);
    CodeFormatter formatter(indexFile);
    formatter.SetIndentSize(1);
    indexDoc.Write(formatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "==> " << indexFilePath << std::endl;
    }
}

bool GenerateNamespaceNames(int level, dom::Element* namespaceTableElement, dom::Element* namespaceParentElement, const std::u32string& prefix,
    std::vector<std::unique_ptr<xpath::XPathObject>>& namespaceXPathObjects, std::vector<dom::Element*>& nsElements)
{
    bool retval = false;
    std::unique_ptr<xpath::XPathObject> namespaceXPathObject = xpath::Evaluate(U"namespaces/namespace", namespaceParentElement);
    if (namespaceXPathObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* namespacesNodeSet = static_cast<xpath::XPathNodeSet*>(namespaceXPathObject.get());
        int n = namespacesNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* nsNode = (*namespacesNodeSet)[i];
            if (nsNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* nsElement = static_cast<dom::Element*>(nsNode);
                nsElements.push_back(nsElement);
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                for (int l = 0; l < level; ++l)
                {
                    std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                    trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                }
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                std::u32string nsName = nsElement->GetAttribute(U"name");
                if (nsName.empty())
                {
                    nsName = U"Global Namespace";
                }
                std::u32string nsId = nsElement->GetAttribute(U"id");
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                linkElement->SetAttribute(U"href", prefix + nsId + U".html");
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(nsName)));
                tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                namespaceTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                retval = true;
                GenerateNamespaceNames(level + 1, namespaceTableElement, nsElement, prefix, namespaceXPathObjects, nsElements);
            }
        }
    }
    namespaceXPathObjects.push_back(std::move(namespaceXPathObject));
    return retval;
}

std::u32string MakeFullNamespaceName(dom::Element* nsElement)
{
    std::u32string fullNamespaceName;
    if (nsElement)
    {
        if (nsElement->Name() == U"namespaces")
        {
            dom::Node* parent = nsElement->Parent();
            if (parent && parent->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* parentElement = static_cast<dom::Element*>(parent);
                return MakeFullNamespaceName(parentElement);
            }
        }
        else if (nsElement->Name() == U"namespace")
        {
            std::u32string namespaceName = nsElement->GetAttribute(U"name");
            if (!namespaceName.empty())
            {
                dom::Node* parent = nsElement->Parent();
                if (parent && parent->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* parentElement = static_cast<dom::Element*>(parent);
                    fullNamespaceName = MakeFullNamespaceName(parentElement);
                    if (!fullNamespaceName.empty())
                    {
                        fullNamespaceName.append(1, '.');
                    }
                }
                fullNamespaceName.append(namespaceName);
            }
        }
    }
    return fullNamespaceName;
}

std::u32string MakeFullClassName(dom::Element* classElement)
{
    std::u32string fullClassName;
    if (classElement)
    {
        if (classElement->Name() == U"namespaces" || classElement->Name() == U"classes")
        {
            dom::Node* parent = classElement->Parent();
            if (parent && parent->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* parentElement = static_cast<dom::Element*>(parent);
                return MakeFullClassName(parentElement);
            }
        }
        else if (classElement->Name() == U"namespace" || classElement->Name() == U"class")
        {
            std::u32string name = classElement->GetAttribute(U"name");
            if (!name.empty())
            {
                dom::Node* parent = classElement->Parent();
                if (parent && parent->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* parentElement = static_cast<dom::Element*>(parent);
                    fullClassName = MakeFullClassName(parentElement);
                    if (!fullClassName.empty())
                    {
                        fullClassName.append(1, '.');
                    }
                }
                fullClassName.append(name);
                if (classElement->Name() == U"class")
                {
                    std::unique_ptr<xpath::XPathObject> templateParametersObject = xpath::Evaluate(U"templateParameters/templateParameter", classElement);
                    if (templateParametersObject->Type() == xpath::XPathObjectType::nodeSet)
                    {
                        xpath::XPathNodeSet* templateParameterNodeSet = static_cast<xpath::XPathNodeSet*>(templateParametersObject.get());
                        int n = templateParameterNodeSet->Length();
                        if (n > 0)
                        {
                            fullClassName.append(1, '<');
                            for (int i = 0; i < n; ++i)
                            {
                                if (i > 0)
                                {
                                    fullClassName.append(U", ");
                                }
                                dom::Node* templateParameterNode = (*templateParameterNodeSet)[i];
                                if (templateParameterNode->GetNodeType() == dom::NodeType::elementNode)
                                {
                                    dom::Element* templateParameterElement = static_cast<dom::Element*>(templateParameterNode);
                                    fullClassName.append(templateParameterElement->GetAttribute(U"name"));
                                }
                            }
                            fullClassName.append(1, '>');
                        }
                    }
                }
            }
        }
    }
    return fullClassName;
}

std::u32string MakeFullConceptName(dom::Element* conceptElement)
{
    std::u32string fullConceptName;
    if (conceptElement)
    {
        if (conceptElement->Name() == U"namespaces" || conceptElement->Name() == U"concepts")
        {
            dom::Node* parent = conceptElement->Parent();
            if (parent && parent->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* parentElement = static_cast<dom::Element*>(parent);
                return MakeFullConceptName(parentElement);
            }
        }
        else if (conceptElement->Name() == U"namespace" || conceptElement->Name() == U"concept")
        {
            std::u32string name = conceptElement->GetAttribute(U"name");
            if (!name.empty())
            {
                dom::Node* parent = conceptElement->Parent();
                if (parent && parent->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* parentElement = static_cast<dom::Element*>(parent);
                    fullConceptName = MakeFullConceptName(parentElement);
                    if (!fullConceptName.empty())
                    {
                        fullConceptName.append(1, '.');
                    }
                }
                fullConceptName.append(name);
                if (conceptElement->Name() == U"concept")
                {
                    std::unique_ptr<xpath::XPathObject> typeParametersObject = xpath::Evaluate(U"typeParameters/typeParameter", conceptElement);
                    if (typeParametersObject->Type() == xpath::XPathObjectType::nodeSet)
                    {
                        xpath::XPathNodeSet* typeParameterNodeSet = static_cast<xpath::XPathNodeSet*>(typeParametersObject.get());
                        int n = typeParameterNodeSet->Length();
                        if (n > 0)
                        {
                            fullConceptName.append(1, '<');
                            for (int i = 0; i < n; ++i)
                            {
                                if (i > 0)
                                {
                                    fullConceptName.append(U", ");
                                }
                                dom::Node* typeParameterNode = (*typeParameterNodeSet)[i];
                                if (typeParameterNode->GetNodeType() == dom::NodeType::elementNode)
                                {
                                    dom::Element* typeParameterElement = static_cast<dom::Element*>(typeParameterNode);
                                    fullConceptName.append(typeParameterElement->GetAttribute(U"name"));
                                }
                            }
                            fullConceptName.append(1, '>');
                        }
                    }
                }
            }
        }
    }
    return fullConceptName;
}

void GetConstructorsAndDestructors(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& ctorDtorObject, std::vector<dom::Element*>& ctorDtorElements)
{
    ctorDtorObject = xpath::Evaluate(U"ctorsDtors/ctorDtor", parentElement);
    if (ctorDtorObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* ctorDtorNodeSet = static_cast<xpath::XPathNodeSet*>(ctorDtorObject.get());
        int n = ctorDtorNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* ctorDtorNode = (*ctorDtorNodeSet)[i];
            if (ctorDtorNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* ctorDtorElement = static_cast<dom::Element*>(ctorDtorNode);
                ctorDtorElements.push_back(ctorDtorElement);
            }
        }
    }
}

void GetFunctions(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& functionObject, std::vector<dom::Element*>& functionElements)
{
    functionObject = xpath::Evaluate(U"functions/function", parentElement);
    if (functionObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* functionNodeSet = static_cast<xpath::XPathNodeSet*>(functionObject.get());
        int n = functionNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* functionNode = (*functionNodeSet)[i];
            if (functionNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* functionElement = static_cast<dom::Element*>(functionNode);
                functionElements.push_back(functionElement);
            }
        }
    }
}

void GetClasses(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& classObject, std::vector<dom::Element*>& classElements)
{
    classObject = xpath::Evaluate(U"classes/class", parentElement);
    if (classObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* classNodeSet = static_cast<xpath::XPathNodeSet*>(classObject.get());
        int n = classNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* classNode = (*classNodeSet)[i];
            if (classNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* classElement = static_cast<dom::Element*>(classNode);
                classElements.push_back(classElement);
            }
        }
    }
}

struct BaseClassInfo
{
    dom::Element* baseClassElement;
    std::u32string baseClassSpecializationName;
    std::string libraryPrefix;
};

BaseClassInfo GetBaseClass(Input* input, dom::Element* classElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    BaseClassInfo baseClassInfo;
    baseClassInfo.baseClassElement = nullptr;
    std::u32string baseClassId = classElement->GetAttribute(U"baseClass");
    baseClassInfo.baseClassSpecializationName = classElement->GetAttribute(U"baseClassSpecializationName");
    if (!baseClassId.empty())
    {
        baseClassInfo.baseClassElement = moduleXmlDoc->GetElementById(baseClassId);
        if (!baseClassInfo.baseClassElement)
        {
            std::lock_guard<std::mutex> lock(GetInputMutex());
            int n = otherModuleXmlDocs.size();
            for (int i = 0; i < n; ++i)
            {
                dom::Document* otherModuleXmlDoc = otherModuleXmlDocs[i];
                std::u32string moduleName = otherModuleXmlDoc->DocumentElement()->GetAttribute(U"module");
                auto it = input->libraryPrefixMap.find(moduleName);
                if (it != input->libraryPrefixMap.cend())
                {
                    baseClassInfo.libraryPrefix = it->second;
                    if (!baseClassInfo.libraryPrefix.empty())
                    {
                        baseClassInfo.libraryPrefix = Path::Combine("../../..", baseClassInfo.libraryPrefix);
                    }
                }
                else
                {
                    throw std::runtime_error("module '" + ToUtf8(moduleName) + "' not found in document generator");
                }
                baseClassInfo.baseClassElement = otherModuleXmlDoc->GetElementById(baseClassId);
                if (baseClassInfo.baseClassElement) break;
            }
        }
        if (!baseClassInfo.baseClassElement)
        {
            throw std::runtime_error("base class id '" + ToUtf8(baseClassId) + "' not found");
        }
    }
    return baseClassInfo;
}

dom::Element* GetRefinedConcept(dom::Element* conceptElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    dom::Element* refinedConceptElement = nullptr;
    std::u32string refinedConceptId = conceptElement->GetAttribute(U"refines");
    if (!refinedConceptId.empty())
    {
        refinedConceptElement = moduleXmlDoc->GetElementById(refinedConceptId);
        if (!refinedConceptElement)
        {
            std::lock_guard<std::mutex> lock(GetInputMutex());
            int n = otherModuleXmlDocs.size();
            for (int i = 0; i < n; ++i)
            {
                dom::Document* otherModuleXmlDoc = otherModuleXmlDocs[i];
                refinedConceptElement = otherModuleXmlDoc->GetElementById(refinedConceptId);
                if (refinedConceptElement) break;
            }
        }
        if (!refinedConceptElement)
        {
            throw std::runtime_error("refined concept id '" + ToUtf8(refinedConceptId) + "' not found");
        }
    }
    return refinedConceptElement;
}

std::vector<dom::Element*> GetDerivedClasses(dom::Element* classElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    std::vector<dom::Element*> derivedClassElements;
    std::u32string moduleName = moduleXmlDoc->DocumentElement()->GetAttribute(U"module");
    std::vector<std::u32string> derivedClassIds = GetDerivedClassIds(moduleName, classElement->GetAttribute(U"id"));
    for (const std::u32string& derivedClassId : derivedClassIds)
    {
        dom::Element* derivedClassElement = nullptr;
        std::lock_guard<std::mutex> lock(GetInputMutex());
        derivedClassElement = moduleXmlDoc->GetElementById(derivedClassId);
        if (!derivedClassElement)
        {
            int n = otherModuleXmlDocs.size();
            for (int i = 0; i < n; ++i)
            {
                dom::Document* otherModuleXmlDoc = otherModuleXmlDocs[i];
                derivedClassElement = otherModuleXmlDoc->GetElementById(derivedClassId);
                if (derivedClassElement) break;
            }
        }
        if (!derivedClassElement)
        {
            throw std::runtime_error("derived class id '" + ToUtf8(derivedClassId) + "' not found");
        }
        derivedClassElements.push_back(derivedClassElement);
    }
    return derivedClassElements;
}

std::vector<dom::Element*> GetDerivedConcepts(dom::Element* conceptElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    std::vector<dom::Element*> derivedConceptElements;
    std::u32string moduleName = moduleXmlDoc->DocumentElement()->GetAttribute(U"module");
    std::vector<std::u32string> derivedConceptIds = GetDerivedConceptIds(moduleName, conceptElement->GetAttribute(U"id"));
    for (const std::u32string& derivedConceptId : derivedConceptIds)
    {
        dom::Element* derivedConceptElement = nullptr;
        std::lock_guard<std::mutex> lock(GetInputMutex());
        derivedConceptElement = moduleXmlDoc->GetElementById(derivedConceptId);
        if (!derivedConceptElement)
        {
            int n = otherModuleXmlDocs.size();
            for (int i = 0; i < n; ++i)
            {
                dom::Document* otherModuleXmlDoc = otherModuleXmlDocs[i];
                derivedConceptElement = otherModuleXmlDoc->GetElementById(derivedConceptId);
                if (derivedConceptElement) break;
            }
        }
        if (!derivedConceptElement)
        {
            throw std::runtime_error("derived concept id '" + ToUtf8(derivedConceptId) + "' not found");
        }
        derivedConceptElements.push_back(derivedConceptElement);
    }
    return derivedConceptElements;
}

void GetTypeParameters(dom::Element* conceptElement, std::unique_ptr<xpath::XPathObject>& typeParametersObject, std::vector<dom::Element*>& typeParameterElements)
{
    typeParametersObject = xpath::Evaluate(U"typeParameters/typeParameter", conceptElement);
    if (typeParametersObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* typeParameterNodeSet = static_cast<xpath::XPathNodeSet*>(typeParametersObject.get());
        int n = typeParameterNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* typeParameterNode = (*typeParameterNodeSet)[i];
            if (typeParameterNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* typeParameterElement = static_cast<dom::Element*>(typeParameterNode);
                typeParameterElements.push_back(typeParameterElement);
            }
        }
    }
}

std::u32string MakeFunctionName(dom::Element* functionElement, std::unique_ptr<xpath::XPathObject>& templateParametersObject, 
    std::vector<dom::Element*>& templateParameterElements)
{
    std::u32string functionName = functionElement->GetAttribute(U"name");
    templateParametersObject = xpath::Evaluate(U"templateParameters/templateParameter", functionElement);
    if (templateParametersObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* templateParameterNodeSet = static_cast<xpath::XPathNodeSet*>(templateParametersObject.get());
        int n = templateParameterNodeSet->Length();
        if (n > 0)
        {
            functionName.append(1, '<');
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    functionName.append(U", ");
                }
                dom::Node* templateParameterNode = (*templateParameterNodeSet)[i];
                if (templateParameterNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* templateParameterElement = static_cast<dom::Element*>(templateParameterNode);
                    templateParameterElements.push_back(templateParameterElement);
                    functionName.append(templateParameterElement->GetAttribute(U"name"));
                }
            }
            functionName.append(1, '>');
        }
    }
    return functionName;
}

std::u32string MakeClassName(dom::Element* classElement, std::unique_ptr<xpath::XPathObject>& templateParametersObject, std::vector<dom::Element*>& templateParameterElements)
{
    std::u32string className = classElement->GetAttribute(U"name");
    templateParametersObject = xpath::Evaluate(U"templateParameters/templateParameter", classElement);
    if (templateParametersObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* templateParameterNodeSet = static_cast<xpath::XPathNodeSet*>(templateParametersObject.get());
        int n = templateParameterNodeSet->Length();
        if (n > 0)
        {
            className.append(1, '<');
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    className.append(U", ");
                }
                dom::Node* templateParameterNode = (*templateParameterNodeSet)[i];
                if (templateParameterNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* templateParameterElement = static_cast<dom::Element*>(templateParameterNode);
                    templateParameterElements.push_back(templateParameterElement);
                    className.append(templateParameterElement->GetAttribute(U"name"));
                }
            }
            className.append(1, '>');
        }
    }
    return className;
}

void AppendSpecifiers(dom::Element* parentElement, const std::u32string& specifiers, const std::u32string& classifier, bool space)
{
    std::vector<std::u32string> components = Split(specifiers, ' ');
    int n = components.size();
    if (n > 0)
    {
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
            }
            const std::u32string& specifier = components[i];
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->SetAttribute(U"class", U"kw");
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(specifier)));
            parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        }
        if (space || !classifier.empty())
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        }
    }
    if (!classifier.empty())
    {
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"kw");
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(classifier)));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        if (space)
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        }
    }
}

void AppendType(Input* input, dom::Element* parentElement, const std::u32string& typeId, const std::u32string& typeName,
    dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, const std::u32string& prefix)
{
    std::string libraryPrefix;
    dom::Element* typeElement = moduleXmlDoc->GetElementById(typeId);
    if (!typeElement)
    {
        std::lock_guard<std::mutex> lock(GetInputMutex());
        int n = otherModuleXmlDocs.size();
        for (int i = 0; i < n; ++i)
        {
            dom::Document* otherModuleXmlDoc = otherModuleXmlDocs[i];
            typeElement = otherModuleXmlDoc->GetElementById(typeId);
            if (typeElement)
            {
                std::u32string moduleName = otherModuleXmlDoc->DocumentElement()->GetAttribute(U"module");
                auto it = input->libraryPrefixMap.find(moduleName);
                if (it != input->libraryPrefixMap.cend())
                {
                    libraryPrefix = it->second;
                    if (!libraryPrefix.empty())
                    {
                        libraryPrefix = Path::Combine("../../..", libraryPrefix);
                    }
                }
                else
                {
                    throw std::runtime_error("module '" + ToUtf8(moduleName) + "' not found in document generator");
                }
                break;
            }
        }
    }
    if (typeElement)
    {
        if (typeElement->Name() == U"basicType")
        {
            std::u32string typeName = typeElement->GetAttribute(U"name");
            if (typeName.empty())
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"BASIC_TYPE")));
            }
            else
            {
                std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                span->SetAttribute(U"class", U"kw");
                span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeName)));
                parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
            }
        }
        else if (typeElement->Name() == U"arrayType")
        {
            std::u32string elementType = typeElement->GetAttribute(U"elementType");
            if (elementType.empty())
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"ELEMENT_TYPE")));
            }
            else
            {
                AppendType(input, parentElement, elementType, U"ELEMENT_TYPE", moduleXmlDoc, otherModuleXmlDocs, prefix);
            }
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"[")));
            std::u32string size = typeElement->GetAttribute(U"size");
            if (!size.empty())
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(size)));
            }
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"]")));
        }
        else if (typeElement->Name() == U"derivedType")
        {
            std::unique_ptr<xpath::XPathObject> derivations = xpath::Evaluate(U"derivation", typeElement);
            if (derivations->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* derivationSet = static_cast<xpath::XPathNodeSet*>(derivations.get());
                int n = derivationSet->Length();
                if (n > 0)
                {
                    for (int i = 0; i < n; ++i)
                    {
                        dom::Node* derivationNode = (*derivationSet)[i];
                        if (derivationNode->GetNodeType() == dom::NodeType::elementNode)
                        {
                            dom::Element* derivationElement = static_cast<dom::Element*>(derivationNode);
                            std::u32string baseType = derivationElement->GetAttribute(U"baseType");
                            if (!baseType.empty())
                            {
                                AppendType(input, parentElement, baseType, U"BASE_TYPE", moduleXmlDoc, otherModuleXmlDocs, prefix);
                            }
                            else
                            {
                                std::u32string derivationName = derivationElement->GetAttribute(U"name");
                                if (derivationName.empty())
                                {
                                    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"DERIVATION")));
                                }
                                else if (derivationName == U"const")
                                {
                                    std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                                    span->SetAttribute(U"class", U"kw");
                                    span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(derivationName)));
                                    parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
                                    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                                }
                                else
                                {
                                    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(derivationName)));
                                }
                            }
                        }
                    }
                }
                else
                {
                    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeElement->GetAttribute(U"name"))));
                }
            }
        }
        else if (typeElement->Name() == U"specialization")
        {
            std::u32string primaryType = typeElement->GetAttribute(U"primaryType");
            AppendType(input, parentElement, primaryType, U"PRIMARY_TYPE", moduleXmlDoc, otherModuleXmlDocs, prefix);
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"<")));
            std::unique_ptr<xpath::XPathObject> templateArgumentTypes = xpath::Evaluate(U"templateArgumentTypes/templateArgumentType", typeElement);
            if (templateArgumentTypes->Type() == xpath::XPathObjectType::nodeSet)
            {
                xpath::XPathNodeSet* templateArgumentTypeNodeSet = static_cast<xpath::XPathNodeSet*>(templateArgumentTypes.get());
                int n = templateArgumentTypeNodeSet->Length();
                for (int i = 0; i < n; ++i)
                {
                    if (i > 0)
                    {
                        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
                    }
                    dom::Node* templateArgumentTypeNode = (*templateArgumentTypeNodeSet)[i];
                    if (templateArgumentTypeNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* templateArgumentTypeElement = static_cast<dom::Element*>(templateArgumentTypeNode);
                        std::u32string templateArgumentType = templateArgumentTypeElement->GetAttribute(U"ref");
                        if (templateArgumentType.empty())
                        {
                            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"TEMPLATE_ARGUMENT_TYPE")));
                        }
                        else
                        {
                            AppendType(input, parentElement, templateArgumentType, U"TEMPLATE_ARGUMENT_TYPE", moduleXmlDoc, otherModuleXmlDocs, prefix);
                        }
                    }
                }
            }
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U">")));
        }
        else if (typeElement->Name() == U"class" || typeElement->Name() == U"delegate" || typeElement->Name() == U"classDelegate" || typeElement->Name() == U"enumType" ||
            typeElement->Name() == U"interface" || typeElement->Name() == U"templateParameter")
        {
            std::u32string link;
            if (libraryPrefix.empty())
            {
                link.append(prefix).append(typeElement->GetAttribute(U"docPath"));
            }
            else
            {
                link.append(prefix).append(ToUtf32(Path::Combine(libraryPrefix, ToUtf8(typeElement->GetAttribute(U"extPath")))));
            }
            std::u32string name = typeElement->GetAttribute(U"name");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", link);
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(name)));
            parentElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"TYPE")));
        }
    }
    else
    {
        if (typeName.empty())
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"TYPE")));
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeName)));
        }
    }
}

void AppendParameters(Input* input, dom::Element* parentElement, dom::Element* functionElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs,
    const std::u32string& prefix)
{
    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(")));
    std::unique_ptr<xpath::XPathObject> parameters = xpath::Evaluate(U"parameters/parameter", functionElement);
    if (parameters->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* parameterNodeSet = static_cast<xpath::XPathNodeSet*>(parameters.get());
        int n = parameterNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
            }
            dom::Node* parameterNode = (*parameterNodeSet)[i];
            if (parameterNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* parameterElement = static_cast<dom::Element*>(parameterNode);
                std::u32string typeId = parameterElement->GetAttribute(U"type");
                AppendType(input, parentElement, typeId, U"TYPE", moduleXmlDoc, otherModuleXmlDocs, prefix);
                std::u32string name = parameterElement->GetAttribute(U"name");
                if (!name.empty())
                {
                    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(name)));
                }
            }
        }
    }
    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U")")));
}

void GetDelegates(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& delegateObject, std::vector<dom::Element*>& delegateElements)
{
    delegateObject = xpath::Evaluate(U"delegates/delegate", parentElement);
    if (delegateObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* delegateNodeSet = static_cast<xpath::XPathNodeSet*>(delegateObject.get());
        int n = delegateNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* delegateNode = (*delegateNodeSet)[i];
            if (delegateNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* delegateElement = static_cast<dom::Element*>(delegateNode);
                delegateElements.push_back(delegateElement);
            }
        }
    }
}

void GetClassDelegates(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& classDelegateObject, std::vector<dom::Element*>& classDelegateElements)
{
    classDelegateObject = xpath::Evaluate(U"classDelegates/classDelegate", parentElement);
    if (classDelegateObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* classDelegateNodeSet = static_cast<xpath::XPathNodeSet*>(classDelegateObject.get());
        int n = classDelegateNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* classDelegateNode = (*classDelegateNodeSet)[i];
            if (classDelegateNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* classDelegateElement = static_cast<dom::Element*>(classDelegateNode);
                classDelegateElements.push_back(classDelegateElement);
            }
        }
    }
}

void GetTypedefs(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& typedefObject, std::vector<dom::Element*>& typedefElements)
{
    typedefObject = xpath::Evaluate(U"typedefs/typedef", parentElement);
    if (typedefObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* typedefNodeSet = static_cast<xpath::XPathNodeSet*>(typedefObject.get());
        int n = typedefNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* typedefNode = (*typedefNodeSet)[i];
            if (typedefNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* typedefElement = static_cast<dom::Element*>(typedefNode);
                typedefElements.push_back(typedefElement);
            }
        }
    }
}

void GetEnumTypes(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& enumTypesObject, std::vector<dom::Element*>& enumTypeElements)
{
    enumTypesObject = xpath::Evaluate(U"enumTypes/enumType", parentElement);
    if (enumTypesObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* enumTypesNodeSet = static_cast<xpath::XPathNodeSet*>(enumTypesObject.get());
        int n = enumTypesNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* enumTypeNode = (*enumTypesNodeSet)[i];
            if (enumTypeNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* enumTypeElement = static_cast<dom::Element*>(enumTypeNode);
                enumTypeElements.push_back(enumTypeElement);
            }
        }
    }
}

void GetEnumConstants(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& enumConstantsObject, std::vector<dom::Element*>& enumConstantElements)
{
    enumConstantsObject = xpath::Evaluate(U"enumConstants/enumConstant", parentElement);
    if (enumConstantsObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* enumConstantsNodeSet = static_cast<xpath::XPathNodeSet*>(enumConstantsObject.get());
        int n = enumConstantsNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* enumConstantNode = (*enumConstantsNodeSet)[i];
            if (enumConstantNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* enumConstantElement = static_cast<dom::Element*>(enumConstantNode);
                enumConstantElements.push_back(enumConstantElement);
            }
        }
    }
}

void GetConstants(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& constantsObject, std::vector<dom::Element*>& constantElements)
{
    constantsObject = xpath::Evaluate(U"constants/constant", parentElement);
    if (constantsObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* constantNodeSet = static_cast<xpath::XPathNodeSet*>(constantsObject.get());
        int n = constantNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* constantNode = (*constantNodeSet)[i];
            if (constantNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* constantElement = static_cast<dom::Element*>(constantNode);
                constantElements.push_back(constantElement);
            }
        }
    }
}

void GetVariables(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& variableObject, std::vector<dom::Element*>& variableElements)
{
    variableObject = xpath::Evaluate(U"variables/variable", parentElement);
    if (variableObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* variableNodeSet = static_cast<xpath::XPathNodeSet*>(variableObject.get());
        int n = variableNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* variableNode = (*variableNodeSet)[i];
            if (variableNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* variableElement = static_cast<dom::Element*>(variableNode);
                variableElements.push_back(variableElement);
            }
        }
    }
}

dom::Element* GetConstraint(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& constraintObject)
{
    constraintObject = xpath::Evaluate(U"where", parentElement);
    if (constraintObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* constraintNodeSet = static_cast<xpath::XPathNodeSet*>(constraintObject.get());
        if (constraintNodeSet->Length() == 1)
        {
            dom::Node* constraintNode = (*constraintNodeSet)[0];
            if (constraintNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* constraintElement = static_cast<dom::Element*>(constraintNode);
                return constraintElement;
            }
        }
    }
    return nullptr;
}

void GetConstraints(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& constraintsObject, std::vector<dom::Element*>& constraintElements)
{
    constraintsObject = xpath::Evaluate(U"constraints/*", parentElement);
    if (constraintsObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* constraintNodeSet = static_cast<xpath::XPathNodeSet*>(constraintsObject.get());
        int n = constraintNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* constraintNode = (*constraintNodeSet)[i];
            if (constraintNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* constraintElement = static_cast<dom::Element*>(constraintNode);
                constraintElements.push_back(constraintElement);
            }
        }
    }
}

void GetAxioms(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& axiomsObject, std::vector<dom::Element*>& axiomElements)
{
    axiomsObject = xpath::Evaluate(U"axioms/axiom", parentElement);
    if (axiomsObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* axiomNodeSet = static_cast<xpath::XPathNodeSet*>(axiomsObject.get());
        int n = axiomNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* axiomNode = (*axiomNodeSet)[i];
            if (axiomNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* axiomElement = static_cast<dom::Element*>(axiomNode);
                axiomElements.push_back(axiomElement);
            }
        }
    }
}

void GetAxiomStatements(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& axiomStatementsObject, std::vector<dom::Element*>& axiomStatementElements)
{
    axiomStatementsObject = xpath::Evaluate(U"axiomStatements/axiomStatement", parentElement);
    if (axiomStatementsObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* axiomStatementNodeSet = static_cast<xpath::XPathNodeSet*>(axiomStatementsObject.get());
        int n = axiomStatementNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* axiomStatementNode = (*axiomStatementNodeSet)[i];
            if (axiomStatementNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* axiomStatementElement = static_cast<dom::Element*>(axiomStatementNode);
                axiomStatementElements.push_back(axiomStatementElement);
            }
        }
    }
}

void AppendSymbol(Input* input, dom::Element* parentElement, dom::Element* symbolElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs,
    const std::u32string& prefix)
{
    std::string libraryPrefix;
    std::u32string elementName = symbolElement->Name();
    if (elementName == U"symbol")
    {
        std::u32string ref = symbolElement->GetAttribute(U"ref");
        std::u32string link;
        dom::Element* typeOrConceptElement = nullptr;
        if (!ref.empty())
        {
            typeOrConceptElement = moduleXmlDoc->GetElementById(ref);
            if (!typeOrConceptElement)
            {
                std::lock_guard<std::mutex> lock(GetInputMutex());
                int n = otherModuleXmlDocs.size();
                for (int i = 0; i < n; ++i)
                {
                    dom::Document* otherModuleXmlDoc = otherModuleXmlDocs[i];
                    typeOrConceptElement = otherModuleXmlDoc->GetElementById(ref);
                    if (typeOrConceptElement)
                    {
                        std::u32string moduleName = otherModuleXmlDoc->DocumentElement()->GetAttribute(U"module");
                        auto it = input->libraryPrefixMap.find(moduleName);
                        if (it != input->libraryPrefixMap.cend())
                        {
                            libraryPrefix = it->second;
                            if (!libraryPrefix.empty())
                            {
                                libraryPrefix = Path::Combine("../../..", libraryPrefix);
                            }
                        }
                        else
                        {
                            throw std::runtime_error("module '" + ToUtf8(moduleName) + "' not found in document generator");
                        }
                        break;
                    }
                }
            }
        }
        if (typeOrConceptElement)
        {
            if (libraryPrefix.empty())
            {
                link.append(prefix).append(typeOrConceptElement->GetAttribute(U"docPath"));
            }
            else
            {
                link.append(prefix).append(ToUtf32(Path::Combine(libraryPrefix, ToUtf8(typeOrConceptElement->GetAttribute(U"extPath")))));
            }
            std::u32string name = typeOrConceptElement->GetAttribute(U"name");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", link);
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(name)));
            parentElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(symbolElement->GetAttribute(U"name"))));
        }
    }
    else if (!elementName.empty())
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(elementName)));
    }
    else
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"SYMBOL_ELEMENT")));
    }
}

void AppendAxiomParameters(Input* input, dom::Element* parentElement, dom::Element* axiomElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs,
    const std::u32string& prefix)
{
    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(")));
    std::unique_ptr<xpath::XPathObject> parameters = xpath::Evaluate(U"parameters/parameter", axiomElement);
    if (parameters->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* parameterNodeSet = static_cast<xpath::XPathNodeSet*>(parameters.get());
        int n = parameterNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
            }
            dom::Node* parameterNode = (*parameterNodeSet)[i];
            if (parameterNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* parameterElement = static_cast<dom::Element*>(parameterNode);
                dom::Node* firstChildNode = parameterElement->FirstChild();
                if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* symbolElement = static_cast<dom::Element*>(firstChildNode);
                    AppendSymbol(input, parentElement, symbolElement, moduleXmlDoc, otherModuleXmlDocs, prefix);
                    dom::Node* secondChildNode = firstChildNode->NextSibling();
                    if (secondChildNode && secondChildNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        dom::Element* symbolElement = static_cast<dom::Element*>(secondChildNode);
                        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                        AppendSymbol(input, parentElement, symbolElement, moduleXmlDoc, otherModuleXmlDocs, prefix);
                    }
                }
                
            }
        }
    }
    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U")")));
}

void AppendConstraint(Input* input, dom::Element* parentElement, dom::Element* element, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, 
    const std::u32string& prefix, bool wherePrefix)
{
    std::u32string elementName = element->Name();
    if (elementName == U"where")
    {
        if (wherePrefix)
        {
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->SetAttribute(U"class", U"kw");
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"where")));
            parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        }
        dom::Node* node = element->FirstChild();
        if (node && node->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* subElement = static_cast<dom::Element*>(node);
            AppendConstraint(input, parentElement, subElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"WHERE_CHILD")));
        }
    }
    else if (elementName == U"is")
    {
        dom::Node* firstChildNode = element->FirstChild();
        if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* typeElement = static_cast<dom::Element*>(firstChildNode);
            AppendConstraint(input, parentElement, typeElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"IS_LEFT_CHILD")));
        }
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"kw");
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"is")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        dom::Node* secondChildNode = firstChildNode->NextSibling();
        if (secondChildNode && secondChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* typeOrConceptElement = static_cast<dom::Element*>(secondChildNode);
            AppendConstraint(input, parentElement, typeOrConceptElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"IS_RIGHT_CHILD")));
        }
    }
    else if (elementName == U"multi")
    {
        dom::Node* firstChildNode = element->FirstChild();
        if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* conceptElement = static_cast<dom::Element*>(firstChildNode);
            AppendConstraint(input, parentElement, conceptElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"MULTI_CHILD")));
        }
        dom::Node* secondChildNode = firstChildNode->NextSibling();
        if (secondChildNode && secondChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* typesElement = static_cast<dom::Element*>(secondChildNode);
            if (typesElement->Name() == U"types")
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"<")));
                dom::Node* typeChildNode = typesElement->FirstChild();
                bool first = true;
                while (typeChildNode)
                {
                    if (typeChildNode->GetNodeType() == dom::NodeType::elementNode)
                    {
                        if (first)
                        {
                            first = false;
                        }
                        else
                        {
                            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U", ")));
                        }
                        dom::Element* typeElement = static_cast<dom::Element*>(typeChildNode);
                        AppendConstraint(input, parentElement, typeElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
                    }
                    typeChildNode = typeChildNode->NextSibling();
                }
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U">")));
            }
            else
            {
                parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"MULTI_TYPES")));
            }
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"MULTI_TYPES"))); 
        }
    }
    else if (elementName == U"and")
    {
        dom::Node* firstChildNode = element->FirstChild();
        if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(firstChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"AND_LEFT_CHILD")));
        }
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"kw");
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"and")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        dom::Node* secondChildNode = firstChildNode->NextSibling();
        if (secondChildNode && secondChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(secondChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"AND_RIGHT_CHILD")));
        }
    }
    else if (elementName == U"or")
    {
        dom::Node* firstChildNode = element->FirstChild();
        if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(firstChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"OR_LEFT_CHILD")));
        }
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"kw");
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"or")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        dom::Node* secondChildNode = firstChildNode->NextSibling();
        if (secondChildNode && secondChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(secondChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"OR_RIGHT_CHILD")));
        }
    }
    else if (elementName == U"dot")
    {
        dom::Node* firstChildNode = element->FirstChild();
        if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(firstChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"DOT_LEFT_CHILD")));
        }
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U".")));
        dom::Node* secondChildNode = firstChildNode->NextSibling();
        if (secondChildNode && secondChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(secondChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"DOT_RIGHT_CHILD")));
        }
    }
    else if (elementName == U"symbol")
    {
        AppendSymbol(input, parentElement, element, moduleXmlDoc, otherModuleXmlDocs, prefix);
    }
    else if (elementName == U"typename")
    {
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"kw");
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"typename")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        dom::Node* firstChildNode = element->FirstChild();
        if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(firstChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"TYPENAME_CHILD")));
        }
    }
    else if (elementName == U"keyword")
    {
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"kw");
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(element->GetAttribute(U"value"))));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    else if (elementName == U"signature")
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(element->GetAttribute(U"value"))));
    }
    else if (elementName == U"predicate")
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(element->GetAttribute(U"value"))));
    }
    else if (elementName == U"parens")
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"(")));
        dom::Node* firstChildNode = element->FirstChild();
        if (firstChildNode && firstChildNode->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* costraintElement = static_cast<dom::Element*>(firstChildNode);
            AppendConstraint(input, parentElement, costraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, wherePrefix);
        }
        else
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"PARENS_CHILD")));
        }
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U")")));
    }
    else if (!elementName.empty())
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(elementName)));
    }
    else
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"CONSTRAINT_ELEMENT")));
    }
}

void GenerateFunctionDoc(Input* input, dom::Element* parentElement, dom::Element* functionElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs,
    const std::u32string& prefix)
{
    dom::Document* docs = input->docs.get();
    std::unique_ptr<dom::Element> h3Element(new dom::Element(U"h3"));
    h3Element->SetAttribute(U"xml:space", U"preserve");
    h3Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(functionElement->GetAttribute(U"name"))));
    h3Element->SetAttribute(U"id", functionElement->GetAttribute(U"id"));
    h3Element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
    h3Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(functionElement->GetAttribute(U"kind"))));
    parentElement->AppendChild(std::unique_ptr<dom::Node>(h3Element.release()));
    std::unique_ptr<dom::Element> h4Element(new dom::Element(U"h4"));
    h4Element->SetAttribute(U"xml:space", U"preserve");
    AppendSpecifiers(h4Element.get(), functionElement->GetAttribute(U"specifiers"), std::u32string(), true);
    std::u32string returnTypeId = functionElement->GetAttribute(U"returnType");
    if (!returnTypeId.empty())
    {
        AppendType(input, h4Element.get(), returnTypeId, U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, prefix);
        h4Element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
    }
    std::unique_ptr<xpath::XPathObject> templateParametersObject;
    std::vector<dom::Element*> templateParameterElements;
    h4Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeFunctionName(functionElement, templateParametersObject, templateParameterElements))));
    AppendParameters(input, h4Element.get(), functionElement, moduleXmlDoc, otherModuleXmlDocs, prefix);
    parentElement->AppendChild(std::unique_ptr<dom::Node>(h4Element.release()));
    if (docs)
    {
        std::unique_ptr<dom::Element> descriptionAndDetails = 
            GetDescriptionAndDetails(input, functionElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, prefix);
        if (descriptionAndDetails)
        {
            parentElement->AppendChild(std::unique_ptr<dom::Node>(descriptionAndDetails.release()));
        }
    }
    bool br = false;
    int ntp = templateParameterElements.size();
    if (ntp > 0)
    {
        br = true;
        std::unique_ptr<dom::Element> h5Element(new dom::Element(U"h5"));
        h5Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Template Parameters")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(h5Element.release()));
        std::unique_ptr<dom::Element> templateParameterTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ntp; ++i)
        {
            dom::Element* templateParameterElement = templateParameterElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", templateParameterElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(templateParameterElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::u32string defaultTypeId = templateParameterElement->GetAttribute(U"default");
            if (!defaultTypeId.empty())
            {
                AppendType(input, td2Element.get(), defaultTypeId, U"DEFAULT_TYPE", moduleXmlDoc, otherModuleXmlDocs, prefix);
            }
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            templateParameterTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        parentElement->AppendChild(std::unique_ptr<dom::Node>(templateParameterTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> constraintObject;
    dom::Element* constraintElement = GetConstraint(functionElement, constraintObject);
    if (constraintElement)
    {
        br = true;
        std::unique_ptr<dom::Element> h5Element(new dom::Element(U"h5"));
        h5Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constraint")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(h5Element.release()));
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"indent");
        AppendConstraint(input, span.get(), constraintElement, moduleXmlDoc, otherModuleXmlDocs, prefix, false);
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    std::u32string filePath = functionElement->GetAttribute(U"filePath");
    std::u32string line = functionElement->GetAttribute(U"line");
    std::u32string fileName = functionElement->GetAttribute(U"fileName");
    if (!filePath.empty() && !line.empty() && !fileName.empty())
    {
        br = true;
        std::unique_ptr<dom::Element> h5Element(new dom::Element(U"h5"));
        h5Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(h5Element.release()));
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"xml:space", U"preserve");
        span->SetAttribute(U"class", U"indent");
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Line")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> lineLinkElement(new dom::Element(U"a"));
        lineLinkElement->SetAttribute(U"href", prefix + filePath + U"#" + line);
        lineLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
        span->AppendChild(std::unique_ptr<dom::Node>(lineLinkElement.release()));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"of")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> fileLinkElement(new dom::Element(U"a"));
        fileLinkElement->SetAttribute(U"href", prefix + filePath);
        fileLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(fileName)));
        span->AppendChild(std::unique_ptr<dom::Node>(fileLinkElement.release()));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    if (br)
    {
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
        parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
    }
    parentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"hr")));
}

void GenerateFunctionDetailsSection(Input* input, dom::Element* parentElement, const std::u32string& title, const std::vector<dom::Element*> functionElements,
    dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, const std::u32string& prefix)
{
    if (functionElements.empty()) return;
    std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
    h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    parentElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
    int n = functionElements.size();
    for (int i = 0; i < n; ++i)
    {
        dom::Element* functionElement(functionElements[i]);
        GenerateFunctionDoc(input, parentElement, functionElement, moduleXmlDoc, otherModuleXmlDocs, prefix);
    }
}

void GenerateInheritanceDiagramNodes(Input* input, dom::Element* classElement, const std::u32string& specializationName, const std::string& libraryPrefix,
    dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, std::vector<DiagramNode>& diagramNodes, int& level, int& depth)
{
    BaseClassInfo baseClassInfo = GetBaseClass(input, classElement, moduleXmlDoc, otherModuleXmlDocs);
    if (baseClassInfo.baseClassElement)
    {
        --level;
        ++depth;
        GenerateInheritanceDiagramNodes(input, baseClassInfo.baseClassElement, baseClassInfo.baseClassSpecializationName, baseClassInfo.libraryPrefix, 
            moduleXmlDoc, otherModuleXmlDocs, diagramNodes, level, depth);
        ++level;
    }
    int index = diagramNodes.size();
    std::u32string name = specializationName;
    if (name.empty())
    {
        name = MakeFullClassName(classElement);
    }
    bool subject = false;
    bool hasDerivedNodes = level < 0;
    std::u32string link;
    if (libraryPrefix.empty())
    {
        link.append(classElement->GetAttribute(U"docPath"));
    }
    else
    {
        link.append(ToUtf32(Path::Combine(libraryPrefix, ToUtf8(classElement->GetAttribute(U"extPath")))));
    }
    diagramNodes.push_back(DiagramNode(index, level, name, subject, hasDerivedNodes, link));
}

void GenerateRefinementDiagramNodes(dom::Element* conceptElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs, 
    std::vector<DiagramNode>& diagramNodes, int& level, int& depth)
{
    dom::Element* refinedConceptElement = GetRefinedConcept(conceptElement, moduleXmlDoc, otherModuleXmlDocs);
    if (refinedConceptElement)
    {
        --level;
        ++depth;
        GenerateRefinementDiagramNodes(refinedConceptElement, moduleXmlDoc, otherModuleXmlDocs, diagramNodes, level, depth);
        ++level;
    }
    int index = diagramNodes.size();
    std::u32string name = MakeFullConceptName(conceptElement);
    bool subject = false;
    bool hasDerivedNodes = level < 0;
    std::u32string link = conceptElement->GetAttribute(U"docPath");
    diagramNodes.push_back(DiagramNode(index, level, name, subject, hasDerivedNodes, link));
}

void GenerateEnumdoc(Input* input, const std::string& docDir, dom::Element* enumTypeElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    dom::Document* docs = input->docs.get();
    std::u32string enumName = enumTypeElement->GetAttribute(U"name");
    enumName.append(U" Enumerated Type");
    std::u32string title = enumName;
    std::string styleFilePath = "../../../style/style.css";
    dom::Document doc;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    if (docs)
    {
        std::unique_ptr<dom::Element> descriptionAndDetails = GetDescriptionAndDetails(input, enumTypeElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
        if (descriptionAndDetails)
        {
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(descriptionAndDetails.release()));
        }
    }
    std::u32string filePath = enumTypeElement->GetAttribute(U"filePath");
    std::u32string line = enumTypeElement->GetAttribute(U"line");
    std::u32string fileName = enumTypeElement->GetAttribute(U"fileName");
    if (!filePath.empty() && !line.empty() && !fileName.empty())
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"xml:space", U"preserve");
        span->SetAttribute(U"class", U"indent");
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Line")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> lineLinkElement(new dom::Element(U"a"));
        lineLinkElement->SetAttribute(U"href", filePath + U"#" + line);
        lineLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
        span->AppendChild(std::unique_ptr<dom::Node>(lineLinkElement.release()));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"of")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> fileLinkElement(new dom::Element(U"a"));
        fileLinkElement->SetAttribute(U"href", filePath);
        fileLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(fileName)));
        span->AppendChild(std::unique_ptr<dom::Node>(fileLinkElement.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    std::u32string underlyingTypeId = enumTypeElement->GetAttribute(U"underlyingType");
    if (!underlyingTypeId.empty())
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Underlying Type")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"xml:space", U"preserve");
        span->SetAttribute(U"class", U"indent");
        AppendType(input, span.get(), underlyingTypeId, U"UNDERLYING_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    std::unique_ptr<xpath::XPathObject> enumConstantsObject;
    std::vector<dom::Element*> enumConstantElements;
    GetEnumConstants(enumTypeElement, enumConstantsObject, enumConstantElements);
    int n = enumConstantElements.size();
    if (n > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Enumeration Constants")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> enumConstantTableElement(new dom::Element(U"table"));
        for (int i = 0; i < n; ++i)
        {
            dom::Element* enumConstantElement = enumConstantElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumConstantElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::u32string value = enumConstantElement->GetAttribute(U"value");
            if (!value.empty())
            {
                td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(value)));
            }
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, enumConstantElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            enumConstantTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(enumConstantTableElement.release()));
    }
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    doc.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    std::string docFilePath = GetFullPath(Path::Combine(docDir, ToUtf8(enumTypeElement->GetAttribute(U"id")) + ".html"));
    std::ofstream docFile(docFilePath);
    CodeFormatter formatter(docFile);
    formatter.SetIndentSize(1);
    doc.Write(formatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "==> " << docFilePath << std::endl;
    }
}

void GenerateClassDoc(Input* input, const std::string& docDir, dom::Element* classElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    dom::Document* docs = input->docs.get();
    std::unique_ptr<xpath::XPathObject> templateParametersObject;
    std::vector<dom::Element*> templateParameterElements;
    std::u32string className = MakeClassName(classElement, templateParametersObject, templateParameterElements);
    std::u32string specifiers = classElement->GetAttribute(U"specifiers");
    if (specifiers.find(U"abstract") != specifiers.npos)
    {
        className.append(U" Abstract");
    }
    if (specifiers.find(U"static") != specifiers.npos)
    {
        className.append(U" Static");
    }
    className.append(U" Class");
    std::u32string title = className;
    std::string styleFilePath = "../../../style/style.css";
    dom::Document doc;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    bool diagramCodeGenerated = false;
    std::string svgId;
    BaseClassInfo baseClassInfo = GetBaseClass(input, classElement, moduleXmlDoc, otherModuleXmlDocs);
    std::vector<dom::Element*> derivedClasses = GetDerivedClasses(classElement, moduleXmlDoc, otherModuleXmlDocs);
    if (baseClassInfo.baseClassElement || !derivedClasses.empty())
    {
        std::vector<DiagramNode> diagramNodes;
        int level = 0;
        int depth = 0;
        if (baseClassInfo.baseClassElement)
        {
            --level;
            ++depth;
            GenerateInheritanceDiagramNodes(input, baseClassInfo.baseClassElement, baseClassInfo.baseClassSpecializationName, baseClassInfo.libraryPrefix, 
                moduleXmlDoc, otherModuleXmlDocs, diagramNodes, level, depth);
            ++level;
        }
        int index = diagramNodes.size();
        std::u32string name = MakeFullClassName(classElement);
        bool subject = true;
        bool hasDerivedNodes = !derivedClasses.empty();
        std::u32string link = classElement->GetAttribute(U"docPath");
        diagramNodes.push_back(DiagramNode(index, level, name, subject, hasDerivedNodes, link));
        ++level;
        int ndc = derivedClasses.size();
        for (int i = 0; i < ndc; ++i)
        {
            dom::Element* derivedClassElement = derivedClasses[i];
            index = diagramNodes.size();
            name = MakeFullClassName(derivedClassElement);
            subject = false;
            hasDerivedNodes = false;
            link = derivedClassElement->GetAttribute(U"docPath");
            diagramNodes.push_back(DiagramNode(index, level, name, subject, hasDerivedNodes, link));
        }
        int n = diagramNodes.size();
        for (int i = 0; i < n; ++i)
        {
            DiagramNode& node = diagramNodes[i];
            node.level += depth;
        }
        std::sort(diagramNodes.begin(), diagramNodes.end(), DiagramNodeByLevelAndName());
        std::string scriptFileName = ToUtf8(classElement->GetAttribute(U"id")) + "_inheritance.js";
        std::string scriptFilePath = Path::Combine(docDir, scriptFileName);
        svgId = "inheritance_svg_diagram";
        GenerateDiagramCode(diagramNodes, "drawInheritanceDiagram", svgId, scriptFilePath);
        diagramCodeGenerated = true;
        std::unique_ptr<dom::Element> scriptElement(new dom::Element(U"script"));
        scriptElement->SetAttribute(U"type", U"text/javascript");
        scriptElement->SetAttribute(U"src", ToUtf32(scriptFileName));
        scriptElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        headElement->AppendChild(std::unique_ptr<dom::Node>(scriptElement.release()));
    }
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    if (docs)
    {
        std::unique_ptr<dom::Element> descriptionAndDetails = GetDescriptionAndDetails(input, classElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
        if (descriptionAndDetails)
        {
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(descriptionAndDetails.release()));
        }
    }
    std::u32string filePath = classElement->GetAttribute(U"filePath");
    std::u32string line = classElement->GetAttribute(U"line");
    std::u32string fileName = classElement->GetAttribute(U"fileName");
    if (!filePath.empty() && !line.empty() && !fileName.empty())
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"xml:space", U"preserve");
        span->SetAttribute(U"class", U"indent");
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Line")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> lineLinkElement(new dom::Element(U"a"));
        lineLinkElement->SetAttribute(U"href", filePath + U"#" + line);
        lineLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
        span->AppendChild(std::unique_ptr<dom::Node>(lineLinkElement.release()));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"of")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> fileLinkElement(new dom::Element(U"a"));
        fileLinkElement->SetAttribute(U"href", filePath);
        fileLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(fileName)));
        span->AppendChild(std::unique_ptr<dom::Node>(fileLinkElement.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    if (diagramCodeGenerated)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Inheritance")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        bodyElement->SetAttribute(U"onload", U"drawInheritanceDiagram()");
        std::unique_ptr<dom::Element> divElement(new dom::Element(U"div"));
        divElement->SetAttribute(U"class", U"diagram");
        std::unique_ptr<dom::Element> svgElement(new dom::Element(U"svg"));
        svgElement->SetAttribute(U"width", U"0");
        svgElement->SetAttribute(U"height", U"0");
        svgElement->SetAttribute(U"id", ToUtf32(svgId));
        svgElement->SetAttribute(U"xmlns", U"http://www.w3.org/2000/svg");
        svgElement->SetAttribute(U"version", U"2.0");
        divElement->AppendChild(std::unique_ptr<dom::Node>(svgElement.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(divElement.release()));
    }
    int ntp = templateParameterElements.size();
    if (ntp > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Template Parameters")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> templateParameterTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ntp; ++i)
        {
            dom::Element* templateParameterElement = templateParameterElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", templateParameterElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(templateParameterElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::u32string defaultTypeId = templateParameterElement->GetAttribute(U"default");
            if (!defaultTypeId.empty())
            {
                AppendType(input, td2Element.get(), defaultTypeId, U"DEFAULT_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            }
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, templateParameterElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            templateParameterTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(templateParameterTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> constraintObject;
    dom::Element* constraintElement = GetConstraint(classElement, constraintObject);
    if (constraintElement)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constraint")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"class", U"indent");
        AppendConstraint(input, span.get(), constraintElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string(), false);
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    std::unique_ptr<xpath::XPathObject> classObject;
    std::vector<dom::Element*> classElements;
    GetClasses(classElement, classObject, classElements);
    int n = classElements.size();
    if (n > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Classes")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> classTableElement(new dom::Element(U"table"));
        for (int i = 0; i < n; ++i)
        {
            dom::Element* classElement = classElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), classElement->GetAttribute(U"specifiers"), U"class", false);
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", classElement->GetAttribute(U"docPath"));
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeClassName(classElement, templateParametersObject, templateParameterElements))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, classElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            classTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            GenerateClassDoc(input, docDir, classElement, moduleXmlDoc, otherModuleXmlDocs);
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(classTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> ctorDtorObject;
    std::vector<dom::Element*> ctorDtorElements;
    GetConstructorsAndDestructors(classElement, ctorDtorObject, ctorDtorElements);
    int ncd = ctorDtorElements.size();
    if (ncd > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constructors & Destructor")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> ctorDtorTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ncd; ++i)
        {
            dom::Element* ctorDtorElement = ctorDtorElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve"); 
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), ctorDtorElement->GetAttribute(U"specifiers"), U"", false);
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", ctorDtorElement->GetAttribute(U"docPath"));
            std::unique_ptr<xpath::XPathObject> templateParametersObject;
            std::vector<dom::Element*> templateParameterElements;
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeFunctionName(ctorDtorElement, templateParametersObject, templateParameterElements))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            AppendParameters(input, td2Element.get(), ctorDtorElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            if (ctorDtorElement->GetAttribute(U"includeConstraint") == U"true")
            {
                std::unique_ptr<xpath::XPathObject> constraintObject;
                dom::Element* constraintElement = GetConstraint(ctorDtorElement, constraintObject);
                if (constraintElement)
                {
                    td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                    AppendConstraint(input, td2Element.get(), constraintElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string(), true);
                }
            }
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, ctorDtorElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            ctorDtorTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(ctorDtorTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> memberFunctionObject;
    std::vector<dom::Element*> memberFunctionElements;
    GetFunctions(classElement, memberFunctionObject, memberFunctionElements);
    int nmf = memberFunctionElements.size();
    if (nmf > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Member Functions")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> memberFunctionTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nmf; ++i)
        {
            dom::Element* memberFunctionElement = memberFunctionElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), memberFunctionElement->GetAttribute(U"specifiers"), U"", true);
            AppendType(input, tdElement.get(), memberFunctionElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", memberFunctionElement->GetAttribute(U"docPath"));
            std::unique_ptr<xpath::XPathObject> templateParametersObject;
            std::vector<dom::Element*> templateParameterElements;
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeFunctionName(memberFunctionElement, templateParametersObject, templateParameterElements))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            AppendParameters(input, td2Element.get(), memberFunctionElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            std::u32string isConst = memberFunctionElement->GetAttribute(U"const");
            if (isConst == U"true")
            {
                td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
                span->SetAttribute(U"class", U"kw");
                span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"const")));
                td2Element->AppendChild(std::unique_ptr<dom::Node>(span.release()));
            }
            if (memberFunctionElement->GetAttribute(U"includeConstraint") == U"true")
            {
                std::unique_ptr<xpath::XPathObject> constraintObject;
                dom::Element* constraintElement = GetConstraint(memberFunctionElement, constraintObject);
                if (constraintElement)
                {
                    td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                    AppendConstraint(input, td2Element.get(), constraintElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string(), true);
                }
            }
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, memberFunctionElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            memberFunctionTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(memberFunctionTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> delegateObject;
    std::vector<dom::Element*> delegateElements;
    GetDelegates(classElement, delegateObject, delegateElements);
    int ndlg = delegateElements.size();
    if (ndlg > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Delegates")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> delegateTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ndlg; ++i)
        {
            dom::Element* delegateElement = delegateElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", delegateElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), delegateElement->GetAttribute(U"specifiers"), U"delegate", true);
            AppendType(input, tdElement.get(), delegateElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(delegateElement->GetAttribute(U"name"))));
            AppendParameters(input, td2Element.get(), delegateElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, delegateElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            delegateTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(delegateTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> classDelegateObject;
    std::vector<dom::Element*> classDelegateElements;
    GetClassDelegates(classElement, classDelegateObject, classDelegateElements);
    int ncdlg = classDelegateElements.size();
    if (ncdlg > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Class Delegates")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> classDelegateTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ncdlg; ++i)
        {
            dom::Element* classDelegateElement = classDelegateElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr")); 
            trElement->SetAttribute(U"id", classDelegateElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), classDelegateElement->GetAttribute(U"specifiers"), U"delegate", true);
            AppendType(input, tdElement.get(), classDelegateElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(classDelegateElement->GetAttribute(U"name"))));
            AppendParameters(input, td2Element.get(), classDelegateElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, classDelegateElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            classDelegateTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(classDelegateTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> typedefObject;
    std::vector<dom::Element*> typedefElements;
    GetTypedefs(classElement, typedefObject, typedefElements);
    int nt = typedefElements.size();
    if (nt > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Typedefs")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> typedefTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nt; ++i)
        {
            dom::Element* typedefElement = typedefElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", typedefElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), typedefElement->GetAttribute(U"specifiers"), U"typedef", true);
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typedefElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            AppendType(input, td2Element.get(), typedefElement->GetAttribute(U"type"), U"TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, typedefElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            typedefTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(typedefTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> enumTypesObject;
    std::vector<dom::Element*> enumTypeElements;
    GetEnumTypes(classElement, enumTypesObject, enumTypeElements);
    int ne = enumTypeElements.size();
    if (ne > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Enumerated Types")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> enumTypeTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ne; ++i)
        {
            dom::Element* enumTypeElement = enumTypeElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), enumTypeElement->GetAttribute(U"specifiers"), U"enum", false);
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", enumTypeElement->GetAttribute(U"docPath"));
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumTypeElement->GetAttribute(U"name"))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, enumTypeElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            enumTypeTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            GenerateEnumdoc(input, docDir, enumTypeElement, moduleXmlDoc, otherModuleXmlDocs);
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(enumTypeTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> constantsObject;
    std::vector<dom::Element*> constantElements;
    GetConstants(classElement, constantsObject, constantElements);
    int nc = constantElements.size();
    if (nc > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constants")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> constantTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nc; ++i)
        {
            dom::Element* constantElement = constantElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), constantElement->GetAttribute(U"specifiers"), U"const", true);
            AppendType(input, tdElement.get(), constantElement->GetAttribute(U"type"), U"CONSTANT_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(constantElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(constantElement->GetAttribute(U"value"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, constantElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            constantTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(constantTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> variableObject;
    std::vector<dom::Element*> variableElements;
    GetVariables(classElement, variableObject, variableElements);
    int nv = variableElements.size();
    if (nv > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Member Variables")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> variableTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nv; ++i)
        {
            dom::Element* variableElement = variableElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), variableElement->GetAttribute(U"specifiers"), std::u32string(), true);
            AppendType(input, tdElement.get(), variableElement->GetAttribute(U"type"), U"VARIABLE_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(variableElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, variableElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            variableTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(variableTableElement.release()));
    }
    GenerateFunctionDetailsSection(input, bodyElement.get(), U"Constructor& Destructor Details", ctorDtorElements, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
    GenerateFunctionDetailsSection(input, bodyElement.get(), U"Member Function Details", memberFunctionElements, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    doc.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    std::string docFilePath = GetFullPath(Path::Combine(docDir, ToUtf8(classElement->GetAttribute(U"id")) + ".html"));
    std::ofstream docFile(docFilePath);
    CodeFormatter formatter(docFile);
    formatter.SetIndentSize(1);
    doc.Write(formatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "==> " << docFilePath << std::endl;
    }
}

void GetConcepts(dom::Element* parentElement, std::unique_ptr<xpath::XPathObject>& conceptObject, std::vector<dom::Element*>& conceptElements)
{
    conceptObject = xpath::Evaluate(U"concepts/concept", parentElement);
    if (conceptObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* conceptNodeSet = static_cast<xpath::XPathNodeSet*>(conceptObject.get());
        int n = conceptNodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            dom::Node* conceptNode = (*conceptNodeSet)[i];
            if (conceptNode->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* conceptElement = static_cast<dom::Element*>(conceptNode);
                conceptElements.push_back(conceptElement);
            }
        }
    }
}

std::u32string MakeConceptName(dom::Element* conceptElement)
{
    std::u32string conceptName = conceptElement->GetAttribute(U"name");
    std::unique_ptr<xpath::XPathObject> typeParameters = xpath::Evaluate(U"typeParameters/typeParameter", conceptElement);
    if (typeParameters->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* typeParameterNodeSet = static_cast<xpath::XPathNodeSet*>(typeParameters.get());
        int n = typeParameterNodeSet->Length();
        if (n > 0)
        {
            conceptName.append(1, '<');
            for (int i = 0; i < n; ++i)
            {
                if (i > 0)
                {
                    conceptName.append(U", ");
                }
                dom::Node* typeParameterNode = (*typeParameterNodeSet)[i];
                if (typeParameterNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* typeParameterElement = static_cast<dom::Element*>(typeParameterNode);
                    conceptName.append(typeParameterElement->GetAttribute(U"name"));
                }
            }
            conceptName.append(1, '>');
        }
    }
    return conceptName;
}

void GenerateConceptDoc(Input* input, const std::string& docDir, dom::Element* conceptElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    dom::Document* docs = input->docs.get();
    std::u32string title = MakeConceptName(conceptElement) + U" Concept";
    std::string styleFilePath = "../../../style/style.css";
    dom::Document doc;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    dom::Element* refinedConceptElement = GetRefinedConcept(conceptElement, moduleXmlDoc, otherModuleXmlDocs);
    std::vector<dom::Element*> derivedConcepts = GetDerivedConcepts(conceptElement, moduleXmlDoc, otherModuleXmlDocs);
    std::string svgId;
    bool diagramCodeGenerated = false;
    if (refinedConceptElement || !derivedConcepts.empty())
    {
        std::vector<DiagramNode> diagramNodes;
        int level = 0;
        int depth = 0;
        if (refinedConceptElement)
        {
            --level;
            ++depth;
            GenerateRefinementDiagramNodes(refinedConceptElement, moduleXmlDoc, otherModuleXmlDocs, diagramNodes, level, depth);
            ++level;
        }
        int index = diagramNodes.size();
        std::u32string name = MakeFullConceptName(conceptElement);
        bool subject = true;
        bool hasDerivedNodes = !derivedConcepts.empty();
        std::u32string link = conceptElement->GetAttribute(U"docPath");
        diagramNodes.push_back(DiagramNode(index, level, name, subject, hasDerivedNodes, link));
        ++level;
        int ndc = derivedConcepts.size();
        for (int i = 0; i < ndc; ++i)
        {
            dom::Element* derivedConceptElement = derivedConcepts[i];
            index = diagramNodes.size();
            name = MakeFullConceptName(derivedConceptElement);
            subject = false;
            hasDerivedNodes = false;
            link = derivedConceptElement->GetAttribute(U"docPath");
            diagramNodes.push_back(DiagramNode(index, level, name, subject, hasDerivedNodes, link));
        }
        int n = diagramNodes.size();
        for (int i = 0; i < n; ++i)
        {
            DiagramNode& node = diagramNodes[i];
            node.level += depth;
        }
        std::sort(diagramNodes.begin(), diagramNodes.end(), DiagramNodeByLevelAndName());
        std::string scriptFileName = ToUtf8(conceptElement->GetAttribute(U"id")) + "_refinement.js";
        std::string scriptFilePath = Path::Combine(docDir, scriptFileName);
        svgId = "refinement_svg_diagram";
        GenerateDiagramCode(diagramNodes, "drawRefinementDiagram", svgId, scriptFilePath);
        diagramCodeGenerated = true;
        std::unique_ptr<dom::Element> scriptElement(new dom::Element(U"script"));
        scriptElement->SetAttribute(U"type", U"text/javascript");
        scriptElement->SetAttribute(U"src", ToUtf32(scriptFileName));
        scriptElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U" ")));
        headElement->AppendChild(std::unique_ptr<dom::Node>(scriptElement.release()));
    }
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    if (docs)
    {
        std::unique_ptr<dom::Element> descriptionAndDetails = GetDescriptionAndDetails(input, conceptElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
        if (descriptionAndDetails)
        {
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(descriptionAndDetails.release()));
        }
    }
    std::u32string filePath = conceptElement->GetAttribute(U"filePath");
    std::u32string line = conceptElement->GetAttribute(U"line");
    std::u32string fileName = conceptElement->GetAttribute(U"fileName");
    if (!filePath.empty() && !line.empty() && !fileName.empty())
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Definition")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
        span->SetAttribute(U"xml:space", U"preserve");
        span->SetAttribute(U"class", U"indent");
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Line")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> lineLinkElement(new dom::Element(U"a"));
        lineLinkElement->SetAttribute(U"href", filePath + U"#" + line);
        lineLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(line)));
        span->AppendChild(std::unique_ptr<dom::Node>(lineLinkElement.release()));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"of")));
        span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
        std::unique_ptr<dom::Element> fileLinkElement(new dom::Element(U"a"));
        fileLinkElement->SetAttribute(U"href", filePath);
        fileLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(fileName)));
        span->AppendChild(std::unique_ptr<dom::Node>(fileLinkElement.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
    }
    if (diagramCodeGenerated)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Refinement")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        bodyElement->SetAttribute(U"onload", U"drawRefinementDiagram()");
        std::unique_ptr<dom::Element> divElement(new dom::Element(U"div"));
        divElement->SetAttribute(U"class", U"diagram");
        std::unique_ptr<dom::Element> svgElement(new dom::Element(U"svg"));
        svgElement->SetAttribute(U"width", U"0");
        svgElement->SetAttribute(U"height", U"0");
        svgElement->SetAttribute(U"id", ToUtf32(svgId));
        svgElement->SetAttribute(U"xmlns", U"http://www.w3.org/2000/svg");
        svgElement->SetAttribute(U"version", U"2.0");
        divElement->AppendChild(std::unique_ptr<dom::Node>(svgElement.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(divElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> typeParametersObject;
    std::vector<dom::Element*> typeParameterElements;
    GetTypeParameters(conceptElement, typeParametersObject, typeParameterElements);
    int ntp = typeParameterElements.size();
    if (ntp > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Type Parameters")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> typeParameterTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ntp; ++i)
        {
            dom::Element* typeParameterElement = typeParameterElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", typeParameterElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typeParameterElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            typeParameterTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(typeParameterTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> constraintsObject;
    std::vector<dom::Element*> constraintElements;
    GetConstraints(conceptElement, constraintsObject, constraintElements);
    int nco = constraintElements.size();
    if (nco > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constraints")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        for (int i = 0; i < nco; ++i)
        {
            dom::Element* constraintElement = constraintElements[i];
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->SetAttribute(U"xml:space", U"preserve");
            span->SetAttribute(U"class", U"indent");
            AppendConstraint(input, span.get(), constraintElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string(), false);
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
        }
    }
    std::unique_ptr<xpath::XPathObject> axiomsObject;
    std::vector<dom::Element*> axiomElements;
    GetAxioms(conceptElement, axiomsObject, axiomElements);
    int na = axiomElements.size();
    if (na > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Axioms")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        for (int i = 0; i < na; ++i)
        {
            dom::Element* axiomElement = axiomElements[i];
            std::unique_ptr<dom::Element> span(new dom::Element(U"span"));
            span->SetAttribute(U"xml:space", U"preserve");
            span->SetAttribute(U"class", U"indent");
            std::unique_ptr<dom::Element> axiomSpan(new dom::Element(U"span"));
            axiomSpan->SetAttribute(U"xml:space", U"preserve");
            axiomSpan->SetAttribute(U"class", U"kw");
            axiomSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"axiom")));
            span->AppendChild(std::unique_ptr<dom::Node>(axiomSpan.release()));
            dom::Node* symbolChild = axiomElement->FirstChild();
            if (symbolChild && symbolChild->GetNodeType() == dom::NodeType::elementNode)
            {
                dom::Element* symbolElement = static_cast<dom::Element*>(symbolChild);
                span->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                AppendSymbol(input, span.get(), symbolElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            }
            AppendAxiomParameters(input, span.get(), axiomElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            span->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(span.release()));
            std::unique_ptr<dom::Element> axiomStatementSpan(new dom::Element(U"span"));
            axiomStatementSpan->SetAttribute(U"xml:space", U"preserve");
            axiomStatementSpan->SetAttribute(U"class", U"indent");
            axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"{")));
            axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(axiomStatementSpan.release()));
            std::unique_ptr<xpath::XPathObject> axiomStatementsObject;
            std::vector<dom::Element*> axiomStatementElements;
            GetAxiomStatements(axiomElement, axiomStatementsObject, axiomStatementElements);
            int ns = axiomStatementElements.size();
            for (int i = 0; i < ns; ++i)
            {
                std::unique_ptr<dom::Element> axiomStatementSpan(new dom::Element(U"span"));
                axiomStatementSpan->SetAttribute(U"xml:space", U"preserve");
                axiomStatementSpan->SetAttribute(U"class", U"indent");
                axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                dom::Element* axiomStatementElement = axiomStatementElements[i];
                axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(axiomStatementElement->GetAttribute(U"value"))));
                axiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
                bodyElement->AppendChild(std::unique_ptr<dom::Node>(axiomStatementSpan.release()));
            }
            std::unique_ptr<dom::Element> endExiomStatementSpan(new dom::Element(U"span"));
            endExiomStatementSpan->SetAttribute(U"xml:space", U"preserve");
            endExiomStatementSpan->SetAttribute(U"class", U"indent");
            endExiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"}")));
            endExiomStatementSpan->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(endExiomStatementSpan.release()));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"br")));
        }
    }
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    doc.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    std::string docFilePath = GetFullPath(Path::Combine(docDir, ToUtf8(conceptElement->GetAttribute(U"id")) + ".html"));
    std::ofstream docFile(docFilePath);
    CodeFormatter formatter(docFile);
    formatter.SetIndentSize(1);
    doc.Write(formatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "==> " << docFilePath << std::endl;
    }
}

void GenerateNamespaceDoc(Input* input, const std::string& docDir, dom::Element* nsElement, dom::Document* moduleXmlDoc, const std::vector<dom::Document*>& otherModuleXmlDocs)
{
    dom::Document* docs = input->docs.get();
    std::u32string title = MakeFullNamespaceName(nsElement) + U" Namespace";
    std::string styleFilePath = "../../../style/style.css";
    dom::Document doc;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    if (docs)
    {
        std::unique_ptr<dom::Element> descriptionAndDetails = GetDescriptionAndDetails(input, nsElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
        if (descriptionAndDetails)
        {
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(descriptionAndDetails.release()));
        }
    }
    std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
    h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Namespaces")));
    std::unique_ptr<dom::Element> namespaceTableElement(new dom::Element(U"table"));
    std::vector<std::unique_ptr<xpath::XPathObject>> namespaceXPathObjects;
    std::vector<dom::Element*> nsElements;
    if (GenerateNamespaceNames(0, namespaceTableElement.get(), nsElement, U"", namespaceXPathObjects, nsElements))
    {
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(namespaceTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> conceptObject;
    std::vector<dom::Element*> conceptElements;
    GetConcepts(nsElement, conceptObject, conceptElements);
    int n = conceptElements.size();
    if (n > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Concepts")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> conceptTableElement(new dom::Element(U"table"));
        for (int i = 0; i < n; ++i)
        {
            dom::Element* conceptElement = conceptElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), conceptElement->GetAttribute(U"specifiers"), U"concept", false);
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", conceptElement->GetAttribute(U"docPath"));
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeConceptName(conceptElement))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, conceptElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            conceptTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            GenerateConceptDoc(input, docDir, conceptElement, moduleXmlDoc, otherModuleXmlDocs);
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(conceptTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> templateParametersObject;
    std::vector<dom::Element*> templateParameterElements;
    std::unique_ptr<xpath::XPathObject> classObject;
    std::vector<dom::Element*> classElements;
    GetClasses(nsElement, classObject, classElements);
    int nc = classElements.size();
    if (nc > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Classes")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> classTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nc; ++i)
        {
            dom::Element* classElement = classElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), classElement->GetAttribute(U"specifiers"), U"class", false);
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", classElement->GetAttribute(U"docPath"));
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeClassName(classElement, templateParametersObject, templateParameterElements))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, classElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            classTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            GenerateClassDoc(input, docDir, classElement, moduleXmlDoc, otherModuleXmlDocs);
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(classTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> functionObject;
    std::vector<dom::Element*> functionElements;
    GetFunctions(nsElement, functionObject, functionElements);
    int nf = functionElements.size();
    if (nf > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Functions")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> functionTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nf; ++i)
        {
            dom::Element* functionElement = functionElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), functionElement->GetAttribute(U"specifiers"), U"", true);
            AppendType(input, tdElement.get(), functionElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", functionElement->GetAttribute(U"docPath"));
            std::unique_ptr<xpath::XPathObject> templateParametersObject;
            std::vector<dom::Element*> templateParameterElements;
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeFunctionName(functionElement, templateParametersObject, templateParameterElements))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            AppendParameters(input, td2Element.get(), functionElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            if (functionElement->GetAttribute(U"includeConstraint") == U"true")
            {
                std::unique_ptr<xpath::XPathObject> constraintObject;
                dom::Element* constraintElement = GetConstraint(functionElement, constraintObject);
                if (constraintElement)
                {
                    td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                    AppendConstraint(input, td2Element.get(), constraintElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string(), true);
                }
            }
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, functionElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            functionTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(functionTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> delegateObject;
    std::vector<dom::Element*> delegateElements;
    GetDelegates(nsElement, delegateObject, delegateElements);
    int ndlg = delegateElements.size();
    if (ndlg > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Delegates")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> delegateTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ndlg; ++i)
        {
            dom::Element* delegateElement = delegateElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", delegateElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), delegateElement->GetAttribute(U"specifiers"), U"delegate", true);
            AppendType(input, tdElement.get(), delegateElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(delegateElement->GetAttribute(U"name"))));
            AppendParameters(input, td2Element.get(), delegateElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, delegateElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            delegateTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(delegateTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> classDelegateObject;
    std::vector<dom::Element*> classDelegateElements;
    GetClassDelegates(nsElement, classDelegateObject, classDelegateElements);
    int ncdlg = classDelegateElements.size();
    if (ncdlg > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Class Delegates")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> classDelegateTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ncdlg; ++i)
        {
            dom::Element* classDelegateElement = classDelegateElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", classDelegateElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), classDelegateElement->GetAttribute(U"specifiers"), U"class delegate", true);
            AppendType(input, tdElement.get(), classDelegateElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(classDelegateElement->GetAttribute(U"name"))));
            AppendParameters(input, td2Element.get(), classDelegateElement, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, classDelegateElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            classDelegateTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(classDelegateTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> typedefObject;
    std::vector<dom::Element*> typedefElements;
    GetTypedefs(nsElement, typedefObject, typedefElements);
    int nt = typedefElements.size();
    if (nt > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Typedefs")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> typedefTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nt; ++i)
        {
            dom::Element* typedefElement = typedefElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            trElement->SetAttribute(U"id", typedefElement->GetAttribute(U"id"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), typedefElement->GetAttribute(U"specifiers"), U"typedef", true);
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typedefElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            AppendType(input, td2Element.get(), typedefElement->GetAttribute(U"type"), U"TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, typedefElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            typedefTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(typedefTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> enumTypesObject;
    std::vector<dom::Element*> enumTypeElements;
    GetEnumTypes(nsElement, enumTypesObject, enumTypeElements);
    int ne = enumTypeElements.size();
    if (ne > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Enumerated Types")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> enumTypeTableElement(new dom::Element(U"table"));
        for (int i = 0; i < ne; ++i)
        {
            dom::Element* enumTypeElement = enumTypeElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), enumTypeElement->GetAttribute(U"specifiers"), U"enum", false);
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", enumTypeElement->GetAttribute(U"docPath"));
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumTypeElement->GetAttribute(U"name"))));
            td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, enumTypeElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            enumTypeTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            GenerateEnumdoc(input, docDir, enumTypeElement, moduleXmlDoc, otherModuleXmlDocs);
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(enumTypeTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> constantsObject;
    std::vector<dom::Element*> constantElements;
    GetConstants(nsElement, constantsObject, constantElements);
    int nco = constantElements.size();
    if (nco > 0)
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constants")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> constantTableElement(new dom::Element(U"table"));
        for (int i = 0; i < nco; ++i)
        {
            dom::Element* constantElement = constantElements[i];
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            tdElement->SetAttribute(U"class", U"rightAlignedCol");
            AppendSpecifiers(tdElement.get(), constantElement->GetAttribute(U"specifiers"), U"const", true);
            AppendType(input, tdElement.get(), constantElement->GetAttribute(U"type"), U"CONSTANT_TYPE", moduleXmlDoc, otherModuleXmlDocs, std::u32string());
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
            tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(constantElement->GetAttribute(U"name"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
            td2Element->SetAttribute(U"xml:space", U"preserve");
            td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(constantElement->GetAttribute(U"value"))));
            trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
            std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
            td3Element->SetAttribute(U"xml:space", U"preserve");
            if (AppendDescription(td3Element.get(), input, constantElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
            {
                trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
            }
            constantTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(constantTableElement.release()));
    }
    GenerateFunctionDetailsSection(input, bodyElement.get(), U"Function Details", functionElements, moduleXmlDoc, otherModuleXmlDocs, std::u32string());
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    doc.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    std::string docFilePath = GetFullPath(Path::Combine(docDir, ToUtf8(nsElement->GetAttribute(U"id")) + ".html"));
    std::ofstream docFile(docFilePath);
    CodeFormatter formatter(docFile);
    formatter.SetIndentSize(1);
    doc.Write(formatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "==> " << docFilePath << std::endl;
    }
}

void GenerateModuleIndexHtml(Input* input, const std::string& moduleDir, const std::u32string& moduleName, dom::Document* moduleXmlDoc, 
    const std::vector<dom::Document*>& otherModuleXmlDocs, std::vector<GrammarInfo>& grammars)
{
    dom::Document* docs = input->docs.get();
    std::string docDir = GetFullPath(Path::Combine(moduleDir, "doc"));
    std::u32string title = moduleName + U" Module";
    std::string styleFilePath = "../../style/style.css";
    dom::Document indexDoc;
    std::unique_ptr<dom::Element> htmlElement(new dom::Element(U"html"));
    std::unique_ptr<dom::Element> headElement(new dom::Element(U"head"));
    std::unique_ptr<dom::Element> metaElement(new dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<dom::Node>(metaElement.release()));
    std::unique_ptr<dom::Element> titleElement(new dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<dom::Node>(titleElement.release()));
    std::unique_ptr<dom::Element> linkElement(new dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(headElement.release()));
    std::unique_ptr<dom::Element> bodyElement(new dom::Element(U"body"));
    std::unique_ptr<dom::Element> h1Element(new dom::Element(U"h1"));
    h1Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(title)));
    bodyElement->AppendChild(std::unique_ptr<dom::Node>(h1Element.release()));
    if (docs)
    {
        std::unique_ptr<dom::Element> descriptionAndDetails = GetDescriptionAndDetails(input, moduleName, docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/");
        if (descriptionAndDetails)
        {
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(descriptionAndDetails.release()));
        }
    }
    if (!grammars.empty())
    {
        std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
        h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Grammars")));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        std::unique_ptr<dom::Element> grammarTableElement(new dom::Element(U"table"));
        for (const GrammarInfo& grammar : grammars)
        {
            std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
            std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
            tdElement->SetAttribute(U"xml:space", U"preserve");
            std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
            linkElement->SetAttribute(U"href", ToUtf32(grammar.grammarFileName));
            linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(grammar.grammarName)));
            tdElement->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
            trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
            grammarTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
        }
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(grammarTableElement.release()));
    }
    bool hasNamespaces = false;
    std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
    h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Namespaces")));
    std::unique_ptr<dom::Element> namespaceTableElement(new dom::Element(U"table"));
    std::unique_ptr<xpath::XPathObject> rootNamespaceObject = xpath::Evaluate(U"/symbolTable/namespace", moduleXmlDoc);
    dom::Element* rootNamespaceElement = nullptr;
    if (rootNamespaceObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* rootNamespaceNodeSet = static_cast<xpath::XPathNodeSet*>(rootNamespaceObject.get());
        int n = rootNamespaceNodeSet->Length();
        if (n == 1)
        {
            dom::Node* rootNamespaceNode = (*rootNamespaceNodeSet)[0];
            if (rootNamespaceNode->GetNodeType() == dom::NodeType::elementNode)
            {
                rootNamespaceElement = static_cast<dom::Element*>(rootNamespaceNode);
                std::vector<std::unique_ptr<xpath::XPathObject>> namespaceXPathObjects;
                std::vector<dom::Element*> nsElements;
                if (GenerateNamespaceNames(0, namespaceTableElement.get(), rootNamespaceElement, U"doc/", namespaceXPathObjects, nsElements))
                {
                    hasNamespaces = true;
                }
                int n = nsElements.size();
                for (int i = 0; i < n; ++i)
                {
                    dom::Element* nsElement = nsElements[i];
                    GenerateNamespaceDoc(input, docDir, nsElement, moduleXmlDoc, otherModuleXmlDocs);
                }
            }
        }
    }
    if (hasNamespaces)
    {
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
        bodyElement->AppendChild(std::unique_ptr<dom::Node>(namespaceTableElement.release()));
    }
    std::unique_ptr<xpath::XPathObject> filesObject;
    filesObject = xpath::Evaluate(U"/symbolTable/files/file", moduleXmlDoc);
    if (filesObject->Type() == xpath::XPathObjectType::nodeSet)
    {
        xpath::XPathNodeSet* filesNodeSet = static_cast<xpath::XPathNodeSet*>(filesObject.get());
        int n = filesNodeSet->Length();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Files")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> fileTableElement(new dom::Element(U"table"));
            for (int i = 0; i < n; ++i)
            {
                dom::Node* fileNode = (*filesNodeSet)[i];
                if (fileNode->GetNodeType() == dom::NodeType::elementNode)
                {
                    dom::Element* fileElement = static_cast<dom::Element*>(fileNode);
                    std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                    std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                    tdElement->SetAttribute(U"xml:space", U"preserve");
                    std::unique_ptr<dom::Element> fileLinkElement(new dom::Element(U"a"));
                    fileLinkElement->SetAttribute(U"href", fileElement->GetAttribute(U"path"));
                    fileLinkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(fileElement->GetAttribute(U"name"))));
                    tdElement->AppendChild(std::unique_ptr<dom::Node>(fileLinkElement.release()));
                    trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                    fileTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                }
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(fileTableElement.release()));
        }
    }
    if (rootNamespaceElement)
    {
        std::unique_ptr<xpath::XPathObject> templateParametersObject;
        std::vector<dom::Element*> templateParameterElements;
        std::unique_ptr<xpath::XPathObject> conceptObject;
        std::vector<dom::Element*> conceptElements;
        GetConcepts(rootNamespaceElement, conceptObject, conceptElements);
        int n = conceptElements.size();
        if (n > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Concepts")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> conceptTableElement(new dom::Element(U"table"));
            for (int i = 0; i < n; ++i)
            {
                dom::Element* conceptElement = conceptElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), conceptElement->GetAttribute(U"specifiers"), U"concept", false);
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                linkElement->SetAttribute(U"href", U"doc/" + conceptElement->GetAttribute(U"id") + U".html");
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeConceptName(conceptElement))));
                td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, conceptElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/"))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                conceptTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                GenerateConceptDoc(input, docDir, conceptElement, moduleXmlDoc, otherModuleXmlDocs);
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(conceptTableElement.release()));
        }
        std::unique_ptr<xpath::XPathObject> classObject;
        std::vector<dom::Element*> classElements;
        GetClasses(rootNamespaceElement, classObject, classElements);
        int nc = classElements.size();
        if (nc > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Classes")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> classTableElement(new dom::Element(U"table"));
            for (int i = 0; i < nc; ++i)
            {
                dom::Element* classElement = classElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), classElement->GetAttribute(U"specifiers"), U"class", false);
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                linkElement->SetAttribute(U"href", U"doc/" + classElement->GetAttribute(U"id") + U".html");
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeClassName(classElement, templateParametersObject, templateParameterElements))));
                td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, classElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/"))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                classTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                GenerateClassDoc(input, docDir, classElement, moduleXmlDoc, otherModuleXmlDocs);
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(classTableElement.release()));
        }
        std::unique_ptr<xpath::XPathObject> functionObject;
        std::vector<dom::Element*> functionElements;
        GetFunctions(rootNamespaceElement, functionObject, functionElements);
        int nf = functionElements.size();
        if (nf > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Functions")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> functionTableElement(new dom::Element(U"table"));
            for (int i = 0; i < nf; ++i)
            {
                dom::Element* functionElement = functionElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), functionElement->GetAttribute(U"specifiers"), U"", true);
                AppendType(input, tdElement.get(), functionElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                td2Element->SetAttribute(U"xml:space", U"preserve");
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                std::u32string link = U"doc/";
                link.append(functionElement->GetAttribute(U"docPath"));
                linkElement->SetAttribute(U"href", link);
                std::unique_ptr<xpath::XPathObject> templateParametersObject;
                std::vector<dom::Element*> templateParameterElements;
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(MakeFunctionName(functionElement, templateParametersObject, templateParameterElements))));
                td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                AppendParameters(input, td2Element.get(), functionElement, moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                if (functionElement->GetAttribute(U"includeConstraint") == U"true")
                {
                    std::unique_ptr<xpath::XPathObject> constraintObject;
                    dom::Element* constraintElement = GetConstraint(functionElement, constraintObject);
                    if (constraintElement)
                    {
                        td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                        AppendConstraint(input, td2Element.get(), constraintElement, moduleXmlDoc, otherModuleXmlDocs, U"doc/", true);
                    }
                }
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, functionElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/"))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                functionTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(functionTableElement.release()));
        }
        std::unique_ptr<xpath::XPathObject> delegateObject;
        std::vector<dom::Element*> delegateElements;
        GetDelegates(rootNamespaceElement, delegateObject, delegateElements);
        int ndlg = delegateElements.size();
        if (ndlg > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Delegates")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> delegateTableElement(new dom::Element(U"table"));
            for (int i = 0; i < ndlg; ++i)
            {
                dom::Element* delegateElement = delegateElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                trElement->SetAttribute(U"id", delegateElement->GetAttribute(U"id"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), delegateElement->GetAttribute(U"specifiers"), U"delegate", true);
                AppendType(input, tdElement.get(), delegateElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(delegateElement->GetAttribute(U"name"))));
                AppendParameters(input, td2Element.get(), delegateElement, moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, delegateElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/"))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                delegateTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(delegateTableElement.release()));
        }
        std::unique_ptr<xpath::XPathObject> classDelegateObject;
        std::vector<dom::Element*> classDelegateElements;
        GetClassDelegates(rootNamespaceElement, classDelegateObject, classDelegateElements);
        int ncdlg = classDelegateElements.size();
        if (ncdlg > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Class Delegates")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> classDelegateTableElement(new dom::Element(U"table"));
            for (int i = 0; i < ncdlg; ++i)
            {
                dom::Element* classDelegateElement = classDelegateElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                trElement->SetAttribute(U"id", classDelegateElement->GetAttribute(U"id"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), classDelegateElement->GetAttribute(U"specifiers"), U"class delegate", true);
                AppendType(input, tdElement.get(), classDelegateElement->GetAttribute(U"returnType"), U"RETURN_TYPE", moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(classDelegateElement->GetAttribute(U"name"))));
                AppendParameters(input, td2Element.get(), classDelegateElement, moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, classDelegateElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/"))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                classDelegateTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(classDelegateTableElement.release()));
        }
        std::unique_ptr<xpath::XPathObject> typedefObject;
        std::vector<dom::Element*> typedefElements;
        GetTypedefs(rootNamespaceElement, typedefObject, typedefElements);
        int nt = typedefElements.size();
        if (nt > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Typedefs")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> typedefTableElement(new dom::Element(U"table"));
            for (int i = 0; i < nt; ++i)
            {
                dom::Element* typedefElement = typedefElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                trElement->SetAttribute(U"id", typedefElement->GetAttribute(U"id"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), typedefElement->GetAttribute(U"specifiers"), U"typedef", true);
                tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(typedefElement->GetAttribute(U"name"))));
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                td2Element->SetAttribute(U"xml:space", U"preserve");
                AppendType(input, td2Element.get(), typedefElement->GetAttribute(U"type"), U"TYPE", moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, typedefElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, std::u32string()))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                typedefTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(typedefTableElement.release()));
        }
        std::unique_ptr<xpath::XPathObject> enumTypesObject;
        std::vector<dom::Element*> enumTypeElements;
        GetEnumTypes(rootNamespaceElement, enumTypesObject, enumTypeElements);
        int ne = enumTypeElements.size();
        if (ne > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Enumerated Types")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> enumTypeTableElement(new dom::Element(U"table"));
            for (int i = 0; i < ne; ++i)
            {
                dom::Element* enumTypeElement = enumTypeElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), enumTypeElement->GetAttribute(U"specifiers"), U"enum", false);
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                td2Element->SetAttribute(U"xml:space", U"preserve");
                std::unique_ptr<dom::Element> linkElement(new dom::Element(U"a"));
                linkElement->SetAttribute(U"href", U"doc/" + enumTypeElement->GetAttribute(U"docPath"));
                linkElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(enumTypeElement->GetAttribute(U"name"))));
                td2Element->AppendChild(std::unique_ptr<dom::Node>(linkElement.release()));
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, enumTypeElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/"))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                enumTypeTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
                GenerateEnumdoc(input, docDir, enumTypeElement, moduleXmlDoc, otherModuleXmlDocs);
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(enumTypeTableElement.release()));
        }
        std::unique_ptr<xpath::XPathObject> constantsObject;
        std::vector<dom::Element*> constantElements;
        GetConstants(rootNamespaceElement, constantsObject, constantElements);
        int nco = constantElements.size();
        if (nco > 0)
        {
            std::unique_ptr<dom::Element> h2Element(new dom::Element(U"h2"));
            h2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(U"Constants")));
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(h2Element.release()));
            std::unique_ptr<dom::Element> constantTableElement(new dom::Element(U"table"));
            for (int i = 0; i < nco; ++i)
            {
                dom::Element* constantElement = constantElements[i];
                std::unique_ptr<dom::Element> trElement(new dom::Element(U"tr"));
                std::unique_ptr<dom::Element> tdElement(new dom::Element(U"td"));
                tdElement->SetAttribute(U"xml:space", U"preserve");
                tdElement->SetAttribute(U"class", U"rightAlignedCol");
                AppendSpecifiers(tdElement.get(), constantElement->GetAttribute(U"specifiers"), U"const", true);
                AppendType(input, tdElement.get(), constantElement->GetAttribute(U"type"), U"CONSTANT_TYPE", moduleXmlDoc, otherModuleXmlDocs, U"doc/");
                tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::EntityReference(U"nbsp")));
                tdElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(constantElement->GetAttribute(U"name"))));
                trElement->AppendChild(std::unique_ptr<dom::Node>(tdElement.release()));
                std::unique_ptr<dom::Element> td2Element(new dom::Element(U"td"));
                td2Element->SetAttribute(U"xml:space", U"preserve");
                td2Element->AppendChild(std::unique_ptr<dom::Node>(new dom::Text(constantElement->GetAttribute(U"value"))));
                trElement->AppendChild(std::unique_ptr<dom::Node>(td2Element.release()));
                std::unique_ptr<dom::Element> td3Element(new dom::Element(U"td"));
                td3Element->SetAttribute(U"xml:space", U"preserve");
                if (AppendDescription(td3Element.get(), input, constantElement->GetAttribute(U"id"), docs, moduleXmlDoc, otherModuleXmlDocs, U"doc/"))
                {
                    trElement->AppendChild(std::unique_ptr<dom::Node>(td3Element.release()));
                }
                constantTableElement->AppendChild(std::unique_ptr<dom::Node>(trElement.release()));
            }
            bodyElement->AppendChild(std::unique_ptr<dom::Node>(constantTableElement.release()));
        }
        GenerateFunctionDetailsSection(input, bodyElement.get(), U"Function Details", functionElements, moduleXmlDoc, otherModuleXmlDocs, U"doc/");
    }
    htmlElement->AppendChild(std::unique_ptr<dom::Node>(bodyElement.release()));
    indexDoc.AppendChild(std::unique_ptr<dom::Node>(htmlElement.release()));
    std::string indexFilePath = GetFullPath(Path::Combine(moduleDir, "index.html"));
    std::ofstream indexFile(indexFilePath);
    CodeFormatter formatter(indexFile);
    formatter.SetIndentSize(1);
    indexDoc.Write(formatter);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        std::cout << "==> " << indexFilePath << std::endl;
    }
}

void BuildDocs(const std::u32string& solutionName, const std::vector<std::u32string>& moduleNames, std::vector<std::string>& grammarFilePaths)
{
    bool verbose = GetGlobalFlag(GlobalFlags::verbose);
    if (verbose)
    {
        std::cout << "Building documentation for solution '" << ToUtf8(solutionName) + "'..." << std::endl;
    }
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    boost::filesystem::create_directories(targetDir);
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    boost::filesystem::create_directories(contentDir);
    std::vector<std::unique_ptr<dom::Document>> moduleXmlFiles;
    std::vector<std::string> moduleLinks;
    for (const std::u32string& moduleName : moduleNames)
    {
        std::string moduleNameStr = ToUtf8(moduleName);
        std::string moduleDir = GetFullPath(Path::Combine(contentDir, moduleNameStr));
        boost::filesystem::create_directories(moduleDir);
        std::string moduleXmlFilePath = Path::Combine(moduleDir, moduleNameStr + ".xml");
        std::string moduleLink = Path::Combine(Path::Combine("content", moduleNameStr), "index.html");
        moduleLinks.push_back(moduleLink);
        std::string docDir = GetFullPath(Path::Combine(moduleDir, "doc"));
        boost::filesystem::create_directories(docDir);
        if (verbose)
        {
            std::cout << "> " << moduleXmlFilePath << std::endl;
        }
        std::unique_ptr<dom::Document> moduleXmlFile = dom::ReadDocument(moduleXmlFilePath);
        moduleXmlFiles.push_back(std::move(moduleXmlFile));
    }
    int n = moduleNames.size();
    for (int i = 0; i < n; ++i)
    {
        const std::u32string& moduleName = moduleNames[i];
        std::string moduleNameStr = ToUtf8(moduleName);
        std::string moduleDir = GetFullPath(Path::Combine(contentDir, moduleNameStr));
        std::string grammarXmlFilePath = GetFullPath(Path::Combine(moduleDir, "grammars.xml"));
        std::string relativeModuleDir = Path::Combine("content", moduleNameStr);
        std::vector<GrammarInfo> grammars;
        BuildParserDocs(input, moduleDir, grammarXmlFilePath, relativeModuleDir, grammarFilePaths, moduleName, grammars);
        dom::Document* moduleXmlDoc = moduleXmlFiles[i].get();
        std::vector<dom::Document*> otherModuleXmlDocs;
        {
            std::lock_guard<std::mutex> lock(GetInputMutex());
            for (const auto& externalModuleDoc : input->externalModuleDocs)
            {
                otherModuleXmlDocs.push_back(externalModuleDoc.get());
            }
        }
        for (int j = 0; j < n; ++j)
        {
            if (i != j)
            {
                otherModuleXmlDocs.push_back(moduleXmlFiles[j].get());
            }
        }
        GenerateModuleIndexHtml(input, moduleDir, moduleName, moduleXmlDoc, otherModuleXmlDocs, grammars);
    }
    GenerateRootIndexHtml(input, targetDir, solutionName, moduleNames, moduleLinks, moduleXmlFiles);
    if (verbose)
    {
        std::cout << "Documentation generated." << std::endl;
    }
}

} } // namespace cmajor::cmdoclib
