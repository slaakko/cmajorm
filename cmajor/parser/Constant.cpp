#include "Constant.hpp"
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

Constant* Constant::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Constant* Constant::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Constant* grammar(new Constant(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Constant::Constant(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Constant"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

ConstantNode* Constant::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    ConstantNode* result = *static_cast<cmajor::parsing::ValueObject<ConstantNode*>*>(value.get());
    stack.pop();
    return result;
}

class Constant::ConstantRule : public cmajor::parsing::Rule
{
public:
    ConstantRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstantNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("strValue")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstantNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstantRule>(this, &ConstantRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstantRule>(this, &ConstantRule::A1Action));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstantRule>(this, &ConstantRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstantRule>(this, &ConstantRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstantRule>(this, &ConstantRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstantRule>(this, &ConstantRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstantRule>(this, &ConstantRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstantRule>(this, &ConstantRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConstantNode(span, context->fromSpecifiers, context->fromTypeExpr, context->fromIdentifier, context->fromExpression);
        context->value->SetStrValue(context->strValue);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->strValue = std::u32string(matchBegin, matchEnd);
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
        Context(): ctx(), value(), strValue(), fromSpecifiers(), fromTypeExpr(), fromIdentifier(), fromExpression() {}
        ParsingContext* ctx;
        ConstantNode* value;
        std::u32string strValue;
        Specifiers fromSpecifiers;
        Node* fromTypeExpr;
        IdentifierNode* fromIdentifier;
        Node* fromExpression;
    };
};

void Constant::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Expression"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Expression::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Specifier"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Specifier::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.TypeExpr"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void Constant::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Specifiers"), this, ToUtf32("Specifier.Specifiers")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRule(new ConstantRule(ToUtf32("Constant"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0),
                                        new cmajor::parsing::KeywordParser(ToUtf32("const"))),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                            new cmajor::parsing::CharParser('=')),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1))),
                    new cmajor::parsing::CharParser(';'))))));
}

} } // namespace cmajor.parser
