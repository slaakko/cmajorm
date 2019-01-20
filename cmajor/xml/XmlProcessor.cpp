// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/xml/XmlProcessor.hpp>
#include <cmajor/util/Util.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace xml {

using namespace cmajor::util;
using namespace cmajor::unicode;

XmlProcessingException::XmlProcessingException(const std::string& message_) : std::runtime_error(message_)
{
}

Entity::~Entity()
{
}

InternalEntity::InternalEntity(const std::u32string& value_) : value(value_)
{
}

XmlProcessor::XmlProcessor(const char32_t* contentStart_, const char32_t* contentEnd_, XmlContentHandler* contentHandler_) : 
    contentStart(contentStart_), contentEnd(contentEnd_), contentHandler(contentHandler_), attValue(nullptr)
{
    entityMap[U"quot"] = std::unique_ptr<Entity>(new InternalEntity(U"\""));
    entityMap[U"amp"] = std::unique_ptr<Entity>(new InternalEntity(U"&"));
    entityMap[U"apos"] = std::unique_ptr<Entity>(new InternalEntity(U"'"));
    entityMap[U"lt"] = std::unique_ptr<Entity>(new InternalEntity(U"<"));
    entityMap[U"gt"] = std::unique_ptr<Entity>(new InternalEntity(U">"));
}

void XmlProcessor::StartDocument()
{
    contentHandler->StartDocument();
}

void XmlProcessor::EndDocument()
{
    contentHandler->EndDocument();
}

void XmlProcessor::Text(const std::u32string& text)
{
    contentHandler->Text(text);
}

void XmlProcessor::Comment(const std::u32string& text)
{
    contentHandler->Comment(text);
}

void XmlProcessor::PI(const std::u32string& target, const std::u32string& data)
{
    contentHandler->PI(target, data);
}

void XmlProcessor::CDataSection(const std::u32string& cdata)
{
    contentHandler->CDataSection(cdata);
}

void XmlProcessor::Version(const std::u32string& version)
{
    contentHandler->Version(version);
}

void XmlProcessor::Standalone(bool standalone)
{
    contentHandler->Standalone(standalone);
}

void XmlProcessor::Encoding(const std::u32string& encoding)
{
    contentHandler->Encoding(encoding);
}

void XmlProcessor::BeginStartTag(const std::u32string& tagName)
{
    tagStack.push(currentTagName);
    currentTagName = tagName;
    namespaceUriStack.push(currentNamespaceUri);
    namespacePrefixStack.push(currentNamespacePrefix);
    attributes.Clear();
}

void XmlProcessor::EndStartTag(const cmajor::parsing::Span& span, const std::string& systemId)
{
    std::u32string localName;
    std::u32string prefix;
    ParseQualifiedName(currentTagName, localName, prefix, span, systemId);
    if (prefix == U"xmlns")
    {
        throw XmlProcessingException(GetErrorLocationStr(systemId, span) + ": 'xmlns' prefix cannot be declared for an element");
    }
    contentHandler->StartElement(GetNamespaceUri(prefix, span, systemId), localName, currentTagName, attributes);
}

void XmlProcessor::EndTag(const std::u32string& tagName, const cmajor::parsing::Span& span, const std::string& systemId)
{
    if (tagStack.empty())
    {
        throw XmlProcessingException(GetErrorLocationStr(systemId, span) + ": end tag '" + ToUtf8(tagName) + "' has no corresponding start tag");
    }
    if (tagName != currentTagName)
    {
        throw XmlProcessingException(GetErrorLocationStr(systemId, span) + ": end tag '" + ToUtf8(tagName) + "' does not match start tag '" + ToUtf8(currentTagName) + "'");
    }
    std::u32string localName;
    std::u32string prefix;
    ParseQualifiedName(currentTagName, localName, prefix, span, systemId);
    if (prefix == U"xmlns")
    {
        throw XmlProcessingException(GetErrorLocationStr(systemId, span) + ": 'xmlns' prefix cannot be declared for an element");
    }
    contentHandler->EndElement(GetNamespaceUri(prefix, span, systemId), localName, currentTagName);
    if (namespaceUriStack.empty())
    {
        throw std::runtime_error("namespace URI stack is empty");
    }
    currentNamespaceUri = namespaceUriStack.top();
    namespaceUriStack.pop();
    namespacePrefixMap.erase(currentNamespacePrefix);
    if (namespacePrefixStack.empty())
    {
        throw std::runtime_error("namespace prefix stack is empty");
    }
    currentNamespacePrefix = namespacePrefixStack.top();
    namespacePrefixStack.pop();
    namespacePrefixMap[currentNamespacePrefix] = currentNamespaceUri;
    currentTagName = tagStack.top();
    tagStack.pop();
}

void XmlProcessor::AddAttribute(const std::u32string& attName, const std::u32string& attValue, const cmajor::parsing::Span& span, const std::string& systemId)
{
    std::u32string localName;
    std::u32string prefix;
    ParseQualifiedName(attName, localName, prefix, span, systemId);
    if (prefix == U"xmlns")
    {
        currentNamespacePrefix = localName;
        currentNamespaceUri = attValue;
        namespacePrefixMap[currentNamespacePrefix] = currentNamespaceUri;
    }
    else if (localName == U"xmlns")
    {
        currentNamespacePrefix.clear();
        currentNamespaceUri = attValue;
    }
    else
    {
        attributes.Add(Attribute(GetNamespaceUri(prefix, span, systemId), localName, attName, attValue));
    }
}

void XmlProcessor::EntityRef(const std::u32string& entityName, const cmajor::parsing::Span& span, const std::string& systemId)
{
    auto it = entityMap.find(entityName);
    if (it != entityMap.cend())
    {
        const std::unique_ptr<Entity>& entity = it->second;
        if (entity->IsInternalEntity())
        {
            InternalEntity* internalEntity = static_cast<InternalEntity*>(entity.get());
            const std::u32string& entityValue = internalEntity->Value();
            if (attValue)
            {
                attValue->append(entityValue);
            }
            else
            {
                Text(entityValue);
            }
        }
        else
        {
            contentHandler->SkippedEntity(entityName);
        }
    }
    else
    {
        contentHandler->SkippedEntity(entityName);
    }
}

void XmlProcessor::BeginAttributeValue(std::u32string* attValue_)
{
    attValueStack.push(attValue);
    attValue = attValue_;
}

void XmlProcessor::EndAttributeValue()
{
    if (attValueStack.empty())
    {
        throw std::runtime_error("attribute value stack is empty");
    }
    attValue = attValueStack.top();
    attValueStack.pop();
}

std::u32string XmlProcessor::GetNamespaceUri(const std::u32string& namespacePrefix, const cmajor::parsing::Span& span, const std::string& systemId)
{
    if (namespacePrefix.empty())
    {
        return currentNamespaceUri;
    }
    auto it = namespacePrefixMap.find(namespacePrefix);
    if (it != namespacePrefixMap.cend())
    {
        return it->second;
    }
    else
    {
        throw XmlProcessingException(GetErrorLocationStr(systemId, span) + ": namespace prefix '" + ToUtf8(namespacePrefix) + "' not bound to any namespace URI");
    }
}

void XmlProcessor::ParseQualifiedName(const std::u32string& qualifiedName, std::u32string& localName, std::u32string& prefix, const cmajor::parsing::Span& span, const std::string& systemId)
{
    std::vector<std::u32string> parts = Split(qualifiedName, ':');
    if (parts.size() > 2)
    {
        throw XmlProcessingException(GetErrorLocationStr(systemId, span) + ": qualified name '" + ToUtf8(qualifiedName) + "' has more than one ':' character");
    }
    else if (parts.size() == 2)
    {
        prefix = parts[0];
        localName = parts[1];
    }
    else
    {
        prefix.clear();
        localName = qualifiedName;
    }
}

int XmlProcessor::GetErrorColumn(int index) const
{
    int errorColumn = 0;
    while (index > 0 && contentStart[index] != '\n' && contentStart[index] != '\r')
    {
        ++errorColumn;
        --index;
    }
    if (errorColumn == 0)
    {
        errorColumn = 1;
    }
    return errorColumn;
}

std::string XmlProcessor::GetErrorLocationStr(const std::string& systemId, const cmajor::parsing::Span& span) const
{
    int errorColumn = GetErrorColumn(span.Start());
    return "error in '" + systemId + "' at line " + std::to_string(span.LineNumber()) + " column " + std::to_string(errorColumn) + " (index " + std::to_string(span.Start()) + ")";
}

} }   // namespace cmajor::xml
