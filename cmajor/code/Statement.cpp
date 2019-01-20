#include "Statement.hpp"
#include <cmajor/parsing/Action.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/parsing/Primitive.hpp>
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/Nonterminal.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/parsing/StdLib.hpp>
#include <cmajor/parsing/XmlLog.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/code/Expression.hpp>
#include <cmajor/code/Declaration.hpp>
#include <cmajor/code/Declarator.hpp>
#include <cmajor/code/Identifier.hpp>
#include <cmajor/codedom/Type.hpp>

namespace cmajor { namespace code {

using namespace cmajor::codedom;
using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Statement* Statement::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Statement* Statement::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Statement* grammar(new Statement(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Statement::Statement(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Statement"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.code")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::codedom::CompoundStatement* Statement::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
{
    cmajor::parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<cmajor::parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new cmajor::parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    cmajor::parsing::ObjectStack stack;
    std::unique_ptr<cmajor::parsing::ParsingData> parsingData(new cmajor::parsing::ParsingData(GetParsingDomain()->GetNumRules()));
    scanner.SetParsingData(parsingData.get());
    cmajor::parsing::Match match = cmajor::parsing::Grammar::Parse(scanner, stack, parsingData.get());
    cmajor::parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw cmajor::parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw cmajor::parsing::ParsingException("grammar '" + ToUtf8(Name()) + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<cmajor::parsing::Object> value = std::move(stack.top());
    cmajor::codedom::CompoundStatement* result = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CompoundStatement*>*>(value.get());
    stack.pop();
    return result;
}

class Statement::StatementRule : public cmajor::parsing::Rule
{
public:
    StatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StatementRule>(this, &StatementRule::A8Action));
        cmajor::parsing::NonterminalParser* labeledStatementNonterminalParser = GetNonterminal(ToUtf32("LabeledStatement"));
        labeledStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostLabeledStatement));
        cmajor::parsing::NonterminalParser* emptyStatementNonterminalParser = GetNonterminal(ToUtf32("EmptyStatement"));
        emptyStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostEmptyStatement));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostCompoundStatement));
        cmajor::parsing::NonterminalParser* selectionStatementNonterminalParser = GetNonterminal(ToUtf32("SelectionStatement"));
        selectionStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostSelectionStatement));
        cmajor::parsing::NonterminalParser* iterationStatementNonterminalParser = GetNonterminal(ToUtf32("IterationStatement"));
        iterationStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostIterationStatement));
        cmajor::parsing::NonterminalParser* jumpStatementNonterminalParser = GetNonterminal(ToUtf32("JumpStatement"));
        jumpStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostJumpStatement));
        cmajor::parsing::NonterminalParser* declarationStatementNonterminalParser = GetNonterminal(ToUtf32("DeclarationStatement"));
        declarationStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostDeclarationStatement));
        cmajor::parsing::NonterminalParser* tryStatementNonterminalParser = GetNonterminal(ToUtf32("TryStatement"));
        tryStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostTryStatement));
        cmajor::parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal(ToUtf32("ExpressionStatement"));
        expressionStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StatementRule>(this, &StatementRule::PostExpressionStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLabeledStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEmptyStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCompoundStatement;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSelectionStatement;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIterationStatement;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromJumpStatement;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDeclarationStatement;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTryStatement;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionStatement;
    }
    void PostLabeledStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLabeledStatement_value = std::move(stack.top());
            context->fromLabeledStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromLabeledStatement_value.get());
            stack.pop();
        }
    }
    void PostEmptyStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEmptyStatement_value = std::move(stack.top());
            context->fromEmptyStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromEmptyStatement_value.get());
            stack.pop();
        }
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PostSelectionStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSelectionStatement_value = std::move(stack.top());
            context->fromSelectionStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromSelectionStatement_value.get());
            stack.pop();
        }
    }
    void PostIterationStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIterationStatement_value = std::move(stack.top());
            context->fromIterationStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromIterationStatement_value.get());
            stack.pop();
        }
    }
    void PostJumpStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromJumpStatement_value = std::move(stack.top());
            context->fromJumpStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromJumpStatement_value.get());
            stack.pop();
        }
    }
    void PostDeclarationStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarationStatement_value = std::move(stack.top());
            context->fromDeclarationStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromDeclarationStatement_value.get());
            stack.pop();
        }
    }
    void PostTryStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTryStatement_value = std::move(stack.top());
            context->fromTryStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TryStatement*>*>(fromTryStatement_value.get());
            stack.pop();
        }
    }
    void PostExpressionStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context->fromExpressionStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromLabeledStatement(), fromEmptyStatement(), fromCompoundStatement(), fromSelectionStatement(), fromIterationStatement(), fromJumpStatement(), fromDeclarationStatement(), fromTryStatement(), fromExpressionStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::Statement* fromLabeledStatement;
        cmajor::codedom::Statement* fromEmptyStatement;
        cmajor::codedom::CompoundStatement* fromCompoundStatement;
        cmajor::codedom::Statement* fromSelectionStatement;
        cmajor::codedom::Statement* fromIterationStatement;
        cmajor::codedom::Statement* fromJumpStatement;
        cmajor::codedom::Statement* fromDeclarationStatement;
        cmajor::codedom::TryStatement* fromTryStatement;
        cmajor::codedom::Statement* fromExpressionStatement;
    };
};

class Statement::LabeledStatementRule : public cmajor::parsing::Rule
{
public:
    LabeledStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LabeledStatementRule>(this, &LabeledStatementRule::A2Action));
        cmajor::parsing::NonterminalParser* labelNonterminalParser = GetNonterminal(ToUtf32("Label"));
        labelNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostLabel));
        cmajor::parsing::NonterminalParser* s1NonterminalParser = GetNonterminal(ToUtf32("s1"));
        s1NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts1));
        cmajor::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::PostConstantExpression));
        cmajor::parsing::NonterminalParser* s2NonterminalParser = GetNonterminal(ToUtf32("s2"));
        s2NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts2));
        cmajor::parsing::NonterminalParser* s3NonterminalParser = GetNonterminal(ToUtf32("s3"));
        s3NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LabeledStatementRule>(this, &LabeledStatementRule::Posts3));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LabeledStatement(context->fromLabel, context->froms1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CaseStatement(context->fromConstantExpression, context->froms2);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DefaultStatement(context->froms3);
    }
    void PostLabel(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLabel_value = std::move(stack.top());
            context->fromLabel = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromLabel_value.get());
            stack.pop();
        }
    }
    void Posts1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> froms1_value = std::move(stack.top());
            context->froms1 = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(froms1_value.get());
            stack.pop();
        }
    }
    void PostConstantExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context->fromConstantExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
    void Posts2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> froms2_value = std::move(stack.top());
            context->froms2 = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(froms2_value.get());
            stack.pop();
        }
    }
    void Posts3(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> froms3_value = std::move(stack.top());
            context->froms3 = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(froms3_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromLabel(), froms1(), fromConstantExpression(), froms2(), froms3() {}
        cmajor::codedom::Statement* value;
        std::u32string fromLabel;
        cmajor::codedom::Statement* froms1;
        cmajor::codedom::CppObject* fromConstantExpression;
        cmajor::codedom::Statement* froms2;
        cmajor::codedom::Statement* froms3;
    };
};

class Statement::LabelRule : public cmajor::parsing::Rule
{
public:
    LabelRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LabelRule>(this, &LabelRule::A0Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LabelRule>(this, &LabelRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
    }
    void PostIdentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromIdentifier() {}
        std::u32string value;
        std::u32string fromIdentifier;
    };
};

class Statement::EmptyStatementRule : public cmajor::parsing::Rule
{
public:
    EmptyStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EmptyStatementRule>(this, &EmptyStatementRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EmptyStatement;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Statement* value;
    };
};

class Statement::ExpressionStatementRule : public cmajor::parsing::Rule
{
public:
    ExpressionStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionStatementRule>(this, &ExpressionStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExpressionStatementRule>(this, &ExpressionStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExpressionStatement(context->fromExpression);
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExpression() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::CppObject* fromExpression;
    };
};

class Statement::CompoundStatementRule : public cmajor::parsing::Rule
{
public:
    CompoundStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CompoundStatement*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CompoundStatement>"), ToUtf32("cs")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CompoundStatement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CompoundStatementRule>(this, &CompoundStatementRule::A2Action));
        cmajor::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CompoundStatementRule>(this, &CompoundStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->cs.reset(new CompoundStatement);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->cs->Add(context->fromStatement);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->cs.release();
    }
    void PostStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), cs(), fromStatement() {}
        cmajor::codedom::CompoundStatement* value;
        std::unique_ptr<CompoundStatement> cs;
        cmajor::codedom::Statement* fromStatement;
    };
};

class Statement::SelectionStatementRule : public cmajor::parsing::Rule
{
public:
    SelectionStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SelectionStatementRule>(this, &SelectionStatementRule::A1Action));
        cmajor::parsing::NonterminalParser* ifStatementNonterminalParser = GetNonterminal(ToUtf32("IfStatement"));
        ifStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostIfStatement));
        cmajor::parsing::NonterminalParser* switchStatementNonterminalParser = GetNonterminal(ToUtf32("SwitchStatement"));
        switchStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SelectionStatementRule>(this, &SelectionStatementRule::PostSwitchStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIfStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSwitchStatement;
    }
    void PostIfStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIfStatement_value = std::move(stack.top());
            context->fromIfStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromIfStatement_value.get());
            stack.pop();
        }
    }
    void PostSwitchStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSwitchStatement_value = std::move(stack.top());
            context->fromSwitchStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromSwitchStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromIfStatement(), fromSwitchStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::Statement* fromIfStatement;
        cmajor::codedom::Statement* fromSwitchStatement;
    };
};

class Statement::IfStatementRule : public cmajor::parsing::Rule
{
public:
    IfStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IfStatementRule>(this, &IfStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostCondition));
        cmajor::parsing::NonterminalParser* thenStatementNonterminalParser = GetNonterminal(ToUtf32("thenStatement"));
        thenStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostthenStatement));
        cmajor::parsing::NonterminalParser* elseStatementNonterminalParser = GetNonterminal(ToUtf32("elseStatement"));
        elseStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IfStatementRule>(this, &IfStatementRule::PostelseStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IfStatement(context->fromCondition, context->fromthenStatement, context->fromelseStatement);
    }
    void PostCondition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostthenStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromthenStatement_value = std::move(stack.top());
            context->fromthenStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromthenStatement_value.get());
            stack.pop();
        }
    }
    void PostelseStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromelseStatement_value = std::move(stack.top());
            context->fromelseStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromelseStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromCondition(), fromthenStatement(), fromelseStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::CppObject* fromCondition;
        cmajor::codedom::Statement* fromthenStatement;
        cmajor::codedom::Statement* fromelseStatement;
    };
};

class Statement::SwitchStatementRule : public cmajor::parsing::Rule
{
public:
    SwitchStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SwitchStatementRule>(this, &SwitchStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostCondition));
        cmajor::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SwitchStatementRule>(this, &SwitchStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SwitchStatement(context->fromCondition, context->fromStatement);
    }
    void PostCondition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromCondition(), fromStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::CppObject* fromCondition;
        cmajor::codedom::Statement* fromStatement;
    };
};

class Statement::IterationStatementRule : public cmajor::parsing::Rule
{
public:
    IterationStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IterationStatementRule>(this, &IterationStatementRule::A2Action));
        cmajor::parsing::NonterminalParser* whileStatementNonterminalParser = GetNonterminal(ToUtf32("WhileStatement"));
        whileStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostWhileStatement));
        cmajor::parsing::NonterminalParser* doStatementNonterminalParser = GetNonterminal(ToUtf32("DoStatement"));
        doStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostDoStatement));
        cmajor::parsing::NonterminalParser* forStatementNonterminalParser = GetNonterminal(ToUtf32("ForStatement"));
        forStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IterationStatementRule>(this, &IterationStatementRule::PostForStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromWhileStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDoStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromForStatement;
    }
    void PostWhileStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromWhileStatement_value = std::move(stack.top());
            context->fromWhileStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromWhileStatement_value.get());
            stack.pop();
        }
    }
    void PostDoStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDoStatement_value = std::move(stack.top());
            context->fromDoStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromDoStatement_value.get());
            stack.pop();
        }
    }
    void PostForStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromForStatement_value = std::move(stack.top());
            context->fromForStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromForStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromWhileStatement(), fromDoStatement(), fromForStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::Statement* fromWhileStatement;
        cmajor::codedom::Statement* fromDoStatement;
        cmajor::codedom::Statement* fromForStatement;
    };
};

class Statement::WhileStatementRule : public cmajor::parsing::Rule
{
public:
    WhileStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<WhileStatementRule>(this, &WhileStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostCondition));
        cmajor::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<WhileStatementRule>(this, &WhileStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new WhileStatement(context->fromCondition, context->fromStatement);
    }
    void PostCondition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromCondition(), fromStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::CppObject* fromCondition;
        cmajor::codedom::Statement* fromStatement;
    };
};

class Statement::DoStatementRule : public cmajor::parsing::Rule
{
public:
    DoStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DoStatementRule>(this, &DoStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostStatement));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DoStatementRule>(this, &DoStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DoStatement(context->fromStatement, context->fromExpression);
    }
    void PostStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromStatement(), fromExpression() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::Statement* fromStatement;
        cmajor::codedom::CppObject* fromExpression;
    };
};

class Statement::ForStatementRule : public cmajor::parsing::Rule
{
public:
    ForStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ForStatementRule>(this, &ForStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* forInitStatementNonterminalParser = GetNonterminal(ToUtf32("ForInitStatement"));
        forInitStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostForInitStatement));
        cmajor::parsing::NonterminalParser* conditionNonterminalParser = GetNonterminal(ToUtf32("Condition"));
        conditionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostCondition));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostExpression));
        cmajor::parsing::NonterminalParser* statementNonterminalParser = GetNonterminal(ToUtf32("Statement"));
        statementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ForStatementRule>(this, &ForStatementRule::PostStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ForStatement(context->fromForInitStatement, context->fromCondition, context->fromExpression, context->fromStatement);
    }
    void PostForInitStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromForInitStatement_value = std::move(stack.top());
            context->fromForInitStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromForInitStatement_value.get());
            stack.pop();
        }
    }
    void PostCondition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCondition_value = std::move(stack.top());
            context->fromCondition = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromCondition_value.get());
            stack.pop();
        }
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStatement_value = std::move(stack.top());
            context->fromStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromForInitStatement(), fromCondition(), fromExpression(), fromStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::CppObject* fromForInitStatement;
        cmajor::codedom::CppObject* fromCondition;
        cmajor::codedom::CppObject* fromExpression;
        cmajor::codedom::Statement* fromStatement;
    };
};

class Statement::ForInitStatementRule : public cmajor::parsing::Rule
{
public:
    ForInitStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ForInitStatementRule>(this, &ForInitStatementRule::A2Action));
        cmajor::parsing::NonterminalParser* emptyStatementNonterminalParser = GetNonterminal(ToUtf32("EmptyStatement"));
        emptyStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostEmptyStatement));
        cmajor::parsing::NonterminalParser* expressionStatementNonterminalParser = GetNonterminal(ToUtf32("ExpressionStatement"));
        expressionStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostExpressionStatement));
        cmajor::parsing::NonterminalParser* simpleDeclarationNonterminalParser = GetNonterminal(ToUtf32("SimpleDeclaration"));
        simpleDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ForInitStatementRule>(this, &ForInitStatementRule::PostSimpleDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEmptyStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleDeclaration;
    }
    void PostEmptyStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEmptyStatement_value = std::move(stack.top());
            context->fromEmptyStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromEmptyStatement_value.get());
            stack.pop();
        }
    }
    void PostExpressionStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpressionStatement_value = std::move(stack.top());
            context->fromExpressionStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromExpressionStatement_value.get());
            stack.pop();
        }
    }
    void PostSimpleDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSimpleDeclaration_value = std::move(stack.top());
            context->fromSimpleDeclaration = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::SimpleDeclaration*>*>(fromSimpleDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromEmptyStatement(), fromExpressionStatement(), fromSimpleDeclaration() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::Statement* fromEmptyStatement;
        cmajor::codedom::Statement* fromExpressionStatement;
        cmajor::codedom::SimpleDeclaration* fromSimpleDeclaration;
    };
};

class Statement::JumpStatementRule : public cmajor::parsing::Rule
{
public:
    JumpStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<JumpStatementRule>(this, &JumpStatementRule::A3Action));
        cmajor::parsing::NonterminalParser* breakStatementNonterminalParser = GetNonterminal(ToUtf32("BreakStatement"));
        breakStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostBreakStatement));
        cmajor::parsing::NonterminalParser* continueStatementNonterminalParser = GetNonterminal(ToUtf32("ContinueStatement"));
        continueStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostContinueStatement));
        cmajor::parsing::NonterminalParser* returnStatementNonterminalParser = GetNonterminal(ToUtf32("ReturnStatement"));
        returnStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostReturnStatement));
        cmajor::parsing::NonterminalParser* gotoStatementNonterminalParser = GetNonterminal(ToUtf32("GotoStatement"));
        gotoStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<JumpStatementRule>(this, &JumpStatementRule::PostGotoStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBreakStatement;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromContinueStatement;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromReturnStatement;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGotoStatement;
    }
    void PostBreakStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromBreakStatement_value = std::move(stack.top());
            context->fromBreakStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromBreakStatement_value.get());
            stack.pop();
        }
    }
    void PostContinueStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromContinueStatement_value = std::move(stack.top());
            context->fromContinueStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromContinueStatement_value.get());
            stack.pop();
        }
    }
    void PostReturnStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromReturnStatement_value = std::move(stack.top());
            context->fromReturnStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromReturnStatement_value.get());
            stack.pop();
        }
    }
    void PostGotoStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGotoStatement_value = std::move(stack.top());
            context->fromGotoStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Statement*>*>(fromGotoStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromBreakStatement(), fromContinueStatement(), fromReturnStatement(), fromGotoStatement() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::Statement* fromBreakStatement;
        cmajor::codedom::Statement* fromContinueStatement;
        cmajor::codedom::Statement* fromReturnStatement;
        cmajor::codedom::Statement* fromGotoStatement;
    };
};

class Statement::BreakStatementRule : public cmajor::parsing::Rule
{
public:
    BreakStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BreakStatementRule>(this, &BreakStatementRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BreakStatement;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Statement* value;
    };
};

class Statement::ContinueStatementRule : public cmajor::parsing::Rule
{
public:
    ContinueStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ContinueStatementRule>(this, &ContinueStatementRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ContinueStatement;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Statement* value;
    };
};

class Statement::ReturnStatementRule : public cmajor::parsing::Rule
{
public:
    ReturnStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ReturnStatementRule>(this, &ReturnStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ReturnStatementRule>(this, &ReturnStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ReturnStatement(context->fromExpression);
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExpression() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::CppObject* fromExpression;
    };
};

class Statement::GotoStatementRule : public cmajor::parsing::Rule
{
public:
    GotoStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<GotoStatementRule>(this, &GotoStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* gotoTargetNonterminalParser = GetNonterminal(ToUtf32("GotoTarget"));
        gotoTargetNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GotoStatementRule>(this, &GotoStatementRule::PostGotoTarget));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new GotoStatement(context->fromGotoTarget);
    }
    void PostGotoTarget(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGotoTarget_value = std::move(stack.top());
            context->fromGotoTarget = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromGotoTarget_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromGotoTarget() {}
        cmajor::codedom::Statement* value;
        std::u32string fromGotoTarget;
    };
};

class Statement::GotoTargetRule : public cmajor::parsing::Rule
{
public:
    GotoTargetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<GotoTargetRule>(this, &GotoTargetRule::A0Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GotoTargetRule>(this, &GotoTargetRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
    }
    void PostIdentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromIdentifier() {}
        std::u32string value;
        std::u32string fromIdentifier;
    };
};

class Statement::DeclarationStatementRule : public cmajor::parsing::Rule
{
public:
    DeclarationStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Statement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Statement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclarationStatementRule>(this, &DeclarationStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* blockDeclarationNonterminalParser = GetNonterminal(ToUtf32("BlockDeclaration"));
        blockDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclarationStatementRule>(this, &DeclarationStatementRule::PostBlockDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DeclarationStatement(context->fromBlockDeclaration);
    }
    void PostBlockDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromBlockDeclaration_value = std::move(stack.top());
            context->fromBlockDeclaration = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromBlockDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromBlockDeclaration() {}
        cmajor::codedom::Statement* value;
        cmajor::codedom::CppObject* fromBlockDeclaration;
    };
};

class Statement::ConditionRule : public cmajor::parsing::Rule
{
public:
    ConditionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<TypeId>"), ToUtf32("ti")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConditionRule>(this, &ConditionRule::A2Action));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostTypeId));
        cmajor::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostDeclarator));
        cmajor::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostAssignmentExpression));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConditionRule>(this, &ConditionRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(context->fromTypeId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConditionWithDeclarator(context->ti.release(), context->fromDeclarator, context->fromAssignmentExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpression;
    }
    void PostTypeId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), ti(), fromTypeId(), fromDeclarator(), fromAssignmentExpression(), fromExpression() {}
        cmajor::codedom::CppObject* value;
        std::unique_ptr<TypeId> ti;
        cmajor::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
        cmajor::codedom::CppObject* fromAssignmentExpression;
        cmajor::codedom::CppObject* fromExpression;
    };
};

class Statement::TryStatementRule : public cmajor::parsing::Rule
{
public:
    TryStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::TryStatement*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TryStatement*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TryStatementRule>(this, &TryStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TryStatementRule>(this, &TryStatementRule::PostCompoundStatement));
        cmajor::parsing::NonterminalParser* handlerSeqNonterminalParser = GetNonterminal(ToUtf32("HandlerSeq"));
        handlerSeqNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TryStatementRule>(this, &TryStatementRule::PreHandlerSeq));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TryStatement(context->fromCompoundStatement);
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
    void PreHandlerSeq(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<TryStatement*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromCompoundStatement() {}
        cmajor::codedom::TryStatement* value;
        cmajor::codedom::CompoundStatement* fromCompoundStatement;
    };
};

class Statement::HandlerSeqRule : public cmajor::parsing::Rule
{
public:
    HandlerSeqRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("TryStatement*"), ToUtf32("st")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> st_value = std::move(stack.top());
        context->st = *static_cast<cmajor::parsing::ValueObject<TryStatement*>*>(st_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<HandlerSeqRule>(this, &HandlerSeqRule::A0Action));
        cmajor::parsing::NonterminalParser* handlerNonterminalParser = GetNonterminal(ToUtf32("Handler"));
        handlerNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<HandlerSeqRule>(this, &HandlerSeqRule::PostHandler));
        cmajor::parsing::NonterminalParser* handlerSeqNonterminalParser = GetNonterminal(ToUtf32("HandlerSeq"));
        handlerSeqNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<HandlerSeqRule>(this, &HandlerSeqRule::PreHandlerSeq));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->st->Add(context->fromHandler);
    }
    void PostHandler(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromHandler_value = std::move(stack.top());
            context->fromHandler = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Handler*>*>(fromHandler_value.get());
            stack.pop();
        }
    }
    void PreHandlerSeq(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<TryStatement*>(context->st)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): st(), fromHandler() {}
        TryStatement* st;
        cmajor::codedom::Handler* fromHandler;
    };
};

class Statement::HandlerRule : public cmajor::parsing::Rule
{
public:
    HandlerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Handler*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Handler*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<HandlerRule>(this, &HandlerRule::A0Action));
        cmajor::parsing::NonterminalParser* exceptionDeclarationNonterminalParser = GetNonterminal(ToUtf32("ExceptionDeclaration"));
        exceptionDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostExceptionDeclaration));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<HandlerRule>(this, &HandlerRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Handler(context->fromExceptionDeclaration, context->fromCompoundStatement);
    }
    void PostExceptionDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExceptionDeclaration_value = std::move(stack.top());
            context->fromExceptionDeclaration = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::ExceptionDeclaration*>*>(fromExceptionDeclaration_value.get());
            stack.pop();
        }
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CompoundStatement*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExceptionDeclaration(), fromCompoundStatement() {}
        cmajor::codedom::Handler* value;
        cmajor::codedom::ExceptionDeclaration* fromExceptionDeclaration;
        cmajor::codedom::CompoundStatement* fromCompoundStatement;
    };
};

class Statement::ExceptionDeclarationRule : public cmajor::parsing::Rule
{
public:
    ExceptionDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::ExceptionDeclaration*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<ExceptionDeclaration>"), ToUtf32("ed")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::ExceptionDeclaration*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::A4Action));
        cmajor::parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierSeq"));
        typeSpecifierSeqNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PreTypeSpecifierSeq));
        cmajor::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostDeclarator));
        cmajor::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExceptionDeclarationRule>(this, &ExceptionDeclarationRule::PostAbstractDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ed.reset(new ExceptionDeclaration);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ed.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ed->GetTypeId()->Declarator() = context->fromDeclarator;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ed->GetTypeId()->Declarator() = context->fromAbstractDeclarator;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->CatchAll() = true;
    }
    void PreTypeSpecifierSeq(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>(context->ed->GetTypeId())));
    }
    void PostDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
    void PostAbstractDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context->fromAbstractDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), ed(), fromDeclarator(), fromAbstractDeclarator() {}
        cmajor::codedom::ExceptionDeclaration* value;
        std::unique_ptr<ExceptionDeclaration> ed;
        std::u32string fromDeclarator;
        std::u32string fromAbstractDeclarator;
    };
};

void Statement::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.code.Declarator"));
    if (!grammar0)
    {
        grammar0 = cmajor::code::Declarator::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.code.Declaration"));
    if (!grammar1)
    {
        grammar1 = cmajor::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.code.Expression"));
    if (!grammar2)
    {
        grammar2 = cmajor::code::Expression::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.code.Identifier"));
    if (!grammar3)
    {
        grammar3 = cmajor::code::Identifier::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar4)
    {
        grammar4 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void Statement::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeSpecifierSeq"), this, ToUtf32("Declarator.TypeSpecifierSeq")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ConstantExpression"), this, ToUtf32("Expression.ConstantExpression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("AbstractDeclarator"), this, ToUtf32("Declarator.AbstractDeclarator")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("AssignmentExpression"), this, ToUtf32("Expression.AssignmentExpression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("Declarator.TypeId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("Declarator.Declarator")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("SimpleDeclaration"), this, ToUtf32("Declaration.SimpleDeclaration")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("BlockDeclaration"), this, ToUtf32("Declaration.BlockDeclaration")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("cmajor.parsing.stdlib.spaces_and_comments")));
    AddRule(new StatementRule(ToUtf32("Statement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("LabeledStatement"), ToUtf32("LabeledStatement"), 0)),
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("EmptyStatement"), ToUtf32("EmptyStatement"), 0))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 0))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("SelectionStatement"), ToUtf32("SelectionStatement"), 0))),
                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("IterationStatement"), ToUtf32("IterationStatement"), 0))),
                        new cmajor::parsing::ActionParser(ToUtf32("A5"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("JumpStatement"), ToUtf32("JumpStatement"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A6"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("DeclarationStatement"), ToUtf32("DeclarationStatement"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A7"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("TryStatement"), ToUtf32("TryStatement"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A8"),
                new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionStatement"), ToUtf32("ExpressionStatement"), 0)))));
    AddRule(new LabeledStatementRule(ToUtf32("LabeledStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("Label"), ToUtf32("Label"), 0),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::DifferenceParser(
                                        new cmajor::parsing::CharParser(':'),
                                        new cmajor::parsing::StringParser(ToUtf32("::"))))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("Statement"), 0))))),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::KeywordParser(ToUtf32("case")),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser(':'))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("Statement"), 0)))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::KeywordParser(ToUtf32("default")),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser(':'))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("s3"), ToUtf32("Statement"), 0))))))));
    AddRule(new LabelRule(ToUtf32("Label"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
    AddRule(new EmptyStatementRule(ToUtf32("EmptyStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::CharParser(';'))));
    AddRule(new ExpressionStatementRule(ToUtf32("ExpressionStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new CompoundStatementRule(ToUtf32("CompoundStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser('{')),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('}'))))));
    AddRule(new SelectionStatementRule(ToUtf32("SelectionStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("IfStatement"), ToUtf32("IfStatement"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("SwitchStatement"), ToUtf32("SwitchStatement"), 0)))));
    AddRule(new IfStatementRule(ToUtf32("IfStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::KeywordParser(ToUtf32("if")),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::CharParser('('))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser(')'))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("thenStatement"), ToUtf32("Statement"), 0))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("else")),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("elseStatement"), ToUtf32("Statement"), 0))))))))));
    AddRule(new SwitchStatementRule(ToUtf32("SwitchStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("switch")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('('))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))));
    AddRule(new IterationStatementRule(ToUtf32("IterationStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("WhileStatement"), ToUtf32("WhileStatement"), 0)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("DoStatement"), ToUtf32("DoStatement"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("ForStatement"), ToUtf32("ForStatement"), 0)))));
    AddRule(new WhileStatementRule(ToUtf32("WhileStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("while")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('('))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))));
    AddRule(new DoStatementRule(ToUtf32("DoStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("do")),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("while")))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('('))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new ForStatementRule(ToUtf32("ForStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::KeywordParser(ToUtf32("for")),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::CharParser('('))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("ForInitStatement"), ToUtf32("ForInitStatement"), 0))),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("Condition"), ToUtf32("Condition"), 0))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser(';'))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Statement"), ToUtf32("Statement"), 0))))));
    AddRule(new ForInitStatementRule(ToUtf32("ForInitStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("EmptyStatement"), ToUtf32("EmptyStatement"), 0)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionStatement"), ToUtf32("ExpressionStatement"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("SimpleDeclaration"), ToUtf32("SimpleDeclaration"), 0)))));
    AddRule(new JumpStatementRule(ToUtf32("JumpStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("BreakStatement"), ToUtf32("BreakStatement"), 0)),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ContinueStatement"), ToUtf32("ContinueStatement"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ReturnStatement"), ToUtf32("ReturnStatement"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::NonterminalParser(ToUtf32("GotoStatement"), ToUtf32("GotoStatement"), 0)))));
    AddRule(new BreakStatementRule(ToUtf32("BreakStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::KeywordParser(ToUtf32("break")),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new ContinueStatementRule(ToUtf32("ContinueStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::KeywordParser(ToUtf32("continue")),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new ReturnStatementRule(ToUtf32("ReturnStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::KeywordParser(ToUtf32("return")),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new GotoStatementRule(ToUtf32("GotoStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::KeywordParser(ToUtf32("goto")),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("GotoTarget"), ToUtf32("GotoTarget"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new GotoTargetRule(ToUtf32("GotoTarget"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
    AddRule(new DeclarationStatementRule(ToUtf32("DeclarationStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("BlockDeclaration"), ToUtf32("BlockDeclaration"), 0))));
    AddRule(new ConditionRule(ToUtf32("Condition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)),
                    new cmajor::parsing::CharParser('=')),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0)))));
    AddRule(new TryStatementRule(ToUtf32("TryStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::KeywordParser(ToUtf32("try")),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 0)))),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("HandlerSeq"), ToUtf32("HandlerSeq"), 1)))));
    AddRule(new HandlerSeqRule(ToUtf32("HandlerSeq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Handler"), ToUtf32("Handler"), 0))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("HandlerSeq"), ToUtf32("HandlerSeq"), 1)))));
    AddRule(new HandlerRule(ToUtf32("Handler"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("catch")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('('))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("ExceptionDeclaration"), ToUtf32("ExceptionDeclaration"), 0))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 0))))));
    AddRule(new ExceptionDeclarationRule(ToUtf32("ExceptionDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeSpecifierSeq"), ToUtf32("TypeSpecifierSeq"), 1),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0))),
                                    new cmajor::parsing::EmptyParser()))),
                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                            new cmajor::parsing::StringParser(ToUtf32("...")))))))));
    SetStartRuleName(ToUtf32("CompoundStatement"));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace cmajor.code
