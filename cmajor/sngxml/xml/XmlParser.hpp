#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP
#include <sngxml/xml/XmlProcessor.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngxml/xml/XmlParser.parser' using soulng parser generator spg version 4.0.0

class TrivialLexer;

struct SNGXML_XML_API XmlParser
{
    static void Parse(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match Document(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match Char(TrivialLexer& lexer);
    static soulng::parser::Match S(TrivialLexer& lexer);
    static soulng::parser::Match NameStartChar(TrivialLexer& lexer);
    static soulng::parser::Match NameChar(TrivialLexer& lexer);
    static soulng::parser::Match Name(TrivialLexer& lexer);
    static soulng::parser::Match Names(TrivialLexer& lexer);
    static soulng::parser::Match Nmtoken(TrivialLexer& lexer);
    static soulng::parser::Match Nmtokens(TrivialLexer& lexer);
    static soulng::parser::Match Prolog(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match XMLDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match VersionInfo(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match VersionNum(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match VersionNumDQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match VersionNumSQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match VersionNumber(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EncodingDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EncodingName(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EncNameDQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EncNameSQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EncName(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match SDDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match DocTypeDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match ExternalID(TrivialLexer& lexer);
    static soulng::parser::Match SystemLiteral(TrivialLexer& lexer);
    static soulng::parser::Match PubidLiteral(TrivialLexer& lexer);
    static soulng::parser::Match PubidChar(TrivialLexer& lexer);
    static soulng::parser::Match IntSubset(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match MarkupDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match DeclSep(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match ElementDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match ContentSpec(TrivialLexer& lexer);
    static soulng::parser::Match Children(TrivialLexer& lexer);
    static soulng::parser::Match CP(TrivialLexer& lexer);
    static soulng::parser::Match Choice(TrivialLexer& lexer);
    static soulng::parser::Match Seq(TrivialLexer& lexer);
    static soulng::parser::Match Mixed(TrivialLexer& lexer);
    static soulng::parser::Match AttlistDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match AttDef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match AttType(TrivialLexer& lexer);
    static soulng::parser::Match StringType(TrivialLexer& lexer);
    static soulng::parser::Match TokenizedType(TrivialLexer& lexer);
    static soulng::parser::Match EnumeratedType(TrivialLexer& lexer);
    static soulng::parser::Match NotationType(TrivialLexer& lexer);
    static soulng::parser::Match Enumeration(TrivialLexer& lexer);
    static soulng::parser::Match DefaultDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EntityDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match GEDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match PEDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EntityDef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match PEDef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EntityValue(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match NDataDecl(TrivialLexer& lexer);
    static soulng::parser::Match PEReference(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match NotationDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match PublicID(TrivialLexer& lexer);
    static soulng::parser::Match Element(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match ETag(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match Content(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match CharDataChar(TrivialLexer& lexer);
    static soulng::parser::Match CharData(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match CDSect(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match Attribute(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match AttValueDQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match AttValueSQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match AttValue(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match EntityRef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match DecCodePoint(TrivialLexer& lexer);
    static soulng::parser::Match HexCodePoint(TrivialLexer& lexer);
    static soulng::parser::Match CharRef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match Reference(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match Misc(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match Comment(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match PI(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor);
    static soulng::parser::Match PITarget(TrivialLexer& lexer);
    static soulng::parser::Match Xml(TrivialLexer& lexer);
    static soulng::parser::Match Eq(TrivialLexer& lexer);
    static soulng::parser::Match YesNo(TrivialLexer& lexer);
};

#endif // XMLPARSER_HPP
