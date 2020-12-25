#ifndef TRACEPARSER_HPP
#define TRACEPARSER_HPP
#include <cmajor/cmtrace/TraceExpr.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/cmtrace/TraceParser.parser' using soulng parser generator spg version 3.0.0

class TraceLexer;

struct TraceParser
{
    static std::unique_ptr<cmtrace::TraceExpr> Parse(TraceLexer& lexer);
    static soulng::parser::Match Expr(TraceLexer& lexer);
    static soulng::parser::Match OrExpr(TraceLexer& lexer);
    static soulng::parser::Match BooleanExpr(TraceLexer& lexer);
    static soulng::parser::Match MatchExpr(TraceLexer& lexer);
    static soulng::parser::Match RelationalExpr(TraceLexer& lexer);
    static soulng::parser::Match RelOp(TraceLexer& lexer);
    static soulng::parser::Match PrimaryExpr(TraceLexer& lexer);
    static soulng::parser::Match Unit(TraceLexer& lexer);
};

#endif // TRACEPARSER_HPP
