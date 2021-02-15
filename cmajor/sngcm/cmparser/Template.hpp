#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Template.hpp>
#include <sngcm/cmparser/Identifier.hpp>
#include <sngcm/cmparser/TypeExpr.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/Template.parser' using soulng parser generator spg version 3.10.0

class CmajorLexer;

struct SNGCM_PARSER_API TemplateParser
{
    static soulng::parser::Match TemplateId(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match TemplateParameter(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match TemplateParameterList(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::Node* owner);
};

#endif // TEMPLATE_HPP
