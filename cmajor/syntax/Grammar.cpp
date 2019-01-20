#include "Grammar.hpp"
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
#include <cmajor/syntax/Rule.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

GrammarGrammar* GrammarGrammar::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

GrammarGrammar* GrammarGrammar::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    GrammarGrammar* grammar(new GrammarGrammar(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

GrammarGrammar::GrammarGrammar(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("GrammarGrammar"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::parsing::Grammar* GrammarGrammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Scope* enclosingScope)
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
    cmajor::parsing::Grammar* result = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>*>(value.get());
    stack.pop();
    return result;
}

class GrammarGrammar::GrammarRule : public cmajor::parsing::Rule
{
public:
    GrammarRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Scope*"), ToUtf32("enclosingScope")));
        SetValueTypeName(ToUtf32("cmajor::parsing::Grammar*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<GrammarRule>(this, &GrammarRule::A0Action));
        cmajor::parsing::NonterminalParser* nameNonterminalParser = GetNonterminal(ToUtf32("name"));
        nameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GrammarRule>(this, &GrammarRule::Postname));
        cmajor::parsing::NonterminalParser* grammarContentNonterminalParser = GetNonterminal(ToUtf32("GrammarContent"));
        grammarContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GrammarRule>(this, &GrammarRule::PreGrammarContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Grammar(context->fromname, context->enclosingScope);
        context->value->SetSpan(span);
        context->value->SetNs(context->enclosingScope->Ns());
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
    void PreGrammarContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): enclosingScope(), value(), fromname() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Grammar* value;
        std::u32string fromname;
    };
};

class GrammarGrammar::GrammarContentRule : public cmajor::parsing::Rule
{
public:
    GrammarContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Grammar*"), ToUtf32("grammar")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<GrammarContentRule>(this, &GrammarContentRule::A0Action));
        cmajor::parsing::NonterminalParser* startClauseNonterminalParser = GetNonterminal(ToUtf32("StartClause"));
        startClauseNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreStartClause));
        cmajor::parsing::NonterminalParser* skipClauseNonterminalParser = GetNonterminal(ToUtf32("SkipClause"));
        skipClauseNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreSkipClause));
        cmajor::parsing::NonterminalParser* ruleLinkNonterminalParser = GetNonterminal(ToUtf32("RuleLink"));
        ruleLinkNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRuleLink));
        cmajor::parsing::NonterminalParser* ruleNonterminalParser = GetNonterminal(ToUtf32("Rule"));
        ruleNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRule));
        ruleNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GrammarContentRule>(this, &GrammarContentRule::PostRule));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->AddRule(context->fromRule);
    }
    void PreStartClause(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>(context->grammar)));
    }
    void PreSkipClause(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>(context->grammar)));
    }
    void PreRuleLink(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>(context->grammar)));
    }
    void PreRule(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->grammar->GetScope())));
    }
    void PostRule(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromRule_value = std::move(stack.top());
            context->fromRule = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Rule*>*>(fromRule_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): grammar(), fromRule() {}
        cmajor::parsing::Grammar* grammar;
        cmajor::parsing::Rule* fromRule;
    };
};

class GrammarGrammar::StartClauseRule : public cmajor::parsing::Rule
{
public:
    StartClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Grammar*"), ToUtf32("grammar")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StartClauseRule>(this, &StartClauseRule::A0Action));
        cmajor::parsing::NonterminalParser* strtNonterminalParser = GetNonterminal(ToUtf32("strt"));
        strtNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StartClauseRule>(this, &StartClauseRule::Poststrt));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->SetStartRuleName(context->fromstrt);
    }
    void Poststrt(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromstrt_value = std::move(stack.top());
            context->fromstrt = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromstrt_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): grammar(), fromstrt() {}
        cmajor::parsing::Grammar* grammar;
        std::u32string fromstrt;
    };
};

class GrammarGrammar::SkipClauseRule : public cmajor::parsing::Rule
{
public:
    SkipClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::Grammar*"), ToUtf32("grammar")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> grammar_value = std::move(stack.top());
        context->grammar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>*>(grammar_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SkipClauseRule>(this, &SkipClauseRule::A0Action));
        cmajor::parsing::NonterminalParser* skpNonterminalParser = GetNonterminal(ToUtf32("skp"));
        skpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SkipClauseRule>(this, &SkipClauseRule::Postskp));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->SetSkipRuleName(context->fromskp);
    }
    void Postskp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromskp_value = std::move(stack.top());
            context->fromskp = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromskp_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): grammar(), fromskp() {}
        cmajor::parsing::Grammar* grammar;
        std::u32string fromskp;
    };
};

void GrammarGrammar::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.syntax.Element"));
    if (!grammar0)
    {
        grammar0 = cmajor::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar1)
    {
        grammar1 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.syntax.Rule"));
    if (!grammar2)
    {
        grammar2 = cmajor::syntax::Rule::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void GrammarGrammar::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Element.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("char"), this, ToUtf32("cmajor.parsing.stdlib.char")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Rule"), this, ToUtf32("Rule.Rule")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("Element.Signature")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("RuleLink"), this, ToUtf32("Element.RuleLink")));
    AddRule(new GrammarRule(ToUtf32("Grammar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("grammar")),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("name"), ToUtf32("Identifier"), 0)))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('{'))),
                new cmajor::parsing::NonterminalParser(ToUtf32("GrammarContent"), ToUtf32("GrammarContent"), 1)),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::CharParser('}')))));
    AddRule(new GrammarContentRule(ToUtf32("GrammarContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("StartClause"), ToUtf32("StartClause"), 1),
                            new cmajor::parsing::NonterminalParser(ToUtf32("SkipClause"), ToUtf32("SkipClause"), 1)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("RuleLink"), ToUtf32("RuleLink"), 1)),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Rule"), ToUtf32("Rule"), 1)))))));
    AddRule(new StartClauseRule(ToUtf32("StartClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::KeywordParser(ToUtf32("start")),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("strt"), ToUtf32("Identifier"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::CharParser(';')))));
    AddRule(new SkipClauseRule(ToUtf32("SkipClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::KeywordParser(ToUtf32("skip")),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("skp"), ToUtf32("QualifiedId"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::CharParser(';')))));
}

} } // namespace cmajor.syntax
