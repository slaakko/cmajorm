#include "Declaration.hpp"
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
#include <cmajor/code/Declarator.hpp>
#include <cmajor/code/Identifier.hpp>
#include <cmajor/code/Expression.hpp>
#include <cmajor/codedom/Type.hpp>

namespace cmajor { namespace code {

using namespace cmajor::codedom;
using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Declaration* Declaration::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Declaration* Declaration::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Declaration* grammar(new Declaration(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Declaration::Declaration(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Declaration"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.code")), parsingDomain_)
{
    SetOwner(0);
    keywords0.push_back(ToUtf32("auto"));
    keywords0.push_back(ToUtf32("extern"));
    keywords0.push_back(ToUtf32("mutable"));
    keywords0.push_back(ToUtf32("register"));
    keywords0.push_back(ToUtf32("static"));
    keywords1.push_back(ToUtf32("bool"));
    keywords1.push_back(ToUtf32("char"));
    keywords1.push_back(ToUtf32("double"));
    keywords1.push_back(ToUtf32("float"));
    keywords1.push_back(ToUtf32("int"));
    keywords1.push_back(ToUtf32("long"));
    keywords1.push_back(ToUtf32("short"));
    keywords1.push_back(ToUtf32("signed"));
    keywords1.push_back(ToUtf32("unsigned"));
    keywords1.push_back(ToUtf32("void"));
    keywords1.push_back(ToUtf32("wchar_t"));
}

cmajor::codedom::CppObject* Declaration::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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

class Declaration::BlockDeclarationRule : public cmajor::parsing::Rule
{
public:
    BlockDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BlockDeclarationRule>(this, &BlockDeclarationRule::A3Action));
        cmajor::parsing::NonterminalParser* namespaceAliasDefinitionNonterminalParser = GetNonterminal(ToUtf32("NamespaceAliasDefinition"));
        namespaceAliasDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostNamespaceAliasDefinition));
        cmajor::parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingDirective"));
        usingDirectiveNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostUsingDirective));
        cmajor::parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal(ToUtf32("UsingDeclaration"));
        usingDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostUsingDeclaration));
        cmajor::parsing::NonterminalParser* simpleDeclarationNonterminalParser = GetNonterminal(ToUtf32("SimpleDeclaration"));
        simpleDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<BlockDeclarationRule>(this, &BlockDeclarationRule::PostSimpleDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNamespaceAliasDefinition;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUsingDirective;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUsingDeclaration;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleDeclaration;
    }
    void PostNamespaceAliasDefinition(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNamespaceAliasDefinition_value = std::move(stack.top());
            context->fromNamespaceAliasDefinition = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::UsingObject*>*>(fromNamespaceAliasDefinition_value.get());
            stack.pop();
        }
    }
    void PostUsingDirective(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUsingDirective_value = std::move(stack.top());
            context->fromUsingDirective = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::UsingObject*>*>(fromUsingDirective_value.get());
            stack.pop();
        }
    }
    void PostUsingDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUsingDeclaration_value = std::move(stack.top());
            context->fromUsingDeclaration = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::UsingObject*>*>(fromUsingDeclaration_value.get());
            stack.pop();
        }
    }
    void PostSimpleDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSimpleDeclaration_value = std::move(stack.top());
            context->fromSimpleDeclaration = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::SimpleDeclaration*>*>(fromSimpleDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromNamespaceAliasDefinition(), fromUsingDirective(), fromUsingDeclaration(), fromSimpleDeclaration() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::UsingObject* fromNamespaceAliasDefinition;
        cmajor::codedom::UsingObject* fromUsingDirective;
        cmajor::codedom::UsingObject* fromUsingDeclaration;
        cmajor::codedom::SimpleDeclaration* fromSimpleDeclaration;
    };
};

class Declaration::SimpleDeclarationRule : public cmajor::parsing::Rule
{
public:
    SimpleDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::SimpleDeclaration*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<SimpleDeclaration>"), ToUtf32("sd")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::SimpleDeclaration*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SimpleDeclarationRule>(this, &SimpleDeclarationRule::A2Action));
        cmajor::parsing::NonterminalParser* declSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("DeclSpecifierSeq"));
        declSpecifierSeqNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PreDeclSpecifierSeq));
        cmajor::parsing::NonterminalParser* initDeclaratorListNonterminalParser = GetNonterminal(ToUtf32("InitDeclaratorList"));
        initDeclaratorListNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SimpleDeclarationRule>(this, &SimpleDeclarationRule::PostInitDeclaratorList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->sd.reset(new SimpleDeclaration);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->sd.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->sd->SetInitDeclaratorList(context->fromInitDeclaratorList);
    }
    void PreDeclSpecifierSeq(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::SimpleDeclaration*>(context->sd.get())));
    }
    void PostInitDeclaratorList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitDeclaratorList_value = std::move(stack.top());
            context->fromInitDeclaratorList = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::InitDeclaratorList*>*>(fromInitDeclaratorList_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), sd(), fromInitDeclaratorList() {}
        cmajor::codedom::SimpleDeclaration* value;
        std::unique_ptr<SimpleDeclaration> sd;
        cmajor::codedom::InitDeclaratorList* fromInitDeclaratorList;
    };
};

class Declaration::DeclSpecifierSeqRule : public cmajor::parsing::Rule
{
public:
    DeclSpecifierSeqRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::codedom::SimpleDeclaration*"), ToUtf32("declaration")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> declaration_value = std::move(stack.top());
        context->declaration = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::SimpleDeclaration*>*>(declaration_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::A1Action));
        cmajor::parsing::NonterminalParser* declSpecifierNonterminalParser = GetNonterminal(ToUtf32("DeclSpecifier"));
        declSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::PostDeclSpecifier));
        cmajor::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclSpecifierSeqRule>(this, &DeclSpecifierSeqRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->declaration->Add(context->fromDeclSpecifier);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->declaration->Add(context->fromTypeName);
    }
    void PostDeclSpecifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclSpecifier_value = std::move(stack.top());
            context->fromDeclSpecifier = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::DeclSpecifier*>*>(fromDeclSpecifier_value.get());
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
        Context(): declaration(), fromDeclSpecifier(), fromTypeName() {}
        cmajor::codedom::SimpleDeclaration* declaration;
        cmajor::codedom::DeclSpecifier* fromDeclSpecifier;
        cmajor::codedom::TypeName* fromTypeName;
    };
};

class Declaration::DeclSpecifierRule : public cmajor::parsing::Rule
{
public:
    DeclSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::DeclSpecifier*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::DeclSpecifier*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclSpecifierRule>(this, &DeclSpecifierRule::A2Action));
        cmajor::parsing::NonterminalParser* storageClassSpecifierNonterminalParser = GetNonterminal(ToUtf32("StorageClassSpecifier"));
        storageClassSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostStorageClassSpecifier));
        cmajor::parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifier"));
        typeSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostTypeSpecifier));
        cmajor::parsing::NonterminalParser* typedefNonterminalParser = GetNonterminal(ToUtf32("Typedef"));
        typedefNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclSpecifierRule>(this, &DeclSpecifierRule::PostTypedef));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromStorageClassSpecifier;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeSpecifier;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypedef;
    }
    void PostStorageClassSpecifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromStorageClassSpecifier_value = std::move(stack.top());
            context->fromStorageClassSpecifier = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::StorageClassSpecifier*>*>(fromStorageClassSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypeSpecifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypeSpecifier_value = std::move(stack.top());
            context->fromTypeSpecifier = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeSpecifier*>*>(fromTypeSpecifier_value.get());
            stack.pop();
        }
    }
    void PostTypedef(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTypedef_value = std::move(stack.top());
            context->fromTypedef = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::DeclSpecifier*>*>(fromTypedef_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromStorageClassSpecifier(), fromTypeSpecifier(), fromTypedef() {}
        cmajor::codedom::DeclSpecifier* value;
        cmajor::codedom::StorageClassSpecifier* fromStorageClassSpecifier;
        cmajor::codedom::TypeSpecifier* fromTypeSpecifier;
        cmajor::codedom::DeclSpecifier* fromTypedef;
    };
};

class Declaration::StorageClassSpecifierRule : public cmajor::parsing::Rule
{
public:
    StorageClassSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::StorageClassSpecifier*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::StorageClassSpecifier*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StorageClassSpecifierRule>(this, &StorageClassSpecifierRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StorageClassSpecifier(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::StorageClassSpecifier* value;
    };
};

class Declaration::TypeSpecifierRule : public cmajor::parsing::Rule
{
public:
    TypeSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::TypeSpecifier*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeSpecifier*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeSpecifierRule>(this, &TypeSpecifierRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeSpecifierRule>(this, &TypeSpecifierRule::A1Action));
        cmajor::parsing::NonterminalParser* simpleTypeSpecifierNonterminalParser = GetNonterminal(ToUtf32("SimpleTypeSpecifier"));
        simpleTypeSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeSpecifierRule>(this, &TypeSpecifierRule::PostSimpleTypeSpecifier));
        cmajor::parsing::NonterminalParser* cVQualifierNonterminalParser = GetNonterminal(ToUtf32("CVQualifier"));
        cVQualifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeSpecifierRule>(this, &TypeSpecifierRule::PostCVQualifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSimpleTypeSpecifier;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCVQualifier;
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
    void PostCVQualifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCVQualifier_value = std::move(stack.top());
            context->fromCVQualifier = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeSpecifier*>*>(fromCVQualifier_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromSimpleTypeSpecifier(), fromCVQualifier() {}
        cmajor::codedom::TypeSpecifier* value;
        cmajor::codedom::TypeSpecifier* fromSimpleTypeSpecifier;
        cmajor::codedom::TypeSpecifier* fromCVQualifier;
    };
};

class Declaration::SimpleTypeSpecifierRule : public cmajor::parsing::Rule
{
public:
    SimpleTypeSpecifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::TypeSpecifier*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeSpecifier*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SimpleTypeSpecifierRule>(this, &SimpleTypeSpecifierRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TypeSpecifier(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::TypeSpecifier* value;
    };
};

class Declaration::TypeNameRule : public cmajor::parsing::Rule
{
public:
    TypeNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::TypeName*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeName*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeNameRule>(this, &TypeNameRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeNameRule>(this, &TypeNameRule::A1Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeNameRule>(this, &TypeNameRule::PostQualifiedId));
        cmajor::parsing::NonterminalParser* templateArgumentListNonterminalParser = GetNonterminal(ToUtf32("TemplateArgumentList"));
        templateArgumentListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TypeNameRule>(this, &TypeNameRule::PreTemplateArgumentList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::codedom::TypeName(std::u32string(matchBegin, matchEnd));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->IsTemplate() = true;
    }
    void PostQualifiedId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context->fromQualifiedId = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
    void PreTemplateArgumentList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeName*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromQualifiedId() {}
        cmajor::codedom::TypeName* value;
        std::u32string fromQualifiedId;
    };
};

class Declaration::TemplateArgumentListRule : public cmajor::parsing::Rule
{
public:
    TemplateArgumentListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::codedom::TypeName*"), ToUtf32("typeName")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> typeName_value = std::move(stack.top());
        context->typeName = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeName*>*>(typeName_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TemplateArgumentListRule>(this, &TemplateArgumentListRule::A0Action));
        cmajor::parsing::NonterminalParser* templateArgumentNonterminalParser = GetNonterminal(ToUtf32("TemplateArgument"));
        templateArgumentNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TemplateArgumentListRule>(this, &TemplateArgumentListRule::PostTemplateArgument));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeName->AddTemplateArgument(context->fromTemplateArgument);
    }
    void PostTemplateArgument(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTemplateArgument_value = std::move(stack.top());
            context->fromTemplateArgument = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::CppObject*>*>(fromTemplateArgument_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): typeName(), fromTemplateArgument() {}
        cmajor::codedom::TypeName* typeName;
        cmajor::codedom::CppObject* fromTemplateArgument;
    };
};

class Declaration::TemplateArgumentRule : public cmajor::parsing::Rule
{
public:
    TemplateArgumentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TemplateArgumentRule>(this, &TemplateArgumentRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TemplateArgumentRule>(this, &TemplateArgumentRule::A1Action));
        cmajor::parsing::NonterminalParser* typeIdNonterminalParser = GetNonterminal(ToUtf32("TypeId"));
        typeIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PostTypeId));
        cmajor::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TemplateArgumentRule>(this, &TemplateArgumentRule::PostAssignmentExpression));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTypeId;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAssignmentExpression;
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
        Context(): value(), fromTypeId(), fromAssignmentExpression() {}
        cmajor::codedom::CppObject* value;
        cmajor::codedom::TypeId* fromTypeId;
        cmajor::codedom::CppObject* fromAssignmentExpression;
    };
};

class Declaration::TypedefRule : public cmajor::parsing::Rule
{
public:
    TypedefRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::DeclSpecifier*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::DeclSpecifier*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypedefRule>(this, &TypedefRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Typedef;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::DeclSpecifier* value;
    };
};

class Declaration::CVQualifierRule : public cmajor::parsing::Rule
{
public:
    CVQualifierRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::TypeSpecifier*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::TypeSpecifier*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CVQualifierRule>(this, &CVQualifierRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CVQualifierRule>(this, &CVQualifierRule::A1Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Const;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Volatile;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::codedom::TypeSpecifier* value;
    };
};

class Declaration::NamespaceAliasDefinitionRule : public cmajor::parsing::Rule
{
public:
    NamespaceAliasDefinitionRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::UsingObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::UsingObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceAliasDefinitionRule>(this, &NamespaceAliasDefinitionRule::A0Action));
        cmajor::parsing::NonterminalParser* identifierNonterminalParser = GetNonterminal(ToUtf32("Identifier"));
        identifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceAliasDefinitionRule>(this, &NamespaceAliasDefinitionRule::PostIdentifier));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceAliasDefinitionRule>(this, &NamespaceAliasDefinitionRule::PostQualifiedId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NamespaceAlias(context->fromIdentifier, context->fromQualifiedId);
    }
    void PostIdentifier(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdentifier_value = std::move(stack.top());
            context->fromIdentifier = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromIdentifier_value.get());
            stack.pop();
        }
    }
    void PostQualifiedId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context->fromQualifiedId = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromIdentifier(), fromQualifiedId() {}
        cmajor::codedom::UsingObject* value;
        std::u32string fromIdentifier;
        std::u32string fromQualifiedId;
    };
};

class Declaration::UsingDeclarationRule : public cmajor::parsing::Rule
{
public:
    UsingDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::UsingObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::UsingObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UsingDeclarationRule>(this, &UsingDeclarationRule::A0Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UsingDeclarationRule>(this, &UsingDeclarationRule::PostQualifiedId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UsingDeclaration(context->fromQualifiedId);
    }
    void PostQualifiedId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context->fromQualifiedId = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromQualifiedId() {}
        cmajor::codedom::UsingObject* value;
        std::u32string fromQualifiedId;
    };
};

class Declaration::UsingDirectiveRule : public cmajor::parsing::Rule
{
public:
    UsingDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::UsingObject*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::UsingObject*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UsingDirectiveRule>(this, &UsingDirectiveRule::A0Action));
        cmajor::parsing::NonterminalParser* qualifiedIdNonterminalParser = GetNonterminal(ToUtf32("QualifiedId"));
        qualifiedIdNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<UsingDirectiveRule>(this, &UsingDirectiveRule::PostQualifiedId));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UsingDirective(context->fromQualifiedId);
    }
    void PostQualifiedId(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromQualifiedId_value = std::move(stack.top());
            context->fromQualifiedId = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromQualifiedId_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromQualifiedId() {}
        cmajor::codedom::UsingObject* value;
        std::u32string fromQualifiedId;
    };
};

void Declaration::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.code.Identifier"));
    if (!grammar0)
    {
        grammar0 = cmajor::code::Identifier::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar1)
    {
        grammar1 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.code.Declarator"));
    if (!grammar2)
    {
        grammar2 = cmajor::code::Declarator::Create(pd);
    }
    AddGrammarReference(grammar2);
    cmajor::parsing::Grammar* grammar3 = pd->GetGrammar(ToUtf32("cmajor.code.Expression"));
    if (!grammar3)
    {
        grammar3 = cmajor::code::Expression::Create(pd);
    }
    AddGrammarReference(grammar3);
}

void Declaration::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Identifier.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("identifier"), this, ToUtf32("cmajor.parsing.stdlib.identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("InitDeclaratorList"), this, ToUtf32("Declarator.InitDeclaratorList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Identifier"), this, ToUtf32("Identifier.Identifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeId"), this, ToUtf32("Declarator.TypeId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("AssignmentExpression"), this, ToUtf32("Expression.AssignmentExpression")));
    AddRule(new BlockDeclarationRule(ToUtf32("BlockDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceAliasDefinition"), ToUtf32("NamespaceAliasDefinition"), 0)),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("UsingDirective"), ToUtf32("UsingDirective"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("UsingDeclaration"), ToUtf32("UsingDeclaration"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::NonterminalParser(ToUtf32("SimpleDeclaration"), ToUtf32("SimpleDeclaration"), 0)))));
    AddRule(new SimpleDeclarationRule(ToUtf32("SimpleDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("DeclSpecifierSeq"), ToUtf32("DeclSpecifierSeq"), 1)),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("InitDeclaratorList"), ToUtf32("InitDeclaratorList"), 0))))),
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new DeclSpecifierSeqRule(ToUtf32("DeclSpecifierSeq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::PositiveParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("DeclSpecifier"), ToUtf32("DeclSpecifier"), 0)))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))));
    AddRule(new DeclSpecifierRule(ToUtf32("DeclSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("StorageClassSpecifier"), ToUtf32("StorageClassSpecifier"), 0)),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("TypeSpecifier"), ToUtf32("TypeSpecifier"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::NonterminalParser(ToUtf32("Typedef"), ToUtf32("Typedef"), 0)))));
    AddRule(new StorageClassSpecifierRule(ToUtf32("StorageClassSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordListParser(ToUtf32("identifier"), keywords0))));
    AddRule(new TypeSpecifierRule(ToUtf32("TypeSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("SimpleTypeSpecifier"), ToUtf32("SimpleTypeSpecifier"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("CVQualifier"), ToUtf32("CVQualifier"), 0)))));
    AddRule(new SimpleTypeSpecifierRule(ToUtf32("SimpleTypeSpecifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordListParser(ToUtf32("identifier"), keywords1))));
    AddRule(new TypeNameRule(ToUtf32("TypeName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0)),
            new cmajor::parsing::OptionalParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                new cmajor::parsing::CharParser('<')),
                            new cmajor::parsing::NonterminalParser(ToUtf32("TemplateArgumentList"), ToUtf32("TemplateArgumentList"), 1)),
                        new cmajor::parsing::CharParser('>')))))));
    AddRule(new TemplateArgumentListRule(ToUtf32("TemplateArgumentList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ListParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TemplateArgument"), ToUtf32("TemplateArgument"), 0)),
            new cmajor::parsing::CharParser(','))));
    AddRule(new TemplateArgumentRule(ToUtf32("TemplateArgument"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TypeId"), ToUtf32("TypeId"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)))));
    AddRule(new TypedefRule(ToUtf32("Typedef"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("typedef")))));
    AddRule(new CVQualifierRule(ToUtf32("CVQualifier"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::KeywordParser(ToUtf32("const"))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::KeywordParser(ToUtf32("volatile"))))));
    AddRule(new NamespaceAliasDefinitionRule(ToUtf32("NamespaceAliasDefinition"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("namespace")),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Identifier"), ToUtf32("Identifier"), 0)),
                    new cmajor::parsing::CharParser('=')),
                new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::CharParser(';')))));
    AddRule(new UsingDeclarationRule(ToUtf32("UsingDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::KeywordParser(ToUtf32("using")),
                new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::CharParser(';')))));
    AddRule(new UsingDirectiveRule(ToUtf32("UsingDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("using")),
                    new cmajor::parsing::KeywordParser(ToUtf32("namespace"))),
                new cmajor::parsing::NonterminalParser(ToUtf32("QualifiedId"), ToUtf32("QualifiedId"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::CharParser(';')))));
}

} } // namespace cmajor.code
