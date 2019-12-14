#ifndef TYPEEXPR_HPP
#define TYPEEXPR_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmparser/TypeExpr.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API TypeExprParser
{
    static std::unique_ptr<sngcm::ast::Node> Parse(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match TypeExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match PrefixTypeExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match PostfixTypeExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match PrimaryTypeExpr(CmajorLexer& lexer, ParsingContext* ctx);
};

#endif // TYPEEXPR_HPP
