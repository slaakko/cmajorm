#include "Declarator.hpp"
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
#include <cmajor/code/Expression.hpp>
#include <cmajor/code/Declaration.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/codedom/Type.hpp>

namespace cmajor { namespace code {

using cmajor::util::Trim;
using namespace cmajor::codedom;
using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Declarator* Declarator::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Declarator* Declarator::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Declarator* grammar(new Declarator(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Declarator::Declarator(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Declarator"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.code")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::codedom::InitDeclaratorList* Declarator::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    cmajor::codedom::InitDeclaratorList* result = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::InitDeclaratorList*>*>(value.get());
    stack.pop();
    return result;
}

class Declarator::InitDeclaratorListRule : public cmajor::parsing::Rule
{
public:
    InitDeclaratorListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::InitDeclaratorList*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<InitDeclaratorList>"), ToUtf32("idl")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::InitDeclaratorList*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitDeclaratorListRule>(this, &InitDeclaratorListRule::A2Action));
        cmajor::parsing::NonterminalParser* initDeclaratorNonterminalParser = GetNonterminal(ToUtf32("InitDeclarator"));
        initDeclaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitDeclaratorListRule>(this, &InitDeclaratorListRule::PostInitDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->idl.reset(new InitDeclaratorList);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->idl.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->idl->Add(context->fromInitDeclarator);
    }
    void PostInitDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitDeclarator_value = std::move(stack.top());
            context->fromInitDeclarator = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::InitDeclarator*>*>(fromInitDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), idl(), fromInitDeclarator() {}
        cmajor::codedom::InitDeclaratorList* value;
        std::unique_ptr<InitDeclaratorList> idl;
        cmajor::codedom::InitDeclarator* fromInitDeclarator;
    };
};

class Declarator::InitDeclaratorRule : public cmajor::parsing::Rule
{
public:
    InitDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::InitDeclarator*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::InitDeclarator*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitDeclaratorRule>(this, &InitDeclaratorRule::A0Action));
        cmajor::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostDeclarator));
        cmajor::parsing::NonterminalParser* initializerNonterminalParser = GetNonterminal(ToUtf32("Initializer"));
        initializerNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitDeclaratorRule>(this, &InitDeclaratorRule::PostInitializer));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new InitDeclarator(context->fromDeclarator, context->fromInitializer);
    }
    void PostDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
    void PostInitializer(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitializer_value = std::move(stack.top());
            context->fromInitializer = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::Initializer*>*>(fromInitializer_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromDeclarator(), fromInitializer() {}
        cmajor::codedom::InitDeclarator* value;
        std::u32string fromDeclarator;
        cmajor::codedom::Initializer* fromInitializer;
    };
};

class Declarator::DeclaratorRule : public cmajor::parsing::Rule
{
public:
    DeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclaratorRule>(this, &DeclaratorRule::A0Action));
        cmajor::parsing::NonterminalParser* directDeclaratorNonterminalParser = GetNonterminal(ToUtf32("DirectDeclarator"));
        cmajor::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclaratorRule>(this, &DeclaratorRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Trim(std::u32string(matchBegin, matchEnd));
    }
    void PostDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromDeclarator() {}
        std::u32string value;
        std::u32string fromDeclarator;
    };
};

class Declarator::DirectDeclaratorRule : public cmajor::parsing::Rule
{
public:
    DirectDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DirectDeclaratorRule>(this, &DirectDeclaratorRule::A0Action));
        cmajor::parsing::NonterminalParser* declaratorIdNonterminalParser = GetNonterminal(ToUtf32("DeclaratorId"));
        cmajor::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DirectDeclaratorRule>(this, &DirectDeclaratorRule::PostConstantExpression));
        cmajor::parsing::NonterminalParser* declaratorNonterminalParser = GetNonterminal(ToUtf32("Declarator"));
        declaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DirectDeclaratorRule>(this, &DirectDeclaratorRule::PostDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromConstantExpression);
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
    void PostDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclarator_value = std::move(stack.top());
            context->fromDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): o(), fromConstantExpression(), fromDeclarator() {}
        std::unique_ptr<CppObject> o;
        cmajor::codedom::CppObject* fromConstantExpression;
        std::u32string fromDeclarator;
    };
};

class Declarator::DeclaratorIdRule : public cmajor::parsing::Rule
{
public:
    DeclaratorIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclaratorIdRule>(this, &DeclaratorIdRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclaratorIdRule>(this, &DeclaratorIdRule::A1Action));
        cmajor::parsing::NonterminalParser* idExpressionNonterminalParser = GetNonterminal(ToUtf32("IdExpression"));
        idExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclaratorIdRule>(this, &DeclaratorIdRule::PostIdExpression));
        cmajor::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclaratorIdRule>(this, &DeclaratorIdRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromIdExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromTypeName);
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
        Context(): o(), fromIdExpression(), fromTypeName() {}
        std::unique_ptr<CppObject> o;
        cmajor::codedom::IdExpr* fromIdExpression;
        cmajor::codedom::TypeName* fromTypeName;
    };
};

class Declarator::TypeIdRule : public cmajor::parsing::Rule
{
public:
    TypeIdRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeIdRule>(this, &TypeIdRule::A3Action));
        cmajor::parsing::NonterminalParser* typeSpecifierSeqNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifierSeq"));
        typeSpecifierSeqNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<TypeIdRule>(this, &TypeIdRule::PreTypeSpecifierSeq));
        cmajor::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeIdRule>(this, &TypeIdRule::PostTypeName));
        cmajor::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeIdRule>(this, &TypeIdRule::PostAbstractDeclarator));
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
        context->ti->Declarator() = context->fromAbstractDeclarator;
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
    void PostAbstractDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context->fromAbstractDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), ti(), fromTypeName(), fromAbstractDeclarator() {}
        cmajor::codedom::TypeId* value;
        std::unique_ptr<TypeId> ti;
        cmajor::codedom::TypeName* fromTypeName;
        std::u32string fromAbstractDeclarator;
    };
};

class Declarator::TypeRule : public cmajor::parsing::Rule
{
public:
    TypeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Type*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<Type>"), ToUtf32("t")));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Type*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeRule>(this, &TypeRule::A3Action));
        cmajor::parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifier"));
        typeSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeRule>(this, &TypeRule::PostTypeSpecifier));
        cmajor::parsing::NonterminalParser* typeNameNonterminalParser = GetNonterminal(ToUtf32("TypeName"));
        typeNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeRule>(this, &TypeRule::PostTypeName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->t.reset(new Type);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->t.release();
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->t->Add(context->fromTypeSpecifier);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->t->Add(context->fromTypeName);
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
        Context(): value(), t(), fromTypeSpecifier(), fromTypeName() {}
        cmajor::codedom::Type* value;
        std::unique_ptr<Type> t;
        cmajor::codedom::TypeSpecifier* fromTypeSpecifier;
        cmajor::codedom::TypeName* fromTypeName;
    };
};

class Declarator::TypeSpecifierSeqRule : public cmajor::parsing::Rule
{
public:
    TypeSpecifierSeqRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::codedom::TypeId*"), ToUtf32("typeId")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> typeId_value = std::move(stack.top());
        context->typeId = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::TypeId*>*>(typeId_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TypeSpecifierSeqRule>(this, &TypeSpecifierSeqRule::A0Action));
        cmajor::parsing::NonterminalParser* typeSpecifierNonterminalParser = GetNonterminal(ToUtf32("TypeSpecifier"));
        typeSpecifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TypeSpecifierSeqRule>(this, &TypeSpecifierSeqRule::PostTypeSpecifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->typeId->Add(context->fromTypeSpecifier);
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
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): typeId(), fromTypeSpecifier() {}
        cmajor::codedom::TypeId* typeId;
        cmajor::codedom::TypeSpecifier* fromTypeSpecifier;
    };
};

class Declarator::AbstractDeclaratorRule : public cmajor::parsing::Rule
{
public:
    AbstractDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AbstractDeclaratorRule>(this, &AbstractDeclaratorRule::A0Action));
        cmajor::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<AbstractDeclaratorRule>(this, &AbstractDeclaratorRule::PostAbstractDeclarator));
        cmajor::parsing::NonterminalParser* directAbstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("DirectAbstractDeclarator"));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Trim(std::u32string(matchBegin, matchEnd));
    }
    void PostAbstractDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context->fromAbstractDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromAbstractDeclarator() {}
        std::u32string value;
        std::u32string fromAbstractDeclarator;
    };
};

class Declarator::DirectAbstractDeclaratorRule : public cmajor::parsing::Rule
{
public:
    DirectAbstractDeclaratorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::A0Action));
        cmajor::parsing::NonterminalParser* constantExpressionNonterminalParser = GetNonterminal(ToUtf32("ConstantExpression"));
        constantExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::PostConstantExpression));
        cmajor::parsing::NonterminalParser* abstractDeclaratorNonterminalParser = GetNonterminal(ToUtf32("AbstractDeclarator"));
        abstractDeclaratorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DirectAbstractDeclaratorRule>(this, &DirectAbstractDeclaratorRule::PostAbstractDeclarator));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromConstantExpression);
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
    void PostAbstractDeclarator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAbstractDeclarator_value = std::move(stack.top());
            context->fromAbstractDeclarator = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromAbstractDeclarator_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): o(), fromConstantExpression(), fromAbstractDeclarator() {}
        std::unique_ptr<CppObject> o;
        cmajor::codedom::CppObject* fromConstantExpression;
        std::u32string fromAbstractDeclarator;
    };
};

class Declarator::CVQualifierSeqRule : public cmajor::parsing::Rule
{
public:
    CVQualifierSeqRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddLocalVariable(AttrOrVariable(ToUtf32("std::unique_ptr<CppObject>"), ToUtf32("o")));
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CVQualifierSeqRule>(this, &CVQualifierSeqRule::A0Action));
        cmajor::parsing::NonterminalParser* cVQualifierNonterminalParser = GetNonterminal(ToUtf32("CVQualifier"));
        cVQualifierNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CVQualifierSeqRule>(this, &CVQualifierSeqRule::PostCVQualifier));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->o.reset(context->fromCVQualifier);
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
        Context(): o(), fromCVQualifier() {}
        std::unique_ptr<CppObject> o;
        cmajor::codedom::TypeSpecifier* fromCVQualifier;
    };
};

class Declarator::InitializerRule : public cmajor::parsing::Rule
{
public:
    InitializerRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::Initializer*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::Initializer*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerRule>(this, &InitializerRule::A1Action));
        cmajor::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostInitializerClause));
        cmajor::parsing::NonterminalParser* expressionListNonterminalParser = GetNonterminal(ToUtf32("ExpressionList"));
        expressionListNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitializerRule>(this, &InitializerRule::PostExpressionList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Initializer(context->fromInitializerClause, GetEmptyVector());
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Initializer(nullptr, context->fromExpressionList);
    }
    void PostInitializerClause(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitializerClause_value = std::move(stack.top());
            context->fromInitializerClause = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::AssignInit*>*>(fromInitializerClause_value.get());
            stack.pop();
        }
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
        Context(): value(), fromInitializerClause(), fromExpressionList() {}
        cmajor::codedom::Initializer* value;
        cmajor::codedom::AssignInit* fromInitializerClause;
        std::vector<cmajor::codedom::CppObject*> fromExpressionList;
    };
};

class Declarator::InitializerClauseRule : public cmajor::parsing::Rule
{
public:
    InitializerClauseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::codedom::AssignInit*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::AssignInit*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerClauseRule>(this, &InitializerClauseRule::A2Action));
        cmajor::parsing::NonterminalParser* assignmentExpressionNonterminalParser = GetNonterminal(ToUtf32("AssignmentExpression"));
        assignmentExpressionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitializerClauseRule>(this, &InitializerClauseRule::PostAssignmentExpression));
        cmajor::parsing::NonterminalParser* initializerListNonterminalParser = GetNonterminal(ToUtf32("InitializerList"));
        initializerListNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<InitializerClauseRule>(this, &InitializerClauseRule::PreInitializerList));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignInit(context->fromAssignmentExpression);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignInit(nullptr);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AssignInit(nullptr);
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
    void PreInitializerList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::codedom::AssignInit*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromAssignmentExpression() {}
        cmajor::codedom::AssignInit* value;
        cmajor::codedom::CppObject* fromAssignmentExpression;
    };
};

class Declarator::InitializerListRule : public cmajor::parsing::Rule
{
public:
    InitializerListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::codedom::AssignInit*"), ToUtf32("init")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> init_value = std::move(stack.top());
        context->init = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::AssignInit*>*>(init_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitializerListRule>(this, &InitializerListRule::A0Action));
        cmajor::parsing::NonterminalParser* initializerClauseNonterminalParser = GetNonterminal(ToUtf32("InitializerClause"));
        initializerClauseNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<InitializerListRule>(this, &InitializerListRule::PostInitializerClause));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->init->Add(context->fromInitializerClause);
    }
    void PostInitializerClause(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitializerClause_value = std::move(stack.top());
            context->fromInitializerClause = *static_cast<cmajor::parsing::ValueObject<cmajor::codedom::AssignInit*>*>(fromInitializerClause_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): init(), fromInitializerClause() {}
        cmajor::codedom::AssignInit* init;
        cmajor::codedom::AssignInit* fromInitializerClause;
    };
};

void Declarator::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.code.Declaration"));
    if (!grammar0)
    {
        grammar0 = cmajor::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.code.Expression"));
    if (!grammar1)
    {
        grammar1 = cmajor::code::Expression::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void Declarator::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ConstantExpression"), this, ToUtf32("Expression.ConstantExpression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("ExpressionList"), this, ToUtf32("Expression.ExpressionList")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("IdExpression"), this, ToUtf32("Expression.IdExpression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("AssignmentExpression"), this, ToUtf32("Expression.AssignmentExpression")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeSpecifier"), this, ToUtf32("Declaration.TypeSpecifier")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("TypeName"), this, ToUtf32("Declaration.TypeName")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("CVQualifier"), this, ToUtf32("Declaration.CVQualifier")));
    AddRule(new InitDeclaratorListRule(ToUtf32("InitDeclaratorList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ListParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("InitDeclarator"), ToUtf32("InitDeclarator"), 0)),
                        new cmajor::parsing::CharParser(',')))))));
    AddRule(new InitDeclaratorRule(ToUtf32("InitDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0),
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("Initializer"), ToUtf32("Initializer"), 0)))))));
    AddRule(new DeclaratorRule(ToUtf32("Declarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("DirectDeclarator"), ToUtf32("DirectDeclarator"), 0),
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("PtrOperator"), ToUtf32("PtrOperator"), 0),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)))))));
    AddRule(new DirectDeclaratorRule(ToUtf32("DirectDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("DeclaratorId"), ToUtf32("DeclaratorId"), 0),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::SequenceParser(
                                        new cmajor::parsing::CharParser('['),
                                        new cmajor::parsing::OptionalParser(
                                            new cmajor::parsing::GroupingParser(
                                                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))))),
                                    new cmajor::parsing::CharParser(']')),
                                new cmajor::parsing::CharParser('(')),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Declarator"), ToUtf32("Declarator"), 0)),
                        new cmajor::parsing::CharParser(')')))))));
    AddRule(new DeclaratorIdRule(ToUtf32("DeclaratorId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("IdExpression"), ToUtf32("IdExpression"), 0)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0)))));
    AddRule(new TypeIdRule(ToUtf32("TypeId"), GetScope(), GetParsingDomain()->GetNextRuleId(),
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
                                    new cmajor::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0))))))))));
    AddRule(new TypeRule(ToUtf32("Type"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::EmptyParser()),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::PositiveParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("TypeSpecifier"), ToUtf32("TypeSpecifier"), 0)))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("TypeName"), ToUtf32("TypeName"), 0))))))));
    AddRule(new TypeSpecifierSeqRule(ToUtf32("TypeSpecifierSeq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("TypeSpecifier"), ToUtf32("TypeSpecifier"), 0))))));
    AddRule(new AbstractDeclaratorRule(ToUtf32("AbstractDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("PtrOperator"), ToUtf32("PtrOperator"), 0),
                        new cmajor::parsing::OptionalParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0))),
                    new cmajor::parsing::NonterminalParser(ToUtf32("DirectAbstractDeclarator"), ToUtf32("DirectAbstractDeclarator"), 0))))));
    AddRule(new DirectAbstractDeclaratorRule(ToUtf32("DirectAbstractDeclarator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('['),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("ConstantExpression"), ToUtf32("ConstantExpression"), 0))))),
                        new cmajor::parsing::CharParser(']')),
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::NonterminalParser(ToUtf32("AbstractDeclarator"), ToUtf32("AbstractDeclarator"), 0)),
                        new cmajor::parsing::CharParser(')')))))));
    AddRule(new cmajor::parsing::Rule(ToUtf32("PtrOperator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('*'),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("CVQualifierSeq"), ToUtf32("CVQualifierSeq"), 0))),
            new cmajor::parsing::CharParser('&'))));
    AddRule(new CVQualifierSeqRule(ToUtf32("CVQualifierSeq"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("CVQualifier"), ToUtf32("CVQualifier"), 0))))));
    AddRule(new InitializerRule(ToUtf32("Initializer"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('='),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('('),
                            new cmajor::parsing::NonterminalParser(ToUtf32("ExpressionList"), ToUtf32("ExpressionList"), 0)),
                        new cmajor::parsing::CharParser(')')))))));
    AddRule(new InitializerClauseRule(ToUtf32("InitializerClause"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("AssignmentExpression"), ToUtf32("AssignmentExpression"), 0)),
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                            new cmajor::parsing::CharParser('{')),
                        new cmajor::parsing::NonterminalParser(ToUtf32("InitializerList"), ToUtf32("InitializerList"), 1)),
                    new cmajor::parsing::CharParser('}'))),
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('{'),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::CharParser('}'))))));
    AddRule(new InitializerListRule(ToUtf32("InitializerList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ListParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::NonterminalParser(ToUtf32("InitializerClause"), ToUtf32("InitializerClause"), 0)),
            new cmajor::parsing::CharParser(','))));
}

} } // namespace cmajor.code
