#include "Expression.hpp"
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
#include <cmajor/code/Declaration.hpp>
#include <cmajor/code/Declarator.hpp>
#include <cmajor/code/Literal.hpp>
#include <cmajor/code/Identifier.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/codedom/Type.hpp>

namespace cmajor { namespace code {

using namespace cmajor::codedom;
using cmajor::util::Trim;
using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Expression* Expression::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Expression* Expression::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Expression* grammar(new Expression(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Expression::Expression(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Expression"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.code")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::codedom::CppObject* Expression::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    cmajor::codedom::CppObject* result = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(value.get());
    stack.pop();
    return result;
}

class Expression::ExpressionRule : public cmajor::parsing::Rule
{
public:
    ExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionRule>(this, &ExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExpressionRule>(this, &ExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::comma, 0, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::ConstantExpressionRule : public cmajor::parsing::Rule
{
public:
    ConstantExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstantExpressionRule>(this, &ConstantExpressionRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstantExpressionRule>(this, &ConstantExpressionRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpression;
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExpression() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromExpression;
    };
};

class Expression::AssignmentExpressionRule : public cmajor::parsing::Rule
{
public:
    AssignmentExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("lor")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AssignmentExpressionRule>(this, &AssignmentExpressionRule::A3Action));
        cmajor::parsing::NonterminalParser* logicalOrExpressionNonterminalParser = GetNonterminal(ToUtf32("LogicalOrExpression"));
        logicalOrExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostLogicalOrExpression));
        cmajor::parsing::NonterminalParser* assingmentOpNonterminalParser = GetNonterminal(ToUtf32("AssingmentOp"));
        assingmentOpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssingmentOp));
        cmajor::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostAssignmentExpression));
        cmajor::parsing::NonterminalParser* conditionalExpressionNonterminalParser = GetNonterminal(ToUtf32("ConditionalExpression"));
        conditionalExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostConditionalExpression));
        cmajor::parsing::NonterminalParser* throwExpressionNonterminalParser = GetNonterminal(ToUtf32("ThrowExpression"));
        throwExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AssignmentExpressionRule>(this, &AssignmentExpressionRule::PostThrowExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->lor.release(), context->fromAssingmentOp, 1, context->fromAssignmentExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->lor.reset(context->fromLogicalOrExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConditionalExpression;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromThrowExpression;
    }
    void PostLogicalOrExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLogicalOrExpression_value = std::move(stack.top());
            context->fromLogicalOrExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromLogicalOrExpression_value.get());
            stack.pop();
        }
    }
    void PostAssingmentOp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAssingmentOp_value = std::move(stack.top());
            context->fromAssingmentOp = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromAssingmentOp_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
    void PostConditionalExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConditionalExpression_value = std::move(stack.top());
            context->fromConditionalExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromConditionalExpression_value.get());
            stack.pop();
        }
    }
    void PostThrowExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromThrowExpression_value = std::move(stack.top());
            context->fromThrowExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromThrowExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), lor(), fromLogicalOrExpression(), fromAssingmentOp(), fromAssignmentExpression(), fromConditionalExpression(), fromThrowExpression() {}
        cmajor::codedom::CppObject* value;
        std::unique_ptr<CppObject> lor;
        cmajor::codedom::CppObject* fromLogicalOrExpression;
        Operator fromAssingmentOp;
        cmajor::codedom::CppObject* fromAssignmentExpression;
        cmajor::codedom::CppObject* fromConditionalExpression;
        cmajor::codedom::CppObject* fromThrowExpression;
    };
};

class Expression::AssingmentOpRule : public cmajor::parsing::Rule
{
public:
    AssingmentOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AssingmentOpRule>(this, &AssingmentOpRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = GetOperator(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::ThrowExpressionRule : public cmajor::parsing::Rule
{
public:
    ThrowExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ThrowExpressionRule>(this, &ThrowExpressionRule::A0Action));
        cmajor::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ThrowExpressionRule>(this, &ThrowExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ThrowExpr(context->fromAssignmentExpression);
    }
    void PostAssignmentExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromAssignmentExpression;
    };
};

class Expression::ConditionalExpressionRule : public cmajor::parsing::Rule
{
public:
    ConditionalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConditionalExpressionRule>(this, &ConditionalExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostExpression));
        cmajor::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConditionalExpressionRule>(this, &ConditionalExpressionRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConditionalExpr(context->value, context->fromExpression, context->fromAssignmentExpression);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostAssignmentExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromExpression(), fromAssignmentExpression() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        cmajor::codedom::CppObject* fromExpression;
        cmajor::codedom::CppObject* fromAssignmentExpression;
    };
};

class Expression::LogicalOrExpressionRule : public cmajor::parsing::Rule
{
public:
    LogicalOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LogicalOrExpressionRule>(this, &LogicalOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::or_, 2, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::LogicalAndExpressionRule : public cmajor::parsing::Rule
{
public:
    LogicalAndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LogicalAndExpressionRule>(this, &LogicalAndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::and_, 3, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::InclusiveOrExpressionRule : public cmajor::parsing::Rule
{
public:
    InclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InclusiveOrExpressionRule>(this, &InclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitor_, 4, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::ExclusiveOrExpressionRule : public cmajor::parsing::Rule
{
public:
    ExclusiveOrExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExclusiveOrExpressionRule>(this, &ExclusiveOrExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitxor, 5, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::AndExpressionRule : public cmajor::parsing::Rule
{
public:
    AndExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AndExpressionRule>(this, &AndExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AndExpressionRule>(this, &AndExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, Operator::bitand_, 6, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::EqualityExpressionRule : public cmajor::parsing::Rule
{
public:
    EqualityExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityExpressionRule>(this, &EqualityExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* eqOpNonterminalParser = GetNonterminal(ToUtf32("EqOp"));
        eqOpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::PostEqOp));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EqualityExpressionRule>(this, &EqualityExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromEqOp, 7, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostEqOp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEqOp_value = std::move(stack.top());
            context->fromEqOp = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromEqOp_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromEqOp(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        Operator fromEqOp;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::EqOpRule : public cmajor::parsing::Rule
{
public:
    EqOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqOpRule>(this, &EqOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::eq;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::notEq;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::RelationalExpressionRule : public cmajor::parsing::Rule
{
public:
    RelationalExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExpressionRule>(this, &RelationalExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* relOpNonterminalParser = GetNonterminal(ToUtf32("RelOp"));
        relOpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::PostRelOp));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalExpressionRule>(this, &RelationalExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromRelOp, 8, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostRelOp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromRelOp_value = std::move(stack.top());
            context->fromRelOp = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromRelOp_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromRelOp(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        Operator fromRelOp;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::RelOpRule : public cmajor::parsing::Rule
{
public:
    RelOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelOpRule>(this, &RelOpRule::A3Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::lessOrEq;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greaterOrEq;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::less;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::greater;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::ShiftExpressionRule : public cmajor::parsing::Rule
{
public:
    ShiftExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftExpressionRule>(this, &ShiftExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* shiftOpNonterminalParser = GetNonterminal(ToUtf32("ShiftOp"));
        shiftOpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::PostShiftOp));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ShiftExpressionRule>(this, &ShiftExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromShiftOp, 9, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostShiftOp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromShiftOp_value = std::move(stack.top());
            context->fromShiftOp = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromShiftOp_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromShiftOp(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        Operator fromShiftOp;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::ShiftOpRule : public cmajor::parsing::Rule
{
public:
    ShiftOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ShiftOpRule>(this, &ShiftOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftLeft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::shiftRight;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::AdditiveExpressionRule : public cmajor::parsing::Rule
{
public:
    AdditiveExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveExpressionRule>(this, &AdditiveExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* addOpNonterminalParser = GetNonterminal(ToUtf32("AddOp"));
        addOpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::PostAddOp));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AdditiveExpressionRule>(this, &AdditiveExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromAddOp, 10, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostAddOp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAddOp_value = std::move(stack.top());
            context->fromAddOp = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromAddOp_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromAddOp(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        Operator fromAddOp;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::AddOpRule : public cmajor::parsing::Rule
{
public:
    AddOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AddOpRule>(this, &AddOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::plus;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::minus;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::MultiplicativeExpressionRule : public cmajor::parsing::Rule
{
public:
    MultiplicativeExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* mulOpNonterminalParser = GetNonterminal(ToUtf32("MulOp"));
        mulOpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::PostMulOp));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiplicativeExpressionRule>(this, &MultiplicativeExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromMulOp, 11, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostMulOp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromMulOp_value = std::move(stack.top());
            context->fromMulOp = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromMulOp_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromMulOp(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        Operator fromMulOp;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::MulOpRule : public cmajor::parsing::Rule
{
public:
    MulOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MulOpRule>(this, &MulOpRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::mul;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::div;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::rem;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::PmExpressionRule : public cmajor::parsing::Rule
{
public:
    PmExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PmExpressionRule>(this, &PmExpressionRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postleft));
        cmajor::parsing::NonterminalParser* pmOpNonterminalParser = GetNonterminal(ToUtf32("PmOp"));
        pmOpNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::PostPmOp));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PmExpressionRule>(this, &PmExpressionRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BinaryOpExpr(context->value, context->fromPmOp, 12, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void PostPmOp(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPmOp_value = std::move(stack.top());
            context->fromPmOp = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromPmOp_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromPmOp(), fromright() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::CppObject* fromleft;
        Operator fromPmOp;
        cmajor::codedom::CppObject* fromright;
    };
};

class Expression::PmOpRule : public cmajor::parsing::Rule
{
public:
    PmOpRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PmOpRule>(this, &PmOpRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::dotStar;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::arrowStar;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::CastExpressionRule : public cmajor::parsing::Rule
{
public:
    CastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ce")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ti")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CastExpressionRule>(this, &CastExpressionRule::A3Action));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostTypeId));
        cmajor::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostCastExpression));
        cmajor::parsing::NonterminalParser* unaryExpressionNonterminalParser = GetNonterminal(ToUtf32("UnaryExpression"));
        unaryExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CastExpressionRule>(this, &CastExpressionRule::PostUnaryExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ce.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ce.reset(new CastExpr(context->ti.release(), context->fromCastExpression));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(context->fromTypeId);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ce.reset(context->fromUnaryExpression);
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
    void PostCastExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
    void PostUnaryExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUnaryExpression_value = std::move(stack.top());
            context->fromUnaryExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromUnaryExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), ce(), ti(), fromTypeId(), fromCastExpression(), fromUnaryExpression() {}
        cmajor::codedom::CppObject* value;
        std::unique_ptr<CppObject> ce;
        std::unique_ptr<CppObject> ti;
        cmajor::codedom::TypeId* fromTypeId;
        cmajor::codedom::CppObject* fromCastExpression;
        cmajor::codedom::CppObject* fromUnaryExpression;
    };
};

class Expression::UnaryExpressionRule : public cmajor::parsing::Rule
{
public:
    UnaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("ue")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExpressionRule>(this, &UnaryExpressionRule::A9Action));
        cmajor::parsing::NonterminalParser* postfixExpressionNonterminalParser = GetNonterminal(ToUtf32("PostfixExpression"));
        postfixExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostfixExpression));
        cmajor::parsing::NonterminalParser* postCastExpressionNonterminalParser = GetNonterminal(ToUtf32("PostCastExpression"));
        postCastExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostPostCastExpression));
        cmajor::parsing::NonterminalParser* e1NonterminalParser = GetNonterminal(ToUtf32("e1"));
        e1NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste1));
        cmajor::parsing::NonterminalParser* e2NonterminalParser = GetNonterminal(ToUtf32("e2"));
        e2NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste2));
        cmajor::parsing::NonterminalParser* unaryOperatorNonterminalParser = GetNonterminal(ToUtf32("UnaryOperator"));
        unaryOperatorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostUnaryOperator));
        cmajor::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostCastExpression));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostTypeId));
        cmajor::parsing::NonterminalParser* e3NonterminalParser = GetNonterminal(ToUtf32("e3"));
        e3NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::Poste3));
        cmajor::parsing::NonterminalParser* newExpressionNonterminalParser = GetNonterminal(ToUtf32("NewExpression"));
        newExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostNewExpression));
        cmajor::parsing::NonterminalParser* deleteExpressionNonterminalParser = GetNonterminal(ToUtf32("DeleteExpression"));
        deleteExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExpressionRule>(this, &UnaryExpressionRule::PostDeleteExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ue.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromPostfixExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromPostCastExpression);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new PreIncrementExpr(context->frome1));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new PreDecrementExpr(context->frome2));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new UnaryOpExpr(context->fromUnaryOperator, context->fromCastExpression));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new SizeOfExpr(context->fromTypeId, true));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(new SizeOfExpr(context->frome3, false));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromNewExpression);
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ue.reset(context->fromDeleteExpression);
    }
    void PostPostfixExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPostfixExpression_value = std::move(stack.top());
            context->fromPostfixExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromPostfixExpression_value.get());
            stack.pop();
        }
    }
    void PostPostCastExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPostCastExpression_value = std::move(stack.top());
            context->fromPostCastExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromPostCastExpression_value.get());
            stack.pop();
        }
    }
    void Poste1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frome1_value = std::move(stack.top());
            context->frome1 = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(frome1_value.get());
            stack.pop();
        }
    }
    void Poste2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frome2_value = std::move(stack.top());
            context->frome2 = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(frome2_value.get());
            stack.pop();
        }
    }
    void PostUnaryOperator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUnaryOperator_value = std::move(stack.top());
            context->fromUnaryOperator = *static_cast<cmajor::parsing::ValueObject<Operator>*>(fromUnaryOperator_value.get());
            stack.pop();
        }
    }
    void PostCastExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
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
    void Poste3(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frome3_value = std::move(stack.top());
            context->frome3 = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(frome3_value.get());
            stack.pop();
        }
    }
    void PostNewExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNewExpression_value = std::move(stack.top());
            context->fromNewExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromNewExpression_value.get());
            stack.pop();
        }
    }
    void PostDeleteExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeleteExpression_value = std::move(stack.top());
            context->fromDeleteExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromDeleteExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), ue(), fromPostfixExpression(), fromPostCastExpression(), frome1(), frome2(), fromUnaryOperator(), fromCastExpression(), fromTypeId(), frome3(), fromNewExpression(), fromDeleteExpression() {}
        cmajor::codedom::CppObject* value;
        std::unique_ptr<CppObject> ue;
        cmajor::codedom::CppObject* fromPostfixExpression;
        cmajor::codedom::CppObject* fromPostCastExpression;
        cmajor::codedom::CppObject* frome1;
        cmajor::codedom::CppObject* frome2;
        Operator fromUnaryOperator;
        cmajor::codedom::CppObject* fromCastExpression;
        cmajor::codedom::TypeId* fromTypeId;
        cmajor::codedom::CppObject* frome3;
        cmajor::codedom::CppObject* fromNewExpression;
        cmajor::codedom::CppObject* fromDeleteExpression;
    };
};

class Expression::UnaryOperatorRule : public cmajor::parsing::Rule
{
public:
    UnaryOperatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Operator"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Operator>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryOperatorRule>(this, &UnaryOperatorRule::A5Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::deref;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::addrOf;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::plus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::minus;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::not_;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Operator::neg;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Operator value;
    };
};

class Expression::NewExpressionRule : public cmajor::parsing::Rule
{
public:
    NewExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("global")));
        AddLocalVariable(AttrOrVariable(ToUtf32("TypeId*"), ToUtf32("typeId")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("parens")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewExpressionRule>(this, &NewExpressionRule::A3Action));
        cmajor::parsing::NonterminalParser* newPlacementNonterminalParser = GetNonterminal(ToUtf32("NewPlacement"));
        newPlacementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewPlacement));
        cmajor::parsing::NonterminalParser* newTypeIdNonterminalParser = GetNonterminal(ToUtf32("NewTypeId"));
        newTypeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewTypeId));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostTypeId));
        cmajor::parsing::NonterminalParser* newInitializerNonterminalParser = GetNonterminal(ToUtf32("NewInitializer"));
        newInitializerNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewExpressionRule>(this, &NewExpressionRule::PostNewInitializer));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NewExpr(context->global, context->fromNewPlacement, context->typeId, context->parens, context->fromNewInitializer);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->global = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId = context->fromNewTypeId;
        context->parens = false;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId = context->fromTypeId;
        context->parens = true;
    }
    void PostNewPlacement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNewPlacement_value = std::move(stack.top());
            context->fromNewPlacement = *static_cast<cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>*>(fromNewPlacement_value.get());
            stack.pop();
        }
    }
    void PostNewTypeId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNewTypeId_value = std::move(stack.top());
            context->fromNewTypeId = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromNewTypeId_value.get());
            stack.pop();
        }
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
    void PostNewInitializer(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNewInitializer_value = std::move(stack.top());
            context->fromNewInitializer = *static_cast<cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>*>(fromNewInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), global(), typeId(), parens(), fromNewPlacement(), fromNewTypeId(), fromTypeId(), fromNewInitializer() {}
        cmajor::codedom::CppObject* value;
        bool global;
        TypeId* typeId;
        bool parens;
        std::vector<cmajor::codedom::CppObject*> fromNewPlacement;
        cmajor::codedom::TypeId* fromNewTypeId;
        cmajor::codedom::TypeId* fromTypeId;
        std::vector<cmajor::codedom::CppObject*> fromNewInitializer;
    };
};

class Expression::NewTypeIdRule : public cmajor::parsing::Rule
{
public:
    NewTypeIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::TypeId*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<TypeId>"), ToUtf32("ti")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewTypeIdRule>(this, &NewTypeIdRule::A3Action));
        cmajor::parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierSeq"));
        typeSpecifierSeqNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NewTypeIdRule>(this, &NewTypeIdRule::PreTypeSpecifierSeq));
        cmajor::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostTypeName));
        cmajor::parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal(ToUtf32("NewDeclarator"));
        newDeclaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewTypeIdRule>(this, &NewTypeIdRule::PostNewDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti.reset(new TypeId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ti.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti->Add(context->fromTypeName);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ti->Declarator() = context->fromNewDeclarator;
    }
    void PreTypeSpecifierSeq(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>(context->ti.get())));
    }
    void PostTypeName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeName_value = std::move(stack.top());
            context->fromTypeName = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
    void PostNewDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context->fromNewDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), ti(), fromTypeName(), fromNewDeclarator() {}
        cmajor::codedom::TypeId* value;
        std::unique_ptr<TypeId> ti;
        cmajor::codedom::TypeName* fromTypeName;
        std::u32string fromNewDeclarator;
    };
};

class Expression::NewDeclaratorRule : public cmajor::parsing::Rule
{
public:
    NewDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewDeclaratorRule>(this, &NewDeclaratorRule::A0Action));
        cmajor::parsing::NonterminalParser* newDeclaratorNonterminalParser = GetNonterminal(ToUtf32("NewDeclarator"));
        newDeclaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewDeclaratorRule>(this, &NewDeclaratorRule::PostNewDeclarator));
        cmajor::parsing::NonterminalParser* directNewDeclaratorNonterminalParser = GetNonterminal(ToUtf32("DirectNewDeclarator"));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Trim(std::u32string(matchBegin, matchEnd));
    }
    void PostNewDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNewDeclarator_value = std::move(stack.top());
            context->fromNewDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromNewDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromNewDeclarator() {}
        std::u32string value;
        std::u32string fromNewDeclarator;
    };
};

class Expression::DirectNewDeclaratorRule : public cmajor::parsing::Rule
{
public:
    DirectNewDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("e")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::A1Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostExpression));
        cmajor::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DirectNewDeclaratorRule>(this, &DirectNewDeclaratorRule::PostConstantExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->e.reset(context->fromExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->e.reset(context->fromConstantExpression);
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostConstantExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstantExpression_value = std::move(stack.top());
            context->fromConstantExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromConstantExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): e(), fromExpression(), fromConstantExpression() {}
        std::unique_ptr<CppObject> e;
        cmajor::codedom::CppObject* fromExpression;
        cmajor::codedom::CppObject* fromConstantExpression;
    };
};

class Expression::NewPlacementRule : public cmajor::parsing::Rule
{
public:
    NewPlacementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<cmajor::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewPlacementRule>(this, &NewPlacementRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewPlacementRule>(this, &NewPlacementRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void PostExpressionList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<cmajor::codedom::CppObject*> value;
        std::vector<cmajor::codedom::CppObject*> fromExpressionList;
    };
};

class Expression::NewInitializerRule : public cmajor::parsing::Rule
{
public:
    NewInitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<cmajor::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NewInitializerRule>(this, &NewInitializerRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NewInitializerRule>(this, &NewInitializerRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpressionList;
    }
    void PostExpressionList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpressionList_value = std::move(stack.top());
            context->fromExpressionList = *static_cast<cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>*>(fromExpressionList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExpressionList() {}
        std::vector<cmajor::codedom::CppObject*> value;
        std::vector<cmajor::codedom::CppObject*> fromExpressionList;
    };
};

class Expression::DeleteExpressionRule : public cmajor::parsing::Rule
{
public:
    DeleteExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("global")));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("array")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeleteExpressionRule>(this, &DeleteExpressionRule::A2Action));
        cmajor::parsing::NonterminalParser* castExpressionNonterminalParser = GetNonterminal(ToUtf32("CastExpression"));
        castExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeleteExpressionRule>(this, &DeleteExpressionRule::PostCastExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DeleteExpr(context->global, context->array, context->fromCastExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->global = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->array = true;
    }
    void PostCastExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCastExpression_value = std::move(stack.top());
            context->fromCastExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromCastExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), global(), array(), fromCastExpression() {}
        cmajor::codedom::CppObject* value;
        bool global;
        bool array;
        cmajor::codedom::CppObject* fromCastExpression;
    };
};

class Expression::PostfixExpressionRule : public cmajor::parsing::Rule
{
public:
    PostfixExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("pe")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostfixExpressionRule>(this, &PostfixExpressionRule::A8Action));
        cmajor::parsing::NonterminalParser* primaryExpressionNonterminalParser = GetNonterminal(ToUtf32("PrimaryExpression"));
        primaryExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostPrimaryExpression));
        cmajor::parsing::NonterminalParser* indexNonterminalParser = GetNonterminal(ToUtf32("index"));
        indexNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postindex));
        cmajor::parsing::NonterminalParser* invokeNonterminalParser = GetNonterminal(ToUtf32("invoke"));
        invokeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postinvoke));
        cmajor::parsing::NonterminalParser* memberNonterminalParser = GetNonterminal(ToUtf32("member"));
        memberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postmember));
        cmajor::parsing::NonterminalParser* ptrmemberNonterminalParser = GetNonterminal(ToUtf32("ptrmember"));
        ptrmemberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::Postptrmember));
        cmajor::parsing::NonterminalParser* typeSpecifierOrTypeNameNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierOrTypeName"));
        typeSpecifierOrTypeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PostTypeSpecifierOrTypeName));
        cmajor::parsing::NonterminalParser* typeConstructionNonterminalParser = GetNonterminal(ToUtf32("typeConstruction"));
        typeConstructionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostfixExpressionRule>(this, &PostfixExpressionRule::PosttypeConstruction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->pe.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromPrimaryExpression);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new IndexExpr(context->pe.release(), context->fromindex));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new InvokeExpr(context->pe.release(), context->frominvoke));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new MemberAccessExpr(context->pe.release(), context->frommember));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PtrMemberAccessExpr(context->pe.release(), context->fromptrmember));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PostIncrementExpr(context->pe.release()));
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new PostDecrementExpr(context->pe.release()));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new InvokeExpr(context->fromTypeSpecifierOrTypeName, context->fromtypeConstruction));
    }
    void PostPrimaryExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrimaryExpression_value = std::move(stack.top());
            context->fromPrimaryExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromPrimaryExpression_value.get());
            stack.pop();
        }
    }
    void Postindex(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromindex_value = std::move(stack.top());
            context->fromindex = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromindex_value.get());
            stack.pop();
        }
    }
    void Postinvoke(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frominvoke_value = std::move(stack.top());
            context->frominvoke = *static_cast<cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>*>(frominvoke_value.get());
            stack.pop();
        }
    }
    void Postmember(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frommember_value = std::move(stack.top());
            context->frommember = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::IdExpr*>*>(frommember_value.get());
            stack.pop();
        }
    }
    void Postptrmember(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromptrmember_value = std::move(stack.top());
            context->fromptrmember = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::IdExpr*>*>(fromptrmember_value.get());
            stack.pop();
        }
    }
    void PostTypeSpecifierOrTypeName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeSpecifierOrTypeName_value = std::move(stack.top());
            context->fromTypeSpecifierOrTypeName = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromTypeSpecifierOrTypeName_value.get());
            stack.pop();
        }
    }
    void PosttypeConstruction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromtypeConstruction_value = std::move(stack.top());
            context->fromtypeConstruction = *static_cast<cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>*>(fromtypeConstruction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), pe(), fromPrimaryExpression(), fromindex(), frominvoke(), frommember(), fromptrmember(), fromTypeSpecifierOrTypeName(), fromtypeConstruction() {}
        cmajor::codedom::CppObject* value;
        std::unique_ptr<CppObject> pe;
        cmajor::codedom::CppObject* fromPrimaryExpression;
        cmajor::codedom::CppObject* fromindex;
        std::vector<cmajor::codedom::CppObject*> frominvoke;
        cmajor::codedom::IdExpr* frommember;
        cmajor::codedom::IdExpr* fromptrmember;
        cmajor::codedom::CppObject* fromTypeSpecifierOrTypeName;
        std::vector<cmajor::codedom::CppObject*> fromtypeConstruction;
    };
};

class Expression::TypeSpecifierOrTypeNameRule : public cmajor::parsing::Rule
{
public:
    TypeSpecifierOrTypeNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::A1Action));
        cmajor::parsing::NonterminalParser* simpleTypeSpecifierNonterminalParser = GetNonterminal(ToUtf32("SimpleTypeSpecifier"));
        simpleTypeSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostSimpleTypeSpecifier));
        cmajor::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeSpecifierOrTypeNameRule>(this, &TypeSpecifierOrTypeNameRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleTypeSpecifier;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeName;
    }
    void PostSimpleTypeSpecifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSimpleTypeSpecifier_value = std::move(stack.top());
            context->fromSimpleTypeSpecifier = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeSpecifier*>*>(fromSimpleTypeSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypeName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeName_value = std::move(stack.top());
            context->fromTypeName = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeName*>*>(fromTypeName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromSimpleTypeSpecifier(), fromTypeName() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::TypeSpecifier* fromSimpleTypeSpecifier;
        cmajor::codedom::TypeName* fromTypeName;
    };
};

class Expression::PostCastExpressionRule : public cmajor::parsing::Rule
{
public:
    PostCastExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PostCastExpressionRule>(this, &PostCastExpressionRule::A4Action));
        cmajor::parsing::NonterminalParser* dctNonterminalParser = GetNonterminal(ToUtf32("dct"));
        dctNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdct));
        cmajor::parsing::NonterminalParser* dceNonterminalParser = GetNonterminal(ToUtf32("dce"));
        dceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postdce));
        cmajor::parsing::NonterminalParser* sctNonterminalParser = GetNonterminal(ToUtf32("sct"));
        sctNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsct));
        cmajor::parsing::NonterminalParser* sceNonterminalParser = GetNonterminal(ToUtf32("sce"));
        sceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postsce));
        cmajor::parsing::NonterminalParser* rctNonterminalParser = GetNonterminal(ToUtf32("rct"));
        rctNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrct));
        cmajor::parsing::NonterminalParser* rceNonterminalParser = GetNonterminal(ToUtf32("rce"));
        rceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postrce));
        cmajor::parsing::NonterminalParser* cctNonterminalParser = GetNonterminal(ToUtf32("cct"));
        cctNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcct));
        cmajor::parsing::NonterminalParser* cceNonterminalParser = GetNonterminal(ToUtf32("cce"));
        cceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::Postcce));
        cmajor::parsing::NonterminalParser* typeidExprNonterminalParser = GetNonterminal(ToUtf32("typeidExpr"));
        typeidExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PostCastExpressionRule>(this, &PostCastExpressionRule::PosttypeidExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"dynamic_cast", context->fromdct, context->fromdce);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"static_cast", context->fromsct, context->fromsce);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"reinterpret_cast", context->fromrct, context->fromrce);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PostCastExpr(U"const_cast", context->fromcct, context->fromcce);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeIdExpr(context->fromtypeidExpr);
    }
    void Postdct(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromdct_value = std::move(stack.top());
            context->fromdct = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromdct_value.get());
            stack.pop();
        }
    }
    void Postdce(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromdce_value = std::move(stack.top());
            context->fromdce = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromdce_value.get());
            stack.pop();
        }
    }
    void Postsct(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromsct_value = std::move(stack.top());
            context->fromsct = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromsct_value.get());
            stack.pop();
        }
    }
    void Postsce(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromsce_value = std::move(stack.top());
            context->fromsce = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromsce_value.get());
            stack.pop();
        }
    }
    void Postrct(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromrct_value = std::move(stack.top());
            context->fromrct = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromrct_value.get());
            stack.pop();
        }
    }
    void Postrce(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromrce_value = std::move(stack.top());
            context->fromrce = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromrce_value.get());
            stack.pop();
        }
    }
    void Postcct(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromcct_value = std::move(stack.top());
            context->fromcct = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(fromcct_value.get());
            stack.pop();
        }
    }
    void Postcce(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromcce_value = std::move(stack.top());
            context->fromcce = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromcce_value.get());
            stack.pop();
        }
    }
    void PosttypeidExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromtypeidExpr_value = std::move(stack.top());
            context->fromtypeidExpr = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromtypeidExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromdct(), fromdce(), fromsct(), fromsce(), fromrct(), fromrce(), fromcct(), fromcce(), fromtypeidExpr() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::TypeId* fromdct;
        cmajor::codedom::CppObject* fromdce;
        cmajor::codedom::TypeId* fromsct;
        cmajor::codedom::CppObject* fromsce;
        cmajor::codedom::TypeId* fromrct;
        cmajor::codedom::CppObject* fromrce;
        cmajor::codedom::TypeId* fromcct;
        cmajor::codedom::CppObject* fromcce;
        cmajor::codedom::CppObject* fromtypeidExpr;
    };
};

class Expression::ExpressionListRule : public cmajor::parsing::Rule
{
public:
    ExpressionListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::vector<cmajor::codedom::CppObject*>"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::vector<cmajor::codedom::CppObject*>>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExpressionListRule>(this, &ExpressionListRule::A0Action));
        cmajor::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExpressionListRule>(this, &ExpressionListRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.push_back(context->fromAssignmentExpression);
    }
    void PostAssignmentExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAssignmentExpression_value = std::move(stack.top());
            context->fromAssignmentExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromAssignmentExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        std::vector<cmajor::codedom::CppObject*> value;
        cmajor::codedom::CppObject* fromAssignmentExpression;
    };
};

class Expression::PrimaryExpressionRule : public cmajor::parsing::Rule
{
public:
    PrimaryExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::CppObject*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("pe")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExpressionRule>(this, &PrimaryExpressionRule::A4Action));
        cmajor::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostLiteral));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostExpression));
        cmajor::parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal(ToUtf32("IdExpression"));
        idExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExpressionRule>(this, &PrimaryExpressionRule::PostIdExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->pe.release();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromLiteral);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(new ThisAccess);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromExpression);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->pe.reset(context->fromIdExpression);
    }
    void PostLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Literal*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpression_value = std::move(stack.top());
            context->fromExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromExpression_value.get());
            stack.pop();
        }
    }
    void PostIdExpression(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdExpression_value = std::move(stack.top());
            context->fromIdExpression = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::IdExpr*>*>(fromIdExpression_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), pe(), fromLiteral(), fromExpression(), fromIdExpression() {}
        cmajor::codedom::CppObject* value;
        std::unique_ptr<CppObject> pe;
        cmajor::codedom::Literal* fromLiteral;
        cmajor::codedom::CppObject* fromExpression;
        cmajor::codedom::IdExpr* fromIdExpression;
    };
};

class Expression::IdExpressionRule : public cmajor::parsing::Rule
{
public:
    IdExpressionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::IdExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::IdExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IdExpressionRule>(this, &IdExpressionRule::A2Action));
        cmajor::parsing::NonterminalParser* id1NonterminalParser = GetNonterminal(ToUtf32("id1"));
        id1NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid1));
        cmajor::parsing::NonterminalParser* id2NonterminalParser = GetNonterminal(ToUtf32("id2"));
        id2NonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IdExpressionRule>(this, &IdExpressionRule::Postid2));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdExpr(std::u32string(matchBegin, matchEnd));
    }
    void Postid1(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromid1_value = std::move(stack.top());
            context->fromid1 = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromid1_value.get());
            stack.pop();
        }
    }
    void Postid2(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromid2_value = std::move(stack.top());
            context->fromid2 = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromid2_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromid1(), fromid2() {}
        cmajor::codedom::IdExpr* value;
        std::u32string fromid1;
        std::u32string fromid2;
    };
};

void Expression::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.code.Literal"));
    if (!grammar0)
    {
        grammar0 = cmajor::code::Literal::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.code.Identifier"));
    if (!grammar1)
    {
        grammar1 = cmajor::code::Identifier::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.code.Declarator"));
    if (!grammar2)
    {
        grammar2 = cmajor::code::Declarator::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.code.Declaration"));
    if (!grammar3)
    {
        grammar3 = cmajor::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void Expression::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Literal"), this, ToUtf32("Literal.Literal")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Identifier.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("Declarator.TypeId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeName"), this, ToUtf32("Declaration.TypeName")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeSpecifierSeq"), this, ToUtf32("Declarator.TypeSpecifierSeq")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("PtrOperator"), this, ToUtf32("Declarator.PtrOperator")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("SimpleTypeSpecifier"), this, ToUtf32("Declaration.SimpleTypeSpecifier")));
    AddRule(new ExpressionRule(ToUtf32("Expression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AssignmentExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser(','),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AssignmentExpression"), 0))))))));
    AddRule(new ConstantExpressionRule(ToUtf32("ConstantExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))));
    AddRule(new AssignmentExpressionRule(ToUtf32("AssignmentExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("LogicalOrExpression"), ToUtf32("LogicalOrExpression"), 0)),
                                new cmajor::parsing::NonterminalParser(ToUtf32("AssingmentOp"), ToUtf32("AssingmentOp"), 0)),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ConditionalExpression"), ToUtf32("ConditionalExpression"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::NonterminalParser(ToUtf32("ThrowExpression"), ToUtf32("ThrowExpression"), 0)))));
    AddRule(new AssingmentOpRule(ToUtf32("AssingmentOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
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
                                                        new cmajor::parsing::CharParser('='),
                                                        new cmajor::parsing::StringParser(ToUtf32("*="))),
                                                    new cmajor::parsing::StringParser(ToUtf32("/="))),
                                                new cmajor::parsing::StringParser(ToUtf32("%="))),
                                            new cmajor::parsing::StringParser(ToUtf32("+="))),
                                        new cmajor::parsing::StringParser(ToUtf32("-="))),
                                    new cmajor::parsing::StringParser(ToUtf32(">>="))),
                                new cmajor::parsing::StringParser(ToUtf32("<<="))),
                            new cmajor::parsing::StringParser(ToUtf32("&="))),
                        new cmajor::parsing::StringParser(ToUtf32("^="))),
                    new cmajor::parsing::StringParser(ToUtf32("|=")))))));
    AddRule(new ThrowExpressionRule(ToUtf32("ThrowExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("throw")),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)))))));
    AddRule(new ConditionalExpressionRule(ToUtf32("ConditionalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("LogicalOrExpression"), 0)),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::CharParser('?'),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                                    new cmajor::parsing::CharParser(':')),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0))))))))));
    AddRule(new LogicalOrExpressionRule(ToUtf32("LogicalOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("LogicalAndExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::StringParser(ToUtf32("||")),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("LogicalAndExpression"), 0)))))))));
    AddRule(new LogicalAndExpressionRule(ToUtf32("LogicalAndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("InclusiveOrExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::StringParser(ToUtf32("&&")),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("InclusiveOrExpression"), 0)))))))));
    AddRule(new InclusiveOrExpressionRule(ToUtf32("InclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ExclusiveOrExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::DifferenceParser(
                                new cmajor::parsing::CharParser('|'),
                                new cmajor::parsing::StringParser(ToUtf32("||")))),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ExclusiveOrExpression"), 0)))))))));
    AddRule(new ExclusiveOrExpressionRule(ToUtf32("ExclusiveOrExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AndExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('^'),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AndExpression"), 0)))))))));
    AddRule(new AndExpressionRule(ToUtf32("AndExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("EqualityExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::DifferenceParser(
                                new cmajor::parsing::CharParser('&'),
                                new cmajor::parsing::StringParser(ToUtf32("&&")))),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("EqualityExpression"), 0)))))))));
    AddRule(new EqualityExpressionRule(ToUtf32("EqualityExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("RelationalExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("EqOp"), ToUtf32("EqOp"), 0),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelationalExpression"), 0)))))))));
    AddRule(new EqOpRule(ToUtf32("EqOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::StringParser(ToUtf32("=="))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::StringParser(ToUtf32("!="))))));
    AddRule(new RelationalExpressionRule(ToUtf32("RelationalExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ShiftExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("RelOp"), ToUtf32("RelOp"), 0),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ShiftExpression"), 0)))))))));
    AddRule(new RelOpRule(ToUtf32("RelOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::StringParser(ToUtf32("<="))),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::StringParser(ToUtf32(">=")))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::DifferenceParser(
                            new cmajor::parsing::CharParser('<'),
                            new cmajor::parsing::StringParser(ToUtf32("<<")))))),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::CharParser('>'),
                        new cmajor::parsing::StringParser(ToUtf32(">>"))))))));
    AddRule(new ShiftExpressionRule(ToUtf32("ShiftExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AdditiveExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("ShiftOp"), ToUtf32("ShiftOp"), 0),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AdditiveExpression"), 0)))))))));
    AddRule(new ShiftOpRule(ToUtf32("ShiftOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::StringParser(ToUtf32("<<")),
                        new cmajor::parsing::StringParser(ToUtf32("<<="))))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::StringParser(ToUtf32(">>")),
                        new cmajor::parsing::StringParser(ToUtf32(">>="))))))));
    AddRule(new AdditiveExpressionRule(ToUtf32("AdditiveExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("MultiplicativeExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("AddOp"), ToUtf32("AddOp"), 0),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("MultiplicativeExpression"), 0)))))))));
    AddRule(new AddOpRule(ToUtf32("AddOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::CharParser('+'),
                        new cmajor::parsing::StringParser(ToUtf32("+="))))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::CharParser('-'),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::StringParser(ToUtf32("-=")),
                                new cmajor::parsing::StringParser(ToUtf32("->"))))))))));
    AddRule(new MultiplicativeExpressionRule(ToUtf32("MultiplicativeExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("PmExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("MulOp"), ToUtf32("MulOp"), 0),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("PmExpression"), 0)))))))));
    AddRule(new MulOpRule(ToUtf32("MulOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::DifferenceParser(
                            new cmajor::parsing::CharParser('*'),
                            new cmajor::parsing::StringParser(ToUtf32("*="))))),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::DifferenceParser(
                            new cmajor::parsing::CharParser('/'),
                            new cmajor::parsing::StringParser(ToUtf32("/=")))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::CharParser('%'),
                        new cmajor::parsing::StringParser(ToUtf32("%="))))))));
    AddRule(new PmExpressionRule(ToUtf32("PmExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("CastExpression"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("PmOp"), ToUtf32("PmOp"), 0),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("CastExpression"), 0)))))))));
    AddRule(new PmOpRule(ToUtf32("PmOp"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::StringParser(ToUtf32(".*"))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::StringParser(ToUtf32("->*"))))));
    AddRule(new CastExpressionRule(ToUtf32("CastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::CharParser('('),
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                                    new cmajor::parsing::CharParser(')')),
                                new cmajor::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0)))),
                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("UnaryExpression"), ToUtf32("UnaryExpression"), 0)))))));
    AddRule(new UnaryExpressionRule(ToUtf32("UnaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::AlternativeParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("PostfixExpression"), ToUtf32("PostfixExpression"), 0)),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("PostCastExpression"), ToUtf32("PostCastExpression"), 0)))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::GroupingParser(
                                                    new cmajor::parsing::SequenceParser(
                                                        new cmajor::parsing::StringParser(ToUtf32("++")),
                                                        new cmajor::parsing::ExpectationParser(
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("e1"), ToUtf32("UnaryExpression"), 0)))))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::StringParser(ToUtf32("--")),
                                                    new cmajor::parsing::ExpectationParser(
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("e2"), ToUtf32("UnaryExpression"), 0)))))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::NonterminalParser(ToUtf32("UnaryOperator"), ToUtf32("UnaryOperator"), 0),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0)))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::KeywordParser(ToUtf32("sizeof")),
                                                    new cmajor::parsing::CharParser('(')),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::CharParser(')')))))),
                            new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::KeywordParser(ToUtf32("sizeof")),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("e3"), ToUtf32("UnaryExpression"), 0)))))),
                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("NewExpression"), ToUtf32("NewExpression"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A9"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("DeleteExpression"), ToUtf32("DeleteExpression"), 0)))))));
    AddRule(new UnaryOperatorRule(ToUtf32("UnaryOperator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::DifferenceParser(
                                        new cmajor::parsing::CharParser('*'),
                                        new cmajor::parsing::StringParser(ToUtf32("*="))))),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::DifferenceParser(
                                        new cmajor::parsing::CharParser('&'),
                                        new cmajor::parsing::StringParser(ToUtf32("&=")))))),
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::DifferenceParser(
                                    new cmajor::parsing::CharParser('+'),
                                    new cmajor::parsing::StringParser(ToUtf32("+=")))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::DifferenceParser(
                                new cmajor::parsing::CharParser('-'),
                                new cmajor::parsing::StringParser(ToUtf32("-=")))))),
                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::DifferenceParser(
                            new cmajor::parsing::CharParser('!'),
                            new cmajor::parsing::StringParser(ToUtf32("!=")))))),
            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                new cmajor::parsing::CharParser('~')))));
    AddRule(new NewExpressionRule(ToUtf32("NewExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::OptionalParser(
                                    new cmajor::parsing::GroupingParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::StringParser(ToUtf32("::"))))),
                                new cmajor::parsing::KeywordParser(ToUtf32("new"))),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("NewPlacement"), ToUtf32("NewPlacement"), 0))),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("NewTypeId"), ToUtf32("NewTypeId"), 0)),
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::CharParser('('),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0))),
                                    new cmajor::parsing::CharParser(')'))))),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("NewInitializer"), ToUtf32("NewInitializer"), 0)))))));
    AddRule(new NewTypeIdRule(ToUtf32("NewTypeId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("TypeSpecifierSeq"), ToUtf32("TypeSpecifierSeq"), 1),
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("NewDeclarator"), ToUtf32("NewDeclarator"), 0))))))))));
    AddRule(new NewDeclaratorRule(ToUtf32("NewDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("PtrOperator"), ToUtf32("PtrOperator"), 0),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("NewDeclarator"), ToUtf32("NewDeclarator"), 0))),
                    new cmajor::parsing::NonterminalParser(ToUtf32("DirectNewDeclarator"), ToUtf32("DirectNewDeclarator"), 0))))));
    AddRule(new DirectNewDeclaratorRule(ToUtf32("DirectNewDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('['),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0))),
                new cmajor::parsing::CharParser(']')),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('['),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))),
                        new cmajor::parsing::CharParser(']')))))));
    AddRule(new NewPlacementRule(ToUtf32("NewPlacement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('('),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0)),
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new NewInitializerRule(ToUtf32("NewInitializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('('),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0))),
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new DeleteExpressionRule(ToUtf32("DeleteExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                        new cmajor::parsing::StringParser(ToUtf32("::"))))),
                            new cmajor::parsing::KeywordParser(ToUtf32("delete"))),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('['),
                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                        new cmajor::parsing::CharParser(']')))))),
                    new cmajor::parsing::NonterminalParser(ToUtf32("CastExpression"), ToUtf32("CastExpression"), 0))))));
    AddRule(new PostfixExpressionRule(ToUtf32("PostfixExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("PrimaryExpression"), ToUtf32("PrimaryExpression"), 0)),
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::AlternativeParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                        new cmajor::parsing::GroupingParser(
                                                            new cmajor::parsing::SequenceParser(
                                                                new cmajor::parsing::SequenceParser(
                                                                    new cmajor::parsing::CharParser('['),
                                                                    new cmajor::parsing::ExpectationParser(
                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("index"), ToUtf32("Expression"), 0))),
                                                                new cmajor::parsing::ExpectationParser(
                                                                    new cmajor::parsing::CharParser(']'))))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                        new cmajor::parsing::GroupingParser(
                                                            new cmajor::parsing::SequenceParser(
                                                                new cmajor::parsing::SequenceParser(
                                                                    new cmajor::parsing::CharParser('('),
                                                                    new cmajor::parsing::OptionalParser(
                                                                        new cmajor::parsing::GroupingParser(
                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("invoke"), ToUtf32("ExpressionList"), 0)))),
                                                                new cmajor::parsing::ExpectationParser(
                                                                    new cmajor::parsing::CharParser(')')))))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::SequenceParser(
                                                            new cmajor::parsing::GroupingParser(
                                                                new cmajor::parsing::DifferenceParser(
                                                                    new cmajor::parsing::CharParser('.'),
                                                                    new cmajor::parsing::StringParser(ToUtf32(".*")))),
                                                            new cmajor::parsing::ExpectationParser(
                                                                new cmajor::parsing::NonterminalParser(ToUtf32("member"), ToUtf32("IdExpression"), 0)))))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                new cmajor::parsing::GroupingParser(
                                                    new cmajor::parsing::SequenceParser(
                                                        new cmajor::parsing::GroupingParser(
                                                            new cmajor::parsing::DifferenceParser(
                                                                new cmajor::parsing::StringParser(ToUtf32("->")),
                                                                new cmajor::parsing::StringParser(ToUtf32("->*")))),
                                                        new cmajor::parsing::ExpectationParser(
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("ptrmember"), ToUtf32("IdExpression"), 0)))))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                            new cmajor::parsing::StringParser(ToUtf32("++")))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                        new cmajor::parsing::StringParser(ToUtf32("--"))))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A8"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("TypeSpecifierOrTypeName"), ToUtf32("TypeSpecifierOrTypeName"), 0),
                                        new cmajor::parsing::CharParser('(')),
                                    new cmajor::parsing::OptionalParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("typeConstruction"), ToUtf32("ExpressionList"), 0))),
                                new cmajor::parsing::CharParser(')')))))))));
    AddRule(new TypeSpecifierOrTypeNameRule(ToUtf32("TypeSpecifierOrTypeName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("SimpleTypeSpecifier"), ToUtf32("SimpleTypeSpecifier"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))));
    AddRule(new PostCastExpressionRule(ToUtf32("PostCastExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::SequenceParser(
                                                        new cmajor::parsing::KeywordParser(ToUtf32("dynamic_cast")),
                                                        new cmajor::parsing::ExpectationParser(
                                                            new cmajor::parsing::CharParser('<'))),
                                                    new cmajor::parsing::ExpectationParser(
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("dct"), ToUtf32("TypeId"), 0))),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::CharParser('>'))),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::CharParser('('))),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("dce"), ToUtf32("Expression"), 0))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::CharParser(')'))))),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::SequenceParser(
                                                        new cmajor::parsing::KeywordParser(ToUtf32("static_cast")),
                                                        new cmajor::parsing::ExpectationParser(
                                                            new cmajor::parsing::CharParser('<'))),
                                                    new cmajor::parsing::ExpectationParser(
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("sct"), ToUtf32("TypeId"), 0))),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::CharParser('>'))),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::CharParser('('))),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("sce"), ToUtf32("Expression"), 0))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::CharParser(')')))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::SequenceParser(
                                                    new cmajor::parsing::KeywordParser(ToUtf32("reinterpret_cast")),
                                                    new cmajor::parsing::ExpectationParser(
                                                        new cmajor::parsing::CharParser('<'))),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("rct"), ToUtf32("TypeId"), 0))),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::CharParser('>'))),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::CharParser('('))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("rce"), ToUtf32("Expression"), 0))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser(')')))))),
                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::KeywordParser(ToUtf32("const_cast")),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::CharParser('<'))),
                                            new cmajor::parsing::ExpectationParser(
                                                new cmajor::parsing::NonterminalParser(ToUtf32("cct"), ToUtf32("TypeId"), 0))),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::CharParser('>'))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::CharParser('('))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("cce"), ToUtf32("Expression"), 0))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser(')')))))),
            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("typeid")),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser('('))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("typeidExpr"), ToUtf32("Expression"), 0))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser(')'))))))));
    AddRule(new ExpressionListRule(ToUtf32("ExpressionList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ListParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)),
            new cmajor::parsing::CharParser(','))));
    AddRule(new PrimaryExpressionRule(ToUtf32("PrimaryExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0)),
                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                new cmajor::parsing::KeywordParser(ToUtf32("this")))),
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('('),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 0)),
                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser(')'))))),
                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("IdExpression"), ToUtf32("IdExpression"), 0)))))));
    AddRule(new IdExpressionRule(ToUtf32("IdExpression"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::TokenParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("id1"), ToUtf32("QualifiedId"), 0),
                                new cmajor::parsing::StringParser(ToUtf32("::"))),
                            new cmajor::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0)))),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::TokenParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::StringParser(ToUtf32("::"))),
                            new cmajor::parsing::NonterminalParser(ToUtf32("OperatorFunctionId"), ToUtf32("OperatorFunctionId"), 0))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("id2"), ToUtf32("QualifiedId"), 0)))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("OperatorFunctionId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::KeywordParser(ToUtf32("operator")),
            new cmajor::parsing::NonterminalParser(ToUtf32("Operator"), ToUtf32("Operator"), 0))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Operator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                                                                            new cmajor::parsing::SequenceParser(
                                                                                                                                                                                new cmajor::parsing::CharParser('['),
                                                                                                                                                                                new cmajor::parsing::CharParser(']')),
                                                                                                                                                                            new cmajor::parsing::SequenceParser(
                                                                                                                                                                                new cmajor::parsing::CharParser('('),
                                                                                                                                                                                new cmajor::parsing::CharParser(')'))),
                                                                                                                                                                        new cmajor::parsing::StringParser(ToUtf32("->*"))),
                                                                                                                                                                    new cmajor::parsing::StringParser(ToUtf32("->"))),
                                                                                                                                                                new cmajor::parsing::CharParser(',')),
                                                                                                                                                            new cmajor::parsing::StringParser(ToUtf32("--"))),
                                                                                                                                                        new cmajor::parsing::StringParser(ToUtf32("++"))),
                                                                                                                                                    new cmajor::parsing::StringParser(ToUtf32("||"))),
                                                                                                                                                new cmajor::parsing::StringParser(ToUtf32("&&"))),
                                                                                                                                            new cmajor::parsing::StringParser(ToUtf32("<="))),
                                                                                                                                        new cmajor::parsing::StringParser(ToUtf32(">="))),
                                                                                                                                    new cmajor::parsing::StringParser(ToUtf32("!="))),
                                                                                                                                new cmajor::parsing::StringParser(ToUtf32("=="))),
                                                                                                                            new cmajor::parsing::StringParser(ToUtf32("<<="))),
                                                                                                                        new cmajor::parsing::StringParser(ToUtf32(">>="))),
                                                                                                                    new cmajor::parsing::StringParser(ToUtf32("<<"))),
                                                                                                                new cmajor::parsing::StringParser(ToUtf32(">>"))),
                                                                                                            new cmajor::parsing::StringParser(ToUtf32("|="))),
                                                                                                        new cmajor::parsing::StringParser(ToUtf32("&="))),
                                                                                                    new cmajor::parsing::StringParser(ToUtf32("^="))),
                                                                                                new cmajor::parsing::StringParser(ToUtf32("%="))),
                                                                                            new cmajor::parsing::StringParser(ToUtf32("/="))),
                                                                                        new cmajor::parsing::StringParser(ToUtf32("*="))),
                                                                                    new cmajor::parsing::StringParser(ToUtf32("-="))),
                                                                                new cmajor::parsing::StringParser(ToUtf32("+="))),
                                                                            new cmajor::parsing::CharParser('<')),
                                                                        new cmajor::parsing::CharParser('>')),
                                                                    new cmajor::parsing::CharParser('=')),
                                                                new cmajor::parsing::CharParser('!')),
                                                            new cmajor::parsing::CharParser('~')),
                                                        new cmajor::parsing::CharParser('|')),
                                                    new cmajor::parsing::CharParser('&')),
                                                new cmajor::parsing::CharParser('^')),
                                            new cmajor::parsing::CharParser('%')),
                                        new cmajor::parsing::CharParser('/')),
                                    new cmajor::parsing::CharParser('*')),
                                new cmajor::parsing::CharParser('-')),
                            new cmajor::parsing::CharParser('+')),
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("new")),
                                new cmajor::parsing::CharParser('[')),
                            new cmajor::parsing::CharParser(']'))),
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::KeywordParser(ToUtf32("delete")),
                            new cmajor::parsing::CharParser('[')),
                        new cmajor::parsing::CharParser(']'))),
                new cmajor::parsing::KeywordParser(ToUtf32("new"))),
            new cmajor::parsing::KeywordParser(ToUtf32("delete")))));
}

} } // namespace cmajor.code
