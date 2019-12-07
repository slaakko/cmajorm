// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_TYPE_BINDER_INCLUDED
#define CMAJOR_BINDER_TYPE_BINDER_INCLUDED
#include <cmajor/binder/BinderApi.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/symbols/Scope.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::ast;
using namespace cmajor::symbols;

class BoundCompileUnit;
class AttributeBinder;
class BoundGlobalVariable;

class BINDER_API ConstraintSymbolBinder
{
public:
    virtual ~ConstraintSymbolBinder();
    virtual void BindConstraintSymbols(Node* node, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit) = 0;
};

BINDER_API void SetConstraintSymbolBinder(ConstraintSymbolBinder* constraintSymbolBinder_);

class BINDER_API TypeBinder : public Visitor
{
public:
    TypeBinder(BoundCompileUnit& boundCompileUnit_);
    void AddUsingNodesToCurrentCompileUnit(Node* node);
    void Visit(CompileUnitNode& compileUnitNode) override;
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(AliasNode& aliasNode) override;
    void Visit(NamespaceImportNode& namespaceImportNode) override;
    void Visit(FunctionNode& functionNode) override;
    void Visit(ClassNode& classNode) override;
    void BindClass(ClassTypeSymbol* classTypeSymbol, ClassNode* classNode, bool fromOwnCompileUnit);
    void BindClassTemplate(ClassTypeSymbol* classTypeSymbol, ClassNode* classNode);
    void Visit(StaticConstructorNode& staticConstructorNode) override;
    void Visit(ConstructorNode& constructorNode) override;
    void Visit(DestructorNode& destructorNode) override;
    void Visit(MemberFunctionNode& memberFunctionNode) override;
    void Visit(ConversionFunctionNode& conversionFunctionNode) override;
    void Visit(MemberVariableNode& memberVariableNode) override;
    void Visit(InterfaceNode& interfaceNode) override;
    void BindInterface(InterfaceTypeSymbol* interfaceTypeSymbol, InterfaceNode* interfaceNode, bool fromOwnCompileUnit);
    void Visit(DelegateNode& delegateNode) override;
    void Visit(ClassDelegateNode& classDelegateNode) override;
    void Visit(ConceptNode& conceptNode) override;
    void BindConcept(ConceptSymbol* conceptSymbol, ConceptNode* conceptNode);

    void Visit(CompoundStatementNode& compoundStatementNode) override;
    void Visit(IfStatementNode& ifStatementNode) override;
    void Visit(WhileStatementNode& whileStatementNode) override;
    void Visit(DoStatementNode& doStatementNode) override;
    void Visit(ForStatementNode& forStatementNode) override;
    void Visit(ConstructionStatementNode& constructionStatementNode) override;
    void Visit(SwitchStatementNode& switchStatementNode) override;
    void Visit(CaseStatementNode& caseStatementNode) override;
    void Visit(DefaultStatementNode& defaultStatementNode) override;
    void Visit(TryStatementNode& tryStatementNode) override;
    void Visit(CatchNode& catchNode) override;
    void Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode) override;
    void Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode) override;
    void Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode) override;
    void Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode) override;
    void Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode) override;
    void Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode) override;

    void Visit(TypedefNode& typedefNode) override;
    void BindTypedef(TypedefSymbol* typedefSymbol, TypedefNode* typedefNode, bool fromOwnCompileUnit);
    void Visit(ConstantNode& constantNode) override;
    void Visit(EnumTypeNode& enumTypeNode) override;
    void Visit(EnumConstantNode& enumConstantNode) override;
    void Visit(GlobalVariableNode& globalVariableNode) override;

    void SetContainerScope(ContainerScope* containerScope_) { containerScope = containerScope_; }
    void SetCurrentFunctionSymbol(FunctionSymbol* currentFunctionSymbol_) { currentFunctionSymbol = currentFunctionSymbol_; }
    void CreateMemberSymbols();
    BoundGlobalVariable* GetBoundGlobalVariable() const { return boundGlobalVariable; }
private:
    BoundCompileUnit& boundCompileUnit;
    SymbolTable& symbolTable;
    Module* module;
    ContainerScope* containerScope;
    std::vector<Node*> usingNodes;
    EnumTypeSymbol* enumType;
    FunctionSymbol* currentFunctionSymbol;
    ClassTypeSymbol* currentClassTypeSymbol;
    std::stack<bool> conditionalCompilationStack;
    TypeResolverFlags typeResolverFlags;
    BoundGlobalVariable* boundGlobalVariable;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_TYPE_BINDER_INCLUDED
