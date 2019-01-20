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
#include <cmajor/parser/Expression.hpp>
#include <sstream>

namespace cmajor { namespace parser {

using namespace cmajor::util;
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

Literal::Literal(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Literal"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

LiteralNode* Literal::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<ParsingContext*>(ctx)));
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
    LiteralNode* result = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(value.get());
    stack.pop();
    return result;
}

class Literal::LiteralRule : public cmajor::parsing::Rule
{
public:
    LiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("LiteralNode*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<LiteralNode*>(context->value)));
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
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A7Action));
        cmajor::parsing::NonterminalParser* booleanLiteralNonterminalParser = GetNonterminal(ToUtf32("BooleanLiteral"));
        booleanLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostBooleanLiteral));
        cmajor::parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal(ToUtf32("FloatingLiteral"));
        floatingLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostFloatingLiteral));
        cmajor::parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal(ToUtf32("IntegerLiteral"));
        integerLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostIntegerLiteral));
        cmajor::parsing::NonterminalParser* charLiteralNonterminalParser = GetNonterminal(ToUtf32("CharLiteral"));
        charLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostCharLiteral));
        cmajor::parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal(ToUtf32("StringLiteral"));
        stringLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostStringLiteral));
        cmajor::parsing::NonterminalParser* nullLiteralNonterminalParser = GetNonterminal(ToUtf32("NullLiteral"));
        nullLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostNullLiteral));
        cmajor::parsing::NonterminalParser* arrayLiteralNonterminalParser = GetNonterminal(ToUtf32("ArrayLiteral"));
        arrayLiteralNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<LiteralRule>(this, &LiteralRule::PreArrayLiteral));
        arrayLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostArrayLiteral));
        cmajor::parsing::NonterminalParser* structuredLiteralNonterminalParser = GetNonterminal(ToUtf32("StructuredLiteral"));
        structuredLiteralNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<LiteralRule>(this, &LiteralRule::PreStructuredLiteral));
        structuredLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LiteralRule>(this, &LiteralRule::PostStructuredLiteral));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBooleanLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFloatingLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIntegerLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStringLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNullLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromArrayLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStructuredLiteral;
        context->value->SetText(std::u32string(matchBegin, matchEnd));
    }
    void PostBooleanLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromBooleanLiteral_value = std::move(stack.top());
            context->fromBooleanLiteral = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(fromBooleanLiteral_value.get());
            stack.pop();
        }
    }
    void PostFloatingLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context->fromFloatingLiteral = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostIntegerLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context->fromIntegerLiteral = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCharLiteral_value = std::move(stack.top());
            context->fromCharLiteral = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(fromCharLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context->fromStringLiteral = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostNullLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNullLiteral_value = std::move(stack.top());
            context->fromNullLiteral = *static_cast<cmajor::parsing::ValueObject<LiteralNode*>*>(fromNullLiteral_value.get());
            stack.pop();
        }
    }
    void PreArrayLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostArrayLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromArrayLiteral_value = std::move(stack.top());
            context->fromArrayLiteral = *static_cast<cmajor::parsing::ValueObject<ArrayLiteralNode*>*>(fromArrayLiteral_value.get());
            stack.pop();
        }
    }
    void PreStructuredLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostStructuredLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStructuredLiteral_value = std::move(stack.top());
            context->fromStructuredLiteral = *static_cast<cmajor::parsing::ValueObject<StructuredLiteralNode*>*>(fromStructuredLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromBooleanLiteral(), fromFloatingLiteral(), fromIntegerLiteral(), fromCharLiteral(), fromStringLiteral(), fromNullLiteral(), fromArrayLiteral(), fromStructuredLiteral() {}
        ParsingContext* ctx;
        LiteralNode* value;
        LiteralNode* fromBooleanLiteral;
        LiteralNode* fromFloatingLiteral;
        LiteralNode* fromIntegerLiteral;
        LiteralNode* fromCharLiteral;
        LiteralNode* fromStringLiteral;
        LiteralNode* fromNullLiteral;
        ArrayLiteralNode* fromArrayLiteral;
        StructuredLiteralNode* fromStructuredLiteral;
    };
};

class Literal::BooleanLiteralRule : public cmajor::parsing::Rule
{
public:
    BooleanLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<LiteralNode*>(context->value)));
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
        context->value = new BooleanLiteralNode(span, true);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BooleanLiteralNode(span, false);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        LiteralNode* value;
    };
};

class Literal::FloatingLiteralRule : public cmajor::parsing::Rule
{
public:
    FloatingLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<LiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FloatingLiteralRule>(this, &FloatingLiteralRule::A2Action));
        cmajor::parsing::NonterminalParser* floatingLiteralValueNonterminalParser = GetNonterminal(ToUtf32("FloatingLiteralValue"));
        floatingLiteralValueNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FloatingLiteralRule>(this, &FloatingLiteralRule::PostFloatingLiteralValue));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = CreateFloatingLiteralNode(context->s, context->fromFloatingLiteralValue, true);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = CreateFloatingLiteralNode(context->s, context->fromFloatingLiteralValue, false);
    }
    void PostFloatingLiteralValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFloatingLiteralValue_value = std::move(stack.top());
            context->fromFloatingLiteralValue = *static_cast<cmajor::parsing::ValueObject<double>*>(fromFloatingLiteralValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), s(), fromFloatingLiteralValue() {}
        LiteralNode* value;
        Span s;
        double fromFloatingLiteralValue;
    };
};

class Literal::FloatingLiteralValueRule : public cmajor::parsing::Rule
{
public:
    FloatingLiteralValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("double"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<double>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FloatingLiteralValueRule>(this, &FloatingLiteralValueRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(std::string(matchBegin, matchEnd));
        s >> context->value;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        double value;
    };
};

class Literal::IntegerLiteralRule : public cmajor::parsing::Rule
{
public:
    IntegerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<LiteralNode*>(context->value)));
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
        cmajor::parsing::NonterminalParser* integerLiteralValueNonterminalParser = GetNonterminal(ToUtf32("IntegerLiteralValue"));
        integerLiteralValueNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IntegerLiteralRule>(this, &IntegerLiteralRule::PostIntegerLiteralValue));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = CreateIntegerLiteralNode(context->s, context->fromIntegerLiteralValue, true);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = CreateIntegerLiteralNode(context->s, context->fromIntegerLiteralValue, false);
    }
    void PostIntegerLiteralValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIntegerLiteralValue_value = std::move(stack.top());
            context->fromIntegerLiteralValue = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromIntegerLiteralValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), s(), fromIntegerLiteralValue() {}
        LiteralNode* value;
        Span s;
        uint64_t fromIntegerLiteralValue;
    };
};

class Literal::IntegerLiteralValueRule : public cmajor::parsing::Rule
{
public:
    IntegerLiteralValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IntegerLiteralValueRule>(this, &IntegerLiteralValueRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IntegerLiteralValueRule>(this, &IntegerLiteralValueRule::A1Action));
        cmajor::parsing::NonterminalParser* hexIntegerLiteralNonterminalParser = GetNonterminal(ToUtf32("HexIntegerLiteral"));
        hexIntegerLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IntegerLiteralValueRule>(this, &IntegerLiteralValueRule::PostHexIntegerLiteral));
        cmajor::parsing::NonterminalParser* decIntegerLiteralNonterminalParser = GetNonterminal(ToUtf32("DecIntegerLiteral"));
        decIntegerLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IntegerLiteralValueRule>(this, &IntegerLiteralValueRule::PostDecIntegerLiteral));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromHexIntegerLiteral;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDecIntegerLiteral;
    }
    void PostHexIntegerLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromHexIntegerLiteral_value = std::move(stack.top());
            context->fromHexIntegerLiteral = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromHexIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostDecIntegerLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDecIntegerLiteral_value = std::move(stack.top());
            context->fromDecIntegerLiteral = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromDecIntegerLiteral_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromHexIntegerLiteral(), fromDecIntegerLiteral() {}
        uint64_t value;
        uint64_t fromHexIntegerLiteral;
        uint64_t fromDecIntegerLiteral;
    };
};

class Literal::HexIntegerLiteralRule : public cmajor::parsing::Rule
{
public:
    HexIntegerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<HexIntegerLiteralRule>(this, &HexIntegerLiteralRule::A0Action));
        cmajor::parsing::NonterminalParser* hexDigitSequenceNonterminalParser = GetNonterminal(ToUtf32("HexDigitSequence"));
        hexDigitSequenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<HexIntegerLiteralRule>(this, &HexIntegerLiteralRule::PostHexDigitSequence));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromHexDigitSequence;
    }
    void PostHexDigitSequence(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromHexDigitSequence_value = std::move(stack.top());
            context->fromHexDigitSequence = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromHexDigitSequence_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromHexDigitSequence() {}
        uint64_t value;
        uint64_t fromHexDigitSequence;
    };
};

class Literal::DecIntegerLiteralRule : public cmajor::parsing::Rule
{
public:
    DecIntegerLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DecIntegerLiteralRule>(this, &DecIntegerLiteralRule::A0Action));
        cmajor::parsing::NonterminalParser* decDigitSequenceNonterminalParser = GetNonterminal(ToUtf32("DecDigitSequence"));
        decDigitSequenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DecIntegerLiteralRule>(this, &DecIntegerLiteralRule::PostDecDigitSequence));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDecDigitSequence;
    }
    void PostDecDigitSequence(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDecDigitSequence_value = std::move(stack.top());
            context->fromDecDigitSequence = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromDecDigitSequence_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromDecDigitSequence() {}
        uint64_t value;
        uint64_t fromDecDigitSequence;
    };
};

class Literal::CharLiteralRule : public cmajor::parsing::Rule
{
public:
    CharLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("char32_t"), ToUtf32("litValue")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<LiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharLiteralRule>(this, &CharLiteralRule::A8Action));
        cmajor::parsing::NonterminalParser* ecNonterminalParser = GetNonterminal(ToUtf32("ec"));
        ecNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharLiteralRule>(this, &CharLiteralRule::Postec));
        cmajor::parsing::NonterminalParser* ewNonterminalParser = GetNonterminal(ToUtf32("ew"));
        ewNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharLiteralRule>(this, &CharLiteralRule::Postew));
        cmajor::parsing::NonterminalParser* euNonterminalParser = GetNonterminal(ToUtf32("eu"));
        euNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharLiteralRule>(this, &CharLiteralRule::Posteu));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharLiteralNode(span, char(context->litValue));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::u32string s = std::u32string(matchBegin, matchEnd);
        if (s.empty()) pass = false;
        else context->litValue = s[0];
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->litValue = context->fromec;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new WCharLiteralNode(span, char16_t(context->litValue));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::u32string s = std::u32string(matchBegin, matchEnd);
        if (s.empty()) pass = false;
        else context->litValue = s[0];
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->litValue = context->fromew;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UCharLiteralNode(span, context->litValue);
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::u32string s = std::u32string(matchBegin, matchEnd);
        if (s.empty()) pass = false;
        else context->litValue = s[0];
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->litValue = context->fromeu;
    }
    void Postec(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromec_value = std::move(stack.top());
            context->fromec = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromec_value.get());
            stack.pop();
        }
    }
    void Postew(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromew_value = std::move(stack.top());
            context->fromew = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromew_value.get());
            stack.pop();
        }
    }
    void Posteu(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromeu_value = std::move(stack.top());
            context->fromeu = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromeu_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), litValue(), fromec(), fromew(), fromeu() {}
        LiteralNode* value;
        char32_t litValue;
        char32_t fromec;
        char32_t fromew;
        char32_t fromeu;
    };
};

class Literal::StringLiteralRule : public cmajor::parsing::Rule
{
public:
    StringLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("s")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<LiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A10Action));
        cmajor::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A11Action));
        cmajor::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A12Action));
        cmajor::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A13Action));
        cmajor::parsing::ActionParser* a14ActionParser = GetAction(ToUtf32("A14"));
        a14ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringLiteralRule>(this, &StringLiteralRule::A14Action));
        cmajor::parsing::NonterminalParser* ecNonterminalParser = GetNonterminal(ToUtf32("ec"));
        ecNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringLiteralRule>(this, &StringLiteralRule::Postec));
        cmajor::parsing::NonterminalParser* ewNonterminalParser = GetNonterminal(ToUtf32("ew"));
        ewNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringLiteralRule>(this, &StringLiteralRule::Postew));
        cmajor::parsing::NonterminalParser* euNonterminalParser = GetNonterminal(ToUtf32("eu"));
        euNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringLiteralRule>(this, &StringLiteralRule::Posteu));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StringLiteralNode(span, ToUtf8(context->s));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = std::u32string(matchBegin, matchEnd);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new WStringLiteralNode(span, ToUtf16(context->s));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = std::u32string(matchBegin, matchEnd);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UStringLiteralNode(span, context->s);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s = std::u32string(matchBegin, matchEnd);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StringLiteralNode(span, ToUtf8(context->s));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(std::u32string(matchBegin, matchEnd));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(1, context->fromec);
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new WStringLiteralNode(span, ToUtf16(context->s));
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(std::u32string(matchBegin, matchEnd));
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(1, context->fromew);
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UStringLiteralNode(span, context->s);
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(std::u32string(matchBegin, matchEnd));
    }
    void A14Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(1, context->fromeu);
    }
    void Postec(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromec_value = std::move(stack.top());
            context->fromec = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromec_value.get());
            stack.pop();
        }
    }
    void Postew(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromew_value = std::move(stack.top());
            context->fromew = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromew_value.get());
            stack.pop();
        }
    }
    void Posteu(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromeu_value = std::move(stack.top());
            context->fromeu = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromeu_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), s(), fromec(), fromew(), fromeu() {}
        LiteralNode* value;
        std::u32string s;
        char32_t fromec;
        char32_t fromew;
        char32_t fromeu;
    };
};

class Literal::NullLiteralRule : public cmajor::parsing::Rule
{
public:
    NullLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("LiteralNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<LiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NullLiteralRule>(this, &NullLiteralRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NullLiteralNode(span);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        LiteralNode* value;
    };
};

class Literal::ArrayLiteralRule : public cmajor::parsing::Rule
{
public:
    ArrayLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ArrayLiteralNode*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ArrayLiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArrayLiteralRule>(this, &ArrayLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArrayLiteralRule>(this, &ArrayLiteralRule::A1Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ArrayLiteralRule>(this, &ArrayLiteralRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArrayLiteralRule>(this, &ArrayLiteralRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ArrayLiteralNode(span);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddValue(context->fromExpression);
    }
    void PreExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromExpression() {}
        ParsingContext* ctx;
        ArrayLiteralNode* value;
        Node* fromExpression;
    };
};

class Literal::StructuredLiteralRule : public cmajor::parsing::Rule
{
public:
    StructuredLiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("StructuredLiteralNode*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<StructuredLiteralNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StructuredLiteralRule>(this, &StructuredLiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StructuredLiteralRule>(this, &StructuredLiteralRule::A1Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<StructuredLiteralRule>(this, &StructuredLiteralRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StructuredLiteralRule>(this, &StructuredLiteralRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StructuredLiteralNode(span);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddMember(context->fromExpression);
    }
    void PreExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromExpression() {}
        ParsingContext* ctx;
        StructuredLiteralNode* value;
        Node* fromExpression;
    };
};

class Literal::CharEscapeRule : public cmajor::parsing::Rule
{
public:
    CharEscapeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A10Action));
        cmajor::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A11Action));
        cmajor::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharEscapeRule>(this, &CharEscapeRule::A12Action));
        cmajor::parsing::NonterminalParser* hexDigitSequenceNonterminalParser = GetNonterminal(ToUtf32("HexDigitSequence"));
        hexDigitSequenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharEscapeRule>(this, &CharEscapeRule::PostHexDigitSequence));
        cmajor::parsing::NonterminalParser* decDigitSequenceNonterminalParser = GetNonterminal(ToUtf32("DecDigitSequence"));
        decDigitSequenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharEscapeRule>(this, &CharEscapeRule::PostDecDigitSequence));
        cmajor::parsing::NonterminalParser* octalDigitSequenceNonterminalParser = GetNonterminal(ToUtf32("OctalDigitSequence"));
        octalDigitSequenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharEscapeRule>(this, &CharEscapeRule::PostOctalDigitSequence));
        cmajor::parsing::NonterminalParser* hexDigit4NonterminalParser = GetNonterminal(ToUtf32("HexDigit4"));
        hexDigit4NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharEscapeRule>(this, &CharEscapeRule::PostHexDigit4));
        cmajor::parsing::NonterminalParser* hexDigit8NonterminalParser = GetNonterminal(ToUtf32("HexDigit8"));
        hexDigit8NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharEscapeRule>(this, &CharEscapeRule::PostHexDigit8));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromHexDigitSequence);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromDecDigitSequence);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromOctalDigitSequence);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromHexDigit4);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromHexDigit8);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t('\a');
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t('\b');
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t('\f');
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t('\n');
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t('\r');
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t('\t');
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t('\v');
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = *matchBegin;
    }
    void PostHexDigitSequence(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromHexDigitSequence_value = std::move(stack.top());
            context->fromHexDigitSequence = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromHexDigitSequence_value.get());
            stack.pop();
        }
    }
    void PostDecDigitSequence(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDecDigitSequence_value = std::move(stack.top());
            context->fromDecDigitSequence = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromDecDigitSequence_value.get());
            stack.pop();
        }
    }
    void PostOctalDigitSequence(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOctalDigitSequence_value = std::move(stack.top());
            context->fromOctalDigitSequence = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromOctalDigitSequence_value.get());
            stack.pop();
        }
    }
    void PostHexDigit4(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromHexDigit4_value = std::move(stack.top());
            context->fromHexDigit4 = *static_cast<cmajor::parsing::ValueObject<uint16_t>*>(fromHexDigit4_value.get());
            stack.pop();
        }
    }
    void PostHexDigit8(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromHexDigit8_value = std::move(stack.top());
            context->fromHexDigit8 = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(fromHexDigit8_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromHexDigitSequence(), fromDecDigitSequence(), fromOctalDigitSequence(), fromHexDigit4(), fromHexDigit8() {}
        char32_t value;
        uint64_t fromHexDigitSequence;
        uint64_t fromDecDigitSequence;
        uint64_t fromOctalDigitSequence;
        uint16_t fromHexDigit4;
        uint32_t fromHexDigit8;
    };
};

class Literal::DecDigitSequenceRule : public cmajor::parsing::Rule
{
public:
    DecDigitSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DecDigitSequenceRule>(this, &DecDigitSequenceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> context->value;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

class Literal::HexDigitSequenceRule : public cmajor::parsing::Rule
{
public:
    HexDigitSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<HexDigitSequenceRule>(this, &HexDigitSequenceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

class Literal::HexDigit4Rule : public cmajor::parsing::Rule
{
public:
    HexDigit4Rule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint16_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint16_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<HexDigit4Rule>(this, &HexDigit4Rule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        uint16_t value;
    };
};

class Literal::HexDigit8Rule : public cmajor::parsing::Rule
{
public:
    HexDigit8Rule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<HexDigit8Rule>(this, &HexDigit8Rule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> context->value;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        uint32_t value;
    };
};

class Literal::OctalDigitSequenceRule : public cmajor::parsing::Rule
{
public:
    OctalDigitSequenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint64_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OctalDigitSequenceRule>(this, &OctalDigitSequenceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = 8 * context->value + *matchBegin - '0';
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        uint64_t value;
    };
};

void Literal::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Expression"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Expression::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Literal::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRule(new LiteralRule(ToUtf32("Literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("BooleanLiteral"), ToUtf32("BooleanLiteral"), 0)),
                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("FloatingLiteral"), ToUtf32("FloatingLiteral"), 0))),
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("IntegerLiteral"), ToUtf32("IntegerLiteral"), 0))),
                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("CharLiteral"), ToUtf32("CharLiteral"), 0))),
                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("StringLiteral"), ToUtf32("StringLiteral"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("NullLiteral"), ToUtf32("NullLiteral"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ArrayLiteral"), ToUtf32("ArrayLiteral"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                new cmajor::parsing::NonterminalParser(ToUtf32("StructuredLiteral"), ToUtf32("StructuredLiteral"), 1)))));
    AddRule(new BooleanLiteralRule(ToUtf32("BooleanLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::KeywordParser(ToUtf32("true"))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::KeywordParser(ToUtf32("false"))))));
    AddRule(new FloatingLiteralRule(ToUtf32("FloatingLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("FloatingLiteralValue"), ToUtf32("FloatingLiteralValue"), 0)),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::CharSetParser(ToUtf32("fF"))),
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::EmptyParser())))))));
    AddRule(new FloatingLiteralValueRule(ToUtf32("FloatingLiteralValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("FractionalFloatingLiteral"), ToUtf32("FractionalFloatingLiteral"), 0),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ExponentFloatingLiteral"), ToUtf32("ExponentFloatingLiteral"), 0))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("FractionalFloatingLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("s0"), ToUtf32("DecDigitSequence"), 0)),
                            new cmajor::parsing::CharParser('.')),
                        new cmajor::parsing::NonterminalParser(ToUtf32("m"), ToUtf32("DecDigitSequence"), 0)),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0)))),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("s1"), ToUtf32("DecDigitSequence"), 0),
                    new cmajor::parsing::CharParser('.'))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("ExponentFloatingLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("DecDigitSequence"), ToUtf32("DecDigitSequence"), 0),
                new cmajor::parsing::NonterminalParser(ToUtf32("ExponentPart"), ToUtf32("ExponentPart"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("ExponentPart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharSetParser(ToUtf32("eE")),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Sign"), ToUtf32("Sign"), 0))),
                new cmajor::parsing::NonterminalParser(ToUtf32("DecDigitSequence"), ToUtf32("DecDigitSequence"), 0)))));
    AddRule(new IntegerLiteralRule(ToUtf32("IntegerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("IntegerLiteralValue"), ToUtf32("IntegerLiteralValue"), 0)),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::CharSetParser(ToUtf32("uU"))),
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::EmptyParser())))))));
    AddRule(new IntegerLiteralValueRule(ToUtf32("IntegerLiteralValue"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("HexIntegerLiteral"), ToUtf32("HexIntegerLiteral"), 0)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("DecIntegerLiteral"), ToUtf32("DecIntegerLiteral"), 0))))));
    AddRule(new HexIntegerLiteralRule(ToUtf32("HexIntegerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::StringParser(ToUtf32("0x")),
                        new cmajor::parsing::StringParser(ToUtf32("0X")))),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("HexDigitSequence"), ToUtf32("HexDigitSequence"), 0))))));
    AddRule(new DecIntegerLiteralRule(ToUtf32("DecIntegerLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("DecDigitSequence"), ToUtf32("DecDigitSequence"), 0))));
    AddRule(new CharLiteralRule(ToUtf32("CharLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::TokenParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('\''),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::PositiveParser(
                                                    new cmajor::parsing::CharSetParser(ToUtf32("\'\\\r\n"), true)))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("ec"), ToUtf32("CharEscape"), 0))))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('\''))))),
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('w'),
                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                        new cmajor::parsing::TokenParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('\''),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                new cmajor::parsing::GroupingParser(
                                                    new cmajor::parsing::PositiveParser(
                                                        new cmajor::parsing::CharSetParser(ToUtf32("\'\\\r\n"), true)))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("ew"), ToUtf32("CharEscape"), 0))))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser('\''))))))),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('u'),
                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                    new cmajor::parsing::TokenParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('\''),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::PositiveParser(
                                                    new cmajor::parsing::CharSetParser(ToUtf32("\'\\\r\n"), true)))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("eu"), ToUtf32("CharEscape"), 0))))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('\'')))))))));
    AddRule(new StringLiteralRule(ToUtf32("StringLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('@'),
                                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                    new cmajor::parsing::TokenParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::CharParser('\"'),
                                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::KleeneStarParser(
                                                            new cmajor::parsing::CharSetParser(ToUtf32("\""), true))))),
                                            new cmajor::parsing::CharParser('\"'))))),
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('w'),
                                    new cmajor::parsing::CharParser('@')),
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::TokenParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::CharParser('\"'),
                                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::KleeneStarParser(
                                                            new cmajor::parsing::CharSetParser(ToUtf32("\""), true))))),
                                            new cmajor::parsing::CharParser('\"')))))),
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('u'),
                                new cmajor::parsing::CharParser('@')),
                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                new cmajor::parsing::TokenParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::CharParser('\"'),
                                            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                new cmajor::parsing::GroupingParser(
                                                    new cmajor::parsing::KleeneStarParser(
                                                        new cmajor::parsing::CharSetParser(ToUtf32("\""), true))))),
                                        new cmajor::parsing::CharParser('\"')))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A6"),
                        new cmajor::parsing::TokenParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('\"'),
                                    new cmajor::parsing::KleeneStarParser(
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::PositiveParser(
                                                            new cmajor::parsing::CharSetParser(ToUtf32("\"\\\r\n"), true)))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("ec"), ToUtf32("CharEscape"), 0)))))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser('\"')))))),
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('w'),
                    new cmajor::parsing::ActionParser(ToUtf32("A9"),
                        new cmajor::parsing::TokenParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('\"'),
                                    new cmajor::parsing::KleeneStarParser(
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A10"),
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::PositiveParser(
                                                            new cmajor::parsing::CharSetParser(ToUtf32("\"\\\r\n"), true)))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A11"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("ew"), ToUtf32("CharEscape"), 0)))))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser('\"'))))))),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('u'),
                new cmajor::parsing::ActionParser(ToUtf32("A12"),
                    new cmajor::parsing::TokenParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('\"'),
                                new cmajor::parsing::KleeneStarParser(
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::ActionParser(ToUtf32("A13"),
                                                new cmajor::parsing::GroupingParser(
                                                    new cmajor::parsing::PositiveParser(
                                                        new cmajor::parsing::CharSetParser(ToUtf32("\"\\\r\n"), true)))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A14"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("eu"), ToUtf32("CharEscape"), 0)))))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('\"')))))))));
    AddRule(new NullLiteralRule(ToUtf32("NullLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("null")))));
    AddRule(new ArrayLiteralRule(ToUtf32("ArrayLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser('[')),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ListParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1)),
                            new cmajor::parsing::CharParser(','))))),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::CharParser(']')))));
    AddRule(new StructuredLiteralRule(ToUtf32("StructuredLiteral"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser('{')),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ListParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1)),
                            new cmajor::parsing::CharParser(','))))),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::CharParser('}')))));
    AddRule(new CharEscapeRule(ToUtf32("CharEscape"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('\\'),
                new cmajor::parsing::GroupingParser(
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
                                                                    new cmajor::parsing::SequenceParser(
                                                                        new cmajor::parsing::CharSetParser(ToUtf32("xX")),
                                                                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("HexDigitSequence"), ToUtf32("HexDigitSequence"), 0))),
                                                                    new cmajor::parsing::SequenceParser(
                                                                        new cmajor::parsing::CharSetParser(ToUtf32("dD")),
                                                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("DecDigitSequence"), ToUtf32("DecDigitSequence"), 0)))),
                                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("OctalDigitSequence"), ToUtf32("OctalDigitSequence"), 0))),
                                                            new cmajor::parsing::SequenceParser(
                                                                new cmajor::parsing::CharParser('u'),
                                                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("HexDigit4"), ToUtf32("HexDigit4"), 0)))),
                                                        new cmajor::parsing::SequenceParser(
                                                            new cmajor::parsing::CharParser('U'),
                                                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                                new cmajor::parsing::NonterminalParser(ToUtf32("HexDigit8"), ToUtf32("HexDigit8"), 0)))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                        new cmajor::parsing::CharParser('a'))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                                    new cmajor::parsing::CharParser('b'))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                                new cmajor::parsing::CharParser('f'))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                            new cmajor::parsing::CharParser('n'))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A9"),
                                        new cmajor::parsing::CharParser('r'))),
                                new cmajor::parsing::ActionParser(ToUtf32("A10"),
                                    new cmajor::parsing::CharParser('t'))),
                            new cmajor::parsing::ActionParser(ToUtf32("A11"),
                                new cmajor::parsing::CharParser('v'))),
                        new cmajor::parsing::ActionParser(ToUtf32("A12"),
                            new cmajor::parsing::AnyCharParser())))))));
    AddRule(new DecDigitSequenceRule(ToUtf32("DecDigitSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::PositiveParser(
                    new cmajor::parsing::DigitParser())))));
    AddRule(new HexDigitSequenceRule(ToUtf32("HexDigitSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::PositiveParser(
                    new cmajor::parsing::HexDigitParser())))));
    AddRule(new HexDigit4Rule(ToUtf32("HexDigit4"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::HexDigitParser(),
                            new cmajor::parsing::HexDigitParser()),
                        new cmajor::parsing::HexDigitParser()),
                    new cmajor::parsing::HexDigitParser())))));
    AddRule(new HexDigit8Rule(ToUtf32("HexDigit8"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::HexDigitParser(),
                                            new cmajor::parsing::HexDigitParser()),
                                        new cmajor::parsing::HexDigitParser()),
                                    new cmajor::parsing::HexDigitParser()),
                                new cmajor::parsing::HexDigitParser()),
                            new cmajor::parsing::HexDigitParser()),
                        new cmajor::parsing::HexDigitParser()),
                    new cmajor::parsing::HexDigitParser())))));
    AddRule(new OctalDigitSequenceRule(ToUtf32("OctalDigitSequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharSetParser(ToUtf32("0-7")))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Sign"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::CharParser('+'),
            new cmajor::parsing::CharParser('-'))));
}

} } // namespace cmajor.parser
