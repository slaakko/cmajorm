#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Interface.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <sngcm/ast/Class.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngcm/cmparser/Interface.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API InterfaceParser
{
    static soulng::parser::Match Interface(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match InterfaceContent(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::InterfaceNode* intf);
    static soulng::parser::Match InterfaceMemFun(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match InterfaceFunctionGroupId(CmajorLexer& lexer);
};

#endif // INTERFACE_HPP
