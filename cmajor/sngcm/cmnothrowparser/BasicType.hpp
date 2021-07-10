#ifndef BASICTYPE_HPP
#define BASICTYPE_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/BasicType.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowBasicTypeParser
{
    static soulng::parser::Match BasicType(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
};

#endif // BASICTYPE_HPP
