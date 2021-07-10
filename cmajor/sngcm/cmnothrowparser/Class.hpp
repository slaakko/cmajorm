#ifndef CLASS_HPP
#define CLASS_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/SyncNode.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Class.parser' using soulng parser generator spg version 4.0.0

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowClassParser
{
    static soulng::parser::Match Class(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match DefiningClassId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
    static soulng::parser::Match InheritanceAndInterfaces(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match BaseClassOrInterface(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match ClassContent(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match ClassMember(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match SyncMember(CmajorNothrowLexer& lexer);
    static soulng::parser::Match StaticConstructor(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match Constructor(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match Destructor(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match Initializer(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match MemberFunction(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match ConversionFunction(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match MemberVariable(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match DefiningMemberVariableId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId);
};

class CmajorNothrowLexer;

struct SNGCM_NOTHROW_PARSER_API NothrowClassMemberParser
{
    static std::unique_ptr<sngcm::ast::Node> Parse(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
    static soulng::parser::Match ClassMemberMain(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx);
};

#endif // CLASS_HPP
