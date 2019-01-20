#include "Project.hpp"
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

namespace cmajor { namespace parser {

using namespace cmajor::parsing;
using namespace cmajor::util;
using namespace cmajor::unicode;

Project* Project::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Project* Project::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Project* grammar(new Project(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Project::Project(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Project"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.parser")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::ast::Project* Project::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, std::string config)
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
    stack.push(std::unique_ptr<cmajor::parsing::Object>(new ValueObject<std::string>(config)));
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
    cmajor::ast::Project* result = *static_cast<cmajor::parsing::ValueObject<cmajor::ast::Project*>*>(value.get());
    stack.pop();
    return result;
}

class Project::ProjectRule : public cmajor::parsing::Rule
{
public:
    ProjectRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("std::string"), ToUtf32("config")));
        SetValueTypeName(ToUtf32("cmajor::ast::Project*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> config_value = std::move(stack.top());
        context->config = *static_cast<cmajor::parsing::ValueObject<std::string>*>(config_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::ast::Project*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ProjectRule>(this, &ProjectRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ProjectRule>(this, &ProjectRule::A1Action));
        cmajor::parsing::NonterminalParser* qualified_idNonterminalParser = GetNonterminal(ToUtf32("qualified_id"));
        qualified_idNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ProjectRule>(this, &ProjectRule::Postqualified_id));
        cmajor::parsing::NonterminalParser* declarationNonterminalParser = GetNonterminal(ToUtf32("Declaration"));
        declarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ProjectRule>(this, &ProjectRule::PostDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new cmajor::ast::Project(context->fromqualified_id, fileName, context->config);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value->AddDeclaration(context->fromDeclaration);
    }
    void Postqualified_id(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromqualified_id_value = std::move(stack.top());
            context->fromqualified_id = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromqualified_id_value.get());
            stack.pop();
        }
    }
    void PostDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDeclaration_value = std::move(stack.top());
            context->fromDeclaration = *static_cast<cmajor::parsing::ValueObject<ProjectDeclaration*>*>(fromDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): config(), value(), fromqualified_id(), fromDeclaration() {}
        std::string config;
        cmajor::ast::Project* value;
        std::u32string fromqualified_id;
        ProjectDeclaration* fromDeclaration;
    };
};

class Project::DeclarationRule : public cmajor::parsing::Rule
{
public:
    DeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("ProjectDeclaration*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ProjectDeclaration*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DeclarationRule>(this, &DeclarationRule::A3Action));
        cmajor::parsing::NonterminalParser* referenceDeclarationNonterminalParser = GetNonterminal(ToUtf32("ReferenceDeclaration"));
        referenceDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostReferenceDeclaration));
        cmajor::parsing::NonterminalParser* sourceFileDeclarationNonterminalParser = GetNonterminal(ToUtf32("SourceFileDeclaration"));
        sourceFileDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostSourceFileDeclaration));
        cmajor::parsing::NonterminalParser* textFileDeclarationNonterminalParser = GetNonterminal(ToUtf32("TextFileDeclaration"));
        textFileDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostTextFileDeclaration));
        cmajor::parsing::NonterminalParser* targetDeclarationNonterminalParser = GetNonterminal(ToUtf32("TargetDeclaration"));
        targetDeclarationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<DeclarationRule>(this, &DeclarationRule::PostTargetDeclaration));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromReferenceDeclaration;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSourceFileDeclaration;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTextFileDeclaration;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTargetDeclaration;
    }
    void PostReferenceDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromReferenceDeclaration_value = std::move(stack.top());
            context->fromReferenceDeclaration = *static_cast<cmajor::parsing::ValueObject<ProjectDeclaration*>*>(fromReferenceDeclaration_value.get());
            stack.pop();
        }
    }
    void PostSourceFileDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSourceFileDeclaration_value = std::move(stack.top());
            context->fromSourceFileDeclaration = *static_cast<cmajor::parsing::ValueObject<ProjectDeclaration*>*>(fromSourceFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PostTextFileDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTextFileDeclaration_value = std::move(stack.top());
            context->fromTextFileDeclaration = *static_cast<cmajor::parsing::ValueObject<ProjectDeclaration*>*>(fromTextFileDeclaration_value.get());
            stack.pop();
        }
    }
    void PostTargetDeclaration(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTargetDeclaration_value = std::move(stack.top());
            context->fromTargetDeclaration = *static_cast<cmajor::parsing::ValueObject<ProjectDeclaration*>*>(fromTargetDeclaration_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromReferenceDeclaration(), fromSourceFileDeclaration(), fromTextFileDeclaration(), fromTargetDeclaration() {}
        ProjectDeclaration* value;
        ProjectDeclaration* fromReferenceDeclaration;
        ProjectDeclaration* fromSourceFileDeclaration;
        ProjectDeclaration* fromTextFileDeclaration;
        ProjectDeclaration* fromTargetDeclaration;
    };
};

class Project::ReferenceDeclarationRule : public cmajor::parsing::Rule
{
public:
    ReferenceDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("ProjectDeclaration*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ProjectDeclaration*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ReferenceDeclarationRule>(this, &ReferenceDeclarationRule::A0Action));
        cmajor::parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal(ToUtf32("FilePath"));
        filePathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ReferenceDeclarationRule>(this, &ReferenceDeclarationRule::PostFilePath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ReferenceDeclaration(context->fromFilePath);
    }
    void PostFilePath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFilePath_value = std::move(stack.top());
            context->fromFilePath = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromFilePath() {}
        ProjectDeclaration* value;
        std::string fromFilePath;
    };
};

class Project::SourceFileDeclarationRule : public cmajor::parsing::Rule
{
public:
    SourceFileDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("ProjectDeclaration*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ProjectDeclaration*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceFileDeclarationRule>(this, &SourceFileDeclarationRule::A0Action));
        cmajor::parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal(ToUtf32("FilePath"));
        filePathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceFileDeclarationRule>(this, &SourceFileDeclarationRule::PostFilePath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SourceFileDeclaration(context->fromFilePath);
    }
    void PostFilePath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFilePath_value = std::move(stack.top());
            context->fromFilePath = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromFilePath() {}
        ProjectDeclaration* value;
        std::string fromFilePath;
    };
};

class Project::TextFileDeclarationRule : public cmajor::parsing::Rule
{
public:
    TextFileDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("ProjectDeclaration*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ProjectDeclaration*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TextFileDeclarationRule>(this, &TextFileDeclarationRule::A0Action));
        cmajor::parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal(ToUtf32("FilePath"));
        filePathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TextFileDeclarationRule>(this, &TextFileDeclarationRule::PostFilePath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TextFileDeclaration(context->fromFilePath);
    }
    void PostFilePath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFilePath_value = std::move(stack.top());
            context->fromFilePath = *static_cast<cmajor::parsing::ValueObject<std::string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromFilePath() {}
        ProjectDeclaration* value;
        std::string fromFilePath;
    };
};

class Project::TargetDeclarationRule : public cmajor::parsing::Rule
{
public:
    TargetDeclarationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("ProjectDeclaration*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<ProjectDeclaration*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TargetDeclarationRule>(this, &TargetDeclarationRule::A0Action));
        cmajor::parsing::NonterminalParser* targetNonterminalParser = GetNonterminal(ToUtf32("Target"));
        targetNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<TargetDeclarationRule>(this, &TargetDeclarationRule::PostTarget));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TargetDeclaration(context->fromTarget);
    }
    void PostTarget(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTarget_value = std::move(stack.top());
            context->fromTarget = *static_cast<cmajor::parsing::ValueObject<Target>*>(fromTarget_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromTarget() {}
        ProjectDeclaration* value;
        Target fromTarget;
    };
};

class Project::TargetRule : public cmajor::parsing::Rule
{
public:
    TargetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Target"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Target>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TargetRule>(this, &TargetRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TargetRule>(this, &TargetRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TargetRule>(this, &TargetRule::A2Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Target::program;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Target::library;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = Target::unitTest;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        Target value;
    };
};

class Project::FilePathRule : public cmajor::parsing::Rule
{
public:
    FilePathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::string"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FilePathRule>(this, &FilePathRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = ToUtf8(std::u32string(matchBegin, matchEnd));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        std::string value;
    };
};

void Project::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void Project::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("cmajor.parsing.stdlib.qualified_id")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("cmajor.parsing.stdlib.spaces_and_comments")));
    AddRule(new ProjectRule(ToUtf32("Project"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("project")),
                    new cmajor::parsing::NonterminalParser(ToUtf32("qualified_id"), ToUtf32("qualified_id"), 0)),
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::CharParser(';'))),
            new cmajor::parsing::KleeneStarParser(
                new cmajor::parsing::GroupingParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Declaration"), ToUtf32("Declaration"), 0)))))));
    AddRule(new DeclarationRule(ToUtf32("Declaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("ReferenceDeclaration"), ToUtf32("ReferenceDeclaration"), 0)),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("SourceFileDeclaration"), ToUtf32("SourceFileDeclaration"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("TextFileDeclaration"), ToUtf32("TextFileDeclaration"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A3"),
                new cmajor::parsing::NonterminalParser(ToUtf32("TargetDeclaration"), ToUtf32("TargetDeclaration"), 0)))));
    AddRule(new ReferenceDeclarationRule(ToUtf32("ReferenceDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("reference")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("FilePath"), ToUtf32("FilePath"), 0))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new SourceFileDeclarationRule(ToUtf32("SourceFileDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("source")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("FilePath"), ToUtf32("FilePath"), 0))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new TextFileDeclarationRule(ToUtf32("TextFileDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("text")),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("FilePath"), ToUtf32("FilePath"), 0))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new TargetDeclarationRule(ToUtf32("TargetDeclaration"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::KeywordParser(ToUtf32("target")),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser('='))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("Target"), ToUtf32("Target"), 0))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(';')))))));
    AddRule(new TargetRule(ToUtf32("Target"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::ActionParser(ToUtf32("A0"),
                    new cmajor::parsing::KeywordParser(ToUtf32("program"))),
                new cmajor::parsing::ActionParser(ToUtf32("A1"),
                    new cmajor::parsing::KeywordParser(ToUtf32("library")))),
            new cmajor::parsing::ActionParser(ToUtf32("A2"),
                new cmajor::parsing::KeywordParser(ToUtf32("unitTest"))))));
    AddRule(new FilePathRule(ToUtf32("FilePath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('<'),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::PositiveParser(
                                new cmajor::parsing::CharSetParser(ToUtf32(">"), true))))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser('>'))))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace cmajor.parser
