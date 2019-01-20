#include "Function.hpp"
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
#include <cmajor/parser/TypeExpr.hpp>
#include <cmajor/parser/Concept.hpp>
#include <cmajor/parser/Identifier.hpp>
#include <cmajor/parser/Template.hpp>
#include <cmajor/parser/Parameter.hpp>
#include <cmajor/parser/Statement.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Function* Function::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Function* Function::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Function* grammar(new Function(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Function::Function(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Function"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

FunctionNode* Function::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    FunctionNode* result = *static_cast<cmajor::parsing::ValueObject<FunctionNode*>*>(value.get());
    stack.pop();
    return result;
}

class Function::FunctionRule : public cmajor::parsing::Rule
{
public:
    FunctionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("FunctionNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<FunctionNode>"), ToUtf32("fun")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("specifierSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("groupIdSpan")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<FunctionNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionRule>(this, &FunctionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionRule>(this, &FunctionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionRule>(this, &FunctionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionRule>(this, &FunctionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionRule>(this, &FunctionRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionRule>(this, &FunctionRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionRule>(this, &FunctionRule::A6Action));
        cmajor::parsing::NonterminalParser* attributesNonterminalParser = GetNonterminal(ToUtf32("Attributes"));
        attributesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionRule>(this, &FunctionRule::PostAttributes));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionRule>(this, &FunctionRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionRule>(this, &FunctionRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionRule>(this, &FunctionRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* functionGroupIdNonterminalParser = GetNonterminal(ToUtf32("FunctionGroupId"));
        functionGroupIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionRule>(this, &FunctionRule::PreFunctionGroupId));
        functionGroupIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionRule>(this, &FunctionRule::PostFunctionGroupId));
        cmajor::parsing::NonterminalParser* templateParameterListNonterminalParser = GetNonterminal(ToUtf32("TemplateParameterList"));
        templateParameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionRule>(this, &FunctionRule::PreTemplateParameterList));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionRule>(this, &FunctionRule::PreParameterList));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionRule>(this, &FunctionRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionRule>(this, &FunctionRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* compoundStatementNonterminalParser = GetNonterminal(ToUtf32("CompoundStatement"));
        compoundStatementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionRule>(this, &FunctionRule::PreCompoundStatement));
        compoundStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionRule>(this, &FunctionRule::PostCompoundStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fun.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fun->GetSpan() = span;
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
        context->fun.reset(new FunctionNode(span, context->fromSpecifiers, context->fromTypeExpr, context->fromFunctionGroupId, context->attributes.release()));
        context->fun->SetSpecifierSpan(context->specifierSpan);
        context->fun->SetGroupIdSpan(context->groupIdSpan);
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fun->SetConstraint(context->fromWhereConstraint);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fun->SetBody(context->fromCompoundStatement);
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
    void PreTemplateParameterList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->fun.get())));
    }
    void PreParameterList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->fun.get())));
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
        Context(): ctx(), value(), fun(), s(), specifierSpan(), groupIdSpan(), attributes(), fromAttributes(), fromSpecifiers(), fromTypeExpr(), fromFunctionGroupId(), fromWhereConstraint(), fromCompoundStatement() {}
        ParsingContext* ctx;
        FunctionNode* value;
        std::unique_ptr<FunctionNode> fun;
        Span s;
        Span specifierSpan;
        Span groupIdSpan;
        std::unique_ptr<Attributes> attributes;
        cmajor::ast::Attributes* fromAttributes;
        Specifiers fromSpecifiers;
        Node* fromTypeExpr;
        std::u32string fromFunctionGroupId;
        WhereConstraintNode* fromWhereConstraint;
        CompoundStatementNode* fromCompoundStatement;
    };
};

class Function::FunctionGroupIdRule : public cmajor::parsing::Rule
{
public:
    FunctionGroupIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("std::u32string"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("id")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionGroupIdRule>(this, &FunctionGroupIdRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionGroupIdRule>(this, &FunctionGroupIdRule::A1Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionGroupIdRule>(this, &FunctionGroupIdRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* operatorFunctionGroupIdNonterminalParser = GetNonterminal(ToUtf32("OperatorFunctionGroupId"));
        operatorFunctionGroupIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionGroupIdRule>(this, &FunctionGroupIdRule::PreOperatorFunctionGroupId));
        operatorFunctionGroupIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionGroupIdRule>(this, &FunctionGroupIdRule::PostOperatorFunctionGroupId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromIdentifier);
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOperatorFunctionGroupId;
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
    void PreOperatorFunctionGroupId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostOperatorFunctionGroupId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOperatorFunctionGroupId_value = std::move(stack.top());
            context->fromOperatorFunctionGroupId = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromOperatorFunctionGroupId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), id(), fromIdentifier(), fromOperatorFunctionGroupId() {}
        ParsingContext* ctx;
        std::u32string value;
        std::unique_ptr<IdentifierNode> id;
        IdentifierNode* fromIdentifier;
        std::u32string fromOperatorFunctionGroupId;
    };
};

class Function::OperatorFunctionGroupIdRule : public cmajor::parsing::Rule
{
public:
    OperatorFunctionGroupIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("std::u32string"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("typeExpr")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A10Action));
        cmajor::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A11Action));
        cmajor::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A12Action));
        cmajor::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A13Action));
        cmajor::parsing::ActionParser* a14ActionParser = GetAction(ToUtf32("A14"));
        a14ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A14Action));
        cmajor::parsing::ActionParser* a15ActionParser = GetAction(ToUtf32("A15"));
        a15ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A15Action));
        cmajor::parsing::ActionParser* a16ActionParser = GetAction(ToUtf32("A16"));
        a16ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A16Action));
        cmajor::parsing::ActionParser* a17ActionParser = GetAction(ToUtf32("A17"));
        a17ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A17Action));
        cmajor::parsing::ActionParser* a18ActionParser = GetAction(ToUtf32("A18"));
        a18ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A18Action));
        cmajor::parsing::ActionParser* a19ActionParser = GetAction(ToUtf32("A19"));
        a19ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A19Action));
        cmajor::parsing::ActionParser* a20ActionParser = GetAction(ToUtf32("A20"));
        a20ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::A20Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<OperatorFunctionGroupIdRule>(this, &OperatorFunctionGroupIdRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator<<");
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromTypeExpr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator>>");
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator==");
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator=");
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator<");
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator->");
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator++");
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator--");
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator+");
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator-");
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator*");
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator/");
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator%");
    }
    void A14Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator&");
    }
    void A15Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator|");
    }
    void A16Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator^");
    }
    void A17Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator!");
    }
    void A18Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator~");
    }
    void A19Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator[]");
    }
    void A20Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf32("operator()");
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
        Context(): ctx(), value(), typeExpr(), fromTypeExpr() {}
        ParsingContext* ctx;
        std::u32string value;
        std::unique_ptr<Node> typeExpr;
        Node* fromTypeExpr;
    };
};

void Function::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Parameter"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Parameter::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Concept"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Concept::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Attribute"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Attribute::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.Specifier"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::Specifier::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parser.TypeExpr"));
    if (!grammar4)
    {
        grammar4 = cmajor::parser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar4);
    cmajor::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar5)
    {
        grammar5 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar5);
    cmajor::parsing::Grammar* grammar6 = pd->GetGrammar(ToUtf32("cmajor.parser.Template"));
    if (!grammar6)
    {
        grammar6 = cmajor::parser::Template::Create(pd);
    }
    AddGrammarReference(grammar6);
    cmajor::parsing::Grammar* grammar7 = pd->GetGrammar(ToUtf32("cmajor.parser.Statement"));
    if (!grammar7)
    {
        grammar7 = cmajor::parser::Statement::Create(pd);
    }
    AddGrammarReference(grammar7);
}

void Function::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("WhereConstraint"), this, ToUtf32("Concept.WhereConstraint")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Attributes"), this, ToUtf32("Attribute.Attributes")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ParameterList"), this, ToUtf32("Parameter.ParameterList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Specifiers"), this, ToUtf32("Specifier.Specifiers")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TemplateParameterList"), this, ToUtf32("Template.TemplateParameterList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("CompoundStatement"), this, ToUtf32("Statement.CompoundStatement")));
    AddRule(new FunctionRule(ToUtf32("Function"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
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
                                                new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("FunctionGroupId"), ToUtf32("FunctionGroupId"), 1))),
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("TemplateParameterList"), ToUtf32("TemplateParameterList"), 2))),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2)))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1))))),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("CompoundStatement"), ToUtf32("CompoundStatement"), 1)),
                            new cmajor::parsing::CharParser(';'))))))));
    AddRule(new FunctionGroupIdRule(ToUtf32("FunctionGroupId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("OperatorFunctionGroupId"), ToUtf32("OperatorFunctionGroupId"), 1)))));
    AddRule(new OperatorFunctionGroupIdRule(ToUtf32("OperatorFunctionGroupId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::KeywordParser(ToUtf32("operator")),
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
                                                                new cmajor::parsing::AlternativeParser(
                                                                    new cmajor::parsing::AlternativeParser(
                                                                        new cmajor::parsing::AlternativeParser(
                                                                            new cmajor::parsing::AlternativeParser(
                                                                                new cmajor::parsing::AlternativeParser(
                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                                                                                new cmajor::parsing::GroupingParser(
                                                                                                    new cmajor::parsing::DifferenceParser(
                                                                                                        new cmajor::parsing::StringParser(ToUtf32("<<")),
                                                                                                        new cmajor::parsing::GroupingParser(
                                                                                                            new cmajor::parsing::SequenceParser(
                                                                                                                new cmajor::parsing::SequenceParser(
                                                                                                                    new cmajor::parsing::SequenceParser(
                                                                                                                        new cmajor::parsing::CharParser('<'),
                                                                                                                        new cmajor::parsing::CharParser('<')),
                                                                                                                    new cmajor::parsing::GroupingParser(
                                                                                                                        new cmajor::parsing::ListParser(
                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                                                                                                                            new cmajor::parsing::CharParser(',')))),
                                                                                                                new cmajor::parsing::CharParser('>')))))),
                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                                                                new cmajor::parsing::StringParser(ToUtf32(">>")))),
                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                                                            new cmajor::parsing::StringParser(ToUtf32("==")))),
                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                                                        new cmajor::parsing::CharParser('='))),
                                                                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                                                    new cmajor::parsing::CharParser('<'))),
                                                                            new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                                                                new cmajor::parsing::StringParser(ToUtf32("->")))),
                                                                        new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                                                            new cmajor::parsing::StringParser(ToUtf32("++")))),
                                                                    new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                                                        new cmajor::parsing::StringParser(ToUtf32("--")))),
                                                                new cmajor::parsing::ActionParser(ToUtf32("A9"),
                                                                    new cmajor::parsing::CharParser('+'))),
                                                            new cmajor::parsing::ActionParser(ToUtf32("A10"),
                                                                new cmajor::parsing::CharParser('-'))),
                                                        new cmajor::parsing::ActionParser(ToUtf32("A11"),
                                                            new cmajor::parsing::CharParser('*'))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A12"),
                                                        new cmajor::parsing::CharParser('/'))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A13"),
                                                    new cmajor::parsing::CharParser('%'))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A14"),
                                                new cmajor::parsing::CharParser('&'))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A15"),
                                            new cmajor::parsing::CharParser('|'))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A16"),
                                        new cmajor::parsing::CharParser('^'))),
                                new cmajor::parsing::ActionParser(ToUtf32("A17"),
                                    new cmajor::parsing::CharParser('!'))),
                            new cmajor::parsing::ActionParser(ToUtf32("A18"),
                                new cmajor::parsing::CharParser('~'))),
                        new cmajor::parsing::ActionParser(ToUtf32("A19"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('['),
                                    new cmajor::parsing::CharParser(']'))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A20"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('('),
                                new cmajor::parsing::CharParser(')')))))))));
}

} } // namespace cmajor.parser
