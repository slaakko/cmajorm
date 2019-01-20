#include "XPath.hpp"
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

namespace cmajor { namespace xpath {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

XPath* XPath::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

XPath* XPath::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    XPath* grammar(new XPath(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

XPath::XPath(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("XPath"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.xpath")), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back(ToUtf32("ancestor"));
    keywords0.push_back(ToUtf32("ancestor-or-self"));
    keywords0.push_back(ToUtf32("attribute"));
    keywords0.push_back(ToUtf32("child"));
    keywords0.push_back(ToUtf32("descendant"));
    keywords0.push_back(ToUtf32("descendant-or-self"));
    keywords0.push_back(ToUtf32("following"));
    keywords0.push_back(ToUtf32("following-sibling"));
    keywords0.push_back(ToUtf32("namespace"));
    keywords0.push_back(ToUtf32("parent"));
    keywords0.push_back(ToUtf32("preceding"));
    keywords0.push_back(ToUtf32("preceding-sibling"));
    keywords0.push_back(ToUtf32("self"));
}

XPathExpr* XPath::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    XPathExpr* result = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(value.get());
    stack.pop();
    return result;
}

class XPath::ExprRule : public cmajor::parsing::Rule
{
public:
    ExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ExprRule>(this, &ExprRule::A0Action));
        cmajor::parsing::NonterminalParser* orExprNonterminalParser = GetNonterminal(ToUtf32("OrExpr"));
        orExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ExprRule>(this, &ExprRule::PostOrExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOrExpr;
    }
    void PostOrExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOrExpr_value = std::move(stack.top());
            context->fromOrExpr = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromOrExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromOrExpr() {}
        XPathExpr* value;
        XPathExpr* fromOrExpr;
    };
};

class XPath::OrExprRule : public cmajor::parsing::Rule
{
public:
    OrExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OrExprRule>(this, &OrExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OrExprRule>(this, &OrExprRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<OrExprRule>(this, &OrExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<OrExprRule>(this, &OrExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathOrExpr(context->value, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::AndExprRule : public cmajor::parsing::Rule
{
public:
    AndExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AndExprRule>(this, &AndExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AndExprRule>(this, &AndExprRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AndExprRule>(this, &AndExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AndExprRule>(this, &AndExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathAndExpr(context->value, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::EqualityExprRule : public cmajor::parsing::Rule
{
public:
    EqualityExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EqualityExprRule>(this, &EqualityExprRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EqualityExprRule>(this, &EqualityExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EqualityExprRule>(this, &EqualityExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::equal;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::notEqual;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::equal: context->value = new XPathEqualExpr(context->value, context->fromright);
            break;
            case Operator::notEqual: context->value = new XPathNotEqualExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::RelationalExprRule : public cmajor::parsing::Rule
{
public:
    RelationalExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelationalExprRule>(this, &RelationalExprRule::A5Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalExprRule>(this, &RelationalExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelationalExprRule>(this, &RelationalExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::lessOrEqual;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::greaterOrEqual;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::less;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::greater;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::less: context->value = new XPathLessExpr(context->value, context->fromright);
            break;
            case Operator::greater: context->value = new XPathGreaterExpr(context->value, context->fromright);
            break;
            case Operator::lessOrEqual: context->value = new XPathLessOrEqualExpr(context->value, context->fromright);
            break;
            case Operator::greaterOrEqual: context->value = new XPathGreaterOrEqualExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::AdditiveExprRule : public cmajor::parsing::Rule
{
public:
    AdditiveExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AdditiveExprRule>(this, &AdditiveExprRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AdditiveExprRule>(this, &AdditiveExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AdditiveExprRule>(this, &AdditiveExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::plus;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::minus;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::plus: context->value = new XPathAddExpr(context->value, context->fromright);
            break;
            case Operator::minus: context->value = new XPathSubExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::MultiplicativeExprRule : public cmajor::parsing::Rule
{
public:
    MultiplicativeExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiplicativeExprRule>(this, &MultiplicativeExprRule::A4Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiplicativeExprRule>(this, &MultiplicativeExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiplicativeExprRule>(this, &MultiplicativeExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::mul;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::div;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::mod;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::mul: context->value = new XPathMulExpr(context->value, context->fromright);
            break;
            case Operator::div: context->value = new XPathDivExpr(context->value, context->fromright);
            break;
            case Operator::mod: context->value = new XPathModExpr(context->value, context->fromright);
            break;
        }
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::UnaryExprRule : public cmajor::parsing::Rule
{
public:
    UnaryExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExprRule>(this, &UnaryExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnaryExprRule>(this, &UnaryExprRule::A1Action));
        cmajor::parsing::NonterminalParser* subjectNonterminalParser = GetNonterminal(ToUtf32("subject"));
        subjectNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExprRule>(this, &UnaryExprRule::Postsubject));
        cmajor::parsing::NonterminalParser* unionExprNonterminalParser = GetNonterminal(ToUtf32("unionExpr"));
        unionExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnaryExprRule>(this, &UnaryExprRule::PostunionExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathUnaryMinusExpr(context->fromsubject);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromunionExpr;
    }
    void Postsubject(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromsubject_value = std::move(stack.top());
            context->fromsubject = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromsubject_value.get());
            stack.pop();
        }
    }
    void PostunionExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromunionExpr_value = std::move(stack.top());
            context->fromunionExpr = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromunionExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromsubject(), fromunionExpr() {}
        XPathExpr* value;
        XPathExpr* fromsubject;
        XPathExpr* fromunionExpr;
    };
};

class XPath::UnionExprRule : public cmajor::parsing::Rule
{
public:
    UnionExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnionExprRule>(this, &UnionExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnionExprRule>(this, &UnionExprRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnionExprRule>(this, &UnionExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnionExprRule>(this, &UnionExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathUnionExpr(context->value, context->fromright);
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromleft(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromleft;
        XPathExpr* fromright;
    };
};

class XPath::PathExprRule : public cmajor::parsing::Rule
{
public:
    PathExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<XPathExpr>"), ToUtf32("fnc")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PathExprRule>(this, &PathExprRule::A5Action));
        cmajor::parsing::NonterminalParser* locationPathNonterminalParser = GetNonterminal(ToUtf32("LocationPath"));
        locationPathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::PostLocationPath));
        cmajor::parsing::NonterminalParser* functionCallNonterminalParser = GetNonterminal(ToUtf32("FunctionCall"));
        functionCallNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::PostFunctionCall));
        cmajor::parsing::NonterminalParser* filterExprNonterminalParser = GetNonterminal(ToUtf32("FilterExpr"));
        filterExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::PostFilterExpr));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PathExprRule>(this, &PathExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLocationPath;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fnc.reset(context->fromFunctionCall);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFilterExpr;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slashSlash;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slash;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::slash: context->value = new XPathCombineStepExpr(context->value, context->fromright);
            break;
            case Operator::slashSlash: context->value = new XPathCombineStepExpr(context->value, new XPathCombineStepExpr(new XPathLocationStepExpr(Axis::descendantOrSelf, new XPathAnyNodeTest), context->fromright));
            break;
        }
    }
    void PostLocationPath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLocationPath_value = std::move(stack.top());
            context->fromLocationPath = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromLocationPath_value.get());
            stack.pop();
        }
    }
    void PostFunctionCall(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFunctionCall_value = std::move(stack.top());
            context->fromFunctionCall = *static_cast<cmajor::parsing::ValueObject<XPathFunctionCall*>*>(fromFunctionCall_value.get());
            stack.pop();
        }
    }
    void PostFilterExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFilterExpr_value = std::move(stack.top());
            context->fromFilterExpr = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromFilterExpr_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), op(), fnc(), fromLocationPath(), fromFunctionCall(), fromFilterExpr(), fromright() {}
        XPathExpr* value;
        Operator op;
        std::unique_ptr<XPathExpr> fnc;
        XPathExpr* fromLocationPath;
        XPathFunctionCall* fromFunctionCall;
        XPathExpr* fromFilterExpr;
        XPathExpr* fromright;
    };
};

class XPath::FilterExprRule : public cmajor::parsing::Rule
{
public:
    FilterExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FilterExprRule>(this, &FilterExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FilterExprRule>(this, &FilterExprRule::A1Action));
        cmajor::parsing::NonterminalParser* primaryNonterminalParser = GetNonterminal(ToUtf32("primary"));
        primaryNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FilterExprRule>(this, &FilterExprRule::Postprimary));
        cmajor::parsing::NonterminalParser* predicateNonterminalParser = GetNonterminal(ToUtf32("predicate"));
        predicateNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FilterExprRule>(this, &FilterExprRule::Postpredicate));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromprimary;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathFilterExpr(context->value, context->frompredicate);
    }
    void Postprimary(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromprimary_value = std::move(stack.top());
            context->fromprimary = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromprimary_value.get());
            stack.pop();
        }
    }
    void Postpredicate(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frompredicate_value = std::move(stack.top());
            context->frompredicate = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(frompredicate_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromprimary(), frompredicate() {}
        XPathExpr* value;
        XPathExpr* fromprimary;
        XPathExpr* frompredicate;
    };
};

class XPath::LocationPathRule : public cmajor::parsing::Rule
{
public:
    LocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LocationPathRule>(this, &LocationPathRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LocationPathRule>(this, &LocationPathRule::A1Action));
        cmajor::parsing::NonterminalParser* absoluteLocationPathNonterminalParser = GetNonterminal(ToUtf32("AbsoluteLocationPath"));
        absoluteLocationPathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LocationPathRule>(this, &LocationPathRule::PostAbsoluteLocationPath));
        cmajor::parsing::NonterminalParser* relativeLocationPathNonterminalParser = GetNonterminal(ToUtf32("RelativeLocationPath"));
        relativeLocationPathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LocationPathRule>(this, &LocationPathRule::PostRelativeLocationPath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbsoluteLocationPath;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRelativeLocationPath;
    }
    void PostAbsoluteLocationPath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbsoluteLocationPath_value = std::move(stack.top());
            context->fromAbsoluteLocationPath = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromAbsoluteLocationPath_value.get());
            stack.pop();
        }
    }
    void PostRelativeLocationPath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromRelativeLocationPath_value = std::move(stack.top());
            context->fromRelativeLocationPath = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromRelativeLocationPath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromAbsoluteLocationPath(), fromRelativeLocationPath() {}
        XPathExpr* value;
        XPathExpr* fromAbsoluteLocationPath;
        XPathExpr* fromRelativeLocationPath;
    };
};

class XPath::AbsoluteLocationPathRule : public cmajor::parsing::Rule
{
public:
    AbsoluteLocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::A2Action));
        cmajor::parsing::NonterminalParser* abbreviatedAbsoluteLocationPathNonterminalParser = GetNonterminal(ToUtf32("AbbreviatedAbsoluteLocationPath"));
        abbreviatedAbsoluteLocationPathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::PostAbbreviatedAbsoluteLocationPath));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AbsoluteLocationPathRule>(this, &AbsoluteLocationPathRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbbreviatedAbsoluteLocationPath;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathRootNodeExpr;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathCombineStepExpr(context->value, context->fromright);
    }
    void PostAbbreviatedAbsoluteLocationPath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbbreviatedAbsoluteLocationPath_value = std::move(stack.top());
            context->fromAbbreviatedAbsoluteLocationPath = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromAbbreviatedAbsoluteLocationPath_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromAbbreviatedAbsoluteLocationPath(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromAbbreviatedAbsoluteLocationPath;
        XPathExpr* fromright;
    };
};

class XPath::RelativeLocationPathRule : public cmajor::parsing::Rule
{
public:
    RelativeLocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Operator"), ToUtf32("op")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RelativeLocationPathRule>(this, &RelativeLocationPathRule::A3Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelativeLocationPathRule>(this, &RelativeLocationPathRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RelativeLocationPathRule>(this, &RelativeLocationPathRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slashSlash;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->op = Operator::slash;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        switch (context->op)
        {
            case Operator::slash: context->value = new XPathCombineStepExpr(context->value, context->fromright);
            break;
            case Operator::slashSlash: context->value = new XPathCombineStepExpr(context->value, new XPathCombineStepExpr(new XPathLocationStepExpr(Axis::descendantOrSelf, new XPathAnyNodeTest), context->fromright));
            break;
        }
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<XPathLocationStepExpr*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathLocationStepExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), op(), fromleft(), fromright() {}
        XPathExpr* value;
        Operator op;
        XPathLocationStepExpr* fromleft;
        XPathLocationStepExpr* fromright;
    };
};

class XPath::AbbreviatedAbsoluteLocationPathRule : public cmajor::parsing::Rule
{
public:
    AbbreviatedAbsoluteLocationPathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbbreviatedAbsoluteLocationPathRule>(this, &AbbreviatedAbsoluteLocationPathRule::A0Action));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AbbreviatedAbsoluteLocationPathRule>(this, &AbbreviatedAbsoluteLocationPathRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathCombineStepExpr(new XPathRootNodeExpr, new XPathCombineStepExpr(new XPathLocationStepExpr(Axis::descendantOrSelf, new XPathAnyNodeTest), context->fromright));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromright() {}
        XPathExpr* value;
        XPathExpr* fromright;
    };
};

class XPath::StepRule : public cmajor::parsing::Rule
{
public:
    StepRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathLocationStepExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathLocationStepExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StepRule>(this, &StepRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StepRule>(this, &StepRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StepRule>(this, &StepRule::A2Action));
        cmajor::parsing::NonterminalParser* axisNonterminalParser = GetNonterminal(ToUtf32("axis"));
        axisNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StepRule>(this, &StepRule::Postaxis));
        cmajor::parsing::NonterminalParser* nodeTestNonterminalParser = GetNonterminal(ToUtf32("nodeTest"));
        nodeTestNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StepRule>(this, &StepRule::PostnodeTest));
        cmajor::parsing::NonterminalParser* predicateNonterminalParser = GetNonterminal(ToUtf32("predicate"));
        predicateNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StepRule>(this, &StepRule::Postpredicate));
        cmajor::parsing::NonterminalParser* abbreviatedStepNonterminalParser = GetNonterminal(ToUtf32("AbbreviatedStep"));
        abbreviatedStepNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StepRule>(this, &StepRule::PostAbbreviatedStep));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLocationStepExpr(context->fromaxis, context->fromnodeTest);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddPredicate(context->frompredicate);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbbreviatedStep;
    }
    void Postaxis(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromaxis_value = std::move(stack.top());
            context->fromaxis = *static_cast<cmajor::parsing::ValueObject<Axis>*>(fromaxis_value.get());
            stack.pop();
        }
    }
    void PostnodeTest(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromnodeTest_value = std::move(stack.top());
            context->fromnodeTest = *static_cast<cmajor::parsing::ValueObject<XPathNodeTestExpr*>*>(fromnodeTest_value.get());
            stack.pop();
        }
    }
    void Postpredicate(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frompredicate_value = std::move(stack.top());
            context->frompredicate = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(frompredicate_value.get());
            stack.pop();
        }
    }
    void PostAbbreviatedStep(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbbreviatedStep_value = std::move(stack.top());
            context->fromAbbreviatedStep = *static_cast<cmajor::parsing::ValueObject<XPathLocationStepExpr*>*>(fromAbbreviatedStep_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromaxis(), fromnodeTest(), frompredicate(), fromAbbreviatedStep() {}
        XPathLocationStepExpr* value;
        Axis fromaxis;
        XPathNodeTestExpr* fromnodeTest;
        XPathExpr* frompredicate;
        XPathLocationStepExpr* fromAbbreviatedStep;
    };
};

class XPath::AbbreviatedStepRule : public cmajor::parsing::Rule
{
public:
    AbbreviatedStepRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathLocationStepExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathLocationStepExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbbreviatedStepRule>(this, &AbbreviatedStepRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbbreviatedStepRule>(this, &AbbreviatedStepRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLocationStepExpr(Axis::parent, new XPathAnyNodeTest);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLocationStepExpr(Axis::self, new XPathAnyNodeTest);
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        XPathLocationStepExpr* value;
    };
};

class XPath::AxisSpecifierRule : public cmajor::parsing::Rule
{
public:
    AxisSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Axis"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Axis>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxisSpecifierRule>(this, &AxisSpecifierRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxisSpecifierRule>(this, &AxisSpecifierRule::A1Action));
        cmajor::parsing::NonterminalParser* axisNameNonterminalParser = GetNonterminal(ToUtf32("AxisName"));
        axisNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AxisSpecifierRule>(this, &AxisSpecifierRule::PostAxisName));
        cmajor::parsing::NonterminalParser* abbreviatedAxisSpecifierNonterminalParser = GetNonterminal(ToUtf32("AbbreviatedAxisSpecifier"));
        abbreviatedAxisSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AxisSpecifierRule>(this, &AxisSpecifierRule::PostAbbreviatedAxisSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAxisName;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAbbreviatedAxisSpecifier;
    }
    void PostAxisName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAxisName_value = std::move(stack.top());
            context->fromAxisName = *static_cast<cmajor::parsing::ValueObject<Axis>*>(fromAxisName_value.get());
            stack.pop();
        }
    }
    void PostAbbreviatedAxisSpecifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbbreviatedAxisSpecifier_value = std::move(stack.top());
            context->fromAbbreviatedAxisSpecifier = *static_cast<cmajor::parsing::ValueObject<Axis>*>(fromAbbreviatedAxisSpecifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromAxisName(), fromAbbreviatedAxisSpecifier() {}
        Axis value;
        Axis fromAxisName;
        Axis fromAbbreviatedAxisSpecifier;
    };
};

class XPath::AxisNameRule : public cmajor::parsing::Rule
{
public:
    AxisNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Axis"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Axis>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxisNameRule>(this, &AxisNameRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = GetAxis(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Axis value;
    };
};

class XPath::AbbreviatedAxisSpecifierRule : public cmajor::parsing::Rule
{
public:
    AbbreviatedAxisSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Axis"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Axis>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbbreviatedAxisSpecifierRule>(this, &AbbreviatedAxisSpecifierRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbbreviatedAxisSpecifierRule>(this, &AbbreviatedAxisSpecifierRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Axis::attribute;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Axis::child;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Axis value;
    };
};

class XPath::PredicateRule : public cmajor::parsing::Rule
{
public:
    PredicateRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PredicateRule>(this, &PredicateRule::A0Action));
        cmajor::parsing::NonterminalParser* predicateExprNonterminalParser = GetNonterminal(ToUtf32("PredicateExpr"));
        predicateExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PredicateRule>(this, &PredicateRule::PostPredicateExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPredicateExpr;
    }
    void PostPredicateExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPredicateExpr_value = std::move(stack.top());
            context->fromPredicateExpr = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromPredicateExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromPredicateExpr() {}
        XPathExpr* value;
        XPathExpr* fromPredicateExpr;
    };
};

class XPath::PredicateExprRule : public cmajor::parsing::Rule
{
public:
    PredicateExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PredicateExprRule>(this, &PredicateExprRule::A0Action));
        cmajor::parsing::NonterminalParser* exprNonterminalParser = GetNonterminal(ToUtf32("Expr"));
        exprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PredicateExprRule>(this, &PredicateExprRule::PostExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpr;
    }
    void PostExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpr_value = std::move(stack.top());
            context->fromExpr = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExpr() {}
        XPathExpr* value;
        XPathExpr* fromExpr;
    };
};

class XPath::NodeTestRule : public cmajor::parsing::Rule
{
public:
    NodeTestRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathNodeTestExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathNodeTestExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NodeTestRule>(this, &NodeTestRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NodeTestRule>(this, &NodeTestRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NodeTestRule>(this, &NodeTestRule::A2Action));
        cmajor::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NodeTestRule>(this, &NodeTestRule::PostLiteral));
        cmajor::parsing::NonterminalParser* nodeTypeNonterminalParser = GetNonterminal(ToUtf32("NodeType"));
        nodeTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NodeTestRule>(this, &NodeTestRule::PostNodeType));
        cmajor::parsing::NonterminalParser* nameTestNonterminalParser = GetNonterminal(ToUtf32("NameTest"));
        nameTestNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NodeTestRule>(this, &NodeTestRule::PostNameTest));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPILiteralTest(context->fromLiteral);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNodeType;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNameTest;
    }
    void PostLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostNodeType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNodeType_value = std::move(stack.top());
            context->fromNodeType = *static_cast<cmajor::parsing::ValueObject<XPathNodeTestExpr*>*>(fromNodeType_value.get());
            stack.pop();
        }
    }
    void PostNameTest(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNameTest_value = std::move(stack.top());
            context->fromNameTest = *static_cast<cmajor::parsing::ValueObject<XPathNodeTestExpr*>*>(fromNameTest_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromLiteral(), fromNodeType(), fromNameTest() {}
        XPathNodeTestExpr* value;
        XPathExpr* fromLiteral;
        XPathNodeTestExpr* fromNodeType;
        XPathNodeTestExpr* fromNameTest;
    };
};

class XPath::NameTestRule : public cmajor::parsing::Rule
{
public:
    NameTestRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathNodeTestExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathNodeTestExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NameTestRule>(this, &NameTestRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NameTestRule>(this, &NameTestRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NameTestRule>(this, &NameTestRule::A2Action));
        cmajor::parsing::NonterminalParser* nCNameNonterminalParser = GetNonterminal(ToUtf32("NCName"));
        nCNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NameTestRule>(this, &NameTestRule::PostNCName));
        cmajor::parsing::NonterminalParser* qNameNonterminalParser = GetNonterminal(ToUtf32("QName"));
        qNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NameTestRule>(this, &NameTestRule::PostQName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPrincipalNodeTest;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPrefixTest(context->fromNCName);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathNameTest(context->fromQName);
    }
    void PostNCName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNCName_value = std::move(stack.top());
            context->fromNCName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromNCName_value.get());
            stack.pop();
        }
    }
    void PostQName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQName_value = std::move(stack.top());
            context->fromQName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromQName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromNCName(), fromQName() {}
        XPathNodeTestExpr* value;
        std::u32string fromNCName;
        std::u32string fromQName;
    };
};

class XPath::NodeTypeRule : public cmajor::parsing::Rule
{
public:
    NodeTypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathNodeTestExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathNodeTestExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NodeTypeRule>(this, &NodeTypeRule::A3Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathCommentNodeTest;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathTextNodeTest;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathPINodeTest;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathAnyNodeTest;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        XPathNodeTestExpr* value;
    };
};

class XPath::PrimaryExprRule : public cmajor::parsing::Rule
{
public:
    PrimaryExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryExprRule>(this, &PrimaryExprRule::A4Action));
        cmajor::parsing::NonterminalParser* functionCallNonterminalParser = GetNonterminal(ToUtf32("FunctionCall"));
        functionCallNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostFunctionCall));
        cmajor::parsing::NonterminalParser* variableReferenceNonterminalParser = GetNonterminal(ToUtf32("VariableReference"));
        variableReferenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostVariableReference));
        cmajor::parsing::NonterminalParser* exprNonterminalParser = GetNonterminal(ToUtf32("Expr"));
        exprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostExpr));
        cmajor::parsing::NonterminalParser* literalNonterminalParser = GetNonterminal(ToUtf32("Literal"));
        literalNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostLiteral));
        cmajor::parsing::NonterminalParser* numberNonterminalParser = GetNonterminal(ToUtf32("Number"));
        numberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryExprRule>(this, &PrimaryExprRule::PostNumber));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunctionCall;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromVariableReference;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpr;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLiteral;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNumber;
    }
    void PostFunctionCall(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFunctionCall_value = std::move(stack.top());
            context->fromFunctionCall = *static_cast<cmajor::parsing::ValueObject<XPathFunctionCall*>*>(fromFunctionCall_value.get());
            stack.pop();
        }
    }
    void PostVariableReference(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromVariableReference_value = std::move(stack.top());
            context->fromVariableReference = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromVariableReference_value.get());
            stack.pop();
        }
    }
    void PostExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpr_value = std::move(stack.top());
            context->fromExpr = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromExpr_value.get());
            stack.pop();
        }
    }
    void PostLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLiteral_value = std::move(stack.top());
            context->fromLiteral = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromLiteral_value.get());
            stack.pop();
        }
    }
    void PostNumber(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNumber_value = std::move(stack.top());
            context->fromNumber = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromNumber_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromFunctionCall(), fromVariableReference(), fromExpr(), fromLiteral(), fromNumber() {}
        XPathExpr* value;
        XPathFunctionCall* fromFunctionCall;
        XPathExpr* fromVariableReference;
        XPathExpr* fromExpr;
        XPathExpr* fromLiteral;
        XPathExpr* fromNumber;
    };
};

class XPath::VariableReferenceRule : public cmajor::parsing::Rule
{
public:
    VariableReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<VariableReferenceRule>(this, &VariableReferenceRule::A0Action));
        cmajor::parsing::NonterminalParser* qNameNonterminalParser = GetNonterminal(ToUtf32("QName"));
        qNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<VariableReferenceRule>(this, &VariableReferenceRule::PostQName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathVariableReference(context->fromQName);
    }
    void PostQName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQName_value = std::move(stack.top());
            context->fromQName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromQName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromQName() {}
        XPathExpr* value;
        std::u32string fromQName;
    };
};

class XPath::LiteralRule : public cmajor::parsing::Rule
{
public:
    LiteralRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LiteralRule>(this, &LiteralRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLiteral(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathLiteral(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        XPathExpr* value;
    };
};

class XPath::NumberRule : public cmajor::parsing::Rule
{
public:
    NumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathNumberExpr(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathNumberExpr(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        XPathExpr* value;
    };
};

class XPath::FunctionCallRule : public cmajor::parsing::Rule
{
public:
    FunctionCallRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathFunctionCall*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathFunctionCall*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionCallRule>(this, &FunctionCallRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionCallRule>(this, &FunctionCallRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionCallRule>(this, &FunctionCallRule::A2Action));
        cmajor::parsing::NonterminalParser* functionNameNonterminalParser = GetNonterminal(ToUtf32("FunctionName"));
        functionNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionCallRule>(this, &FunctionCallRule::PostFunctionName));
        cmajor::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionCallRule>(this, &FunctionCallRule::Postfirst));
        cmajor::parsing::NonterminalParser* nextNonterminalParser = GetNonterminal(ToUtf32("next"));
        nextNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionCallRule>(this, &FunctionCallRule::Postnext));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new XPathFunctionCall(context->fromFunctionName);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddArgument(context->fromfirst);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddArgument(context->fromnext);
    }
    void PostFunctionName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFunctionName_value = std::move(stack.top());
            context->fromFunctionName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromFunctionName_value.get());
            stack.pop();
        }
    }
    void Postfirst(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postnext(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromnext_value = std::move(stack.top());
            context->fromnext = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromnext_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromFunctionName(), fromfirst(), fromnext() {}
        XPathFunctionCall* value;
        std::u32string fromFunctionName;
        XPathExpr* fromfirst;
        XPathExpr* fromnext;
    };
};

class XPath::ArgumentRule : public cmajor::parsing::Rule
{
public:
    ArgumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("XPathExpr*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<XPathExpr*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ArgumentRule>(this, &ArgumentRule::A0Action));
        cmajor::parsing::NonterminalParser* exprNonterminalParser = GetNonterminal(ToUtf32("Expr"));
        exprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ArgumentRule>(this, &ArgumentRule::PostExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromExpr;
    }
    void PostExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromExpr_value = std::move(stack.top());
            context->fromExpr = *static_cast<cmajor::parsing::ValueObject<XPathExpr*>*>(fromExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromExpr() {}
        XPathExpr* value;
        XPathExpr* fromExpr;
    };
};

class XPath::FunctionNameRule : public cmajor::parsing::Rule
{
public:
    FunctionNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<XPathExpr>"), ToUtf32("nodeType")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionNameRule>(this, &FunctionNameRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionNameRule>(this, &FunctionNameRule::A1Action));
        cmajor::parsing::NonterminalParser* qNameNonterminalParser = GetNonterminal(ToUtf32("QName"));
        qNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionNameRule>(this, &FunctionNameRule::PostQName));
        cmajor::parsing::NonterminalParser* nodeTypeNonterminalParser = GetNonterminal(ToUtf32("NodeType"));
        nodeTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionNameRule>(this, &FunctionNameRule::PostNodeType));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromQName;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->nodeType.reset(context->fromNodeType);
    }
    void PostQName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQName_value = std::move(stack.top());
            context->fromQName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromQName_value.get());
            stack.pop();
        }
    }
    void PostNodeType(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNodeType_value = std::move(stack.top());
            context->fromNodeType = *static_cast<cmajor::parsing::ValueObject<XPathNodeTestExpr*>*>(fromNodeType_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), nodeType(), fromQName(), fromNodeType() {}
        std::u32string value;
        std::unique_ptr<XPathExpr> nodeType;
        std::u32string fromQName;
        XPathNodeTestExpr* fromNodeType;
    };
};

class XPath::QNameRule : public cmajor::parsing::Rule
{
public:
    QNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<QNameRule>(this, &QNameRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<QNameRule>(this, &QNameRule::A1Action));
        cmajor::parsing::NonterminalParser* prefixedNameNonterminalParser = GetNonterminal(ToUtf32("PrefixedName"));
        prefixedNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<QNameRule>(this, &QNameRule::PostPrefixedName));
        cmajor::parsing::NonterminalParser* unprefixedNameNonterminalParser = GetNonterminal(ToUtf32("UnprefixedName"));
        unprefixedNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<QNameRule>(this, &QNameRule::PostUnprefixedName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrefixedName;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnprefixedName;
    }
    void PostPrefixedName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrefixedName_value = std::move(stack.top());
            context->fromPrefixedName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromPrefixedName_value.get());
            stack.pop();
        }
    }
    void PostUnprefixedName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUnprefixedName_value = std::move(stack.top());
            context->fromUnprefixedName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromUnprefixedName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromPrefixedName(), fromUnprefixedName() {}
        std::u32string value;
        std::u32string fromPrefixedName;
        std::u32string fromUnprefixedName;
    };
};

class XPath::PrefixedNameRule : public cmajor::parsing::Rule
{
public:
    PrefixedNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixedNameRule>(this, &PrefixedNameRule::A0Action));
        cmajor::parsing::NonterminalParser* prefixNonterminalParser = GetNonterminal(ToUtf32("Prefix"));
        prefixNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrefixedNameRule>(this, &PrefixedNameRule::PostPrefix));
        cmajor::parsing::NonterminalParser* localPartNonterminalParser = GetNonterminal(ToUtf32("LocalPart"));
        localPartNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrefixedNameRule>(this, &PrefixedNameRule::PostLocalPart));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = std::u32string(matchBegin, matchEnd);
    }
    void PostPrefix(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrefix_value = std::move(stack.top());
            context->fromPrefix = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromPrefix_value.get());
            stack.pop();
        }
    }
    void PostLocalPart(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLocalPart_value = std::move(stack.top());
            context->fromLocalPart = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromLocalPart_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromPrefix(), fromLocalPart() {}
        std::u32string value;
        std::u32string fromPrefix;
        std::u32string fromLocalPart;
    };
};

class XPath::UnprefixedNameRule : public cmajor::parsing::Rule
{
public:
    UnprefixedNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnprefixedNameRule>(this, &UnprefixedNameRule::A0Action));
        cmajor::parsing::NonterminalParser* localPartNonterminalParser = GetNonterminal(ToUtf32("LocalPart"));
        localPartNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UnprefixedNameRule>(this, &UnprefixedNameRule::PostLocalPart));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLocalPart;
    }
    void PostLocalPart(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLocalPart_value = std::move(stack.top());
            context->fromLocalPart = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromLocalPart_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromLocalPart() {}
        std::u32string value;
        std::u32string fromLocalPart;
    };
};

class XPath::PrefixRule : public cmajor::parsing::Rule
{
public:
    PrefixRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrefixRule>(this, &PrefixRule::A0Action));
        cmajor::parsing::NonterminalParser* nCNameNonterminalParser = GetNonterminal(ToUtf32("NCName"));
        nCNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrefixRule>(this, &PrefixRule::PostNCName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNCName;
    }
    void PostNCName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNCName_value = std::move(stack.top());
            context->fromNCName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromNCName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromNCName() {}
        std::u32string value;
        std::u32string fromNCName;
    };
};

class XPath::LocalPartRule : public cmajor::parsing::Rule
{
public:
    LocalPartRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LocalPartRule>(this, &LocalPartRule::A0Action));
        cmajor::parsing::NonterminalParser* nCNameNonterminalParser = GetNonterminal(ToUtf32("NCName"));
        nCNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<LocalPartRule>(this, &LocalPartRule::PostNCName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNCName;
    }
    void PostNCName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNCName_value = std::move(stack.top());
            context->fromNCName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromNCName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromNCName() {}
        std::u32string value;
        std::u32string fromNCName;
    };
};

class XPath::NCNameRule : public cmajor::parsing::Rule
{
public:
    NCNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NCNameRule>(this, &NCNameRule::A0Action));
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

void XPath::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.xml.Xml"));
    if (!grammar0)
    {
        grammar0 = cmajor::xml::Xml::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar1)
    {
        grammar1 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void XPath::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("NameStartChar"), this, ToUtf32("cmajor.xml.Xml.NameStartChar")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("NameChar"), this, ToUtf32("cmajor.xml.Xml.NameChar")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("cmajor.parsing.stdlib.identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces"), this, ToUtf32("cmajor.parsing.stdlib.spaces")));
    AddRule(new ExprRule(ToUtf32("Expr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("OrExpr"), ToUtf32("OrExpr"), 0))));
    AddRule(new OrExprRule(ToUtf32("OrExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AndExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("or")),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AndExpr"), 0))))))));
    AddRule(new AndExprRule(ToUtf32("AndExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("EqualityExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("and")),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("EqualityExpr"), 0))))))));
    AddRule(new EqualityExprRule(ToUtf32("EqualityExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("RelationalExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::CharParser('=')),
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::StringParser(ToUtf32("!="))))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelationalExpr"), 0))))))));
    AddRule(new RelationalExprRule(ToUtf32("RelationalExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("AdditiveExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::StringParser(ToUtf32("<="))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::StringParser(ToUtf32(">=")))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                        new cmajor::parsing::CharParser('<'))),
                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                    new cmajor::parsing::CharParser('>')))),
                        new cmajor::parsing::ActionParser(ToUtf32("A5"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("AdditiveExpr"), 0))))))));
    AddRule(new AdditiveExprRule(ToUtf32("AdditiveExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("MultiplicativeExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::CharParser('+')),
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::CharParser('-')))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("MultiplicativeExpr"), 0))))))));
    AddRule(new MultiplicativeExprRule(ToUtf32("MultiplicativeExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("UnaryExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                        new cmajor::parsing::CharParser('*')),
                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                        new cmajor::parsing::StringParser(ToUtf32("div")))),
                                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                    new cmajor::parsing::StringParser(ToUtf32("mod"))))),
                        new cmajor::parsing::ActionParser(ToUtf32("A4"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("UnaryExpr"), 0))))))));
    AddRule(new UnaryExprRule(ToUtf32("UnaryExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('-'),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("subject"), ToUtf32("UnaryExpr"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("unionExpr"), ToUtf32("UnionExpr"), 0)))));
    AddRule(new UnionExprRule(ToUtf32("UnionExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("PathExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('|'),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("PathExpr"), 0))))))));
    AddRule(new PathExprRule(ToUtf32("PathExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::DifferenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("LocationPath"), ToUtf32("LocationPath"), 0),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("FunctionCall"), ToUtf32("FunctionCall"), 0))))),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("FilterExpr"), ToUtf32("FilterExpr"), 0)),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                        new cmajor::parsing::StringParser(ToUtf32("//"))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                        new cmajor::parsing::CharParser('/')))),
                            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelativeLocationPath"), 0)))))))));
    AddRule(new FilterExprRule(ToUtf32("FilterExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("primary"), ToUtf32("PrimaryExpr"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("predicate"), ToUtf32("Predicate"), 0)))))));
    AddRule(new LocationPathRule(ToUtf32("LocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("AbsoluteLocationPath"), ToUtf32("AbsoluteLocationPath"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("RelativeLocationPath"), ToUtf32("RelativeLocationPath"), 0)))));
    AddRule(new AbsoluteLocationPathRule(ToUtf32("AbsoluteLocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("AbbreviatedAbsoluteLocationPath"), ToUtf32("AbbreviatedAbsoluteLocationPath"), 0)),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::CharParser('/')),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelativeLocationPath"), 0))))))));
    AddRule(new RelativeLocationPathRule(ToUtf32("RelativeLocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("Step"), 0)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::StringParser(ToUtf32("//"))),
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::CharParser('/')))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("Step"), 0))))))));
    AddRule(new AbbreviatedAbsoluteLocationPathRule(ToUtf32("AbbreviatedAbsoluteLocationPath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::StringParser(ToUtf32("//")),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("RelativeLocationPath"), 0)))));
    AddRule(new StepRule(ToUtf32("Step"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("axis"), ToUtf32("AxisSpecifier"), 0),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("nodeTest"), ToUtf32("NodeTest"), 0))),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("predicate"), ToUtf32("Predicate"), 0))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("AbbreviatedStep"), ToUtf32("AbbreviatedStep"), 0)))));
    AddRule(new AbbreviatedStepRule(ToUtf32("AbbreviatedStep"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::StringParser(ToUtf32(".."))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::CharParser('.')))));
    AddRule(new AxisSpecifierRule(ToUtf32("AxisSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("AxisName"), ToUtf32("AxisName"), 0),
                        new cmajor::parsing::StringParser(ToUtf32("::"))))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("AbbreviatedAxisSpecifier"), ToUtf32("AbbreviatedAxisSpecifier"), 0)))));
    AddRule(new AxisNameRule(ToUtf32("AxisName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordListParser(ToUtf32("identifier"), keywords0))));
    AddRule(new AbbreviatedAxisSpecifierRule(ToUtf32("AbbreviatedAxisSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::CharParser('@')),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::EmptyParser()))));
    AddRule(new PredicateRule(ToUtf32("Predicate"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('['),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("PredicateExpr"), ToUtf32("PredicateExpr"), 0))),
            new cmajor::parsing::CharParser(']'))));
    AddRule(new PredicateExprRule(ToUtf32("PredicateExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Expr"), ToUtf32("Expr"), 0))));
    AddRule(new NodeTestRule(ToUtf32("NodeTest"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::KeywordParser(ToUtf32("processing-instruction")),
                                    new cmajor::parsing::CharParser('(')),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0)),
                            new cmajor::parsing::CharParser(')')))),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("NodeType"), ToUtf32("NodeType"), 0),
                                new cmajor::parsing::CharParser('(')),
                            new cmajor::parsing::CharParser(')'))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("NameTest"), ToUtf32("NameTest"), 0)))));
    AddRule(new NameTestRule(ToUtf32("NameTest"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser('*')),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("NCName"), ToUtf32("NCName"), 0),
                                new cmajor::parsing::CharParser(':')),
                            new cmajor::parsing::CharParser('*'))))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("QName"), ToUtf32("QName"), 0)))));
    AddRule(new NodeTypeRule(ToUtf32("NodeType"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::KeywordParser(ToUtf32("comment"))),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::KeywordParser(ToUtf32("text")))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::KeywordParser(ToUtf32("processing-instruction")))),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::KeywordParser(ToUtf32("node"))))));
    AddRule(new PrimaryExprRule(ToUtf32("PrimaryExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("FunctionCall"), ToUtf32("FunctionCall"), 0)),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("VariableReference"), ToUtf32("VariableReference"), 0))),
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Expr"), ToUtf32("Expr"), 0)),
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::CharParser(')')))),
                new cmajor::parsing::ActionParser(ToUtf32("A3"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Literal"), ToUtf32("Literal"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Number"), ToUtf32("Number"), 0)))));
    AddRule(new VariableReferenceRule(ToUtf32("VariableReference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('$'),
                    new cmajor::parsing::NonterminalParser(ToUtf32("QName"), ToUtf32("QName"), 0))))));
    AddRule(new LiteralRule(ToUtf32("Literal"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('\"'),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::KleeneStarParser(
                                    new cmajor::parsing::CharSetParser(ToUtf32("\""), true))))),
                    new cmajor::parsing::CharParser('\"'))),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('\''),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::KleeneStarParser(
                                    new cmajor::parsing::CharSetParser(ToUtf32("\'"), true))))),
                    new cmajor::parsing::CharParser('\''))))));
    AddRule(new NumberRule(ToUtf32("Number"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Digits"), ToUtf32("Digits"), 0),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::CharParser('.'),
                                    new cmajor::parsing::OptionalParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("Digits"), ToUtf32("Digits"), 0)))))))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('.'),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Digits"), ToUtf32("Digits"), 0)))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("Digits"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::PositiveParser(
                new cmajor::parsing::CharSetParser(ToUtf32("0-9"))))));
    AddRule(new FunctionCallRule(ToUtf32("FunctionCall"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("FunctionName"), ToUtf32("FunctionName"), 0)),
                        new cmajor::parsing::CharParser('('))),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("Argument"), 0)),
                            new cmajor::parsing::KleeneStarParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::CharParser(','),
                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("next"), ToUtf32("Argument"), 0))))))))),
            new cmajor::parsing::CharParser(')'))));
    AddRule(new ArgumentRule(ToUtf32("Argument"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Expr"), ToUtf32("Expr"), 0))));
    AddRule(new FunctionNameRule(ToUtf32("FunctionName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::DifferenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("QName"), ToUtf32("QName"), 0),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("NodeType"), ToUtf32("NodeType"), 0)))))));
    AddRule(new QNameRule(ToUtf32("QName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("PrefixedName"), ToUtf32("PrefixedName"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("UnprefixedName"), ToUtf32("UnprefixedName"), 0)))));
    AddRule(new PrefixedNameRule(ToUtf32("PrefixedName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Prefix"), ToUtf32("Prefix"), 0),
                        new cmajor::parsing::CharParser(':')),
                    new cmajor::parsing::NonterminalParser(ToUtf32("LocalPart"), ToUtf32("LocalPart"), 0))))));
    AddRule(new UnprefixedNameRule(ToUtf32("UnprefixedName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("LocalPart"), ToUtf32("LocalPart"), 0))));
    AddRule(new PrefixRule(ToUtf32("Prefix"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("NCName"), ToUtf32("NCName"), 0))));
    AddRule(new LocalPartRule(ToUtf32("LocalPart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("NCName"), ToUtf32("NCName"), 0))));
    AddRule(new NCNameRule(ToUtf32("NCName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::DifferenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("NameStartChar"), ToUtf32("NameStartChar"), 0),
                            new cmajor::parsing::CharParser(':'))),
                    new cmajor::parsing::KleeneStarParser(
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::DifferenceParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("NameChar"), ToUtf32("NameChar"), 0),
                                new cmajor::parsing::CharParser(':')))))))));
    SetSkipRuleName(ToUtf32("spaces"));
}

} } // namespace cmajor.xpath
