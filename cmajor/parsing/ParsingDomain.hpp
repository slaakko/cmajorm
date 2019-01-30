// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_PARSING_DOMAIN_INCLUDED
#define CMAJOR_PARSING_PARSING_DOMAIN_INCLUDED
#include <cmajor/parsing/ParsingObject.hpp>
#include <string>
#include <stack>
#include <unordered_map>

namespace cmajor { namespace parsing {

class Scope;
class Grammar;
class Namespace;

class PARSING_API ParsingDomain: public ParsingObject
{
public:
    ParsingDomain();
    Scope* GetNamespaceScope(const std::u32string& fullNamespaceName);
    Grammar* GetGrammar(const std::u32string& grammarName);
    void AddGrammar(Grammar* grammar);
    void BeginNamespace(const std::u32string& ns);
    void EndNamespace();
    Namespace* GlobalNamespace() const { return globalNamespace; }
    Namespace* CurrentNamespace() const { return currentNamespace; }
    Scope* CurrentScope() const;
    void Accept(Visitor& visitor) override;
    int GetNextRuleId() { return nextRuleId++; }
    int GetNumRules() const { return nextRuleId; }
private:
    typedef std::unordered_map<std::u32string, Grammar*> GrammarMap;
    typedef GrammarMap::const_iterator GrammarMapIt;
    GrammarMap grammarMap;
    Namespace* globalNamespace;
    Scope* globalScope;
    typedef std::unordered_map<std::u32string, Namespace*> NamespaceMap;
    typedef NamespaceMap::const_iterator NamespaceMapIt;
    NamespaceMap namespaceMap;
    std::stack<Namespace*> namespaceStack;
    Namespace* currentNamespace;
    int nextRuleId;
};

PARSING_API void RegisterParsingDomain(ParsingDomain* parsingDomain);
PARSING_API void ParsingDomainInit();
PARSING_API void ParsingDomainDone();

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_PARSING_DOMAIN_INCLUDED
