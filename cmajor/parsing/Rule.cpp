// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/Scope.hpp>
#include <cmajor/parsing/Action.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/parsing/Nonterminal.hpp>
#include <cmajor/parsing/XmlLog.hpp>
#include <cmajor/parsing/Visitor.hpp>
#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/codedom/Visitor.hpp>
#include <cmajor/codedom/Expression.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace parsing {

using namespace cmajor::util;
using namespace cmajor::unicode;
using namespace cmajor::unicode;

AttrOrVariable::AttrOrVariable(const std::u32string& typeName_, const std::u32string& name_): typeName(typeName_), name(name_) 
{
}

Rule::Rule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_) :
    Parser(name_, U"<" + name_ + U">", ObjectKind::rule),
    id(id_),
    grammar(nullptr),
    definition(definition_),
    inheritedAttributes(),
    localVariables(),
    valueTypeName(),
    actions(),
    nonterminals()
{
    Own(definition);
    SetScope(new Scope(Name(), EnclosingScope()));
}

Rule::Rule(const std::u32string& name_, Scope* enclosingScope_, Parser* definition_) :
    Parser(name_, U"<" + name_ + U">", ObjectKind::rule),
    id(-1),
    grammar(nullptr),
    definition(definition_),
    inheritedAttributes(),
    localVariables(),
    valueTypeName(),
    actions(),
    nonterminals()
{
    Own(definition);
    SetScope(new Scope(Name(), EnclosingScope()));
}

Rule::Rule(const std::u32string& name_, Scope* enclosingScope_) : 
    Parser(name_, U"<" + name_ + U">", ObjectKind::rule),
    id(-1),
    grammar(nullptr),
    definition(),
    inheritedAttributes(),
    localVariables(),
    valueTypeName(),
    actions(),
    nonterminals()
{
    SetScope(new Scope(Name(), EnclosingScope()));
}

void Rule::SetDefinition(Parser* definition_) 
{ 
    definition = definition_; 
    Own(definition);
}

void Rule::AddAction(ActionParser* action)
{
    Own(action);
    actions.push_back(action);
    action->SetEnclosingScope(GetScope());
    action->AddToScope();
}

ActionParser* Rule::GetAction(const std::u32string& actionName) const
{
    ParsingObject* object = GetScope()->Get(actionName, ObjectKind::parser);
    if (!object)
    {
        ThrowException("action '" + ToUtf8(actionName) + "' not found in rule '" + ToUtf8(FullName()) + "'", GetSpan());
    }
    else if (object->IsActionParser())
    {
        return static_cast<ActionParser*>(object);
    }
    else
    {
        ThrowException("'" + ToUtf8(actionName) + "' is not an action", GetSpan());
    }
    return nullptr;
}

void Rule::AddNonterminal(NonterminalParser* nonterminal)
{
    Own(nonterminal);
    nonterminals.push_back(nonterminal);
    nonterminal->SetEnclosingScope(GetScope());
    nonterminal->AddToScope();
}

NonterminalParser* Rule::GetNonterminal(const std::u32string& nonterminalName) const
{
    ParsingObject* object = GetScope()->Get(nonterminalName, ObjectKind::parser);
    if (!object)
    {
        ThrowException("nonterminal '" + ToUtf8(nonterminalName) + "' not found", GetSpan());
    }
    else if (object->IsNonterminalParser())
    {
        return static_cast<NonterminalParser*>(object);
    }
    else
    {
        ThrowException("'" + ToUtf8(nonterminalName) + "' is not a nonterminal", GetSpan());
    }
    return nullptr;
}

using cmajor::codedom::IdExpr;

class CodeExpandingVisitor : public cmajor::codedom::Visitor
{
public:
    CodeExpandingVisitor(std::unordered_map<std::u32string, std::u32string>& expandMap_): expandMap(expandMap_) {}
    virtual void Visit(IdExpr& expr)
    {
        std::unordered_map<std::u32string, std::u32string>::const_iterator i = expandMap.find(expr.Name());
        if (i != expandMap.end())
        {
            expr.SetName(i->second);
        }
    }
private:
    std::unordered_map<std::u32string, std::u32string>& expandMap;
};

void Rule::ExpandCode()
{
    std::unordered_map<std::u32string, std::u32string> expandMap;
    expandMap[U"value"] = U"context->value";
    int n = int(nonterminals.size());
    for (int i = 0; i < n; ++i)
    {
        NonterminalParser* nonterminal = nonterminals[i];
        expandMap[nonterminal->Name()] = U"context->" + nonterminal->ValueFieldName();
    }
    n = int(inheritedAttributes.size());
    for (int i = 0; i < n; ++i)
    {
        const AttrOrVariable& attr = inheritedAttributes[i];
        expandMap[attr.Name()] = U"context->" + attr.Name();
    }
    n = int(localVariables.size());
    for (int i = 0; i < n; ++i)
    {
        const AttrOrVariable& var = localVariables[i];
        expandMap[var.Name()] = U"context->" + var.Name();
    }
    CodeExpandingVisitor visitor(expandMap);
    n = int(actions.size());
    for (int i = 0; i < n; ++i)
    {
        ActionParser* action = actions[i];
        action->SuccessCode()->Accept(visitor);
        if (action->FailCode())
        {
            action->FailCode()->Accept(visitor);
        }
    }
    n = int(nonterminals.size());
    for (int i = 0; i < n; ++i)
    {
        NonterminalParser* nonterminal = nonterminals[i];
        if (!nonterminal->Arguments().empty())
        {
            int m = int(nonterminal->Arguments().size());
            for (int j = 0; j < m; ++j)
            {
                cmajor::codedom::CppObject* argument = nonterminal->Arguments()[j];
                argument->Accept(visitor);
            }
        }
    }
}

Match Rule::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    bool writeToLog = !scanner.Skipping() && scanner.Log();
    if (writeToLog)
    {
        scanner.Log()->WriteBeginRule(ToUtf8(Name()));
        scanner.Log()->IncIndent();
        scanner.Log()->WriteTry(scanner.RestOfLine(scanner.Log()->MaxLineLength()));
        scanner.Log()->IncIndent();
    }
    int startIndex = scanner.GetSpan().Start();
    Enter(stack, parsingData);
    Match match = definition ? definition->Parse(scanner, stack, parsingData) : Match::Nothing();
    Leave(stack, parsingData, match.Hit());
    if (writeToLog)
    {
        scanner.Log()->DecIndent();
        if (match.Hit())
        {
            std::string matched(scanner.Start() + startIndex, scanner.Start() + scanner.GetSpan().Start());
            scanner.Log()->WriteSuccess(matched);
        }
        else
        {
            scanner.Log()->WriteFail();
        }
        scanner.Log()->DecIndent();
        scanner.Log()->WriteEndRule(ToUtf8(Name()));
    }
    return match;
}

void Rule::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    if (definition)
    {
        definition->Accept(visitor);
    }
    visitor.EndVisit(*this);
}

std::u32string GetPrefix(const std::u32string& fullName)
{
    std::u32string::size_type dotPos = fullName.find('.');
    if (dotPos != std::u32string::npos)
    {
        return fullName.substr(0, dotPos);
    }
    return fullName;
}

std::u32string GetCommonName(const std::u32string& qualifiedId)
{
    std::u32string::size_type lastDotPos = qualifiedId.rfind('.');
    if (lastDotPos != std::u32string::npos)
    {
        return qualifiedId.substr(lastDotPos + 1);
    }
    return qualifiedId;
}

RuleLink::RuleLink(const std::u32string& name_, Grammar* grammar_, const std::u32string& linkedRuleName_): 
    ParsingObject(name_, grammar_->GetScope(), ObjectKind::ruleLink), linkedRuleName(linkedRuleName_), grammar(grammar_)
{
    std::u32string prefix = GetPrefix(linkedRuleName);
    if (prefix == U"stdlib")
    {
        linkedRuleName = U"cmajor.parsing.stdlib." + GetCommonName(linkedRuleName);
    }
}

RuleLink::RuleLink(Grammar* grammar_, const std::u32string& linkedRuleName_) : 
    ParsingObject(GetCommonName(linkedRuleName_), grammar_->GetScope(), ObjectKind::ruleLink), linkedRuleName(linkedRuleName_), grammar(grammar_)
{
    std::u32string prefix = GetPrefix(linkedRuleName);
    if (prefix == U"stdlib")
    {
        linkedRuleName = U"cmajor.parsing.stdlib." + GetCommonName(linkedRuleName);
    }
}

void RuleLink::AddToScope()
{
    ParsingObject::AddToScope();
    grammar->AddRuleLink(this);
}

void RuleLink::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cmajor::parsing
