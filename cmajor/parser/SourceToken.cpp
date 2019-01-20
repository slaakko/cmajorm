#include "SourceToken.hpp"
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
#include <cmajor/parser/Keyword.hpp>
#include <cmajor/parser/Identifier.hpp>
#include <cmajor/parser/Literal.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

SourceToken* SourceToken::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

SourceToken* SourceToken::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    SourceToken* grammar(new SourceToken(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

SourceToken::SourceToken(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("SourceToken"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

void SourceToken::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceTokenFormatter* formatter)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<SourceTokenFormatter*>(formatter)));
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

class SourceToken::SourceTokensRule : public cmajor::parsing::Rule
{
public:
    SourceTokensRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceTokenFormatter*"), ToUtf32("formatter")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> formatter_value = std::move(stack.top());
        context->formatter = *static_cast<cmajor::parsing::ValueObject<SourceTokenFormatter*>*>(formatter_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokensRule>(this, &SourceTokensRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokensRule>(this, &SourceTokensRule::A1Action));
        cmajor::parsing::NonterminalParser* sourceTokenNonterminalParser = GetNonterminal(ToUtf32("SourceToken"));
        sourceTokenNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SourceTokensRule>(this, &SourceTokensRule::PreSourceToken));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->BeginFormat();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->EndFormat();
    }
    void PreSourceToken(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<SourceTokenFormatter*>(context->formatter)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): formatter() {}
        SourceTokenFormatter* formatter;
    };
};

class SourceToken::SourceTokenRule : public cmajor::parsing::Rule
{
public:
    SourceTokenRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("SourceTokenFormatter*"), ToUtf32("formatter")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> formatter_value = std::move(stack.top());
        context->formatter = *static_cast<cmajor::parsing::ValueObject<SourceTokenFormatter*>*>(formatter_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceTokenRule>(this, &SourceTokenRule::A10Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::PostQualifiedId));
        cmajor::parsing::NonterminalParser* floatingLiteralNonterminalParser = GetNonterminal(ToUtf32("FloatingLiteral"));
        floatingLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::PostFloatingLiteral));
        cmajor::parsing::NonterminalParser* integerLiteralNonterminalParser = GetNonterminal(ToUtf32("IntegerLiteral"));
        integerLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::PostIntegerLiteral));
        cmajor::parsing::NonterminalParser* charLiteralNonterminalParser = GetNonterminal(ToUtf32("CharLiteral"));
        charLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::PostCharLiteral));
        cmajor::parsing::NonterminalParser* stringLiteralNonterminalParser = GetNonterminal(ToUtf32("StringLiteral"));
        stringLiteralNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::PostStringLiteral));
        cmajor::parsing::NonterminalParser* spacesNonterminalParser = GetNonterminal(ToUtf32("Spaces"));
        spacesNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::PostSpaces));
        cmajor::parsing::NonterminalParser* otherNonterminalParser = GetNonterminal(ToUtf32("Other"));
        otherNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceTokenRule>(this, &SourceTokenRule::PostOther));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Keyword(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Keyword(std::u32string(matchBegin, matchEnd));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Identifier(std::u32string(matchBegin, matchEnd));
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Number(std::u32string(matchBegin, matchEnd));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Char(std::u32string(matchBegin, matchEnd));
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->String(std::u32string(matchBegin, matchEnd));
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Spaces(context->fromSpaces);
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Comment(std::u32string(matchBegin, matchEnd));
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Comment(std::u32string(matchBegin, matchEnd));
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->NewLine(std::u32string(matchBegin, matchEnd));
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->formatter->Other(context->fromOther);
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
    void PostFloatingLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFloatingLiteral_value = std::move(stack.top());
            context->fromFloatingLiteral = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromFloatingLiteral_value.get());
            stack.pop();
        }
    }
    void PostIntegerLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIntegerLiteral_value = std::move(stack.top());
            context->fromIntegerLiteral = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromIntegerLiteral_value.get());
            stack.pop();
        }
    }
    void PostCharLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCharLiteral_value = std::move(stack.top());
            context->fromCharLiteral = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromCharLiteral_value.get());
            stack.pop();
        }
    }
    void PostStringLiteral(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStringLiteral_value = std::move(stack.top());
            context->fromStringLiteral = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromStringLiteral_value.get());
            stack.pop();
        }
    }
    void PostSpaces(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSpaces_value = std::move(stack.top());
            context->fromSpaces = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromSpaces_value.get());
            stack.pop();
        }
    }
    void PostOther(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOther_value = std::move(stack.top());
            context->fromOther = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromOther_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): formatter(), fromQualifiedId(), fromFloatingLiteral(), fromIntegerLiteral(), fromCharLiteral(), fromStringLiteral(), fromSpaces(), fromOther() {}
        SourceTokenFormatter* formatter;
        IdentifierNode* fromQualifiedId;
        Node* fromFloatingLiteral;
        Node* fromIntegerLiteral;
        Node* fromCharLiteral;
        Node* fromStringLiteral;
        std::u32string fromSpaces;
        std::u32string fromOther;
    };
};

class SourceToken::SpacesRule : public cmajor::parsing::Rule
{
public:
    SpacesRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpacesRule>(this, &SpacesRule::A0Action));
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

class SourceToken::OtherRule : public cmajor::parsing::Rule
{
public:
    OtherRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OtherRule>(this, &OtherRule::A0Action));
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

void SourceToken::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Literal"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Literal::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Keyword"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Keyword::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar3)
    {
        grammar3 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void SourceToken::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Keyword"), this, ToUtf32("Keyword.Keyword")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("PPKeyword"), this, ToUtf32("Keyword.PPKeyword")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Identifier.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("FloatingLiteral"), this, ToUtf32("Literal.FloatingLiteral")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("IntegerLiteral"), this, ToUtf32("Literal.IntegerLiteral")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("block_comment"), this, ToUtf32("cmajor.parsing.stdlib.block_comment")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("CharLiteral"), this, ToUtf32("Literal.CharLiteral")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("StringLiteral"), this, ToUtf32("Literal.StringLiteral")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("cmajor.parsing.stdlib.newline")));
    AddRule(new SourceTokensRule(ToUtf32("SourceTokens"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::EmptyParser()),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("SourceToken"), ToUtf32("SourceToken"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::EmptyParser()))));
    AddRule(new SourceTokenRule(ToUtf32("SourceToken"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("PPKeyword"), ToUtf32("PPKeyword"), 0)),
                                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::AlternativeParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("FloatingLiteral"), ToUtf32("FloatingLiteral"), 0),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("IntegerLiteral"), ToUtf32("IntegerLiteral"), 0))))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("CharLiteral"), ToUtf32("CharLiteral"), 0))),
                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("StringLiteral"), ToUtf32("StringLiteral"), 0))),
                            new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Spaces"), ToUtf32("Spaces"), 0))),
                        new cmajor::parsing::ActionParser(ToUtf32("A7"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("LineComment"), ToUtf32("LineComment"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A8"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("block_comment"), ToUtf32("block_comment"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A9"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A10"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Other"), ToUtf32("Other"), 0)))));
    AddRule(new SpacesRule(ToUtf32("Spaces"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::PositiveParser(
                    new cmajor::parsing::CharSetParser(ToUtf32("\t ")))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("LineComment"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::StringParser(ToUtf32("//")),
                new cmajor::parsing::KleeneStarParser(
                    new cmajor::parsing::CharSetParser(ToUtf32("\r\n"), true))),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))));
    AddRule(new OtherRule(ToUtf32("Other"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::CharSetParser(ToUtf32("\r\n"), true))));
}

} } // namespace cmajor.parser
