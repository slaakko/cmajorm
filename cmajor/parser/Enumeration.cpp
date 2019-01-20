#include "Enumeration.hpp"
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
#include <cmajor/parser/Specifier.hpp>
#include <cmajor/parser/Identifier.hpp>
#include <cmajor/parser/TypeExpr.hpp>
#include <cmajor/parser/Expression.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Enumeration* Enumeration::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Enumeration* Enumeration::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Enumeration* grammar(new Enumeration(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Enumeration::Enumeration(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Enumeration"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

EnumTypeNode* Enumeration::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    EnumTypeNode* result = *static_cast<cmajor::parsing::ValueObject<EnumTypeNode*>*>(value.get());
    stack.pop();
    return result;
}

class Enumeration::EnumTypeRule : public cmajor::parsing::Rule
{
public:
    EnumTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("EnumTypeNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<EnumTypeNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumTypeRule>(this, &EnumTypeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumTypeRule>(this, &EnumTypeRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumTypeRule>(this, &EnumTypeRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumTypeRule>(this, &EnumTypeRule::A3Action));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EnumTypeRule>(this, &EnumTypeRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* enumTypeIdNonterminalParser = GetNonterminal(ToUtf32("enumTypeId"));
        enumTypeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EnumTypeRule>(this, &EnumTypeRule::PostenumTypeId));
        cmajor::parsing::NonterminalParser* underlyingTypeNonterminalParser = GetNonterminal(ToUtf32("UnderlyingType"));
        underlyingTypeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EnumTypeRule>(this, &EnumTypeRule::PreUnderlyingType));
        underlyingTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EnumTypeRule>(this, &EnumTypeRule::PostUnderlyingType));
        cmajor::parsing::NonterminalParser* enumConstantsNonterminalParser = GetNonterminal(ToUtf32("EnumConstants"));
        enumConstantsNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EnumTypeRule>(this, &EnumTypeRule::PreEnumConstants));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EnumTypeNode(span, context->fromSpecifiers, context->fromenumTypeId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->SetUnderlyingType(context->fromUnderlyingType);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->beginBraceSpan = span;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->endBraceSpan = span;
        context->value->GetSpan().SetEnd(span.End());
        context->value->SetBeginBraceSpan(context->beginBraceSpan);
        context->value->SetEndBraceSpan(context->endBraceSpan);
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
    void PostenumTypeId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromenumTypeId_value = std::move(stack.top());
            context->fromenumTypeId = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromenumTypeId_value.get());
            stack.pop();
        }
    }
    void PreUnderlyingType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostUnderlyingType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUnderlyingType_value = std::move(stack.top());
            context->fromUnderlyingType = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromUnderlyingType_value.get());
            stack.pop();
        }
    }
    void PreEnumConstants(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<EnumTypeNode*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), beginBraceSpan(), endBraceSpan(), fromSpecifiers(), fromenumTypeId(), fromUnderlyingType() {}
        ParsingContext* ctx;
        EnumTypeNode* value;
        Span beginBraceSpan;
        Span endBraceSpan;
        Specifiers fromSpecifiers;
        IdentifierNode* fromenumTypeId;
        Node* fromUnderlyingType;
    };
};

class Enumeration::UnderlyingTypeRule : public cmajor::parsing::Rule
{
public:
    UnderlyingTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnderlyingTypeRule>(this, &UnderlyingTypeRule::A0Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<UnderlyingTypeRule>(this, &UnderlyingTypeRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnderlyingTypeRule>(this, &UnderlyingTypeRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeExpr;
    }
    void PreTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeExpr_value = std::move(stack.top());
            context->fromTypeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTypeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromTypeExpr() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTypeExpr;
    };
};

class Enumeration::EnumConstantsRule : public cmajor::parsing::Rule
{
public:
    EnumConstantsRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("EnumTypeNode*"), ToUtf32("enumType")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> enumType_value = std::move(stack.top());
        context->enumType = *static_cast<cmajor::parsing::ValueObject<EnumTypeNode*>*>(enumType_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumConstantsRule>(this, &EnumConstantsRule::A0Action));
        cmajor::parsing::NonterminalParser* enumConstantNonterminalParser = GetNonterminal(ToUtf32("EnumConstant"));
        enumConstantNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EnumConstantsRule>(this, &EnumConstantsRule::PreEnumConstant));
        enumConstantNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EnumConstantsRule>(this, &EnumConstantsRule::PostEnumConstant));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->enumType->AddConstant(context->fromEnumConstant);
    }
    void PreEnumConstant(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<EnumTypeNode*>(context->enumType)));
    }
    void PostEnumConstant(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEnumConstant_value = std::move(stack.top());
            context->fromEnumConstant = *static_cast<cmajor::parsing::ValueObject<EnumConstantNode*>*>(fromEnumConstant_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), enumType(), fromEnumConstant() {}
        ParsingContext* ctx;
        EnumTypeNode* enumType;
        EnumConstantNode* fromEnumConstant;
    };
};

class Enumeration::EnumConstantRule : public cmajor::parsing::Rule
{
public:
    EnumConstantRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("EnumTypeNode*"), ToUtf32("enumType")));
        SetValueTypeName(ToUtf32("EnumConstantNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> enumType_value = std::move(stack.top());
        context->enumType = *static_cast<cmajor::parsing::ValueObject<EnumTypeNode*>*>(enumType_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> ctx_value = std::move(stack.top());
        context->ctx = *static_cast<cmajor::parsing::ValueObject<ParsingContext*>*>(ctx_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<EnumConstantNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumConstantRule>(this, &EnumConstantRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumConstantRule>(this, &EnumConstantRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumConstantRule>(this, &EnumConstantRule::A2Action));
        cmajor::parsing::NonterminalParser* constantIdNonterminalParser = GetNonterminal(ToUtf32("constantId"));
        constantIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EnumConstantRule>(this, &EnumConstantRule::PostconstantId));
        cmajor::parsing::NonterminalParser* constantValueNonterminalParser = GetNonterminal(ToUtf32("constantValue"));
        constantValueNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EnumConstantRule>(this, &EnumConstantRule::PreconstantValue));
        constantValueNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EnumConstantRule>(this, &EnumConstantRule::PostconstantValue));
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
        context->value = new EnumConstantNode(context->s, context->fromconstantId, context->fromconstantValue);
        context->value->SetHasValue();
        context->value->SetStrValue(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = new EnumConstantNode(context->s, context->fromconstantId, MakeNextEnumConstantValue(context->s, context->enumType));
    }
    void PostconstantId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromconstantId_value = std::move(stack.top());
            context->fromconstantId = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromconstantId_value.get());
            stack.pop();
        }
    }
    void PreconstantValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostconstantValue(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromconstantValue_value = std::move(stack.top());
            context->fromconstantValue = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromconstantValue_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), enumType(), value(), s(), fromconstantId(), fromconstantValue() {}
        ParsingContext* ctx;
        EnumTypeNode* enumType;
        EnumConstantNode* value;
        Span s;
        IdentifierNode* fromconstantId;
        Node* fromconstantValue;
    };
};

void Enumeration::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Specifier"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Specifier::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.TypeExpr"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.Expression"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::Expression::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void Enumeration::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Specifiers"), this, ToUtf32("Specifier.Specifiers")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRule(new EnumTypeRule(ToUtf32("EnumType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0),
                                        new cmajor::parsing::KeywordParser(ToUtf32("enum"))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("enumTypeId"), ToUtf32("Identifier"), 0))))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("UnderlyingType"), ToUtf32("UnderlyingType"), 1))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('{')))),
                new cmajor::parsing::NonterminalParser(ToUtf32("EnumConstants"), ToUtf32("EnumConstants"), 2)),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('}'))))));
    AddRule(new UnderlyingTypeRule(ToUtf32("UnderlyingType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::CharParser(':'),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)))));
    AddRule(new EnumConstantsRule(ToUtf32("EnumConstants"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ListParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("EnumConstant"), ToUtf32("EnumConstant"), 2))),
            new cmajor::parsing::CharParser(','))));
    AddRule(new EnumConstantRule(ToUtf32("EnumConstant"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("constantId"), ToUtf32("Identifier"), 0))),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('='),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("constantValue"), ToUtf32("Expression"), 1))),
                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                        new cmajor::parsing::EmptyParser()))))));
}

} } // namespace cmajor.parser
