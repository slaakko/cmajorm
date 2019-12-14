#include "ProjectFile.hpp"
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

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace soulng::util;
using namespace cmajor::unicode;

ProjectFile* ProjectFile::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

ProjectFile* ProjectFile::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    ProjectFile* grammar(new ProjectFile(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

ProjectFile::ProjectFile(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("ProjectFile"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

Project* ProjectFile::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
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
    Project* result = *static_cast<cmajor::parsing::ValueObject<Project*>*>(value.get());
    stack.pop();
    return result;
}

class ProjectFile::ProjectFileRule : public cmajor::parsing::Rule
{
public:
    ProjectFileRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("Project*"));
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
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Project*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ProjectFileRule>(this, &ProjectFileRule::A0Action));
        cmajor::parsing::NonterminalParser* projectNameNonterminalParser = GetNonterminal(ToUtf32("projectName"));
        projectNameNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ProjectFileRule>(this, &ProjectFileRule::PostprojectName));
        cmajor::parsing::NonterminalParser* projectFileContentNonterminalParser = GetNonterminal(ToUtf32("ProjectFileContent"));
        projectFileContentNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<ProjectFileRule>(this, &ProjectFileRule::PreProjectFileContent));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new Project(ToUtf8(context->fromprojectName), fileName);
    }
    void PostprojectName(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromprojectName_value = std::move(stack.top());
            context->fromprojectName = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromprojectName_value.get());
            stack.pop();
        }
    }
    void PreProjectFileContent(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<Project*>(context->value)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromprojectName() {}
        Project* value;
        std::u32string fromprojectName;
    };
};

class ProjectFile::ProjectFileContentRule : public cmajor::parsing::Rule
{
public:
    ProjectFileContentRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        AddInheritedAttribute(AttrOrVariable(ToUtf32("Project*"), ToUtf32("project")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        std::unique_ptr<cmajor::parsing::Object> project_value = std::move(stack.top());
        context->project = *static_cast<cmajor::parsing::ValueObject<Project*>*>(project_value.get());
        stack.pop();
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ProjectFileContentRule>(this, &ProjectFileContentRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ProjectFileContentRule>(this, &ProjectFileContentRule::A1Action));
        cmajor::parsing::NonterminalParser* sourceNonterminalParser = GetNonterminal(ToUtf32("Source"));
        sourceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ProjectFileContentRule>(this, &ProjectFileContentRule::PostSource));
        cmajor::parsing::NonterminalParser* referenceNonterminalParser = GetNonterminal(ToUtf32("Reference"));
        referenceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ProjectFileContentRule>(this, &ProjectFileContentRule::PostReference));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->project->AddSourceFile(ToUtf8(context->fromSource));
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->project->AddReferenceFile(ToUtf8(context->fromReference));
    }
    void PostSource(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSource_value = std::move(stack.top());
            context->fromSource = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromSource_value.get());
            stack.pop();
        }
    }
    void PostReference(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromReference_value = std::move(stack.top());
            context->fromReference = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromReference_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): project(), fromSource(), fromReference() {}
        Project* project;
        std::u32string fromSource;
        std::u32string fromReference;
    };
};

class ProjectFile::SourceRule : public cmajor::parsing::Rule
{
public:
    SourceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SourceRule>(this, &SourceRule::A0Action));
        cmajor::parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal(ToUtf32("FilePath"));
        filePathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<SourceRule>(this, &SourceRule::PostFilePath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFilePath;
    }
    void PostFilePath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFilePath_value = std::move(stack.top());
            context->fromFilePath = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromFilePath() {}
        std::u32string value;
        std::u32string fromFilePath;
    };
};

class ProjectFile::ReferenceRule : public cmajor::parsing::Rule
{
public:
    ReferenceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ReferenceRule>(this, &ReferenceRule::A0Action));
        cmajor::parsing::NonterminalParser* filePathNonterminalParser = GetNonterminal(ToUtf32("FilePath"));
        filePathNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<ReferenceRule>(this, &ReferenceRule::PostFilePath));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFilePath;
    }
    void PostFilePath(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFilePath_value = std::move(stack.top());
            context->fromFilePath = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromFilePath_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromFilePath() {}
        std::u32string value;
        std::u32string fromFilePath;
    };
};

class ProjectFile::FilePathRule : public cmajor::parsing::Rule
{
public:
    FilePathRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
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
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FilePathRule>(this, &FilePathRule::A0Action));
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

void ProjectFile::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
}

void ProjectFile::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("qualified_id"), this, ToUtf32("cmajor.parsing.stdlib.qualified_id")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("spaces_and_comments"), this, ToUtf32("cmajor.parsing.stdlib.spaces_and_comments")));
    AddRule(new ProjectFileRule(ToUtf32("ProjectFile"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("project")),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("projectName"), ToUtf32("qualified_id"), 0))),
                new cmajor::parsing::CharParser(';')),
            new cmajor::parsing::NonterminalParser(ToUtf32("ProjectFileContent"), ToUtf32("ProjectFileContent"), 1))));
    AddRule(new ProjectFileContentRule(ToUtf32("ProjectFileContent"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::KleeneStarParser(
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Source"), ToUtf32("Source"), 0)),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Reference"), ToUtf32("Reference"), 0)))))));
    AddRule(new SourceRule(ToUtf32("Source"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("source")),
                        new cmajor::parsing::NonterminalParser(ToUtf32("FilePath"), ToUtf32("FilePath"), 0)),
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new ReferenceRule(ToUtf32("Reference"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::KeywordParser(ToUtf32("reference")),
                        new cmajor::parsing::NonterminalParser(ToUtf32("FilePath"), ToUtf32("FilePath"), 0)),
                    new cmajor::parsing::CharParser(';'))))));
    AddRule(new FilePathRule(ToUtf32("FilePath"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::TokenParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('<'),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::GroupingParser(
                            new cmajor::parsing::PositiveParser(
                                new cmajor::parsing::CharSetParser(ToUtf32(">\r\n"), true))))),
                new cmajor::parsing::CharParser('>')))));
    SetSkipRuleName(ToUtf32("spaces_and_comments"));
}

} } // namespace cmajor.syntax
