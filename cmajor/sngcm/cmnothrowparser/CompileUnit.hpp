#ifndef COMPILEUNIT_HPP
#define COMPILEUNIT_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/SyncNode.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <memory>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/CompileUnit.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowCompileUnitParser
{
    static std::unique_ptr<sngcm::ast::CompileUnitNode> Parse(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match CompileUnit(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match NamespaceContent(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match UsingDirectives(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match UsingDirective(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match UsingAliasDirective(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match UsingNamespaceDirective(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match Definitions(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match Definition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match SyncDef(CmajorNothrowLexer& lexer);
    static soulng::parser::Match NamespaceDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::CompileUnitNode* cu, sngcm::ast::NamespaceNode* ns);
    static soulng::parser::Match DefiningNamespaceId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match TypedefDeclaration(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match ConceptDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match FunctionDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match ClassDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match InterfaceDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match EnumTypeDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match ConstantDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match DelegateDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match ClassDelegateDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match GlobalVariableDefinition(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::CompileUnitNode* cu);
    static soulng::parser::Match InstantiationRequestDeclaration(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
};

#endif // COMPILEUNIT_HPP
