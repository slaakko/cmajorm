// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xml/XmlParserInterface.hpp>
#include <sngxml/xml/XmlParser.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <soulng/lexer/XmlParsingLog.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <iostream>

namespace sngxml { namespace xml {

using namespace soulng::util;
using namespace soulng::unicode;

void ParseXmlFile(const std::string& xmlFileName, XmlContentHandler* contentHandler)
{
    ParseXmlFile(xmlFileName, contentHandler, Flags::none);
}

void ParseXmlFile(const std::string& xmlFileName, XmlContentHandler* contentHandler, Flags flags)
{
    std::string xmlContent = ReadFile(xmlFileName);
    ParseXmlContent(xmlContent, xmlFileName, contentHandler, flags);
}

void ParseXmlContent(const std::string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler)
{
    ParseXmlContent(ToUtf32(xmlContent), systemId, contentHandler, Flags::none);
}

void ParseXmlContent(const std::string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler, Flags flags)
{
    ParseXmlContent(ToUtf32(xmlContent), systemId, contentHandler, flags);
}

void ParseXmlContent(const std::u32string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler)
{
    ParseXmlContent(xmlContent, systemId, contentHandler, Flags::none);
}

void ParseXmlContent(const std::u32string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler, Flags flags)
{
    soulng::lexer::XmlParsingLog debugLog(std::cerr);
    TrivialLexer xmlLexer(xmlContent, systemId, 0);
    if ((flags & Flags::debug) != Flags::none)
    {
        xmlLexer.SetLog(&debugLog);
    }
    XmlProcessor xmlProcessor(xmlLexer, contentHandler);
    XmlParser::Parse(xmlLexer, &xmlProcessor);
}

} } // namespace sngxml::xml
