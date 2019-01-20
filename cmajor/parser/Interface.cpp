#include "Interface.hpp"
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
#include <cmajor/parser/Attribute.hpp>
#include <cmajor/parser/Specifier.hpp>
#include <cmajor/parser/Identifier.hpp>
#include <cmajor/parser/TypeExpr.hpp>
#include <cmajor/parser/Parameter.hpp>
#include <cmajor/ast/Class.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Interface* Interface::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Interface* Interface::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Interface* grammar(new Interface(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Interface::Interface(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Interface"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

InterfaceNode* Interface::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    InterfaceNode* result = *static_cast<cmajor::parsing::ValueObject<InterfaceNode*>*>(value.get());
    stack.pop();
    return result;
}

class Interface::InterfaceRule : public cmajor::parsing::Rule
{
public:
    InterfaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("InterfaceNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("beginBraceSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("endBraceSpan")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<InterfaceNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceRule>(this, &InterfaceRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceRule>(this, &InterfaceRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceRule>(this, &InterfaceRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceRule>(this, &InterfaceRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceRule>(this, &InterfaceRule::A4Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceRule>(this, &InterfaceRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceRule>(this, &InterfaceRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceRule>(this, &InterfaceRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* interfaceContentNonterminalParser = GetNonterminal(ToUtf32("InterfaceContent"));
        interfaceContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InterfaceRule>(this, &InterfaceRule::PreInterfaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new InterfaceNode(span, context->fromSpecifiers, context->fromIdentifier, context->attributes.release());
        context->value->SetSpecifierSpan(context->specifierSpan);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->attributes.reset(context->fromAttributes);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->specifierSpan = span;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->beginBraceSpan = span;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->endBraceSpan = span;
        context->value->SetBeginBraceSpan(context->beginBraceSpan);
        context->value->SetEndBraceSpan(context->endBraceSpan);
    }
    void PostAttributes(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAttributes_value = std::move(stack.top());
            context->fromAttributes = *static_cast<cmajor::parsing::ValueObject<cmajor::ast::Attributes*>*>(fromAttributes_value.get());
            stack.pop();
        }
    }
    void PostSpecifiers(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSpecifiers_value = std::move(stack.top());
            context->fromSpecifiers = *static_cast<cmajor::parsing::ValueObject<Specifiers>*>(fromSpecifiers_value.get());
            stack.pop();
        }
    }
    void PostIdentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PreInterfaceContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<InterfaceNode*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), attributes(), specifierSpan(), beginBraceSpan(), endBraceSpan(), fromAttributes(), fromSpecifiers(), fromIdentifier() {}
        ParsingContext* ctx;
        InterfaceNode* value;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        Span beginBraceSpan;
        Span endBraceSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        IdentifierNode* fromIdentifier;
    };
};

class Interface::InterfaceContentRule : public cmajor::parsing::Rule
{
public:
    InterfaceContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("InterfaceNode*"), ToUtf32("intf")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> intf_value = std::move(stack.top());
        context->intf = *static_cast<cmajor::parsing::ValueObject<InterfaceNode*>*>(intf_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceContentRule>(this, &InterfaceContentRule::A0Action));
        cmajor::parsing::NonterminalParser* interfaceMemFunNonterminalParser = GetNonterminal(ToUtf32("InterfaceMemFun"));
        interfaceMemFunNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InterfaceContentRule>(this, &InterfaceContentRule::PreInterfaceMemFun));
        interfaceMemFunNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceContentRule>(this, &InterfaceContentRule::PostInterfaceMemFun));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->intf->AddMember(context->fromInterfaceMemFun);
    }
    void PreInterfaceMemFun(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInterfaceMemFun(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInterfaceMemFun_value = std::move(stack.top());
            context->fromInterfaceMemFun = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromInterfaceMemFun_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), intf(), fromInterfaceMemFun() {}
        ParsingContext* ctx;
        InterfaceNode* intf;
        Node* fromInterfaceMemFun;
    };
};

class Interface::InterfaceMemFunRule : public cmajor::parsing::Rule
{
public:
    InterfaceMemFunRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<MemberFunctionNode>"), ToUtf32("memFun")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceMemFunRule>(this, &InterfaceMemFunRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceMemFunRule>(this, &InterfaceMemFunRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceMemFunRule>(this, &InterfaceMemFunRule::A2Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PostAttributes));
        cmajor::parsing::NonterminalParser* returnTypeExprNonterminalParser = GetNonterminal(ToUtf32("returnTypeExpr"));
        returnTypeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PrereturnTypeExpr));
        returnTypeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PostreturnTypeExpr));
        cmajor::parsing::NonterminalParser* groupIdNonterminalParser = GetNonterminal(ToUtf32("groupId"));
        groupIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PostgroupId));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InterfaceMemFunRule>(this, &InterfaceMemFunRule::PreParameterList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->memFun.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->attributes.reset(context->fromAttributes);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->memFun.reset(new MemberFunctionNode(span, Specifiers(), context->fromreturnTypeExpr, context->fromgroupId, context->attributes.release()));
    }
    void PostAttributes(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAttributes_value = std::move(stack.top());
            context->fromAttributes = *static_cast<cmajor::parsing::ValueObject<cmajor::ast::Attributes*>*>(fromAttributes_value.get());
            stack.pop();
        }
    }
    void PrereturnTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostreturnTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromreturnTypeExpr_value = std::move(stack.top());
            context->fromreturnTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromreturnTypeExpr_value.get());
            stack.pop();
        }
    }
    void PostgroupId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromgroupId_value = std::move(stack.top());
            context->fromgroupId = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromgroupId_value.get());
            stack.pop();
        }
    }
    void PreParameterList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->memFun.get())));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), memFun(), attributes(), fromAttributes(), fromreturnTypeExpr(), fromgroupId() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<MemberFunctionNode> memFun;
        std::unique_ptr<Attributes> attributes;
        cmajor::ast::Attributes* fromAttributes;
        Node* fromreturnTypeExpr;
        std::u32string fromgroupId;
    };
};

class Interface::InterfaceFunctionGroupIdRule : public cmajor::parsing::Rule
{
public:
    InterfaceFunctionGroupIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("id")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceFunctionGroupIdRule>(this, &InterfaceFunctionGroupIdRule::A0Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceFunctionGroupIdRule>(this, &InterfaceFunctionGroupIdRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromIdentifier);
        context->value = context->id->Str();
    }
    void PostIdentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), id(), fromIdentifier() {}
        std::u32string value;
        std::unique_ptr<IdentifierNode> id;
        IdentifierNode* fromIdentifier;
    };
};

void Interface::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Specifier"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Specifier::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Attribute"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Attribute::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.TypeExpr"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parser.Parameter"));
    if (!grammar4)
    {
        grammar4 = cmajor::parser::Parameter::Create(pd);
    }
    AddGrammarReference(grammar4);
}

void Interface::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Specifiers"), this, ToUtf32("Specifier.Specifiers")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Attributes"), this, ToUtf32("Attribute.Attributes")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ParameterList"), this, ToUtf32("Parameter.ParameterList")));
    AddRule(new InterfaceRule(ToUtf32("Interface"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Attributes"), ToUtf32("Attributes"), 0)))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0))),
                                    new cmajor::parsing::KeywordParser(ToUtf32("interface"))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('{')))),
                new cmajor::parsing::NonterminalParser(ToUtf32("InterfaceContent"), ToUtf32("InterfaceContent"), 2)),
            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('}'))))));
    AddRule(new InterfaceContentRule(ToUtf32("InterfaceContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("InterfaceMemFun"), ToUtf32("InterfaceMemFun"), 1))))));
    AddRule(new InterfaceMemFunRule(ToUtf32("InterfaceMemFun"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::OptionalParser(
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Attributes"), ToUtf32("Attributes"), 0)))),
                                new cmajor::parsing::NonterminalParser(ToUtf32("returnTypeExpr"), ToUtf32("TypeExpr"), 1)),
                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("groupId"), ToUtf32("InterfaceFunctionGroupId"), 0)))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new InterfaceFunctionGroupIdRule(ToUtf32("InterfaceFunctionGroupId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))));
}

} } // namespace cmajor.parser
