#ifndef CLASS_HPP
#define CLASS_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/Class.parser' using soulng parser generator spg version 3.10.0

class CmajorLexer;

struct SNGCM_PARSER_API ClassParser
{
    static soulng::parser::Match Class(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match InheritanceAndInterfaces(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match BaseClassOrInterface(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match ClassContent(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match ClassMember(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match StaticConstructor(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match Constructor(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match Destructor(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::ClassNode* classNode);
    static soulng::parser::Match Initializer(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match MemberFunction(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match ConversionFunction(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
    static soulng::parser::Match MemberVariable(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx);
};

#endif // CLASS_HPP
