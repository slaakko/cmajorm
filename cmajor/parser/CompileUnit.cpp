#include "CompileUnit.hpp"
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
#include <cmajor/parser/Typedef.hpp>
#include <cmajor/parser/Identifier.hpp>
#include <cmajor/parser/Concept.hpp>
#include <cmajor/parser/Function.hpp>
#include <cmajor/parser/Class.hpp>
#include <cmajor/parser/Interface.hpp>
#include <cmajor/parser/Enumeration.hpp>
#include <cmajor/parser/Constant.hpp>
#include <cmajor/parser/Delegate.hpp>
#include <cmajor/parser/GlobalVariable.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

CompileUnit* CompileUnit::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

CompileUnit* CompileUnit::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    CompileUnit* grammar(new CompileUnit(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

CompileUnit::CompileUnit(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("CompileUnit"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

CompileUnitNode* CompileUnit::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx)
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
    CompileUnitNode* result = *static_cast<cmajor::parsing::ValueObject<CompileUnitNode*>*>(value.get());
    stack.pop();
    return result;
}

class CompileUnit::CompileUnitRule : public cmajor::parsing::Rule
{
public:
    CompileUnitRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("CompileUnitNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CompileUnitRule>(this, &CompileUnitRule::A0Action));
        cmajor::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<CompileUnitRule>(this, &CompileUnitRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CompileUnitNode(span, fileName);
    }
    void PreNamespaceContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->value)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->value->GlobalNs())));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value() {}
        ParsingContext* ctx;
        CompileUnitNode* value;
    };
};

class CompileUnit::NamespaceContentRule : public cmajor::parsing::Rule
{
public:
    NamespaceContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("CompileUnitNode*"), ToUtf32("cu")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("NamespaceNode*"), ToUtf32("ns")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ns_value = std::move(stack.top());
        context->ns = *static_cast<cmajor::parsing::ValueObject<NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> cu_value = std::move(stack.top());
        context->cu = *static_cast<cmajor::parsing::ValueObject<CompileUnitNode*>*>(cu_value.get());
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
        cmajor::parsing::NonterminalParser* usingDirectivesNonterminalParser = GetNonterminal(ToUtf32("UsingDirectives"));
        usingDirectivesNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreUsingDirectives));
        cmajor::parsing::NonterminalParser* definitionsNonterminalParser = GetNonterminal(ToUtf32("Definitions"));
        definitionsNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreDefinitions));
    }
    void PreUsingDirectives(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->ns)));
    }
    void PreDefinitions(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->cu)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->ns)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), cu(), ns() {}
        ParsingContext* ctx;
        CompileUnitNode* cu;
        NamespaceNode* ns;
    };
};

class CompileUnit::UsingDirectivesRule : public cmajor::parsing::Rule
{
public:
    UsingDirectivesRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("NamespaceNode*"), ToUtf32("ns")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ns_value = std::move(stack.top());
        context->ns = *static_cast<cmajor::parsing::ValueObject<NamespaceNode*>*>(ns_value.get());
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
        cmajor::parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingDirective"));
        usingDirectiveNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<UsingDirectivesRule>(this, &UsingDirectivesRule::PreUsingDirective));
    }
    void PreUsingDirective(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->ns)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), ns() {}
        ParsingContext* ctx;
        NamespaceNode* ns;
    };
};

class CompileUnit::UsingDirectiveRule : public cmajor::parsing::Rule
{
public:
    UsingDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("NamespaceNode*"), ToUtf32("ns")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ns_value = std::move(stack.top());
        context->ns = *static_cast<cmajor::parsing::ValueObject<NamespaceNode*>*>(ns_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UsingDirectiveRule>(this, &UsingDirectiveRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UsingDirectiveRule>(this, &UsingDirectiveRule::A1Action));
        cmajor::parsing::NonterminalParser* usingAliasDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingAliasDirective"));
        usingAliasDirectiveNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PostUsingAliasDirective));
        cmajor::parsing::NonterminalParser* usingNamespaceDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingNamespaceDirective"));
        usingNamespaceDirectiveNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PostUsingNamespaceDirective));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ns->AddMember(context->fromUsingAliasDirective);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ns->AddMember(context->fromUsingNamespaceDirective);
    }
    void PostUsingAliasDirective(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUsingAliasDirective_value = std::move(stack.top());
            context->fromUsingAliasDirective = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromUsingAliasDirective_value.get());
            stack.pop();
        }
    }
    void PostUsingNamespaceDirective(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUsingNamespaceDirective_value = std::move(stack.top());
            context->fromUsingNamespaceDirective = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromUsingNamespaceDirective_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), ns(), fromUsingAliasDirective(), fromUsingNamespaceDirective() {}
        ParsingContext* ctx;
        NamespaceNode* ns;
        Node* fromUsingAliasDirective;
        Node* fromUsingNamespaceDirective;
    };
};

class CompileUnit::UsingAliasDirectiveRule : public cmajor::parsing::Rule
{
public:
    UsingAliasDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Node*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::A1Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UsingAliasDirectiveRule>(this, &UsingAliasDirectiveRule::PostQualifiedId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AliasNode(span, context->id.release(), context->fromQualifiedId);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->id.reset(context->fromIdentifier);
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
        Context(): value(), id(), fromIdentifier(), fromQualifiedId() {}
        Node* value;
        std::unique_ptr<IdentifierNode> id;
        IdentifierNode* fromIdentifier;
        IdentifierNode* fromQualifiedId;
    };
};

class CompileUnit::UsingNamespaceDirectiveRule : public cmajor::parsing::Rule
{
public:
    UsingNamespaceDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Node*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Node*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UsingNamespaceDirectiveRule>(this, &UsingNamespaceDirectiveRule::A0Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UsingNamespaceDirectiveRule>(this, &UsingNamespaceDirectiveRule::PostQualifiedId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NamespaceImportNode(span, context->fromQualifiedId);
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
        Context(): value(), fromQualifiedId() {}
        Node* value;
        IdentifierNode* fromQualifiedId;
    };
};

class CompileUnit::DefinitionsRule : public cmajor::parsing::Rule
{
public:
    DefinitionsRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("CompileUnitNode*"), ToUtf32("cu")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("NamespaceNode*"), ToUtf32("ns")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ns_value = std::move(stack.top());
        context->ns = *static_cast<cmajor::parsing::ValueObject<NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> cu_value = std::move(stack.top());
        context->cu = *static_cast<cmajor::parsing::ValueObject<CompileUnitNode*>*>(cu_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionsRule>(this, &DefinitionsRule::A0Action));
        cmajor::parsing::NonterminalParser* definitionNonterminalParser = GetNonterminal(ToUtf32("Definition"));
        definitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionsRule>(this, &DefinitionsRule::PreDefinition));
        definitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionsRule>(this, &DefinitionsRule::PostDefinition));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->ns->AddMember(context->fromDefinition);
    }
    void PreDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->cu)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->ns)));
    }
    void PostDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDefinition_value = std::move(stack.top());
            context->fromDefinition = *static_cast<cmajor::parsing::ValueObject<Node*>*>(fromDefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), cu(), ns(), fromDefinition() {}
        ParsingContext* ctx;
        CompileUnitNode* cu;
        NamespaceNode* ns;
        Node* fromDefinition;
    };
};

class CompileUnit::DefinitionRule : public cmajor::parsing::Rule
{
public:
    DefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("CompileUnitNode*"), ToUtf32("cu")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("NamespaceNode*"), ToUtf32("ns")));
        SetValueTypeName(ToUtf32("Node*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ns_value = std::move(stack.top());
        context->ns = *static_cast<cmajor::parsing::ValueObject<NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> cu_value = std::move(stack.top());
        context->cu = *static_cast<cmajor::parsing::ValueObject<CompileUnitNode*>*>(cu_value.get());
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DefinitionRule>(this, &DefinitionRule::A10Action));
        cmajor::parsing::NonterminalParser* namespaceDefinitionNonterminalParser = GetNonterminal(ToUtf32("NamespaceDefinition"));
        namespaceDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreNamespaceDefinition));
        namespaceDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostNamespaceDefinition));
        cmajor::parsing::NonterminalParser* typedefDeclarationNonterminalParser = GetNonterminal(ToUtf32("TypedefDeclaration"));
        typedefDeclarationNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreTypedefDeclaration));
        typedefDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostTypedefDeclaration));
        cmajor::parsing::NonterminalParser* conceptDefinitionNonterminalParser = GetNonterminal(ToUtf32("ConceptDefinition"));
        conceptDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreConceptDefinition));
        conceptDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostConceptDefinition));
        cmajor::parsing::NonterminalParser* functionDefinitionNonterminalParser = GetNonterminal(ToUtf32("FunctionDefinition"));
        functionDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreFunctionDefinition));
        functionDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostFunctionDefinition));
        cmajor::parsing::NonterminalParser* classDefinitionNonterminalParser = GetNonterminal(ToUtf32("ClassDefinition"));
        classDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreClassDefinition));
        classDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostClassDefinition));
        cmajor::parsing::NonterminalParser* interfaceDefinitionNonterminalParser = GetNonterminal(ToUtf32("InterfaceDefinition"));
        interfaceDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreInterfaceDefinition));
        interfaceDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostInterfaceDefinition));
        cmajor::parsing::NonterminalParser* enumTypeDefinitionNonterminalParser = GetNonterminal(ToUtf32("EnumTypeDefinition"));
        enumTypeDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreEnumTypeDefinition));
        enumTypeDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostEnumTypeDefinition));
        cmajor::parsing::NonterminalParser* constantDefinitionNonterminalParser = GetNonterminal(ToUtf32("ConstantDefinition"));
        constantDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreConstantDefinition));
        constantDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostConstantDefinition));
        cmajor::parsing::NonterminalParser* delegateDefinitionNonterminalParser = GetNonterminal(ToUtf32("DelegateDefinition"));
        delegateDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreDelegateDefinition));
        delegateDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostDelegateDefinition));
        cmajor::parsing::NonterminalParser* classDelegateDefinitionNonterminalParser = GetNonterminal(ToUtf32("ClassDelegateDefinition"));
        classDelegateDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreClassDelegateDefinition));
        classDelegateDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostClassDelegateDefinition));
        cmajor::parsing::NonterminalParser* globalVariableDefinitionNonterminalParser = GetNonterminal(ToUtf32("GlobalVariableDefinition"));
        globalVariableDefinitionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DefinitionRule>(this, &DefinitionRule::PreGlobalVariableDefinition));
        globalVariableDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DefinitionRule>(this, &DefinitionRule::PostGlobalVariableDefinition));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNamespaceDefinition;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypedefDeclaration;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConceptDefinition;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunctionDefinition;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClassDefinition;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromInterfaceDefinition;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnumTypeDefinition;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConstantDefinition;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDelegateDefinition;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClassDelegateDefinition;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGlobalVariableDefinition;
    }
    void PreNamespaceDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->cu)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->ns)));
    }
    void PostNamespaceDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNamespaceDefinition_value = std::move(stack.top());
            context->fromNamespaceDefinition = *static_cast<cmajor::parsing::ValueObject<NamespaceNode*>*>(fromNamespaceDefinition_value.get());
            stack.pop();
        }
    }
    void PreTypedefDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostTypedefDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypedefDeclaration_value = std::move(stack.top());
            context->fromTypedefDeclaration = *static_cast<cmajor::parsing::ValueObject<TypedefNode*>*>(fromTypedefDeclaration_value.get());
            stack.pop();
        }
    }
    void PreConceptDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConceptDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConceptDefinition_value = std::move(stack.top());
            context->fromConceptDefinition = *static_cast<cmajor::parsing::ValueObject<ConceptNode*>*>(fromConceptDefinition_value.get());
            stack.pop();
        }
    }
    void PreFunctionDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostFunctionDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFunctionDefinition_value = std::move(stack.top());
            context->fromFunctionDefinition = *static_cast<cmajor::parsing::ValueObject<FunctionNode*>*>(fromFunctionDefinition_value.get());
            stack.pop();
        }
    }
    void PreClassDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromClassDefinition_value = std::move(stack.top());
            context->fromClassDefinition = *static_cast<cmajor::parsing::ValueObject<ClassNode*>*>(fromClassDefinition_value.get());
            stack.pop();
        }
    }
    void PreInterfaceDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInterfaceDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInterfaceDefinition_value = std::move(stack.top());
            context->fromInterfaceDefinition = *static_cast<cmajor::parsing::ValueObject<InterfaceNode*>*>(fromInterfaceDefinition_value.get());
            stack.pop();
        }
    }
    void PreEnumTypeDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostEnumTypeDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEnumTypeDefinition_value = std::move(stack.top());
            context->fromEnumTypeDefinition = *static_cast<cmajor::parsing::ValueObject<EnumTypeNode*>*>(fromEnumTypeDefinition_value.get());
            stack.pop();
        }
    }
    void PreConstantDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConstantDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConstantDefinition_value = std::move(stack.top());
            context->fromConstantDefinition = *static_cast<cmajor::parsing::ValueObject<ConstantNode*>*>(fromConstantDefinition_value.get());
            stack.pop();
        }
    }
    void PreDelegateDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostDelegateDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDelegateDefinition_value = std::move(stack.top());
            context->fromDelegateDefinition = *static_cast<cmajor::parsing::ValueObject<DelegateNode*>*>(fromDelegateDefinition_value.get());
            stack.pop();
        }
    }
    void PreClassDelegateDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostClassDelegateDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromClassDelegateDefinition_value = std::move(stack.top());
            context->fromClassDelegateDefinition = *static_cast<cmajor::parsing::ValueObject<ClassDelegateNode*>*>(fromClassDelegateDefinition_value.get());
            stack.pop();
        }
    }
    void PreGlobalVariableDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->cu)));
    }
    void PostGlobalVariableDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGlobalVariableDefinition_value = std::move(stack.top());
            context->fromGlobalVariableDefinition = *static_cast<cmajor::parsing::ValueObject<GlobalVariableNode*>*>(fromGlobalVariableDefinition_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), cu(), ns(), value(), fromNamespaceDefinition(), fromTypedefDeclaration(), fromConceptDefinition(), fromFunctionDefinition(), fromClassDefinition(), fromInterfaceDefinition(), fromEnumTypeDefinition(), fromConstantDefinition(), fromDelegateDefinition(), fromClassDelegateDefinition(), fromGlobalVariableDefinition() {}
        ParsingContext* ctx;
        CompileUnitNode* cu;
        NamespaceNode* ns;
        Node* value;
        NamespaceNode* fromNamespaceDefinition;
        TypedefNode* fromTypedefDeclaration;
        ConceptNode* fromConceptDefinition;
        FunctionNode* fromFunctionDefinition;
        ClassNode* fromClassDefinition;
        InterfaceNode* fromInterfaceDefinition;
        EnumTypeNode* fromEnumTypeDefinition;
        ConstantNode* fromConstantDefinition;
        DelegateNode* fromDelegateDefinition;
        ClassDelegateNode* fromClassDelegateDefinition;
        GlobalVariableNode* fromGlobalVariableDefinition;
    };
};

class CompileUnit::NamespaceDefinitionRule : public cmajor::parsing::Rule
{
public:
    NamespaceDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("CompileUnitNode*"), ToUtf32("cu")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("NamespaceNode*"), ToUtf32("ns")));
        SetValueTypeName(ToUtf32("NamespaceNode*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> ns_value = std::move(stack.top());
        context->ns = *static_cast<cmajor::parsing::ValueObject<NamespaceNode*>*>(ns_value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> cu_value = std::move(stack.top());
        context->cu = *static_cast<cmajor::parsing::ValueObject<CompileUnitNode*>*>(cu_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PostQualifiedId));
        cmajor::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceDefinitionRule>(this, &NamespaceDefinitionRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NamespaceNode(span, context->fromQualifiedId);
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
    void PreNamespaceContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->cu)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<NamespaceNode*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), cu(), ns(), value(), fromQualifiedId() {}
        ParsingContext* ctx;
        CompileUnitNode* cu;
        NamespaceNode* ns;
        NamespaceNode* value;
        IdentifierNode* fromQualifiedId;
    };
};

class CompileUnit::TypedefDeclarationRule : public cmajor::parsing::Rule
{
public:
    TypedefDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("TypedefNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<TypedefNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypedefDeclarationRule>(this, &TypedefDeclarationRule::A0Action));
        cmajor::parsing::NonterminalParser* typedefNonterminalParser = GetNonterminal(ToUtf32("Typedef"));
        typedefNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TypedefDeclarationRule>(this, &TypedefDeclarationRule::PreTypedef));
        typedefNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypedefDeclarationRule>(this, &TypedefDeclarationRule::PostTypedef));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypedef;
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromTypedef() {}
        ParsingContext* ctx;
        TypedefNode* value;
        TypedefNode* fromTypedef;
    };
};

class CompileUnit::ConceptDefinitionRule : public cmajor::parsing::Rule
{
public:
    ConceptDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConceptNode*"));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConceptDefinitionRule>(this, &ConceptDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* conceptNonterminalParser = GetNonterminal(ToUtf32("Concept"));
        conceptNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConceptDefinitionRule>(this, &ConceptDefinitionRule::PreConcept));
        conceptNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConceptDefinitionRule>(this, &ConceptDefinitionRule::PostConcept));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConcept;
    }
    void PreConcept(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostConcept(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConcept_value = std::move(stack.top());
            context->fromConcept = *static_cast<cmajor::parsing::ValueObject<ConceptNode*>*>(fromConcept_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromConcept() {}
        ParsingContext* ctx;
        ConceptNode* value;
        ConceptNode* fromConcept;
    };
};

class CompileUnit::FunctionDefinitionRule : public cmajor::parsing::Rule
{
public:
    FunctionDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("FunctionNode*"));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FunctionDefinitionRule>(this, &FunctionDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* functionNonterminalParser = GetNonterminal(ToUtf32("Function"));
        functionNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<FunctionDefinitionRule>(this, &FunctionDefinitionRule::PreFunction));
        functionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<FunctionDefinitionRule>(this, &FunctionDefinitionRule::PostFunction));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFunction;
    }
    void PreFunction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostFunction(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFunction_value = std::move(stack.top());
            context->fromFunction = *static_cast<cmajor::parsing::ValueObject<FunctionNode*>*>(fromFunction_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromFunction() {}
        ParsingContext* ctx;
        FunctionNode* value;
        FunctionNode* fromFunction;
    };
};

class CompileUnit::ClassDefinitionRule : public cmajor::parsing::Rule
{
public:
    ClassDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ClassNode*"));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassDefinitionRule>(this, &ClassDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* classNonterminalParser = GetNonterminal(ToUtf32("Class"));
        classNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassDefinitionRule>(this, &ClassDefinitionRule::PreClass));
        classNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassDefinitionRule>(this, &ClassDefinitionRule::PostClass));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClass;
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromClass() {}
        ParsingContext* ctx;
        ClassNode* value;
        ClassNode* fromClass;
    };
};

class CompileUnit::InterfaceDefinitionRule : public cmajor::parsing::Rule
{
public:
    InterfaceDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("InterfaceNode*"));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InterfaceDefinitionRule>(this, &InterfaceDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* interfaceNonterminalParser = GetNonterminal(ToUtf32("Interface"));
        interfaceNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InterfaceDefinitionRule>(this, &InterfaceDefinitionRule::PreInterface));
        interfaceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InterfaceDefinitionRule>(this, &InterfaceDefinitionRule::PostInterface));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromInterface;
    }
    void PreInterface(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
    }
    void PostInterface(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInterface_value = std::move(stack.top());
            context->fromInterface = *static_cast<cmajor::parsing::ValueObject<InterfaceNode*>*>(fromInterface_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromInterface() {}
        ParsingContext* ctx;
        InterfaceNode* value;
        InterfaceNode* fromInterface;
    };
};

class CompileUnit::EnumTypeDefinitionRule : public cmajor::parsing::Rule
{
public:
    EnumTypeDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("EnumTypeNode*"));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnumTypeDefinitionRule>(this, &EnumTypeDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* enumTypeNonterminalParser = GetNonterminal(ToUtf32("EnumType"));
        enumTypeNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<EnumTypeDefinitionRule>(this, &EnumTypeDefinitionRule::PreEnumType));
        enumTypeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<EnumTypeDefinitionRule>(this, &EnumTypeDefinitionRule::PostEnumType));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnumType;
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromEnumType() {}
        ParsingContext* ctx;
        EnumTypeNode* value;
        EnumTypeNode* fromEnumType;
    };
};

class CompileUnit::ConstantDefinitionRule : public cmajor::parsing::Rule
{
public:
    ConstantDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ConstantNode*"));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConstantDefinitionRule>(this, &ConstantDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* constantNonterminalParser = GetNonterminal(ToUtf32("Constant"));
        constantNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ConstantDefinitionRule>(this, &ConstantDefinitionRule::PreConstant));
        constantNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ConstantDefinitionRule>(this, &ConstantDefinitionRule::PostConstant));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConstant;
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromConstant() {}
        ParsingContext* ctx;
        ConstantNode* value;
        ConstantNode* fromConstant;
    };
};

class CompileUnit::DelegateDefinitionRule : public cmajor::parsing::Rule
{
public:
    DelegateDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("DelegateNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<DelegateNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DelegateDefinitionRule>(this, &DelegateDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* delegateNonterminalParser = GetNonterminal(ToUtf32("Delegate"));
        delegateNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<DelegateDefinitionRule>(this, &DelegateDefinitionRule::PreDelegate));
        delegateNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DelegateDefinitionRule>(this, &DelegateDefinitionRule::PostDelegate));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDelegate;
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), value(), fromDelegate() {}
        ParsingContext* ctx;
        DelegateNode* value;
        DelegateNode* fromDelegate;
    };
};

class CompileUnit::ClassDelegateDefinitionRule : public cmajor::parsing::Rule
{
public:
    ClassDelegateDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        SetValueTypeName(ToUtf32("ClassDelegateNode*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ClassDelegateNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClassDelegateDefinitionRule>(this, &ClassDelegateDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* classDelegateNonterminalParser = GetNonterminal(ToUtf32("ClassDelegate"));
        classDelegateNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ClassDelegateDefinitionRule>(this, &ClassDelegateDefinitionRule::PreClassDelegate));
        classDelegateNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ClassDelegateDefinitionRule>(this, &ClassDelegateDefinitionRule::PostClassDelegate));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClassDelegate;
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
        Context(): ctx(), value(), fromClassDelegate() {}
        ParsingContext* ctx;
        ClassDelegateNode* value;
        ClassDelegateNode* fromClassDelegate;
    };
};

class CompileUnit::GlobalVariableDefinitionRule : public cmajor::parsing::Rule
{
public:
    GlobalVariableDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParsingContext*"), ToUtf32("ctx")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("CompileUnitNode*"), ToUtf32("cu")));
        SetValueTypeName(ToUtf32("GlobalVariableNode*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> cu_value = std::move(stack.top());
        context->cu = *static_cast<cmajor::parsing::ValueObject<CompileUnitNode*>*>(cu_value.get());
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<GlobalVariableNode*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<GlobalVariableDefinitionRule>(this, &GlobalVariableDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* globalVariableNonterminalParser = GetNonterminal(ToUtf32("GlobalVariable"));
        globalVariableNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<GlobalVariableDefinitionRule>(this, &GlobalVariableDefinitionRule::PreGlobalVariable));
        globalVariableNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<GlobalVariableDefinitionRule>(this, &GlobalVariableDefinitionRule::PostGlobalVariable));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGlobalVariable;
    }
    void PreGlobalVariable(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParsingContext*>(context->ctx)));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<CompileUnitNode*>(context->cu)));
    }
    void PostGlobalVariable(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGlobalVariable_value = std::move(stack.top());
            context->fromGlobalVariable = *static_cast<cmajor::parsing::ValueObject<GlobalVariableNode*>*>(fromGlobalVariable_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): ctx(), cu(), value(), fromGlobalVariable() {}
        ParsingContext* ctx;
        CompileUnitNode* cu;
        GlobalVariableNode* value;
        GlobalVariableNode* fromGlobalVariable;
    };
};

void CompileUnit::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parser.Delegate"));
    if (!grammar0)
    {
        grammar0 = cmajor::parser::Delegate::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parser.Class"));
    if (!grammar1)
    {
        grammar1 = cmajor::parser::Class::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.parser.Concept"));
    if (!grammar2)
    {
        grammar2 = cmajor::parser::Concept::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.parser.Typedef"));
    if (!grammar3)
    {
        grammar3 = cmajor::parser::Typedef::Create(pd);
    }
    AddGrammarReference(grammar3);
    cmajor::parsing::Grammar* grammar4 = pd->GetGrammar(ToUtf32("cmajor.parser.Identifier"));
    if (!grammar4)
    {
        grammar4 = cmajor::parser::Identifier::Create(pd);
    }
    AddGrammarReference(grammar4);
    cmajor::parsing::Grammar* grammar5 = pd->GetGrammar(ToUtf32("cmajor.parser.Enumeration"));
    if (!grammar5)
    {
        grammar5 = cmajor::parser::Enumeration::Create(pd);
    }
    AddGrammarReference(grammar5);
    cmajor::parsing::Grammar* grammar6 = pd->GetGrammar(ToUtf32("cmajor.parser.Interface"));
    if (!grammar6)
    {
        grammar6 = cmajor::parser::Interface::Create(pd);
    }
    AddGrammarReference(grammar6);
    cmajor::parsing::Grammar* grammar7 = pd->GetGrammar(ToUtf32("cmajor.parser.Function"));
    if (!grammar7)
    {
        grammar7 = cmajor::parser::Function::Create(pd);
    }
    AddGrammarReference(grammar7);
    cmajor::parsing::Grammar* grammar8 = pd->GetGrammar(ToUtf32("cmajor.parser.Constant"));
    if (!grammar8)
    {
        grammar8 = cmajor::parser::Constant::Create(pd);
    }
    AddGrammarReference(grammar8);
    cmajor::parsing::Grammar* grammar9 = pd->GetGrammar(ToUtf32("cmajor.parser.GlobalVariable"));
    if (!grammar9)
    {
        grammar9 = cmajor::parser::GlobalVariable::Create(pd);
    }
    AddGrammarReference(grammar9);
    cmajor::parsing::Grammar* grammar10 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar10)
    {
        grammar10 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar10);
}

void CompileUnit::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Concept"), this, ToUtf32("Concept.Concept")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Interface"), this, ToUtf32("Interface.Interface")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Identifier.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Function"), this, ToUtf32("Function.Function")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Constant"), this, ToUtf32("Constant.Constant")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Typedef"), this, ToUtf32("Typedef.Typedef")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Class"), this, ToUtf32("Class.Class")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("EnumType"), this, ToUtf32("Enumeration.EnumType")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Delegate"), this, ToUtf32("Delegate.Delegate")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ClassDelegate"), this, ToUtf32("Delegate.ClassDelegate")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("GlobalVariable"), this, ToUtf32("GlobalVariable.GlobalVariable")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("cmajor.parsing.stdlib.spaces_and_comments")));
    AddRule(new CompileUnitRule(ToUtf32("CompileUnit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 3))));
    AddRule(new NamespaceContentRule(ToUtf32("NamespaceContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("UsingDirectives"), ToUtf32("UsingDirectives"), 2),
            new cmajor::parsing::NonterminalParser(ToUtf32("Definitions"), ToUtf32("Definitions"), 3))));
    AddRule(new UsingDirectivesRule(ToUtf32("UsingDirectives"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("UsingDirective"), ToUtf32("UsingDirective"), 2))));
    AddRule(new UsingDirectiveRule(ToUtf32("UsingDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("UsingAliasDirective"), ToUtf32("UsingAliasDirective"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("UsingNamespaceDirective"), ToUtf32("UsingNamespaceDirective"), 0)))));
    AddRule(new UsingAliasDirectiveRule(ToUtf32("UsingAliasDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("using")),
                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0))),
                            new cmajor::parsing::CharParser('=')),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new UsingNamespaceDirectiveRule(ToUtf32("UsingNamespaceDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("using")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new DefinitionsRule(ToUtf32("Definitions"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Definition"), ToUtf32("Definition"), 3))))));
    AddRule(new DefinitionRule(ToUtf32("Definition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceDefinition"), ToUtf32("NamespaceDefinition"), 3)),
                                                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("TypedefDeclaration"), ToUtf32("TypedefDeclaration"), 1))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("ConceptDefinition"), ToUtf32("ConceptDefinition"), 1))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("FunctionDefinition"), ToUtf32("FunctionDefinition"), 1))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("ClassDefinition"), ToUtf32("ClassDefinition"), 1))),
                                new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("InterfaceDefinition"), ToUtf32("InterfaceDefinition"), 1))),
                            new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("EnumTypeDefinition"), ToUtf32("EnumTypeDefinition"), 1))),
                        new cmajor::parsing::ActionParser(ToUtf32("A7"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("ConstantDefinition"), ToUtf32("ConstantDefinition"), 1))),
                    new cmajor::parsing::ActionParser(ToUtf32("A8"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("DelegateDefinition"), ToUtf32("DelegateDefinition"), 1))),
                new cmajor::parsing::ActionParser(ToUtf32("A9"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("ClassDelegateDefinition"), ToUtf32("ClassDelegateDefinition"), 1))),
            new cmajor::parsing::ActionParser(ToUtf32("A10"),
                new cmajor::parsing::NonterminalParser(ToUtf32("GlobalVariableDefinition"), ToUtf32("GlobalVariableDefinition"), 2)))));
    AddRule(new NamespaceDefinitionRule(ToUtf32("NamespaceDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::KeywordParser(ToUtf32("namespace")),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0))))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('{'))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 3))),
            new cmajor::parsing::ExpectationParser(
                new cmajor::parsing::CharParser('}')))));
    AddRule(new TypedefDeclarationRule(ToUtf32("TypedefDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Typedef"), ToUtf32("Typedef"), 1))));
    AddRule(new ConceptDefinitionRule(ToUtf32("ConceptDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Concept"), ToUtf32("Concept"), 1))));
    AddRule(new FunctionDefinitionRule(ToUtf32("FunctionDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Function"), ToUtf32("Function"), 1))));
    AddRule(new ClassDefinitionRule(ToUtf32("ClassDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Class"), ToUtf32("Class"), 1))));
    AddRule(new InterfaceDefinitionRule(ToUtf32("InterfaceDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Interface"), ToUtf32("Interface"), 1))));
    AddRule(new EnumTypeDefinitionRule(ToUtf32("EnumTypeDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("EnumType"), ToUtf32("EnumType"), 1))));
    AddRule(new ConstantDefinitionRule(ToUtf32("ConstantDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Constant"), ToUtf32("Constant"), 1))));
    AddRule(new DelegateDefinitionRule(ToUtf32("DelegateDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("Delegate"), ToUtf32("Delegate"), 1))));
    AddRule(new ClassDelegateDefinitionRule(ToUtf32("ClassDelegateDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("ClassDelegate"), ToUtf32("ClassDelegate"), 1))));
    AddRule(new GlobalVariableDefinitionRule(ToUtf32("GlobalVariableDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("GlobalVariable"), ToUtf32("GlobalVariable"), 2))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace cmajor.parser
