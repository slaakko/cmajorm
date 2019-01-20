// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/parsing/Namespace.hpp>
#include <cmajor/parsing/Scope.hpp>
#include <cmajor/parsing/Grammar.hpp>
#include <memory>
#include <unordered_set>

namespace cmajor { namespace parsing {

class ParsingDomainRepository
{
public:
    static void Init();
    static void Done();
    static ParsingDomainRepository& Instance();
    void Register(ParsingDomain* parsingDomain);
private:
    ParsingDomainRepository();
    static std::unique_ptr<ParsingDomainRepository> instance;
    static int initCount;
    typedef std::unordered_set<std::unique_ptr<ParsingDomain>> ParsingDomainSet;
    typedef ParsingDomainSet::const_iterator ParsingDomainSetIt;
    ParsingDomainSet parsingDomains;
};

ParsingDomainRepository::ParsingDomainRepository()
{
}

void ParsingDomainRepository::Register(ParsingDomain* parsingDomain)
{
    if (!parsingDomain->IsOwned())
    {
        parsingDomain->SetOwned();
        parsingDomains.insert(std::unique_ptr<ParsingDomain>(parsingDomain));
    }
}

int ParsingDomainRepository::initCount = 0;

void ParsingDomainRepository::Init()
{
    if (initCount++ == 0)
    {
        instance.reset(new ParsingDomainRepository());
    }
}

void ParsingDomainRepository::Done()
{
    if (--initCount == 0)
    {
        instance.reset();
    }
}

ParsingDomainRepository& ParsingDomainRepository::Instance()
{
    return *instance;
}

std::unique_ptr<ParsingDomainRepository> ParsingDomainRepository::instance;

ParsingDomain::ParsingDomain(): ParsingObject(U"parsingDomain", ObjectKind::parsingDomain), 
    globalNamespace(new Namespace(U"", nullptr)), globalScope(globalNamespace->GetScope()), currentNamespace(globalNamespace), nextRuleId(0)
{
    Own(globalNamespace);
    globalScope->SetNs(globalNamespace);
    namespaceMap[currentNamespace->FullName()] = currentNamespace;
}

Scope* ParsingDomain::CurrentScope() const 
{ 
    return currentNamespace->GetScope(); 
}

void ParsingDomain::BeginNamespace(const std::u32string& ns)
{
    std::u32string fullNamespaceName = currentNamespace->FullName().empty() ? ns : currentNamespace->FullName() + U"." + ns;
    namespaceStack.push(currentNamespace);
    NamespaceMapIt i = namespaceMap.find(fullNamespaceName);
    if (i != namespaceMap.end())
    {
        currentNamespace = i->second;
    }
    else
    {
        Namespace* newNs(new Namespace(fullNamespaceName, globalScope));
        Own(newNs);
        newNs->GetScope()->SetNs(newNs);
        namespaceMap[newNs->FullName()] = newNs;
        currentNamespace = newNs;
        globalScope->AddNamespace(newNs);
    }
}

void ParsingDomain::EndNamespace()
{
    currentNamespace = namespaceStack.top();
    namespaceStack.pop();
}

Scope* ParsingDomain::GetNamespaceScope(const std::u32string& fullNamespaceName)
{
    if (fullNamespaceName.empty())
    {
        return globalScope;
    }
    Namespace* ns = globalScope->GetNamespace(fullNamespaceName);
    if (!ns)
    {
        ns = new Namespace(fullNamespaceName, globalScope);
        Own(ns);
        globalScope->AddNamespace(ns);
    }
    return ns->GetScope();
}

Grammar* ParsingDomain::GetGrammar(const std::u32string& grammarName)
{
    GrammarMapIt i = grammarMap.find(grammarName);
    if (i != grammarMap.end())
    {
        return i->second;
    }
    return nullptr;
}

void ParsingDomain::AddGrammar(Grammar* grammar)
{
    Own(grammar);
    std::u32string grammarFullName = grammar->FullName();
    GrammarMapIt i = grammarMap.find(grammarFullName);
    if (i == grammarMap.end())
    {
        grammarMap[grammarFullName] = grammar;
        grammar->SetNs(currentNamespace);
        grammar->AddToScope();
    }
}

void ParsingDomain::Accept(Visitor& visitor)
{
    for (const std::pair<std::u32string, Grammar*>& p : grammarMap)
    {
        Grammar* grammar = p.second;
        grammar->Accept(visitor);
    }
}

void RegisterParsingDomain(ParsingDomain* parsingDomain)
{
    ParsingDomainRepository::Instance().Register(parsingDomain);
}

void ParsingDomainInit()
{
    ParsingDomainRepository::Init();
}

void ParsingDomainDone()
{
    ParsingDomainRepository::Done();
}

} } // namespace cmajor::parsing
