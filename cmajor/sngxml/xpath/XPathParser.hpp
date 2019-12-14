#ifndef XPATHPARSER_HPP
#define XPATHPARSER_HPP
#include <sngxml/xpath/XPathExpr.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngxml/xpath/XPathParser.parser' using soulng parser generator spg version 3.0.0

class XPathLexer;

struct XPathParser
{
    static std::unique_ptr<sngxml::xpath::XPathExpr> Parse(XPathLexer& lexer);
    static soulng::parser::Match Expr(XPathLexer& lexer);
    static soulng::parser::Match OrExpr(XPathLexer& lexer);
    static soulng::parser::Match AndExpr(XPathLexer& lexer);
    static soulng::parser::Match EqualityExpr(XPathLexer& lexer);
    static soulng::parser::Match RelationalExpr(XPathLexer& lexer);
    static soulng::parser::Match AdditiveExpr(XPathLexer& lexer);
    static soulng::parser::Match MultiplicativeExpr(XPathLexer& lexer);
    static soulng::parser::Match UnaryExpr(XPathLexer& lexer);
    static soulng::parser::Match UnionExpr(XPathLexer& lexer);
    static soulng::parser::Match PathExpr(XPathLexer& lexer);
    static soulng::parser::Match FilterExpr(XPathLexer& lexer);
    static soulng::parser::Match LocationPath(XPathLexer& lexer);
    static soulng::parser::Match AbsoluteLocationPath(XPathLexer& lexer);
    static soulng::parser::Match AbbreviatedAbsoluteLocationPath(XPathLexer& lexer);
    static soulng::parser::Match RelativeLocationPath(XPathLexer& lexer);
    static soulng::parser::Match Step(XPathLexer& lexer);
    static soulng::parser::Match AxisSpecifier(XPathLexer& lexer);
    static soulng::parser::Match AxisName(XPathLexer& lexer);
    static soulng::parser::Match AbbreviatedAxisSpecifier(XPathLexer& lexer);
    static soulng::parser::Match NodeTest(XPathLexer& lexer);
    static soulng::parser::Match NodeType(XPathLexer& lexer);
    static soulng::parser::Match NameTest(XPathLexer& lexer);
    static soulng::parser::Match AbbreviatedStep(XPathLexer& lexer);
    static soulng::parser::Match Literal(XPathLexer& lexer);
    static soulng::parser::Match Number(XPathLexer& lexer);
    static soulng::parser::Match Predicate(XPathLexer& lexer);
    static soulng::parser::Match PredicateExpr(XPathLexer& lexer);
    static soulng::parser::Match PrimaryExpr(XPathLexer& lexer);
    static soulng::parser::Match VariableReference(XPathLexer& lexer);
    static soulng::parser::Match FunctionCall(XPathLexer& lexer);
    static soulng::parser::Match FunctionName(XPathLexer& lexer);
    static soulng::parser::Match Argument(XPathLexer& lexer);
    static soulng::parser::Match QName(XPathLexer& lexer);
    static soulng::parser::Match PrefixedName(XPathLexer& lexer);
    static soulng::parser::Match Prefix(XPathLexer& lexer);
    static soulng::parser::Match UnprefixedName(XPathLexer& lexer);
    static soulng::parser::Match LocalPart(XPathLexer& lexer);
    static soulng::parser::Match NCName(XPathLexer& lexer);
};

#endif // XPATHPARSER_HPP
