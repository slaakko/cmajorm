#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngcm/cmparser/Expression.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API ExpressionParser
{
    static soulng::parser::Match Expression(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Equivalence(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Implication(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Disjunction(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Conjunction(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match BitOr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match BitXor(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match BitAnd(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Equality(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Relational(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Shift(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Additive(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Multiplicative(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Prefix(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Postfix(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Primary(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match SizeOfExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match TypeNameExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match TypeIdExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match CastExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ConstructExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match NewExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ArgumentList(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::Node* node);
    static soulng::parser::Match ExpressionList(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::Node* node);
    static soulng::parser::Match InvokeExpr(CmajorLexer& lexer, ParsingContext* ctx);
};

#endif // EXPRESSION_HPP
