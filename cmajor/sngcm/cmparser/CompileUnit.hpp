#ifndef COMPILEUNIT_HPP
#define COMPILEUNIT_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <memory>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/CompileUnit.parser' using soulng parser generator spg version 3.10.0

class CmajorLexer;

struct SNGCM_PARSER_API CompileUnitParser
{
    static std::unique_ptr<sngcm::ast::CompileUnitNode> Parse(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match CompileUnit(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match NamespaceContent(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match UsingDirectives(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match UsingDirective(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match UsingAliasDirective(CmajorLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match UsingNamespaceDirective(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match Definitions(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match Definition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match NamespaceDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match TypedefDeclaration(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match ConceptDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match FunctionDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match ClassDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match InterfaceDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match EnumTypeDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match ConstantDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match DelegateDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match ClassDelegateDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match GlobalVariableDefinition(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::CompileUnitNode* cu);
    static soulng::parser::Match InstantiationRequestDeclaration(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
};

#endif // COMPILEUNIT_HPP
