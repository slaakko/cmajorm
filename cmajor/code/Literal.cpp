#include "Literal.hpp"
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

namespace cmajor { namespace code {

using namespace cmajor::codedom;
using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Literal* Literal::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Literal* Literal::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Literal* grammar(new Literal(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Literal::Literal(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Literal"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.code")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::codedom::Literal* Literal::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    cmajor::codedom::Literal* result = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(value.get());
    stack.pop();
    return result;
}

class Literal::LiteralRule : public cmajor::parsing::Rule
{
public:
    LiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Literal*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A5Action));
        cmajor::parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal(ToUtf32("IntegerLiteral"));
        integerLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostIntegerLiteral));
        cmajor::parsing::NonterminalParser* characterLiteralNonterminalParser = GetNonterminal(ToUtf32("CharacterLiteral"));
        characterLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostCharacterLiteral));
        cmajor::parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal(ToUtf32("FloatingLiteral"));
        floatingLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostFloatingLiteral));
        cmajor::parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal(ToUtf32("StringLiteral"));
        stringLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostStringLiteral));
        cmajor::parsing::NonterminalParser* booleanLiteralNonterminalParser = GetNonterminal(ToUtf32("BooleanLiteral"));
        booleanLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostBooleanLiteral));
        cmajor::parsing::NonterminalParser* pointerLiteralNonterminalParser = GetNonterminal(ToUtf32("PointerLiteral"));
        pointerLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostPointerLiteral));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIntegerLiteral;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharacterLiteral;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFloatingLiteral;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStringLiteral;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBooleanLiteral;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPointerLiteral;
    }
    void PostIntegerLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context->fromIntegerLiteral = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharacterLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCharacterLiteral_value = std::move(stack.top());
            context->fromCharacterLiteral = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(fromCharacterLiteral_value.get());
            stack.pop();
        }
    }
    void PostFloatingLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context->fromFloatingLiteral = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context->fromStringLiteral = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostBooleanLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromBooleanLiteral_value = std::move(stack.top());
            context->fromBooleanLiteral = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(fromBooleanLiteral_value.get());
            stack.pop();
        }
    }
    void PostPointerLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPointerLiteral_value = std::move(stack.top());
            context->fromPointerLiteral = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(fromPointerLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromIntegerLiteral(), fromCharacterLiteral(), fromFloatingLiteral(), fromStringLiteral(), fromBooleanLiteral(), fromPointerLiteral() {}
        cmajor::codedom::Literal* value;
        cmajor::codedom::Literal* fromIntegerLiteral;
        cmajor::codedom::Literal* fromCharacterLiteral;
        cmajor::codedom::Literal* fromFloatingLiteral;
        cmajor::codedom::Literal* fromStringLiteral;
        cmajor::codedom::Literal* fromBooleanLiteral;
        cmajor::codedom::Literal* fromPointerLiteral;
    };
};

class Literal::IntegerLiteralRule : public cmajor::parsing::Rule
{
public:
    IntegerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Literal*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IntegerLiteralRule>(this, &IntegerLiteralRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Literal* value;
    };
};

class Literal::CharacterLiteralRule : public cmajor::parsing::Rule
{
public:
    CharacterLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Literal*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharacterLiteralRule>(this, &CharacterLiteralRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Literal* value;
    };
};

class Literal::CCharSequenceRule : public cmajor::parsing::Rule
{
public:
    CCharSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CCharSequenceRule>(this, &CCharSequenceRule::A0Action));
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

class Literal::FloatingLiteralRule : public cmajor::parsing::Rule
{
public:
    FloatingLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Literal*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Literal* value;
    };
};

class Literal::StringLiteralRule : public cmajor::parsing::Rule
{
public:
    StringLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Literal*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Literal* value;
    };
};

class Literal::BooleanLiteralRule : public cmajor::parsing::Rule
{
public:
    BooleanLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Literal*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BooleanLiteralRule>(this, &BooleanLiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(U"true");
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(U"false");
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Literal* value;
    };
};

class Literal::PointerLiteralRule : public cmajor::parsing::Rule
{
public:
    PointerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Literal*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Literal*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PointerLiteralRule>(this, &PointerLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::Literal(U"nullptr");
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::Literal* value;
    };
};

void Literal::GetReferencedGrammars()
{
}

void Literal::CreateRules()
{
    AddRule(new LiteralRule(ToUtf32("Literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("IntegerLiteral"), ToUtf32("IntegerLiteral"), 0)),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("CharacterLiteral"), ToUtf32("CharacterLiteral"), 0))),
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("FloatingLiteral"), ToUtf32("FloatingLiteral"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("StringLiteral"), ToUtf32("StringLiteral"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("BooleanLiteral"), ToUtf32("BooleanLiteral"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                new cmajor::parsing::NonterminalParser(ToUtf32("PointerLiteral"), ToUtf32("PointerLiteral"), 0)))));
    AddRule(new IntegerLiteralRule(ToUtf32("IntegerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::TokenParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("dl"), ToUtf32("DecimalLiteral"), 0),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("is1"), ToUtf32("IntegerSuffix"), 0))))),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::TokenParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("ol"), ToUtf32("OctalLiteral"), 0),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("is2"), ToUtf32("IntegerSuffix"), 0)))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("hl"), ToUtf32("HexadecimalLiteral"), 0),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("is3"), ToUtf32("IntegerSuffix"), 0))))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("DecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharSetParser(ToUtf32("1-9")),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::DigitParser())))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("OctalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('0'),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("OctalDigit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::CharSetParser(ToUtf32("0-7"))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("HexadecimalLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::StringParser(ToUtf32("0x")),
                        new cmajor::parsing::StringParser(ToUtf32("0X")))),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::PositiveParser(
                        new cmajor::parsing::HexDigitParser()))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("IntegerSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("LongLongSuffix"), ToUtf32("LongLongSuffix"), 0),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("LongSuffix"), ToUtf32("LongSuffix"), 0)))))),
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("LongLongSuffix"), ToUtf32("LongLongSuffix"), 0),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0))))),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("LongSuffix"), ToUtf32("LongSuffix"), 0),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("UnsignedSuffix"), ToUtf32("UnsignedSuffix"), 0)))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("UnsignedSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::CharParser('u'),
            new cmajor::parsing::CharParser('U'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("LongLongSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::StringParser(ToUtf32("ll")),
            new cmajor::parsing::StringParser(ToUtf32("LL")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("LongSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::CharParser('l'),
            new cmajor::parsing::CharParser('L'))));
    AddRule(new CharacterLiteralRule(ToUtf32("CharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("NarrowCharacterLiteral"), ToUtf32("NarrowCharacterLiteral"), 0)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("UniversalCharacterLiteral"), ToUtf32("UniversalCharacterLiteral"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("WideCharacterLiteral"), ToUtf32("WideCharacterLiteral"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("NarrowCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\''),
                    new cmajor::parsing::NonterminalParser(ToUtf32("CCharSequence"), ToUtf32("CCharSequence"), 0)),
                new cmajor::parsing::CharParser('\'')))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("UniversalCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('u'),
                            new cmajor::parsing::CharParser('\'')),
                        new cmajor::parsing::NonterminalParser(ToUtf32("cs1"), ToUtf32("CCharSequence"), 0)),
                    new cmajor::parsing::CharParser('\''))),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('U'),
                            new cmajor::parsing::CharParser('\'')),
                        new cmajor::parsing::NonterminalParser(ToUtf32("cs2"), ToUtf32("CCharSequence"), 0)),
                    new cmajor::parsing::CharParser('\''))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("WideCharacterLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('L'),
                        new cmajor::parsing::CharParser('\'')),
                    new cmajor::parsing::NonterminalParser(ToUtf32("CCharSequence"), ToUtf32("CCharSequence"), 0)),
                new cmajor::parsing::CharParser('\'')))));
    AddRule(new CCharSequenceRule(ToUtf32("CCharSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::PositiveParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("CChar"), ToUtf32("CChar"), 0))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("CChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::CharSetParser(ToUtf32("\'\\\n"), true),
            new cmajor::parsing::NonterminalParser(ToUtf32("EscapeSequence"), ToUtf32("EscapeSequence"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("EscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("SimpleEscapeSequence"), ToUtf32("SimpleEscapeSequence"), 0),
                new cmajor::parsing::NonterminalParser(ToUtf32("OctalEscapeSequence"), ToUtf32("OctalEscapeSequence"), 0)),
            new cmajor::parsing::NonterminalParser(ToUtf32("HexadecimalEscapeSequence"), ToUtf32("HexadecimalEscapeSequence"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("SimpleEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                new cmajor::parsing::StringParser(ToUtf32("\\\'")),
                                                new cmajor::parsing::StringParser(ToUtf32("\\\""))),
                                            new cmajor::parsing::StringParser(ToUtf32("\\?"))),
                                        new cmajor::parsing::StringParser(ToUtf32("\\\\"))),
                                    new cmajor::parsing::StringParser(ToUtf32("\\a"))),
                                new cmajor::parsing::StringParser(ToUtf32("\\b"))),
                            new cmajor::parsing::StringParser(ToUtf32("\\f"))),
                        new cmajor::parsing::StringParser(ToUtf32("\\n"))),
                    new cmajor::parsing::StringParser(ToUtf32("\\r"))),
                new cmajor::parsing::StringParser(ToUtf32("\\t"))),
            new cmajor::parsing::StringParser(ToUtf32("\\v")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("OctalEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\\'),
                    new cmajor::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0)),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("OctalDigit"), ToUtf32("OctalDigit"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("HexadecimalEscapeSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::StringParser(ToUtf32("\\x")),
            new cmajor::parsing::PositiveParser(
                new cmajor::parsing::HexDigitParser()))));
    AddRule(new FloatingLiteralRule(ToUtf32("FloatingLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("FractionalConstant"), ToUtf32("FractionalConstant"), 0),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("FloatingSuffix"), ToUtf32("FloatingSuffix"), 0))))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0),
                            new cmajor::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0)),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("FloatingSuffix"), ToUtf32("FloatingSuffix"), 0))))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("FractionalConstant"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0)),
                    new cmajor::parsing::CharParser('.')),
                new cmajor::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0)),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0),
                new cmajor::parsing::CharParser('.')))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("DigitSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::DigitParser())));
    AddRule(new cmajor::parsing::Rule(ToUtf32("ExponentPart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::CharParser('e'),
                        new cmajor::parsing::CharParser('E'))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Sign"), ToUtf32("Sign"), 0))),
            new cmajor::parsing::NonterminalParser(ToUtf32("DigitSequence"), ToUtf32("DigitSequence"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Sign"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::CharParser('+'),
            new cmajor::parsing::CharParser('-'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("FloatingSuffix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::CharSetParser(ToUtf32("fFlL"))));
    AddRule(new StringLiteralRule(ToUtf32("StringLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("EncodingPrefix"), ToUtf32("EncodingPrefix"), 0)),
                            new cmajor::parsing::CharParser('\"')),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("SCharSequence"), ToUtf32("SCharSequence"), 0))),
                    new cmajor::parsing::CharParser('\"'))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("EncodingPrefix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::StringParser(ToUtf32("u8")),
                    new cmajor::parsing::CharParser('u')),
                new cmajor::parsing::CharParser('U')),
            new cmajor::parsing::CharParser('L'))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("SCharSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::GroupingParser(
            new cmajor::parsing::PositiveParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("SChar"), ToUtf32("SChar"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("SChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::CharSetParser(ToUtf32("\"\\\n"), true),
            new cmajor::parsing::NonterminalParser(ToUtf32("EscapeSequence"), ToUtf32("EscapeSequence"), 0))));
    AddRule(new BooleanLiteralRule(ToUtf32("BooleanLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::KeywordParser(ToUtf32("true"))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::KeywordParser(ToUtf32("false"))))));
    AddRule(new PointerLiteralRule(ToUtf32("PointerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("nullptr")))));
}

} } // namespace cmajor.code
