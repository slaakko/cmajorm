#include "Expression.hpp"
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
#include <cmajor/parser/Literal.hpp>
#include <cmajor/parser/BasicType.hpp>
#include <cmajor/parser/Template.hpp>
#include <cmajor/parser/Identifier.hpp>
#include <cmajor/parser/TypeExpr.hpp>
#include <cmajor/parser/Operator.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Expression* Expression::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Expression* Expression::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Expression* grammar(new Expression(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Expression::Expression(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Expression"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

Node* Expression::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<ParsingContext*>(ctx)));
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
    Node* result = *static_cast<cmajor::parsing::ValueObject<Node*>*>(value.get());
    stack.pop();
    return result;
}

class Expression::ExpressionRule : public cmajor::parsing::Rule
{
public:
    ExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A0Action));
        cmajor::parsing::NonterminalParser* equivalenceNonterminalParser = GetNonterminal(ToUtf32("Equivalence"));
        equivalenceNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExpressionRule>(this, &ExpressionRule::PreEquivalence));
        equivalenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::PostEquivalence));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEquivalence;
    }
    void PreEquivalence(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEquivalence(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEquivalence_value = std::move(stack.top());
            context->fromEquivalence = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromEquivalence_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromEquivalence() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromEquivalence;
    };
};

class Expression::EquivalenceRule : public cmajor::parsing::Rule
{
public:
    EquivalenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EquivalenceRule>(this, &EquivalenceRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EquivalenceRule>(this, &EquivalenceRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EquivalenceRule>(this, &EquivalenceRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EquivalenceRule>(this, &EquivalenceRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EquivalenceRule>(this, &EquivalenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EquivalenceRule>(this, &EquivalenceRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EquivalenceRule>(this, &EquivalenceRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EquivalenceRule>(this, &EquivalenceRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (!context->ctx->ParsingConcept() || context->ctx->ParsingTemplateId()) pass = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new EquivalenceNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::ImplicationRule : public cmajor::parsing::Rule
{
public:
    ImplicationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ImplicationRule>(this, &ImplicationRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ImplicationRule>(this, &ImplicationRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ImplicationRule>(this, &ImplicationRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ImplicationRule>(this, &ImplicationRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ImplicationRule>(this, &ImplicationRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ImplicationRule>(this, &ImplicationRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ImplicationRule>(this, &ImplicationRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ImplicationRule>(this, &ImplicationRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (!context->ctx->ParsingConcept() || context->ctx->ParsingTemplateId()) pass = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new ImplicationNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::DisjunctionRule : public cmajor::parsing::Rule
{
public:
    DisjunctionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DisjunctionRule>(this, &DisjunctionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DisjunctionRule>(this, &DisjunctionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DisjunctionRule>(this, &DisjunctionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DisjunctionRule>(this, &DisjunctionRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DisjunctionRule>(this, &DisjunctionRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DisjunctionRule>(this, &DisjunctionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DisjunctionRule>(this, &DisjunctionRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DisjunctionRule>(this, &DisjunctionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new DisjunctionNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::ConjunctionRule : public cmajor::parsing::Rule
{
public:
    ConjunctionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConjunctionRule>(this, &ConjunctionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConjunctionRule>(this, &ConjunctionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConjunctionRule>(this, &ConjunctionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConjunctionRule>(this, &ConjunctionRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConjunctionRule>(this, &ConjunctionRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConjunctionRule>(this, &ConjunctionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConjunctionRule>(this, &ConjunctionRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConjunctionRule>(this, &ConjunctionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new ConjunctionNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::BitOrRule : public cmajor::parsing::Rule
{
public:
    BitOrRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitOrRule>(this, &BitOrRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitOrRule>(this, &BitOrRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitOrRule>(this, &BitOrRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitOrRule>(this, &BitOrRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<BitOrRule>(this, &BitOrRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BitOrRule>(this, &BitOrRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<BitOrRule>(this, &BitOrRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BitOrRule>(this, &BitOrRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new BitOrNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::BitXorRule : public cmajor::parsing::Rule
{
public:
    BitXorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitXorRule>(this, &BitXorRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitXorRule>(this, &BitXorRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitXorRule>(this, &BitXorRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitXorRule>(this, &BitXorRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<BitXorRule>(this, &BitXorRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BitXorRule>(this, &BitXorRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<BitXorRule>(this, &BitXorRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BitXorRule>(this, &BitXorRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new BitXorNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::BitAndRule : public cmajor::parsing::Rule
{
public:
    BitAndRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitAndRule>(this, &BitAndRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitAndRule>(this, &BitAndRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitAndRule>(this, &BitAndRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BitAndRule>(this, &BitAndRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<BitAndRule>(this, &BitAndRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BitAndRule>(this, &BitAndRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<BitAndRule>(this, &BitAndRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BitAndRule>(this, &BitAndRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new BitAndNode(context->s, context->expr.release(), context->fromright));
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::EqualityRule : public cmajor::parsing::Rule
{
public:
    EqualityRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityRule>(this, &EqualityRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityRule>(this, &EqualityRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityRule>(this, &EqualityRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityRule>(this, &EqualityRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityRule>(this, &EqualityRule::A4Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EqualityRule>(this, &EqualityRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EqualityRule>(this, &EqualityRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EqualityRule>(this, &EqualityRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EqualityRule>(this, &EqualityRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::eq;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::neq;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        switch (context->op)
        {
            case Operator::eq: context->expr.reset(new EqualNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::neq: context->expr.reset(new NotEqualNode(context->s, context->expr.release(), context->fromright));
            break;
        }
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::RelationalRule : public cmajor::parsing::Rule
{
public:
    RelationalRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalRule>(this, &RelationalRule::A9Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RelationalRule>(this, &RelationalRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalRule>(this, &RelationalRule::Postleft));
        cmajor::parsing::NonterminalParser* isTypeNonterminalParser = GetNonterminal(ToUtf32("isType"));
        isTypeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RelationalRule>(this, &RelationalRule::PreisType));
        isTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalRule>(this, &RelationalRule::PostisType));
        cmajor::parsing::NonterminalParser* asTypeNonterminalParser = GetNonterminal(ToUtf32("asType"));
        asTypeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RelationalRule>(this, &RelationalRule::PreasType));
        asTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalRule>(this, &RelationalRule::PostasType));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RelationalRule>(this, &RelationalRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalRule>(this, &RelationalRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else
        {
            context->op = Operator::lessOrEq;
            context->ctx->PushParsingIsOrAs(false);
        }
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else
        {
            context->op = Operator::greaterOrEq;
            context->ctx->PushParsingIsOrAs(false);
        }
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else
        {
            context->op = Operator::less;
            context->ctx->PushParsingIsOrAs(false);
        }
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else
        {
            context->op = Operator::greater;
            context->ctx->PushParsingIsOrAs(false);
        }
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else
        {
            context->op = Operator::is;
            context->ctx->PushParsingIsOrAs(true);
        }
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else
        {
            context->op = Operator::as;
            context->ctx->PushParsingIsOrAs(true);
        }
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->PopParsingIsOrAs();
        context->s.SetEnd(span.End());
        switch (context->op)
        {
            case Operator::lessOrEq: context->expr.reset(new LessOrEqualNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::greaterOrEq: context->expr.reset(new GreaterOrEqualNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::less: context->expr.reset(new LessNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::greater: context->expr.reset(new GreaterNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::is: context->expr.reset(new IsNode(context->s, context->expr.release(), context->fromisType));
            break;
            case Operator::as: context->expr.reset(new AsNode(context->s, context->expr.release(), context->fromasType));
            break;
        }
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (!context->ctx->ParsingIsOrAs()) pass = false;
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PreisType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostisType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromisType_value = std::move(stack.top());
            context->fromisType = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromisType_value.get());
            stack.pop();
        }
    }
    void PreasType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostasType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromasType_value = std::move(stack.top());
            context->fromasType = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromasType_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromisType(), fromasType(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromisType;
        Node* fromasType;
        Node* fromright;
    };
};

class Expression::ShiftRule : public cmajor::parsing::Rule
{
public:
    ShiftRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftRule>(this, &ShiftRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftRule>(this, &ShiftRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftRule>(this, &ShiftRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftRule>(this, &ShiftRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftRule>(this, &ShiftRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftRule>(this, &ShiftRule::A5Action));
        a5ActionParser->SetFailureAction(new cmajor::parsing::MemberFailureAction<ShiftRule>(this, &ShiftRule::A5ActionFail));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ShiftRule>(this, &ShiftRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ShiftRule>(this, &ShiftRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ShiftRule>(this, &ShiftRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ShiftRule>(this, &ShiftRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue()) pass = false;
        else context->op = Operator::shiftLeft;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue()) pass = false;
        else context->op = Operator::shiftRight;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingArguments();
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        switch (context->op)
        {
            case Operator::shiftLeft: context->expr.reset(new ShiftLeftNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::shiftRight: context->expr.reset(new ShiftRightNode(context->s, context->expr.release(), context->fromright));
            break;
        }
        context->ctx->EndParsingArguments();
    }
    void A5ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingArguments();
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::AdditiveRule : public cmajor::parsing::Rule
{
public:
    AdditiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveRule>(this, &AdditiveRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveRule>(this, &AdditiveRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveRule>(this, &AdditiveRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveRule>(this, &AdditiveRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveRule>(this, &AdditiveRule::A4Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AdditiveRule>(this, &AdditiveRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AdditiveRule>(this, &AdditiveRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AdditiveRule>(this, &AdditiveRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AdditiveRule>(this, &AdditiveRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::plus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::minus;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        switch (context->op)
        {
            case Operator::plus: context->expr.reset(new AddNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::minus: context->expr.reset(new SubNode(context->s, context->expr.release(), context->fromright));
            break;
        }
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::MultiplicativeRule : public cmajor::parsing::Rule
{
public:
    MultiplicativeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeRule>(this, &MultiplicativeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeRule>(this, &MultiplicativeRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeRule>(this, &MultiplicativeRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeRule>(this, &MultiplicativeRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeRule>(this, &MultiplicativeRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeRule>(this, &MultiplicativeRule::A5Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MultiplicativeRule>(this, &MultiplicativeRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiplicativeRule>(this, &MultiplicativeRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MultiplicativeRule>(this, &MultiplicativeRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiplicativeRule>(this, &MultiplicativeRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(context->fromleft);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::mul;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::div;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->ctx->ParsingLvalue() || context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::rem;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        switch (context->op)
        {
            case Operator::mul: context->expr.reset(new MulNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::div: context->expr.reset(new DivNode(context->s, context->expr.release(), context->fromright));
            break;
            case Operator::rem: context->expr.reset(new RemNode(context->s, context->expr.release(), context->fromright));
            break;
        }
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), op(), fromleft(), fromright() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Operator op;
        Node* fromleft;
        Node* fromright;
    };
};

class Expression::PrefixRule : public cmajor::parsing::Rule
{
public:
    PrefixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A9Action));
        cmajor::parsing::NonterminalParser* prefixNonterminalParser = GetNonterminal(ToUtf32("prefix"));
        prefixNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrefixRule>(this, &PrefixRule::Preprefix));
        prefixNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrefixRule>(this, &PrefixRule::Postprefix));
        cmajor::parsing::NonterminalParser* postfixNonterminalParser = GetNonterminal(ToUtf32("Postfix"));
        postfixNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrefixRule>(this, &PrefixRule::PrePostfix));
        postfixNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrefixRule>(this, &PrefixRule::PostPostfix));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        context->op = Operator::plusplus;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        context->op = Operator::minusminus;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        if (context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::minus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        if (context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::plus;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        if (context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::not_;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        if (context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::complement;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        if (context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::deref;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        if (context->ctx->ParsingExpressionStatement() && !context->ctx->ParsingArguments()) pass = false;
        else context->op = Operator::addrOf;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        switch (context->op)
        {
            case Operator::plusplus: context->value = new PrefixIncrementNode(context->s, context->fromprefix);
            break;
            case Operator::minusminus: context->value = new PrefixDecrementNode(context->s, context->fromprefix);
            break;
            case Operator::minus: context->value = new UnaryMinusNode(context->s, context->fromprefix);
            break;
            case Operator::plus: context->value = new UnaryPlusNode(context->s, context->fromprefix);
            break;
            case Operator::not_: context->value = new NotNode(context->s, context->fromprefix);
            break;
            case Operator::complement: context->value = new ComplementNode(context->s, context->fromprefix);
            break;
            case Operator::deref: context->value = new DerefNode(context->s, context->fromprefix);
            break;
            case Operator::addrOf: context->value = new AddrOfNode(context->s, context->fromprefix);
            break;
        }
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPostfix;
    }
    void Preprefix(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postprefix(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromprefix_value = std::move(stack.top());
            context->fromprefix = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromprefix_value.get());
            stack.pop();
        }
    }
    void PrePostfix(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPostfix(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPostfix_value = std::move(stack.top());
            context->fromPostfix = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromPostfix_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), s(), op(), fromprefix(), fromPostfix() {}
        ParsingContext* ctx;
        Node* value;
        Span s;
        Operator op;
        Node* fromprefix;
        Node* fromPostfix;
    };
};

class Expression::PostfixRule : public cmajor::parsing::Rule
{
public:
    PostfixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A8Action));
        a8ActionParser->SetFailureAction(new cmajor::parsing::MemberFailureAction<PostfixRule>(this, &PostfixRule::A8ActionFail));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A10Action));
        cmajor::parsing::NonterminalParser* primaryNonterminalParser = GetNonterminal(ToUtf32("Primary"));
        primaryNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PostfixRule>(this, &PostfixRule::PrePrimary));
        primaryNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::PostPrimary));
        cmajor::parsing::NonterminalParser* dotMemberIdNonterminalParser = GetNonterminal(ToUtf32("dotMemberId"));
        dotMemberIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::PostdotMemberId));
        cmajor::parsing::NonterminalParser* arrowMemberIdNonterminalParser = GetNonterminal(ToUtf32("arrowMemberId"));
        arrowMemberIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::PostarrowMemberId));
        cmajor::parsing::NonterminalParser* indexNonterminalParser = GetNonterminal(ToUtf32("index"));
        indexNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PostfixRule>(this, &PostfixRule::Preindex));
        indexNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::Postindex));
        cmajor::parsing::NonterminalParser* argumentListNonterminalParser = GetNonterminal(ToUtf32("ArgumentList"));
        argumentListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PostfixRule>(this, &PostfixRule::PreArgumentList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        context->expr.reset(context->fromPrimary);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new PostfixIncrementNode(context->s, context->expr.release()));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new PostfixDecrementNode(context->s, context->expr.release()));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new DotNode(context->s, context->expr.release(), context->fromdotMemberId));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new ArrowNode(context->s, context->expr.release(), context->fromarrowMemberId));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new IndexingNode(context->s, context->expr.release(), context->fromindex));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->PushParsingLvalue(false);
        context->ctx->PushParsingExpressionStatement(false);
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->PopParsingLvalue();
        context->ctx->PopParsingExpressionStatement();
    }
    void A8ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->PopParsingLvalue();
        context->ctx->PopParsingExpressionStatement();
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr->GetSpan().SetEnd(span.End());
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(new InvokeNode(context->s, context->expr.release()));
    }
    void PrePrimary(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPrimary(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrimary_value = std::move(stack.top());
            context->fromPrimary = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromPrimary_value.get());
            stack.pop();
        }
    }
    void PostdotMemberId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromdotMemberId_value = std::move(stack.top());
            context->fromdotMemberId = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromdotMemberId_value.get());
            stack.pop();
        }
    }
    void PostarrowMemberId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromarrowMemberId_value = std::move(stack.top());
            context->fromarrowMemberId = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromarrowMemberId_value.get());
            stack.pop();
        }
    }
    void Preindex(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postindex(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromindex_value = std::move(stack.top());
            context->fromindex = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromindex_value.get());
            stack.pop();
        }
    }
    void PreArgumentList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->expr.get())));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromPrimary(), fromdotMemberId(), fromarrowMemberId(), fromindex() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromPrimary;
        IdentifierNode* fromdotMemberId;
        IdentifierNode* fromarrowMemberId;
        Node* fromindex;
    };
};

class Expression::PrimaryRule : public cmajor::parsing::Rule
{
public:
    PrimaryRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A10Action));
        cmajor::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryRule>(this, &PrimaryRule::A11Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostExpression));
        cmajor::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreLiteral));
        literalNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostLiteral));
        cmajor::parsing::NonterminalParser* basicTypeNonterminalParser = GetNonterminal(ToUtf32("BasicType"));
        basicTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostBasicType));
        cmajor::parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal(ToUtf32("TemplateId"));
        templateIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostTemplateId));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* sizeOfExprNonterminalParser = GetNonterminal(ToUtf32("SizeOfExpr"));
        sizeOfExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreSizeOfExpr));
        sizeOfExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostSizeOfExpr));
        cmajor::parsing::NonterminalParser* typeNameExprNonterminalParser = GetNonterminal(ToUtf32("TypeNameExpr"));
        typeNameExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreTypeNameExpr));
        typeNameExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostTypeNameExpr));
        cmajor::parsing::NonterminalParser* castExprNonterminalParser = GetNonterminal(ToUtf32("CastExpr"));
        castExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreCastExpr));
        castExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostCastExpr));
        cmajor::parsing::NonterminalParser* constructExprNonterminalParser = GetNonterminal(ToUtf32("ConstructExpr"));
        constructExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreConstructExpr));
        constructExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostConstructExpr));
        cmajor::parsing::NonterminalParser* newExprNonterminalParser = GetNonterminal(ToUtf32("NewExpr"));
        newExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreNewExpr));
        newExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostNewExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParenthesizedExpressionNode(span, context->fromExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLiteral;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBasicType;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTemplateId;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ThisNode(span);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BaseNode(span);
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSizeOfExpr;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeNameExpr;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCastExpr;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConstructExpr;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNewExpr;
    }
    void PreExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PreLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostBasicType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromBasicType_value = std::move(stack.top());
            context->fromBasicType = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromBasicType_value.get());
            stack.pop();
        }
    }
    void PreTemplateId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTemplateId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTemplateId_value = std::move(stack.top());
            context->fromTemplateId = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTemplateId_value.get());
            stack.pop();
        }
    }
    void PostIdentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreSizeOfExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostSizeOfExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSizeOfExpr_value = std::move(stack.top());
            context->fromSizeOfExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromSizeOfExpr_value.get());
            stack.pop();
        }
    }
    void PreTypeNameExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeNameExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeNameExpr_value = std::move(stack.top());
            context->fromTypeNameExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTypeNameExpr_value.get());
            stack.pop();
        }
    }
    void PreCastExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCastExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCastExpr_value = std::move(stack.top());
            context->fromCastExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromCastExpr_value.get());
            stack.pop();
        }
    }
    void PreConstructExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConstructExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstructExpr_value = std::move(stack.top());
            context->fromConstructExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromConstructExpr_value.get());
            stack.pop();
        }
    }
    void PreNewExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostNewExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNewExpr_value = std::move(stack.top());
            context->fromNewExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromNewExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromExpression(), fromLiteral(), fromBasicType(), fromTemplateId(), fromIdentifier(), fromSizeOfExpr(), fromTypeNameExpr(), fromCastExpr(), fromConstructExpr(), fromNewExpr() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpression;
        LiteralNode* fromLiteral;
        Node* fromBasicType;
        Node* fromTemplateId;
        IdentifierNode* fromIdentifier;
        Node* fromSizeOfExpr;
        Node* fromTypeNameExpr;
        Node* fromCastExpr;
        Node* fromConstructExpr;
        Node* fromNewExpr;
    };
};

class Expression::SizeOfExprRule : public cmajor::parsing::Rule
{
public:
    SizeOfExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SizeOfExprRule>(this, &SizeOfExprRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SizeOfExprRule>(this, &SizeOfExprRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SizeOfExprRule>(this, &SizeOfExprRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SizeOfNode(span, context->fromExpression);
    }
    void PreExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpression;
    };
};

class Expression::TypeNameExprRule : public cmajor::parsing::Rule
{
public:
    TypeNameExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeNameExprRule>(this, &TypeNameExprRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TypeNameExprRule>(this, &TypeNameExprRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeNameExprRule>(this, &TypeNameExprRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeNameNode(span, context->fromExpression);
    }
    void PreExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromExpression;
    };
};

class Expression::CastExprRule : public cmajor::parsing::Rule
{
public:
    CastExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CastExprRule>(this, &CastExprRule::A0Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<CastExprRule>(this, &CastExprRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CastExprRule>(this, &CastExprRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<CastExprRule>(this, &CastExprRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CastExprRule>(this, &CastExprRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CastNode(span, context->fromTypeExpr, context->fromExpression);
    }
    void PreTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PreExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromTypeExpr(), fromExpression() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTypeExpr;
        Node* fromExpression;
    };
};

class Expression::ConstructExprRule : public cmajor::parsing::Rule
{
public:
    ConstructExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructExprRule>(this, &ConstructExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructExprRule>(this, &ConstructExprRule::A1Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstructExprRule>(this, &ConstructExprRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructExprRule>(this, &ConstructExprRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstructExprRule>(this, &ConstructExprRule::PreExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConstructNode(span, context->fromTypeExpr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->GetSpan().SetEnd(span.End());
    }
    void PreTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PreExpressionList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTypeExpr;
    };
};

class Expression::NewExprRule : public cmajor::parsing::Rule
{
public:
    NewExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewExprRule>(this, &NewExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewExprRule>(this, &NewExprRule::A1Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NewExprRule>(this, &NewExprRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewExprRule>(this, &NewExprRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* argumentListNonterminalParser = GetNonterminal(ToUtf32("ArgumentList"));
        argumentListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NewExprRule>(this, &NewExprRule::PreArgumentList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NewNode(span, context->fromTypeExpr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->GetSpan().SetEnd(span.End());
    }
    void PreTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
    void PreArgumentList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTypeExpr;
    };
};

class Expression::ArgumentListRule : public cmajor::parsing::Rule
{
public:
    ArgumentListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("Node*"), ToUtf32("node")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> node_value = std::move(stack.top());
        context->node = *static_cast<cmajor::parsing::ValueObject<Node*>*>(node_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ArgumentListRule>(this, &ArgumentListRule::PreExpressionList));
    }
    void PreExpressionList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->node)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), node() {}
        ParsingContext* ctx;
        Node* node;
    };
};

class Expression::ExpressionListRule : public cmajor::parsing::Rule
{
public:
    ExpressionListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("Node*"), ToUtf32("node")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> node_value = std::move(stack.top());
        context->node = *static_cast<cmajor::parsing::ValueObject<Node*>*>(node_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A1Action));
        a1ActionParser->SetFailureAction(new cmajor::parsing::MemberFailureAction<ExpressionListRule>(this, &ExpressionListRule::A1ActionFail));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A2Action));
        cmajor::parsing::NonterminalParser* argNonterminalParser = GetNonterminal(ToUtf32("arg"));
        argNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExpressionListRule>(this, &ExpressionListRule::Prearg));
        argNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExpressionListRule>(this, &ExpressionListRule::Postarg));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingArguments();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingArguments();
    }
    void A1ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingArguments();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->node->AddArgument(context->fromarg);
    }
    void Prearg(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postarg(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromarg_value = std::move(stack.top());
            context->fromarg = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromarg_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), node(), fromarg() {}
        ParsingContext* ctx;
        Node* node;
        Node* fromarg;
    };
};

class Expression::InvokeExprRule : public cmajor::parsing::Rule
{
public:
    InvokeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("expr")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InvokeExprRule>(this, &InvokeExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InvokeExprRule>(this, &InvokeExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InvokeExprRule>(this, &InvokeExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InvokeExprRule>(this, &InvokeExprRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InvokeExprRule>(this, &InvokeExprRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InvokeExprRule>(this, &InvokeExprRule::A5Action));
        cmajor::parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal(ToUtf32("TemplateId"));
        templateIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InvokeExprRule>(this, &InvokeExprRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InvokeExprRule>(this, &InvokeExprRule::PostTemplateId));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InvokeExprRule>(this, &InvokeExprRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* dotMemberIdNonterminalParser = GetNonterminal(ToUtf32("dotMemberId"));
        dotMemberIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InvokeExprRule>(this, &InvokeExprRule::PostdotMemberId));
        cmajor::parsing::NonterminalParser* argumentListNonterminalParser = GetNonterminal(ToUtf32("ArgumentList"));
        argumentListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InvokeExprRule>(this, &InvokeExprRule::PreArgumentList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->expr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        context->expr.reset(context->fromTemplateId);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
        context->expr.reset(context->fromIdentifier);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->expr.reset(new DotNode(context->s, context->expr.release(), context->fromdotMemberId));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr.reset(new InvokeNode(context->s, context->expr.release()));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->expr->GetSpan().SetEnd(span.End());
    }
    void PreTemplateId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTemplateId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTemplateId_value = std::move(stack.top());
            context->fromTemplateId = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTemplateId_value.get());
            stack.pop();
        }
    }
    void PostIdentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PostdotMemberId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromdotMemberId_value = std::move(stack.top());
            context->fromdotMemberId = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromdotMemberId_value.get());
            stack.pop();
        }
    }
    void PreArgumentList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->expr.get())));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), expr(), s(), fromTemplateId(), fromIdentifier(), fromdotMemberId() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> expr;
        Span s;
        Node* fromTemplateId;
        IdentifierNode* fromIdentifier;
        IdentifierNode* fromdotMemberId;
    };
};

void Expression::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Literal"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Literal::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.BasicType"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::BasicType::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Template"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Template::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parser.TypeExpr"));
    if (!grammar4)
    {
        grammar4 = cmajor::parser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar4);
    cmajor::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar5)
    {
        grammar5 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar5);
}

void Expression::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Literal"), this, ToUtf32("Literal.Literal")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("BasicType"), this, ToUtf32("BasicType.BasicType")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TemplateId"), this, ToUtf32("Template.TemplateId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("cmajor.parsing.stdlib.identifier")));
    AddRule(new ExpressionRule(ToUtf32("Expression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Equivalence"), ToUtf32("Equivalence"), 1))));
    AddRule(new EquivalenceRule(ToUtf32("Equivalence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Implication"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::StringParser(ToUtf32("<=>"))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Implication"), 1)))))))))));
    AddRule(new ImplicationRule(ToUtf32("Implication"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Disjunction"), 1)),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::StringParser(ToUtf32("=>"))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Implication"), 1)))))))))));
    AddRule(new DisjunctionRule(ToUtf32("Disjunction"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Conjunction"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::StringParser(ToUtf32("||"))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Conjunction"), 1)))))))))));
    AddRule(new ConjunctionRule(ToUtf32("Conjunction"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("BitOr"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::StringParser(ToUtf32("&&"))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("BitOr"), 1)))))))))));
    AddRule(new BitOrRule(ToUtf32("BitOr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("BitXor"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::DifferenceParser(
                                            new cmajor::parsing::CharParser('|'),
                                            new cmajor::parsing::StringParser(ToUtf32("||"))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("BitXor"), 1)))))))))));
    AddRule(new BitXorRule(ToUtf32("BitXor"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("BitAnd"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::CharParser('^')),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("BitAnd"), 1)))))))))));
    AddRule(new BitAndRule(ToUtf32("BitAnd"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Equality"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::DifferenceParser(
                                            new cmajor::parsing::CharParser('&'),
                                            new cmajor::parsing::StringParser(ToUtf32("&&"))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Equality"), 1)))))))))));
    AddRule(new EqualityRule(ToUtf32("Equality"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Relational"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::StringParser(ToUtf32("=="))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::StringParser(ToUtf32("!="))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Relational"), 1)))))))))));
    AddRule(new RelationalRule(ToUtf32("Relational"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Shift"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::AlternativeParser(
                                                    new cmajor::parsing::AlternativeParser(
                                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                            new cmajor::parsing::GroupingParser(
                                                                new cmajor::parsing::DifferenceParser(
                                                                    new cmajor::parsing::StringParser(ToUtf32("<=")),
                                                                    new cmajor::parsing::StringParser(ToUtf32("<=>"))))),
                                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                            new cmajor::parsing::StringParser(ToUtf32(">=")))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                        new cmajor::parsing::GroupingParser(
                                                            new cmajor::parsing::DifferenceParser(
                                                                new cmajor::parsing::CharParser('<'),
                                                                new cmajor::parsing::GroupingParser(
                                                                    new cmajor::parsing::AlternativeParser(
                                                                        new cmajor::parsing::StringParser(ToUtf32("<<")),
                                                                        new cmajor::parsing::StringParser(ToUtf32("<=>")))))))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::DifferenceParser(
                                                            new cmajor::parsing::CharParser('>'),
                                                            new cmajor::parsing::StringParser(ToUtf32(">>")))))),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                                        new cmajor::parsing::KeywordParser(ToUtf32("is"))),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("isType"), ToUtf32("TypeExpr"), 1)))),
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                                    new cmajor::parsing::KeywordParser(ToUtf32("as"))),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("asType"), ToUtf32("TypeExpr"), 1))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Shift"), 1),
                                            new cmajor::parsing::ActionParser(ToUtf32("A9"),
                                                new cmajor::parsing::EmptyParser()))))))))))));
    AddRule(new ShiftRule(ToUtf32("Shift"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Additive"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::StringParser(ToUtf32("<<"))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::StringParser(ToUtf32(">>"))))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                        new cmajor::parsing::EmptyParser())),
                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Additive"), 1)))))))))));
    AddRule(new AdditiveRule(ToUtf32("Additive"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Multiplicative"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::DifferenceParser(
                                                    new cmajor::parsing::CharParser('+'),
                                                    new cmajor::parsing::StringParser(ToUtf32("++"))))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::DifferenceParser(
                                                    new cmajor::parsing::CharParser('-'),
                                                    new cmajor::parsing::StringParser(ToUtf32("--"))))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Multiplicative"), 1)))))))))));
    AddRule(new MultiplicativeRule(ToUtf32("Multiplicative"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Prefix"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::CharParser('*')),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::CharParser('/'))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                            new cmajor::parsing::CharParser('%')))),
                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Prefix"), 1)))))))))));
    AddRule(new PrefixRule(ToUtf32("Prefix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::AlternativeParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                                        new cmajor::parsing::StringParser(ToUtf32("++"))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                        new cmajor::parsing::StringParser(ToUtf32("--")))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::CharParser('-'))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::CharParser('+'))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::DifferenceParser(
                                                    new cmajor::parsing::CharParser('!'),
                                                    new cmajor::parsing::StringParser(ToUtf32("!=")))))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                        new cmajor::parsing::CharParser('~'))),
                                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                    new cmajor::parsing::CharParser('*'))),
                            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::DifferenceParser(
                                        new cmajor::parsing::CharParser('&'),
                                        new cmajor::parsing::StringParser(ToUtf32("&&"))))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A8"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("prefix"), ToUtf32("Prefix"), 1)))),
            new cmajor::parsing::ActionParser(ToUtf32("A9"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Postfix"), ToUtf32("Postfix"), 1)))));
    AddRule(new PostfixRule(ToUtf32("Postfix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Primary"), ToUtf32("Primary"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::StringParser(ToUtf32("++"))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                    new cmajor::parsing::StringParser(ToUtf32("--")))),
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::CharParser('.'),
                                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("dotMemberId"), ToUtf32("Identifier"), 0)))),
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::StringParser(ToUtf32("->")),
                                            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("arrowMemberId"), ToUtf32("Identifier"), 0)))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                                        new cmajor::parsing::CharParser('[')),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("index"), ToUtf32("Expression"), 1))),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::CharParser(']')))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A9"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A10"),
                                                    new cmajor::parsing::CharParser('(')),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("ArgumentList"), ToUtf32("ArgumentList"), 2)),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::CharParser(')')))))))))))));
    AddRule(new PrimaryRule(ToUtf32("Primary"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::AlternativeParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                                        new cmajor::parsing::GroupingParser(
                                                            new cmajor::parsing::SequenceParser(
                                                                new cmajor::parsing::SequenceParser(
                                                                    new cmajor::parsing::CharParser('('),
                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1)),
                                                                new cmajor::parsing::CharParser(')')))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 1))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("BasicType"), ToUtf32("BasicType"), 0))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("TemplateId"), ToUtf32("TemplateId"), 1))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                        new cmajor::parsing::KeywordParser(ToUtf32("this"), ToUtf32("identifier")))),
                                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                    new cmajor::parsing::KeywordParser(ToUtf32("base"), ToUtf32("identifier")))),
                            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("SizeOfExpr"), ToUtf32("SizeOfExpr"), 1))),
                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeNameExpr"), ToUtf32("TypeNameExpr"), 1))),
                    new cmajor::parsing::ActionParser(ToUtf32("A9"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("CastExpr"), ToUtf32("CastExpr"), 1))),
                new cmajor::parsing::ActionParser(ToUtf32("A10"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ConstructExpr"), ToUtf32("ConstructExpr"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A11"),
                new cmajor::parsing::NonterminalParser(ToUtf32("NewExpr"), ToUtf32("NewExpr"), 1)))));
    AddRule(new SizeOfExprRule(ToUtf32("SizeOfExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::KeywordParser(ToUtf32("sizeof")),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('('))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(')')))))));
    AddRule(new TypeNameExprRule(ToUtf32("TypeNameExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::KeywordParser(ToUtf32("typename")),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('('))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(')')))))));
    AddRule(new CastExprRule(ToUtf32("CastExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::KeywordParser(ToUtf32("cast")),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::CharParser('<'))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser('>'))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('('))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(')')))))));
    AddRule(new ConstructExprRule(ToUtf32("ConstructExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::KeywordParser(ToUtf32("construct")),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::CharParser('<'))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('>'))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('('))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 2))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new NewExprRule(ToUtf32("NewExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("new")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('('),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("ArgumentList"), ToUtf32("ArgumentList"), 2)),
                                new cmajor::parsing::CharParser(')')))))))));
    AddRule(new ArgumentListRule(ToUtf32("ArgumentList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::OptionalParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 2))));
    AddRule(new ExpressionListRule(ToUtf32("ExpressionList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ListParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("arg"), ToUtf32("Expression"), 1))),
                        new cmajor::parsing::CharParser(',')))))));
    AddRule(new InvokeExprRule(ToUtf32("InvokeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("TemplateId"), ToUtf32("TemplateId"), 1)),
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)))),
                                new cmajor::parsing::KleeneStarParser(
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::CharParser('.'),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("dotMemberId"), ToUtf32("Identifier"), 0))))))),
                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                new cmajor::parsing::CharParser('('))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ArgumentList"), ToUtf32("ArgumentList"), 2)),
                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser(')'))))))));
}

} } // namespace cmajor.parser
