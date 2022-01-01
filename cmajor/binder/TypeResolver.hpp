// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_TYPE_RESOLVER_INCLUDED
#define CMAJOR_BINDER_TYPE_RESOLVER_INCLUDED
#include <sngcm/ast/TypeExpr.hpp>
#include <cmajor/symbols/TypeSymbol.hpp>


namespace cmajor {  namespace binder {

using namespace sngcm::ast;
using namespace cmajor::symbols;

class BoundCompileUnit;

class NamespaceTypeSymbol : public TypeSymbol
{
public:
    NamespaceTypeSymbol(NamespaceSymbol* ns_);
    bool IsInComplete() const override { return true; }
    const NamespaceSymbol* Ns() const { return ns; }
    NamespaceSymbol* Ns() { return ns; }
    void* IrType(Emitter& emitter) override { Assert(false, "tried to get ir type of namespace type"); return nullptr; }
    void* CreateDefaultIrValue(Emitter& emitter) override { Assert(false, "tried to create default ir value of namespace type"); return nullptr; }
    const char* ClassName() const override { return "NamespaceTypeSymbol"; }
private:
    NamespaceSymbol* ns;
};

enum class TypeResolverFlags : uint8_t
{
    none = 0,
    resolveClassGroup = 1 << 0,
    createMemberSymbols = 1 << 1
};

inline TypeResolverFlags operator&(TypeResolverFlags left, TypeResolverFlags right)
{
    return TypeResolverFlags(uint8_t(left) & uint8_t(right));
}

inline TypeResolverFlags operator|(TypeResolverFlags left, TypeResolverFlags right)
{
    return TypeResolverFlags(uint8_t(left) | uint8_t(right));
}

TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, ClassTypeSymbol* currentClass);
TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, TypeResolverFlags flags);
TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, TypeResolverFlags flags, ClassTypeSymbol* currentClass);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_TYPE_RESOLVER_INCLUDED
