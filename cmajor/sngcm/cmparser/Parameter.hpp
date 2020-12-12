#ifndef PARAMETER_HPP
#define PARAMETER_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Parameter.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/Parameter.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API ParameterParser
{
    static soulng::parser::Match ParameterList(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::Node* owner);
    static soulng::parser::Match Parameter(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
};

#endif // PARAMETER_HPP
