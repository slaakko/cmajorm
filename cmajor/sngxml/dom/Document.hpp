// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_DOM_DOCUMENT_INCLUDED
#define SNGXML_DOM_DOCUMENT_INCLUDED
#include <sngxml/dom/Node.hpp>
#include <unordered_map>

namespace sngxml { namespace dom {

class Element;
class DocumentType;

class SNGXML_DOM_API Document : public ParentNode
{
public:
    Document();
    Document(const Document&) = delete;
    Document& operator=(const Document&) = delete;
    Document(Document&&) = delete;
    Document& operator=(Document&&) = delete;
    std::unique_ptr<Node> CloneNode(bool deep) override;
    Element* DocumentElement() { return documentElement; }
    void Write(CodeFormatter& formatter) override;
    Node* InsertBefore(std::unique_ptr<Node>&& newChild, Node* refChild) override;
    std::unique_ptr<Node> ReplaceChild(std::unique_ptr<Node>&& newChild, Node* oldChild) override;
    std::unique_ptr<Node> RemoveChild(Node* oldChild) override;
    Node* AppendChild(std::unique_ptr<Node>&& newChild) override;
    Element* GetElementById(const std::u32string& elementId);
    void SetXmlStandalone(bool xmlStandalone_) { xmlStandalone = xmlStandalone_; }
    bool XmlStandalone() const { return xmlStandalone; }
    void SetXmlVersion(const std::u32string& xmlVersion_) { xmlVersion = xmlVersion_; }
    const std::u32string& XmlVersion() const { return xmlVersion; }
    void SetXmlEncoding(const std::u32string& xmlEncoding_) { xmlEncoding = xmlEncoding_; }
    const std::u32string& XmlEncoding() const { return xmlEncoding; }
    void Accept(Visitor& visitor) override;
    void InternalInvalidateIndex();
private:
    Element* documentElement;
    DocumentType* docType;
    void CheckValidInsert(Node* node, Node* refNode);
    std::unordered_map<std::u32string, Element*> elementsByIdMap;
    bool indexValid;
    bool xmlStandalone;
    std::u32string xmlVersion;
    std::u32string xmlEncoding;
};

} } // namespace sngxml::dom

#endif // SNGXML_DOM_DOCUMENT_INCLUDED

