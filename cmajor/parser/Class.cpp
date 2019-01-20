#include "Class.hpp"
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
#include <cmajor/parser/Concept.hpp>
#include <cmajor/parser/Expression.hpp>
#include <cmajor/parser/Statement.hpp>
#include <cmajor/parser/Parameter.hpp>
#include <cmajor/parser/Function.hpp>
#include <cmajor/parser/Template.hpp>
#include <cmajor/parser/Typedef.hpp>
#include <cmajor/parser/Enumeration.hpp>
#include <cmajor/parser/Constant.hpp>
#include <cmajor/parser/Delegate.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Class* Class::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Class* Class::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Class* grammar(new Class(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Class::Class(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Class"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

ClassNode* Class::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    ClassNode* result = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(value.get());
    stack.pop();
    return result;
}

class Class::ClassRule : public cmajor::parsing::Rule
{
public:
    ClassRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ClassNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("classSpan")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassRule>(this, &ClassRule::A7Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassRule>(this, &ClassRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassRule>(this, &ClassRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassRule>(this, &ClassRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* templateParameterListNonterminalParser = GetNonterminal(ToUtf32("TemplateParameterList"));
        templateParameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreTemplateParameterList));
        cmajor::parsing::NonterminalParser* inheritanceAndInterfacesNonterminalParser = GetNonterminal(ToUtf32("InheritanceAndInterfaces"));
        inheritanceAndInterfacesNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreInheritanceAndInterfaces));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassRule>(this, &ClassRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* classContentNonterminalParser = GetNonterminal(ToUtf32("ClassContent"));
        classContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassRule>(this, &ClassRule::PreClassContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->GetSpan() = span;
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
        context->classSpan = span;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ClassNode(span, context->fromSpecifiers, context->fromIdentifier, context->attributes.release());
        context->value->SetSpecifierSpan(context->specifierSpan);
        context->value->SetClassSpan(context->classSpan);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->SetConstraint(context->fromWhereConstraint);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->beginBraceSpan = span;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
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
    void PreTemplateParameterList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
    }
    void PreInheritanceAndInterfaces(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassNode*>(context->value)));
    }
    void PreWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context->fromWhereConstraint = *static_cast<cmajor::parsing::ValueObject<WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
    void PreClassContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassNode*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), attributes(), specifierSpan(), classSpan(), beginBraceSpan(), endBraceSpan(), fromAttributes(), fromSpecifiers(), fromIdentifier(), fromWhereConstraint() {}
        ParsingContext* ctx;
        ClassNode* value;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        Span classSpan;
        Span beginBraceSpan;
        Span endBraceSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        IdentifierNode* fromIdentifier;
        WhereConstraintNode* fromWhereConstraint;
    };
};

class Class::InheritanceAndInterfacesRule : public cmajor::parsing::Rule
{
public:
    InheritanceAndInterfacesRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InheritanceAndInterfacesRule>(this, &InheritanceAndInterfacesRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InheritanceAndInterfacesRule>(this, &InheritanceAndInterfacesRule::A1Action));
        a1ActionParser->SetFailureAction(new cmajor::parsing::MemberFailureAction<InheritanceAndInterfacesRule>(this, &InheritanceAndInterfacesRule::A1ActionFail));
        cmajor::parsing::NonterminalParser* baseClassOrInterfaceNonterminalParser = GetNonterminal(ToUtf32("BaseClassOrInterface"));
        baseClassOrInterfaceNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InheritanceAndInterfacesRule>(this, &InheritanceAndInterfacesRule::PreBaseClassOrInterface));
        baseClassOrInterfaceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InheritanceAndInterfacesRule>(this, &InheritanceAndInterfacesRule::PostBaseClassOrInterface));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingTypeExpr();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->classNode->AddBaseClassOrInterface(context->fromBaseClassOrInterface);
        context->ctx->EndParsingTypeExpr();
    }
    void A1ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingTypeExpr();
    }
    void PreBaseClassOrInterface(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostBaseClassOrInterface(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromBaseClassOrInterface_value = std::move(stack.top());
            context->fromBaseClassOrInterface = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromBaseClassOrInterface_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), classNode(), fromBaseClassOrInterface() {}
        ParsingContext* ctx;
        ClassNode* classNode;
        Node* fromBaseClassOrInterface;
    };
};

class Class::BaseClassOrInterfaceRule : public cmajor::parsing::Rule
{
public:
    BaseClassOrInterfaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BaseClassOrInterfaceRule>(this, &BaseClassOrInterfaceRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BaseClassOrInterfaceRule>(this, &BaseClassOrInterfaceRule::A1Action));
        cmajor::parsing::NonterminalParser* templateIdNonterminalParser = GetNonterminal(ToUtf32("TemplateId"));
        templateIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<BaseClassOrInterfaceRule>(this, &BaseClassOrInterfaceRule::PreTemplateId));
        templateIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BaseClassOrInterfaceRule>(this, &BaseClassOrInterfaceRule::PostTemplateId));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BaseClassOrInterfaceRule>(this, &BaseClassOrInterfaceRule::PostQualifiedId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTemplateId;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromQualifiedId;
    }
    void PreTemplateId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTemplateId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTemplateId_value = std::move(stack.top());
            context->fromTemplateId = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromTemplateId_value.get());
            stack.pop();
        }
    }
    void PostQualifiedId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context->fromQualifiedId = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromTemplateId(), fromQualifiedId() {}
        ParsingContext* ctx;
        Node* value;
        Node* fromTemplateId;
        IdentifierNode* fromQualifiedId;
    };
};

class Class::ClassContentRule : public cmajor::parsing::Rule
{
public:
    ClassContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassContentRule>(this, &ClassContentRule::A0Action));
        cmajor::parsing::NonterminalParser* classMemberNonterminalParser = GetNonterminal(ToUtf32("ClassMember"));
        classMemberNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassContentRule>(this, &ClassContentRule::PreClassMember));
        classMemberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassContentRule>(this, &ClassContentRule::PostClassMember));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->classNode->AddMember(context->fromClassMember);
    }
    void PreClassMember(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassNode*>(context->classNode)));
    }
    void PostClassMember(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromClassMember_value = std::move(stack.top());
            context->fromClassMember = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromClassMember_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), classNode(), fromClassMember() {}
        ParsingContext* ctx;
        ClassNode* classNode;
        Node* fromClassMember;
    };
};

class Class::ClassMemberRule : public cmajor::parsing::Rule
{
public:
    ClassMemberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A10Action));
        cmajor::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassMemberRule>(this, &ClassMemberRule::A11Action));
        cmajor::parsing::NonterminalParser* staticConstructorNonterminalParser = GetNonterminal(ToUtf32("StaticConstructor"));
        staticConstructorNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreStaticConstructor));
        staticConstructorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostStaticConstructor));
        cmajor::parsing::NonterminalParser* constructorNonterminalParser = GetNonterminal(ToUtf32("Constructor"));
        constructorNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreConstructor));
        constructorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostConstructor));
        cmajor::parsing::NonterminalParser* destructorNonterminalParser = GetNonterminal(ToUtf32("Destructor"));
        destructorNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreDestructor));
        destructorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostDestructor));
        cmajor::parsing::NonterminalParser* memberFunctionNonterminalParser = GetNonterminal(ToUtf32("MemberFunction"));
        memberFunctionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreMemberFunction));
        memberFunctionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostMemberFunction));
        cmajor::parsing::NonterminalParser* conversionFunctionNonterminalParser = GetNonterminal(ToUtf32("ConversionFunction"));
        conversionFunctionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreConversionFunction));
        conversionFunctionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostConversionFunction));
        cmajor::parsing::NonterminalParser* memberVariableNonterminalParser = GetNonterminal(ToUtf32("MemberVariable"));
        memberVariableNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreMemberVariable));
        memberVariableNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostMemberVariable));
        cmajor::parsing::NonterminalParser* typedefNonterminalParser = GetNonterminal(ToUtf32("Typedef"));
        typedefNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreTypedef));
        typedefNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostTypedef));
        cmajor::parsing::NonterminalParser* classNonterminalParser = GetNonterminal(ToUtf32("Class"));
        classNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreClass));
        classNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostClass));
        cmajor::parsing::NonterminalParser* enumTypeNonterminalParser = GetNonterminal(ToUtf32("EnumType"));
        enumTypeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreEnumType));
        enumTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostEnumType));
        cmajor::parsing::NonterminalParser* constantNonterminalParser = GetNonterminal(ToUtf32("Constant"));
        constantNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreConstant));
        constantNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostConstant));
        cmajor::parsing::NonterminalParser* delegateNonterminalParser = GetNonterminal(ToUtf32("Delegate"));
        delegateNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreDelegate));
        delegateNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostDelegate));
        cmajor::parsing::NonterminalParser* classDelegateNonterminalParser = GetNonterminal(ToUtf32("ClassDelegate"));
        classDelegateNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassMemberRule>(this, &ClassMemberRule::PreClassDelegate));
        classDelegateNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassMemberRule>(this, &ClassMemberRule::PostClassDelegate));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStaticConstructor;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConstructor;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDestructor;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMemberFunction;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConversionFunction;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMemberVariable;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypedef;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClass;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnumType;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConstant;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDelegate;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClassDelegate;
    }
    void PreStaticConstructor(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassNode*>(context->classNode)));
    }
    void PostStaticConstructor(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStaticConstructor_value = std::move(stack.top());
            context->fromStaticConstructor = *static_cast<cmajor::parsing::ValueObject<StaticConstructorNode*>*>(fromStaticConstructor_value.get());
            stack.pop();
        }
    }
    void PreConstructor(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassNode*>(context->classNode)));
    }
    void PostConstructor(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstructor_value = std::move(stack.top());
            context->fromConstructor = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromConstructor_value.get());
            stack.pop();
        }
    }
    void PreDestructor(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassNode*>(context->classNode)));
    }
    void PostDestructor(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDestructor_value = std::move(stack.top());
            context->fromDestructor = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromDestructor_value.get());
            stack.pop();
        }
    }
    void PreMemberFunction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostMemberFunction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromMemberFunction_value = std::move(stack.top());
            context->fromMemberFunction = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromMemberFunction_value.get());
            stack.pop();
        }
    }
    void PreConversionFunction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConversionFunction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConversionFunction_value = std::move(stack.top());
            context->fromConversionFunction = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromConversionFunction_value.get());
            stack.pop();
        }
    }
    void PreMemberVariable(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostMemberVariable(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromMemberVariable_value = std::move(stack.top());
            context->fromMemberVariable = *static_cast<cmajor::parsing::ValueObject<MemberVariableNode*>*>(fromMemberVariable_value.get());
            stack.pop();
        }
    }
    void PreTypedef(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypedef(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypedef_value = std::move(stack.top());
            context->fromTypedef = *static_cast<cmajor::parsing::ValueObject<TypedefNode*>*>(fromTypedef_value.get());
            stack.pop();
        }
    }
    void PreClass(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClass(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromClass_value = std::move(stack.top());
            context->fromClass = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(fromClass_value.get());
            stack.pop();
        }
    }
    void PreEnumType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEnumType_value = std::move(stack.top());
            context->fromEnumType = *static_cast<cmajor::parsing::ValueObject<EnumTypeNode*>*>(fromEnumType_value.get());
            stack.pop();
        }
    }
    void PreConstant(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConstant(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstant_value = std::move(stack.top());
            context->fromConstant = *static_cast<cmajor::parsing::ValueObject<ConstantNode*>*>(fromConstant_value.get());
            stack.pop();
        }
    }
    void PreDelegate(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDelegate(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDelegate_value = std::move(stack.top());
            context->fromDelegate = *static_cast<cmajor::parsing::ValueObject<DelegateNode*>*>(fromDelegate_value.get());
            stack.pop();
        }
    }
    void PreClassDelegate(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassDelegate(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromClassDelegate_value = std::move(stack.top());
            context->fromClassDelegate = *static_cast<cmajor::parsing::ValueObject<ClassDelegateNode*>*>(fromClassDelegate_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), classNode(), value(), fromStaticConstructor(), fromConstructor(), fromDestructor(), fromMemberFunction(), fromConversionFunction(), fromMemberVariable(), fromTypedef(), fromClass(), fromEnumType(), fromConstant(), fromDelegate(), fromClassDelegate() {}
        ParsingContext* ctx;
        ClassNode* classNode;
        Node* value;
        StaticConstructorNode* fromStaticConstructor;
        Node* fromConstructor;
        Node* fromDestructor;
        Node* fromMemberFunction;
        Node* fromConversionFunction;
        MemberVariableNode* fromMemberVariable;
        TypedefNode* fromTypedef;
        ClassNode* fromClass;
        EnumTypeNode* fromEnumType;
        ConstantNode* fromConstant;
        DelegateNode* fromDelegate;
        ClassDelegateNode* fromClassDelegate;
    };
};

class Class::StaticConstructorRule : public cmajor::parsing::Rule
{
public:
    StaticConstructorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
        SetValueTypeName(ToUtf32("StaticConstructorNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("id")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<StaticConstructorNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StaticConstructorRule>(this, &StaticConstructorRule::A6Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::Postspecifiers));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* initializerNonterminalParser = GetNonterminal(ToUtf32("Initializer"));
        initializerNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<StaticConstructorRule>(this, &StaticConstructorRule::PreInitializer));
        initializerNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostInitializer));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<StaticConstructorRule>(this, &StaticConstructorRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<StaticConstructorRule>(this, &StaticConstructorRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StaticConstructorRule>(this, &StaticConstructorRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->id->Str() != context->classNode->Id()->Str()) pass = false;
        else if (!StaticConstructorSpecifiers(context->fromspecifiers)) pass = false;
        else
        {
            context->value = new StaticConstructorNode(span, context->fromspecifiers, context->attributes.release());
            context->value->SetClassId(context->id.release());
            context->value->SetSpecifierSpan(context->specifierSpan);
        }
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
        context->id.reset(context->fromIdentifier);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddInitializer(context->fromInitializer);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->SetConstraint(context->fromWhereConstraint);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->SetBody(context->fromCompoundStatement);
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
    void Postspecifiers(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromspecifiers_value = std::move(stack.top());
            context->fromspecifiers = *static_cast<cmajor::parsing::ValueObject<Specifiers>*>(fromspecifiers_value.get());
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
    void PreInitializer(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInitializer(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitializer_value = std::move(stack.top());
            context->fromInitializer = *static_cast<cmajor::parsing::ValueObject<InitializerNode*>*>(fromInitializer_value.get());
            stack.pop();
        }
    }
    void PreWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context->fromWhereConstraint = *static_cast<cmajor::parsing::ValueObject<WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), classNode(), value(), id(), attributes(), specifierSpan(), fromAttributes(), fromspecifiers(), fromIdentifier(), fromInitializer(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        ClassNode* classNode;
        StaticConstructorNode* value;
        std::unique_ptr<IdentifierNode> id;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromspecifiers;
        IdentifierNode* fromIdentifier;
        InitializerNode* fromInitializer;
        WhereConstraintNode* fromWhereConstraint;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Class::ConstructorRule : public cmajor::parsing::Rule
{
public:
    ConstructorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("id")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<ConstructorNode>"), ToUtf32("ctor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorRule>(this, &ConstructorRule::A8Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreParameterList));
        cmajor::parsing::NonterminalParser* initializerNonterminalParser = GetNonterminal(ToUtf32("Initializer"));
        initializerNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreInitializer));
        initializerNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostInitializer));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstructorRule>(this, &ConstructorRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructorRule>(this, &ConstructorRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctor->GetSpan() = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->id->Str() != context->classNode->Id()->Str()) pass = false;
        else
        {
            context->ctor.reset(new ConstructorNode(span, context->fromSpecifiers, context->attributes.release()));
            context->ctor->SetClassId(context->id.release());
            context->ctor->SetSpecifierSpan(context->specifierSpan);
        }
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->attributes.reset(context->fromAttributes);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->specifierSpan = span;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromIdentifier);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctor->AddInitializer(context->fromInitializer);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctor->SetConstraint(context->fromWhereConstraint);
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ctor.release();
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctor->SetBody(context->fromCompoundStatement);
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
    void PreParameterList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->ctor.get())));
    }
    void PreInitializer(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInitializer(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitializer_value = std::move(stack.top());
            context->fromInitializer = *static_cast<cmajor::parsing::ValueObject<InitializerNode*>*>(fromInitializer_value.get());
            stack.pop();
        }
    }
    void PreWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context->fromWhereConstraint = *static_cast<cmajor::parsing::ValueObject<WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), classNode(), value(), id(), ctor(), attributes(), specifierSpan(), fromAttributes(), fromSpecifiers(), fromIdentifier(), fromInitializer(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        ClassNode* classNode;
        Node* value;
        std::unique_ptr<IdentifierNode> id;
        std::unique_ptr<ConstructorNode> ctor;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        IdentifierNode* fromIdentifier;
        InitializerNode* fromInitializer;
        WhereConstraintNode* fromWhereConstraint;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Class::DestructorRule : public cmajor::parsing::Rule
{
public:
    DestructorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ClassNode*"), ToUtf32("classNode")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("id")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<DestructorNode>"), ToUtf32("dtor")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> classNode_value = std::move(stack.top());
        context->classNode = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(classNode_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorRule>(this, &DestructorRule::A6Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DestructorRule>(this, &DestructorRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DestructorRule>(this, &DestructorRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DestructorRule>(this, &DestructorRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (context->id->Str() != context->classNode->Id()->Str()) pass = false;
        else
        {
            context->dtor.reset(new DestructorNode(span, context->fromSpecifiers, context->attributes.release()));
            context->dtor->SetClassId(context->id.release());
            context->dtor->SetSpecifierSpan(context->specifierSpan);
        }
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
        context->id.reset(context->fromIdentifier);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->dtor->SetConstraint(context->fromWhereConstraint);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->dtor.release();
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->dtor->SetBody(context->fromCompoundStatement);
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
    void PreWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context->fromWhereConstraint = *static_cast<cmajor::parsing::ValueObject<WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), classNode(), value(), id(), dtor(), attributes(), specifierSpan(), fromAttributes(), fromSpecifiers(), fromIdentifier(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        ClassNode* classNode;
        Node* value;
        std::unique_ptr<IdentifierNode> id;
        std::unique_ptr<DestructorNode> dtor;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        IdentifierNode* fromIdentifier;
        WhereConstraintNode* fromWhereConstraint;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Class::InitializerRule : public cmajor::parsing::Rule
{
public:
    InitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("InitializerNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<InitializerNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A2Action));
        cmajor::parsing::NonterminalParser* thisArgsNonterminalParser = GetNonterminal(ToUtf32("thisArgs"));
        thisArgsNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PrethisArgs));
        cmajor::parsing::NonterminalParser* baseArgsNonterminalParser = GetNonterminal(ToUtf32("baseArgs"));
        baseArgsNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PrebaseArgs));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* memberArgsNonterminalParser = GetNonterminal(ToUtf32("memberArgs"));
        memberArgsNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InitializerRule>(this, &InitializerRule::PrememberArgs));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ThisInitializerNode(span);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BaseInitializerNode(span);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MemberInitializerNode(span, context->fromIdentifier);
    }
    void PrethisArgs(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
    }
    void PrebaseArgs(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
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
    void PrememberArgs(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromIdentifier() {}
        ParsingContext* ctx;
        InitializerNode* value;
        IdentifierNode* fromIdentifier;
    };
};

class Class::MemberFunctionRule : public cmajor::parsing::Rule
{
public:
    MemberFunctionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<MemberFunctionNode>"), ToUtf32("memFun")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("qid")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("groupIdSpan")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionRule>(this, &MemberFunctionRule::A8Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* functionGroupIdNonterminalParser = GetNonterminal(ToUtf32("FunctionGroupId"));
        functionGroupIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreFunctionGroupId));
        functionGroupIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostFunctionGroupId));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreParameterList));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionRule>(this, &MemberFunctionRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionRule>(this, &MemberFunctionRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->memFun->GetSpan() = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->memFun.reset(new MemberFunctionNode(span, context->fromSpecifiers, context->fromTypeExpr, context->fromFunctionGroupId, context->attributes.release()));
        context->memFun->SetSpecifierSpan(context->specifierSpan);
        context->memFun->SetGroupIdSpan(context->groupIdSpan);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->attributes.reset(context->fromAttributes);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->specifierSpan = span;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->groupIdSpan = span;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->memFun->SetConst();
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->memFun->SetConstraint(context->fromWhereConstraint);
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->memFun.release();
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->memFun->SetBody(context->fromCompoundStatement);
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
    void PreFunctionGroupId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostFunctionGroupId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFunctionGroupId_value = std::move(stack.top());
            context->fromFunctionGroupId = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromFunctionGroupId_value.get());
            stack.pop();
        }
    }
    void PreParameterList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->memFun.get())));
    }
    void PreWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context->fromWhereConstraint = *static_cast<cmajor::parsing::ValueObject<WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), memFun(), qid(), attributes(), specifierSpan(), groupIdSpan(), fromAttributes(), fromSpecifiers(), fromTypeExpr(), fromFunctionGroupId(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<MemberFunctionNode> memFun;
        std::unique_ptr<IdentifierNode> qid;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        Span groupIdSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        Node* fromTypeExpr;
        std::u32string fromFunctionGroupId;
        WhereConstraintNode* fromWhereConstraint;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Class::ConversionFunctionRule : public cmajor::parsing::Rule
{
public:
    ConversionFunctionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("Node*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<ConversionFunctionNode>"), ToUtf32("conversionFun")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConversionFunctionRule>(this, &ConversionFunctionRule::A7Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConversionFunctionRule>(this, &ConversionFunctionRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->conversionFun->GetSpan() = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->conversionFun.reset(new ConversionFunctionNode(span, context->fromSpecifiers, context->fromTypeExpr, context->attributes.release()));
        context->conversionFun->SetSpecifierSpan(context->specifierSpan);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->attributes.reset(context->fromAttributes);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->specifierSpan = span;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->conversionFun->SetConst();
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->conversionFun->SetConstraint(context->fromWhereConstraint);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->conversionFun.release();
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->conversionFun->SetBody(context->fromCompoundStatement);
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
    void PreWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostWhereConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromWhereConstraint_value = std::move(stack.top());
            context->fromWhereConstraint = *static_cast<cmajor::parsing::ValueObject<WhereConstraintNode*>*>(fromWhereConstraint_value.get());
            stack.pop();
        }
    }
    void PreCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostCompoundStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCompoundStatement_value = std::move(stack.top());
            context->fromCompoundStatement = *static_cast<cmajor::parsing::ValueObject<CompoundStatementNode*>*>(fromCompoundStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), conversionFun(), attributes(), specifierSpan(), fromAttributes(), fromSpecifiers(), fromTypeExpr(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        Node* value;
        std::unique_ptr<ConversionFunctionNode> conversionFun;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        Node* fromTypeExpr;
        WhereConstraintNode* fromWhereConstraint;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Class::MemberVariableRule : public cmajor::parsing::Rule
{
public:
    MemberVariableRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("MemberVariableNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Attributes>"), ToUtf32("attributes")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<MemberVariableNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberVariableRule>(this, &MemberVariableRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberVariableRule>(this, &MemberVariableRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberVariableRule>(this, &MemberVariableRule::A2Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberVariableRule>(this, &MemberVariableRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberVariableRule>(this, &MemberVariableRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberVariableRule>(this, &MemberVariableRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberVariableRule>(this, &MemberVariableRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberVariableRule>(this, &MemberVariableRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MemberVariableNode(span, context->fromSpecifiers, context->fromTypeExpr, context->fromIdentifier, context->attributes.release());
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
        Context(): ctx(), value(), attributes(), specifierSpan(), fromAttributes(), fromSpecifiers(), fromTypeExpr(), fromIdentifier() {}
        ParsingContext* ctx;
        MemberVariableNode* value;
        std::unique_ptr<Attributes> attributes;
        Span specifierSpan;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        Node* fromTypeExpr;
        IdentifierNode* fromIdentifier;
    };
};

void Class::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Attribute"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Attribute::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Enumeration"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Enumeration::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Concept"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Concept::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parser.Template"));
    if (!grammar4)
    {
        grammar4 = cmajor::parser::Template::Create(pd);
    }
    AddGrammarReference(grammar4);
    cmajor::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("cmajor.parser.Parameter"));
    if (!grammar5)
    {
        grammar5 = cmajor::parser::Parameter::Create(pd);
    }
    AddGrammarReference(grammar5);
    cmajor::parsing::Grammar* grammar6 = pd->GetGrammar(ToUtf32("cmajor.parser.Specifier"));
    if (!grammar6)
    {
        grammar6 = cmajor::parser::Specifier::Create(pd);
    }
    AddGrammarReference(grammar6);
    cmajor::parsing::Grammar* grammar7 = pd->GetGrammar(ToUtf32("cmajor.parser.Constant"));
    if (!grammar7)
    {
        grammar7 = cmajor::parser::Constant::Create(pd);
    }
    AddGrammarReference(grammar7);
    cmajor::parsing::Grammar* grammar8 = pd->GetGrammar(ToUtf32("cmajor.parser.TypeExpr"));
    if (!grammar8)
    {
        grammar8 = cmajor::parser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar8);
    cmajor::parsing::Grammar* grammar9 = pd->GetGrammar(ToUtf32("cmajor.parser.Expression"));
    if (!grammar9)
    {
        grammar9 = cmajor::parser::Expression::Create(pd);
    }
    AddGrammarReference(grammar9);
    cmajor::parsing::Grammar* grammar10 = pd->GetGrammar(ToUtf32("cmajor.parser.Statement"));
    if (!grammar10)
    {
        grammar10 = cmajor::parser::Statement::Create(pd);
    }
    AddGrammarReference(grammar10);
    cmajor::parsing::Grammar* grammar11 = pd->GetGrammar(ToUtf32("cmajor.parser.Function"));
    if (!grammar11)
    {
        grammar11 = cmajor::parser::Function::Create(pd);
    }
    AddGrammarReference(grammar11);
    cmajor::parsing::Grammar* grammar12 = pd->GetGrammar(ToUtf32("cmajor.parser.Typedef"));
    if (!grammar12)
    {
        grammar12 = cmajor::parser::Typedef::Create(pd);
    }
    AddGrammarReference(grammar12);
    cmajor::parsing::Grammar* grammar13 = pd->GetGrammar(ToUtf32("cmajor.parser.Delegate"));
    if (!grammar13)
    {
        grammar13 = cmajor::parser::Delegate::Create(pd);
    }
    AddGrammarReference(grammar13);
}

void Class::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("WhereConstraint"), this, ToUtf32("Concept.WhereConstraint")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("EnumType"), this, ToUtf32("Enumeration.EnumType")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Attributes"), this, ToUtf32("Attribute.Attributes")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ParameterList"), this, ToUtf32("Parameter.ParameterList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TemplateParameterList"), this, ToUtf32("Template.TemplateParameterList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Specifiers"), this, ToUtf32("Specifier.Specifiers")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Identifier.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Constant"), this, ToUtf32("Constant.Constant")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ArgumentList"), this, ToUtf32("Expression.ArgumentList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("CompoundStatement"), this, ToUtf32("Statement.CompoundStatement")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("FunctionGroupId"), this, ToUtf32("Function.FunctionGroupId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TemplateId"), this, ToUtf32("Template.TemplateId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Typedef"), this, ToUtf32("Typedef.Typedef")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Delegate"), this, ToUtf32("Delegate.Delegate")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ClassDelegate"), this, ToUtf32("Delegate.ClassDelegate")));
    AddRule(new ClassRule(ToUtf32("Class"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
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
                                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                    new cmajor::parsing::KeywordParser(ToUtf32("class")))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("TemplateParameterList"), ToUtf32("TemplateParameterList"), 2))))),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("InheritanceAndInterfaces"), ToUtf32("InheritanceAndInterfaces"), 2))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A6"),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('{')))),
                new cmajor::parsing::NonterminalParser(ToUtf32("ClassContent"), ToUtf32("ClassContent"), 2)),
            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('}'))))));
    AddRule(new InheritanceAndInterfacesRule(ToUtf32("InheritanceAndInterfaces"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::CharParser(':'),
            new cmajor::parsing::ListParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::EmptyParser()),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("BaseClassOrInterface"), ToUtf32("BaseClassOrInterface"), 1)))),
                new cmajor::parsing::CharParser(',')))));
    AddRule(new BaseClassOrInterfaceRule(ToUtf32("BaseClassOrInterface"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TemplateId"), ToUtf32("TemplateId"), 1)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0)))));
    AddRule(new ClassContentRule(ToUtf32("ClassContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ClassMember"), ToUtf32("ClassMember"), 2))))));
    AddRule(new ClassMemberRule(ToUtf32("ClassMember"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("StaticConstructor"), ToUtf32("StaticConstructor"), 2)),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Constructor"), ToUtf32("Constructor"), 2))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Destructor"), ToUtf32("Destructor"), 2))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("MemberFunction"), ToUtf32("MemberFunction"), 1))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("ConversionFunction"), ToUtf32("ConversionFunction"), 1))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("MemberVariable"), ToUtf32("MemberVariable"), 1))),
                                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Typedef"), ToUtf32("Typedef"), 1))),
                            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Class"), ToUtf32("Class"), 1))),
                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("EnumType"), ToUtf32("EnumType"), 1))),
                    new cmajor::parsing::ActionParser(ToUtf32("A9"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Constant"), ToUtf32("Constant"), 1))),
                new cmajor::parsing::ActionParser(ToUtf32("A10"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Delegate"), ToUtf32("Delegate"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A11"),
                new cmajor::parsing::NonterminalParser(ToUtf32("ClassDelegate"), ToUtf32("ClassDelegate"), 1)))));
    AddRule(new StaticConstructorRule(ToUtf32("StaticConstructor"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
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
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("specifiers"), ToUtf32("Specifiers"), 0))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                                    new cmajor::parsing::CharParser('(')),
                                new cmajor::parsing::CharParser(')')))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser(':'),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ListParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Initializer"), ToUtf32("Initializer"), 1)),
                                        new cmajor::parsing::CharParser(','))))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A5"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1))))),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A6"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1)),
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new ConstructorRule(ToUtf32("Constructor"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::OptionalParser(
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Attributes"), ToUtf32("Attributes"), 0)))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))))),
                                new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2)))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser(':'),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ListParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Initializer"), ToUtf32("Initializer"), 1)),
                                        new cmajor::parsing::CharParser(','))))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A6"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1))))),
            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1)),
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new DestructorRule(ToUtf32("Destructor"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::OptionalParser(
                                                new cmajor::parsing::GroupingParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Attributes"), ToUtf32("Attributes"), 0)))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0))),
                                        new cmajor::parsing::CharParser('~')),
                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                                new cmajor::parsing::CharParser('(')),
                            new cmajor::parsing::CharParser(')')))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1))))),
            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A6"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1)),
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new InitializerRule(ToUtf32("Initializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                new cmajor::parsing::KeywordParser(ToUtf32("this"))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('('))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("thisArgs"), ToUtf32("ArgumentList"), 2))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(')'))),
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::KeywordParser(ToUtf32("base"))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('('))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("baseArgs"), ToUtf32("ArgumentList"), 2))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(')')))),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('('))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("memberArgs"), ToUtf32("ArgumentList"), 2))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new MemberFunctionRule(ToUtf32("MemberFunction"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::OptionalParser(
                                                        new cmajor::parsing::GroupingParser(
                                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                                new cmajor::parsing::NonterminalParser(ToUtf32("Attributes"), ToUtf32("Attributes"), 0)))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0))),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("FunctionGroupId"), ToUtf32("FunctionGroupId"), 1))))),
                                new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2)),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                        new cmajor::parsing::KeywordParser(ToUtf32("const")))))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A6"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1))))),
            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1)),
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new ConversionFunctionRule(ToUtf32("ConversionFunction"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::SequenceParser(
                                                        new cmajor::parsing::OptionalParser(
                                                            new cmajor::parsing::GroupingParser(
                                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Attributes"), ToUtf32("Attributes"), 0)))),
                                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0))),
                                                    new cmajor::parsing::KeywordParser(ToUtf32("operator"))),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::CharParser('('))),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::CharParser(')'))))),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                        new cmajor::parsing::KeywordParser(ToUtf32("const")))))))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A5"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1))))),
            new cmajor::parsing::ActionParser(ToUtf32("A6"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A7"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1)),
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new MemberVariableRule(ToUtf32("MemberVariable"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0))),
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                    new cmajor::parsing::CharParser(';'))))));
}

} } // namespace cmajor.parser
