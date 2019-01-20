// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/dom/Parser.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/xml/XmlParser.hpp>
#include <cmajor/util/MappedInputFile.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <string>
#include <stack>

namespace cmajor { namespace dom {

using namespace cmajor::xml;
using namespace cmajor::util;
using namespace cmajor::unicode;

class DomDocumentHandler : public XmlContentHandler
{
public:
    DomDocumentHandler();
    std::unique_ptr<Document> GetDocument();
    void StartDocument() override;
    void EndDocument() override;
    void Version(const std::u32string& xmlVersion) override;
    void Standalone(bool standalone) override;
    void Encoding(const std::u32string& encoding) override;
    void Text(const std::u32string& text) override;
    void Comment(const std::u32string& comment) override;
    void PI(const std::u32string& target, const std::u32string& data) override;
    void CDataSection(const std::u32string& data) override;
    void StartElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName, const Attributes& attributes) override;
    void EndElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName) override;
    void SkippedEntity(const std::u32string& entityName) override;
private:
    std::unique_ptr<Document> document;
    std::unique_ptr<Element> currentElement;
    std::stack<std::unique_ptr<Element>> elementStack;
    std::u32string textContent;
    void AddTextContent();
    void AddTextContent(bool addSpace);
};

DomDocumentHandler::DomDocumentHandler() : document(new Document())
{
}

std::unique_ptr<Document> DomDocumentHandler::GetDocument()
{
    return std::move(document);
}

void DomDocumentHandler::AddTextContent()
{
    AddTextContent(false);
}

void DomDocumentHandler::AddTextContent(bool addSpace)
{
    if (currentElement)
    {
        std::u32string text = TrimAll(textContent);
        if (!text.empty())
        {
            if (addSpace)
            {
                text.append(1, ' ');
            }
            std::unique_ptr<dom::Text> textNode(new dom::Text(text));
            currentElement->AppendChild(std::move(textNode));
        }
    }
    textContent.clear();
}

void DomDocumentHandler::StartDocument()
{
    // todo
}

void DomDocumentHandler::EndDocument()
{
    // todo
}

void DomDocumentHandler::Version(const std::u32string& xmlVersion)
{
    document->SetXmlVersion(xmlVersion);
}

void DomDocumentHandler::Standalone(bool standalone)
{
    document->SetXmlStandalone(standalone);
}

void DomDocumentHandler::Encoding(const std::u32string& encoding)
{
    document->SetXmlEncoding(encoding);
}

void DomDocumentHandler::Text(const std::u32string& text)
{
    textContent.append(text);
}

void DomDocumentHandler::Comment(const std::u32string& comment)
{
    AddTextContent();
    std::unique_ptr<dom::Comment> commentNode(new dom::Comment(comment));
    if (currentElement)
    {
        currentElement->AppendChild(std::move(commentNode));
    }
    else
    {
        document->AppendChild(std::move(commentNode));
    }
}

void DomDocumentHandler::PI(const std::u32string& target, const std::u32string& data)
{
    AddTextContent();
    std::unique_ptr<dom::ProcessingInstruction> processingInstructionNode(new dom::ProcessingInstruction(target, data));
    if (currentElement)
    {
        currentElement->AppendChild(std::move(processingInstructionNode));
    }
    else
    {
        document->AppendChild(std::move(processingInstructionNode));
    }
}

void DomDocumentHandler::CDataSection(const std::u32string& data)
{
    AddTextContent();
    std::unique_ptr<dom::CDataSection> cdataSection(new dom::CDataSection(data));
    if (currentElement)
    {
        currentElement->AppendChild(std::move(cdataSection));
    }
    else
    {
        document->AppendChild(std::move(cdataSection));
    }
}

void DomDocumentHandler::StartElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName, const Attributes& attributes)
{
    AddTextContent(true);
    elementStack.push(std::move(currentElement));
    std::map<std::u32string, std::unique_ptr<Attr>> attrs;
    for (const Attribute& attr : attributes)
    {
        attrs[attr.QualifiedName()] = std::unique_ptr<Attr>(new Attr(attr.QualifiedName(), attr.Value()));
    }
    currentElement.reset(new Element(qualifiedName, std::move(attrs)));
    if (!namespaceUri.empty())
    {
        currentElement->InternalSetNamespaceUri(namespaceUri);
    }
}

void DomDocumentHandler::EndElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName)
{
    AddTextContent();
    std::unique_ptr<Element> parentElement = std::move(elementStack.top());
    elementStack.pop();
    if (parentElement)
    {
        parentElement->AppendChild(std::move(currentElement));
        currentElement = std::move(parentElement);
    }
    else
    {
        document->AppendChild(std::move(currentElement));
    }
}

void DomDocumentHandler::SkippedEntity(const std::u32string& entityName) 
{
    // todo
}

std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId)
{
    DomDocumentHandler domDocumentHandler;
    ParseXmlContent(content, systemId, &domDocumentHandler);
    return domDocumentHandler.GetDocument();
}

std::unique_ptr<Document> ReadDocument(const std::string& fileName)
{
    std::u32string content = ToUtf32(ReadFile(fileName));
    return ParseDocument(content, fileName);
}

} } // namespace cmajor::dom
