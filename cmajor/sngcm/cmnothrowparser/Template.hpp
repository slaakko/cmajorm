#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Template.hpp>
#include <sngcm/cmnothrowparser/Identifier.hpp>
#include <sngcm/cmnothrowparser/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Template.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowTemplateParser
{
    static soulng::parser::Match TemplateId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match TemplateParameter(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match TemplateParameterList(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::Node* owner);
    static soulng::parser::Match InstantiationRequest(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
};

#endif // TEMPLATE_HPP
