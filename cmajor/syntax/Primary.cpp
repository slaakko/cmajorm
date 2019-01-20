#include "Primary.hpp"
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
#include <cmajor/code/Statement.hpp>
#include <cmajor/syntax/Primitive.hpp>
#include <cmajor/syntax/Composite.hpp>
#include <cmajor/syntax/Element.hpp>
#include <cmajor/parsing/Nonterminal.hpp>
#include <cmajor/codedom/Statement.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::code;
using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Primary* Primary::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Primary* Primary::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Primary* grammar(new Primary(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Primary::Primary(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Primary"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::parsing::Parser* Primary::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Scope* enclosingScope)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<cmajor::parsing::Scope*>(enclosingScope)));
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
    cmajor::parsing::Parser* result = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(value.get());
    stack.pop();
    return result;
}

class Primary::PrimaryRule : public cmajor::parsing::Rule
{
public:
    PrimaryRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
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
        cmajor::parsing::NonterminalParser* ruleCallNonterminalParser = GetNonterminal(ToUtf32("RuleCall"));
        ruleCallNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostRuleCall));
        cmajor::parsing::NonterminalParser* primitiveNonterminalParser = GetNonterminal(ToUtf32("Primitive"));
        primitiveNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostPrimitive));
        cmajor::parsing::NonterminalParser* groupingNonterminalParser = GetNonterminal(ToUtf32("Grouping"));
        groupingNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreGrouping));
        groupingNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostGrouping));
        cmajor::parsing::NonterminalParser* tokenNonterminalParser = GetNonterminal(ToUtf32("Token"));
        tokenNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreToken));
        tokenNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostToken));
        cmajor::parsing::NonterminalParser* expectationNonterminalParser = GetNonterminal(ToUtf32("Expectation"));
        expectationNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreExpectation));
        expectationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostExpectation));
        cmajor::parsing::NonterminalParser* actionNonterminalParser = GetNonterminal(ToUtf32("Action"));
        actionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryRule>(this, &PrimaryRule::PreAction));
        actionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryRule>(this, &PrimaryRule::PostAction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRuleCall;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrimitive;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGrouping;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromToken;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpectation;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAction;
    }
    void PostRuleCall(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromRuleCall_value = std::move(stack.top());
            context->fromRuleCall = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromRuleCall_value.get());
            stack.pop();
        }
    }
    void PostPrimitive(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrimitive_value = std::move(stack.top());
            context->fromPrimitive = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromPrimitive_value.get());
            stack.pop();
        }
    }
    void PreGrouping(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void PostGrouping(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGrouping_value = std::move(stack.top());
            context->fromGrouping = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromGrouping_value.get());
            stack.pop();
        }
    }
    void PreToken(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void PostToken(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromToken_value = std::move(stack.top());
            context->fromToken = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromToken_value.get());
            stack.pop();
        }
    }
    void PreExpectation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
    }
    void PostExpectation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpectation_value = std::move(stack.top());
            context->fromExpectation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromExpectation_value.get());
            stack.pop();
        }
    }
    void PreAction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
    }
    void PostAction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAction_value = std::move(stack.top());
            context->fromAction = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromAction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromRuleCall(), fromPrimitive(), fromGrouping(), fromToken(), fromExpectation(), fromAction() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromRuleCall;
        cmajor::parsing::Parser* fromPrimitive;
        cmajor::parsing::Parser* fromGrouping;
        cmajor::parsing::Parser* fromToken;
        cmajor::parsing::Parser* fromExpectation;
        cmajor::parsing::Parser* fromAction;
    };
};

class Primary::RuleCallRule : public cmajor::parsing::Rule
{
public:
    RuleCallRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RuleCallRule>(this, &RuleCallRule::A0Action));
        cmajor::parsing::NonterminalParser* ntWithArgsNonterminalParser = GetNonterminal(ToUtf32("ntWithArgs"));
        ntWithArgsNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostntWithArgs));
        cmajor::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostExpressionList));
        cmajor::parsing::NonterminalParser* ntNonterminalParser = GetNonterminal(ToUtf32("nt"));
        ntNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::Postnt));
        cmajor::parsing::NonterminalParser* aliasNonterminalParser = GetNonterminal(ToUtf32("Alias"));
        aliasNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleCallRule>(this, &RuleCallRule::PostAlias));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::u32string ruleName;
        if (!context->fromntWithArgs.empty())
        {
            ruleName = context->fromntWithArgs;
        }
        else
        {
            ruleName = context->fromnt;
        }
        std::u32string alias = context->fromAlias;
        std::u32string name = !alias.empty() ? alias : ruleName;
        NonterminalParser * parser(new NonterminalParser(name, ruleName));
        parser->SetSpan(span);
        if (!context->fromExpressionList.empty())
        {
            parser->SetArguments(context->fromExpressionList);
        }
        context->value = parser;
    }
    void PostntWithArgs(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromntWithArgs_value = std::move(stack.top());
            context->fromntWithArgs = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromntWithArgs_value.get());
            stack.pop();
        }
    }
    void PostExpressionList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
    void Postnt(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromnt_value = std::move(stack.top());
            context->fromnt = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromnt_value.get());
            stack.pop();
        }
    }
    void PostAlias(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAlias_value = std::move(stack.top());
            context->fromAlias = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromAlias_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromntWithArgs(), fromExpressionList(), fromnt(), fromAlias() {}
        cmajor::parsing::Parser* value;
        std::u32string fromntWithArgs;
        std::vector<cmajor::codedom::CppObject*> fromExpressionList;
        std::u32string fromnt;
        std::u32string fromAlias;
    };
};

class Primary::NonterminalRule : public cmajor::parsing::Rule
{
public:
    NonterminalRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NonterminalRule>(this, &NonterminalRule::A0Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NonterminalRule>(this, &NonterminalRule::PostIdentifier));
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

class Primary::AliasRule : public cmajor::parsing::Rule
{
public:
    AliasRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AliasRule>(this, &AliasRule::A0Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AliasRule>(this, &AliasRule::PostIdentifier));
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

class Primary::GroupingRule : public cmajor::parsing::Rule
{
public:
    GroupingRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<GroupingRule>(this, &GroupingRule::A0Action));
        cmajor::parsing::NonterminalParser* alternativeNonterminalParser = GetNonterminal(ToUtf32("Alternative"));
        alternativeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GroupingRule>(this, &GroupingRule::PreAlternative));
        alternativeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GroupingRule>(this, &GroupingRule::PostAlternative));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new GroupingParser(context->fromAlternative);
    }
    void PreAlternative(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void PostAlternative(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAlternative_value = std::move(stack.top());
            context->fromAlternative = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromAlternative_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromAlternative() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromAlternative;
    };
};

class Primary::TokenRule : public cmajor::parsing::Rule
{
public:
    TokenRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> enclosingScope_value = std::move(stack.top());
        context->enclosingScope = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Scope*>*>(enclosingScope_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TokenRule>(this, &TokenRule::A0Action));
        cmajor::parsing::NonterminalParser* alternativeNonterminalParser = GetNonterminal(ToUtf32("Alternative"));
        alternativeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TokenRule>(this, &TokenRule::PreAlternative));
        alternativeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TokenRule>(this, &TokenRule::PostAlternative));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TokenParser(context->fromAlternative);
    }
    void PreAlternative(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void PostAlternative(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAlternative_value = std::move(stack.top());
            context->fromAlternative = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromAlternative_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromAlternative() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromAlternative;
    };
};

class Primary::ExpectationRule : public cmajor::parsing::Rule
{
public:
    ExpectationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Parser*"), ToUtf32("child")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> child_value = std::move(stack.top());
        context->child = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(child_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpectationRule>(this, &ExpectationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExpectationParser(context->child);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): child(), value() {}
        cmajor::parsing::Parser* child;
        cmajor::parsing::Parser* value;
    };
};

class Primary::ActionRule : public cmajor::parsing::Rule
{
public:
    ActionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Parser*"), ToUtf32("child")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> child_value = std::move(stack.top());
        context->child = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(child_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ActionRule>(this, &ActionRule::A0Action));
        cmajor::parsing::NonterminalParser* actionNonterminalParser = GetNonterminal(ToUtf32("action"));
        actionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ActionRule>(this, &ActionRule::Postaction));
        cmajor::parsing::NonterminalParser* failureActionNonterminalParser = GetNonterminal(ToUtf32("failureAction"));
        failureActionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ActionRule>(this, &ActionRule::PostfailureAction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ActionParser(U"", context->fromaction, context->fromfailureAction, context->child);
    }
    void Postaction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromaction_value = std::move(stack.top());
            context->fromaction = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CompoundStatement*>*>(fromaction_value.get());
            stack.pop();
        }
    }
    void PostfailureAction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromfailureAction_value = std::move(stack.top());
            context->fromfailureAction = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CompoundStatement*>*>(fromfailureAction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): child(), value(), fromaction(), fromfailureAction() {}
        cmajor::parsing::Parser* child;
        cmajor::parsing::Parser* value;
        cmajor::codedom::CompoundStatement* fromaction;
        cmajor::codedom::CompoundStatement* fromfailureAction;
    };
};

void Primary::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.code.Expression"));
    if (!grammar0)
    {
        grammar0 = cmajor::code::Expression::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.syntax.Composite"));
    if (!grammar1)
    {
        grammar1 = cmajor::syntax::Composite::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.syntax.Element"));
    if (!grammar2)
    {
        grammar2 = cmajor::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.code.Statement"));
    if (!grammar3)
    {
        grammar3 = cmajor::code::Statement::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.syntax.Primitive"));
    if (!grammar4)
    {
        grammar4 = cmajor::syntax::Primitive::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void Primary::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ExpressionList"), this, ToUtf32("cmajor.code.Expression.ExpressionList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Alternative"), this, ToUtf32("Composite.Alternative")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Primitive"), this, ToUtf32("Primitive.Primitive")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("CompoundStatement"), this, ToUtf32("cmajor.code.Statement.CompoundStatement")));
    AddRule(new PrimaryRule(ToUtf32("Primary"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("RuleCall"), ToUtf32("RuleCall"), 0)),
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Primitive"), ToUtf32("Primitive"), 0))),
                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Grouping"), ToUtf32("Grouping"), 1))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Token"), ToUtf32("Token"), 1)))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Expectation"), ToUtf32("Expectation"), 1))))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Action"), ToUtf32("Action"), 1)))))));
    AddRule(new RuleCallRule(ToUtf32("RuleCall"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::TokenParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("ntWithArgs"), ToUtf32("Nonterminal"), 0),
                                            new cmajor::parsing::CharParser('('))),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0)),
                                new cmajor::parsing::CharParser(')')),
                            new cmajor::parsing::NonterminalParser(ToUtf32("nt"), ToUtf32("Nonterminal"), 0))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Alias"), ToUtf32("Alias"), 0)))))));
    AddRule(new NonterminalRule(ToUtf32("Nonterminal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
    AddRule(new AliasRule(ToUtf32("Alias"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::CharParser(':'),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))))));
    AddRule(new GroupingRule(ToUtf32("Grouping"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('('),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Alternative"), ToUtf32("Alternative"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new TokenRule(ToUtf32("Token"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("token")),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('('))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Alternative"), ToUtf32("Alternative"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new ExpectationRule(ToUtf32("Expectation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::CharParser('!'))));
    AddRule(new ActionRule(ToUtf32("Action"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("action"), ToUtf32("CompoundStatement"), 0),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('/'),
                                new cmajor::parsing::NonterminalParser(ToUtf32("failureAction"), ToUtf32("CompoundStatement"), 0)))))))));
}

} } // namespace cmajor.syntax
