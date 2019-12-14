// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_XML_CONTENT_HANDLER
#define SNGXML_XML_XML_CONTENT_HANDLER
#include <sngxml/xml/XmlApi.hpp>
#include <string>
#include <vector>

namespace sngxml { namespace xml {

class SNGXML_XML_API Attribute
{
public:
    Attribute(const std::u32string& namespaceUri_, const std::u32string& localName_, const std::u32string& qualifiedName_, const std::u32string& value_);
    const std::u32string& NamespaceUri() const { return namespaceUri; }
    const std::u32string& LocalName() const { return localName; }
    const std::u32string& QualifiedName() const { return qualifiedName; }
    const std::u32string& Value() const { return value; }
private:
    std::u32string namespaceUri;
    std::u32string localName;
    std::u32string qualifiedName;
    std::u32string value;
};

class SNGXML_XML_API Attributes
{
public:
    typedef std::vector<Attribute>::const_iterator const_iterator;
    const_iterator begin() const { return attributes.begin(); }
    const_iterator end() const { return attributes.end(); }
    const_iterator cbegin() const { return attributes.cbegin(); }
    const_iterator cend() const { return attributes.cend(); }
    void Add(const Attribute& attribute);
    void Clear();
    const std::u32string* GetAttributeValue(const std::u32string& namespaceUri, const std::u32string& localName) const;
    const std::u32string* GetAttributeValue(const std::u32string& qualifiedName) const;
private:
    std::vector<Attribute> attributes;
};

class SNGXML_XML_API XmlContentHandler
{
public:
    virtual ~XmlContentHandler();
    virtual void StartDocument() {}
    virtual void EndDocument() {}
    virtual void Version(const std::u32string& xmlVersion) {}
    virtual void Standalone(bool standalone) {}
    virtual void Encoding(const std::u32string& encoding) {}
    virtual void Text(const std::u32string& text) {}
    virtual void Comment(const std::u32string& comment) {}
    virtual void PI(const std::u32string& target, const std::u32string& data) {}
    virtual void CDataSection(const std::u32string& cdata) {}
    virtual void StartElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName, const Attributes& attributes) {}
    virtual void EndElement(const std::u32string& namespaceUri, const std::u32string& localName, const std::u32string& qualifiedName) {}
    virtual void SkippedEntity(const std::u32string& entityName) {}
};

} } // namespace sngxml::xml

#endif // SNGXML_XML_XML_CONTENT_HANDLER

