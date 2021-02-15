#ifndef LITERAL_HPP
#define LITERAL_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/Literal.parser' using soulng parser generator spg version 3.10.0

class CmajorLexer;

struct SNGCM_PARSER_API LiteralParser
{
    static soulng::parser::Match Literal(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match SimpleLiteral(CmajorLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match ComplexLiteral(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match ArrayLiteral(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match StructuredLiteral(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
};

#endif // LITERAL_HPP
