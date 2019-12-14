// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xml/XmlContentHandler.hpp>

namespace sngxml { namespace xml {

Attribute::Attribute(const std::u32string& namespaceUri_, const std::u32string& localName_, const std::u32string& qualifiedName_, const std::u32string& value_) :
    namespaceUri(namespaceUri_), localName(localName_), qualifiedName(qualifiedName_), value(value_)
{
}

void Attributes::Add(const Attribute& attribute)
{
    attributes.push_back(attribute);
}

void Attributes::Clear()
{
    attributes.clear();
}

const std::u32string* Attributes::GetAttributeValue(const std::u32string& namespaceUri, const std::u32string& localName) const
{
    for (const Attribute& attribute : attributes)
    {
        if (attribute.NamespaceUri() == namespaceUri && attribute.LocalName() == localName)
        {
            return &attribute.Value();
        }
    }
    return nullptr;
}

const std::u32string* Attributes::GetAttributeValue(const std::u32string& qualifiedName) const
{
    for (const Attribute& attribute : attributes)
    {
        if (attribute.QualifiedName() == qualifiedName)
        {
            return &attribute.Value();
        }
    }
    return nullptr;
}

XmlContentHandler::~XmlContentHandler()
{
}

} } // namespace sngxml::xml
