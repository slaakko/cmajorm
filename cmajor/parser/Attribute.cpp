#include "Attribute.hpp"
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

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Attribute* Attribute::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Attribute* Attribute::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Attribute* grammar(new Attribute(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Attribute::Attribute(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Attribute"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::ast::Attributes* Attribute::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    cmajor::ast::Attributes* result = *static_cast<cmajor::parsing::ValueObject<cmajor::ast::Attributes*>*>(value.get());
    stack.pop();
    return result;
}

class Attribute::AttributesRule : public cmajor::parsing::Rule
{
public:
    AttributesRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::ast::Attributes*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::ast::Attributes*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttributesRule>(this, &AttributesRule::A0Action));
        cmajor::parsing::NonterminalParser* attributeNonterminalParser = GetNonterminal(ToUtf32("Attribute"));
        attributeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AttributesRule>(this, &AttributesRule::PreAttribute));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::ast::Attributes;
    }
    void PreAttribute(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::ast::Attributes*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::ast::Attributes* value;
    };
};

class Attribute::AttributeRule : public cmajor::parsing::Rule
{
public:
    AttributeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::ast::Attributes*"), ToUtf32("attributes")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> attributes_value = std::move(stack.top());
        context->attributes = *static_cast<cmajor::parsing::ValueObject<cmajor::ast::Attributes*>*>(attributes_value.get());
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
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AttributeRule>(this, &AttributeRule::A1Action));
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::Postname));
        cmajor::parsing::NonterminalParser* valNonterminalParser = GetNonterminal(ToUtf32("val"));
        valNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AttributeRule>(this, &AttributeRule::Postval));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->attributes->AddAttribute(span, context->fromname, context->fromval);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->attributes->AddAttribute(span, context->fromname);
    }
    void Postname(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromname_value = std::move(stack.top());
            context->fromname = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromname_value.get());
            stack.pop();
        }
    }
    void Postval(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromval_value = std::move(stack.top());
            context->fromval = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromval_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): attributes(), fromname(), fromval() {}
        cmajor::ast::Attributes* attributes;
        std::u32string fromname;
        std::u32string fromval;
    };
};

void Attribute::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Attribute::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("cmajor.parsing.stdlib.identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("cmajor.parsing.stdlib.string")));
    AddRule(new AttributesRule(ToUtf32("Attributes"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser('[')),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ListParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Attribute"), ToUtf32("Attribute"), 1),
                            new cmajor::parsing::CharParser(','))))),
            new cmajor::parsing::CharParser(']'))));
    AddRule(new AttributeRule(ToUtf32("Attribute"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("name"), ToUtf32("identifier"), 0),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('='),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("val"), ToUtf32("string"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::EmptyParser()))))));
}

} } // namespace cmajor.parser
