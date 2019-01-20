// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Scope.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/parsing/Visitor.hpp>
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/XmlLog.hpp>
#include <cmajor/parsing/Linking.hpp>
#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace parsing {

using namespace cmajor::util;
using namespace cmajor::unicode;

Grammar::Grammar(const std::u32string& name_, Scope* enclosingScope_) :
    ParsingObject(name_, enclosingScope_, ObjectKind::grammar), parsingDomain(new ParsingDomain()), ns(nullptr),
    linking(false), linked(false), contentParser(nullptr), startRule(nullptr), skipRule(nullptr), log(0), maxLogLineLength(80)
{
    RegisterParsingDomain(parsingDomain);
    SetScope(new Scope(Name(), EnclosingScope()));
}

Grammar::Grammar(const std::u32string& name_, Scope* enclosingScope_, ParsingDomain* parsingDomain_) : 
    ParsingObject(name_, enclosingScope_, ObjectKind::grammar), parsingDomain(parsingDomain_), ns(nullptr),
    linking(false), linked(false), contentParser(nullptr), startRule(nullptr), skipRule(nullptr), log(0), maxLogLineLength(80)
{
    SetScope(new Scope(Name(), EnclosingScope()));
}

void Grammar::AddRule(Rule* rule)
{
    Own(rule);
    rule->SetGrammar(this);
    rule->GetScope()->SetEnclosingScope(GetScope());
    rule->SetEnclosingScope(GetScope());
    rules.push_back(rule);
    try
    {
        GetScope()->Add(rule);
    }
    catch (std::exception& ex)
    {
        ThrowException(ex.what(), GetSpan());
    }
}

Rule* Grammar::GetRule(const std::u32string& ruleName) const
{
    ParsingObject* object = GetScope()->Get(ruleName, ObjectKind::rule | ObjectKind::ruleLink);
    if (!object)
    {
        ThrowException("rule '" + ToUtf8(ruleName) + "' not found in grammar '" + ToUtf8(FullName()) + "'", GetSpan());
    }
    if (object->IsRule())
    {
        return static_cast<Rule*>(object);
    }
    else if (object->IsRuleLink())
    {
        RuleLink* ruleLink = static_cast<RuleLink*>(object);
        Rule* rule = ruleLink->GetRule();
        if (rule)
        {
            return rule;
        }
        else
        {
            ThrowException("rule link '" + ToUtf8(ruleName) + "' not bound in grammar '" + ToUtf8(FullName()) + "'", GetSpan());
        }
    }
    else
    {
        ThrowException("'" + ToUtf8(ruleName) + "' is not a rule or rule link", GetSpan());
    }
    return nullptr;
}

void Grammar::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    RuleLinkSetIt e = ruleLinks.cend();
    for (RuleLinkSetIt i = ruleLinks.cbegin(); i != e; ++i)
    {
        RuleLink* ruleLink = *i;
        ruleLink->Accept(visitor);
    }
    int n = int(rules.size());
    for (int i = 0; i < n; ++i)
    {
        Rule* rule = rules[i];
        rule->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

void Grammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingData* parsingData)
{
    Scanner scanner(start, end, fileName, fileIndex, skipRule);
    scanner.SetParsingData(parsingData);
    std::unique_ptr<XmlLog> xmlLog;
    if (log)
    {
        xmlLog.reset(new XmlLog(*log, maxLogLineLength));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
        xmlLog->IncIndent();
    }
    ObjectStack stack;
    Match match = Parse(scanner, stack, parsingData);
    Span stop = scanner.GetSpan();
    if (log)
    {
        xmlLog->DecIndent();
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (startRule)
        {
            throw ExpectationFailure(startRule->Info(), fileName, stop, start, end);
        }
        else
        {
            throw ParsingException("grammar '" + ToUtf8(FullName()) + "' has no start rule", fileName, stop, start, end);
        }
    }
}

Match Grammar::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (startRule)
    {
        if (!contentParser)
        {
            if (skipRule)
            {
                contentParser = new SequenceParser(new SequenceParser(new OptionalParser(skipRule), startRule), new OptionalParser(skipRule));
                Own(contentParser);
            }
            else
            {
                contentParser = startRule;
            }
        }
        return contentParser->Parse(scanner, stack, parsingData);
    }
    return Match::Nothing();
}

void Grammar::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
{
    std::unique_ptr<ParsingData> parsingData(new ParsingData(parsingDomain->GetNumRules()));
    Parse(start, end, fileIndex, fileName, parsingData.get());
}

Match Grammar::Parse(Scanner& scanner, ObjectStack& stack)
{
    std::unique_ptr<ParsingData> parsingData(new ParsingData(parsingDomain->GetNumRules()));
    return Parse(scanner, stack, parsingData.get());
}

void Grammar::ResolveStartRule()
{
    if (startRuleName.empty())
    {
        if (!rules.empty())
        {
            startRule = rules.front();
        }
        else
        {
            ThrowException("cannot resolve start rule because grammar '" + ToUtf8(FullName()) + "' is empty", GetSpan());
        }
    }
    else
    {
        startRule = GetRule(startRuleName);
    }
}

void Grammar::ResolveSkipRule()
{
    if (!skipRuleName.empty())
    {
        skipRule = GetRule(skipRuleName);
    }
}

void Grammar::Link()
{
    if (!linked && !linking)
    {
        linking = true;
        GetReferencedGrammars();
        GrammarSetIt e = grammarReferences.end();
        for (GrammarSetIt i = grammarReferences.begin(); i != e; ++i)
        {
            Grammar* grammarReference = *i;
            grammarReference->Link();
        }
        LinkerVisitor linkerVisitor;
        Accept(linkerVisitor);
        linking = false;
        linked = true;
    }
}

void Grammar::AddGrammarReference(Grammar* grammarReference)
{
    Own(grammarReference);
    grammarReferences.insert(grammarReference);
}

void Grammar::AddRuleLink(RuleLink* ruleLink)
{
    Own(ruleLink);
    ruleLinks.insert(ruleLink);
}

} } // namespace cmajor::parsing
