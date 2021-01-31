#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Identifier.parser' using soulng parser generator spg version 3.10.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowIdentifierParser
{
    static soulng::parser::Match Identifier(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match QualifiedId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
};

#endif // IDENTIFIER_HPP
