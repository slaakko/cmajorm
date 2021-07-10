#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Interface.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <sngcm/ast/Class.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Interface.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowInterfaceParser
{
    static soulng::parser::Match Interface(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match DefiningInterfaceId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match InterfaceContent(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::InterfaceNode* intf);
    static soulng::parser::Match InterfaceMemFun(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match InterfaceFunctionGroupId(CmajorNothrowLexer& lexer);
};

#endif // INTERFACE_HPP
