#ifndef ENUMERATION_HPP
#define ENUMERATION_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Enumeration.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngcm/cmparser/Enumeration.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API EnumerationParser
{
    static soulng::parser::Match EnumType(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match UnderlyingType(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match EnumConstants(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::EnumTypeNode* enumType);
    static soulng::parser::Match EnumConstant(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::EnumTypeNode* enumType);
};

#endif // ENUMERATION_HPP
