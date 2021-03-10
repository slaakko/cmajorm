#ifndef XMLSERPARSER_HPP
#define XMLSERPARSER_HPP
#include <sngxml/serialization/XmlSerLexer.hpp>
#include <sngxml/serialization/XmlSerAst.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngxml/serialization/XmlSerParser.parser' using soulng parser generator spg version 3.10.0

class XmlSerLexer;

struct SNGXML_SERIALIZATION_API XmlSerParser
{
    static std::unique_ptr<sngxml::xmlser::SourceFileNode> Parse(XmlSerLexer& lexer);
    static soulng::parser::Match SourceFile(XmlSerLexer& lexer);
    static soulng::parser::Match IncludeDirective(XmlSerLexer& lexer);
    static soulng::parser::Match NamespaceContent(XmlSerLexer& lexer, sngxml::xmlser::NamespaceNode* ownerNs);
    static soulng::parser::Match Definition(XmlSerLexer& lexer);
    static soulng::parser::Match Namespace(XmlSerLexer& lexer);
    static soulng::parser::Match Class(XmlSerLexer& lexer);
    static soulng::parser::Match Api(XmlSerLexer& lexer);
    static soulng::parser::Match ClassKey(XmlSerLexer& lexer);
    static soulng::parser::Match Inheritance(XmlSerLexer& lexer);
    static soulng::parser::Match ClassContent(XmlSerLexer& lexer, sngxml::xmlser::ClassNode* classNode);
    static soulng::parser::Match MemberVariable(XmlSerLexer& lexer);
    static soulng::parser::Match Type(XmlSerLexer& lexer);
    static soulng::parser::Match ScalarType(XmlSerLexer& lexer);
    static soulng::parser::Match ClassType(XmlSerLexer& lexer);
};

#endif // XMLSERPARSER_HPP
