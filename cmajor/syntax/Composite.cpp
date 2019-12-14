#include "Composite.hpp"
#include <cmajor/parsing/Action.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/parsing/Primitive.hpp>
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/Nonterminal.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/parsing/StdLib.hpp>
#include <cmajor/parsing/XmlLog.hpp>
#include <soulng/util/Unicode.hpp>
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/syntax/Primary.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace soulng::util;
using namespace cmajor::unicode;

Composite* Composite::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Composite* Composite::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Composite* grammar(new Composite(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Composite::Composite(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Composite"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::parsing::Parser* Composite::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Scope* enclosingScope)
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

class Composite::AlternativeRule : public cmajor::parsing::Rule
{
public:
    AlternativeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AlternativeRule>(this, &AlternativeRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AlternativeRule>(this, &AlternativeRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AlternativeRule>(this, &AlternativeRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AlternativeParser(context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromleft;
        cmajor::parsing::Parser* fromright;
    };
};

class Composite::SequenceRule : public cmajor::parsing::Rule
{
public:
    SequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SequenceRule>(this, &SequenceRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SequenceRule>(this, &SequenceRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SequenceRule>(this, &SequenceRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SequenceParser(context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromleft;
        cmajor::parsing::Parser* fromright;
    };
};

class Composite::DifferenceRule : public cmajor::parsing::Rule
{
public:
    DifferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DifferenceRule>(this, &DifferenceRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DifferenceRule>(this, &DifferenceRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DifferenceRule>(this, &DifferenceRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DifferenceParser(context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromleft;
        cmajor::parsing::Parser* fromright;
    };
};

class Composite::ExclusiveOrRule : public cmajor::parsing::Rule
{
public:
    ExclusiveOrRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExclusiveOrRule>(this, &ExclusiveOrRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExclusiveOrRule>(this, &ExclusiveOrRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ExclusiveOrParser(context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromleft;
        cmajor::parsing::Parser* fromright;
    };
};

class Composite::IntersectionRule : public cmajor::parsing::Rule
{
public:
    IntersectionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IntersectionRule>(this, &IntersectionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IntersectionRule>(this, &IntersectionRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IntersectionRule>(this, &IntersectionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IntersectionParser(context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromleft;
        cmajor::parsing::Parser* fromright;
    };
};

class Composite::ListRule : public cmajor::parsing::Rule
{
public:
    ListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ListRule>(this, &ListRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ListRule>(this, &ListRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ListRule>(this, &ListRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ListRule>(this, &ListRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ListRule>(this, &ListRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ListRule>(this, &ListRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ListParser(context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromleft(), fromright() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromleft;
        cmajor::parsing::Parser* fromright;
    };
};

class Composite::PostfixRule : public cmajor::parsing::Rule
{
public:
    PostfixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixRule>(this, &PostfixRule::A3Action));
        cmajor::parsing::NonterminalParser* primaryNonterminalParser = GetNonterminal(ToUtf32("Primary"));
        primaryNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PostfixRule>(this, &PostfixRule::PrePrimary));
        primaryNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixRule>(this, &PostfixRule::PostPrimary));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrimary;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KleeneStarParser(context->value);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PositiveParser(context->value);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OptionalParser(context->value);
    }
    void PrePrimary(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void PostPrimary(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrimary_value = std::move(stack.top());
            context->fromPrimary = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromPrimary_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromPrimary() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromPrimary;
    };
};

void Composite::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.syntax.Primary"));
    if (!grammar0)
    {
        grammar0 = cmajor::syntax::Primary::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Composite::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Primary"), this, ToUtf32("Primary.Primary")));
    AddRule(new AlternativeRule(ToUtf32("Alternative"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Sequence"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('|'),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Sequence"), 1)))))))));
    AddRule(new SequenceRule(ToUtf32("Sequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Difference"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Difference"), 1)))))));
    AddRule(new DifferenceRule(ToUtf32("Difference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ExclusiveOr"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('-'),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ExclusiveOr"), 1)))))))));
    AddRule(new ExclusiveOrRule(ToUtf32("ExclusiveOr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Intersection"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('^'),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Intersection"), 1)))))))));
    AddRule(new IntersectionRule(ToUtf32("Intersection"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("List"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('&'),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("List"), 1))))))));
    AddRule(new ListRule(ToUtf32("List"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Postfix"), 1)),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('%'),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Postfix"), 1))))))));
    AddRule(new PostfixRule(ToUtf32("Postfix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Primary"), ToUtf32("Primary"), 1)),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::CharParser('*')),
                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                new cmajor::parsing::CharParser('+'))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::CharParser('?'))))))));
}

} } // namespace cmajor.syntax
