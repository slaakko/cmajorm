#include "TypeExpr.hpp"
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
#include <cmajor/parser/BasicType.hpp>
#include <cmajor/parser/Expression.hpp>
#include <cmajor/parser/Template.hpp>
#include <cmajor/parser/Identifier.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

TypeExpr* TypeExpr::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

TypeExpr* TypeExpr::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    TypeExpr* grammar(new TypeExpr(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

TypeExpr::TypeExpr(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("TypeExpr"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

Node* TypeExpr::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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

class TypeExpr::TypeExprRule : public cmajor::parsing::Rule
{
public:
    TypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeExprRule>(this, &TypeExprRule::A0Action));
        cmajor::parsing::NonterminalParser* prefixTypeExprNonterminalParser = GetNonterminal(ToUtf32("PrefixTypeExpr"));
        prefixTypeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TypeExprRule>(this, &TypeExprRule::PrePrefixTypeExpr));
        prefixTypeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeExprRule>(this, &TypeExprRule::PostPrefixTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrefixTypeExpr;
    }
    void PrePrefixTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPrefixTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrefixTypeExpr_value = std::move(stack.top());
            context->fromPrefixTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromPrefixTypeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromPrefixTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromPrefixTypeExpr;
    };
};

class TypeExpr::PrefixTypeExprRule : public cmajor::parsing::Rule
{
public:
    PrefixTypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixTypeExprRule>(this, &PrefixTypeExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixTypeExprRule>(this, &PrefixTypeExprRule::A1Action));
        cmajor::parsing::NonterminalParser* constTypeExprNonterminalParser = GetNonterminal(ToUtf32("constTypeExpr"));
        constTypeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::PreconstTypeExpr));
        constTypeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::PostconstTypeExpr));
        cmajor::parsing::NonterminalParser* pfNonterminalParser = GetNonterminal(ToUtf32("pf"));
        pfNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::Prepf));
        pfNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrefixTypeExprRule>(this, &PrefixTypeExprRule::Postpf));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConstNode(span, context->fromconstTypeExpr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->frompf;
    }
    void PreconstTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostconstTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromconstTypeExpr_value = std::move(stack.top());
            context->fromconstTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromconstTypeExpr_value.get());
            stack.pop();
        }
    }
    void Prepf(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postpf(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frompf_value = std::move(stack.top());
            context->frompf = *static_cast<cmajor::parsing::ValueObject<Node*>*>(frompf_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromconstTypeExpr(), frompf() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromconstTypeExpr;
        Node* frompf;
    };
};

class TypeExpr::PostfixTypeExprRule : public cmajor::parsing::Rule
{
public:
    PostfixTypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixTypeExprRule>(this, &PostfixTypeExprRule::A6Action));
        cmajor::parsing::NonterminalParser* primaryTypeExprNonterminalParser = GetNonterminal(ToUtf32("PrimaryTypeExpr"));
        primaryTypeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::PrePrimaryTypeExpr));
        primaryTypeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::PostPrimaryTypeExpr));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* sizeNonterminalParser = GetNonterminal(ToUtf32("size"));
        sizeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::Presize));
        sizeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixTypeExprRule>(this, &PostfixTypeExprRule::Postsize));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->typeExpr.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromPrimaryTypeExpr);
        context->s = span;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new DotNode(context->s, context->typeExpr.release(), context->fromIdentifier));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new PointerNode(context->s, context->typeExpr.release()));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new RValueRefNode(context->s, context->typeExpr.release()));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new LValueRefNode(context->s, context->typeExpr.release()));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->typeExpr.reset(new ArrayNode(context->s, context->typeExpr.release(), context->fromsize));
    }
    void PrePrimaryTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPrimaryTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrimaryTypeExpr_value = std::move(stack.top());
            context->fromPrimaryTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromPrimaryTypeExpr_value.get());
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
    void Presize(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postsize(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromsize_value = std::move(stack.top());
            context->fromsize = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromsize_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), typeExpr(), s(), fromPrimaryTypeExpr(), fromIdentifier(), fromsize() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<Node> typeExpr;
        Span s;
        Node* fromPrimaryTypeExpr;
        IdentifierNode* fromIdentifier;
        Node* fromsize;
    };
};

class TypeExpr::PrimaryTypeExprRule : public cmajor::parsing::Rule
{
public:
    PrimaryTypeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::A2Action));
        cmajor::parsing::NonterminalParser* basicTypeNonterminalParser = GetNonterminal(ToUtf32("BasicType"));
        basicTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostBasicType));
        cmajor::parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal(ToUtf32("TemplateId"));
        templateIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostTemplateId));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryTypeExprRule>(this, &PrimaryTypeExprRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBasicType;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTemplateId;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdentifier;
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromBasicType(), fromTemplateId(), fromIdentifier() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromBasicType;
        Node* fromTemplateId;
        IdentifierNode* fromIdentifier;
    };
};

void TypeExpr::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Expression"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Expression::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.BasicType"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::BasicType::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar2)
    {
        grammar2 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.Template"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::Template::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar4)
    {
        grammar4 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void TypeExpr::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("BasicType"), this, ToUtf32("BasicType.BasicType")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TemplateId"), this, ToUtf32("Template.TemplateId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces"), this, ToUtf32("cmajor.parsing.stdlib.spaces")));
    AddRule(new TypeExprRule(ToUtf32("TypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("PrefixTypeExpr"), ToUtf32("PrefixTypeExpr"), 1))));
    AddRule(new PrefixTypeExprRule(ToUtf32("PrefixTypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("const")),
                        new cmajor::parsing::NonterminalParser(ToUtf32("constTypeExpr"), ToUtf32("PostfixTypeExpr"), 1)))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("pf"), ToUtf32("PostfixTypeExpr"), 1)))));
    AddRule(new PostfixTypeExprRule(ToUtf32("PostfixTypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("PrimaryTypeExpr"), ToUtf32("PrimaryTypeExpr"), 1)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::CharParser('.'),
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::ExpectationParser(
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::CharParser('*'))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                            new cmajor::parsing::StringParser(ToUtf32("&&")))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                        new cmajor::parsing::CharParser('&'))),
                                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::CharParser('['),
                                                new cmajor::parsing::OptionalParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("size"), ToUtf32("Expression"), 1))),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::CharParser(']')))))))))))));
    AddRule(new PrimaryTypeExprRule(ToUtf32("PrimaryTypeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("BasicType"), ToUtf32("BasicType"), 0)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("TemplateId"), ToUtf32("TemplateId"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)))));
    SetSkipRuleName(ToUtf32("spaces"));
}

} } // namespace cmajor.parser
