// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/xml/XmlParser.hpp>
#include <cmajor/xml/XmlGrammar.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/MappedInputFile.hpp>

namespace cmajor { namespace xml {

using namespace cmajor::util;
using namespace cmajor::unicode;

void ParseXmlFile(const std::string& xmlFileName, XmlContentHandler* contentHandler)
{
    std::string xmlContent = ReadFile(xmlFileName);
    ParseXmlContent(xmlContent, xmlFileName, contentHandler);
}

void ParseXmlContent(const std::string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler)
{
    ParseXmlContent(ToUtf32(xmlContent), systemId, contentHandler);
}

Xml* xmlGrammar = nullptr;

void ParseXmlContent(const std::u32string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler)
{
    const char32_t* contentStart = &xmlContent[0];
    const char32_t* contentEnd = &xmlContent[0] + xmlContent.length();
    XmlProcessor xmlProcessor(contentStart, contentEnd, contentHandler);
    if (xmlGrammar == nullptr)
    {
        xmlGrammar = Xml::Create();
    }
    xmlGrammar->Parse(contentStart, contentEnd, 0, systemId, &xmlProcessor);
}

} } // namespace cmajor::xml
