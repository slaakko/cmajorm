// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_XML_PROCESSOR
#define SNGXML_XML_XML_PROCESSOR
#include <sngxml/xml/XmlContentHandler.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <stack>
#include <memory>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace sngxml { namespace xml {

class SNGXML_XML_API XmlProcessingException : public std::runtime_error
{
public:
    XmlProcessingException(const std::string& message);
};

class SNGXML_XML_API Entity
{
public:
    virtual ~Entity();
    virtual bool IsInternalEntity() const { return false; }
};

class SNGXML_XML_API InternalEntity : public Entity
{
public:
    InternalEntity(const std::u32string& value_);
    const std::u32string& Value() const { return value; }
    bool IsInternalEntity() const override { return true; }
private:
    std::u32string value;
};

class SNGXML_XML_API XmlProcessor
{
public:
    XmlProcessor(const XmlProcessor&) = delete;
    XmlProcessor& operator=(const XmlProcessor&) = delete;
    XmlProcessor(XmlProcessor&&) = delete;
    XmlProcessor&& operator=(XmlProcessor&&) = delete;
    XmlProcessor(TrivialLexer& lexer_, XmlContentHandler* contentHandler_);
    void StartDocument();
    void EndDocument();
    void Text(const std::u32string& text);
    void Comment(const std::u32string& text);
    void PI(const std::u32string& target, const std::u32string& data);
    void CDataSection(const std::u32string& cdata);
    void Version(const std::u32string& version);
    void Standalone(bool standalone);
    void Encoding(const std::u32string& encoding);
    void BeginStartTag(const std::u32string& tagName);
    void EndStartTag(const soulng::lexer::Span& span, const std::string& systemId);
    void EndTag(const std::u32string& tagName, const soulng::lexer::Span& span, const std::string& systemId);
    void AddAttribute(const std::u32string& attName, const std::u32string& attValue, const soulng::lexer::Span& span, const std::string& systemId);
    void EntityRef(const std::u32string& entityName, const soulng::lexer::Span& span, const std::string& systemId);
    void BeginAttributeValue();
    void EndAttributeValue();
    const std::u32string& AttValue() const { return attValue; }
    std::u32string& AttValue() { return attValue; }
    TrivialLexer& Lexer() const { return lexer; }
private:
    TrivialLexer& lexer;
    XmlContentHandler* contentHandler;
    std::stack<std::u32string> tagStack;
    std::u32string currentTagName;
    std::stack<std::u32string> attValueStack;
    std::u32string attValue;
    std::stack<std::u32string> namespaceUriStack;
    std::u32string currentNamespaceUri;
    std::stack<std::u32string> namespacePrefixStack;
    std::u32string currentNamespacePrefix;
    std::unordered_map<std::u32string, std::u32string> namespacePrefixMap;
    Attributes attributes;
    std::unordered_map<std::u32string, std::unique_ptr<Entity>> entityMap;
    int GetErrorColumn(int index) const;
    std::string GetErrorLocationStr(const std::string& systemId, const soulng::lexer::Span& span) const;
    void ParseQualifiedName(const std::u32string& qualifiedName, std::u32string& localName, std::u32string& prefix, const soulng::lexer::Span& span, const std::string& systemId);
    std::u32string GetNamespaceUri(const std::u32string& namespacePrefix, const soulng::lexer::Span& span, const std::string& systemId);
};

} } // namespace sngxml::xml

#endif // SNGXML_XML_XML_PROCESSOR

