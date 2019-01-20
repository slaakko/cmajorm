#include "Concept.hpp"
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
#include <cmajor/ast/Expression.hpp>
#include <cmajor/parser/Specifier.hpp>
#include <cmajor/parser/Identifier.hpp>
#include <cmajor/parser/Parameter.hpp>
#include <cmajor/parser/Expression.hpp>
#include <cmajor/parser/Function.hpp>
#include <cmajor/parser/TypeExpr.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Concept* Concept::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Concept* Concept::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Concept* grammar(new Concept(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Concept::Concept(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Concept"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

ConceptNode* Concept::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    ConceptNode* result = *static_cast<cmajor::parsing::ValueObject<ConceptNode*>*>(value.get());
    stack.pop();
    return result;
}

class Concept::ConceptRule : public cmajor::parsing::Rule
{
public:
    ConceptRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConceptNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConceptNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A1Action));
        a1ActionParser->SetFailureAction(new cmajor::parsing::MemberFailureAction<ConceptRule>(this, &ConceptRule::A1ActionFail));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptRule>(this, &ConceptRule::A8Action));
        cmajor::parsing::NonterminalParser* specifiersNonterminalParser = GetNonterminal(ToUtf32("Specifiers"));
        specifiersNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostSpecifiers));
        cmajor::parsing::NonterminalParser* conceptNameNonterminalParser = GetNonterminal(ToUtf32("conceptName"));
        conceptNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostconceptName));
        cmajor::parsing::NonterminalParser* typeParameterNonterminalParser = GetNonterminal(ToUtf32("typeParameter"));
        typeParameterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PosttypeParameter));
        cmajor::parsing::NonterminalParser* refinementNonterminalParser = GetNonterminal(ToUtf32("Refinement"));
        refinementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostRefinement));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptRule>(this, &ConceptRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptRule>(this, &ConceptRule::PostWhereConstraint));
        cmajor::parsing::NonterminalParser* conceptBodyNonterminalParser = GetNonterminal(ToUtf32("ConceptBody"));
        conceptBodyNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptRule>(this, &ConceptRule::PreConceptBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->BeginParsingConcept();
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingConcept();
        context->value->SetBeginBraceSpan(context->beginBraceSpan);
        context->value->SetEndBraceSpan(context->endBraceSpan);
    }
    void A1ActionFail(ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ctx->EndParsingConcept();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConceptNode(span, context->fromSpecifiers, context->fromconceptName);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddTypeParameter(context->fromtypeParameter);
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->GetSpan().SetEnd(span.End());
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->SetRefinement(context->fromRefinement);
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromWhereConstraint->SetHeaderConstraint();
        context->value->AddConstraint(context->fromWhereConstraint);
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->beginBraceSpan = span;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->endBraceSpan = span;
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
    void PostconceptName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromconceptName_value = std::move(stack.top());
            context->fromconceptName = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromconceptName_value.get());
            stack.pop();
        }
    }
    void PosttypeParameter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromtypeParameter_value = std::move(stack.top());
            context->fromtypeParameter = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(fromtypeParameter_value.get());
            stack.pop();
        }
    }
    void PostRefinement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromRefinement_value = std::move(stack.top());
            context->fromRefinement = *static_cast<cmajor::parsing::ValueObject<ConceptIdNode*>*>(fromRefinement_value.get());
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
    void PreConceptBody(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConceptNode*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), beginBraceSpan(), endBraceSpan(), fromSpecifiers(), fromconceptName(), fromtypeParameter(), fromRefinement(), fromWhereConstraint() {}
        ParsingContext* ctx;
        ConceptNode* value;
        Span beginBraceSpan;
        Span endBraceSpan;
        Specifiers fromSpecifiers;
        IdentifierNode* fromconceptName;
        IdentifierNode* fromtypeParameter;
        ConceptIdNode* fromRefinement;
        WhereConstraintNode* fromWhereConstraint;
    };
};

class Concept::RefinementRule : public cmajor::parsing::Rule
{
public:
    RefinementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("ConceptIdNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConceptIdNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RefinementRule>(this, &RefinementRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RefinementRule>(this, &RefinementRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RefinementRule>(this, &RefinementRule::A2Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RefinementRule>(this, &RefinementRule::PostQualifiedId));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RefinementRule>(this, &RefinementRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConceptIdNode(span, context->fromQualifiedId);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddTypeParameter(context->fromIdentifier);
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
        Context(): value(), fromQualifiedId(), fromIdentifier() {}
        ConceptIdNode* value;
        IdentifierNode* fromQualifiedId;
        IdentifierNode* fromIdentifier;
    };
};

class Concept::ConceptBodyRule : public cmajor::parsing::Rule
{
public:
    ConceptBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ConceptNode*"), ToUtf32("concept")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> concept_value = std::move(stack.top());
        context->concept = *static_cast<cmajor::parsing::ValueObject<ConceptNode*>*>(concept_value.get());
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
        cmajor::parsing::NonterminalParser* conceptBodyConstraintNonterminalParser = GetNonterminal(ToUtf32("ConceptBodyConstraint"));
        conceptBodyConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptBodyRule>(this, &ConceptBodyRule::PreConceptBodyConstraint));
        cmajor::parsing::NonterminalParser* axiomNonterminalParser = GetNonterminal(ToUtf32("Axiom"));
        axiomNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptBodyRule>(this, &ConceptBodyRule::PreAxiom));
    }
    void PreConceptBodyConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConceptNode*>(context->concept)));
    }
    void PreAxiom(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConceptNode*>(context->concept)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), concept() {}
        ParsingContext* ctx;
        ConceptNode* concept;
    };
};

class Concept::ConceptBodyConstraintRule : public cmajor::parsing::Rule
{
public:
    ConceptBodyConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ConceptNode*"), ToUtf32("concept")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> concept_value = std::move(stack.top());
        context->concept = *static_cast<cmajor::parsing::ValueObject<ConceptNode*>*>(concept_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::A2Action));
        cmajor::parsing::NonterminalParser* typeNameConstraintNonterminalParser = GetNonterminal(ToUtf32("TypeNameConstraint"));
        typeNameConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PreTypeNameConstraint));
        typeNameConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PostTypeNameConstraint));
        cmajor::parsing::NonterminalParser* signatureConstraintNonterminalParser = GetNonterminal(ToUtf32("SignatureConstraint"));
        signatureConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PreSignatureConstraint));
        signatureConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PostSignatureConstraint));
        cmajor::parsing::NonterminalParser* embeddedConstraintNonterminalParser = GetNonterminal(ToUtf32("EmbeddedConstraint"));
        embeddedConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PreEmbeddedConstraint));
        embeddedConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptBodyConstraintRule>(this, &ConceptBodyConstraintRule::PostEmbeddedConstraint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->concept->AddConstraint(context->fromTypeNameConstraint);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->concept->AddConstraint(context->fromSignatureConstraint);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->concept->AddConstraint(context->fromEmbeddedConstraint);
    }
    void PreTypeNameConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypeNameConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeNameConstraint_value = std::move(stack.top());
            context->fromTypeNameConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromTypeNameConstraint_value.get());
            stack.pop();
        }
    }
    void PreSignatureConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<IdentifierNode*>(context->concept->TypeParameters()[0])));
    }
    void PostSignatureConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSignatureConstraint_value = std::move(stack.top());
            context->fromSignatureConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromSignatureConstraint_value.get());
            stack.pop();
        }
    }
    void PreEmbeddedConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEmbeddedConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEmbeddedConstraint_value = std::move(stack.top());
            context->fromEmbeddedConstraint = *static_cast<cmajor::parsing::ValueObject<WhereConstraintNode*>*>(fromEmbeddedConstraint_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), concept(), fromTypeNameConstraint(), fromSignatureConstraint(), fromEmbeddedConstraint() {}
        ParsingContext* ctx;
        ConceptNode* concept;
        ConstraintNode* fromTypeNameConstraint;
        ConstraintNode* fromSignatureConstraint;
        WhereConstraintNode* fromEmbeddedConstraint;
    };
};

class Concept::TypeNameConstraintRule : public cmajor::parsing::Rule
{
public:
    TypeNameConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeNameConstraintRule>(this, &TypeNameConstraintRule::A0Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TypeNameConstraintRule>(this, &TypeNameConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeNameConstraintRule>(this, &TypeNameConstraintRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeNameConstraintNode(span, context->fromTypeExpr);
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
        ConstraintNode* value;
        Node* fromTypeExpr;
    };
};

class Concept::SignatureConstraintRule : public cmajor::parsing::Rule
{
public:
    SignatureConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("IdentifierNode*"), ToUtf32("firstTypeParameter")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> firstTypeParameter_value = std::move(stack.top());
        context->firstTypeParameter = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(firstTypeParameter_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SignatureConstraintRule>(this, &SignatureConstraintRule::A3Action));
        cmajor::parsing::NonterminalParser* constructorConstraintNonterminalParser = GetNonterminal(ToUtf32("ConstructorConstraint"));
        constructorConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreConstructorConstraint));
        constructorConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostConstructorConstraint));
        cmajor::parsing::NonterminalParser* destructorConstraintNonterminalParser = GetNonterminal(ToUtf32("DestructorConstraint"));
        destructorConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreDestructorConstraint));
        destructorConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostDestructorConstraint));
        cmajor::parsing::NonterminalParser* memberFunctionConstraintNonterminalParser = GetNonterminal(ToUtf32("MemberFunctionConstraint"));
        memberFunctionConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreMemberFunctionConstraint));
        memberFunctionConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostMemberFunctionConstraint));
        cmajor::parsing::NonterminalParser* functionConstraintNonterminalParser = GetNonterminal(ToUtf32("FunctionConstraint"));
        functionConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PreFunctionConstraint));
        functionConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SignatureConstraintRule>(this, &SignatureConstraintRule::PostFunctionConstraint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConstructorConstraint;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDestructorConstraint;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMemberFunctionConstraint;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunctionConstraint;
    }
    void PreConstructorConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<IdentifierNode*>(context->firstTypeParameter)));
    }
    void PostConstructorConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstructorConstraint_value = std::move(stack.top());
            context->fromConstructorConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromConstructorConstraint_value.get());
            stack.pop();
        }
    }
    void PreDestructorConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<IdentifierNode*>(context->firstTypeParameter)));
    }
    void PostDestructorConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDestructorConstraint_value = std::move(stack.top());
            context->fromDestructorConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromDestructorConstraint_value.get());
            stack.pop();
        }
    }
    void PreMemberFunctionConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostMemberFunctionConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromMemberFunctionConstraint_value = std::move(stack.top());
            context->fromMemberFunctionConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromMemberFunctionConstraint_value.get());
            stack.pop();
        }
    }
    void PreFunctionConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostFunctionConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFunctionConstraint_value = std::move(stack.top());
            context->fromFunctionConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromFunctionConstraint_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), firstTypeParameter(), value(), fromConstructorConstraint(), fromDestructorConstraint(), fromMemberFunctionConstraint(), fromFunctionConstraint() {}
        ParsingContext* ctx;
        IdentifierNode* firstTypeParameter;
        ConstraintNode* value;
        ConstraintNode* fromConstructorConstraint;
        ConstraintNode* fromDestructorConstraint;
        ConstraintNode* fromMemberFunctionConstraint;
        ConstraintNode* fromFunctionConstraint;
    };
};

class Concept::ConstructorConstraintRule : public cmajor::parsing::Rule
{
public:
    ConstructorConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("IdentifierNode*"), ToUtf32("firstTypeParameter")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("id")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<ConstraintNode>"), ToUtf32("ctorConstraint")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> firstTypeParameter_value = std::move(stack.top());
        context->firstTypeParameter = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(firstTypeParameter_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorConstraintRule>(this, &ConstructorConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstructorConstraintRule>(this, &ConstructorConstraintRule::A1Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstructorConstraintRule>(this, &ConstructorConstraintRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstructorConstraintRule>(this, &ConstructorConstraintRule::PreParameterList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->ctorConstraint.release();
        context->value->GetSpan() = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromIdentifier);
        pass = context->fromIdentifier->Str() == context->firstTypeParameter->Str();
        if (pass) context->ctorConstraint.reset(new ConstructorConstraintNode(span, context->id.release()));
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
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->ctorConstraint.get())));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), firstTypeParameter(), value(), id(), ctorConstraint(), fromIdentifier() {}
        ParsingContext* ctx;
        IdentifierNode* firstTypeParameter;
        ConstraintNode* value;
        std::unique_ptr<IdentifierNode> id;
        std::unique_ptr<ConstraintNode> ctorConstraint;
        IdentifierNode* fromIdentifier;
    };
};

class Concept::DestructorConstraintRule : public cmajor::parsing::Rule
{
public:
    DestructorConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("IdentifierNode*"), ToUtf32("firstTypeParameter")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("id")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> firstTypeParameter_value = std::move(stack.top());
        context->firstTypeParameter = *static_cast<cmajor::parsing::ValueObject<IdentifierNode*>*>(firstTypeParameter_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorConstraintRule>(this, &DestructorConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DestructorConstraintRule>(this, &DestructorConstraintRule::A1Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DestructorConstraintRule>(this, &DestructorConstraintRule::PostIdentifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DestructorConstraintNode(span, context->id.release());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromIdentifier);
        pass = context->fromIdentifier->Str() == context->firstTypeParameter->Str();
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
        Context(): ctx(), firstTypeParameter(), value(), id(), fromIdentifier() {}
        ParsingContext* ctx;
        IdentifierNode* firstTypeParameter;
        ConstraintNode* value;
        std::unique_ptr<IdentifierNode> id;
        IdentifierNode* fromIdentifier;
    };
};

class Concept::MemberFunctionConstraintRule : public cmajor::parsing::Rule
{
public:
    MemberFunctionConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Node>"), ToUtf32("returnType")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<IdentifierNode>"), ToUtf32("typeParam")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::A3Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* functionGroupIdNonterminalParser = GetNonterminal(ToUtf32("FunctionGroupId"));
        functionGroupIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PreFunctionGroupId));
        functionGroupIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PostFunctionGroupId));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MemberFunctionConstraintRule>(this, &MemberFunctionConstraintRule::PreParameterList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->GetSpan() = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->returnType.reset(context->fromTypeExpr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeParam.reset(context->fromIdentifier);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MemberFunctionConstraintNode(span, context->returnType.release(), context->typeParam.release(), context->fromFunctionGroupId);
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
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), returnType(), typeParam(), fromTypeExpr(), fromIdentifier(), fromFunctionGroupId() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        std::unique_ptr<Node> returnType;
        std::unique_ptr<IdentifierNode> typeParam;
        Node* fromTypeExpr;
        IdentifierNode* fromIdentifier;
        std::u32string fromFunctionGroupId;
    };
};

class Concept::FunctionConstraintRule : public cmajor::parsing::Rule
{
public:
    FunctionConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionConstraintRule>(this, &FunctionConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionConstraintRule>(this, &FunctionConstraintRule::A1Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* functionGroupIdNonterminalParser = GetNonterminal(ToUtf32("FunctionGroupId"));
        functionGroupIdNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PreFunctionGroupId));
        functionGroupIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PostFunctionGroupId));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionConstraintRule>(this, &FunctionConstraintRule::PreParameterList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->GetSpan() = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new FunctionConstraintNode(span, context->fromTypeExpr, context->fromFunctionGroupId);
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
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromTypeExpr(), fromFunctionGroupId() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        Node* fromTypeExpr;
        std::u32string fromFunctionGroupId;
    };
};

class Concept::EmbeddedConstraintRule : public cmajor::parsing::Rule
{
public:
    EmbeddedConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("WhereConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<WhereConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EmbeddedConstraintRule>(this, &EmbeddedConstraintRule::A0Action));
        cmajor::parsing::NonterminalParser* whereConstraintNonterminalParser = GetNonterminal(ToUtf32("WhereConstraint"));
        whereConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EmbeddedConstraintRule>(this, &EmbeddedConstraintRule::PreWhereConstraint));
        whereConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EmbeddedConstraintRule>(this, &EmbeddedConstraintRule::PostWhereConstraint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromWhereConstraint;
        context->value->SetSemicolon();
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromWhereConstraint() {}
        ParsingContext* ctx;
        WhereConstraintNode* value;
        WhereConstraintNode* fromWhereConstraint;
    };
};

class Concept::WhereConstraintRule : public cmajor::parsing::Rule
{
public:
    WhereConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("WhereConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<WhereConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<WhereConstraintRule>(this, &WhereConstraintRule::A0Action));
        cmajor::parsing::NonterminalParser* constraintExprNonterminalParser = GetNonterminal(ToUtf32("ConstraintExpr"));
        constraintExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<WhereConstraintRule>(this, &WhereConstraintRule::PreConstraintExpr));
        constraintExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<WhereConstraintRule>(this, &WhereConstraintRule::PostConstraintExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new WhereConstraintNode(span, context->fromConstraintExpr);
    }
    void PreConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstraintExpr_value = std::move(stack.top());
            context->fromConstraintExpr = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromConstraintExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromConstraintExpr() {}
        ParsingContext* ctx;
        WhereConstraintNode* value;
        ConstraintNode* fromConstraintExpr;
    };
};

class Concept::ConstraintExprRule : public cmajor::parsing::Rule
{
public:
    ConstraintExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstraintExprRule>(this, &ConstraintExprRule::A0Action));
        cmajor::parsing::NonterminalParser* disjunctiveConstraintExprNonterminalParser = GetNonterminal(ToUtf32("DisjunctiveConstraintExpr"));
        disjunctiveConstraintExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstraintExprRule>(this, &ConstraintExprRule::PreDisjunctiveConstraintExpr));
        disjunctiveConstraintExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstraintExprRule>(this, &ConstraintExprRule::PostDisjunctiveConstraintExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDisjunctiveConstraintExpr;
    }
    void PreDisjunctiveConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDisjunctiveConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDisjunctiveConstraintExpr_value = std::move(stack.top());
            context->fromDisjunctiveConstraintExpr = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromDisjunctiveConstraintExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromDisjunctiveConstraintExpr() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        ConstraintNode* fromDisjunctiveConstraintExpr;
    };
};

class Concept::DisjunctiveConstraintExprRule : public cmajor::parsing::Rule
{
public:
    DisjunctiveConstraintExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DisjunctiveConstraintExprRule>(this, &DisjunctiveConstraintExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = new DisjunctiveConstraintNode(context->s, context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        Span s;
        ConstraintNode* fromleft;
        ConstraintNode* fromright;
    };
};

class Concept::ConjunctiveConstraintExprRule : public cmajor::parsing::Rule
{
public:
    ConjunctiveConstraintExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("s")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::A1Action));
        cmajor::parsing::NonterminalParser* leftNonterminalParser = GetNonterminal(ToUtf32("left"));
        leftNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Preleft));
        leftNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Postleft));
        cmajor::parsing::NonterminalParser* rightNonterminalParser = GetNonterminal(ToUtf32("right"));
        rightNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Preright));
        rightNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConjunctiveConstraintExprRule>(this, &ConjunctiveConstraintExprRule::Postright));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromleft;
        context->s = span;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.SetEnd(span.End());
        context->value = new ConjunctiveConstraintNode(context->s, context->value, context->fromright);
    }
    void Preleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postleft(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromleft_value = std::move(stack.top());
            context->fromleft = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromleft_value.get());
            stack.pop();
        }
    }
    void Preright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void Postright(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromright_value = std::move(stack.top());
            context->fromright = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromright_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), s(), fromleft(), fromright() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        Span s;
        ConstraintNode* fromleft;
        ConstraintNode* fromright;
    };
};

class Concept::PrimaryConstraintExprRule : public cmajor::parsing::Rule
{
public:
    PrimaryConstraintExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::A1Action));
        cmajor::parsing::NonterminalParser* constraintExprNonterminalParser = GetNonterminal(ToUtf32("ConstraintExpr"));
        constraintExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PreConstraintExpr));
        constraintExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PostConstraintExpr));
        cmajor::parsing::NonterminalParser* atomicConstraintExprNonterminalParser = GetNonterminal(ToUtf32("AtomicConstraintExpr"));
        atomicConstraintExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PreAtomicConstraintExpr));
        atomicConstraintExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimaryConstraintExprRule>(this, &PrimaryConstraintExprRule::PostAtomicConstraintExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParenthesizedConstraintNode(span, context->fromConstraintExpr);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAtomicConstraintExpr;
    }
    void PreConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstraintExpr_value = std::move(stack.top());
            context->fromConstraintExpr = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromConstraintExpr_value.get());
            stack.pop();
        }
    }
    void PreAtomicConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostAtomicConstraintExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAtomicConstraintExpr_value = std::move(stack.top());
            context->fromAtomicConstraintExpr = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromAtomicConstraintExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromConstraintExpr(), fromAtomicConstraintExpr() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        ConstraintNode* fromConstraintExpr;
        ConstraintNode* fromAtomicConstraintExpr;
    };
};

class Concept::AtomicConstraintExprRule : public cmajor::parsing::Rule
{
public:
    AtomicConstraintExprRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::A2Action));
        cmajor::parsing::NonterminalParser* predicateConstraintNonterminalParser = GetNonterminal(ToUtf32("PredicateConstraint"));
        predicateConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PrePredicateConstraint));
        predicateConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PostPredicateConstraint));
        cmajor::parsing::NonterminalParser* isConstraintNonterminalParser = GetNonterminal(ToUtf32("IsConstraint"));
        isConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PreIsConstraint));
        isConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PostIsConstraint));
        cmajor::parsing::NonterminalParser* multiParamConstraintNonterminalParser = GetNonterminal(ToUtf32("MultiParamConstraint"));
        multiParamConstraintNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PreMultiParamConstraint));
        multiParamConstraintNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AtomicConstraintExprRule>(this, &AtomicConstraintExprRule::PostMultiParamConstraint));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPredicateConstraint;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIsConstraint;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMultiParamConstraint;
    }
    void PrePredicateConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostPredicateConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPredicateConstraint_value = std::move(stack.top());
            context->fromPredicateConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromPredicateConstraint_value.get());
            stack.pop();
        }
    }
    void PreIsConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostIsConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIsConstraint_value = std::move(stack.top());
            context->fromIsConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromIsConstraint_value.get());
            stack.pop();
        }
    }
    void PreMultiParamConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostMultiParamConstraint(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromMultiParamConstraint_value = std::move(stack.top());
            context->fromMultiParamConstraint = *static_cast<cmajor::parsing::ValueObject<ConstraintNode*>*>(fromMultiParamConstraint_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromPredicateConstraint(), fromIsConstraint(), fromMultiParamConstraint() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        ConstraintNode* fromPredicateConstraint;
        ConstraintNode* fromIsConstraint;
        ConstraintNode* fromMultiParamConstraint;
    };
};

class Concept::PredicateConstraintRule : public cmajor::parsing::Rule
{
public:
    PredicateConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PredicateConstraintRule>(this, &PredicateConstraintRule::A0Action));
        cmajor::parsing::NonterminalParser* invokeExprNonterminalParser = GetNonterminal(ToUtf32("InvokeExpr"));
        invokeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<PredicateConstraintRule>(this, &PredicateConstraintRule::PreInvokeExpr));
        invokeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PredicateConstraintRule>(this, &PredicateConstraintRule::PostInvokeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PredicateConstraintNode(context->fromInvokeExpr->GetSpan(), context->fromInvokeExpr);
    }
    void PreInvokeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInvokeExpr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInvokeExpr_value = std::move(stack.top());
            context->fromInvokeExpr = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromInvokeExpr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromInvokeExpr() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        Node* fromInvokeExpr;
    };
};

class Concept::IsConstraintRule : public cmajor::parsing::Rule
{
public:
    IsConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IsConstraintRule>(this, &IsConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IsConstraintRule>(this, &IsConstraintRule::A1Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IsConstraintRule>(this, &IsConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IsConstraintRule>(this, &IsConstraintRule::PostTypeExpr));
        cmajor::parsing::NonterminalParser* conceptOrTypeNameNonterminalParser = GetNonterminal(ToUtf32("ConceptOrTypeName"));
        conceptOrTypeNameNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IsConstraintRule>(this, &IsConstraintRule::PreConceptOrTypeName));
        conceptOrTypeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IsConstraintRule>(this, &IsConstraintRule::PostConceptOrTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IsConstraintNode(span, context->typeExpr.release(), context->fromConceptOrTypeName);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeExpr.reset(context->fromTypeExpr);
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
    void PreConceptOrTypeName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConceptOrTypeName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConceptOrTypeName_value = std::move(stack.top());
            context->fromConceptOrTypeName = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromConceptOrTypeName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), typeExpr(), fromTypeExpr(), fromConceptOrTypeName() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        std::unique_ptr<Node> typeExpr;
        Node* fromTypeExpr;
        Node* fromConceptOrTypeName;
    };
};

class Concept::ConceptOrTypeNameRule : public cmajor::parsing::Rule
{
public:
    ConceptOrTypeNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptOrTypeNameRule>(this, &ConceptOrTypeNameRule::A0Action));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptOrTypeNameRule>(this, &ConceptOrTypeNameRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptOrTypeNameRule>(this, &ConceptOrTypeNameRule::PostTypeExpr));
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

class Concept::MultiParamConstraintRule : public cmajor::parsing::Rule
{
public:
    MultiParamConstraintRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstraintNode*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<MultiParamConstraintNode>"), ToUtf32("constraint")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ConstraintNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiParamConstraintRule>(this, &MultiParamConstraintRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiParamConstraintRule>(this, &MultiParamConstraintRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MultiParamConstraintRule>(this, &MultiParamConstraintRule::A2Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiParamConstraintRule>(this, &MultiParamConstraintRule::PostQualifiedId));
        cmajor::parsing::NonterminalParser* typeExprNonterminalParser = GetNonterminal(ToUtf32("TypeExpr"));
        typeExprNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<MultiParamConstraintRule>(this, &MultiParamConstraintRule::PreTypeExpr));
        typeExprNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<MultiParamConstraintRule>(this, &MultiParamConstraintRule::PostTypeExpr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->constraint.release();
        context->value->GetSpan().SetEnd(span.End());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->constraint.reset(new MultiParamConstraintNode(span, context->fromQualifiedId));
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->constraint->AddTypeExpr(context->fromTypeExpr);
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
        Context(): ctx(), value(), constraint(), fromQualifiedId(), fromTypeExpr() {}
        ParsingContext* ctx;
        ConstraintNode* value;
        std::unique_ptr<MultiParamConstraintNode> constraint;
        IdentifierNode* fromQualifiedId;
        Node* fromTypeExpr;
    };
};

class Concept::AxiomRule : public cmajor::parsing::Rule
{
public:
    AxiomRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ConceptNode*"), ToUtf32("concept")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<AxiomNode>"), ToUtf32("axiom")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("axiomSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("beginBraceSpan")));
        AddLocalVariable(AttrOrVariable(ToUtf32("Span"), ToUtf32("endBraceSpan")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> concept_value = std::move(stack.top());
        context->concept = *static_cast<cmajor::parsing::ValueObject<ConceptNode*>*>(concept_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomRule>(this, &AxiomRule::A5Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AxiomRule>(this, &AxiomRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* parameterListNonterminalParser = GetNonterminal(ToUtf32("ParameterList"));
        parameterListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AxiomRule>(this, &AxiomRule::PreParameterList));
        cmajor::parsing::NonterminalParser* axiomBodyNonterminalParser = GetNonterminal(ToUtf32("AxiomBody"));
        axiomBodyNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AxiomRule>(this, &AxiomRule::PreAxiomBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->axiom->SetBeginBraceSpan(context->beginBraceSpan);
        context->axiom->SetEndBraceSpan(context->endBraceSpan);
        context->concept->AddAxiom(context->axiom.release());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->axiom->GetSpan().SetEnd(span.End());
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->axiomSpan = span;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->axiom.reset(new AxiomNode(context->axiomSpan, context->fromIdentifier));
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->beginBraceSpan = span;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->endBraceSpan = span;
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
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->axiom.get())));
    }
    void PreAxiomBody(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<AxiomNode*>(context->axiom.get())));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), concept(), axiom(), axiomSpan(), beginBraceSpan(), endBraceSpan(), fromIdentifier() {}
        ParsingContext* ctx;
        ConceptNode* concept;
        std::unique_ptr<AxiomNode> axiom;
        Span axiomSpan;
        Span beginBraceSpan;
        Span endBraceSpan;
        IdentifierNode* fromIdentifier;
    };
};

class Concept::AxiomBodyRule : public cmajor::parsing::Rule
{
public:
    AxiomBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("AxiomNode*"), ToUtf32("axiom")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> axiom_value = std::move(stack.top());
        context->axiom = *static_cast<cmajor::parsing::ValueObject<AxiomNode*>*>(axiom_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomBodyRule>(this, &AxiomBodyRule::A0Action));
        cmajor::parsing::NonterminalParser* axiomStatementNonterminalParser = GetNonterminal(ToUtf32("AxiomStatement"));
        axiomStatementNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AxiomBodyRule>(this, &AxiomBodyRule::PreAxiomStatement));
        axiomStatementNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AxiomBodyRule>(this, &AxiomBodyRule::PostAxiomStatement));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->axiom->AddStatement(context->fromAxiomStatement);
    }
    void PreAxiomStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostAxiomStatement(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAxiomStatement_value = std::move(stack.top());
            context->fromAxiomStatement = *static_cast<cmajor::parsing::ValueObject<AxiomStatementNode*>*>(fromAxiomStatement_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), axiom(), fromAxiomStatement() {}
        ParsingContext* ctx;
        AxiomNode* axiom;
        AxiomStatementNode* fromAxiomStatement;
    };
};

class Concept::AxiomStatementRule : public cmajor::parsing::Rule
{
public:
    AxiomStatementRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("AxiomStatementNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<AxiomStatementNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AxiomStatementRule>(this, &AxiomStatementRule::A0Action));
        cmajor::parsing::NonterminalParser* expressionNonterminalParser = GetNonterminal(ToUtf32("Expression"));
        expressionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<AxiomStatementRule>(this, &AxiomStatementRule::PreExpression));
        expressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AxiomStatementRule>(this, &AxiomStatementRule::PostExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AxiomStatementNode(span, context->fromExpression, std::u32string(matchBegin, matchEnd));
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
        Context(): ctx(), value(), fromExpression() {}
        ParsingContext* ctx;
        AxiomStatementNode* value;
        Node* fromExpression;
    };
};

void Concept::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Expression"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Expression::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Parameter"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Parameter::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Specifier"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Specifier::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parser.TypeExpr"));
    if (!grammar4)
    {
        grammar4 = cmajor::parser::TypeExpr::Create(pd);
    }
    AddGrammarReference(grammar4);
    cmajor::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("cmajor.parser.Function"));
    if (!grammar5)
    {
        grammar5 = cmajor::parser::Function::Create(pd);
    }
    AddGrammarReference(grammar5);
}

void Concept::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("InvokeExpr"), this, ToUtf32("Expression.InvokeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Specifiers"), this, ToUtf32("Specifier.Specifiers")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ParameterList"), this, ToUtf32("Parameter.ParameterList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Identifier.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Expression"), this, ToUtf32("Expression.Expression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeExpr"), this, ToUtf32("TypeExpr.TypeExpr")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("FunctionGroupId"), this, ToUtf32("Function.FunctionGroupId")));
    AddRule(new ConceptRule(ToUtf32("Concept"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::SequenceParser(
                                                            new cmajor::parsing::SequenceParser(
                                                                new cmajor::parsing::NonterminalParser(ToUtf32("Specifiers"), ToUtf32("Specifiers"), 0),
                                                                new cmajor::parsing::KeywordParser(ToUtf32("concept"))),
                                                            new cmajor::parsing::ExpectationParser(
                                                                new cmajor::parsing::NonterminalParser(ToUtf32("conceptName"), ToUtf32("Identifier"), 0))))),
                                                new cmajor::parsing::ExpectationParser(
                                                    new cmajor::parsing::CharParser('<'))),
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::ListParser(
                                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("typeParameter"), ToUtf32("Identifier"), 0)),
                                                    new cmajor::parsing::CharParser(',')))),
                                        new cmajor::parsing::ExpectationParser(
                                            new cmajor::parsing::CharParser('>'))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                        new cmajor::parsing::GroupingParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::OptionalParser(
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Refinement"), ToUtf32("Refinement"), 0)))),
                                                new cmajor::parsing::OptionalParser(
                                                    new cmajor::parsing::GroupingParser(
                                                        new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1)))))))),
                                new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::CharParser('{')))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("ConceptBody"), ToUtf32("ConceptBody"), 2))),
                        new cmajor::parsing::ActionParser(ToUtf32("A8"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('}')))))))));
    AddRule(new RefinementRule(ToUtf32("Refinement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser(':'),
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0)))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('<'))),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::ListParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                                new cmajor::parsing::CharParser(',')))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('>')))))));
    AddRule(new ConceptBodyRule(ToUtf32("ConceptBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("ConceptBodyConstraint"), ToUtf32("ConceptBodyConstraint"), 2),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Axiom"), ToUtf32("Axiom"), 2))))));
    AddRule(new ConceptBodyConstraintRule(ToUtf32("ConceptBodyConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("TypeNameConstraint"), ToUtf32("TypeNameConstraint"), 1)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("SignatureConstraint"), ToUtf32("SignatureConstraint"), 2))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("EmbeddedConstraint"), ToUtf32("EmbeddedConstraint"), 1)))));
    AddRule(new TypeNameConstraintRule(ToUtf32("TypeNameConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("typename")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))),
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new SignatureConstraintRule(ToUtf32("SignatureConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ConstructorConstraint"), ToUtf32("ConstructorConstraint"), 2)),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("DestructorConstraint"), ToUtf32("DestructorConstraint"), 2))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("MemberFunctionConstraint"), ToUtf32("MemberFunctionConstraint"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::NonterminalParser(ToUtf32("FunctionConstraint"), ToUtf32("FunctionConstraint"), 1)))));
    AddRule(new ConstructorConstraintRule(ToUtf32("ConstructorConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("explicit"))),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2)),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new DestructorConstraintRule(ToUtf32("DestructorConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::CharParser('~'),
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('('))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser(')'))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new MemberFunctionConstraintRule(ToUtf32("MemberFunctionConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                                new cmajor::parsing::CharParser('.')),
                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("FunctionGroupId"), ToUtf32("FunctionGroupId"), 1)))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new FunctionConstraintRule(ToUtf32("FunctionConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1),
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("FunctionGroupId"), ToUtf32("FunctionGroupId"), 1)))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new EmbeddedConstraintRule(ToUtf32("EmbeddedConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("WhereConstraint"), ToUtf32("WhereConstraint"), 1),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new WhereConstraintRule(ToUtf32("WhereConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("where")),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("ConstraintExpr"), ToUtf32("ConstraintExpr"), 1)))))));
    AddRule(new ConstraintExprRule(ToUtf32("ConstraintExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("DisjunctiveConstraintExpr"), ToUtf32("DisjunctiveConstraintExpr"), 1))));
    AddRule(new DisjunctiveConstraintExprRule(ToUtf32("DisjunctiveConstraintExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("ConjunctiveConstraintExpr"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("or")),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("ConjunctiveConstraintExpr"), 1))))))));
    AddRule(new ConjunctiveConstraintExprRule(ToUtf32("ConjunctiveConstraintExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("left"), ToUtf32("PrimaryConstraintExpr"), 1)),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("and")),
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("right"), ToUtf32("PrimaryConstraintExpr"), 1))))))));
    AddRule(new PrimaryConstraintExprRule(ToUtf32("PrimaryConstraintExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::NonterminalParser(ToUtf32("ConstraintExpr"), ToUtf32("ConstraintExpr"), 1)),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser(')'))))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("AtomicConstraintExpr"), ToUtf32("AtomicConstraintExpr"), 1)))));
    AddRule(new AtomicConstraintExprRule(ToUtf32("AtomicConstraintExpr"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("PredicateConstraint"), ToUtf32("PredicateConstraint"), 1)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("IsConstraint"), ToUtf32("IsConstraint"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("MultiParamConstraint"), ToUtf32("MultiParamConstraint"), 1)))));
    AddRule(new PredicateConstraintRule(ToUtf32("PredicateConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("InvokeExpr"), ToUtf32("InvokeExpr"), 1))));
    AddRule(new IsConstraintRule(ToUtf32("IsConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                        new cmajor::parsing::KeywordParser(ToUtf32("is"))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("ConceptOrTypeName"), ToUtf32("ConceptOrTypeName"), 1)))))));
    AddRule(new ConceptOrTypeNameRule(ToUtf32("ConceptOrTypeName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1))));
    AddRule(new MultiParamConstraintRule(ToUtf32("MultiParamConstraint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0)),
                            new cmajor::parsing::CharParser('<')),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::ListParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("TypeExpr"), ToUtf32("TypeExpr"), 1)),
                                new cmajor::parsing::CharParser(',')))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('>')))))));
    AddRule(new AxiomRule(ToUtf32("Axiom"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::SequenceParser(
                                            new cmajor::parsing::SequenceParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                    new cmajor::parsing::KeywordParser(ToUtf32("axiom"))),
                                                new cmajor::parsing::OptionalParser(
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                new cmajor::parsing::EmptyParser())),
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::NonterminalParser(ToUtf32("ParameterList"), ToUtf32("ParameterList"), 2))))),
                            new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::CharParser('{')))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("AxiomBody"), ToUtf32("AxiomBody"), 2))),
                    new cmajor::parsing::ActionParser(ToUtf32("A5"),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::CharParser('}'))))))));
    AddRule(new AxiomBodyRule(ToUtf32("AxiomBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("AxiomStatement"), ToUtf32("AxiomStatement"), 1))))));
    AddRule(new AxiomStatementRule(ToUtf32("AxiomStatement"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Expression"), ToUtf32("Expression"), 1),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
}

} } // namespace cmajor.parser
