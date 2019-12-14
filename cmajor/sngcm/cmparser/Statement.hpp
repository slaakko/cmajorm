#ifndef STATEMENT_HPP
#define STATEMENT_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Statement.hpp>
#include <sngcm/cmparser/ParsingContext.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmparser/Statement.parser' using soulng parser generator spg version 3.0.0

class CmajorLexer;

struct SNGCM_PARSER_API StatementParser
{
    static soulng::parser::Match Statement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Label(CmajorLexer& lexer);
    static soulng::parser::Match LabeledStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ControlStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match CompoundStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ReturnStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match IfStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match WhileStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match DoStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ForStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ForInitStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ForLoopStatementExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match RangeForStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match BreakStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ContinueStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match GotoStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match SwitchStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match CaseStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match DefaultStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match GotoCaseStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match GotoDefaultStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match AssignmentStatementExpr(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match AssignmentStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ConstructionStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match DeleteStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match DestroyStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ExpressionStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match EmptyStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ThrowStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match TryStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match Catch(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match AssertStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ConditionalCompilationStatement(CmajorLexer& lexer, ParsingContext* ctx);
    static soulng::parser::Match ConditionalCompilationExpression(CmajorLexer& lexer);
    static soulng::parser::Match ConditionalCompilationDisjunction(CmajorLexer& lexer);
    static soulng::parser::Match ConditionalCompilationConjunction(CmajorLexer& lexer);
    static soulng::parser::Match ConditionalCompilationPrefix(CmajorLexer& lexer);
    static soulng::parser::Match ConditionalCompilationPrimary(CmajorLexer& lexer);
};

#endif // STATEMENT_HPP
