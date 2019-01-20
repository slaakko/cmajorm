#include "Specifier.hpp"
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

Specifier* Specifier::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Specifier* Specifier::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Specifier* grammar(new Specifier(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Specifier::Specifier(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Specifier"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

Specifiers Specifier::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    Specifiers result = *static_cast<cmajor::parsing::ValueObject<Specifiers>*>(value.get());
    stack.pop();
    return result;
}

class Specifier::SpecifiersRule : public cmajor::parsing::Rule
{
public:
    SpecifiersRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifiers"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Specifiers>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifiersRule>(this, &SpecifiersRule::A0Action));
        cmajor::parsing::NonterminalParser* specifierNonterminalParser = GetNonterminal(ToUtf32("Specifier"));
        specifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SpecifiersRule>(this, &SpecifiersRule::PostSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->value | context->fromSpecifier;
    }
    void PostSpecifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSpecifier_value = std::move(stack.top());
            context->fromSpecifier = *static_cast<cmajor::parsing::ValueObject<Specifiers>*>(fromSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromSpecifier() {}
        Specifiers value;
        Specifiers fromSpecifier;
    };
};

class Specifier::SpecifierRule : public cmajor::parsing::Rule
{
public:
    SpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Specifiers"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Specifiers>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A10Action));
        cmajor::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A11Action));
        cmajor::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A12Action));
        cmajor::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A13Action));
        cmajor::parsing::ActionParser* a14ActionParser = GetAction(ToUtf32("A14"));
        a14ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A14Action));
        cmajor::parsing::ActionParser* a15ActionParser = GetAction(ToUtf32("A15"));
        a15ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A15Action));
        cmajor::parsing::ActionParser* a16ActionParser = GetAction(ToUtf32("A16"));
        a16ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A16Action));
        cmajor::parsing::ActionParser* a17ActionParser = GetAction(ToUtf32("A17"));
        a17ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A17Action));
        cmajor::parsing::ActionParser* a18ActionParser = GetAction(ToUtf32("A18"));
        a18ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpecifierRule>(this, &SpecifierRule::A18Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::public_;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::protected_;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::private_;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::internal_;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::static_;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::virtual_;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::override_;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::abstract_;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::inline_;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::explicit_;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::external_;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::suppress_;
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::default_;
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::constexpr_;
    }
    void A14Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::cdecl_;
    }
    void A15Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::nothrow_;
    }
    void A16Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::throw_;
    }
    void A17Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::new_;
    }
    void A18Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Specifiers::unit_test_;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Specifiers value;
    };
};

void Specifier::GetReferencedGrammars()
{
}

void Specifier::CreateRules()
{
    AddRule(new SpecifiersRule(ToUtf32("Specifiers"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Specifier"), ToUtf32("Specifier"), 0))))));
    AddRule(new SpecifierRule(ToUtf32("Specifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                            new cmajor::parsing::AlternativeParser(
                                                                new cmajor::parsing::AlternativeParser(
                                                                    new cmajor::parsing::AlternativeParser(
                                                                        new cmajor::parsing::AlternativeParser(
                                                                            new cmajor::parsing::AlternativeParser(
                                                                                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                                                                    new cmajor::parsing::KeywordParser(ToUtf32("public"))),
                                                                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                                                    new cmajor::parsing::KeywordParser(ToUtf32("protected")))),
                                                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                                                new cmajor::parsing::KeywordParser(ToUtf32("private")))),
                                                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                                            new cmajor::parsing::KeywordParser(ToUtf32("internal")))),
                                                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                                        new cmajor::parsing::KeywordParser(ToUtf32("static")))),
                                                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                                    new cmajor::parsing::KeywordParser(ToUtf32("virtual")))),
                                                            new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                                                new cmajor::parsing::KeywordParser(ToUtf32("override")))),
                                                        new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                                            new cmajor::parsing::KeywordParser(ToUtf32("abstract")))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                                        new cmajor::parsing::KeywordParser(ToUtf32("inline")))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A9"),
                                                    new cmajor::parsing::KeywordParser(ToUtf32("explicit")))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A10"),
                                                new cmajor::parsing::KeywordParser(ToUtf32("extern")))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A11"),
                                            new cmajor::parsing::KeywordParser(ToUtf32("suppress")))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A12"),
                                        new cmajor::parsing::KeywordParser(ToUtf32("default")))),
                                new cmajor::parsing::ActionParser(ToUtf32("A13"),
                                    new cmajor::parsing::KeywordParser(ToUtf32("constexpr")))),
                            new cmajor::parsing::ActionParser(ToUtf32("A14"),
                                new cmajor::parsing::KeywordParser(ToUtf32("cdecl")))),
                        new cmajor::parsing::ActionParser(ToUtf32("A15"),
                            new cmajor::parsing::KeywordParser(ToUtf32("nothrow")))),
                    new cmajor::parsing::ActionParser(ToUtf32("A16"),
                        new cmajor::parsing::KeywordParser(ToUtf32("throw")))),
                new cmajor::parsing::ActionParser(ToUtf32("A17"),
                    new cmajor::parsing::KeywordParser(ToUtf32("new")))),
            new cmajor::parsing::ActionParser(ToUtf32("A18"),
                new cmajor::parsing::KeywordParser(ToUtf32("unit_test"))))));
}

} } // namespace cmajor.parser
