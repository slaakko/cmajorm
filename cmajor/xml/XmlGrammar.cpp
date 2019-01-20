#include "XmlGrammar.hpp"
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

namespace cmajor { namespace xml {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Xml* Xml::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Xml* Xml::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Xml* grammar(new Xml(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Xml::Xml(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Xml"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.xml")), parsingDomain_)
{
    SetOwner(0);
}

void Xml::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, XmlProcessor* processor)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<XmlProcessor*>(processor)));
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
}

class Xml::DocumentRule : public cmajor::parsing::Rule
{
public:
    DocumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DocumentRule>(this, &DocumentRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DocumentRule>(this, &DocumentRule::A1Action));
        cmajor::parsing::NonterminalParser* prologNonterminalParser = GetNonterminal(ToUtf32("Prolog"));
        prologNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreProlog));
        cmajor::parsing::NonterminalParser* elementNonterminalParser = GetNonterminal(ToUtf32("Element"));
        elementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreElement));
        cmajor::parsing::NonterminalParser* miscNonterminalParser = GetNonterminal(ToUtf32("Misc"));
        miscNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DocumentRule>(this, &DocumentRule::PreMisc));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->StartDocument();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndDocument();
    }
    void PreProlog(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreElement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreMisc(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::NameRule : public cmajor::parsing::Rule
{
public:
    NameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NameRule>(this, &NameRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Xml::EntityValueRule : public cmajor::parsing::Rule
{
public:
    EntityValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EntityValueRule>(this, &EntityValueRule::A5Action));
        cmajor::parsing::NonterminalParser* pr1NonterminalParser = GetNonterminal(ToUtf32("pr1"));
        pr1NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Prepr1));
        cmajor::parsing::NonterminalParser* ref1NonterminalParser = GetNonterminal(ToUtf32("ref1"));
        ref1NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Preref1));
        cmajor::parsing::NonterminalParser* pr2NonterminalParser = GetNonterminal(ToUtf32("pr2"));
        pr2NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Prepr2));
        cmajor::parsing::NonterminalParser* ref2NonterminalParser = GetNonterminal(ToUtf32("ref2"));
        ref2NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EntityValueRule>(this, &EntityValueRule::Preref2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void Prepr1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prepr2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), value() {}
        XmlProcessor* processor;
        std::u32string value;
    };
};

class Xml::AttValueRule : public cmajor::parsing::Rule
{
public:
    AttValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttValueRule>(this, &AttValueRule::A5Action));
        cmajor::parsing::NonterminalParser* ref1NonterminalParser = GetNonterminal(ToUtf32("ref1"));
        ref1NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AttValueRule>(this, &AttValueRule::Preref1));
        cmajor::parsing::NonterminalParser* ref2NonterminalParser = GetNonterminal(ToUtf32("ref2"));
        ref2NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AttValueRule>(this, &AttValueRule::Preref2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->BeginAttributeValue(&context->value);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndAttributeValue();
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->BeginAttributeValue(&context->value);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, *matchBegin);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndAttributeValue();
    }
    void Preref1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Preref2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), value() {}
        XmlProcessor* processor;
        std::u32string value;
    };
};

class Xml::SystemLiteralRule : public cmajor::parsing::Rule
{
public:
    SystemLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SystemLiteralRule>(this, &SystemLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SystemLiteralRule>(this, &SystemLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Xml::PubidLiteralRule : public cmajor::parsing::Rule
{
public:
    PubidLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PubidLiteralRule>(this, &PubidLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PubidLiteralRule>(this, &PubidLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Xml::CharDataRule : public cmajor::parsing::Rule
{
public:
    CharDataRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharDataRule>(this, &CharDataRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Text(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::CommentRule : public cmajor::parsing::Rule
{
public:
    CommentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CommentRule>(this, &CommentRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Comment(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::PIRule : public cmajor::parsing::Rule
{
public:
    PIRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("data")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PIRule>(this, &PIRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PIRule>(this, &PIRule::A1Action));
        cmajor::parsing::NonterminalParser* targetNonterminalParser = GetNonterminal(ToUtf32("target"));
        targetNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PIRule>(this, &PIRule::Posttarget));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->PI(context->fromtarget, context->data);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->data = std::u32string(matchBegin, matchEnd);
    }
    void Posttarget(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromtarget_value = std::move(stack.top());
            context->fromtarget = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromtarget_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), data(), fromtarget() {}
        XmlProcessor* processor;
        std::u32string data;
        std::u32string fromtarget;
    };
};

class Xml::PITargetRule : public cmajor::parsing::Rule
{
public:
    PITargetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PITargetRule>(this, &PITargetRule::A0Action));
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PITargetRule>(this, &PITargetRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromName;
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromName() {}
        std::u32string value;
        std::u32string fromName;
    };
};

class Xml::CDSectRule : public cmajor::parsing::Rule
{
public:
    CDSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CDSectRule>(this, &CDSectRule::A0Action));
        cmajor::parsing::NonterminalParser* cDataNonterminalParser = GetNonterminal(ToUtf32("CData"));
        cDataNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CDSectRule>(this, &CDSectRule::PostCData));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->CDataSection(context->fromCData);
    }
    void PostCData(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCData_value = std::move(stack.top());
            context->fromCData = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromCData_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromCData() {}
        XmlProcessor* processor;
        std::u32string fromCData;
    };
};

class Xml::CDataRule : public cmajor::parsing::Rule
{
public:
    CDataRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CDataRule>(this, &CDataRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Xml::PrologRule : public cmajor::parsing::Rule
{
public:
    PrologRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* xMLDeclNonterminalParser = GetNonterminal(ToUtf32("XMLDecl"));
        xMLDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrologRule>(this, &PrologRule::PreXMLDecl));
        cmajor::parsing::NonterminalParser* m1NonterminalParser = GetNonterminal(ToUtf32("m1"));
        m1NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrologRule>(this, &PrologRule::Prem1));
        cmajor::parsing::NonterminalParser* docTypeDeclNonterminalParser = GetNonterminal(ToUtf32("DocTypeDecl"));
        docTypeDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrologRule>(this, &PrologRule::PreDocTypeDecl));
        cmajor::parsing::NonterminalParser* m2NonterminalParser = GetNonterminal(ToUtf32("m2"));
        m2NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrologRule>(this, &PrologRule::Prem2));
    }
    void PreXMLDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prem1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDocTypeDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Prem2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::XMLDeclRule : public cmajor::parsing::Rule
{
public:
    XMLDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* versionInfoNonterminalParser = GetNonterminal(ToUtf32("VersionInfo"));
        versionInfoNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreVersionInfo));
        cmajor::parsing::NonterminalParser* encodingDeclNonterminalParser = GetNonterminal(ToUtf32("EncodingDecl"));
        encodingDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreEncodingDecl));
        cmajor::parsing::NonterminalParser* sDDeclNonterminalParser = GetNonterminal(ToUtf32("SDDecl"));
        sDDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<XMLDeclRule>(this, &XMLDeclRule::PreSDDecl));
    }
    void PreVersionInfo(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEncodingDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreSDDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::VersionInfoRule : public cmajor::parsing::Rule
{
public:
    VersionInfoRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<VersionInfoRule>(this, &VersionInfoRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<VersionInfoRule>(this, &VersionInfoRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Version(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Version(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::MiscRule : public cmajor::parsing::Rule
{
public:
    MiscRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MiscRule>(this, &MiscRule::PreComment));
        cmajor::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MiscRule>(this, &MiscRule::PrePI));
    }
    void PreComment(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::DocTypeDeclRule : public cmajor::parsing::Rule
{
public:
    DocTypeDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* rootElementNameNonterminalParser = GetNonterminal(ToUtf32("rootElementName"));
        rootElementNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DocTypeDeclRule>(this, &DocTypeDeclRule::PostrootElementName));
        cmajor::parsing::NonterminalParser* intSubsetNonterminalParser = GetNonterminal(ToUtf32("IntSubset"));
        intSubsetNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DocTypeDeclRule>(this, &DocTypeDeclRule::PreIntSubset));
    }
    void PostrootElementName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromrootElementName_value = std::move(stack.top());
            context->fromrootElementName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromrootElementName_value.get());
            stack.pop();
        }
    }
    void PreIntSubset(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromrootElementName() {}
        XmlProcessor* processor;
        std::u32string fromrootElementName;
    };
};

class Xml::DeclSepRule : public cmajor::parsing::Rule
{
public:
    DeclSepRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* pEReferenceNonterminalParser = GetNonterminal(ToUtf32("PEReference"));
        pEReferenceNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DeclSepRule>(this, &DeclSepRule::PrePEReference));
    }
    void PrePEReference(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::IntSubsetRule : public cmajor::parsing::Rule
{
public:
    IntSubsetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* markupDeclNonterminalParser = GetNonterminal(ToUtf32("MarkupDecl"));
        markupDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IntSubsetRule>(this, &IntSubsetRule::PreMarkupDecl));
        cmajor::parsing::NonterminalParser* declSepNonterminalParser = GetNonterminal(ToUtf32("DeclSep"));
        declSepNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IntSubsetRule>(this, &IntSubsetRule::PreDeclSep));
    }
    void PreMarkupDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDeclSep(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::MarkupDeclRule : public cmajor::parsing::Rule
{
public:
    MarkupDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* elementDeclNonterminalParser = GetNonterminal(ToUtf32("ElementDecl"));
        elementDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreElementDecl));
        cmajor::parsing::NonterminalParser* attlistDeclNonterminalParser = GetNonterminal(ToUtf32("AttlistDecl"));
        attlistDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreAttlistDecl));
        cmajor::parsing::NonterminalParser* entityDeclNonterminalParser = GetNonterminal(ToUtf32("EntityDecl"));
        entityDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreEntityDecl));
        cmajor::parsing::NonterminalParser* notationDeclNonterminalParser = GetNonterminal(ToUtf32("NotationDecl"));
        notationDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreNotationDecl));
        cmajor::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PrePI));
        cmajor::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MarkupDeclRule>(this, &MarkupDeclRule::PreComment));
    }
    void PreElementDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreAttlistDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEntityDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreNotationDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreComment(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ExtSubsetRule : public cmajor::parsing::Rule
{
public:
    ExtSubsetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* textDeclNonterminalParser = GetNonterminal(ToUtf32("TextDecl"));
        textDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExtSubsetRule>(this, &ExtSubsetRule::PreTextDecl));
        cmajor::parsing::NonterminalParser* extSubsetDeclNonterminalParser = GetNonterminal(ToUtf32("ExtSubsetDecl"));
        extSubsetDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExtSubsetRule>(this, &ExtSubsetRule::PreExtSubsetDecl));
    }
    void PreTextDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreExtSubsetDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ExtSubsetDeclRule : public cmajor::parsing::Rule
{
public:
    ExtSubsetDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* markupDeclNonterminalParser = GetNonterminal(ToUtf32("MarkupDecl"));
        markupDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreMarkupDecl));
        cmajor::parsing::NonterminalParser* conditionalSectNonterminalParser = GetNonterminal(ToUtf32("ConditionalSect"));
        conditionalSectNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreConditionalSect));
        cmajor::parsing::NonterminalParser* declSepNonterminalParser = GetNonterminal(ToUtf32("DeclSep"));
        declSepNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExtSubsetDeclRule>(this, &ExtSubsetDeclRule::PreDeclSep));
    }
    void PreMarkupDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreConditionalSect(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreDeclSep(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::SDDeclRule : public cmajor::parsing::Rule
{
public:
    SDDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("standalone")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SDDeclRule>(this, &SDDeclRule::A4Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Standalone(context->standalone);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = true;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->standalone = false;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), standalone() {}
        XmlProcessor* processor;
        bool standalone;
    };
};

class Xml::ElementRule : public cmajor::parsing::Rule
{
public:
    ElementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("tagName")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ElementRule>(this, &ElementRule::A2Action));
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ElementRule>(this, &ElementRule::PostName));
        cmajor::parsing::NonterminalParser* attributeNonterminalParser = GetNonterminal(ToUtf32("Attribute"));
        attributeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreAttribute));
        cmajor::parsing::NonterminalParser* contentNonterminalParser = GetNonterminal(ToUtf32("Content"));
        contentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreContent));
        cmajor::parsing::NonterminalParser* eTagNonterminalParser = GetNonterminal(ToUtf32("ETag"));
        eTagNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ElementRule>(this, &ElementRule::PreETag));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->tagName = context->fromName;
        context->processor->BeginStartTag(context->tagName);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndStartTag(span, fileName);
        context->processor->EndTag(context->tagName, span, fileName);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndStartTag(span, fileName);
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreAttribute(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreETag(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), tagName(), fromName() {}
        XmlProcessor* processor;
        std::u32string tagName;
        std::u32string fromName;
    };
};

class Xml::AttributeRule : public cmajor::parsing::Rule
{
public:
    AttributeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttributeRule>(this, &AttributeRule::A0Action));
        cmajor::parsing::NonterminalParser* attNameNonterminalParser = GetNonterminal(ToUtf32("attName"));
        attNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::PostattName));
        cmajor::parsing::NonterminalParser* attValueNonterminalParser = GetNonterminal(ToUtf32("attValue"));
        attValueNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AttributeRule>(this, &AttributeRule::PreattValue));
        attValueNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::PostattValue));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->AddAttribute(context->fromattName, context->fromattValue, span, fileName);
    }
    void PostattName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromattName_value = std::move(stack.top());
            context->fromattName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromattName_value.get());
            stack.pop();
        }
    }
    void PreattValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostattValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromattValue_value = std::move(stack.top());
            context->fromattValue = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromattValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromattName(), fromattValue() {}
        XmlProcessor* processor;
        std::u32string fromattName;
        std::u32string fromattValue;
    };
};

class Xml::ETagRule : public cmajor::parsing::Rule
{
public:
    ETagRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ETagRule>(this, &ETagRule::A0Action));
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ETagRule>(this, &ETagRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EndTag(context->fromName, span, fileName);
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::ContentRule : public cmajor::parsing::Rule
{
public:
    ContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* cd1NonterminalParser = GetNonterminal(ToUtf32("cd1"));
        cd1NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ContentRule>(this, &ContentRule::Precd1));
        cmajor::parsing::NonterminalParser* elementNonterminalParser = GetNonterminal(ToUtf32("Element"));
        elementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreElement));
        cmajor::parsing::NonterminalParser* referenceNonterminalParser = GetNonterminal(ToUtf32("Reference"));
        referenceNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreReference));
        cmajor::parsing::NonterminalParser* cDSectNonterminalParser = GetNonterminal(ToUtf32("CDSect"));
        cDSectNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreCDSect));
        cmajor::parsing::NonterminalParser* pINonterminalParser = GetNonterminal(ToUtf32("PI"));
        pINonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PrePI));
        cmajor::parsing::NonterminalParser* commentNonterminalParser = GetNonterminal(ToUtf32("Comment"));
        commentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ContentRule>(this, &ContentRule::PreComment));
        cmajor::parsing::NonterminalParser* cd2NonterminalParser = GetNonterminal(ToUtf32("cd2"));
        cd2NonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ContentRule>(this, &ContentRule::Precd2));
    }
    void Precd1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreElement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreReference(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreCDSect(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePI(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreComment(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void Precd2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ElementDeclRule : public cmajor::parsing::Rule
{
public:
    ElementDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* elementNameNonterminalParser = GetNonterminal(ToUtf32("elementName"));
        elementNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ElementDeclRule>(this, &ElementDeclRule::PostelementName));
    }
    void PostelementName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromelementName_value = std::move(stack.top());
            context->fromelementName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromelementName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromelementName() {}
        XmlProcessor* processor;
        std::u32string fromelementName;
    };
};

class Xml::AttlistDeclRule : public cmajor::parsing::Rule
{
public:
    AttlistDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AttlistDeclRule>(this, &AttlistDeclRule::PostName));
        cmajor::parsing::NonterminalParser* attDefNonterminalParser = GetNonterminal(ToUtf32("AttDef"));
        attDefNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AttlistDeclRule>(this, &AttlistDeclRule::PreAttDef));
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreAttDef(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::AttDefRule : public cmajor::parsing::Rule
{
public:
    AttDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AttDefRule>(this, &AttDefRule::PostName));
        cmajor::parsing::NonterminalParser* defaultDeclNonterminalParser = GetNonterminal(ToUtf32("DefaultDecl"));
        defaultDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AttDefRule>(this, &AttDefRule::PreDefaultDecl));
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
    void PreDefaultDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::DefaultDeclRule : public cmajor::parsing::Rule
{
public:
    DefaultDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* attValueNonterminalParser = GetNonterminal(ToUtf32("AttValue"));
        attValueNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefaultDeclRule>(this, &DefaultDeclRule::PreAttValue));
        attValueNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefaultDeclRule>(this, &DefaultDeclRule::PostAttValue));
    }
    void PreAttValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostAttValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAttValue_value = std::move(stack.top());
            context->fromAttValue = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromAttValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromAttValue() {}
        XmlProcessor* processor;
        std::u32string fromAttValue;
    };
};

class Xml::ConditionalSectRule : public cmajor::parsing::Rule
{
public:
    ConditionalSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* includeSectNonterminalParser = GetNonterminal(ToUtf32("IncludeSect"));
        includeSectNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConditionalSectRule>(this, &ConditionalSectRule::PreIncludeSect));
    }
    void PreIncludeSect(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::IncludeSectRule : public cmajor::parsing::Rule
{
public:
    IncludeSectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* extSubsetDeclNonterminalParser = GetNonterminal(ToUtf32("ExtSubsetDecl"));
        extSubsetDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IncludeSectRule>(this, &IncludeSectRule::PreExtSubsetDecl));
    }
    void PreExtSubsetDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::CharRefRule : public cmajor::parsing::Rule
{
public:
    CharRefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("char32_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<char32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharRefRule>(this, &CharRefRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharRefRule>(this, &CharRefRule::A1Action));
        cmajor::parsing::NonterminalParser* dcrNonterminalParser = GetNonterminal(ToUtf32("dcr"));
        dcrNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharRefRule>(this, &CharRefRule::Postdcr));
        cmajor::parsing::NonterminalParser* hcrNonterminalParser = GetNonterminal(ToUtf32("hcr"));
        hcrNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharRefRule>(this, &CharRefRule::Posthcr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = static_cast<char32_t>(context->fromdcr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = static_cast<char32_t>(context->fromhcr);
    }
    void Postdcr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromdcr_value = std::move(stack.top());
            context->fromdcr = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(fromdcr_value.get());
            stack.pop();
        }
    }
    void Posthcr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromhcr_value = std::move(stack.top());
            context->fromhcr = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(fromhcr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromdcr(), fromhcr() {}
        char32_t value;
        uint32_t fromdcr;
        uint32_t fromhcr;
    };
};

class Xml::ReferenceRule : public cmajor::parsing::Rule
{
public:
    ReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ReferenceRule>(this, &ReferenceRule::A0Action));
        cmajor::parsing::NonterminalParser* entityRefNonterminalParser = GetNonterminal(ToUtf32("EntityRef"));
        entityRefNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ReferenceRule>(this, &ReferenceRule::PreEntityRef));
        cmajor::parsing::NonterminalParser* charRefNonterminalParser = GetNonterminal(ToUtf32("CharRef"));
        charRefNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ReferenceRule>(this, &ReferenceRule::PostCharRef));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Text(std::u32string(1, context->fromCharRef));
    }
    void PreEntityRef(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostCharRef(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCharRef_value = std::move(stack.top());
            context->fromCharRef = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromCharRef_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromCharRef() {}
        XmlProcessor* processor;
        char32_t fromCharRef;
    };
};

class Xml::EntityRefRule : public cmajor::parsing::Rule
{
public:
    EntityRefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EntityRefRule>(this, &EntityRefRule::A0Action));
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EntityRefRule>(this, &EntityRefRule::PostName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->EntityRef(context->fromName, span, fileName);
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::PEReferenceRule : public cmajor::parsing::Rule
{
public:
    PEReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PEReferenceRule>(this, &PEReferenceRule::PostName));
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

class Xml::EntityDeclRule : public cmajor::parsing::Rule
{
public:
    EntityDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* gEDeclNonterminalParser = GetNonterminal(ToUtf32("GEDecl"));
        gEDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EntityDeclRule>(this, &EntityDeclRule::PreGEDecl));
        cmajor::parsing::NonterminalParser* pEDeclNonterminalParser = GetNonterminal(ToUtf32("PEDecl"));
        pEDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EntityDeclRule>(this, &EntityDeclRule::PrePEDecl));
    }
    void PreGEDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PrePEDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::GEDeclRule : public cmajor::parsing::Rule
{
public:
    GEDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* entityNameNonterminalParser = GetNonterminal(ToUtf32("entityName"));
        entityNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GEDeclRule>(this, &GEDeclRule::PostentityName));
        cmajor::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("entityValue"));
        entityValueNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GEDeclRule>(this, &GEDeclRule::PreentityValue));
    }
    void PostentityName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromentityName_value = std::move(stack.top());
            context->fromentityName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromentityName_value.get());
            stack.pop();
        }
    }
    void PreentityValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromentityName() {}
        XmlProcessor* processor;
        std::u32string fromentityName;
    };
};

class Xml::PEDeclRule : public cmajor::parsing::Rule
{
public:
    PEDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* peNameNonterminalParser = GetNonterminal(ToUtf32("peName"));
        peNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PEDeclRule>(this, &PEDeclRule::PostpeName));
        cmajor::parsing::NonterminalParser* peValueNonterminalParser = GetNonterminal(ToUtf32("peValue"));
        peValueNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PEDeclRule>(this, &PEDeclRule::PrepeValue));
    }
    void PostpeName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frompeName_value = std::move(stack.top());
            context->frompeName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(frompeName_value.get());
            stack.pop();
        }
    }
    void PrepeValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), frompeName() {}
        XmlProcessor* processor;
        std::u32string frompeName;
    };
};

class Xml::EntityDefRule : public cmajor::parsing::Rule
{
public:
    EntityDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("EntityValue"));
        entityValueNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EntityDefRule>(this, &EntityDefRule::PreEntityValue));
        entityValueNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EntityDefRule>(this, &EntityDefRule::PostEntityValue));
    }
    void PreEntityValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostEntityValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEntityValue_value = std::move(stack.top());
            context->fromEntityValue = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromEntityValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromEntityValue() {}
        XmlProcessor* processor;
        std::u32string fromEntityValue;
    };
};

class Xml::PEDefRule : public cmajor::parsing::Rule
{
public:
    PEDefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* entityValueNonterminalParser = GetNonterminal(ToUtf32("EntityValue"));
        entityValueNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PEDefRule>(this, &PEDefRule::PreEntityValue));
        entityValueNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PEDefRule>(this, &PEDefRule::PostEntityValue));
    }
    void PreEntityValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PostEntityValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEntityValue_value = std::move(stack.top());
            context->fromEntityValue = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromEntityValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromEntityValue() {}
        XmlProcessor* processor;
        std::u32string fromEntityValue;
    };
};

class Xml::TextDeclRule : public cmajor::parsing::Rule
{
public:
    TextDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* versionInfoNonterminalParser = GetNonterminal(ToUtf32("VersionInfo"));
        versionInfoNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TextDeclRule>(this, &TextDeclRule::PreVersionInfo));
        cmajor::parsing::NonterminalParser* encodingDeclNonterminalParser = GetNonterminal(ToUtf32("EncodingDecl"));
        encodingDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TextDeclRule>(this, &TextDeclRule::PreEncodingDecl));
    }
    void PreVersionInfo(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreEncodingDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::ExtParsedEntRule : public cmajor::parsing::Rule
{
public:
    ExtParsedEntRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* textDeclNonterminalParser = GetNonterminal(ToUtf32("TextDecl"));
        textDeclNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExtParsedEntRule>(this, &ExtParsedEntRule::PreTextDecl));
        cmajor::parsing::NonterminalParser* contentNonterminalParser = GetNonterminal(ToUtf32("Content"));
        contentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ExtParsedEntRule>(this, &ExtParsedEntRule::PreContent));
    }
    void PreTextDecl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
    void PreContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XmlProcessor*>(context->processor)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor() {}
        XmlProcessor* processor;
    };
};

class Xml::EncodingDeclRule : public cmajor::parsing::Rule
{
public:
    EncodingDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EncodingDeclRule>(this, &EncodingDeclRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EncodingDeclRule>(this, &EncodingDeclRule::A1Action));
        cmajor::parsing::NonterminalParser* en1NonterminalParser = GetNonterminal(ToUtf32("en1"));
        en1NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EncodingDeclRule>(this, &EncodingDeclRule::Posten1));
        cmajor::parsing::NonterminalParser* en2NonterminalParser = GetNonterminal(ToUtf32("en2"));
        en2NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EncodingDeclRule>(this, &EncodingDeclRule::Posten2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Encoding(context->fromen1);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->processor->Encoding(context->fromen2);
    }
    void Posten1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromen1_value = std::move(stack.top());
            context->fromen1 = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromen1_value.get());
            stack.pop();
        }
    }
    void Posten2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromen2_value = std::move(stack.top());
            context->fromen2 = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromen2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromen1(), fromen2() {}
        XmlProcessor* processor;
        std::u32string fromen1;
        std::u32string fromen2;
    };
};

class Xml::EncNameRule : public cmajor::parsing::Rule
{
public:
    EncNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EncNameRule>(this, &EncNameRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::u32string value;
    };
};

class Xml::NotationDeclRule : public cmajor::parsing::Rule
{
public:
    NotationDeclRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("XmlProcessor*"), ToUtf32("processor")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> processor_value = std::move(stack.top());
        context->processor = *static_cast<cmajor::parsing::ValueObject<XmlProcessor*>*>(processor_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("Name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NotationDeclRule>(this, &NotationDeclRule::PostName));
    }
    void PostName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromName_value = std::move(stack.top());
            context->fromName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): processor(), fromName() {}
        XmlProcessor* processor;
        std::u32string fromName;
    };
};

void Xml::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Xml::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("uint"), this, ToUtf32("cmajor.parsing.stdlib.uint")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("hexuint"), this, ToUtf32("cmajor.parsing.stdlib.hexuint")));
    AddRule(new DocumentRule(ToUtf32("Document"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Prolog"), ToUtf32("Prolog"), 1),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Element"), ToUtf32("Element"), 1)),
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Misc"), ToUtf32("Misc"), 1))))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::CharSetParser(ToUtf32("\t\n\r")),
                    new cmajor::parsing::RangeParser(32, 55295)),
                new cmajor::parsing::RangeParser(57344, 65533)),
            new cmajor::parsing::RangeParser(65536, 1114111))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("S"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::CharSetParser(ToUtf32(" \t\r\n")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("NameStartChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                    new cmajor::parsing::AlternativeParser(
                                                        new cmajor::parsing::AlternativeParser(
                                                            new cmajor::parsing::CharParser(':'),
                                                            new cmajor::parsing::CharSetParser(ToUtf32("A-Z_a-z"))),
                                                        new cmajor::parsing::RangeParser(192, 214)),
                                                    new cmajor::parsing::RangeParser(216, 246)),
                                                new cmajor::parsing::RangeParser(248, 767)),
                                            new cmajor::parsing::RangeParser(880, 893)),
                                        new cmajor::parsing::RangeParser(895, 8191)),
                                    new cmajor::parsing::RangeParser(8204, 8205)),
                                new cmajor::parsing::RangeParser(8304, 8591)),
                            new cmajor::parsing::RangeParser(11264, 12271)),
                        new cmajor::parsing::RangeParser(12289, 55295)),
                    new cmajor::parsing::RangeParser(63744, 64975)),
                new cmajor::parsing::RangeParser(65008, 65533)),
            new cmajor::parsing::RangeParser(65536, 983039))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("NameChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                        new cmajor::parsing::CharSetParser(ToUtf32("-.0-9"))),
                    new cmajor::parsing::RangeParser(183, 183)),
                new cmajor::parsing::RangeParser(768, 879)),
            new cmajor::parsing::RangeParser(8255, 8256))));
    AddRule(new NameRule(ToUtf32("Name"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0)))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Names"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("f"), ToUtf32("Name"), 0),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser(' '),
                        new cmajor::parsing::NonterminalParser(ToUtf32("n"), ToUtf32("Name"), 0)))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Nmtoken"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Nmtokens"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser(' '),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0)))))));
    AddRule(new EntityValueRule(ToUtf32("EntityValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\"'),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                        new cmajor::parsing::CharSetParser(ToUtf32("%&\""), true)),
                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("pr1"), ToUtf32("PEReference"), 1))),
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("ref1"), ToUtf32("Reference"), 1)))))),
                new cmajor::parsing::CharParser('\"')),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\''),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                        new cmajor::parsing::CharSetParser(ToUtf32("%&\'"), true)),
                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("pr2"), ToUtf32("PEReference"), 1))),
                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("ref2"), ToUtf32("Reference"), 1)))))),
                new cmajor::parsing::CharParser('\'')))));
    AddRule(new AttValueRule(ToUtf32("AttValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::CharParser('\"')),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::CharSetParser(ToUtf32("<&\""), true)),
                                new cmajor::parsing::NonterminalParser(ToUtf32("ref1"), ToUtf32("Reference"), 1))))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::CharParser('\"'))),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                        new cmajor::parsing::CharParser('\'')),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                    new cmajor::parsing::CharSetParser(ToUtf32("<&\'"), true)),
                                new cmajor::parsing::NonterminalParser(ToUtf32("ref2"), ToUtf32("Reference"), 1))))),
                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                    new cmajor::parsing::CharParser('\''))))));
    AddRule(new SystemLiteralRule(ToUtf32("SystemLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('\"'),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::KleeneStarParser(
                                    new cmajor::parsing::CharSetParser(ToUtf32("\""), true))))),
                    new cmajor::parsing::CharParser('\"'))),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('\''),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::KleeneStarParser(
                                    new cmajor::parsing::CharSetParser(ToUtf32("\'"), true))))),
                    new cmajor::parsing::CharParser('\''))))));
    AddRule(new PubidLiteralRule(ToUtf32("PubidLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\"'),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::KleeneStarParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("PubidChar"), ToUtf32("PubidChar"), 0))))),
                new cmajor::parsing::CharParser('\"')),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\''),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::KleeneStarParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::DifferenceParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("PubidChar"), ToUtf32("PubidChar"), 0),
                                        new cmajor::parsing::CharParser('\''))))))),
                new cmajor::parsing::CharParser('\'')))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("PubidChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::CharSetParser(ToUtf32(" \r\n")),
                new cmajor::parsing::CharSetParser(ToUtf32("a-zA-Z0-9"))),
            new cmajor::parsing::CharSetParser(ToUtf32("-\'()+,./:=?;!*#@$_%")))));
    AddRule(new CharDataRule(ToUtf32("CharData"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::DifferenceParser(
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::CharSetParser(ToUtf32("<&"), true)),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::KleeneStarParser(
                                    new cmajor::parsing::CharSetParser(ToUtf32("<&"), true)),
                                new cmajor::parsing::StringParser(ToUtf32("]]>"))),
                            new cmajor::parsing::KleeneStarParser(
                                new cmajor::parsing::CharSetParser(ToUtf32("<&"), true)))))))));
    AddRule(new CommentRule(ToUtf32("Comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::StringParser(ToUtf32("<!--")),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::DifferenceParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                            new cmajor::parsing::CharParser('-'))),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::CharParser('-'),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::DifferenceParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                                    new cmajor::parsing::CharParser('-'))))))))))),
            new cmajor::parsing::StringParser(ToUtf32("-->")))));
    AddRule(new PIRule(ToUtf32("PI"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::StringParser(ToUtf32("<?")),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("target"), ToUtf32("PITarget"), 0),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::KleeneStarParser(
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::DifferenceParser(
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                                                            new cmajor::parsing::StringParser(ToUtf32("?>")))))))))))))),
            new cmajor::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new PITargetRule(ToUtf32("PITarget"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::DifferenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::CharParser('X'),
                                        new cmajor::parsing::CharParser('x'))),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::CharParser('M'),
                                        new cmajor::parsing::CharParser('m')))),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::CharParser('L'),
                                    new cmajor::parsing::CharParser('l'))))))))));
    AddRule(new CDSectRule(ToUtf32("CDSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("CDStart"), ToUtf32("CDStart"), 0),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("CData"), ToUtf32("CData"), 0))),
            new cmajor::parsing::NonterminalParser(ToUtf32("CDEnd"), ToUtf32("CDEnd"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("CDStart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::StringParser(ToUtf32("<![CDATA["))));
    AddRule(new CDataRule(ToUtf32("CData"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::DifferenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0),
                            new cmajor::parsing::StringParser(ToUtf32("]]>")))))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("CDEnd"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::StringParser(ToUtf32("]]>"))));
    AddRule(new PrologRule(ToUtf32("Prolog"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("XMLDecl"), ToUtf32("XMLDecl"), 1)),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("m1"), ToUtf32("Misc"), 1))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("DocTypeDecl"), ToUtf32("DocTypeDecl"), 1),
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("m2"), ToUtf32("Misc"), 1))))))));
    AddRule(new XMLDeclRule(ToUtf32("XMLDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::StringParser(ToUtf32("<?xml")),
                            new cmajor::parsing::NonterminalParser(ToUtf32("VersionInfo"), ToUtf32("VersionInfo"), 1)),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("EncodingDecl"), ToUtf32("EncodingDecl"), 1))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("SDDecl"), ToUtf32("SDDecl"), 1))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new VersionInfoRule(ToUtf32("VersionInfo"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new cmajor::parsing::StringParser(ToUtf32("version"))),
                new cmajor::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('\''),
                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("VersionNum"), ToUtf32("VersionNum"), 0))),
                        new cmajor::parsing::CharParser('\'')),
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('\"'),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("VersionNum"), ToUtf32("VersionNum"), 0))),
                        new cmajor::parsing::CharParser('\"')))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Eq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                new cmajor::parsing::CharParser('=')),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("VersionNum"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::StringParser(ToUtf32("1.")),
            new cmajor::parsing::PositiveParser(
                new cmajor::parsing::CharSetParser(ToUtf32("0-9"))))));
    AddRule(new MiscRule(ToUtf32("Misc"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1),
                new cmajor::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))));
    AddRule(new DocTypeDeclRule(ToUtf32("DocTypeDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("<!DOCTYPE")),
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new cmajor::parsing::NonterminalParser(ToUtf32("rootElementName"), ToUtf32("Name"), 0)),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0))))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('['),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("IntSubset"), ToUtf32("IntSubset"), 1)),
                                new cmajor::parsing::CharParser(']')),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)))))),
            new cmajor::parsing::CharParser('>'))));
    AddRule(new DeclSepRule(ToUtf32("DeclSep"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("PEReference"), ToUtf32("PEReference"), 1),
            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))));
    AddRule(new IntSubsetRule(ToUtf32("IntSubset"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("MarkupDecl"), ToUtf32("MarkupDecl"), 1),
                    new cmajor::parsing::NonterminalParser(ToUtf32("DeclSep"), ToUtf32("DeclSep"), 1))))));
    AddRule(new MarkupDeclRule(ToUtf32("MarkupDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("ElementDecl"), ToUtf32("ElementDecl"), 1),
                            new cmajor::parsing::NonterminalParser(ToUtf32("AttlistDecl"), ToUtf32("AttlistDecl"), 1)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("EntityDecl"), ToUtf32("EntityDecl"), 1)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("NotationDecl"), ToUtf32("NotationDecl"), 1)),
                new cmajor::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
            new cmajor::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1))));
    AddRule(new ExtSubsetRule(ToUtf32("ExtSubset"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("TextDecl"), ToUtf32("TextDecl"), 1)),
            new cmajor::parsing::NonterminalParser(ToUtf32("ExtSubsetDecl"), ToUtf32("ExtSubsetDecl"), 1))));
    AddRule(new ExtSubsetDeclRule(ToUtf32("ExtSubsetDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("MarkupDecl"), ToUtf32("MarkupDecl"), 1),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ConditionalSect"), ToUtf32("ConditionalSect"), 1)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("DeclSep"), ToUtf32("DeclSep"), 1))))));
    AddRule(new SDDeclRule(ToUtf32("SDDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                            new cmajor::parsing::StringParser(ToUtf32("standalone"))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::CharParser('\''),
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                    new cmajor::parsing::StringParser(ToUtf32("yes"))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::StringParser(ToUtf32("no")))))),
                                    new cmajor::parsing::CharParser('\''))),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::CharParser('\"'),
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                    new cmajor::parsing::StringParser(ToUtf32("yes"))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                    new cmajor::parsing::StringParser(ToUtf32("no")))))),
                                    new cmajor::parsing::CharParser('\"'))))))))));
    AddRule(new ElementRule(ToUtf32("Element"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('<'),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Attribute"), ToUtf32("Attribute"), 1))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::StringParser(ToUtf32("/>"))),
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                new cmajor::parsing::CharParser('>')),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Content"), ToUtf32("Content"), 1)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ETag"), ToUtf32("ETag"), 1)))))));
    AddRule(new AttributeRule(ToUtf32("Attribute"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("attName"), ToUtf32("Name"), 0),
                new cmajor::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("attValue"), ToUtf32("AttValue"), 1)))));
    AddRule(new ETagRule(ToUtf32("ETag"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::StringParser(ToUtf32("</")),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser('>'))));
    AddRule(new ContentRule(ToUtf32("Content"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("cd1"), ToUtf32("CharData"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Element"), ToUtf32("Element"), 1),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Reference"), ToUtf32("Reference"), 1)),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("CDSect"), ToUtf32("CDSect"), 1)),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("PI"), ToUtf32("PI"), 1)),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Comment"), ToUtf32("Comment"), 1))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("cd2"), ToUtf32("CharData"), 1))))))));
    AddRule(new ElementDeclRule(ToUtf32("ElementDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("<!ELEMENT")),
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new cmajor::parsing::NonterminalParser(ToUtf32("elementName"), ToUtf32("Name"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ContentSpec"), ToUtf32("ContentSpec"), 0)),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser('>'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("ContentSpec"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::StringParser(ToUtf32("EMPTY")),
                    new cmajor::parsing::StringParser(ToUtf32("ANY"))),
                new cmajor::parsing::NonterminalParser(ToUtf32("Mixed"), ToUtf32("Mixed"), 0)),
            new cmajor::parsing::NonterminalParser(ToUtf32("Children"), ToUtf32("Children"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Children"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Choice"), ToUtf32("Choice"), 0),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Seq"), ToUtf32("Seq"), 0))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::CharParser('?'),
                            new cmajor::parsing::CharParser('*')),
                        new cmajor::parsing::CharParser('+')))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("CP"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Choice"), ToUtf32("Choice"), 0)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Seq"), ToUtf32("Seq"), 0))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::CharParser('?'),
                            new cmajor::parsing::CharParser('*')),
                        new cmajor::parsing::CharParser('+')))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Choice"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)),
                    new cmajor::parsing::PositiveParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new cmajor::parsing::CharParser('|')),
                                    new cmajor::parsing::OptionalParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new cmajor::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser(')'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Seq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new cmajor::parsing::CharParser(',')),
                                    new cmajor::parsing::OptionalParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new cmajor::parsing::NonterminalParser(ToUtf32("CP"), ToUtf32("CP"), 0))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser(')'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Mixed"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('('),
                                new cmajor::parsing::OptionalParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new cmajor::parsing::StringParser(ToUtf32("#PCDATA"))),
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::OptionalParser(
                                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                            new cmajor::parsing::CharParser('|')),
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new cmajor::parsing::StringParser(ToUtf32(")*"))),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new cmajor::parsing::StringParser(ToUtf32("#PCDATA"))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new cmajor::parsing::CharParser(')')))));
    AddRule(new AttlistDeclRule(ToUtf32("AttlistDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::StringParser(ToUtf32("<!ATTLIST")),
                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("AttDef"), ToUtf32("AttDef"), 1))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser('>'))));
    AddRule(new AttDefRule(ToUtf32("AttDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("AttType"), ToUtf32("AttType"), 0)),
                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new cmajor::parsing::NonterminalParser(ToUtf32("DefaultDecl"), ToUtf32("DefaultDecl"), 1))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("AttType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("StringType"), ToUtf32("StringType"), 0),
                new cmajor::parsing::NonterminalParser(ToUtf32("TokenizedType"), ToUtf32("TokenizedType"), 0)),
            new cmajor::parsing::NonterminalParser(ToUtf32("EnumeratedType"), ToUtf32("EnumeratedType"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("StringType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::StringParser(ToUtf32("CDATA"))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("TokenizedType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::StringParser(ToUtf32("ID")),
                                new cmajor::parsing::StringParser(ToUtf32("IDREF"))),
                            new cmajor::parsing::StringParser(ToUtf32("IDREFS"))),
                        new cmajor::parsing::StringParser(ToUtf32("ENTITY"))),
                    new cmajor::parsing::StringParser(ToUtf32("ENTITIES"))),
                new cmajor::parsing::StringParser(ToUtf32("NMTOKEN"))),
            new cmajor::parsing::StringParser(ToUtf32("NMTOKENS")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("EnumeratedType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("NotationType"), ToUtf32("NotationType"), 0),
            new cmajor::parsing::NonterminalParser(ToUtf32("Enumeration"), ToUtf32("Enumeration"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("NotationType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::StringParser(ToUtf32("NOTATION")),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                new cmajor::parsing::CharParser('(')),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("f"), ToUtf32("Name"), 0)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new cmajor::parsing::CharParser('|')),
                                    new cmajor::parsing::OptionalParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new cmajor::parsing::NonterminalParser(ToUtf32("n"), ToUtf32("Name"), 0))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser(')'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Enumeration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0)),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new cmajor::parsing::CharParser('|')),
                                    new cmajor::parsing::OptionalParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Nmtoken"), ToUtf32("Nmtoken"), 0))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser(')'))));
    AddRule(new DefaultDeclRule(ToUtf32("DefaultDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::StringParser(ToUtf32("#REQUIRED")),
                new cmajor::parsing::StringParser(ToUtf32("#IMPLIED"))),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("#FIXED")),
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)))),
                    new cmajor::parsing::NonterminalParser(ToUtf32("AttValue"), ToUtf32("AttValue"), 1))))));
    AddRule(new ConditionalSectRule(ToUtf32("ConditionalSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("IncludeSect"), ToUtf32("IncludeSect"), 1),
            new cmajor::parsing::NonterminalParser(ToUtf32("IgnoreSect"), ToUtf32("IgnoreSect"), 0))));
    AddRule(new IncludeSectRule(ToUtf32("IncludeSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("<!")),
                                new cmajor::parsing::OptionalParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new cmajor::parsing::StringParser(ToUtf32("INCLUDE"))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                    new cmajor::parsing::CharParser('[')),
                new cmajor::parsing::NonterminalParser(ToUtf32("ExtSubsetDecl"), ToUtf32("ExtSubsetDecl"), 1)),
            new cmajor::parsing::StringParser(ToUtf32("]]>")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("IgnoreSect"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("<!")),
                                new cmajor::parsing::OptionalParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                            new cmajor::parsing::StringParser(ToUtf32("IGNORE"))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                    new cmajor::parsing::CharParser('[')),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("IgnoreSectContents"), ToUtf32("IgnoreSectContents"), 0))),
            new cmajor::parsing::StringParser(ToUtf32("]]>")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("IgnoreSectContents"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("Ignore"), ToUtf32("Ignore"), 0),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("<![")),
                                new cmajor::parsing::NonterminalParser(ToUtf32("IgnoreSectContents"), ToUtf32("IgnoreSectContents"), 0)),
                            new cmajor::parsing::StringParser(ToUtf32("]]>"))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Ignore"), ToUtf32("Ignore"), 0)))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Ignore"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::DifferenceParser(
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::StringParser(ToUtf32("<![")),
                                new cmajor::parsing::StringParser(ToUtf32("]]>"))))),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)))))));
    AddRule(new CharRefRule(ToUtf32("CharRef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::StringParser(ToUtf32("&#")),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("dcr"), ToUtf32("uint"), 0))),
                new cmajor::parsing::CharParser(';')),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::StringParser(ToUtf32("&#x")),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("hcr"), ToUtf32("hexuint"), 0))),
                new cmajor::parsing::CharParser(';')))));
    AddRule(new ReferenceRule(ToUtf32("Reference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("EntityRef"), ToUtf32("EntityRef"), 1),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("CharRef"), ToUtf32("CharRef"), 0)))));
    AddRule(new EntityRefRule(ToUtf32("EntityRef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('&'),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))),
            new cmajor::parsing::CharParser(';'))));
    AddRule(new PEReferenceRule(ToUtf32("PEReference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('%'),
                new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
            new cmajor::parsing::CharParser(';'))));
    AddRule(new EntityDeclRule(ToUtf32("EntityDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("GEDecl"), ToUtf32("GEDecl"), 1),
            new cmajor::parsing::NonterminalParser(ToUtf32("PEDecl"), ToUtf32("PEDecl"), 1))));
    AddRule(new GEDeclRule(ToUtf32("GEDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::GroupingParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::StringParser(ToUtf32("<!ENTITY")),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                new cmajor::parsing::NonterminalParser(ToUtf32("entityName"), ToUtf32("Name"), 0)),
                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("entityValue"), ToUtf32("EntityDef"), 1)),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new cmajor::parsing::CharParser('>')))));
    AddRule(new PEDeclRule(ToUtf32("PEDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::GroupingParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::StringParser(ToUtf32("<!ENTITY")),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                        new cmajor::parsing::CharParser('%')),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                                new cmajor::parsing::NonterminalParser(ToUtf32("peName"), ToUtf32("Name"), 0)),
                            new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("peValue"), ToUtf32("PEDef"), 1)),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
                new cmajor::parsing::CharParser('>')))));
    AddRule(new EntityDefRule(ToUtf32("EntityDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("EntityValue"), ToUtf32("EntityValue"), 1),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("notation"), ToUtf32("NDataDecl"), 0)))))));
    AddRule(new PEDefRule(ToUtf32("PEDef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("EntityValue"), ToUtf32("EntityValue"), 1),
            new cmajor::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("ExternalID"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::StringParser(ToUtf32("SYSTEM")),
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("SystemLiteral"), 0))),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("PUBLIC")),
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new cmajor::parsing::NonterminalParser(ToUtf32("p2"), ToUtf32("PubidLiteral"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("s2"), ToUtf32("SystemLiteral"), 0))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("NDataDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new cmajor::parsing::StringParser(ToUtf32("NDATA"))),
                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0))));
    AddRule(new TextDeclRule(ToUtf32("TextDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::StringParser(ToUtf32("<?xml")),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("VersionInfo"), ToUtf32("VersionInfo"), 1))),
                    new cmajor::parsing::NonterminalParser(ToUtf32("EncodingDecl"), ToUtf32("EncodingDecl"), 1)),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::StringParser(ToUtf32("?>")))));
    AddRule(new ExtParsedEntRule(ToUtf32("ExtParsedEnt"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("TextDecl"), ToUtf32("TextDecl"), 1)),
            new cmajor::parsing::NonterminalParser(ToUtf32("Content"), ToUtf32("Content"), 1))));
    AddRule(new EncodingDeclRule(ToUtf32("EncodingDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0),
                    new cmajor::parsing::StringParser(ToUtf32("encoding"))),
                new cmajor::parsing::NonterminalParser(ToUtf32("Eq"), ToUtf32("Eq"), 0)),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('\"'),
                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("en1"), ToUtf32("EncName"), 0))),
                        new cmajor::parsing::CharParser('\"')),
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('\''),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("en2"), ToUtf32("EncName"), 0))),
                        new cmajor::parsing::CharParser('\'')))))));
    AddRule(new EncNameRule(ToUtf32("EncName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharSetParser(ToUtf32("A-Za-z")),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::CharSetParser(ToUtf32("A-Za-z0-9._-"))))))));
    AddRule(new NotationDeclRule(ToUtf32("NotationDecl"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::StringParser(ToUtf32("<!NOTATION")),
                                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Name"), ToUtf32("Name"), 0)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("ExternalID"), ToUtf32("ExternalID"), 0),
                            new cmajor::parsing::NonterminalParser(ToUtf32("PublicID"), ToUtf32("PublicID"), 0)))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0))),
            new cmajor::parsing::CharParser('>'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("PublicID"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::StringParser(ToUtf32("PUBLIC")),
                new cmajor::parsing::NonterminalParser(ToUtf32("S"), ToUtf32("S"), 0)),
            new cmajor::parsing::NonterminalParser(ToUtf32("PubidLiteral"), ToUtf32("PubidLiteral"), 0))));
}

} } // namespace cmajor.xml
