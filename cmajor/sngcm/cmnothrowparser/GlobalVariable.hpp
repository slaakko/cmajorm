#ifndef GLOBALVARIABLE_HPP
#define GLOBALVARIABLE_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/GlobalVariable.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/GlobalVariable.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowGlobalVariableParser
{
    static soulng::parser::Match GlobalVariable(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::CompileUnitNode* cu);
    static soulng::parser::Match DefininigGlobalVariableId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
};

#endif // GLOBALVARIABLE_HPP
