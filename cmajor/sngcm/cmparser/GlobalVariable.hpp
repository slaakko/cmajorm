#ifndef GLOBALVARIABLE_HPP
#define GLOBALVARIABLE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/GlobalVariable.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/GlobalVariable.parser' using soulng parser generator spg version 3.10.0

class CmajorLexer;

struct SNGCM_PARSER_API GlobalVariableParser
{
    static soulng::parser::Match GlobalVariable(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::CompileUnitNode* cu);
};

#endif // GLOBALVARIABLE_HPP
