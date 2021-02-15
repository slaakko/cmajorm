#ifndef FUNCTION_HPP
#define FUNCTION_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Function.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/Function.parser' using soulng parser generator spg version 3.10.0

class CmajorLexer;

struct SNGCM_PARSER_API FunctionParser
{
    static soulng::parser::Match Function(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match FunctionGroupId(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match OperatorFunctionGroupId(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
};

#endif // FUNCTION_HPP
