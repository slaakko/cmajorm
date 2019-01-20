#include "CommandLine.hpp"
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

CommandLine* CommandLine::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

CommandLine* CommandLine::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    CommandLine* grammar(new CommandLine(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

CommandLine::CommandLine(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("CommandLine"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

std::vector<std::string> CommandLine::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    std::vector<std::string> result = *static_cast<cmajor::parsing::ValueObject<std::vector<std::string>>*>(value.get());
    stack.pop();
    return result;
}

class CommandLine::CommandLineRule : public cmajor::parsing::Rule
{
public:
    CommandLineRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<std::string>"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::vector<std::string>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CommandLineRule>(this, &CommandLineRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CommandLineRule>(this, &CommandLineRule::A1Action));
        cmajor::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CommandLineRule>(this, &CommandLineRule::Postfirst));
        cmajor::parsing::NonterminalParser* nextNonterminalParser = GetNonterminal(ToUtf32("next"));
        nextNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CommandLineRule>(this, &CommandLineRule::Postnext));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.push_back(context->fromfirst);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.push_back(context->fromnext);
    }
    void Postfirst(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postnext(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromnext_value = std::move(stack.top());
            context->fromnext = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromnext_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromfirst(), fromnext() {}
        std::vector<std::string> value;
        std::string fromfirst;
        std::string fromnext;
    };
};

class CommandLine::ArgumentRule : public cmajor::parsing::Rule
{
public:
    ArgumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArgumentRule>(this, &ArgumentRule::A0Action));
        cmajor::parsing::NonterminalParser* argElementNonterminalParser = GetNonterminal(ToUtf32("ArgElement"));
        argElementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArgumentRule>(this, &ArgumentRule::PostArgElement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(context->fromArgElement);
    }
    void PostArgElement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromArgElement_value = std::move(stack.top());
            context->fromArgElement = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromArgElement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromArgElement() {}
        std::string value;
        std::string fromArgElement;
    };
};

class CommandLine::ArgElementRule : public cmajor::parsing::Rule
{
public:
    ArgElementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArgElementRule>(this, &ArgElementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArgElementRule>(this, &ArgElementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArgElementRule>(this, &ArgElementRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArgElementRule>(this, &ArgElementRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArgElementRule>(this, &ArgElementRule::A4Action));
        cmajor::parsing::NonterminalParser* oddBackslashesAndLiteralQuotationMarkNonterminalParser = GetNonterminal(ToUtf32("OddBackslashesAndLiteralQuotationMark"));
        oddBackslashesAndLiteralQuotationMarkNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArgElementRule>(this, &ArgElementRule::PostOddBackslashesAndLiteralQuotationMark));
        cmajor::parsing::NonterminalParser* sNonterminalParser = GetNonterminal(ToUtf32("s"));
        sNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArgElementRule>(this, &ArgElementRule::Posts));
        cmajor::parsing::NonterminalParser* stringCharNonterminalParser = GetNonterminal(ToUtf32("StringChar"));
        stringCharNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArgElementRule>(this, &ArgElementRule::PostStringChar));
        cmajor::parsing::NonterminalParser* eNonterminalParser = GetNonterminal(ToUtf32("e"));
        eNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArgElementRule>(this, &ArgElementRule::Poste));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOddBackslashesAndLiteralQuotationMark;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(context->froms);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(context->fromStringChar);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(context->frome);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(ToUtf8(std::u32string(matchBegin, matchEnd)));
    }
    void PostOddBackslashesAndLiteralQuotationMark(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOddBackslashesAndLiteralQuotationMark_value = std::move(stack.top());
            context->fromOddBackslashesAndLiteralQuotationMark = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromOddBackslashesAndLiteralQuotationMark_value.get());
            stack.pop();
        }
    }
    void Posts(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> froms_value = std::move(stack.top());
            context->froms = *static_cast<cmajor::parsing::ValueObject<std::string>*>(froms_value.get());
            stack.pop();
        }
    }
    void PostStringChar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStringChar_value = std::move(stack.top());
            context->fromStringChar = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromStringChar_value.get());
            stack.pop();
        }
    }
    void Poste(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frome_value = std::move(stack.top());
            context->frome = *static_cast<cmajor::parsing::ValueObject<std::string>*>(frome_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromOddBackslashesAndLiteralQuotationMark(), froms(), fromStringChar(), frome() {}
        std::string value;
        std::string fromOddBackslashesAndLiteralQuotationMark;
        std::string froms;
        std::string fromStringChar;
        std::string frome;
    };
};

class CommandLine::OddBackslashesAndLiteralQuotationMarkRule : public cmajor::parsing::Rule
{
public:
    OddBackslashesAndLiteralQuotationMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OddBackslashesAndLiteralQuotationMarkRule>(this, &OddBackslashesAndLiteralQuotationMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::u32string bs = std::u32string(matchBegin, matchEnd);
        int n = bs.length() - 1;
        pass = n % 2 == 1;
        if (pass)
        {
            context->value = std::string(n / 2, '\\');
            context->value.append(1, '"');
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::string value;
    };
};

class CommandLine::EvenBackslashesAndQuotationMarkRule : public cmajor::parsing::Rule
{
public:
    EvenBackslashesAndQuotationMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EvenBackslashesAndQuotationMarkRule>(this, &EvenBackslashesAndQuotationMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::u32string bs = std::u32string(matchBegin, matchEnd);
        int n = bs.length() - 1;
        pass = n % 2 == 0;
        if (pass) context->value = std::string(n / 2, '\\');
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::string value;
    };
};

class CommandLine::StringCharRule : public cmajor::parsing::Rule
{
public:
    StringCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringCharRule>(this, &StringCharRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringCharRule>(this, &StringCharRule::A1Action));
        cmajor::parsing::NonterminalParser* oddBackslashesAndLiteralQuotationMarkNonterminalParser = GetNonterminal(ToUtf32("OddBackslashesAndLiteralQuotationMark"));
        oddBackslashesAndLiteralQuotationMarkNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringCharRule>(this, &StringCharRule::PostOddBackslashesAndLiteralQuotationMark));
        cmajor::parsing::NonterminalParser* evenBackslashesAndQuotationMarkNonterminalParser = GetNonterminal(ToUtf32("EvenBackslashesAndQuotationMark"));
        evenBackslashesAndQuotationMarkNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringCharRule>(this, &StringCharRule::PostEvenBackslashesAndQuotationMark));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOddBackslashesAndLiteralQuotationMark;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf8(std::u32string(matchBegin, matchEnd));
    }
    void PostOddBackslashesAndLiteralQuotationMark(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOddBackslashesAndLiteralQuotationMark_value = std::move(stack.top());
            context->fromOddBackslashesAndLiteralQuotationMark = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromOddBackslashesAndLiteralQuotationMark_value.get());
            stack.pop();
        }
    }
    void PostEvenBackslashesAndQuotationMark(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEvenBackslashesAndQuotationMark_value = std::move(stack.top());
            context->fromEvenBackslashesAndQuotationMark = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromEvenBackslashesAndQuotationMark_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromOddBackslashesAndLiteralQuotationMark(), fromEvenBackslashesAndQuotationMark() {}
        std::string value;
        std::string fromOddBackslashesAndLiteralQuotationMark;
        std::string fromEvenBackslashesAndQuotationMark;
    };
};

void CommandLine::GetReferencedGrammars()
{
}

void CommandLine::CreateRules()
{
    AddRule(new CommandLineRule(ToUtf32("CommandLine"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Spaces"), ToUtf32("Spaces"), 0)),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("Argument"), 0))),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Spaces"), ToUtf32("Spaces"), 0),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("next"), ToUtf32("Argument"), 0)))))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("Spaces"), ToUtf32("Spaces"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Spaces"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::CharSetParser(ToUtf32("\t ")))));
    AddRule(new ArgumentRule(ToUtf32("Argument"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ArgElement"), ToUtf32("ArgElement"), 0))))));
    AddRule(new ArgElementRule(ToUtf32("ArgElement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("OddBackslashesAndLiteralQuotationMark"), ToUtf32("OddBackslashesAndLiteralQuotationMark"), 0)),
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("s"), ToUtf32("EvenBackslashesAndQuotationMark"), 0)),
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("StringChar"), ToUtf32("StringChar"), 0))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("e"), ToUtf32("EvenBackslashesAndQuotationMark"), 0)))),
            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                new cmajor::parsing::CharSetParser(ToUtf32("\t "), true)))));
    AddRule(new OddBackslashesAndLiteralQuotationMarkRule(ToUtf32("OddBackslashesAndLiteralQuotationMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::PositiveParser(
                        new cmajor::parsing::StringParser(ToUtf32("\\"))),
                    new cmajor::parsing::CharParser('\"'))))));
    AddRule(new EvenBackslashesAndQuotationMarkRule(ToUtf32("EvenBackslashesAndQuotationMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::StringParser(ToUtf32("\\"))),
                    new cmajor::parsing::CharParser('\"'))))));
    AddRule(new StringCharRule(ToUtf32("StringChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("OddBackslashesAndLiteralQuotationMark"), ToUtf32("OddBackslashesAndLiteralQuotationMark"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::AnyCharParser(),
                        new cmajor::parsing::NonterminalParser(ToUtf32("EvenBackslashesAndQuotationMark"), ToUtf32("EvenBackslashesAndQuotationMark"), 0)))))));
}

} } // namespace cmajor.parser
