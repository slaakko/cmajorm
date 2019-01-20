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
#include <sstream>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace parsing {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

stdlib* stdlib::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

stdlib* stdlib::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    stdlib* grammar(new stdlib(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

stdlib::stdlib(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("stdlib"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parsing")), parsingDomain_)
{
    SetOwner(0);
}

class stdlib::intRule : public cmajor::parsing::Rule
{
public:
    intRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("int32_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<int32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<intRule>(this, &intRule::A0Action));
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
        int32_t value;
    };
};

class stdlib::uintRule : public cmajor::parsing::Rule
{
public:
    uintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<uintRule>(this, &uintRule::A0Action));
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
        uint32_t value;
    };
};

class stdlib::longRule : public cmajor::parsing::Rule
{
public:
    longRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("int64_t"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<int64_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<longRule>(this, &longRule::A0Action));
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
        int64_t value;
    };
};

class stdlib::ulongRule : public cmajor::parsing::Rule
{
public:
    ulongRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ulongRule>(this, &ulongRule::A0Action));
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

class stdlib::hexuintRule : public cmajor::parsing::Rule
{
public:
    hexuintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<hexuintRule>(this, &hexuintRule::A0Action));
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

class stdlib::hexRule : public cmajor::parsing::Rule
{
public:
    hexRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<hexRule>(this, &hexRule::A0Action));
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

class stdlib::hex_literalRule : public cmajor::parsing::Rule
{
public:
    hex_literalRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<hex_literalRule>(this, &hex_literalRule::A0Action));
        cmajor::parsing::NonterminalParser* hexNonterminalParser = GetNonterminal(ToUtf32("hex"));
        hexNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<hex_literalRule>(this, &hex_literalRule::Posthex));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromhex;
    }
    void Posthex(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromhex_value = std::move(stack.top());
            context->fromhex = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromhex_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromhex() {}
        uint64_t value;
        uint64_t fromhex;
    };
};

class stdlib::realRule : public cmajor::parsing::Rule
{
public:
    realRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<realRule>(this, &realRule::A0Action));
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
        double value;
    };
};

class stdlib::urealRule : public cmajor::parsing::Rule
{
public:
    urealRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<urealRule>(this, &urealRule::A0Action));
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
        double value;
    };
};

class stdlib::numRule : public cmajor::parsing::Rule
{
public:
    numRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<numRule>(this, &numRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<numRule>(this, &numRule::A1Action));
        cmajor::parsing::NonterminalParser* rNonterminalParser = GetNonterminal(ToUtf32("r"));
        rNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<numRule>(this, &numRule::Postr));
        cmajor::parsing::NonterminalParser* iNonterminalParser = GetNonterminal(ToUtf32("i"));
        iNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<numRule>(this, &numRule::Posti));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromr;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromi;
    }
    void Postr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromr_value = std::move(stack.top());
            context->fromr = *static_cast<cmajor::parsing::ValueObject<double>*>(fromr_value.get());
            stack.pop();
        }
    }
    void Posti(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromi_value = std::move(stack.top());
            context->fromi = *static_cast<cmajor::parsing::ValueObject<int32_t>*>(fromi_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromr(), fromi() {}
        double value;
        double fromr;
        int32_t fromi;
    };
};

class stdlib::boolRule : public cmajor::parsing::Rule
{
public:
    boolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("bool"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<bool>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<boolRule>(this, &boolRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<boolRule>(this, &boolRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = true;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = false;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        bool value;
    };
};

class stdlib::identifierRule : public cmajor::parsing::Rule
{
public:
    identifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<identifierRule>(this, &identifierRule::A0Action));
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

class stdlib::qualified_idRule : public cmajor::parsing::Rule
{
public:
    qualified_idRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<qualified_idRule>(this, &qualified_idRule::A0Action));
        cmajor::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<qualified_idRule>(this, &qualified_idRule::Postfirst));
        cmajor::parsing::NonterminalParser* restNonterminalParser = GetNonterminal(ToUtf32("rest"));
        restNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<qualified_idRule>(this, &qualified_idRule::Postrest));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void Postfirst(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postrest(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromrest_value = std::move(stack.top());
            context->fromrest = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromrest_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromfirst(), fromrest() {}
        std::u32string value;
        std::u32string fromfirst;
        std::u32string fromrest;
    };
};

class stdlib::escapeRule : public cmajor::parsing::Rule
{
public:
    escapeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<escapeRule>(this, &escapeRule::A2Action));
        cmajor::parsing::NonterminalParser* xNonterminalParser = GetNonterminal(ToUtf32("x"));
        xNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<escapeRule>(this, &escapeRule::Postx));
        cmajor::parsing::NonterminalParser* decimalEscapeNonterminalParser = GetNonterminal(ToUtf32("decimalEscape"));
        decimalEscapeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<escapeRule>(this, &escapeRule::PostdecimalEscape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromx);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = char32_t(context->fromdecimalEscape);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        char32_t c = *matchBegin;
        switch (c)
        {
            case 'a': context->value = '\a';
            break;
            case 'b': context->value = '\b';
            break;
            case 'f': context->value = '\f';
            break;
            case 'n': context->value = '\n';
            break;
            case 'r': context->value = '\r';
            break;
            case 't': context->value = '\t';
            break;
            case 'v': context->value = '\v';
            break;
            case '0': context->value = '\0';
            break;
            default: context->value = c;
            break;
        }
    }
    void Postx(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromx_value = std::move(stack.top());
            context->fromx = *static_cast<cmajor::parsing::ValueObject<uint64_t>*>(fromx_value.get());
            stack.pop();
        }
    }
    void PostdecimalEscape(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromdecimalEscape_value = std::move(stack.top());
            context->fromdecimalEscape = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(fromdecimalEscape_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromx(), fromdecimalEscape() {}
        char32_t value;
        uint64_t fromx;
        uint32_t fromdecimalEscape;
    };
};

class stdlib::charRule : public cmajor::parsing::Rule
{
public:
    charRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<charRule>(this, &charRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<charRule>(this, &charRule::A1Action));
        cmajor::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<charRule>(this, &charRule::Postescape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = *matchBegin;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromescape;
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
        char32_t value;
        char32_t fromescape;
    };
};

class stdlib::stringRule : public cmajor::parsing::Rule
{
public:
    stringRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<stringRule>(this, &stringRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<stringRule>(this, &stringRule::A1Action));
        cmajor::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<stringRule>(this, &stringRule::Postescape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromescape);
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
        std::u32string value;
        char32_t fromescape;
    };
};

void stdlib::GetReferencedGrammars()
{
}

void stdlib::CreateRules()
{
    AddRule(new cmajor::parsing::Rule(ToUtf32("spaces"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::SpaceParser())));
    AddRule(new cmajor::parsing::Rule(ToUtf32("newline"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::StringParser(ToUtf32("\r\n")),
                new cmajor::parsing::StringParser(ToUtf32("\n"))),
            new cmajor::parsing::StringParser(ToUtf32("\r")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("line_comment"), ToUtf32("line_comment"), 0),
            new cmajor::parsing::NonterminalParser(ToUtf32("block_comment"), ToUtf32("block_comment"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("line_comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::StringParser(ToUtf32("//")),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::CharSetParser(ToUtf32("\r\n"), true))),
            new cmajor::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("block_comment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::StringParser(ToUtf32("/*")),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("string"), ToUtf32("string"), 0),
                                new cmajor::parsing::NonterminalParser(ToUtf32("char"), ToUtf32("char"), 0)),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::DifferenceParser(
                                    new cmajor::parsing::AnyCharParser(),
                                    new cmajor::parsing::StringParser(ToUtf32("*/")))))))),
            new cmajor::parsing::StringParser(ToUtf32("*/")))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("spaces_and_comments"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SpaceParser(),
                    new cmajor::parsing::NonterminalParser(ToUtf32("comment"), ToUtf32("comment"), 0))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("digit_sequence"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::PositiveParser(
                new cmajor::parsing::DigitParser()))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("sign"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::CharParser('+'),
            new cmajor::parsing::CharParser('-'))));
    AddRule(new intRule(ToUtf32("int"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))))));
    AddRule(new uintRule(ToUtf32("uint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))));
    AddRule(new longRule(ToUtf32("long"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0)),
                    new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))))));
    AddRule(new ulongRule(ToUtf32("ulong"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0))));
    AddRule(new hexuintRule(ToUtf32("hexuint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::PositiveParser(
                    new cmajor::parsing::HexDigitParser())))));
    AddRule(new hexRule(ToUtf32("hex"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::PositiveParser(
                    new cmajor::parsing::HexDigitParser())))));
    AddRule(new hex_literalRule(ToUtf32("hex_literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::StringParser(ToUtf32("0x")),
                            new cmajor::parsing::StringParser(ToUtf32("0X")))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("hex"), ToUtf32("hex"), 0)))))));
    AddRule(new realRule(ToUtf32("real"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0)),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("fractional_real"), ToUtf32("fractional_real"), 0),
                            new cmajor::parsing::NonterminalParser(ToUtf32("exponent_real"), ToUtf32("exponent_real"), 0))))))));
    AddRule(new urealRule(ToUtf32("ureal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("fractional_real"), ToUtf32("fractional_real"), 0),
                    new cmajor::parsing::NonterminalParser(ToUtf32("exponent_real"), ToUtf32("exponent_real"), 0))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("fractional_real"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)),
                            new cmajor::parsing::CharParser('.')),
                        new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("exponent_part"), ToUtf32("exponent_part"), 0)))),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0),
                    new cmajor::parsing::CharParser('.'))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("exponent_real"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0),
                new cmajor::parsing::NonterminalParser(ToUtf32("exponent_part"), ToUtf32("exponent_part"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("exponent_part"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharSetParser(ToUtf32("eE")),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("sign"), ToUtf32("sign"), 0))),
                new cmajor::parsing::NonterminalParser(ToUtf32("digit_sequence"), ToUtf32("digit_sequence"), 0)))));
    AddRule(new numRule(ToUtf32("num"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("r"), ToUtf32("real"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("i"), ToUtf32("int"), 0)))));
    AddRule(new boolRule(ToUtf32("bool"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::KeywordParser(ToUtf32("true"))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::KeywordParser(ToUtf32("false"))))));
    AddRule(new identifierRule(ToUtf32("identifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::IdStartParser(),
                            new cmajor::parsing::CharParser('_'))),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::IdContParser()))))));
    AddRule(new qualified_idRule(ToUtf32("qualified_id"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("identifier"), 0),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('.'),
                                new cmajor::parsing::NonterminalParser(ToUtf32("rest"), ToUtf32("identifier"), 0)))))))));
    AddRule(new escapeRule(ToUtf32("escape"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('\\'),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharSetParser(ToUtf32("xX")),
                                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("x"), ToUtf32("hex"), 0))),
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharSetParser(ToUtf32("dD")),
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("decimalEscape"), ToUtf32("uint"), 0)))),
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::CharSetParser(ToUtf32("dDxX"), true))))))));
    AddRule(new charRule(ToUtf32("char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\''),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                new cmajor::parsing::CharSetParser(ToUtf32("\\\r\n"), true)),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0))))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('\''))))));
    AddRule(new stringRule(ToUtf32("string"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('\"'),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::PositiveParser(
                                            new cmajor::parsing::CharSetParser(ToUtf32("\"\\\r\n"), true)))),
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0)))))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('\"'))))));
}

} } // namespace cmajor.parsing
