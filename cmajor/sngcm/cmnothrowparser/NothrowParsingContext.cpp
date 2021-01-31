// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>

NothrowParsingContext::NothrowParsingContext() :
    parsingExpressionStatement(false), parsingLvalue(false), parsingArguments(false), parsingIsOrAs(false), parsingTypeExpr(false), parsingConcept(false), parsingTemplateId(false)
{
}

void NothrowParsingContext::PushParsingExpressionStatement(bool enable)
{
    parsingExpressionStatementStack.push(parsingExpressionStatement);
    parsingExpressionStatement = enable;
}

void NothrowParsingContext::PopParsingExpressionStatement()
{
    parsingExpressionStatement = parsingExpressionStatementStack.top();
    parsingExpressionStatementStack.pop();
}

void NothrowParsingContext::PushParsingLvalue(bool enable)
{
    parsingLvalueStack.push(parsingLvalue);
    parsingLvalue = enable;
}

void NothrowParsingContext::PopParsingLvalue()
{
    parsingLvalue = parsingLvalueStack.top();
    parsingLvalueStack.pop();
}

void NothrowParsingContext::BeginParsingArguments()
{
    parsingArgumentsStack.push(parsingArguments);
    parsingArguments = true;
}

void NothrowParsingContext::EndParsingArguments()
{
    parsingArguments = parsingArgumentsStack.top();
    parsingArgumentsStack.pop();
}

void NothrowParsingContext::PushParsingIsOrAs(bool enable)
{
    parsingIsOrAsStack.push(parsingIsOrAs);
    parsingIsOrAs = enable;
}

void NothrowParsingContext::PopParsingIsOrAs()
{
    parsingIsOrAs = parsingIsOrAsStack.top();
    parsingIsOrAsStack.pop();
}

void NothrowParsingContext::BeginParsingTypeExpr()
{
    parsingTypeExprStack.push(parsingTypeExpr);
    parsingTypeExpr = true;
}

void NothrowParsingContext::EndParsingTypeExpr()
{
    parsingTypeExpr = parsingTypeExprStack.top();
    parsingTypeExprStack.pop();
}

void NothrowParsingContext::BeginParsingConcept()
{
    parsingConceptStack.push(parsingConcept);
    parsingConcept = true;
}

void NothrowParsingContext::EndParsingConcept()
{
    parsingConcept = parsingConceptStack.top();
    parsingConceptStack.pop();
}

void NothrowParsingContext::BeginParsingTemplateId()
{
    parsingTemplateIdStack.push(parsingTemplateId);
    parsingTemplateId = true;
}

void NothrowParsingContext::EndParsingTemplateId()
{
    parsingTemplateId = parsingTemplateIdStack.top();
    parsingTemplateIdStack.pop();
}
