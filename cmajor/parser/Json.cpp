#include "Json.hpp"
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

Json* Json::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Json* Json::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Json* grammar(new Json(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Json::Json(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Json"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

JsonValue* Json::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    JsonValue* result = *static_cast<cmajor::parsing::ValueObject<JsonValue*>*>(value.get());
    stack.pop();
    return result;
}

class Json::ValueRule : public cmajor::parsing::Rule
{
public:
    ValueRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("JsonValue*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<JsonValue*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ValueRule>(this, &ValueRule::A6Action));
        cmajor::parsing::NonterminalParser* stringNonterminalParser = GetNonterminal(ToUtf32("String"));
        stringNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostString));
        cmajor::parsing::NonterminalParser* numberNonterminalParser = GetNonterminal(ToUtf32("Number"));
        numberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostNumber));
        cmajor::parsing::NonterminalParser* objectNonterminalParser = GetNonterminal(ToUtf32("Object"));
        objectNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostObject));
        cmajor::parsing::NonterminalParser* arrayNonterminalParser = GetNonterminal(ToUtf32("Array"));
        arrayNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ValueRule>(this, &ValueRule::PostArray));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromString;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNumber;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromObject;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromArray;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new JsonBool(true);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new JsonBool(false);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new JsonNull;
    }
    void PostString(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromString_value = std::move(stack.top());
            context->fromString = *static_cast<cmajor::parsing::ValueObject<JsonString*>*>(fromString_value.get());
            stack.pop();
        }
    }
    void PostNumber(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNumber_value = std::move(stack.top());
            context->fromNumber = *static_cast<cmajor::parsing::ValueObject<JsonNumber*>*>(fromNumber_value.get());
            stack.pop();
        }
    }
    void PostObject(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromObject_value = std::move(stack.top());
            context->fromObject = *static_cast<cmajor::parsing::ValueObject<JsonObject*>*>(fromObject_value.get());
            stack.pop();
        }
    }
    void PostArray(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromArray_value = std::move(stack.top());
            context->fromArray = *static_cast<cmajor::parsing::ValueObject<JsonArray*>*>(fromArray_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromString(), fromNumber(), fromObject(), fromArray() {}
        JsonValue* value;
        JsonString* fromString;
        JsonNumber* fromNumber;
        JsonObject* fromObject;
        JsonArray* fromArray;
    };
};

class Json::StringRule : public cmajor::parsing::Rule
{
public:
    StringRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("JsonString*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<JsonString*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringRule>(this, &StringRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringRule>(this, &StringRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringRule>(this, &StringRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringRule>(this, &StringRule::A3Action));
        cmajor::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringRule>(this, &StringRule::Postescape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new JsonString;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->Append(*matchBegin);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        uint32_t hexValue;
        std::stringstream s;
        s.str(ToUtf8(std::u32string(matchBegin, matchEnd)));
        s >> std::hex >> hexValue;
        context->value->Append(char32_t(hexValue));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->Append(context->fromescape);
    }
    void Postescape(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromescape_value = std::move(stack.top());
            context->fromescape = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromescape_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromescape() {}
        JsonString* value;
        char32_t fromescape;
    };
};

class Json::NumberRule : public cmajor::parsing::Rule
{
public:
    NumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("JsonNumber*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<JsonNumber*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A0Action));
        cmajor::parsing::NonterminalParser* nNonterminalParser = GetNonterminal(ToUtf32("n"));
        nNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NumberRule>(this, &NumberRule::Postn));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new JsonNumber(context->fromn);
    }
    void Postn(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromn_value = std::move(stack.top());
            context->fromn = *static_cast<cmajor::parsing::ValueObject<double>*>(fromn_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromn() {}
        JsonNumber* value;
        double fromn;
    };
};

class Json::ObjectRule : public cmajor::parsing::Rule
{
public:
    ObjectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("JsonObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<JsonString>"), ToUtf32("js")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<JsonValue>"), ToUtf32("jv")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<JsonObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ObjectRule>(this, &ObjectRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ObjectRule>(this, &ObjectRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ObjectRule>(this, &ObjectRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ObjectRule>(this, &ObjectRule::A3Action));
        cmajor::parsing::NonterminalParser* sNonterminalParser = GetNonterminal(ToUtf32("s"));
        sNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ObjectRule>(this, &ObjectRule::Posts));
        cmajor::parsing::NonterminalParser* vNonterminalParser = GetNonterminal(ToUtf32("v"));
        vNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ObjectRule>(this, &ObjectRule::Postv));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new JsonObject;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddField(context->js->Value(), std::move(context->jv));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->js.reset(context->froms);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->jv.reset(context->fromv);
    }
    void Posts(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> froms_value = std::move(stack.top());
            context->froms = *static_cast<cmajor::parsing::ValueObject<JsonString*>*>(froms_value.get());
            stack.pop();
        }
    }
    void Postv(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromv_value = std::move(stack.top());
            context->fromv = *static_cast<cmajor::parsing::ValueObject<JsonValue*>*>(fromv_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), js(), jv(), froms(), fromv() {}
        JsonObject* value;
        std::unique_ptr<JsonString> js;
        std::unique_ptr<JsonValue> jv;
        JsonString* froms;
        JsonValue* fromv;
    };
};

class Json::ArrayRule : public cmajor::parsing::Rule
{
public:
    ArrayRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("JsonArray*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<JsonValue>"), ToUtf32("item")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<JsonArray*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArrayRule>(this, &ArrayRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArrayRule>(this, &ArrayRule::A1Action));
        cmajor::parsing::NonterminalParser* iNonterminalParser = GetNonterminal(ToUtf32("i"));
        iNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArrayRule>(this, &ArrayRule::Posti));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new JsonArray;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->item.reset(context->fromi);
        context->value->AddItem(std::move(context->item));
    }
    void Posti(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromi_value = std::move(stack.top());
            context->fromi = *static_cast<cmajor::parsing::ValueObject<JsonValue*>*>(fromi_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), item(), fromi() {}
        JsonArray* value;
        std::unique_ptr<JsonValue> item;
        JsonValue* fromi;
    };
};

void Json::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Json::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces"), this, ToUtf32("cmajor.parsing.stdlib.spaces")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("escape"), this, ToUtf32("cmajor.parsing.stdlib.escape")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("num"), this, ToUtf32("cmajor.parsing.stdlib.num")));
    AddRule(new ValueRule(ToUtf32("Value"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("String"), ToUtf32("String"), 0)),
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Number"), ToUtf32("Number"), 0))),
                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Object"), ToUtf32("Object"), 0))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Array"), ToUtf32("Array"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                        new cmajor::parsing::KeywordParser(ToUtf32("true")))),
                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                    new cmajor::parsing::KeywordParser(ToUtf32("false")))),
            new cmajor::parsing::ActionParser(ToUtf32("A6"),
                new cmajor::parsing::KeywordParser(ToUtf32("null"))))));
    AddRule(new StringRule(ToUtf32("String"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::CharParser('\"')),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                        new cmajor::parsing::CharSetParser(ToUtf32("\"\\"), true)),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::StringParser(ToUtf32("\\u")),
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::GroupingParser(
                                                    new cmajor::parsing::SequenceParser(
                                                        new cmajor::parsing::SequenceParser(
                                                            new cmajor::parsing::SequenceParser(
                                                                new cmajor::parsing::HexDigitParser(),
                                                                new cmajor::parsing::HexDigitParser()),
                                                            new cmajor::parsing::HexDigitParser()),
                                                        new cmajor::parsing::HexDigitParser())))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0)))))),
                new cmajor::parsing::CharParser('\"')))));
    AddRule(new NumberRule(ToUtf32("Number"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("n"), ToUtf32("num"), 0))));
    AddRule(new ObjectRule(ToUtf32("Object"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser('{')),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ListParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("s"), ToUtf32("String"), 0)),
                                            new cmajor::parsing::CharParser(':')),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("v"), ToUtf32("Value"), 0))))),
                            new cmajor::parsing::CharParser(','))))),
            new cmajor::parsing::CharParser('}'))));
    AddRule(new ArrayRule(ToUtf32("Array"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser('[')),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ListParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("i"), ToUtf32("Value"), 0)),
                            new cmajor::parsing::CharParser(','))))),
            new cmajor::parsing::CharParser(']'))));
    SetSkipRuleName(ToUtf32("spaces"));
}

} } // namespace cmajor.parser
