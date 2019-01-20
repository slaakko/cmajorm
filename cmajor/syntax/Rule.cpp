#include "Rule.hpp"
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
#include <cmajor/syntax/Element.hpp>
#include <cmajor/syntax/Composite.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Rule* Rule::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Rule* Rule::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Rule* grammar(new Rule(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Rule::Rule(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Rule"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::parsing::Rule* Rule::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Scope* enclosingScope)
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
    cmajor::parsing::Rule* result = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Rule*>*>(value.get());
    stack.pop();
    return result;
}

class Rule::RuleRule : public cmajor::parsing::Rule
{
public:
    RuleRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Rule*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RuleRule>(this, &RuleRule::A0Action));
        cmajor::parsing::NonterminalParser* ruleHeaderNonterminalParser = GetNonterminal(ToUtf32("RuleHeader"));
        ruleHeaderNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleHeader));
        ruleHeaderNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleRule>(this, &RuleRule::PostRuleHeader));
        cmajor::parsing::NonterminalParser* ruleBodyNonterminalParser = GetNonterminal(ToUtf32("RuleBody"));
        ruleBodyNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreRuleBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRuleHeader;
    }
    void PreRuleHeader(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->enclosingScope)));
    }
    void PostRuleHeader(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromRuleHeader_value = std::move(stack.top());
            context->fromRuleHeader = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Rule*>*>(fromRuleHeader_value.get());
            stack.pop();
        }
    }
    void PreRuleBody(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromRuleHeader() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Rule* value;
        cmajor::parsing::Rule* fromRuleHeader;
    };
};

class Rule::RuleHeaderRule : public cmajor::parsing::Rule
{
public:
    RuleHeaderRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Rule*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RuleHeaderRule>(this, &RuleHeaderRule::A0Action));
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleHeaderRule>(this, &RuleHeaderRule::Postname));
        cmajor::parsing::NonterminalParser* signatureNonterminalParser = GetNonterminal(ToUtf32("Signature"));
        signatureNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RuleHeaderRule>(this, &RuleHeaderRule::PreSignature));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Rule(context->fromname, context->enclosingScope);
        context->value->SetSpan(span);
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
    void PreSignature(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromname() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Rule* value;
        std::u32string fromname;
    };
};

class Rule::RuleBodyRule : public cmajor::parsing::Rule
{
public:
    RuleBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Rule*"), ToUtf32("rule")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> rule_value = std::move(stack.top());
        context->rule = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Rule*>*>(rule_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RuleBodyRule>(this, &RuleBodyRule::A0Action));
        cmajor::parsing::NonterminalParser* definitionNonterminalParser = GetNonterminal(ToUtf32("definition"));
        definitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RuleBodyRule>(this, &RuleBodyRule::Predefinition));
        definitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleBodyRule>(this, &RuleBodyRule::Postdefinition));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->SetDefinition(context->fromdefinition);
    }
    void Predefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->rule->GetScope())));
    }
    void Postdefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromdefinition_value = std::move(stack.top());
            context->fromdefinition = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromdefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): rule(), fromdefinition() {}
        cmajor::parsing::Rule* rule;
        cmajor::parsing::Parser* fromdefinition;
    };
};

void Rule::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.syntax.Element"));
    if (!grammar0)
    {
        grammar0 = cmajor::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.syntax.Composite"));
    if (!grammar1)
    {
        grammar1 = cmajor::syntax::Composite::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar2)
    {
        grammar2 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void Rule::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Alternative"), this, ToUtf32("Composite.Alternative")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("Element.Signature")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("cmajor.parsing.stdlib.string")));
    AddRule(new RuleRule(ToUtf32("Rule"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("RuleHeader"), ToUtf32("RuleHeader"), 1)),
                new cmajor::parsing::StringParser(ToUtf32("::="))),
            new cmajor::parsing::NonterminalParser(ToUtf32("RuleBody"), ToUtf32("RuleBody"), 1))));
    AddRule(new RuleHeaderRule(ToUtf32("RuleHeader"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("name"), ToUtf32("Identifier"), 0)),
            new cmajor::parsing::NonterminalParser(ToUtf32("Signature"), ToUtf32("Signature"), 1))));
    AddRule(new RuleBodyRule(ToUtf32("RuleBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("definition"), ToUtf32("Alternative"), 1)),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
}

} } // namespace cmajor.syntax
