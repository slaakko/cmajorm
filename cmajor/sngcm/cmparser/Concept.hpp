#ifndef CONCEPT_HPP
#define CONCEPT_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Concept.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngcm/cmparser/Concept.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API ConceptParser
{
    static soulng::parser::Match Concept(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Refinement(CmajorLexer& lexer);
    static soulng::parser::Match ConceptBody(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::ConceptNode* conceptNode);
    static soulng::parser::Match ConceptBodyConstraint(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::ConceptNode* conceptNode);
    static soulng::parser::Match TypeNameConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match SignatureConstraint(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::IdentifierNode* firstTypeParameter);
    static soulng::parser::Match ConstructorConstraint(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::IdentifierNode* firstTypeParameter);
    static soulng::parser::Match DestructorConstraint(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::IdentifierNode* firstTypeParameter);
    static soulng::parser::Match MemberFunctionConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match FunctionConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match EmbeddedConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match WhereConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ConstraintExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match DisjunctiveConstraintExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ConjunctiveConstraintExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match PrimaryConstraintExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match AtomicConstraintExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match PredicateConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match IsConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ConceptOrTypeName(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match MultiParamConstraint(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Axiom(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::ConceptNode* conceptNode);
    static soulng::parser::Match AxiomBody(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::AxiomNode* axiom);
    static soulng::parser::Match AxiomStatement(CmajorLexer& lexer, ParsingContext* ctx);
};

#endif // CONCEPT_HPP
