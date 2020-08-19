#ifndef DEBUGEXPRPARSER_HPP
#define DEBUGEXPRPARSER_HPP
#include <cmajor/cmdebug/DebugExpr.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/DebugExprParser.parser' using soulng parser generator spg version 3.0.0

class DebugExprLexer;

struct DEBUG_API DebugExprParser
{
    static std::unique_ptr<cmajor::debug::DebugExprNode> Parse(DebugExprLexer& lexer);
    static soulng::parser::Match DebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match DisjunctiveDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match ConjunctiveDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match BitOrDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match BitXorDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match BitAndDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match EqualityDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match RelationalDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match ShiftDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match AdditiveDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match MultiplicativeDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match PrefixDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match PostfixDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match PrimaryDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match CastDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match TypeIdExpr(DebugExprLexer& lexer);
};

#endif // DEBUGEXPRPARSER_HPP
