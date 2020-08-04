// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngxml/xml/XmlParserInterface.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <string>
#include <stack>
#include <sstream>

namespace sngxml { namespace dom {

using namespace sngxml::xml;
using namespace soulng::util;
using namespace soulng::unicode;

class SNGXML_DOM_API DomDocumentHandler : public XmlContentHandler
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
    currentElement->InternalSetOwnerDocument(document.get());
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
    return ParseDocument(content, systemId, Flags::none);
}

std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId, Flags flags)
{
    DomDocumentHandler domDocumentHandler;
    sngxml::xml::Flags xmlFlags = sngxml::xml::Flags::none;
    if ((flags & Flags::debug) != Flags::none)
    {
        xmlFlags = xmlFlags | sngxml::xml::Flags::debug;
    }
    ParseXmlContent(content, systemId, &domDocumentHandler, xmlFlags);
    return domDocumentHandler.GetDocument();
}

std::unique_ptr<Document> ReadDocument(const std::string& fileName)
{
    return ReadDocument(fileName, Flags::none);
}

std::unique_ptr<Document> ReadDocument(const std::string& fileName, Flags flags)
{
    std::u32string content = ToUtf32(ReadFile(fileName));
    return ParseDocument(content, fileName, flags);
}

void SendDocument(soulng::util::TcpSocket& socket, Document& document)
{
    std::stringstream sstream;
    CodeFormatter formatter(sstream);
    document.Write(formatter);
    Write(socket, sstream.str());
}

std::unique_ptr<Document> ReceiveDocument(soulng::util::TcpSocket& socket)
{
    std::string str = ReadStr(socket);
    if (str.empty()) return std::unique_ptr<Document>();
    std::u32string content = ToUtf32(str);
    std::unique_ptr<Document> doc = ParseDocument(content, "socket");
    return doc;
}

} } // namespace sngxml::dom
