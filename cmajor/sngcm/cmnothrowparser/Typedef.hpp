#ifndef TYPEDEF_HPP
#define TYPEDEF_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Typedef.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Typedef.parser' using soulng parser generator spg version 3.10.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowTypedefParser
{
    static soulng::parser::Match Typedef(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
};

#endif // TYPEDEF_HPP
