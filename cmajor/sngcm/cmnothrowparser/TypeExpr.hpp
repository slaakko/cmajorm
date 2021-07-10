#ifndef TYPEEXPR_HPP
#define TYPEEXPR_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/TypeExpr.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowTypeExprParser
{
    static std::unique_ptr<sngcm::ast::Node> Parse(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match TypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match PrefixTypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match PostfixTypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match PrimaryTypeExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
};

#endif // TYPEEXPR_HPP
