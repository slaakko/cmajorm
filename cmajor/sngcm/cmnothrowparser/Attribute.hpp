#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Attribute.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Attribute.parser' using soulng parser generator spg version 3.10.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowAttributeParser
{
    static soulng::parser::Match Attributes(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match Attribute(CmajorNothrowLexer& lexer, sngcm::ast::AttributesNode* attributes, boost::uuids::uuid* moduleId);
};

#endif // ATTRIBUTE_HPP
