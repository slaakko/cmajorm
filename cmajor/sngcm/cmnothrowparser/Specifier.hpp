#ifndef SPECIFIER_HPP
#define SPECIFIER_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Specifier.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Specifier.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowSpecifierParser
{
    static soulng::parser::Match Specifiers(CmajorNothrowLexer& lexer);
    static soulng::parser::Match Specifier(CmajorNothrowLexer& lexer);
};

#endif // SPECIFIER_HPP
