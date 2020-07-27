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
    static soulng::parser::Match AdditiveDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match PrefixDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match PostfixDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match PrimaryDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match CastDebugExpr(DebugExprLexer& lexer);
    static soulng::parser::Match TypeIdExpr(DebugExprLexer& lexer);
};

#endif // DEBUGEXPRPARSER_HPP
