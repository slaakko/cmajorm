#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Attribute.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmparser/Attribute.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API AttributeParser
{
    static soulng::parser::Match Attributes(CmajorLexer& lexer);
    static soulng::parser::Match Attribute(CmajorLexer& lexer, sngcm::ast::Attributes* attributes);
};

#endif // ATTRIBUTE_HPP
