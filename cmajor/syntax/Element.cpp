#include "Element.hpp"
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
#include <cmajor/codedom/Type.hpp>
#include <cmajor/code/Declarator.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace soulng::util;
using namespace cmajor::unicode;

Element* Element::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Element* Element::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Element* grammar(new Element(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Element::Element(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Element"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back(ToUtf32("alphabetic"));
    keywords0.push_back(ToUtf32("anychar"));
    keywords0.push_back(ToUtf32("basechar"));
    keywords0.push_back(ToUtf32("cased_letter"));
    keywords0.push_back(ToUtf32("close_punctuation"));
    keywords0.push_back(ToUtf32("connector_punctuation"));
    keywords0.push_back(ToUtf32("control"));
    keywords0.push_back(ToUtf32("currency_symbol"));
    keywords0.push_back(ToUtf32("dash_punctuation"));
    keywords0.push_back(ToUtf32("decimal_number"));
    keywords0.push_back(ToUtf32("digit"));
    keywords0.push_back(ToUtf32("empty"));
    keywords0.push_back(ToUtf32("enclosing_mark"));
    keywords0.push_back(ToUtf32("end"));
    keywords0.push_back(ToUtf32("final_punctuation"));
    keywords0.push_back(ToUtf32("format"));
    keywords0.push_back(ToUtf32("grammar"));
    keywords0.push_back(ToUtf32("graphic"));
    keywords0.push_back(ToUtf32("hexdigit"));
    keywords0.push_back(ToUtf32("idcont"));
    keywords0.push_back(ToUtf32("idstart"));
    keywords0.push_back(ToUtf32("initial_punctuation"));
    keywords0.push_back(ToUtf32("keyword"));
    keywords0.push_back(ToUtf32("keyword_list"));
    keywords0.push_back(ToUtf32("letter"));
    keywords0.push_back(ToUtf32("letter_number"));
    keywords0.push_back(ToUtf32("line_separator"));
    keywords0.push_back(ToUtf32("lower_letter"));
    keywords0.push_back(ToUtf32("mark"));
    keywords0.push_back(ToUtf32("math_symbol"));
    keywords0.push_back(ToUtf32("modifier_letter"));
    keywords0.push_back(ToUtf32("modifier_symbol"));
    keywords0.push_back(ToUtf32("nonspacing_mark"));
    keywords0.push_back(ToUtf32("number"));
    keywords0.push_back(ToUtf32("open_punctuation"));
    keywords0.push_back(ToUtf32("other"));
    keywords0.push_back(ToUtf32("other_letter"));
    keywords0.push_back(ToUtf32("other_number"));
    keywords0.push_back(ToUtf32("other_punctuation"));
    keywords0.push_back(ToUtf32("other_symbol"));
    keywords0.push_back(ToUtf32("paragraph_separator"));
    keywords0.push_back(ToUtf32("private_use"));
    keywords0.push_back(ToUtf32("punctuation"));
    keywords0.push_back(ToUtf32("range"));
    keywords0.push_back(ToUtf32("separator"));
    keywords0.push_back(ToUtf32("skip"));
    keywords0.push_back(ToUtf32("space"));
    keywords0.push_back(ToUtf32("space_separator"));
    keywords0.push_back(ToUtf32("spacing_mark"));
    keywords0.push_back(ToUtf32("start"));
    keywords0.push_back(ToUtf32("surrogate"));
    keywords0.push_back(ToUtf32("symbol"));
    keywords0.push_back(ToUtf32("title_letter"));
    keywords0.push_back(ToUtf32("token"));
    keywords0.push_back(ToUtf32("unassigned"));
    keywords0.push_back(ToUtf32("upper_letter"));
    keywords0.push_back(ToUtf32("using"));
    keywords0.push_back(ToUtf32("var"));
}

void Element::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Grammar* grammar)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<cmajor::parsing::Grammar*>(grammar)));
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

class Element::RuleLinkRule : public cmajor::parsing::Rule
{
public:
    RuleLinkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RuleLinkRule>(this, &RuleLinkRule::A1Action));
        cmajor::parsing::NonterminalParser* aliasNameNonterminalParser = GetNonterminal(ToUtf32("aliasName"));
        aliasNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostaliasName));
        cmajor::parsing::NonterminalParser* ruleNameNonterminalParser = GetNonterminal(ToUtf32("ruleName"));
        ruleNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::PostruleName));
        cmajor::parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal(ToUtf32("qualified_id"));
        qualified_idNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RuleLinkRule>(this, &RuleLinkRule::Postqualified_id));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        RuleLink * link(new RuleLink(context->fromaliasName, context->grammar, context->fromruleName));
        link->SetSpan(span);
        context->grammar->AddRuleLink(link);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        RuleLink * link(new RuleLink(context->grammar, context->fromqualified_id));
        link->SetSpan(span);
        context->grammar->AddRuleLink(link);
    }
    void PostaliasName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromaliasName_value = std::move(stack.top());
            context->fromaliasName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromaliasName_value.get());
            stack.pop();
        }
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
    void Postqualified_id(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromqualified_id_value = std::move(stack.top());
            context->fromqualified_id = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromqualified_id_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): grammar(), fromaliasName(), fromruleName(), fromqualified_id() {}
        cmajor::parsing::Grammar* grammar;
        std::u32string fromaliasName;
        std::u32string fromruleName;
        std::u32string fromqualified_id;
    };
};

class Element::SignatureRule : public cmajor::parsing::Rule
{
public:
    SignatureRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreParameterList));
        cmajor::parsing::NonterminalParser* returnTypeNonterminalParser = GetNonterminal(ToUtf32("ReturnType"));
        returnTypeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SignatureRule>(this, &SignatureRule::PreReturnType));
    }
    void PreParameterList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->rule)));
    }
    void PreReturnType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->rule)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): rule() {}
        cmajor::parsing::Rule* rule;
    };
};

class Element::ParameterListRule : public cmajor::parsing::Rule
{
public:
    ParameterListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        cmajor::parsing::NonterminalParser* variableNonterminalParser = GetNonterminal(ToUtf32("Variable"));
        variableNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreVariable));
        cmajor::parsing::NonterminalParser* parameterNonterminalParser = GetNonterminal(ToUtf32("Parameter"));
        parameterNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ParameterListRule>(this, &ParameterListRule::PreParameter));
    }
    void PreVariable(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->rule)));
    }
    void PreParameter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Rule*>(context->rule)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): rule() {}
        cmajor::parsing::Rule* rule;
    };
};

class Element::VariableRule : public cmajor::parsing::Rule
{
public:
    VariableRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<VariableRule>(this, &VariableRule::A0Action));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostTypeId));
        cmajor::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<VariableRule>(this, &VariableRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->AddLocalVariable(AttrOrVariable(context->fromTypeId->ToString(), context->fromDeclarator));
        delete context->fromTypeId;
    }
    void PostTypeId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        cmajor::parsing::Rule* rule;
        cmajor::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
    };
};

class Element::ParameterRule : public cmajor::parsing::Rule
{
public:
    ParameterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ParameterRule>(this, &ParameterRule::A0Action));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostTypeId));
        cmajor::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ParameterRule>(this, &ParameterRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->AddInheritedAttribute(AttrOrVariable(context->fromTypeId->ToString(), context->fromDeclarator));
        delete context->fromTypeId;
    }
    void PostTypeId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
    void PostDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): rule(), fromTypeId(), fromDeclarator() {}
        cmajor::parsing::Rule* rule;
        cmajor::codedom::TypeId* fromTypeId;
        std::u32string fromDeclarator;
    };
};

class Element::ReturnTypeRule : public cmajor::parsing::Rule
{
public:
    ReturnTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ReturnTypeRule>(this, &ReturnTypeRule::A0Action));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ReturnTypeRule>(this, &ReturnTypeRule::PostTypeId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->rule->SetValueTypeName(context->fromTypeId->ToString());
        delete context->fromTypeId;
    }
    void PostTypeId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeId_value = std::move(stack.top());
            context->fromTypeId = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromTypeId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): rule(), fromTypeId() {}
        cmajor::parsing::Rule* rule;
        cmajor::codedom::TypeId* fromTypeId;
    };
};

class Element::IdentifierRule : public cmajor::parsing::Rule
{
public:
    IdentifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IdentifierRule>(this, &IdentifierRule::A0Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IdentifierRule>(this, &IdentifierRule::Postidentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void Postidentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromidentifier_value = std::move(stack.top());
            context->fromidentifier = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromidentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromidentifier() {}
        std::u32string value;
        std::u32string fromidentifier;
    };
};

class Element::QualifiedIdRule : public cmajor::parsing::Rule
{
public:
    QualifiedIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<QualifiedIdRule>(this, &QualifiedIdRule::A0Action));
        cmajor::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postfirst));
        cmajor::parsing::NonterminalParser* restNonterminalParser = GetNonterminal(ToUtf32("rest"));
        restNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<QualifiedIdRule>(this, &QualifiedIdRule::Postrest));
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

class Element::StringArrayRule : public cmajor::parsing::Rule
{
public:
    StringArrayRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("std::vector<std::u32string>*"), ToUtf32("array")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> array_value = std::move(stack.top());
        context->array = *static_cast<cmajor::parsing::ValueObject<std::vector<std::u32string>*>*>(array_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringArrayRule>(this, &StringArrayRule::A0Action));
        cmajor::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringArrayRule>(this, &StringArrayRule::Poststr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->array->push_back(context->fromstr);
    }
    void Poststr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): array(), fromstr() {}
        std::vector<std::u32string>* array;
        std::u32string fromstr;
    };
};

void Element::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.code.Declarator"));
    if (!grammar0)
    {
        grammar0 = cmajor::code::Declarator::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar1)
    {
        grammar1 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void Element::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("cmajor.code.Declarator.TypeId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("cmajor.parsing.stdlib.qualified_id")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("cmajor.parsing.stdlib.string")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("cmajor.parsing.stdlib.identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Declarator"), this, ToUtf32("cmajor.code.Declarator.Declarator")));
    AddRule(new RuleLinkRule(ToUtf32("RuleLink"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::KeywordParser(ToUtf32("using")),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("aliasName"), ToUtf32("identifier"), 0)),
                                new cmajor::parsing::CharParser('=')),
                            new cmajor::parsing::NonterminalParser(ToUtf32("ruleName"), ToUtf32("qualified_id"), 0)),
                        new cmajor::parsing::CharParser(';')))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::KeywordParser(ToUtf32("using")),
                            new cmajor::parsing::NonterminalParser(ToUtf32("qualified_id"), ToUtf32("qualified_id"), 0)),
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new SignatureRule(ToUtf32("Signature"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 1)),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("ReturnType"), ToUtf32("ReturnType"), 1)))));
    AddRule(new ParameterListRule(ToUtf32("ParameterList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('('),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ListParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("Variable"), ToUtf32("Variable"), 1),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Parameter"), ToUtf32("Parameter"), 1))),
                        new cmajor::parsing::CharParser(',')))),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::CharParser(')')))));
    AddRule(new VariableRule(ToUtf32("Variable"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::KeywordParser(ToUtf32("var")),
                new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))));
    AddRule(new ParameterRule(ToUtf32("Parameter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))));
    AddRule(new ReturnTypeRule(ToUtf32("ReturnType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::DifferenceParser(
                    new cmajor::parsing::CharParser(':'),
                    new cmajor::parsing::StringParser(ToUtf32("::")))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Keyword"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KeywordListParser(ToUtf32("identifier"), keywords0)));
    AddRule(new IdentifierRule(ToUtf32("Identifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::DifferenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("identifier"), ToUtf32("identifier"), 0),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0))))));
    AddRule(new QualifiedIdRule(ToUtf32("QualifiedId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("Identifier"), 0),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('.'),
                                new cmajor::parsing::NonterminalParser(ToUtf32("rest"), ToUtf32("Identifier"), 0)))))))));
    AddRule(new StringArrayRule(ToUtf32("StringArray"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('['),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ListParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0)),
                        new cmajor::parsing::CharParser(',')))),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::CharParser(']')))));
}

} } // namespace cmajor.syntax
