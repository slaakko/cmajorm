// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSER_PARSING_CONTEXT_INCLUDED
#define CMAJOR_PARSER_PARSING_CONTEXT_INCLUDED
#include <stdint.h>
#include <stack>

namespace cmajor { namespace parser {

class ParsingContext
{
public:
    ParsingContext();
    bool ParsingExpressionStatement() const { return parsingExpressionStatement; }
    void PushParsingExpressionStatement(bool enable);
    void PopParsingExpressionStatement();
    bool ParsingLvalue() const { return parsingLvalue; }
    void PushParsingLvalue(bool enable);
    void PopParsingLvalue();
    bool ParsingArguments() const { return parsingArguments; }
    void BeginParsingArguments();
    void EndParsingArguments();
    bool ParsingIsOrAs() const { return parsingIsOrAs; }
    void PushParsingIsOrAs(bool enable);
    void PopParsingIsOrAs();
    bool ParsingTypeExpr() const { return parsingTypeExpr; }
    void BeginParsingTypeExpr();
    void EndParsingTypeExpr();
    bool ParsingConcept() const { return parsingConcept; }
    void BeginParsingConcept();
    void EndParsingConcept();
    bool ParsingTemplateId() const { return parsingTemplateId; }
    void BeginParsingTemplateId();
    void EndParsingTemplateId();
private:
    bool parsingExpressionStatement;
    std::stack<bool> parsingExpressionStatementStack;
    bool parsingLvalue;
    std::stack<bool> parsingLvalueStack;
    bool parsingArguments;
    std::stack<bool> parsingArgumentsStack;
    bool parsingIsOrAs;
    std::stack<bool> parsingIsOrAsStack;
    bool parsingTypeExpr;
    std::stack<bool> parsingTypeExprStack;
    bool parsingConcept;
    std::stack<bool> parsingConceptStack;
    bool parsingTemplateId;
    std::stack<bool> parsingTemplateIdStack;
};

} } // namespace cmajor::parser

#endif // CMAJOR_PARSER_PARSING_CONTEXT_INCLUDED
