// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/Concept.hpp>
#include <cmajor/binder/Evaluator.hpp>
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/binder/BoundExpression.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/GlobalVariable.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/DelegateSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/ConstantSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

ConstraintSymbolBinder* constraintSymbolBinder = nullptr;

ConstraintSymbolBinder::~ConstraintSymbolBinder()
{
}

void SetConstraintSymbolBinder(ConstraintSymbolBinder* constraintSymbolBinder_)
{
    constraintSymbolBinder = constraintSymbolBinder_;
}

class UsingNodeAdder : public Visitor
{
public:
    UsingNodeAdder(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerscope);
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(AliasNode& aliasNode) override;
    void Visit(NamespaceImportNode& namespaceImportNode) override;
private:
    BoundCompileUnit& boundCompileUnit;
    SymbolTable& symbolTable;
    ContainerScope* containerScope;
};

UsingNodeAdder::UsingNodeAdder(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_) :
    boundCompileUnit(boundCompileUnit_), symbolTable(boundCompileUnit.GetSymbolTable()), containerScope(containerScope_)
{
}

void UsingNodeAdder::Visit(NamespaceNode& namespaceNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = symbolTable.GetSymbol(&namespaceNode);
    containerScope = symbol->GetContainerScope();
    int n = namespaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = namespaceNode.Members()[i];
        member->Accept(*this);
    }
    containerScope = prevContainerScope;
}

void UsingNodeAdder::Visit(AliasNode& aliasNode)
{
    boundCompileUnit.FirstFileScope()->InstallAlias(containerScope, &aliasNode);
}

void UsingNodeAdder::Visit(NamespaceImportNode& namespaceImportNode)
{
    boundCompileUnit.FirstFileScope()->InstallNamespaceImport(containerScope, &namespaceImportNode);
}

TypeBinder::TypeBinder(BoundCompileUnit& boundCompileUnit_) : 
    boundCompileUnit(boundCompileUnit_), symbolTable(boundCompileUnit.GetSymbolTable()), module(&boundCompileUnit.GetModule()), 
    containerScope(), enumType(nullptr), currentFunctionSymbol(nullptr), currentClassTypeSymbol(nullptr), typeResolverFlags(TypeResolverFlags::none), boundGlobalVariable(nullptr)
{
}

void TypeBinder::AddUsingNodesToCurrentCompileUnit(Node* node)
{
    NamespaceNode* namespaceNode = nullptr;
    Node* parent = node->Parent();
    while (parent)
    {
        if (parent->GetNodeType() == NodeType::namespaceNode)
        {
            namespaceNode = static_cast<NamespaceNode*>(parent);
        }
        parent = parent->Parent();
    }
    if (!namespaceNode)
    {
        throw Exception(module, "global namespace parent not found for node", node->GetSpan());
    }
    UsingNodeAdder usingNodeAdder(boundCompileUnit, containerScope);
    namespaceNode->Accept(usingNodeAdder);
}

void TypeBinder::Visit(CompileUnitNode& compileUnitNode)
{
    if (compileUnitNode.IsProgramMainUnit())
    {
        boundCompileUnit.GenerateGlobalInitializationFunction();
    }
    boundCompileUnit.AddFileScope(new FileScope(&boundCompileUnit.GetModule()));
    compileUnitNode.GlobalNs()->Accept(*this);
}

void TypeBinder::Visit(NamespaceNode& namespaceNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = symbolTable.GetSymbol(&namespaceNode);
    symbol->ComputeMangledName();
    containerScope = symbol->GetContainerScope();
    int n = namespaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = namespaceNode.Members()[i];
        member->Accept(*this);
    }
    containerScope = prevContainerScope;
}

void TypeBinder::Visit(AliasNode& aliasNode)
{
    boundCompileUnit.FirstFileScope()->InstallAlias(containerScope, &aliasNode);
    usingNodes.push_back(&aliasNode);
}

void TypeBinder::Visit(NamespaceImportNode& namespaceImportNode)
{
    boundCompileUnit.FirstFileScope()->InstallNamespaceImport(containerScope, &namespaceImportNode);
    usingNodes.push_back(&namespaceImportNode);
}

void TypeBinder::Visit(FunctionNode& functionNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = symbolTable.GetSymbol(&functionNode);
    Assert(symbol->GetSymbolType() == SymbolType::functionSymbol, "function symbol expected");
    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
    if (functionSymbol->IsBound()) return;
    functionSymbol->SetBound();
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(&functionNode, functionSymbol);
    }
    FunctionSymbol* prevFunctionSymbol = currentFunctionSymbol;
    currentFunctionSymbol = functionSymbol;
    if (GetGlobalFlag(GlobalFlags::cmdoc) && functionNode.WhereConstraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(functionNode.WhereConstraint(), containerScope, boundCompileUnit);
    }
    if (functionSymbol->IsFunctionTemplate())
    {
        functionSymbol->CloneUsingNodes(usingNodes);
        if (functionNode.WhereConstraint())
        {
            CloneContext cloneContext;
            functionSymbol->SetConstraint(static_cast<WhereConstraintNode*>(functionNode.WhereConstraint()->Clone(cloneContext)));
        }
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc) && functionSymbol->Constraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(functionSymbol->Constraint(), functionSymbol->GetContainerScope(), boundCompileUnit);
    }
    containerScope = functionSymbol->GetContainerScope();
    Specifiers specifiers = functionNode.GetSpecifiers();
    functionSymbol->SetSpecifiers(specifiers);
    if (GetGlobalFlag(GlobalFlags::release) && functionSymbol->IsInline())
    {
        functionSymbol->CloneUsingNodes(usingNodes);
    }
    int n = functionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = functionNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags);
        Symbol* symbol = symbolTable.GetSymbol(parameterNode);
        Assert(symbol->GetSymbolType() == SymbolType::parameterSymbol, "parameter symbol expected");
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameterSymbol->SetType(parameterType);
    }
    TypeSymbol* returnType = ResolveType(functionNode.ReturnTypeExpr(), boundCompileUnit, containerScope, typeResolverFlags);
    functionSymbol->SetReturnType(returnType);
    if (!functionSymbol->Constraint() && functionNode.WhereConstraint())
    {
        CloneContext cloneContext;
        functionSymbol->SetConstraint(static_cast<WhereConstraintNode*>(functionNode.WhereConstraint()->Clone(cloneContext)));
    }
    functionSymbol->ComputeName();
    for (ParameterSymbol* parameterSymbol : functionSymbol->Parameters())
    {
        parameterSymbol->ComputeMangledName();
    }
    if (functionSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ParameterSymbol* returnParam = new ParameterSymbol(functionNode.ReturnTypeExpr()->GetSpan(), U"@return");
        returnParam->SetParent(functionSymbol);
        returnParam->SetType(returnType->AddPointer(functionNode.GetSpan()));
        functionSymbol->SetReturnParam(returnParam);
    }
    if (functionNode.Body() && !functionSymbol->IsFunctionTemplate())
    {
        functionNode.Body()->Accept(*this);
    }
    else
    {
        if (!functionSymbol->IsExternal() && !functionSymbol->IsFunctionTemplate())
        {
            throw Exception(module, "function has no body", functionSymbol->GetSpan());
        }
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(functionNode.GetAttributes(), symbol, boundCompileUnit, containerScope);
    containerScope = prevContainerScope;
    currentFunctionSymbol = prevFunctionSymbol;
}

void TypeBinder::Visit(ClassNode& classNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&classNode);
    Assert(symbol->GetSymbolType() == SymbolType::classTypeSymbol || symbol->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type symbol expected");
    ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
    BindClass(classTypeSymbol, &classNode, true);
}

void TypeBinder::BindClassTemplate(ClassTypeSymbol* classTemplate, ClassNode* classNode)
{
    classTemplate->CloneUsingNodes(usingNodes);
    if (classNode->WhereConstraint())
    {
        CloneContext cloneContext;
        classTemplate->SetConstraint(static_cast<ConstraintNode*>(classNode->WhereConstraint()->Clone(cloneContext)));
        if (GetGlobalFlag(GlobalFlags::cmdoc))
        {
            constraintSymbolBinder->BindConstraintSymbols(classNode->WhereConstraint(), classTemplate->GetContainerScope(), boundCompileUnit);
        }
    }
    classTemplate->SetAccess(classNode->GetSpecifiers() & Specifiers::access_);
    classTemplate->ComputeName();
    std::vector<TypeSymbol*> templateArgumentTypes;
    for (TemplateParameterSymbol* templateParam : classTemplate->TemplateParameters())
    {
        templateArgumentTypes.push_back(templateParam);
    }
    ClassTemplateSpecializationSymbol* prototype = symbolTable.MakeClassTemplateSpecialization(classTemplate, templateArgumentTypes, classTemplate->GetSpan());
    prototype->SetPrototype();
    prototype->SetAccess(SymbolAccess::public_);
    boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(prototype, containerScope, classTemplate->GetSpan());
    classTemplate->SetPrototype(prototype);
    if (GetGlobalFlag(GlobalFlags::cmdoc) && prototype->Constraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(prototype->Constraint(), prototype->GetContainerScope(), boundCompileUnit);
    }
}

void TypeBinder::BindClass(ClassTypeSymbol* classTypeSymbol, ClassNode* classNode, bool fromOwnCompileUnit)
{
    if (classTypeSymbol->IsBound()) return;
    classTypeSymbol->SetBound();
    ClassTypeSymbol* prevClassTypeSymbol = currentClassTypeSymbol;
    currentClassTypeSymbol = classTypeSymbol;
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(classNode->Id(), classTypeSymbol);
    }
    if (!fromOwnCompileUnit)
    {
        AddUsingNodesToCurrentCompileUnit(classNode);
    }
    if (classTypeSymbol->IsClassTemplate())
    {
        BindClassTemplate(classTypeSymbol, classNode);
        currentClassTypeSymbol = prevClassTypeSymbol;
        return;
    }
    ContainerScope* prevContainerScope = containerScope;
    containerScope = classTypeSymbol->GetContainerScope();
    classTypeSymbol->SetSpecifiers(classNode->GetSpecifiers());
    if (classNode->WhereConstraint())
    {
        CloneContext cloneContext;
        classTypeSymbol->SetConstraint(static_cast<ConstraintNode*>(classNode->WhereConstraint()->Clone(cloneContext)));
        if (GetGlobalFlag(GlobalFlags::cmdoc))
        {
            constraintSymbolBinder->BindConstraintSymbols(classNode->WhereConstraint(), containerScope, boundCompileUnit);
        }
    }
    classTypeSymbol->ComputeName();
    if (classTypeSymbol->GroupName() == U"NodeList")
    {
        int x = 0;
    }
    int nb = classNode->BaseClassOrInterfaces().Count();
    for (int i = 0; i < nb; ++i)
    {
        Node* baseOrInterfaceNode = classNode->BaseClassOrInterfaces()[i];
        TypeSymbol* baseOrInterfaceSymbol = ResolveType(baseOrInterfaceNode, boundCompileUnit, containerScope, typeResolverFlags);
        if (baseOrInterfaceSymbol->IsClassTypeSymbol())
        {
            ClassTypeSymbol* baseClassSymbol = static_cast<ClassTypeSymbol*>(baseOrInterfaceSymbol);
            if (baseClassSymbol->IsProject() && !GetGlobalFlag(GlobalFlags::info))
            {
                Node* node = symbolTable.GetNode(baseClassSymbol);
                Assert(node->GetNodeType() == NodeType::classNode, "class node expected");
                ClassNode* baseClassNode = static_cast<ClassNode*>(node);
                BindClass(baseClassSymbol, baseClassNode, false);
            }
            if (classTypeSymbol->BaseClass())
            {
                throw Exception(module, "class type can have at most one base class", classTypeSymbol->GetSpan(), baseClassSymbol->GetSpan());
            }
            else if (baseClassSymbol == classTypeSymbol)
            {
                throw Exception(module, "class cannot derive from itself", classTypeSymbol->GetSpan());
            }
            else
            {
                classTypeSymbol->SetBaseClass(baseClassSymbol);
            }
        }
        else if (baseOrInterfaceSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol)
        {
            InterfaceTypeSymbol* interfaceTypeSymbol = static_cast<InterfaceTypeSymbol*>(baseOrInterfaceSymbol);
            if (interfaceTypeSymbol->IsProject())
            {
                Node* node = symbolTable.GetNode(interfaceTypeSymbol);
                Assert(node->GetNodeType() == NodeType::interfaceNode, "interface node expected");
                InterfaceNode* interfaceNode = static_cast<InterfaceNode*>(node);
                BindInterface(interfaceTypeSymbol, interfaceNode, false);
            }
            classTypeSymbol->AddImplementedInterface(interfaceTypeSymbol);
        }
        else
        {
            throw Exception(module, "symbol '" + ToUtf8(baseOrInterfaceSymbol->FullName()) + "' is not a class or interface type symbol", baseOrInterfaceNode->GetSpan(), baseOrInterfaceSymbol->GetSpan());
        }
    }
    int nm = classNode->Members().Count();
    for (int i = 0; i < nm; ++i)
    {
        Node* member = classNode->Members()[i];
        member->Accept(*this);
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(classNode->GetAttributes(), classTypeSymbol, boundCompileUnit, containerScope);
    classTypeSymbol->InitVmt();
    classTypeSymbol->InitImts();
    classTypeSymbol->CreateLayouts();
    if (classTypeSymbol->IsPolymorphic() && !classTypeSymbol->IsPrototypeTemplateSpecialization())
    {
        symbolTable.AddPolymorphicClass(classTypeSymbol);
    }
    if (classTypeSymbol->StaticConstructor())
    {
        symbolTable.AddClassHavingStaticConstructor(classTypeSymbol);
    }
    if (classTypeSymbol->HasNontrivialDestructor())
    {
        classTypeSymbol->CreateDestructorSymbol();
    }
    containerScope = prevContainerScope;
    currentClassTypeSymbol = prevClassTypeSymbol;
}

void TypeBinder::Visit(StaticConstructorNode& staticConstructorNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&staticConstructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::staticConstructorSymbol, "static constructor symbol expected");
    StaticConstructorSymbol* staticConstructorSymbol = static_cast<StaticConstructorSymbol*>(symbol);
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(staticConstructorNode.ClassId(), staticConstructorSymbol);
    }
    FunctionSymbol* prevFunctionSymbol = currentFunctionSymbol;
    currentFunctionSymbol = staticConstructorSymbol;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = staticConstructorSymbol->GetContainerScope();
    staticConstructorSymbol->SetSpecifiers(staticConstructorNode.GetSpecifiers());
    if (staticConstructorSymbol->Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        staticConstructorSymbol->SetTemplateSpecialization();
        staticConstructorSymbol->SetLinkOnceOdrLinkage();
    }
    if (!staticConstructorSymbol->Constraint() && staticConstructorNode.WhereConstraint())
    {
        CloneContext cloneContext;
        staticConstructorSymbol->SetConstraint(static_cast<WhereConstraintNode*>(staticConstructorNode.WhereConstraint()->Clone(cloneContext)));
    }
    staticConstructorSymbol->ComputeName();
    if (GetGlobalFlag(GlobalFlags::cmdoc) && staticConstructorNode.WhereConstraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(staticConstructorNode.WhereConstraint(), containerScope, boundCompileUnit);
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc) && staticConstructorSymbol->Constraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(staticConstructorSymbol->Constraint(), containerScope, boundCompileUnit);
    }
    if (staticConstructorNode.Body())
    {
        staticConstructorNode.Body()->Accept(*this);
    }
    else
    {
        if (!staticConstructorSymbol->IsTemplateSpecialization())
        {
            throw Exception(module, "static constructor has no body", staticConstructorSymbol->GetSpan());
        }
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(staticConstructorNode.GetAttributes(), symbol, boundCompileUnit, containerScope);
    containerScope = prevContainerScope;
    currentFunctionSymbol = prevFunctionSymbol;
}

void TypeBinder::Visit(ConstructorNode& constructorNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&constructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::constructorSymbol, "constructor symbol expected");
    ConstructorSymbol* constructorSymbol = static_cast<ConstructorSymbol*>(symbol);
    if (constructorSymbol->IsBound()) return;
    constructorSymbol->SetBound();
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(constructorNode.ClassId(), constructorSymbol);
    }
    FunctionSymbol* prevFunctionSymbol = currentFunctionSymbol;
    currentFunctionSymbol = constructorSymbol;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = constructorSymbol->GetContainerScope();
    constructorSymbol->SetSpecifiers(constructorNode.GetSpecifiers());
    if (GetGlobalFlag(GlobalFlags::release) && constructorSymbol->IsInline())
    {
        constructorSymbol->CloneUsingNodes(usingNodes);
    }
    Symbol* parent = constructorSymbol->Parent();
    if (parent->IsStatic())
    {
        throw Exception(module, "static class cannot contain instance constructors", constructorSymbol->GetSpan(), parent->GetSpan());
    }
    if (parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        constructorSymbol->SetTemplateSpecialization();
        constructorSymbol->SetLinkOnceOdrLinkage();
    }
    Assert(parent->IsClassTypeSymbol(), "class type symbol expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parent);
    int n = constructorNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = constructorNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
        Symbol* symbol = symbolTable.GetSymbol(parameterNode);
        Assert(symbol->GetSymbolType() == SymbolType::parameterSymbol, "parameter symbol expected");
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameterSymbol->SetType(parameterType);
    }
    if (!constructorSymbol->Constraint() && constructorNode.WhereConstraint())
    {
        CloneContext cloneContext;
        constructorSymbol->SetConstraint(static_cast<WhereConstraintNode*>(constructorNode.WhereConstraint()->Clone(cloneContext)));
    }
    constructorSymbol->ComputeName();
    if (GetGlobalFlag(GlobalFlags::cmdoc) && constructorNode.WhereConstraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(constructorNode.WhereConstraint(), containerScope, boundCompileUnit);
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc) && constructorSymbol->Constraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(constructorSymbol->Constraint(), containerScope, boundCompileUnit);
    }
    for (ParameterSymbol* parameterSymbol : constructorSymbol->Parameters())
    {
        parameterSymbol->ComputeMangledName();
    }
    if (constructorSymbol->IsDefaultConstructor())
    {
        classType->SetDefaultConstructor(constructorSymbol);
    }
    else if (constructorSymbol->IsCopyConstructor())
    {
        classType->SetCopyConstructor(constructorSymbol);
    }
    else if (constructorSymbol->IsMoveConstructor())
    {
        classType->SetMoveConstructor(constructorSymbol);
    }
    else if (constructorSymbol->Arity() == 2 && !constructorSymbol->IsExplicit() && !constructorSymbol->IsGeneratedFunction())
    {
        constructorSymbol->SetConversion();
        symbolTable.AddConversion(constructorSymbol);
    }
    if (constructorNode.Body())
    {
        if ((constructorSymbol->IsDefault() || constructorSymbol->IsSuppressed()) && !constructorSymbol->HasArtificialBody())
        {
            throw Exception(module, "default or suppressed constructor cannot have a body", constructorSymbol->GetSpan());
        }
        constructorNode.Body()->Accept(*this);
    }
    else
    {
        if (!constructorSymbol->IsDefault() && !constructorSymbol->IsSuppressed() && !constructorSymbol->IsTemplateSpecialization())
        {
            throw Exception(module, "constructor has no body", constructorSymbol->GetSpan());
        }
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(constructorNode.GetAttributes(), symbol, boundCompileUnit, containerScope);
    containerScope = prevContainerScope;
    currentFunctionSymbol = prevFunctionSymbol;
}

void TypeBinder::Visit(DestructorNode& destructorNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&destructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::destructorSymbol, "destructor symbol expected");
    DestructorSymbol* destructorSymbol = static_cast<DestructorSymbol*>(symbol);
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(destructorNode.ClassId(), destructorSymbol);
    }
    FunctionSymbol* prevFunctionSymbol = currentFunctionSymbol;
    currentFunctionSymbol = destructorSymbol;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = destructorSymbol->GetContainerScope();
    destructorSymbol->SetSpecifiers(destructorNode.GetSpecifiers());
    const Symbol* parent = destructorSymbol->Parent();
    if (parent->IsStatic())
    {
        throw Exception(module, "static class cannot contain a destructor", destructorSymbol->GetSpan(), parent->GetSpan());
    }
    if (parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        destructorSymbol->SetTemplateSpecialization();
        destructorSymbol->SetLinkOnceOdrLinkage();
    }
    if (!destructorSymbol->Constraint() && destructorNode.WhereConstraint())
    {
        CloneContext cloneContext;
        destructorSymbol->SetConstraint(static_cast<WhereConstraintNode*>(destructorNode.WhereConstraint()->Clone(cloneContext)));
    }
    destructorSymbol->ComputeName();
    if (GetGlobalFlag(GlobalFlags::cmdoc) && destructorNode.WhereConstraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(destructorNode.WhereConstraint(), containerScope, boundCompileUnit);
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc) && destructorSymbol->Constraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(destructorSymbol->Constraint(), containerScope, boundCompileUnit);
    }
    if (destructorNode.Body())
    {
        if (destructorSymbol->IsDefault() && !destructorSymbol->HasArtificialBody())
        {
            throw Exception(module, "default destructor cannot have a body", destructorSymbol->GetSpan());
        }
        destructorNode.Body()->Accept(*this);
    }
    else
    {
        if (!destructorSymbol->IsDefault() && !destructorSymbol->IsTemplateSpecialization())
        {
            throw Exception(module, "destructor has no body", destructorSymbol->GetSpan());
        }
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(destructorNode.GetAttributes(), symbol, boundCompileUnit, containerScope);
    containerScope = prevContainerScope;
    currentFunctionSymbol = prevFunctionSymbol;
}

void TypeBinder::Visit(MemberFunctionNode& memberFunctionNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&memberFunctionNode);
    Assert(symbol->GetSymbolType() == SymbolType::memberFunctionSymbol, "member function symbol expected");
    MemberFunctionSymbol* memberFunctionSymbol = static_cast<MemberFunctionSymbol*>(symbol);
    if (memberFunctionSymbol->IsBound()) return;
    memberFunctionSymbol->SetBound();
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(&memberFunctionNode, memberFunctionSymbol);
    }
    FunctionSymbol* prevFunctionSymbol = currentFunctionSymbol;
    currentFunctionSymbol = memberFunctionSymbol;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = memberFunctionSymbol->GetContainerScope();
    memberFunctionSymbol->SetSpecifiers(memberFunctionNode.GetSpecifiers());
    if (GetGlobalFlag(GlobalFlags::release) && memberFunctionSymbol->IsInline())
    {
        memberFunctionSymbol->CloneUsingNodes(usingNodes);
    }
    Symbol* parent = memberFunctionSymbol->Parent();
    if (parent->IsStatic() && !memberFunctionSymbol->IsStatic())
    {
        throw Exception(module, "static class cannot contain nonstatic member functions", memberFunctionSymbol->GetSpan(), parent->GetSpan());
    }
    if (parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        memberFunctionSymbol->SetTemplateSpecialization();
        memberFunctionSymbol->SetLinkOnceOdrLinkage();
    }
    int n = memberFunctionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = memberFunctionNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
        Symbol* symbol = symbolTable.GetSymbol(parameterNode);
        Assert(symbol->GetSymbolType() == SymbolType::parameterSymbol, "parameter symbol expected");
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameterSymbol->SetType(parameterType);
    }
    TypeSymbol* returnType = ResolveType(memberFunctionNode.ReturnTypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    memberFunctionSymbol->SetReturnType(returnType);
    if (!memberFunctionSymbol->Constraint() && memberFunctionNode.WhereConstraint())
    {
        CloneContext cloneContext;
        memberFunctionSymbol->SetConstraint(static_cast<WhereConstraintNode*>(memberFunctionNode.WhereConstraint()->Clone(cloneContext)));
    }
    memberFunctionSymbol->ComputeName();
    if (GetGlobalFlag(GlobalFlags::cmdoc) && memberFunctionNode.WhereConstraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(memberFunctionNode.WhereConstraint(), containerScope, boundCompileUnit);
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc) && memberFunctionSymbol->Constraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(memberFunctionSymbol->Constraint(), containerScope, boundCompileUnit);
    }
    for (ParameterSymbol* parameterSymbol : memberFunctionSymbol->Parameters())
    {
        parameterSymbol->ComputeMangledName();
    }
    if (memberFunctionSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ParameterSymbol* returnParam = new ParameterSymbol(memberFunctionNode.ReturnTypeExpr()->GetSpan(), U"@return");
        returnParam->SetParent(memberFunctionSymbol);
        returnParam->SetType(returnType->AddPointer(memberFunctionNode.GetSpan()));
        memberFunctionSymbol->SetReturnParam(returnParam);
    }
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parent);
    if (memberFunctionSymbol->IsCopyAssignment())
    {
        classType->SetCopyAssignment(memberFunctionSymbol);
    }
    else if (memberFunctionSymbol->IsMoveAssignment())
    {
        classType->SetMoveAssignment(memberFunctionSymbol);
    }
    if (memberFunctionNode.Body())
    {
        if ((memberFunctionSymbol->IsDefault() || memberFunctionSymbol->IsSuppressed()) && !memberFunctionSymbol->HasArtificialBody())
        {
            throw Exception(module, "default or suppressed member function cannot have a body", memberFunctionSymbol->GetSpan());
        }
        memberFunctionNode.Body()->Accept(*this);
    }
    else
    {
        if (memberFunctionSymbol->Parent()->GetSymbolType() != SymbolType::interfaceTypeSymbol && 
            !memberFunctionSymbol->IsAbstract() && 
            !memberFunctionSymbol->IsDefault() && 
            !memberFunctionSymbol->IsSuppressed() && 
            !memberFunctionSymbol->IsTemplateSpecialization())
        {
            throw Exception(module, "member function has no body", memberFunctionSymbol->GetSpan());
        }
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(memberFunctionNode.GetAttributes(), symbol, boundCompileUnit, containerScope);
    containerScope = prevContainerScope;
    currentFunctionSymbol = prevFunctionSymbol;
}

void TypeBinder::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&conversionFunctionNode);
    Assert(symbol->GetSymbolType() == SymbolType::conversionFunctionSymbol, "conversion function symbol expected");
    ConversionFunctionSymbol* conversionFunctionSymbol = static_cast<ConversionFunctionSymbol*>(symbol);
    if (conversionFunctionSymbol->IsBound()) return;
    conversionFunctionSymbol->SetBound();
    FunctionSymbol* prevFunctionSymbol = currentFunctionSymbol;
    currentFunctionSymbol = conversionFunctionSymbol;
    ContainerScope* prevContainerScope = containerScope;
    containerScope = conversionFunctionSymbol->GetContainerScope();
    conversionFunctionSymbol->SetSpecifiers(conversionFunctionNode.GetSpecifiers());
    if (GetGlobalFlag(GlobalFlags::release) && conversionFunctionSymbol->IsInline())
    {
        conversionFunctionSymbol->CloneUsingNodes(usingNodes);
    }
    const Symbol* parent = conversionFunctionSymbol->Parent();
    if (parent->IsStatic())
    {
        throw Exception(module, "static class cannot contain conversion functions", conversionFunctionSymbol->GetSpan(), parent->GetSpan());
    }
    bool requireBody = true;
    if (parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        conversionFunctionSymbol->SetTemplateSpecialization();
        conversionFunctionSymbol->SetLinkOnceOdrLinkage();
        requireBody = false;
    }
    TypeSymbol* returnType = ResolveType(conversionFunctionNode.ReturnTypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    conversionFunctionSymbol->SetReturnType(returnType);
    if (!conversionFunctionSymbol->Constraint() && conversionFunctionNode.WhereConstraint())
    {
        CloneContext cloneContext;
        conversionFunctionSymbol->SetConstraint(static_cast<WhereConstraintNode*>(conversionFunctionNode.WhereConstraint()->Clone(cloneContext)));
    }
    conversionFunctionSymbol->ComputeName();
    if (GetGlobalFlag(GlobalFlags::cmdoc) && conversionFunctionNode.WhereConstraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(conversionFunctionNode.WhereConstraint(), containerScope, boundCompileUnit);
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc) && conversionFunctionSymbol->Constraint())
    {
        constraintSymbolBinder->BindConstraintSymbols(conversionFunctionSymbol->Constraint(), containerScope, boundCompileUnit);
    }
    if (conversionFunctionSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ParameterSymbol* returnParam = new ParameterSymbol(conversionFunctionNode.ReturnTypeExpr()->GetSpan(), U"@return");
        returnParam->SetParent(conversionFunctionSymbol);
        returnParam->SetType(returnType->AddPointer(conversionFunctionNode.GetSpan()));
        conversionFunctionSymbol->SetReturnParam(returnParam);
    }
    symbolTable.AddConversion(conversionFunctionSymbol);
    if (conversionFunctionNode.Body())
    {
        conversionFunctionNode.Body()->Accept(*this);
    }
    else
    {
        if (requireBody)
        {
            throw Exception(module, "conversion function has no body", conversionFunctionSymbol->GetSpan());
        }
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(conversionFunctionNode.GetAttributes(), symbol, boundCompileUnit, containerScope);
    containerScope = prevContainerScope;
    currentFunctionSymbol = prevFunctionSymbol;
}

void TypeBinder::Visit(MemberVariableNode& memberVariableNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&memberVariableNode);
    Assert(symbol->GetSymbolType() == SymbolType::memberVariableSymbol, "member variable symbol expected");
    MemberVariableSymbol* memberVariableSymbol = static_cast<MemberVariableSymbol*>(symbol);
    memberVariableSymbol->SetSpecifiers(memberVariableNode.GetSpecifiers());
    memberVariableSymbol->ComputeMangledName();
    const Symbol* parent = memberVariableSymbol->Parent();
    if (parent->IsStatic() && !memberVariableSymbol->IsStatic())
    {
        throw Exception(module, "static class cannot contain instance variables", memberVariableSymbol->GetSpan(), parent->GetSpan());
    }
    TypeSymbol* memberVariableType = ResolveType(memberVariableNode.TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    memberVariableSymbol->SetType(memberVariableType);
    if (memberVariableType->IsClassTypeSymbol() && memberVariableType->IsProject() && !memberVariableType->IsBound() && !GetGlobalFlag(GlobalFlags::info))
    {
        ClassTypeSymbol* memberVariableClassType = static_cast<ClassTypeSymbol*>(memberVariableType);
        if (memberVariableClassType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(static_cast<ClassTemplateSpecializationSymbol*>(memberVariableClassType),
                containerScope, memberVariableNode.GetSpan());
        }
        Node* node = symbolTable.GetNode(memberVariableClassType);
        Assert(node->GetNodeType() == NodeType::classNode, "class node expected");
        ClassNode* classNode = static_cast<ClassNode*>(node);
        BindClass(memberVariableClassType, classNode, false);
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(memberVariableNode.GetAttributes(), symbol, boundCompileUnit, containerScope);
}

void TypeBinder::Visit(InterfaceNode& interfaceNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&interfaceNode);
    Assert(symbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type symbol expected");
    InterfaceTypeSymbol* interfaceTypeSymbol = static_cast<InterfaceTypeSymbol*>(symbol);
    BindInterface(interfaceTypeSymbol, &interfaceNode, true);
}

void TypeBinder::BindInterface(InterfaceTypeSymbol* interfaceTypeSymbol, InterfaceNode* interfaceNode, bool fromOwnCompileUnit)
{
    if (interfaceTypeSymbol->IsBound()) return;
    interfaceTypeSymbol->SetBound();
    interfaceTypeSymbol->SetSpecifiers(interfaceNode->GetSpecifiers());
    if (!fromOwnCompileUnit)
    {
        AddUsingNodesToCurrentCompileUnit(interfaceNode);
    }
    ContainerScope* prevContainerScope = containerScope;
    containerScope = interfaceTypeSymbol->GetContainerScope();
    int nm = interfaceNode->Members().Count();
    for (int i = 0; i < nm; ++i)
    {
        Node* member = interfaceNode->Members()[i];
        member->Accept(*this);
    }
    boundCompileUnit.GetAttributeBinder()->BindAttributes(interfaceNode->GetAttributes(), interfaceTypeSymbol, boundCompileUnit, containerScope);
    containerScope = prevContainerScope;
}

void TypeBinder::Visit(DelegateNode& delegateNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&delegateNode);
    Assert(symbol->GetSymbolType() == SymbolType::delegateTypeSymbol, "delegate type symbol expected");
    DelegateTypeSymbol* delegateTypeSymbol = static_cast<DelegateTypeSymbol*>(symbol);
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(delegateNode.Id(), delegateTypeSymbol);
    }
    delegateTypeSymbol->SetSpecifiers(delegateNode.GetSpecifiers());
    delegateTypeSymbol->ComputeMangledName();
    int n = delegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = delegateNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
        Symbol* symbol = symbolTable.GetSymbol(parameterNode);
        Assert(symbol->GetSymbolType() == SymbolType::parameterSymbol, "parameter symbol expected");
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameterSymbol->SetType(parameterType);
    }
    TypeSymbol* returnType = ResolveType(delegateNode.ReturnTypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    delegateTypeSymbol->SetReturnType(returnType);
    if (delegateTypeSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ParameterSymbol* returnParam = new ParameterSymbol(delegateNode.ReturnTypeExpr()->GetSpan(), U"@return");
        returnParam->SetParent(delegateTypeSymbol);
        returnParam->SetType(returnType->AddPointer(delegateNode.GetSpan()));
        delegateTypeSymbol->SetReturnParam(returnParam);
    }
    for (ParameterSymbol* parameterSymbol : delegateTypeSymbol->Parameters())
    {
        parameterSymbol->ComputeMangledName();
    }
    DelegateTypeDefaultConstructor* defaultConstructor = new DelegateTypeDefaultConstructor(delegateTypeSymbol);
    symbolTable.SetFunctionIdFor(defaultConstructor);
    delegateTypeSymbol->AddMember(defaultConstructor);
    DelegateTypeCopyConstructor* copyConstructor = new DelegateTypeCopyConstructor(delegateTypeSymbol);
    symbolTable.SetFunctionIdFor(copyConstructor);
    delegateTypeSymbol->AddMember(copyConstructor);
    DelegateTypeMoveConstructor* moveConstructor = new DelegateTypeMoveConstructor(delegateTypeSymbol);
    symbolTable.SetFunctionIdFor(moveConstructor);
    delegateTypeSymbol->AddMember(moveConstructor);
    DelegateTypeCopyAssignment* copyAssignment = new DelegateTypeCopyAssignment(delegateTypeSymbol, symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(copyAssignment);
    delegateTypeSymbol->AddMember(copyAssignment);
    DelegateTypeMoveAssignment* moveAssignment = new DelegateTypeMoveAssignment(delegateTypeSymbol, symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(moveAssignment);
    delegateTypeSymbol->AddMember(moveAssignment);
    DelegateTypeReturn* returnFun = new DelegateTypeReturn(delegateTypeSymbol);
    symbolTable.SetFunctionIdFor(returnFun);
    delegateTypeSymbol->AddMember(returnFun); 
    DelegateTypeEquality* equality = new DelegateTypeEquality(delegateTypeSymbol, symbolTable.GetTypeByName(U"bool"));
    symbolTable.SetFunctionIdFor(equality);
    delegateTypeSymbol->Ns()->AddMember(equality);
}

void TypeBinder::Visit(ClassDelegateNode& classDelegateNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&classDelegateNode);
    Assert(symbol->GetSymbolType() == SymbolType::classDelegateTypeSymbol, "class delegate type symbol expected");
    ClassDelegateTypeSymbol* classDelegateTypeSymbol = static_cast<ClassDelegateTypeSymbol*>(symbol);
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(classDelegateNode.Id(), classDelegateTypeSymbol);
    }
    classDelegateTypeSymbol->SetSpecifiers(classDelegateNode.GetSpecifiers());
    classDelegateTypeSymbol->ComputeMangledName();
    DelegateTypeSymbol* memberDelegateType = new DelegateTypeSymbol(classDelegateNode.GetSpan(), U"delegate_type");
    memberDelegateType->SetModule(module);
    symbolTable.SetTypeIdFor(memberDelegateType);
    ParameterSymbol* objectParam = new ParameterSymbol(classDelegateNode.GetSpan(), U"@obj");
    TypeSymbol* voidPtrType = symbolTable.GetTypeByName(U"void")->AddPointer(classDelegateNode.GetSpan());
    objectParam->SetType(voidPtrType);
    memberDelegateType->AddMember(objectParam);
    int n = classDelegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = classDelegateNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
        Symbol* symbol = symbolTable.GetSymbol(parameterNode);
        Assert(symbol->GetSymbolType() == SymbolType::parameterSymbol, "parameter symbol expected");
        ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
        parameterSymbol->SetType(parameterType);
        ParameterSymbol* memberParam = new ParameterSymbol(classDelegateNode.GetSpan(), ToUtf32("@p" + std::to_string(i)));
        memberParam->SetType(parameterType);
        memberDelegateType->AddMember(memberParam);
    }
    TypeSymbol* returnType = ResolveType(classDelegateNode.ReturnTypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    classDelegateTypeSymbol->SetReturnType(returnType);
    if (classDelegateTypeSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ParameterSymbol* returnParam = new ParameterSymbol(classDelegateNode.ReturnTypeExpr()->GetSpan(), U"@return");
        returnParam->SetParent(classDelegateTypeSymbol);
        returnParam->SetType(returnType->AddPointer(classDelegateNode.GetSpan()));
        classDelegateTypeSymbol->SetReturnParam(returnParam);
        ParameterSymbol* memberReturnParam = new ParameterSymbol(classDelegateNode.ReturnTypeExpr()->GetSpan(), U"@return");
        memberReturnParam->SetParent(memberDelegateType);
        memberReturnParam->SetType(returnType->AddPointer(classDelegateNode.GetSpan()));
        memberDelegateType->SetReturnParam(memberReturnParam);
    }
    memberDelegateType->SetReturnType(returnType);
    for (ParameterSymbol* parameterSymbol : classDelegateTypeSymbol->Parameters())
    {
        parameterSymbol->ComputeMangledName();
    }
    classDelegateTypeSymbol->AddMember(memberDelegateType);
    ClassTypeSymbol* objectDelegatePairType = new ClassTypeSymbol(classDelegateNode.GetSpan(), U"@objectDelegatePairType");
    objectDelegatePairType->SetAccess(SymbolAccess::public_);
    objectDelegatePairType->SetGroupName(U"@objectDelegatePairType");
    MemberVariableSymbol* objVar = new MemberVariableSymbol(classDelegateNode.GetSpan(), U"obj");
    objVar->SetAccess(SymbolAccess::public_);
    objVar->SetType(voidPtrType);
    MemberVariableSymbol* dlgVar = new MemberVariableSymbol(classDelegateNode.GetSpan(), U"dlg");
    dlgVar->SetAccess(SymbolAccess::public_);
    dlgVar->SetType(memberDelegateType);
    objectDelegatePairType->AddMember(objVar);
    objectDelegatePairType->AddMember(dlgVar);
    symbolTable.SetTypeIdFor(objectDelegatePairType);
    objectDelegatePairType->InitVmt();
    objectDelegatePairType->InitImts();
    objectDelegatePairType->CreateLayouts();
    objectDelegatePairType->SetBound();
    classDelegateTypeSymbol->AddMember(objectDelegatePairType);
    ClassDelegateTypeDefaultConstructor* defaultConstructor = new ClassDelegateTypeDefaultConstructor(classDelegateTypeSymbol);
    symbolTable.SetFunctionIdFor(defaultConstructor);
    classDelegateTypeSymbol->AddMember(defaultConstructor);
    ClassDelegateTypeCopyConstructor* copyConstructor = new ClassDelegateTypeCopyConstructor(classDelegateTypeSymbol);
    symbolTable.SetFunctionIdFor(copyConstructor);
    classDelegateTypeSymbol->AddMember(copyConstructor);
    ClassDelegateTypeMoveConstructor* moveConstructor = new ClassDelegateTypeMoveConstructor(classDelegateTypeSymbol);
    symbolTable.SetFunctionIdFor(moveConstructor);
    classDelegateTypeSymbol->AddMember(moveConstructor);
    ClassDelegateTypeCopyAssignment* copyAssignment = new ClassDelegateTypeCopyAssignment(classDelegateTypeSymbol, symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(copyAssignment);
    classDelegateTypeSymbol->AddMember(copyAssignment);
    ClassDelegateTypeMoveAssignment* moveAssignment = new ClassDelegateTypeMoveAssignment(classDelegateTypeSymbol, symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(moveAssignment);
    classDelegateTypeSymbol->AddMember(moveAssignment);
    ClassDelegateTypeEquality* equality = new ClassDelegateTypeEquality(classDelegateTypeSymbol, symbolTable.GetTypeByName(U"bool"));
    symbolTable.SetFunctionIdFor(equality);
    classDelegateTypeSymbol->Ns()->AddMember(equality);
}

void TypeBinder::Visit(ConceptNode& conceptNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&conceptNode);
    Assert(symbol->GetSymbolType() == SymbolType::conceptSymbol, "concept symbol expected");
    ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(symbol);
    BindConcept(conceptSymbol, &conceptNode);
}

void TypeBinder::BindConcept(ConceptSymbol* conceptSymbol, ConceptNode* conceptNode)
{
    if (conceptSymbol->IsBound()) return;
    conceptSymbol->SetBound();
    ContainerScope* prevContainerScope = containerScope;
    containerScope = conceptSymbol->GetContainerScope();
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(conceptNode->Id(), conceptSymbol);
        int n = conceptNode->TypeParameters().Count();
        for (int i = 0; i < n; ++i)
        {
            symbolTable.MapSymbol(conceptNode->TypeParameters()[i], conceptSymbol->TemplateParameters()[i]);
        }
        constraintSymbolBinder->BindConstraintSymbols(conceptNode, containerScope, boundCompileUnit);
    }
    conceptSymbol->SetSpecifiers(conceptNode->GetSpecifiers());
    conceptSymbol->ComputeName();
    if (conceptNode->Refinement())
    {
        ConceptIdNode* refinedConceptIdNode = conceptNode->Refinement();
        ConceptSymbol* refinedConceptSymbol = ResolveConceptId(refinedConceptIdNode, boundCompileUnit, containerScope);
        if (refinedConceptSymbol->IsProject())
        {
            Node* node = symbolTable.GetNode(refinedConceptSymbol);
            Assert(node->GetNodeType() == NodeType::conceptNode, "concept node expected");
            ConceptNode* refinedConceptNode = static_cast<ConceptNode*>(node);
            BindConcept(refinedConceptSymbol, refinedConceptNode);
        }
        conceptSymbol->SetRefinedConcept(refinedConceptSymbol);
    }
    containerScope = prevContainerScope;
}

void TypeBinder::Visit(CompoundStatementNode& compoundStatementNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&compoundStatementNode);
    Assert(symbol->GetSymbolType() == SymbolType::declarationBlock, "declaration block expected");
    DeclarationBlock* declarationBlock = static_cast<DeclarationBlock*>(symbol);
    ContainerScope* prevContainerScope = containerScope;
    containerScope = declarationBlock->GetContainerScope();
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = compoundStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
    containerScope = prevContainerScope;
}

void TypeBinder::Visit(IfStatementNode& ifStatementNode)
{
    ifStatementNode.ThenS()->Accept(*this);
    if (ifStatementNode.ElseS())
    {
        ifStatementNode.ElseS()->Accept(*this);
    }
}

void TypeBinder::Visit(WhileStatementNode& whileStatementNode)
{
    whileStatementNode.Statement()->Accept(*this);
}

void TypeBinder::Visit(DoStatementNode& doStatementNode)
{
    doStatementNode.Statement()->Accept(*this);
}

void TypeBinder::Visit(ForStatementNode& forStatementNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&forStatementNode);
    Assert(symbol->GetSymbolType() == SymbolType::declarationBlock, "declaration block expected");
    DeclarationBlock* declarationBlock = static_cast<DeclarationBlock*>(symbol);
    ContainerScope* prevContainerScope = containerScope;
    containerScope = declarationBlock->GetContainerScope();
    forStatementNode.InitS()->Accept(*this);
    forStatementNode.ActionS()->Accept(*this);
    containerScope = prevContainerScope;
}

void TypeBinder::Visit(ConstructionStatementNode& constructionStatementNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&constructionStatementNode);
    Assert(symbol->GetSymbolType() == SymbolType::localVariableSymbol, "local variable symbol expected");
    LocalVariableSymbol* localVariableSymbol = static_cast<LocalVariableSymbol*>(symbol);
    TypeSymbol* type = ResolveType(constructionStatementNode.TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    localVariableSymbol->SetType(type);
}

void TypeBinder::Visit(SwitchStatementNode& switchStatementNode)
{
    int n = switchStatementNode.Cases().Count();
    for (int i = 0; i < n; ++i)
    {
        CaseStatementNode* caseStatement = switchStatementNode.Cases()[i];
        caseStatement->Accept(*this);
    }
    if (switchStatementNode.Default())
    {
        switchStatementNode.Default()->Accept(*this);
    }
}

void TypeBinder::Visit(CaseStatementNode& caseStatementNode)
{
    int n = caseStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = caseStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
}

void TypeBinder::Visit(DefaultStatementNode& defaultStatementNode)
{
    int n = defaultStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = defaultStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
}

void TypeBinder::Visit(TryStatementNode& tryStatementNode)
{
    currentFunctionSymbol->SetHasTry();
    tryStatementNode.TryBlock()->Accept(*this);
    int n = tryStatementNode.Catches().Count();
    for (int i = 0; i < n; ++i)
    {
        CatchNode* catchNode = tryStatementNode.Catches()[i];
        catchNode->Accept(*this);
    }
}

void TypeBinder::Visit(CatchNode& catchNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&catchNode);
    Assert(symbol->GetSymbolType() == SymbolType::declarationBlock, "declaration block expected");
    DeclarationBlock* declarationBlock = static_cast<DeclarationBlock*>(symbol);
    ContainerScope* prevContainerScope = containerScope;
    containerScope = declarationBlock->GetContainerScope();
    if (catchNode.Id())
    {
        Symbol* symbol = symbolTable.GetSymbol(catchNode.Id()); 
        Assert(symbol->GetSymbolType() == SymbolType::localVariableSymbol, "local variable symbol expected");
        LocalVariableSymbol* exceptionVarSymbol = static_cast<LocalVariableSymbol*>(symbol);
        TypeSymbol* type = ResolveType(catchNode.TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
        if (type->BaseType()->IsClassTypeSymbol())
        {
            ClassTypeSymbol* exceptionVarClassType = static_cast<ClassTypeSymbol*>(type->BaseType());
            IdentifierNode systemExceptionNode(catchNode.GetSpan(), U"System.Exception");
            TypeSymbol* systemExceptionType = ResolveType(&systemExceptionNode, boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
            Assert(systemExceptionType->IsClassTypeSymbol(), "System.Exception not of class type");
            ClassTypeSymbol* systemExceptionClassType = static_cast<ClassTypeSymbol*>(systemExceptionType);
            if (exceptionVarClassType->IsProject())
            {
                Node* exceptionVarNode = symbolTable.GetNode(exceptionVarClassType);
                Assert(exceptionVarNode->GetNodeType() == NodeType::classNode, "class node expected");
                ClassNode* exceptionVarClassNode = static_cast<ClassNode*>(exceptionVarNode);
                BindClass(exceptionVarClassType, exceptionVarClassNode, false);
            }
            if (exceptionVarClassType == systemExceptionClassType || exceptionVarClassType->HasBaseClass(systemExceptionClassType))
            {
                exceptionVarSymbol->SetType(type);
            }
            else
            {
                throw Exception(module, "exception variable must be of class type equal to System.Exception class or derive from it", catchNode.TypeExpr()->GetSpan());
            }
        }
        else
        {
            throw Exception(module, "exception variable must be of class type equal to System.Exception class or derive from it", catchNode.TypeExpr()->GetSpan());
        }
    }
    catchNode.CatchBlock()->Accept(*this);
    containerScope = prevContainerScope;
}

void TypeBinder::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    conditionalCompilationPartNode.Expr()->Accept(*this);
}

void TypeBinder::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    conditionalCompilationDisjunctionNode.Left()->Accept(*this);
    bool left = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationDisjunctionNode.Right()->Accept(*this);
    bool right = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(left || right);
}

void TypeBinder::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    conditionalCompilationConjunctionNode.Left()->Accept(*this);
    bool left = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationConjunctionNode.Right()->Accept(*this);
    bool right = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(left && right);
}

void TypeBinder::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    conditionalCompilationNotNode.Expr()->Accept(*this);
    bool operand = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(!operand);
}

void TypeBinder::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    bool defined = module->IsSymbolDefined(conditionalCompilationPrimaryNode.Symbol());
    conditionalCompilationStack.push(defined);
}

void TypeBinder::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    conditionalCompilationStatementNode.IfPart()->Accept(*this);
    bool defined = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    if (defined)
    {
        int n = conditionalCompilationStatementNode.IfPart()->Statements().Count();
        for (int i = 0; i < n; ++i)
        {
            StatementNode* statement = conditionalCompilationStatementNode.IfPart()->Statements()[i];
            statement->Accept(*this);
        }
    }
    else
    {
        bool executed = false;
        int n = conditionalCompilationStatementNode.ElifParts().Count();
        for (int i = 0; i < n; ++i)
        {
            ConditionalCompilationPartNode* elifPart = conditionalCompilationStatementNode.ElifParts()[i];
            elifPart->Accept(*this);
            bool defined = conditionalCompilationStack.top();
            conditionalCompilationStack.pop();
            if (defined)
            {
                int n = elifPart->Statements().Count();
                for (int i = 0; i < n; ++i)
                {
                    StatementNode* statement = elifPart->Statements()[i];
                    statement->Accept(*this);
                }
                executed = true;
                break;
            }
        }
        if (!executed)
        {
            ConditionalCompilationPartNode* elsePart = conditionalCompilationStatementNode.ElsePart();
            if (elsePart)
            {
                int n = elsePart->Statements().Count();
                for (int i = 0; i < n; ++i)
                {
                    StatementNode* statement = elsePart->Statements()[i];
                    statement->Accept(*this);
                }
            }
        }
    }
}

void TypeBinder::Visit(TypedefNode& typedefNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&typedefNode);
    Assert(symbol->GetSymbolType() == SymbolType::typedefSymbol, "typedef symbol expected");
    TypedefSymbol* typedefSymbol = static_cast<TypedefSymbol*>(symbol);
    BindTypedef(typedefSymbol, &typedefNode, true);
}

void TypeBinder::BindTypedef(TypedefSymbol* typedefSymbol, TypedefNode* typedefNode, bool fromOwnCompileUnit)
{
    if (typedefSymbol->IsBound()) return;
    typedefSymbol->SetBound();
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(typedefNode->Id(), typedefSymbol);
    }
    typedefSymbol->SetSpecifiers(typedefNode->GetSpecifiers());
    typedefSymbol->ComputeMangledName();
    if (!fromOwnCompileUnit)
    {
        AddUsingNodesToCurrentCompileUnit(typedefNode);
    }
    TypeSymbol* typeSymbol = ResolveType(typedefNode->TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    typedefSymbol->SetType(typeSymbol);
}

void TypeBinder::Visit(ConstantNode& constantNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&constantNode);
    Assert(symbol->GetSymbolType() == SymbolType::constantSymbol, "constant symbol expected");
    ConstantSymbol* constantSymbol = static_cast<ConstantSymbol*>(symbol);
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(constantNode.Id(), constantSymbol);
    }
    constantSymbol->SetSpecifiers(constantNode.GetSpecifiers());
    constantSymbol->ComputeMangledName();
    TypeSymbol* typeSymbol = ResolveType(constantNode.TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    constantSymbol->SetType(typeSymbol);
    constantSymbol->SetEvaluating();
    std::unique_ptr<Value> value;
    value = Evaluate(constantNode.Value(), typeSymbol, containerScope, boundCompileUnit, false, nullptr, constantNode.GetSpan());
    Value* val = value.get();
    if (val)
    {
        constantSymbol->SetType(value->GetType(&symbolTable));
        constantSymbol->SetValue(value.release());
    }
    constantSymbol->ResetEvaluating();
    if (val)
    {
        if (val->GetValueType() == ValueType::arrayValue)
        {
            boundCompileUnit.AddConstantArray(constantSymbol);
        }
        else if (val->GetValueType() == ValueType::structuredValue)
        {
            boundCompileUnit.AddConstantStructure(constantSymbol);
        }
    }
}

void TypeBinder::Visit(EnumTypeNode& enumTypeNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&enumTypeNode);
    Assert(symbol->GetSymbolType() == SymbolType::enumTypeSymbol, "enumerated type symbol expected");
    EnumTypeSymbol* enumTypeSymbol = static_cast<EnumTypeSymbol*>(symbol);
    if (enumTypeSymbol->IsBound()) return;
    enumTypeSymbol->SetBound();
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(enumTypeNode.Id(), enumTypeSymbol);
    }
    EnumTypeSymbol* prevEnumType = enumType;
    enumType = enumTypeSymbol;
    enumTypeSymbol->SetSpecifiers(enumTypeNode.GetSpecifiers());
    enumTypeSymbol->ComputeMangledName();
    TypeSymbol* underlyingType = symbolTable.GetTypeByName(U"int");
    if (enumTypeNode.GetUnderlyingType())
    {
        underlyingType = ResolveType(enumTypeNode.GetUnderlyingType(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    }
    enumTypeSymbol->SetUnderlyingType(underlyingType);
    ContainerScope* prevContainerScope = containerScope;
    containerScope = enumTypeSymbol->GetContainerScope();
    int n = enumTypeNode.Constants().Count();
    for (int i = 0; i < n; ++i)
    {
        EnumConstantNode* enumConstantNode = enumTypeNode.Constants()[i];
        enumConstantNode->Accept(*this);
    }
    TypedefSymbol* underlyingTypedef = new TypedefSymbol(enumTypeNode.GetSpan(), U"UnderlyingType");
    underlyingTypedef->SetModule(module);
    underlyingTypedef->SetAccess(SymbolAccess::public_);
    underlyingTypedef->SetType(underlyingType);
    underlyingTypedef->SetBound();
    enumTypeSymbol->AddMember(underlyingTypedef);
    EnumTypeDefaultConstructor* defaultConstructor = new EnumTypeDefaultConstructor(enumTypeSymbol);
    symbolTable.SetFunctionIdFor(defaultConstructor);
    enumTypeSymbol->AddMember(defaultConstructor);
    EnumTypeCopyConstructor* copyConstructor = new EnumTypeCopyConstructor(enumTypeSymbol);
    symbolTable.SetFunctionIdFor(copyConstructor);
    enumTypeSymbol->AddMember(copyConstructor);
    EnumTypeMoveConstructor* moveConstructor = new EnumTypeMoveConstructor(enumTypeSymbol);
    symbolTable.SetFunctionIdFor(moveConstructor);
    enumTypeSymbol->AddMember(moveConstructor);
    EnumTypeCopyAssignment* copyAssignment = new EnumTypeCopyAssignment(enumTypeSymbol, symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(copyAssignment);
    enumTypeSymbol->AddMember(copyAssignment);
    EnumTypeMoveAssignment* moveAssignment = new EnumTypeMoveAssignment(enumTypeSymbol, symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(moveAssignment);
    enumTypeSymbol->AddMember(moveAssignment);
    EnumTypeReturn* returnFun = new EnumTypeReturn(enumTypeSymbol);
    symbolTable.SetFunctionIdFor(returnFun);
    enumTypeSymbol->AddMember(returnFun);
    EnumTypeEqualityOp* equality = new EnumTypeEqualityOp(enumTypeSymbol, symbolTable.GetTypeByName(U"bool"));
    symbolTable.SetFunctionIdFor(equality);
    enumTypeSymbol->Ns()->AddMember(equality);
    EnumTypeToUnderlyingTypeConversion* enum2underlying = new EnumTypeToUnderlyingTypeConversion(enumTypeNode.GetSpan(), U"enum2underlying", enumTypeSymbol, underlyingType);
    symbolTable.SetFunctionIdFor(enum2underlying);
    enum2underlying->SetParent(enumTypeSymbol);
    symbolTable.AddConversion(enum2underlying);
    enumTypeSymbol->AddMember(enum2underlying);
    UnderlyingTypeToEnumTypeConversion* underlying2enum = new UnderlyingTypeToEnumTypeConversion(enumTypeNode.GetSpan(), U"underlying2enum", underlyingType, enumTypeSymbol);
    symbolTable.SetFunctionIdFor(underlying2enum);
    underlying2enum->SetParent(enumTypeSymbol);
    symbolTable.AddConversion(underlying2enum);
    enumTypeSymbol->AddMember(underlying2enum);
    containerScope = prevContainerScope;
    enumType = prevEnumType;
}

void TypeBinder::Visit(EnumConstantNode& enumConstantNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&enumConstantNode);
    Assert(symbol->GetSymbolType() == SymbolType::enumConstantSymbol, "enumeration constant symbol expected");
    EnumConstantSymbol* enumConstantSymbol = static_cast<EnumConstantSymbol*>(symbol);
    enumConstantSymbol->ComputeMangledName();
    enumConstantSymbol->SetEvaluating();
    std::unique_ptr<Value> value = Evaluate(enumConstantNode.GetValue(), enumType->UnderlyingType(), containerScope, boundCompileUnit, false, nullptr, enumConstantNode.GetSpan());
    enumConstantSymbol->SetValue(value.release());
    enumConstantSymbol->ResetEvaluating();
}

void TypeBinder::Visit(GlobalVariableNode& globalVariableNode)
{
    Symbol* symbol = symbolTable.GetSymbol(&globalVariableNode);
    Assert(symbol->GetSymbolType() == SymbolType::globalVariableSymbol, "global variable symbol expected");
    GlobalVariableSymbol* globalVariableSymbol = static_cast<GlobalVariableSymbol*>(symbol);
    if (GetGlobalFlag(GlobalFlags::cmdoc))
    {
        symbolTable.MapSymbol(globalVariableNode.Id(), globalVariableSymbol);
    }
    globalVariableSymbol->SetSpecifiers(globalVariableNode.GetSpecifiers());
    globalVariableSymbol->ComputeMangledName();
    ContainerScope* prevContainerScope = containerScope;
    containerScope = globalVariableSymbol->GetContainerScope();
    TypeSymbol* typeSymbol = ResolveType(globalVariableNode.TypeExpr(), boundCompileUnit, containerScope, typeResolverFlags, currentClassTypeSymbol);
    globalVariableSymbol->SetType(typeSymbol);
    if (globalVariableNode.Initializer())
    {
        std::unique_ptr<Value> value;
        value = Evaluate(globalVariableNode.Initializer(), typeSymbol, containerScope, boundCompileUnit, false, nullptr, globalVariableNode.GetSpan());
        Value* val = value.get();
        if (val)
        {
            globalVariableSymbol->SetType(value->GetType(&symbolTable));
            globalVariableSymbol->SetInitializer(std::move(value));
        }
    }
    boundGlobalVariable = new BoundGlobalVariable(module, globalVariableSymbol->GetSpan(), globalVariableSymbol);
    boundCompileUnit.AddBoundNode(std::unique_ptr<BoundNode>(boundGlobalVariable));
    containerScope = prevContainerScope;
}

void TypeBinder::CreateMemberSymbols()
{
    typeResolverFlags = typeResolverFlags | TypeResolverFlags::createMemberSymbols;
}

} } // namespace cmajor::binder
