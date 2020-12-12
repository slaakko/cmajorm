#ifndef SPECIFIER_HPP
#define SPECIFIER_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Specifier.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/Specifier.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API SpecifierParser
{
    static soulng::parser::Match Specifiers(CmajorLexer& lexer);
    static soulng::parser::Match Specifier(CmajorLexer& lexer);
};

#endif // SPECIFIER_HPP
