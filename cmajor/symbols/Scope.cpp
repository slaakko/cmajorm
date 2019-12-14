// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Scope.hpp>
#include <cmajor/symbols/ContainerSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/NamespaceSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Util.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;
using namespace soulng::util;

Scope::~Scope()
{
}

ContainerScope::ContainerScope() : container(nullptr), parentScope(nullptr), symbolMap()
{
}

ContainerScope* ContainerScope::BaseScope() const
{
    if (container)
    {
        if (container->GetSymbolType() == SymbolType::classTypeSymbol || container->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(container);
            ClassTypeSymbol* baseClass = cls->BaseClass();
            if (baseClass)
            {
                return baseClass->GetContainerScope();
            }
        }
    }
    return nullptr;
}

ContainerScope* ContainerScope::ParentScope() const
{
    if (parentScope)
    {
        return parentScope;
    }
    if (container)
    {
        Symbol* parent = nullptr;
        if (container->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(container);
            ClassTypeSymbol* classTemplate = specialization->GetClassTemplate();
            parent = classTemplate->Parent();
        }
        else
        {
            parent = container->Parent();
        }
        if (parent)
        {
            if (!parent->GetModule()->IsRootModule() && parent->GetSymbolType() == SymbolType::namespaceSymbol)
            {
                NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(parent);
                Module* rootModule = GetRootModuleForCurrentThread();
                NamespaceSymbol* mappedNs = rootModule->GetSymbolTable().GetMappedNs(ns);
                if (mappedNs)
                {
                    return mappedNs->GetContainerScope();
                }
            }
            return parent->GetContainerScope();
        }
    }
    return nullptr;
}

void ContainerScope::Install(Symbol* symbol)
{
    auto it = symbolMap.find(symbol->Name());
    if (symbol->GetSymbolType() != SymbolType::namespaceSymbol && 
        symbol->GetSymbolType() != SymbolType::declarationBlock && 
        it != symbolMap.cend())
    {
        Symbol* prev = it->second;
        if (prev != symbol)
        {
            const Span& defined = symbol->GetSpan();
            const Span& referenced = prev->GetSpan();
            throw Exception(GetRootModuleForCurrentThread(), "symbol '" + ToUtf8(symbol->Name()) + "' already defined", defined, referenced);
        }
    }
    else
    {
        symbolMap[symbol->Name()] = symbol;
    }
}

Symbol* ContainerScope::Lookup(const std::u32string& name) const
{
    return Lookup(name, ScopeLookup::this_);
}

int CountQualifiedNameComponents(const std::u32string& qualifiedName)
{
    int numComponents = 0;
    int componentSize = 0;
    int state = 0;
    int angleBracketCount = 0;
    for (char32_t c : qualifiedName)
    {
        switch (state)
        {
            case 0:
            {
                if (c == '.')
                {
                    ++numComponents;
                    componentSize = 0;
                }
                else if (c == '<')
                {
                    ++componentSize;
                    angleBracketCount = 1;
                    state = 1;
                }
                else
                {
                    ++componentSize;
                }
                break;
            }
            case 1:
            {
                ++componentSize;
                if (c == '<')
                {
                    ++angleBracketCount;
                }
                else if (c == '>')
                {
                    --angleBracketCount;
                    if (angleBracketCount == 0)
                    {
                        state = 0;
                    }
                }
                break;
            }
        }
    }
    if (componentSize > 0)
    {
        ++numComponents;
    }
    return numComponents;
}

std::vector<std::u32string> ParseQualifiedName(const std::u32string& qualifiedName)
{
    std::vector<std::u32string> components;
    int state = 0;
    std::u32string component;
    int angleBracketCount = 0;
    for (char32_t c : qualifiedName)
    {
        switch (state)
        {
            case 0:
            {
                if (c == '.')
                {
                    components.push_back(component);
                    component.clear();
                }
                else if (c == '<')
                {
                    component.append(1, c);
                    angleBracketCount = 1;
                    state = 1;
                }
                else
                {
                    component.append(1, c);
                }
                break;
            }
            case 1:
            {
                component.append(1, c);
                if (c == '<')
                {
                    ++angleBracketCount;
                }
                else if (c == '>')
                {
                    --angleBracketCount;
                    if (angleBracketCount == 0)
                    {
                        state = 0;
                    }
                }
                break;
            }
        }
    }
    if (!component.empty())
    {
        components.push_back(component);
    }
    return components;
}

Symbol* ContainerScope::Lookup(const std::u32string& name, ScopeLookup lookup) const
{
    int numQualifiedNameComponents = CountQualifiedNameComponents(name);
    if (numQualifiedNameComponents > 1)
    {
        std::vector<std::u32string> components = ParseQualifiedName(name);
        return LookupQualified(components, lookup);
    }
    else
    {
        auto it = symbolMap.find(name);
        if (it != symbolMap.end())
        {
            Symbol* s = it->second;
            return s;
        }
        if ((lookup & ScopeLookup::base) != ScopeLookup::none)
        {
            ContainerScope* baseScope = BaseScope();
            if (baseScope)
            {
                Symbol* s = baseScope->Lookup(name, lookup);
                if (s)
                {
                    return s;
                }
            }
        }
        if ((lookup & ScopeLookup::parent) != ScopeLookup::none)
        {
            ContainerScope* parentScope = ParentScope();
            if (parentScope)
            {
                Symbol* s = parentScope->Lookup(name, lookup);
                if (s)
                {
                    return s;
                }
            }
        }
        return nullptr;
    }
}

Symbol* ContainerScope::LookupQualified(const std::vector<std::u32string>& components, ScopeLookup lookup) const
{
    const ContainerScope* scope = this;
    Symbol* s = nullptr;
    int n = int(components.size());
    bool allComponentsMatched = true;
    for (int i = 0; i < n; ++i)
    {
        const std::u32string& component = components[i];
        if (scope)
        {
            s = scope->Lookup(component, ScopeLookup::this_);
            if (s)
            {
                scope = s->GetContainerScope();
            }
            else
            {
                allComponentsMatched = false;
            }
        }
    }
    if (!s || !allComponentsMatched)
    {
        if ((lookup & ScopeLookup::parent) != ScopeLookup::none)
        {
            ContainerScope* parentScope = ParentScope();
            if (parentScope)
            {
                return parentScope->LookupQualified(components, lookup);
            }
            else
            {
                return nullptr;
            }
        }
    }
    return s;
}

const NamespaceSymbol* ContainerScope::Ns() const
{
    return container->Ns();
}

NamespaceSymbol* ContainerScope::Ns()
{
    return container->Ns();
}

void ContainerScope::Clear()
{
    symbolMap.clear();
}

NamespaceSymbol* ContainerScope::CreateNamespace(const std::u32string& qualifiedNsName, const Span& span)
{
    ContainerScope* scope = this;
    NamespaceSymbol* parentNs = scope->Ns();
    std::vector<std::u32string> components = Split(qualifiedNsName, '.');
    for (const std::u32string& component : components)
    {
        Symbol* s = scope->Lookup(component);
        if (s)
        {
            if (s->GetSymbolType() == SymbolType::namespaceSymbol)
            {
                scope = s->GetContainerScope();
                parentNs = scope->Ns();
            }
            else
            {
                throw Exception(GetRootModuleForCurrentThread(), "symbol '" + ToUtf8(s->Name()) + "' does not denote a namespace", s->GetSpan());
            }
        }
        else
        {
            NamespaceSymbol* newNs = new NamespaceSymbol(span, component);
            newNs->SetModule(container->GetModule());
            scope = newNs->GetContainerScope();
            parentNs->AddMember(newNs);
            parentNs = newNs;
        }
    }
    return parentNs;
}

void ContainerScope::CollectViableFunctions(int arity, const std::u32string& groupName, std::unordered_set<ContainerScope*>& scopesLookedUp, ScopeLookup scopeLookup, 
    ViableFunctionSet& viableFunctions, Module* module)
{
    if ((scopeLookup & ScopeLookup::this_) != ScopeLookup::none)
    {
        if (scopesLookedUp.find(this) == scopesLookedUp.cend())
        {
            scopesLookedUp.insert(this);
            Symbol* symbol = Lookup(groupName);
            if (symbol)
            {
                if (symbol->GetSymbolType() == SymbolType::functionGroupSymbol)
                {
                    FunctionGroupSymbol* functionGroupSymbol = static_cast<FunctionGroupSymbol*>(symbol);
                    functionGroupSymbol->CollectViableFunctions(arity, viableFunctions, module);
                }
            }
        }
    }
    if ((scopeLookup & ScopeLookup::base) != ScopeLookup::none)
    {
        ContainerScope* baseScope = BaseScope();
        if (baseScope)
        {
            baseScope->CollectViableFunctions(arity, groupName, scopesLookedUp, scopeLookup, viableFunctions, module);
        }
    }
    if ((scopeLookup & ScopeLookup::parent) != ScopeLookup::none)
    {
        ContainerScope* parentScope = ParentScope();
        if (parentScope)
        {
            parentScope->CollectViableFunctions(arity, groupName, scopesLookedUp, scopeLookup, viableFunctions, module);
        }
    }
}

FileScope::FileScope(Module* module_) : module(module_)
{
}

void FileScope::InstallAlias(ContainerScope* containerScope, AliasNode* aliasNode)
{
    Assert(containerScope, "container scope is null");
    std::u32string qualifiedName = aliasNode->Qid()->Str();
    Symbol* symbol = containerScope->Lookup(qualifiedName, ScopeLookup::this_and_parent);
    if (symbol)
    {
        std::u32string aliasName = aliasNode->Id()->Str();
        aliasSymbolMap[aliasName] = symbol;
    }
    else
    {
        throw Exception(module, "referred symbol '" + ToUtf8(aliasNode->Qid()->Str()) + "' not found", aliasNode->Qid()->GetSpan());
    }
}

void FileScope::AddContainerScope(ContainerScope* containerScope)
{
    if (std::find(containerScopes.cbegin(), containerScopes.cend(), containerScope) == containerScopes.cend())
    {
        containerScopes.push_back(containerScope);
    }
}

void FileScope::InstallNamespaceImport(ContainerScope* containerScope, NamespaceImportNode* namespaceImportNode)
{
    try
    {
        Assert(containerScope, "container scope is null");
        std::u32string importedNamespaceName = namespaceImportNode->Ns()->Str();
        Symbol* symbol = containerScope->Lookup(importedNamespaceName, ScopeLookup::this_and_parent);
        if (symbol)
        {
            if (symbol->GetSymbolType() == SymbolType::namespaceSymbol)
            {
                ContainerScope* symbolContainerScope = symbol->GetContainerScope();
                if (std::find(containerScopes.cbegin(), containerScopes.cend(), symbolContainerScope) == containerScopes.cend())
                {
                    containerScopes.push_back(symbolContainerScope);
                }
            }
            else
            {
                throw Exception(module, "'" + ToUtf8(namespaceImportNode->Ns()->Str()) + "' does not denote a namespace", namespaceImportNode->Ns()->GetSpan());
            }
        }
        else
        {
            throw Exception(module, "referred namespace symbol '" + ToUtf8(namespaceImportNode->Ns()->Str()) + "' not found", namespaceImportNode->Ns()->GetSpan());
        }
    }
    catch (const Exception&)
    {
        throw;
    }
}

Symbol* FileScope::Lookup(const std::u32string& name) const
{
    return Lookup(name, ScopeLookup::this_);
}

Symbol* FileScope::Lookup(const std::u32string& name, ScopeLookup lookup) const
{
    if (lookup != ScopeLookup::this_)
    {
        throw std::runtime_error("file scope supports only this scope lookup");
    }
    std::unordered_set<Symbol*> foundSymbols;
    auto it = aliasSymbolMap.find(name);
    if (it != aliasSymbolMap.cend())
    {
        Symbol* symbol = it->second;
        foundSymbols.insert(symbol);
    }
    else
    {
        for (ContainerScope* containerScope : containerScopes)
        {
            Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_);
            if (symbol)
            {
                foundSymbols.insert(symbol);
            }
        }
    }
    if (foundSymbols.empty())
    {
        return nullptr;
    }
    else if (foundSymbols.size() > 1)
    {
        std::string message("reference to object '" + ToUtf8(name) + "' is ambiguous: ");
        bool first = true;
        Span span;
        for (Symbol* symbol : foundSymbols)
        {
            if (first)
            {
                first = false;
                span = symbol->GetSpan();
            }
            else
            {
                message.append(" or ");
            }
            message.append(ToUtf8(symbol->FullName()));
        }
        throw Exception(module, message, span);
    }
    else
    {
        return *foundSymbols.begin();
    }
}

void FileScope::CollectViableFunctions(int arity, const std::u32string&  groupName, std::unordered_set<ContainerScope*>& scopesLookedUp, ViableFunctionSet& viableFunctions, 
    Module* module)
{
    for (ContainerScope* containerScope : containerScopes)
    {
        if (scopesLookedUp.find(containerScope) == scopesLookedUp.cend())
        {
            containerScope->CollectViableFunctions(arity, groupName, scopesLookedUp, ScopeLookup::this_, viableFunctions, module);
            scopesLookedUp.insert(containerScope);
        }
    }
}

} } // namespace cmajor::symbols
