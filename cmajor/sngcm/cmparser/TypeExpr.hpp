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

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/TypeExpr.parser' using soulng parser generator spg version 3.10.0

class CmajorLexer;

struct SNGCM_PARSER_API TypeExprParser
{
    static std::unique_ptr<sngcm::ast::Node> Parse(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match TypeExpr(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match PrefixTypeExpr(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match PostfixTypeExpr(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match PrimaryTypeExpr(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
};

#endif // TYPEEXPR_HPP
