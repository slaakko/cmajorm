// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/Evaluator.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/DerivedTypeSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Log.hpp>
#include <cmajor/util/Time.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::unicode;

NamespaceTypeSymbol::NamespaceTypeSymbol(NamespaceSymbol* ns_) : TypeSymbol(SymbolType::namespaceTypeSymbol, ns_->GetSpan(), ns_->Name()), ns(ns_)
{
}

class TypeResolver : public Visitor
{
public:
    TypeResolver(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_, TypeResolverFlags flags_, ClassTypeSymbol* currentClass_);
    TypeSymbol* GetType() { return type; }
    const TypeDerivationRec& DerivationRec() const { return derivationRec; }
    void Visit(BoolNode& boolNode) override;
    void Visit(SByteNode& sbyteNode) override;
    void Visit(ByteNode& byteNode) override;
    void Visit(ShortNode& shortNode) override;
    void Visit(UShortNode& ushortNode) override;
    void Visit(IntNode& intNode) override;
    void Visit(UIntNode& uintNode) override;
    void Visit(LongNode& longNode) override;
    void Visit(ULongNode& ulongNode) override;
    void Visit(FloatNode& floatNode) override;
    void Visit(DoubleNode& doubleNode) override;
    void Visit(CharNode& charNode) override;
    void Visit(WCharNode& wcharNode) override;
    void Visit(UCharNode& ucharNode) override;
    void Visit(VoidNode& voidNode) override;
    void Visit(ConstNode& constNode) override;
    void Visit(LValueRefNode& lvalueRefNode) override;
    void Visit(RValueRefNode& rvalueRefNode) override;
    void Visit(PointerNode& pointerNode) override;
    void Visit(ArrayNode& arrayNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(TemplateIdNode& templateIdNode) override;
    void Visit(DotNode& dotNode) override;
private:
    BoundCompileUnit& boundCompileUnit;
    SymbolTable& symbolTable;
    Module* module;
    ContainerScope* containerScope;
    ClassTemplateRepository& classTemplateRepository;
    TypeSymbol* type;
    TypeDerivationRec derivationRec;
    std::unique_ptr<NamespaceTypeSymbol> nsTypeSymbol;
    TypeResolverFlags flags;
    ClassTypeSymbol* currentClass;
    void ResolveSymbol(Node& node, IdentifierNode* idNode, Symbol* symbol);
};

TypeResolver::TypeResolver(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_, TypeResolverFlags flags_, ClassTypeSymbol* currentClass_) :
    boundCompileUnit(boundCompileUnit_), symbolTable(boundCompileUnit.GetSymbolTable()), module(&boundCompileUnit.GetModule()),
    classTemplateRepository(boundCompileUnit.GetClassTemplateRepository()), containerScope(containerScope_),
    type(nullptr), derivationRec(), nsTypeSymbol(), flags(flags_), currentClass(currentClass_)
{
}

void TypeResolver::Visit(BoolNode& boolNode)
{
    type = symbolTable.GetTypeByName(U"bool");
}

void TypeResolver::Visit(SByteNode& sbyteNode)
{
    type = symbolTable.GetTypeByName(U"sbyte");
}

void TypeResolver::Visit(ByteNode& byteNode)
{
    type = symbolTable.GetTypeByName(U"byte");
}

void TypeResolver::Visit(ShortNode& shortNode)
{
    type = symbolTable.GetTypeByName(U"short");
}

void TypeResolver::Visit(UShortNode& ushortNode)
{
    type = symbolTable.GetTypeByName(U"ushort");
}

void TypeResolver::Visit(IntNode& intNode)
{
    type = symbolTable.GetTypeByName(U"int");
}

void TypeResolver::Visit(UIntNode& uintNode)
{
    type = symbolTable.GetTypeByName(U"uint");
}

void TypeResolver::Visit(LongNode& longNode)
{
    type = symbolTable.GetTypeByName(U"long");
}

void TypeResolver::Visit(ULongNode& ulongNode)
{
    type = symbolTable.GetTypeByName(U"ulong");
}

void TypeResolver::Visit(FloatNode& floatNode)
{
    type = symbolTable.GetTypeByName(U"float");
}

void TypeResolver::Visit(DoubleNode& doubleNode)
{
    type = symbolTable.GetTypeByName(U"double");
}

void TypeResolver::Visit(CharNode& charNode)
{
    type = symbolTable.GetTypeByName(U"char");
}

void TypeResolver::Visit(WCharNode& wcharNode)
{
    type = symbolTable.GetTypeByName(U"wchar");
}

void TypeResolver::Visit(UCharNode& ucharNode)
{
    type = symbolTable.GetTypeByName(U"uchar");
}

void TypeResolver::Visit(VoidNode& voidNode)
{
    type = symbolTable.GetTypeByName(U"void");
}

void TypeResolver::Visit(ConstNode& constNode)
{
    derivationRec.derivations.push_back(Derivation::constDerivation);
    constNode.Subject()->Accept(*this);
}

void TypeResolver::Visit(LValueRefNode& lvalueRefNode)
{
    lvalueRefNode.Subject()->Accept(*this);
    if (HasReferenceDerivation(derivationRec.derivations))
    {
        throw Exception(module, "cannot have reference to reference type", lvalueRefNode.GetSpan());
    }
    derivationRec.derivations.push_back(Derivation::lvalueRefDerivation);
}

void TypeResolver::Visit(RValueRefNode& rvalueRefNode)
{
    rvalueRefNode.Subject()->Accept(*this);
    if (HasReferenceDerivation(derivationRec.derivations))
    {
        throw Exception(module, "cannot have reference to reference type", rvalueRefNode.GetSpan());
    }
    derivationRec.derivations.push_back(Derivation::rvalueRefDerivation);
}

void TypeResolver::Visit(PointerNode& pointerNode)
{
    pointerNode.Subject()->Accept(*this);
    if (HasReferenceDerivation(derivationRec.derivations))
    {
        throw Exception(module, "cannot have pointer to reference type", pointerNode.GetSpan());
    }
    derivationRec.derivations.push_back(Derivation::pointerDerivation);
}

void TypeResolver::Visit(ArrayNode& arrayNode)
{
    type = ResolveType(arrayNode.Subject(), boundCompileUnit, containerScope, currentClass);
    if (type->IsReferenceType())
    {
        throw Exception(module, "cannot have array of reference type", arrayNode.GetSpan());
    }
    int64_t size = -1;
    if (arrayNode.Size())
    {
        std::unique_ptr<Value> sizeValue = Evaluate(arrayNode.Size(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"long"), containerScope, boundCompileUnit, false, nullptr, arrayNode.GetSpan());
        if (sizeValue->GetValueType() == ValueType::longValue)
        {
            LongValue* longSizeValue = static_cast<LongValue*>(sizeValue.get());
            size = longSizeValue->GetValue();
        }
        else
        {
            throw Exception(module, "long type value expected ", arrayNode.Size()->GetSpan());
        }
    }
    type = symbolTable.MakeArrayType(type, size, arrayNode.GetSpan());
}

void TypeResolver::ResolveSymbol(Node& node, IdentifierNode* idNode, Symbol* symbol)
{
    if (symbol->IsTypeSymbol())
    {
        type = static_cast<TypeSymbol*>(symbol);
        if (GetGlobalFlag(GlobalFlags::cmdoc))
        {
            TypeSymbol* cmdocType = type;
            if (type->GetSymbolType() == SymbolType::classGroupTypeSymbol)
            {
                ClassGroupTypeSymbol* group = static_cast<ClassGroupTypeSymbol*>(type);
                cmdocType = group->GetClass(0);
            }
            symbolTable.MapSymbol(idNode, cmdocType);
        }
    }
    else
    {
        switch (symbol->GetSymbolType())
        {
            case SymbolType::typedefSymbol:
            {
                TypedefSymbol* typedefSymbol = static_cast<TypedefSymbol*>(symbol);
                if (typedefSymbol->IsProject() && !typedefSymbol->IsBound())
                {
                    TypeBinder typeBinder(boundCompileUnit);
                    typeBinder.SetContainerScope(typedefSymbol->Parent()->GetContainerScope());
                    Node* node = symbolTable.GetNode(typedefSymbol);
                    Assert(node->GetNodeType() == NodeType::typedefNode, "typedef node expected");
                    TypedefNode* typedefNode = static_cast<TypedefNode*>(node);
                    typeBinder.BindTypedef(typedefSymbol, typedefNode, false);
                }
                type = typedefSymbol->GetType();
                if (GetGlobalFlag(GlobalFlags::cmdoc))
                {
                    symbolTable.MapSymbol(idNode, typedefSymbol);
                }
                break;
            }
            case SymbolType::boundTemplateParameterSymbol:
            {
                BoundTemplateParameterSymbol* boundTemplateParameterSymbol = static_cast<BoundTemplateParameterSymbol*>(symbol);
                type = boundTemplateParameterSymbol->GetType();
                break;
            }
            case SymbolType::namespaceSymbol:
            {
                NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(symbol);
                nsTypeSymbol.reset(new NamespaceTypeSymbol(ns));
                symbolTable.SetTypeIdFor(nsTypeSymbol.get());
                type = nsTypeSymbol.get();
                break;
            }
            default:
            {
                throw Exception(module, "symbol '" + ToUtf8(symbol->FullName()) + "' does not denote a type", node.GetSpan(), symbol->GetSpan());
            }
        }
    }
}

void TypeResolver::Visit(IdentifierNode& identifierNode)
{
    symbolTable.SetLatestIdentifier(&identifierNode);
    std::u32string name = identifierNode.Str();
    Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        for (const std::unique_ptr<FileScope>& fileScope : boundCompileUnit.FileScopes())
        {
            symbol = fileScope->Lookup(name);
            if (symbol)
            {
                break;
            }
        }
    }
    if (symbol)
    {
        ResolveSymbol(identifierNode, &identifierNode, symbol);
    }
    else
    {
        throw Exception(module, "type symbol '" + ToUtf8(name) + "' not found", identifierNode.GetSpan());
    }
}

void TypeResolver::Visit(TemplateIdNode& templateIdNode)
{
    IdentifierNode* prevId = symbolTable.GetLatestIdentifier();
    int arity = templateIdNode.TemplateArguments().Count();
    TypeSymbol* primaryTemplateType = ResolveType(templateIdNode.Primary(), boundCompileUnit, containerScope, TypeResolverFlags::resolveClassGroup, currentClass);
    if (primaryTemplateType->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(primaryTemplateType);
        ClassTypeSymbol* classType = classGroup->GetClass(arity);
        if (classType)
        {
            primaryTemplateType = classType;
        }
        else
        {
            throw Exception(classGroup->GetModule(), "primary class template with arity '" + std::to_string(arity) + "' not found", classGroup->GetSpan());
        }
    }
    if (!primaryTemplateType->IsClassTypeSymbol())
    {
        throw Exception(module, "class type symbol expected", templateIdNode.Primary()->GetSpan());
    }
    ClassTypeSymbol* classTemplate = static_cast<ClassTypeSymbol*>(primaryTemplateType);
    if (!classTemplate->IsClassTemplate())
    {
        throw Exception(module, "class template expected", templateIdNode.Primary()->GetSpan());
    }
    IdentifierNode* idNode = symbolTable.GetLatestIdentifier();
    symbolTable.SetLatestIdentifier(prevId);
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(idNode, classTemplate);
    }
    std::vector<TypeSymbol*> templateArgumentTypes;
    int n = arity;
    for (int i = 0; i < n; ++i)
    {
        IdentifierNode* prevId = symbolTable.GetLatestIdentifier();
        TypeSymbol* templateArgumentType = ResolveType(templateIdNode.TemplateArguments()[i], boundCompileUnit, containerScope, currentClass);
        templateArgumentTypes.push_back(templateArgumentType);
        IdentifierNode* idNode = symbolTable.GetLatestIdentifier();
        if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
        {
            symbolTable.MapSymbol(idNode, templateArgumentType);
        }
        symbolTable.SetLatestIdentifier(prevId);
    }
    int m = classTemplate->TemplateParameters().size();
    if (n < m)
    {
        classTemplateRepository.ResolveDefaultTemplateArguments(templateArgumentTypes, classTemplate, containerScope, templateIdNode.GetSpan());
    }
    ClassTemplateSpecializationSymbol* classTemplateSpecialization = symbolTable.MakeClassTemplateSpecialization(classTemplate, templateArgumentTypes, templateIdNode.GetSpan());
    if (!classTemplateSpecialization->IsBound())
    {
        classTemplateRepository.BindClassTemplateSpecialization(classTemplateSpecialization, containerScope, templateIdNode.GetSpan());
    }
    type = classTemplateSpecialization;
}

void TypeResolver::Visit(DotNode& dotNode)
{
    dotNode.Subject()->Accept(*this);
    Scope* scope = nullptr;
    if (type->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(type);
        if (currentClass && classGroup->Name() == currentClass->Name())
        {
            type = currentClass;
        }
        else
        {
            type = classGroup->GetClass(0);
        }
        if (!type)
        {
            throw Exception(module, "symbol '" + ToUtf8(type->FullName()) + "' does not denote a class type, an array type or a namespace", dotNode.GetSpan(), type->GetSpan());
        }
    }
    if (type->GetSymbolType() == SymbolType::namespaceTypeSymbol)
    {
        NamespaceTypeSymbol* nsType = static_cast<NamespaceTypeSymbol*>(type);
        scope = nsType->Ns()->GetContainerScope();
    }
    else if (type->IsClassTypeSymbol() ||
        type->IsArrayType() ||
        type->IsEnumeratedType() || 
        (flags & TypeResolverFlags::createMemberSymbols) != TypeResolverFlags::none && type->GetSymbolType() == SymbolType::templateParameterSymbol)
    {
        scope = type->GetContainerScope();
    }
    else
    {
        throw Exception(module, "symbol '" + ToUtf8(type->FullName()) + "' does not denote a class type, an array type or a namespace", dotNode.GetSpan(), type->GetSpan());
    }
    std::u32string name = dotNode.MemberId()->Str();
    Symbol* symbol = scope->Lookup(name, ScopeLookup::this_and_base);
    if (symbol)
    {
        ResolveSymbol(dotNode, dotNode.MemberId(), symbol);
    }
    else
    {
        if ((flags & TypeResolverFlags::createMemberSymbols) != TypeResolverFlags::none && type->GetSymbolType() == SymbolType::templateParameterSymbol)
        {
            TemplateParameterSymbol* templateParameterSymbol = new TemplateParameterSymbol(dotNode.GetSpan(), name);
            templateParameterSymbol->SetModule(module);
            symbolTable.SetTypeIdFor(templateParameterSymbol);
            type->AddMember(templateParameterSymbol);
            ResolveSymbol(dotNode, dotNode.MemberId(), templateParameterSymbol);
        }
        else
        {
            throw Exception(module, "type symbol '" + ToUtf8(name) + "' not found", dotNode.GetSpan());
        }
    }
}

TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    return ResolveType(typeExprNode, boundCompileUnit, containerScope, TypeResolverFlags::none, nullptr);
}

TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, ClassTypeSymbol* currentClass)
{
    return ResolveType(typeExprNode, boundCompileUnit, containerScope, TypeResolverFlags::none, currentClass);
}

TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, TypeResolverFlags flags)
{
    return ResolveType(typeExprNode, boundCompileUnit, containerScope, flags, nullptr);
}

TypeSymbol* ResolveType(Node* typeExprNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, TypeResolverFlags flags, ClassTypeSymbol* currentClass)
{
    Module* module = &boundCompileUnit.GetModule();
    bool resolveClassGroup = (flags & TypeResolverFlags::resolveClassGroup) != TypeResolverFlags::none;
    TypeResolver typeResolver(boundCompileUnit, containerScope, flags, currentClass);
    typeExprNode->Accept(typeResolver);
    TypeSymbol* type = typeResolver.GetType();
    if (resolveClassGroup && type && type->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        return type;
    }
    if (type && type->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(type);
        if (currentClass && classGroup->Name() == currentClass->GroupName())
        {
            type = currentClass;
        }
        else
        {
            type = classGroup->GetClass(0);
        }
    }
    if (!type || type->IsInComplete())
    {
        throw Exception(module, "incomplete type expression", typeExprNode->GetSpan());
    }
    TypeDerivationRec derivationRec = UnifyDerivations(typeResolver.DerivationRec(), type->DerivationRec());
    if (!derivationRec.derivations.empty())
    {
        return boundCompileUnit.GetSymbolTable().MakeDerivedType(type->BaseType(), derivationRec, typeExprNode->GetSpan());
    }
    return type;
}

} } // namespace cmajor::binder
