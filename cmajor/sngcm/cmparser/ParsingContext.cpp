// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/cmparser/ParsingContext.hpp>

ParsingContext::ParsingContext() :
    parsingExpressionStatement(false), parsingLvalue(false), parsingArguments(false), parsingIsOrAs(false), parsingTypeExpr(false), parsingConcept(false), parsingTemplateId(false)
{
}

void ParsingContext::PushParsingExpressionStatement(bool enable)
{
    parsingExpressionStatementStack.push(parsingExpressionStatement);
    parsingExpressionStatement = enable;
}

void ParsingContext::PopParsingExpressionStatement()
{
    parsingExpressionStatement = parsingExpressionStatementStack.top();
    parsingExpressionStatementStack.pop();
}

void ParsingContext::PushParsingLvalue(bool enable)
{
    parsingLvalueStack.push(parsingLvalue);
    parsingLvalue = enable;
}

void ParsingContext::PopParsingLvalue()
{
    parsingLvalue = parsingLvalueStack.top();
    parsingLvalueStack.pop();
}

void ParsingContext::BeginParsingArguments()
{
    parsingArgumentsStack.push(parsingArguments);
    parsingArguments = true;
}

void ParsingContext::EndParsingArguments()
{
    parsingArguments = parsingArgumentsStack.top();
    parsingArgumentsStack.pop();
}

void ParsingContext::PushParsingIsOrAs(bool enable)
{
    parsingIsOrAsStack.push(parsingIsOrAs);
    parsingIsOrAs = enable;
}

void ParsingContext::PopParsingIsOrAs()
{
    parsingIsOrAs = parsingIsOrAsStack.top();
    parsingIsOrAsStack.pop();
}

void ParsingContext::BeginParsingTypeExpr()
{
    parsingTypeExprStack.push(parsingTypeExpr);
    parsingTypeExpr = true;
}

void ParsingContext::EndParsingTypeExpr()
{
    parsingTypeExpr = parsingTypeExprStack.top();
    parsingTypeExprStack.pop();
}

void ParsingContext::BeginParsingConcept()
{
    parsingConceptStack.push(parsingConcept);
    parsingConcept = true;
}

void ParsingContext::EndParsingConcept()
{
    parsingConcept = parsingConceptStack.top();
    parsingConceptStack.pop();
}

void ParsingContext::BeginParsingTemplateId()
{
    parsingTemplateIdStack.push(parsingTemplateId);
    parsingTemplateId = true;
}

void ParsingContext::EndParsingTemplateId()
{
    parsingTemplateId = parsingTemplateIdStack.top();
    parsingTemplateIdStack.pop();
}
