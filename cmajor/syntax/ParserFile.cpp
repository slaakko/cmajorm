#include "ParserFile.hpp"
#include <cmajor/parsing/Action.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/parsing/Primitive.hpp>
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/Nonterminal.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/parsing/StdLib.hpp>
#include <cmajor/parsing/XmlLog.hpp>
#include <soulng/util/Unicode.hpp>
#include <cmajor/parsing/Namespace.hpp>
#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/code/Declaration.hpp>
#include <cmajor/syntax/Element.hpp>
#include <cmajor/syntax/Grammar.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace soulng::util;
using namespace cmajor::unicode;

ParserFile* ParserFile::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

ParserFile* ParserFile::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ParserFile* grammar(new ParserFile(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ParserFile::ParserFile(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("ParserFile"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

ParserFileContent* ParserFile::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, int id_, cmajor::parsing::ParsingDomain* parsingDomain_)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<int>(id_)));
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<cmajor::parsing::ParsingDomain*>(parsingDomain_)));
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
    ParserFileContent* result = *static_cast<cmajor::parsing::ValueObject<ParserFileContent*>*>(value.get());
    stack.pop();
    return result;
}

class ParserFile::ParserFileRule : public cmajor::parsing::Rule
{
public:
    ParserFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("int"), ToUtf32("id_")));
        AddInheritedAttribute(AttrOrVariable(ToUtf32("cmajor::parsing::ParsingDomain*"), ToUtf32("parsingDomain_")));
        SetValueTypeName(ToUtf32("ParserFileContent*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parsingDomain__value = std::move(stack.top());
        context->parsingDomain_ = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::ParsingDomain*>*>(parsingDomain__value.get());
        stack.pop();
        std::unique_ptr<cmajor::parsing::Object> id__value = std::move(stack.top());
        context->id_ = *static_cast<cmajor::parsing::ValueObject<int>*>(id__value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParserFileContent*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ParserFileRule>(this, &ParserFileRule::A0Action));
        cmajor::parsing::NonterminalParser* includeDirectivesNonterminalParser = GetNonterminal(ToUtf32("IncludeDirectives"));
        includeDirectivesNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreIncludeDirectives));
        cmajor::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ParserFileRule>(this, &ParserFileRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParserFileContent(context->id_, context->parsingDomain_);
        context->value->SetFilePath(fileName);
    }
    void PreIncludeDirectives(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParserFileContent*>(context->value)));
    }
    void PreNamespaceContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParserFileContent*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): id_(), parsingDomain_(), value() {}
        int id_;
        cmajor::parsing::ParsingDomain* parsingDomain_;
        ParserFileContent* value;
    };
};

class ParserFile::IncludeDirectivesRule : public cmajor::parsing::Rule
{
public:
    IncludeDirectivesRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<cmajor::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::NonterminalParser* includeDirectiveNonterminalParser = GetNonterminal(ToUtf32("IncludeDirective"));
        includeDirectiveNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<IncludeDirectivesRule>(this, &IncludeDirectivesRule::PreIncludeDirective));
    }
    void PreIncludeDirective(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): parserFileContent() {}
        ParserFileContent* parserFileContent;
    };
};

class ParserFile::IncludeDirectiveRule : public cmajor::parsing::Rule
{
public:
    IncludeDirectiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<cmajor::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IncludeDirectiveRule>(this, &IncludeDirectiveRule::A0Action));
        cmajor::parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal(ToUtf32("FileAttribute"));
        fileAttributeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostFileAttribute));
        cmajor::parsing::NonterminalParser* includeFileNameNonterminalParser = GetNonterminal(ToUtf32("IncludeFileName"));
        includeFileNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<IncludeDirectiveRule>(this, &IncludeDirectiveRule::PostIncludeFileName));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->AddIncludeDirective(ToUtf8(context->fromIncludeFileName), ToUtf8(context->fromFileAttribute));
    }
    void PostFileAttribute(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context->fromFileAttribute = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromFileAttribute_value.get());
            stack.pop();
        }
    }
    void PostIncludeFileName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIncludeFileName_value = std::move(stack.top());
            context->fromIncludeFileName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromIncludeFileName_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): parserFileContent(), fromFileAttribute(), fromIncludeFileName() {}
        ParserFileContent* parserFileContent;
        std::u32string fromFileAttribute;
        std::u32string fromIncludeFileName;
    };
};

class ParserFile::FileAttributeRule : public cmajor::parsing::Rule
{
public:
    FileAttributeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FileAttributeRule>(this, &FileAttributeRule::A0Action));
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

class ParserFile::IncludeFileNameRule : public cmajor::parsing::Rule
{
public:
    IncludeFileNameRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IncludeFileNameRule>(this, &IncludeFileNameRule::A0Action));
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

class ParserFile::NamespaceContentRule : public cmajor::parsing::Rule
{
public:
    NamespaceContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<cmajor::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceContentRule>(this, &NamespaceContentRule::A3Action));
        cmajor::parsing::NonterminalParser* fileAttributeNonterminalParser = GetNonterminal(ToUtf32("FileAttribute"));
        fileAttributeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostFileAttribute));
        cmajor::parsing::NonterminalParser* usingDeclarationNonterminalParser = GetNonterminal(ToUtf32("UsingDeclaration"));
        usingDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDeclaration));
        cmajor::parsing::NonterminalParser* usingDirectiveNonterminalParser = GetNonterminal(ToUtf32("UsingDirective"));
        usingDirectiveNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostUsingDirective));
        cmajor::parsing::NonterminalParser* namespaceAliasDefinitionNonterminalParser = GetNonterminal(ToUtf32("NamespaceAliasDefinition"));
        namespaceAliasDefinitionNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostNamespaceAliasDefinition));
        cmajor::parsing::NonterminalParser* grammarNonterminalParser = GetNonterminal(ToUtf32("Grammar"));
        grammarNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreGrammar));
        grammarNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceContentRule>(this, &NamespaceContentRule::PostGrammar));
        cmajor::parsing::NonterminalParser* namespaceNonterminalParser = GetNonterminal(ToUtf32("Namespace"));
        namespaceNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceContentRule>(this, &NamespaceContentRule::PreNamespace));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromUsingDeclaration->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromUsingDeclaration);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromUsingDirective->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromUsingDirective);
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->fromNamespaceAliasDefinition->SetFileAttr(context->fromFileAttribute);
        context->parserFileContent->AddUsingObject(context->fromNamespaceAliasDefinition);
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->AddGrammar(context->fromGrammar);
    }
    void PostFileAttribute(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFileAttribute_value = std::move(stack.top());
            context->fromFileAttribute = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromFileAttribute_value.get());
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
    void PreGrammar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Scope*>(context->parserFileContent->CurrentScope())));
    }
    void PostGrammar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGrammar_value = std::move(stack.top());
            context->fromGrammar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Grammar*>*>(fromGrammar_value.get());
            stack.pop();
        }
    }
    void PreNamespace(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): parserFileContent(), fromFileAttribute(), fromUsingDeclaration(), fromUsingDirective(), fromNamespaceAliasDefinition(), fromGrammar() {}
        ParserFileContent* parserFileContent;
        std::u32string fromFileAttribute;
        cmajor::codedom::UsingObject* fromUsingDeclaration;
        cmajor::codedom::UsingObject* fromUsingDirective;
        cmajor::codedom::UsingObject* fromNamespaceAliasDefinition;
        cmajor::parsing::Grammar* fromGrammar;
    };
};

class ParserFile::NamespaceRule : public cmajor::parsing::Rule
{
public:
    NamespaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("ParserFileContent*"), ToUtf32("parserFileContent")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> parserFileContent_value = std::move(stack.top());
        context->parserFileContent = *static_cast<cmajor::parsing::ValueObject<ParserFileContent*>*>(parserFileContent_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NamespaceRule>(this, &NamespaceRule::A1Action));
        cmajor::parsing::NonterminalParser* nsNonterminalParser = GetNonterminal(ToUtf32("ns"));
        nsNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<NamespaceRule>(this, &NamespaceRule::Postns));
        cmajor::parsing::NonterminalParser* namespaceContentNonterminalParser = GetNonterminal(ToUtf32("NamespaceContent"));
        namespaceContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<NamespaceRule>(this, &NamespaceRule::PreNamespaceContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->BeginNamespace(context->fromns);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->parserFileContent->EndNamespace();
    }
    void Postns(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromns_value = std::move(stack.top());
            context->fromns = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromns_value.get());
            stack.pop();
        }
    }
    void PreNamespaceContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ParserFileContent*>(context->parserFileContent)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): parserFileContent(), fromns() {}
        ParserFileContent* parserFileContent;
        std::u32string fromns;
    };
};

void ParserFile::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.syntax.GrammarGrammar"));
    if (!grammar0)
    {
        grammar0 = cmajor::syntax::GrammarGrammar::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar1)
    {
        grammar1 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar1);
    cmajor::parsing::Grammar* grammar2 = pd->GetGrammar(ToUtf32("cmajor.code.Declaration"));
    if (!grammar2)
    {
        grammar2 = cmajor::code::Declaration::Create(pd);
    }
    AddGrammarReference(grammar2);
}

void ParserFile::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("Grammar"), this, ToUtf32("GrammarGrammar.Grammar")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("newline"), this, ToUtf32("cmajor.parsing.stdlib.newline")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("cmajor.parsing.stdlib.spaces_and_comments")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("cmajor.parsing.stdlib.qualified_id")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("UsingDeclaration"), this, ToUtf32("cmajor.code.Declaration.UsingDeclaration")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("UsingDirective"), this, ToUtf32("cmajor.code.Declaration.UsingDirective")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("NamespaceAliasDefinition"), this, ToUtf32("cmajor.code.Declaration.NamespaceAliasDefinition")));
    AddRule(new ParserFileRule(ToUtf32("ParserFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::EmptyParser()),
                new cmajor::parsing::OptionalParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("IncludeDirectives"), ToUtf32("IncludeDirectives"), 1))),
            new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1))));
    AddRule(new IncludeDirectivesRule(ToUtf32("IncludeDirectives"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::PositiveParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("IncludeDirective"), ToUtf32("IncludeDirective"), 1))));
    AddRule(new IncludeDirectiveRule(ToUtf32("IncludeDirective"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::OptionalParser(
                        new cmajor::parsing::NonterminalParser(ToUtf32("FileAttribute"), ToUtf32("FileAttribute"), 0)),
                    new cmajor::parsing::CharParser('#')),
                new cmajor::parsing::KeywordParser(ToUtf32("include"))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::TokenParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("IncludeFileName"), ToUtf32("IncludeFileName"), 0),
                            new cmajor::parsing::KleeneStarParser(
                                new cmajor::parsing::CharSetParser(ToUtf32("\r\n"), true))),
                        new cmajor::parsing::NonterminalParser(ToUtf32("newline"), ToUtf32("newline"), 0)))))));
    AddRule(new FileAttributeRule(ToUtf32("FileAttribute"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::CharParser('['),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::GroupingParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::StringParser(ToUtf32("cpp")),
                            new cmajor::parsing::StringParser(ToUtf32("hpp")))))),
            new cmajor::parsing::CharParser(']'))));
    AddRule(new IncludeFileNameRule(ToUtf32("IncludeFileName"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('<'),
                        new cmajor::parsing::PositiveParser(
                            new cmajor::parsing::CharSetParser(ToUtf32(">\r\n"), true))),
                    new cmajor::parsing::CharParser('>'))))));
    AddRule(new NamespaceContentRule(ToUtf32("NamespaceContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("FileAttribute"), ToUtf32("FileAttribute"), 0)),
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("UsingDeclaration"), ToUtf32("UsingDeclaration"), 0)),
                                        new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("UsingDirective"), ToUtf32("UsingDirective"), 0))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceAliasDefinition"), ToUtf32("NamespaceAliasDefinition"), 0))))),
                        new cmajor::parsing::ActionParser(ToUtf32("A3"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("Grammar"), ToUtf32("Grammar"), 1))),
                    new cmajor::parsing::NonterminalParser(ToUtf32("Namespace"), ToUtf32("Namespace"), 1))))));
    AddRule(new NamespaceRule(ToUtf32("Namespace"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("namespace")),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("ns"), ToUtf32("qualified_id"), 0)))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('{'))),
                new cmajor::parsing::NonterminalParser(ToUtf32("NamespaceContent"), ToUtf32("NamespaceContent"), 1)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('}'))))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace cmajor.syntax
