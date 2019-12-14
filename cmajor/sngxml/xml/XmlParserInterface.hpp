// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

// ==========================================================
// Interface to a non-validating XML parser with SAX-like API
// Author: S. Laakko
// ==========================================================

#ifndef SNGXML_XML_XML_PARSER
#define SNGXML_XML_XML_PARSER
#include <sngxml/xml/XmlContentHandler.hpp>

namespace sngxml { namespace xml {

enum class Flags : int
{
    none = 0, debug = 1 << 0
};

inline Flags operator&(Flags flags, Flags flag)
{
    return Flags(int(flags) & int(flag));
}

inline Flags operator|(Flags flags, Flags flag)
{
    return Flags(int(flags) | int(flag));
}

inline Flags operator~(Flags flags)
{
    return Flags(~int(flags));
}

//  ==================================================================================
//  ParseXmlFile parses given UTF-8 encoded XML file using given content handler.
//  ==================================================================================

SNGXML_XML_API void ParseXmlFile(const std::string& xmlFileName, XmlContentHandler* contentHandler);
SNGXML_XML_API void ParseXmlFile(const std::string& xmlFileName, XmlContentHandler* contentHandler, Flags flags);

//  ==================================================================================
//  ParseXmlContent parses given UTF-8 encoded XML string using given content handler.
//  systemId is used for error messages only. It can be for example a file name or URL 
//  that identifies the XML string to parse.
//  ===================================================================================

SNGXML_XML_API void ParseXmlContent(const std::string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler);
SNGXML_XML_API void ParseXmlContent(const std::string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler, Flags flags);

//  ===================================================================================
//  ParseXmlContent parses given UTF-32 encoded XML string using given content handler.
//  systemId is used for error messages only. It can be for example a file name or URL 
//  that identifies the XML string to parse.
//  ===================================================================================

SNGXML_XML_API void ParseXmlContent(const std::u32string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler);
SNGXML_XML_API void ParseXmlContent(const std::u32string& xmlContent, const std::string& systemId, XmlContentHandler* contentHandler, Flags flags);

} } // namespace sngxml::xml

#endif // SNGXML_XML_XML_PARSER
