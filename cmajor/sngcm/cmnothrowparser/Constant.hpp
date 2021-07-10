#ifndef CONSTANT_HPP
#define CONSTANT_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Constant.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Constant.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowConstantParser
{
    static soulng::parser::Match Constant(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
};

#endif // CONSTANT_HPP
