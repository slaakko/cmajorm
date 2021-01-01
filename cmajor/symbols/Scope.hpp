// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SCOPE_INCLUDED
#define CMAJOR_SYMBOLS_SCOPE_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <sngcm/ast/Namespace.hpp>
#include <unordered_map>
#include <unordered_set>

namespace cmajor { namespace symbols {

using soulng::lexer::Span;
using namespace sngcm::ast;

class Symbol;
class ContainerSymbol;
class NamespaceSymbol;
class FunctionSymbol;
class Module;
class ViableFunctionSet;

enum class ScopeLookup : uint8_t
{
    none = 0,
    this_ = 1 << 0,
    base = 1 << 1,
    parent = 1 << 2,
    this_and_base = this_ | base,
    this_and_parent = this_ | parent,
    this_and_base_and_parent = this_ | base | parent,
    fileScopes = 1 << 3
};

inline ScopeLookup operator&(ScopeLookup left, ScopeLookup right)
{
    return ScopeLookup(uint8_t(left) & uint8_t(right));
}

inline ScopeLookup operator~(ScopeLookup subject)
{
    return ScopeLookup(~uint8_t(subject));
}

class SYMBOLS_API Scope
{
public:
    virtual ~Scope();
    virtual Symbol* Lookup(const std::u32string& name) const = 0;
    virtual Symbol* Lookup(const std::u32string& name, ScopeLookup lookup) const = 0;
};

class SYMBOLS_API ContainerScope : public Scope
{
public:
    ContainerScope();
    ContainerScope* BaseScope() const;
    ContainerScope* ParentScope() const;
    void SetParentScope(ContainerScope* parentScope_) { parentScope = parentScope_; }
    ContainerSymbol* Container() { return container; }
    void SetContainer(ContainerSymbol* container_) { container = container_; }
    void Install(Symbol* symbol);
    Symbol* Lookup(const std::u32string& name) const override;
    Symbol* Lookup(const std::u32string& name, ScopeLookup lookup) const override;
    Symbol* LookupQualified(const std::vector<std::u32string>& components, ScopeLookup lookup) const;
    const NamespaceSymbol* Ns() const;
    NamespaceSymbol* Ns();
    void Clear();
    NamespaceSymbol* CreateNamespace(const std::u32string& qualifiedNsName, const Span& span, const boost::uuids::uuid& sourceModuleId);
    void CollectViableFunctions(int arity, const std::u32string& groupName, std::unordered_set<ContainerScope*>& scopesLookedUp, ScopeLookup scopeLookup, 
        ViableFunctionSet& viableFunctions, Module* module);
    const std::unordered_map<std::u32string, Symbol*>& SymbolMap() const { return symbolMap; }
private:
    ContainerSymbol* container;
    ContainerScope* parentScope;
    std::unordered_map<std::u32string, Symbol*> symbolMap;
};

class SYMBOLS_API FileScope : public Scope
{
public:
    FileScope();
    void InstallAlias(ContainerScope* containerScope, AliasNode* aliasNode);
    void AddContainerScope(ContainerScope* containerScope);
    void InstallNamespaceImport(ContainerScope* containerScope, NamespaceImportNode* namespaceImportNode);
    Symbol* Lookup(const std::u32string& name) const override;
    Symbol* Lookup(const std::u32string& name, ScopeLookup lookup) const override;
    void CollectViableFunctions(int arity, const std::u32string&  groupName, std::unordered_set<ContainerScope*>& scopesLookedUp, ViableFunctionSet& viableFunctions,
        Module* module);
private:
    std::vector<ContainerScope*> containerScopes;
    std::unordered_map<std::u32string, Symbol*> aliasSymbolMap;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_SCOPE_INCLUDED
