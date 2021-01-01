// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/InlineFunctionRepository.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/util/Util.hpp>

namespace cmajor { namespace binder {

using namespace soulng::util;

InlineFunctionRepository::InlineFunctionRepository(BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
}

FunctionSymbol* InlineFunctionRepository::Instantiate(FunctionSymbol* inlineFunction, ContainerScope* containerScope, const Span& span, const boost::uuids::uuid& moduleId)
{
    if (inlineFunction->GetCompileUnit() == boundCompileUnit.GetCompileUnitNode()) return inlineFunction;
    while (inlineFunction->Master())
    {
        inlineFunction = inlineFunction->Master();
    }
    auto it = inlineFunctionMap.find(inlineFunction);
    if (it != inlineFunctionMap.cend())
    {
        return it->second;
    }
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    Node* node = symbolTable.GetNodeNoThrow(inlineFunction);
    if (!node)
    {
        node = inlineFunction->GetFunctionNode();
        symbolTable.MapNode(node, inlineFunction);
        Assert(node, "function node not read");
    }
    FunctionNode* functionNode = static_cast<FunctionNode*>(node);
    std::unique_ptr<NamespaceNode> globalNs(new NamespaceNode(functionNode->GetSpan(), functionNode->ModuleId(), new IdentifierNode(functionNode->GetSpan(), functionNode->ModuleId(), U"")));
    NamespaceNode* currentNs = globalNs.get();
    CloneContext cloneContext;
    cloneContext.SetInstantiateFunctionNode();
    bool fileScopeAdded = false;
    int n = inlineFunction->UsingNodes().Count();
    if (!inlineFunction->Ns()->IsGlobalNamespace() || n > 0)
    {
        FileScope* primaryFileScope = new FileScope();
        if (!inlineFunction->Ns()->IsGlobalNamespace())
        {
            primaryFileScope->AddContainerScope(inlineFunction->Ns()->GetContainerScope());
        }
        for (int i = 0; i < n; ++i)
        {
            Node* usingNode = inlineFunction->UsingNodes()[i];
            if (usingNode->GetNodeType() == NodeType::namespaceImportNode)
            {
                primaryFileScope->InstallNamespaceImport(containerScope, static_cast<NamespaceImportNode*>(usingNode));
            }
            else if (usingNode->GetNodeType() == NodeType::aliasNode)
            {
                primaryFileScope->InstallAlias(containerScope, static_cast<AliasNode*>(usingNode));
            }
        }
        boundCompileUnit.AddFileScope(primaryFileScope);
        fileScopeAdded = true;
        std::u32string fullNsName = inlineFunction->Ns()->FullName();
        std::vector<std::u32string> nsComponents = Split(fullNsName, '.');
        for (const std::u32string& nsComponent : nsComponents)
        {
            NamespaceNode* nsNode = new NamespaceNode(functionNode->GetSpan(), functionNode->ModuleId(), new IdentifierNode(functionNode->GetSpan(), functionNode->ModuleId(), nsComponent));
            currentNs->AddMember(nsNode);
            currentNs = nsNode;
        }
    }
    FunctionNode* functionInstanceNode = static_cast<FunctionNode*>(functionNode->Clone(cloneContext));
    if (inlineFunction->IsDefault())
    {
        functionInstanceNode->SetBody(new sngcm::ast::CompoundStatementNode(span, moduleId));
        inlineFunction->SetHasArtificialBody();
    }
    currentNs->AddMember(functionInstanceNode);
    symbolTable.SetCurrentCompileUnit(boundCompileUnit.GetCompileUnitNode());
    if (!inlineFunction->Parent()->IsClassTypeSymbol())
    {
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        symbolCreatorVisitor.SetLeaveFunction();
        globalNs->Accept(symbolCreatorVisitor);
        std::unique_ptr<FunctionSymbol> functionSymbol(symbolTable.GetCreatedFunctionSymbol());
        if (inlineFunction->IsDefault())
        {
            functionSymbol->SetHasArtificialBody();
        }
        functionSymbol->SetParent(inlineFunction->Parent());
        functionSymbol->SetLinkOnceOdrLinkage();
        if (inlineFunction->IsSystemDefault())
        {
            functionSymbol->SetSystemDefault();
        }
        TypeBinder typeBinder(boundCompileUnit);
        typeBinder.SetContainerScope(functionSymbol->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(functionSymbol.get());
        functionInstanceNode->Accept(typeBinder);
        StatementBinder statementBinder(boundCompileUnit);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, functionSymbol.get()));
        statementBinder.SetCurrentFunction(boundFunction.get());
        statementBinder.SetContainerScope(functionSymbol->GetContainerScope());
        functionInstanceNode->Body()->Accept(statementBinder);
        BoundStatement* boundStatement = statementBinder.ReleaseStatement();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        boundCompileUnit.AddBoundNode(std::move(boundFunction));
        if (fileScopeAdded)
        {
            boundCompileUnit.RemoveLastFileScope();
        }
        FunctionSymbol* result = functionSymbol.get();
        boundCompileUnit.GetSymbolTable().AddFunctionSymbol(std::move(functionSymbol));
        boundCompileUnit.AddGlobalNs(std::move(globalNs));
        inlineFunctionMap[inlineFunction] = result;
        result->SetFunctionId(inlineFunction->FunctionId());
        result->SetMaster(inlineFunction);
        result->SetCopy();
        return result;
    }
    else
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(inlineFunction->Parent());
        symbolTable.SetCurrentClass(classTypeSymbol);
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        symbolCreatorVisitor.SetLeaveFunction();
        globalNs->Accept(symbolCreatorVisitor);
        std::unique_ptr<FunctionSymbol> functionSymbol(symbolTable.GetCreatedFunctionSymbol());
        functionSymbol->SetVmtIndex(inlineFunction->VmtIndex());
        functionSymbol->SetImtIndex(inlineFunction->ImtIndex());
        if (inlineFunction->IsDefault())
        {
            functionSymbol->SetHasArtificialBody();
        }
        functionSymbol->SetParent(classTypeSymbol);
        functionSymbol->SetLinkOnceOdrLinkage();
        if (inlineFunction->IsSystemDefault())
        {
            functionSymbol->SetSystemDefault();
        }
        TypeBinder typeBinder(boundCompileUnit);
        typeBinder.SetContainerScope(functionSymbol->GetContainerScope());
        functionInstanceNode->Accept(typeBinder);
        StatementBinder statementBinder(boundCompileUnit);
        std::unique_ptr<BoundClass> boundClass(new BoundClass(classTypeSymbol));
        boundClass->SetInlineFunctionContainer();
        statementBinder.SetCurrentClass(boundClass.get());
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, functionSymbol.get()));
        statementBinder.SetCurrentFunction(boundFunction.get());
        statementBinder.SetContainerScope(functionSymbol->GetContainerScope());
        if (functionSymbol->GetSymbolType() == SymbolType::constructorSymbol)
        {
            ConstructorSymbol* constructorSymbol = static_cast<ConstructorSymbol*>(functionSymbol.get());
            Node* node = symbolTable.GetNode(functionSymbol.get());
            Assert(node->GetNodeType() == NodeType::constructorNode, "constructor node expected");
            ConstructorNode* constructorNode = static_cast<ConstructorNode*>(node);
            statementBinder.SetCurrentConstructor(constructorSymbol, constructorNode);
        }
        else if (functionSymbol->GetSymbolType() == SymbolType::destructorSymbol)
        {
            DestructorSymbol* destructorSymbol = static_cast<DestructorSymbol*>(functionSymbol.get());
            Node* node = symbolTable.GetNode(functionSymbol.get());
            Assert(node->GetNodeType() == NodeType::destructorNode, "destructor node expected");
            DestructorNode* destructorNode = static_cast<DestructorNode*>(node);
            statementBinder.SetCurrentDestructor(destructorSymbol, destructorNode);
        }
        else if (functionSymbol->GetSymbolType() == SymbolType::memberFunctionSymbol)
        {
            MemberFunctionSymbol* memberFunctionSymbol = static_cast<MemberFunctionSymbol*>(functionSymbol.get());
            Node* node = symbolTable.GetNode(functionSymbol.get());
            Assert(node->GetNodeType() == NodeType::memberFunctionNode, "member function node expected");
            MemberFunctionNode* memberFunctionNode = static_cast<MemberFunctionNode*>(node);
            statementBinder.SetCurrentMemberFunction(memberFunctionSymbol, memberFunctionNode);
        }
        functionInstanceNode->Body()->Accept(statementBinder);
        BoundStatement* boundStatement = statementBinder.ReleaseStatement();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        boundClass->AddMember(std::move(boundFunction));
        boundCompileUnit.AddBoundNode(std::move(boundClass));
        FunctionSymbol* result = functionSymbol.get();
        boundCompileUnit.AddGlobalNs(std::move(globalNs));
        boundCompileUnit.GetSymbolTable().AddFunctionSymbol(std::move(functionSymbol));
        if (fileScopeAdded)
        {
            boundCompileUnit.RemoveLastFileScope();
        }
        inlineFunctionMap[inlineFunction] = result;
        result->SetFunctionId(inlineFunction->FunctionId());
        result->SetMaster(inlineFunction);
        result->SetCopy();
        return result;
    }
}

} } // namespace cmajor::binder
