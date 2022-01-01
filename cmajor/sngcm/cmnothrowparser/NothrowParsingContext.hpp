// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_PARSER_PARSING_CONTEXT_HPP
#define SNGCM_PARSER_PARSING_CONTEXT_HPP
#include <sngcm/cmnothrowparser/ParserApi.hpp>
#include <stack>

struct SNGCM_NOTHROW_PARSER_API NothrowParsingContext
{
    NothrowParsingContext();
    void PushParsingExpressionStatement(bool enable);
    void PopParsingExpressionStatement();
    void PushParsingLvalue(bool enable);
    void PopParsingLvalue();
    void BeginParsingArguments();
    void EndParsingArguments();
    void PushParsingIsOrAs(bool enable);
    void PopParsingIsOrAs();
    void BeginParsingTypeExpr();
    void EndParsingTypeExpr();
    void BeginParsingConcept();
    void EndParsingConcept();
    void BeginParsingTemplateId();
    void EndParsingTemplateId();
    bool parsingExpressionStatement;
    bool parsingLvalue;
    bool parsingArguments;
    bool parsingIsOrAs;
    bool parsingTypeExpr;
    bool parsingConcept;
    bool parsingTemplateId;
private:
    std::stack<bool> parsingExpressionStatementStack;
    std::stack<bool> parsingLvalueStack;
    std::stack<bool> parsingArgumentsStack;
    std::stack<bool> parsingIsOrAsStack;
    std::stack<bool> parsingTypeExprStack;
    std::stack<bool> parsingConceptStack;
    std::stack<bool> parsingTemplateIdStack;
};

#endif // SNGCM_PARSER_PARSING_CONTEXT_HPP
