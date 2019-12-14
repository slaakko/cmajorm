#include "LibraryFile.hpp"
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
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/Namespace.hpp>
#include <cmajor/syntax/Element.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace soulng::util;
using namespace cmajor::unicode;

LibraryFile* LibraryFile::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

LibraryFile* LibraryFile::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    LibraryFile* grammar(new LibraryFile(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

LibraryFile::LibraryFile(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("LibraryFile"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

void LibraryFile::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::ParsingDomain* parsingDomain)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<cmajor::parsing::ParsingDomain*>(parsingDomain)));
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

class LibraryFile::LibraryFileRule : public cmajor::parsing::Rule
{
public:
    LibraryFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<LibraryFileRule>(this, &LibraryFileRule::PreNamespaceContent));
    }
    void PreNamespaceContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): parsingDomain() {}
        cmajor::parsing::ParsingDomain* parsingDomain;
    };
};

class LibraryFile::NamespaceContentRule : public cmajor::parsing::Rule
{
public:
    NamespaceContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A0Action));
        cmajor::parsing::NonterminalParser* grammarNonterminalParser = GetNonterminal(ToUtf32("Grammar"));
        grammarNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreGrammar));
        grammarNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostGrammar));
        cmajor::parsing::NonterminalParser* namespaceNonterminalParser = GetNonterminal(ToUtf32("Namespace"));
        namespaceNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreNamespace));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parsingDomain->AddGrammar(context->fromGrammar);
    }
    void PreGrammar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->parsingDomain->CurrentNamespace()->GetScope())));
    }
    void PostGrammar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGrammar_value = std::move(stack.top());
            context->fromGrammar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>*>(fromGrammar_value.get());
            stack.pop();
        }
    }
    void PreNamespace(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): parsingDomain(), fromGrammar() {}
        cmajor::parsing::ParsingDomain* parsingDomain;
        cmajor::parsing::Grammar* fromGrammar;
    };
};

class LibraryFile::NamespaceRule : public cmajor::parsing::Rule
{
public:
    NamespaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::ParsingDomain*"), ToUtf32("parsingDomain")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parsingDomain_value = std::move(stack.top());
        context->parsingDomain = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::ParsingDomain*>*>(parsingDomain_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A1Action));
        cmajor::parsing::NonterminalParser* nsNonterminalParser = GetNonterminal(ToUtf32("ns"));
        nsNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceRule>(this, &NamespaceRule::Postns));
        cmajor::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceRule>(this, &NamespaceRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parsingDomain->BeginNamespace(context->fromns);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parsingDomain->EndNamespace();
    }
    void Postns(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromns_value = std::move(stack.top());
            context->fromns = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromns_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::ParsingDomain*>(context->parsingDomain)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): parsingDomain(), fromns() {}
        cmajor::parsing::ParsingDomain* parsingDomain;
        std::u32string fromns;
    };
};

class LibraryFile::GrammarRule : public cmajor::parsing::Rule
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
        cmajor::parsing::NonterminalParser* grammarNameNonterminalParser = GetNonterminal(ToUtf32("grammarName"));
        grammarNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GrammarRule>(this, &GrammarRule::PostgrammarName));
        cmajor::parsing::NonterminalParser* grammarContentNonterminalParser = GetNonterminal(ToUtf32("GrammarContent"));
        grammarContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GrammarRule>(this, &GrammarRule::PreGrammarContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Grammar(context->fromgrammarName, context->enclosingScope);
        context->value->SetNs(context->enclosingScope->Ns());
    }
    void PostgrammarName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromgrammarName_value = std::move(stack.top());
            context->fromgrammarName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromgrammarName_value.get());
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
        Context(): enclosingScope(), value(), fromgrammarName() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Grammar* value;
        std::u32string fromgrammarName;
    };
};

class LibraryFile::GrammarContentRule : public cmajor::parsing::Rule
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
        cmajor::parsing::NonterminalParser* ruleNonterminalParser = GetNonterminal(ToUtf32("Rule"));
        ruleNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GrammarContentRule>(this, &GrammarContentRule::PreRule));
        ruleNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GrammarContentRule>(this, &GrammarContentRule::PostRule));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->grammar->AddRule(context->fromRule);
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

class LibraryFile::RuleRule : public cmajor::parsing::Rule
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
        cmajor::parsing::NonterminalParser* ruleNameNonterminalParser = GetNonterminal(ToUtf32("ruleName"));
        ruleNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleRule>(this, &RuleRule::PostruleName));
        cmajor::parsing::NonterminalParser* signatureNonterminalParser = GetNonterminal(ToUtf32("Signature"));
        signatureNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<RuleRule>(this, &RuleRule::PreSignature));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::parsing::Rule(context->fromruleName, context->enclosingScope);
    }
    void PostruleName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromruleName_value = std::move(stack.top());
            context->fromruleName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromruleName_value.get());
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
        Context(): enclosingScope(), value(), fromruleName() {}
        cmajor::parsing::Scope* enclosingScope;
        cmajor::parsing::Rule* value;
        std::u32string fromruleName;
    };
};

void LibraryFile::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.syntax.Element"));
    if (!grammar1)
    {
        grammar1 = cmajor::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void LibraryFile::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("cmajor.parsing.stdlib.identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("cmajor.parsing.stdlib.qualified_id")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("cmajor.parsing.stdlib.spaces_and_comments")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Element.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Signature"), this, ToUtf32("Element.Signature")));
    AddRule(new LibraryFileRule(ToUtf32("LibraryFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1)));
    AddRule(new NamespaceContentRule(ToUtf32("NamespaceContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Grammar"), ToUtf32("Grammar"), 1)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Namespace"), ToUtf32("Namespace"), 1))))));
    AddRule(new NamespaceRule(ToUtf32("Namespace"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("namespace")),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("ns"), ToUtf32("qualified_id"), 0)))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('{'))),
                new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('}'))))));
    AddRule(new GrammarRule(ToUtf32("Grammar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("grammar")),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("grammarName"), ToUtf32("Identifier"), 0)))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('{'))),
                new cmajor::parsing::NonterminalParser(ToUtf32("GrammarContent"), ToUtf32("GrammarContent"), 1)),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::CharParser('}')))));
    AddRule(new GrammarContentRule(ToUtf32("GrammarContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Rule"), ToUtf32("Rule"), 1))))));
    AddRule(new RuleRule(ToUtf32("Rule"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ruleName"), ToUtf32("Identifier"), 0)),
                new cmajor::parsing::NonterminalParser(ToUtf32("Signature"), ToUtf32("Signature"), 1)),
            new cmajor::parsing::CharParser(';'))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace cmajor.syntax
