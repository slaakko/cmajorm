// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/ClassTemplateRepository.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/Concept.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/util/Util.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::util;
using namespace soulng::unicode;

size_t ClassIdMemberFunctionIndexHash::operator()(const std::pair<boost::uuids::uuid, int>& p) const
{
    return boost::hash<boost::uuids::uuid>()(p.first) ^ boost::hash<int>()(p.second);
}

ClassTemplateRepository::ClassTemplateRepository(BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
}

void ClassTemplateRepository::ResolveDefaultTemplateArguments(std::vector<TypeSymbol*>& templateArgumentTypes, ClassTypeSymbol* classTemplate, ContainerScope* containerScope, const Span& span)
{
    int n = classTemplate->TemplateParameters().size();
    int m = templateArgumentTypes.size();
    if (m == n) return;
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    Node* node = symbolTable.GetNodeNoThrow(classTemplate);
    if (!node)
    {
        node = classTemplate->GetClassNode();
        Assert(node, "class node not read");
    }
    Assert(node->GetNodeType() == NodeType::classNode, "class node expected");
    ClassNode* classNode = static_cast<ClassNode*>(node);
    int numFileScopeAdded = 0;
    int nu = classTemplate->UsingNodes().Count();
    if (nu > 0)
    {
        FileScope* fileScope = new FileScope(&boundCompileUnit.GetModule());
        for (int i = 0; i < nu; ++i)
        {
            Node* usingNode = classTemplate->UsingNodes()[i];
            if (usingNode->GetNodeType() == NodeType::namespaceImportNode)
            {
                NamespaceImportNode* namespaceImportNode = static_cast<NamespaceImportNode*>(usingNode);
                fileScope->InstallNamespaceImport(containerScope, namespaceImportNode);
            }
            else if (usingNode->GetNodeType() == NodeType::aliasNode)
            {
                AliasNode* aliasNode = static_cast<AliasNode*>(usingNode);
                fileScope->InstallAlias(containerScope, aliasNode);
            }
        }
        boundCompileUnit.AddFileScope(fileScope);
        ++numFileScopeAdded;
    }
    if (!classTemplate->Ns()->IsGlobalNamespace())
    {
        FileScope* primaryFileScope = new FileScope(&boundCompileUnit.GetModule());
        primaryFileScope->AddContainerScope(classTemplate->Ns()->GetContainerScope());
        boundCompileUnit.AddFileScope(primaryFileScope);
        ++numFileScopeAdded;
    }
    ContainerScope resolveScope;
    resolveScope.SetParentScope(containerScope);
    std::vector<std::unique_ptr<BoundTemplateParameterSymbol>> boundTemplateParameters;
    for (int i = 0; i < n; ++i)
    {
        TemplateParameterSymbol* templateParameterSymbol = classTemplate->TemplateParameters()[i];
        BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, templateParameterSymbol->Name());
        boundTemplateParameters.push_back(std::unique_ptr<BoundTemplateParameterSymbol>(boundTemplateParameter));
        if (i < m)
        {
            boundTemplateParameter->SetType(templateArgumentTypes[i]);
            resolveScope.Install(boundTemplateParameter);
        }
        else
        {
            if (i >= classNode->TemplateParameters().Count())
            {
                throw Exception(&boundCompileUnit.GetModule(), "too few template arguments", span);
            }
            Node* defaultTemplateArgumentNode = classNode->TemplateParameters()[i]->DefaultTemplateArgument();
            if (!defaultTemplateArgumentNode)
            {
                throw Exception(&boundCompileUnit.GetModule(), "too few template arguments", span);
            }
            TypeSymbol* templateArgumentType = ResolveType(defaultTemplateArgumentNode, boundCompileUnit, &resolveScope);
            templateArgumentTypes.push_back(templateArgumentType);
        }
    }
    for (int i = 0; i < numFileScopeAdded; ++i)
    {
        boundCompileUnit.RemoveLastFileScope();
    }
}

void ClassTemplateRepository::BindClassTemplateSpecialization(ClassTemplateSpecializationSymbol* classTemplateSpecialization, ContainerScope* containerScope, const Span& span)
{
    if (classTemplateSpecialization->IsBound()) return;
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    ClassTypeSymbol* classTemplate = classTemplateSpecialization->GetClassTemplate();
    Node* node = symbolTable.GetNodeNoThrow(classTemplate);
    if (!node)
    {
        node = classTemplate->GetClassNode();
        Assert(node, "class node not read");
    }
    Assert(node->GetNodeType() == NodeType::classNode, "class node expected");
    ClassNode* classNode = static_cast<ClassNode*>(node);
    std::unique_ptr<NamespaceNode> globalNs(new NamespaceNode(classTemplate->GetSpan(), new IdentifierNode(classTemplate->GetSpan(), U"")));
    NamespaceNode* currentNs = globalNs.get();
    CloneContext cloneContext;
    cloneContext.SetInstantiateClassNode();
    int nu = classTemplate->UsingNodes().Count();
    for (int i = 0; i < nu; ++i)
    {
        Node* usingNode = classTemplate->UsingNodes()[i];
        globalNs->AddMember(usingNode->Clone(cloneContext));
    }
    bool fileScopeAdded = false;
    if (!classTemplate->Ns()->IsGlobalNamespace())
    {
        FileScope* primaryFileScope = new FileScope(&boundCompileUnit.GetModule());
        primaryFileScope->AddContainerScope(classTemplate->Ns()->GetContainerScope());
        boundCompileUnit.AddFileScope(primaryFileScope);
        fileScopeAdded = true;
        std::u32string fullNsName = classTemplate->Ns()->FullName();
        std::vector<std::u32string> nsComponents = Split(fullNsName, '.');
        for (const std::u32string& nsComponent : nsComponents)
        {
            NamespaceNode* nsNode = new NamespaceNode(classTemplate->GetSpan(), new IdentifierNode(classTemplate->GetSpan(), nsComponent));
            currentNs->AddMember(nsNode);
            currentNs = nsNode;
        }
    }
    ClassNode* classInstanceNode = static_cast<ClassNode*>(classNode->Clone(cloneContext));
    currentNs->AddMember(classInstanceNode);
    int n = classTemplate->TemplateParameters().size();
    int m = classTemplateSpecialization->TemplateArgumentTypes().size();
    if (n != m)
    {
        throw Exception(&boundCompileUnit.GetModule(), "wrong number of template arguments", span);
    }
    bool templateParameterBinding = false;
    ContainerScope resolveScope;
    resolveScope.SetParentScope(containerScope);
    for (int i = 0; i < n; ++i)
    {
        TemplateParameterSymbol* templateParameter = classTemplate->TemplateParameters()[i];
        BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, templateParameter->Name());
        boundTemplateParameter->SetParent(classTemplateSpecialization);
        TypeSymbol* templateArgumentType = classTemplateSpecialization->TemplateArgumentTypes()[i];
        boundTemplateParameter->SetType(templateArgumentType);
        if (templateArgumentType->GetSymbolType() == SymbolType::templateParameterSymbol)
        {
            templateParameterBinding = true;
            if (classTemplateSpecialization->IsPrototype())
            {
                if (classTemplateSpecialization->IsProject())
                {
                    resolveScope.Install(boundTemplateParameter);
                    TemplateParameterNode* templateParameterNode = classNode->TemplateParameters()[i];
                    Node* defaultTemplateArgumentNode = templateParameterNode->DefaultTemplateArgument();
                    if (defaultTemplateArgumentNode)
                    {
                        TypeSymbol* templateArgumentType = ResolveType(defaultTemplateArgumentNode, boundCompileUnit, &resolveScope);
                        templateParameter->SetDefaultType(templateArgumentType);
                    }
                }
            }
        }
        classTemplateSpecialization->AddMember(boundTemplateParameter);
    }
    symbolTable.SetCurrentCompileUnit(boundCompileUnit.GetCompileUnitNode());
    SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
    symbolCreatorVisitor.SetClassInstanceNode(classInstanceNode);
    symbolCreatorVisitor.SetClassTemplateSpecialization(classTemplateSpecialization);
    globalNs->Accept(symbolCreatorVisitor);
    TypeBinder typeBinder(boundCompileUnit);
    if (templateParameterBinding)
    {
        typeBinder.CreateMemberSymbols();
    }
    globalNs->Accept(typeBinder);
    if (templateParameterBinding)
    {
        classTemplateSpecialization->SetGlobalNs(std::move(globalNs)); 
        if (fileScopeAdded)
        {
            boundCompileUnit.RemoveLastFileScope();
        }
    }
    else if (boundCompileUnit.BindingTypes())
    {
        classTemplateSpecialization->SetGlobalNs(std::move(globalNs)); 
        classTemplateSpecialization->SetStatementsNotBound();
        if (fileScopeAdded)
        {
            FileScope* fileScope = boundCompileUnit.ReleaseLastFileScope();
            classTemplateSpecialization->SetFileScope(fileScope);
        }
    }
    else
    {
        StatementBinder statementBinder(boundCompileUnit);
        globalNs->Accept(statementBinder);
        classTemplateSpecialization->SetGlobalNs(std::move(globalNs)); 
        if (fileScopeAdded)
        {
            boundCompileUnit.RemoveLastFileScope();
        }
    }
}

bool ClassTemplateRepository::Instantiate(FunctionSymbol* memberFunction, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span)
{
/*
    if (memberFunction->IsDefault())
    {
        throw Exception(&boundCompileUnit.GetModule(), "Compilation of class template member function '" + ToUtf8(memberFunction->FullName()) +
            "' failed: 'default' members in class templates not supported.", memberFunction->GetSpan(), span);
    }
*/
    if (instantiatedMemberFunctions.find(memberFunction) != instantiatedMemberFunctions.cend()) return true;
    instantiatedMemberFunctions.insert(memberFunction);
    try
    {
        SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
        Symbol* parent = memberFunction->Parent();
        Assert(parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class template specialization expected");
        ClassTemplateSpecializationSymbol* classTemplateSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(parent);
        std::pair<boost::uuids::uuid, int> classIdMemFunIndexPair = std::make_pair(classTemplateSpecialization->TypeId(), memberFunction->GetIndex()); 
        if (classIdMemberFunctionIndexSet.find(classIdMemFunIndexPair) != classIdMemberFunctionIndexSet.cend())
        {
//          If <parent class id, member function index> pair is found from the classIdMemberFunctionIndexSet, the member function is already instantiated 
//          for this compile unit, so return true.
            instantiatedMemberFunctions.insert(memberFunction);
            return true;
        }
        Assert(classTemplateSpecialization->IsBound(), "class template specialization not bound");
        Node* node = symbolTable.GetNodeNoThrow(memberFunction);
        if (!node)
        {
            return false;
        }
        boundCompileUnit.FinalizeBinding(classTemplateSpecialization);
        ClassTypeSymbol* classTemplate = classTemplateSpecialization->GetClassTemplate();
        std::unordered_map<TemplateParameterSymbol*, TypeSymbol*> templateParameterMap;
        int n = classTemplateSpecialization->TemplateArgumentTypes().size();
        for (int i = 0; i < n; ++i)
        {
            TemplateParameterSymbol* templateParameter = classTemplate->TemplateParameters()[i];
            TypeSymbol* templateArgument = classTemplateSpecialization->TemplateArgumentTypes()[i];
            templateParameterMap[templateParameter] = templateArgument;
        }
        if (!classTemplateSpecialization->IsConstraintChecked())
        {
            classTemplateSpecialization->SetConstraintChecked();
            if (classTemplate->Constraint())
            {
                std::unique_ptr<BoundConstraint> boundConstraint;
                std::unique_ptr<Exception> conceptCheckException;
                if (!CheckConstraint(classTemplate->Constraint(), classTemplate->UsingNodes(), boundCompileUnit, containerScope, currentFunction, classTemplate->TemplateParameters(),
                    templateParameterMap, boundConstraint, span, memberFunction, conceptCheckException))
                {
                    if (conceptCheckException)
                    {
                        throw Exception(&boundCompileUnit.GetModule(), "concept check of class template specialization '" + ToUtf8(classTemplateSpecialization->FullName()) + "' failed: " + conceptCheckException->Message(), span,
                            conceptCheckException->References());
                    }
                    else
                    {
                        throw Exception(&boundCompileUnit.GetModule(), "concept check of class template specialization '" + ToUtf8(classTemplateSpecialization->FullName()) + "' failed.", span);
                    }
                }
            }
        }
        FileScope* fileScope = new FileScope(&boundCompileUnit.GetModule());
        int nu = classTemplate->UsingNodes().Count();
        for (int i = 0; i < nu; ++i)
        {
            Node* usingNode = classTemplate->UsingNodes()[i];
            if (usingNode->GetNodeType() == NodeType::namespaceImportNode)
            {
                NamespaceImportNode* namespaceImportNode = static_cast<NamespaceImportNode*>(usingNode);
                fileScope->InstallNamespaceImport(containerScope, namespaceImportNode);
            }
            else if (usingNode->GetNodeType() == NodeType::aliasNode)
            {
                AliasNode* aliasNode = static_cast<AliasNode*>(usingNode);
                fileScope->InstallAlias(containerScope, aliasNode);
            }
        }
        if (!classTemplate->Ns()->IsGlobalNamespace())
        {
            fileScope->AddContainerScope(classTemplate->Ns()->GetContainerScope());
        }
        boundCompileUnit.AddFileScope(fileScope);
        Assert(node->IsFunctionNode(), "function node expected");
        FunctionNode* functionInstanceNode = static_cast<FunctionNode*>(node);
        if (memberFunction->IsDefault())
        {
            functionInstanceNode->SetBodySource(new sngcm::ast::CompoundStatementNode(span));
        }
        Assert(functionInstanceNode->BodySource(), "body source expected");
        CloneContext cloneContext;
        functionInstanceNode->SetBody(static_cast<CompoundStatementNode*>(functionInstanceNode->BodySource()->Clone(cloneContext)));
        if (functionInstanceNode->WhereConstraint())
        {
            std::unique_ptr<BoundConstraint> boundConstraint;
            std::unique_ptr<Exception> conceptCheckException;
            FileScope* classTemplateScope = new FileScope(&boundCompileUnit.GetModule());
            classTemplateScope->AddContainerScope(classTemplateSpecialization->GetContainerScope());
            boundCompileUnit.AddFileScope(classTemplateScope);
            if (!CheckConstraint(functionInstanceNode->WhereConstraint(), classTemplate->UsingNodes(), boundCompileUnit, containerScope, currentFunction, classTemplate->TemplateParameters(),
                templateParameterMap, boundConstraint, span, memberFunction, conceptCheckException))
            {
                boundCompileUnit.RemoveLastFileScope();
                if (conceptCheckException)
                {
                    std::vector<Span> references;
                    references.push_back(conceptCheckException->Defined());
                    references.insert(references.end(), conceptCheckException->References().begin(), conceptCheckException->References().end());
                    throw Exception(&boundCompileUnit.GetModule(), "concept check of class template member function '" + ToUtf8(memberFunction->FullName()) + "' failed: " + conceptCheckException->Message(), span, references);
                }
                else
                {
                    throw Exception(&boundCompileUnit.GetModule(), "concept check of class template template member function '" + ToUtf8(memberFunction->FullName()) + "' failed.", span);
                }
            }
            else
            {
                boundCompileUnit.RemoveLastFileScope();
            }
        }
        symbolTable.SetCurrentCompileUnit(boundCompileUnit.GetCompileUnitNode());
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        symbolTable.BeginContainer(memberFunction);
        functionInstanceNode->Body()->Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(boundCompileUnit);
        typeBinder.SetContainerScope(memberFunction->GetContainerScope());
        functionInstanceNode->Body()->Accept(typeBinder);
        StatementBinder statementBinder(boundCompileUnit);
        std::unique_ptr<BoundClass> boundClass(new BoundClass(&boundCompileUnit.GetModule(), classTemplateSpecialization));
        statementBinder.SetCurrentClass(boundClass.get());
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit.GetModule(), memberFunction));
        statementBinder.SetCurrentFunction(boundFunction.get());
        statementBinder.SetContainerScope(memberFunction->GetContainerScope());
        if (memberFunction->GetSymbolType() == SymbolType::constructorSymbol)
        {
            ConstructorSymbol* constructorSymbol = static_cast<ConstructorSymbol*>(memberFunction);
            Node* node = symbolTable.GetNode(memberFunction);
            Assert(node->GetNodeType() == NodeType::constructorNode, "constructor node expected");
            ConstructorNode* constructorNode = static_cast<ConstructorNode*>(node);
            statementBinder.SetCurrentConstructor(constructorSymbol, constructorNode);
        }
        else if (memberFunction->GetSymbolType() == SymbolType::destructorSymbol)
        {
            DestructorSymbol* destructorSymbol = static_cast<DestructorSymbol*>(memberFunction);
            Node* node = symbolTable.GetNode(memberFunction);
            Assert(node->GetNodeType() == NodeType::destructorNode, "destructor node expected");
            DestructorNode* destructorNode = static_cast<DestructorNode*>(node);
            statementBinder.SetCurrentDestructor(destructorSymbol, destructorNode);
        }
        else if (memberFunction->GetSymbolType() == SymbolType::memberFunctionSymbol)
        {
            MemberFunctionSymbol* memberFunctionSymbol = static_cast<MemberFunctionSymbol*>(memberFunction);
            Node* node = symbolTable.GetNode(memberFunction);
            Assert(node->GetNodeType() == NodeType::memberFunctionNode, "member function node expected");
            MemberFunctionNode* memberFunctionNode = static_cast<MemberFunctionNode*>(node);
            statementBinder.SetCurrentMemberFunction(memberFunctionSymbol, memberFunctionNode);
        }
        functionInstanceNode->Body()->Accept(statementBinder);
        BoundStatement* boundStatement = statementBinder.ReleaseStatement();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        std::u32string instantiatedMemberFunctionMangledName = boundFunction->GetFunctionSymbol()->MangledName();
        boundClass->AddMember(std::move(boundFunction));
        classIdMemberFunctionIndexSet.insert(classIdMemFunIndexPair);
        boundCompileUnit.AddBoundNode(std::move(boundClass));
        boundCompileUnit.RemoveLastFileScope();
        return InstantiateDestructorAndVirtualFunctions(classTemplateSpecialization, containerScope, currentFunction, span);
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(memberFunction->GetSpan());
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(&boundCompileUnit.GetModule(), "could not instantiate member function '" + ToUtf8(memberFunction->FullName()) + "'. Reason: " + ex.Message(), span, references);
    }
}

bool ClassTemplateRepository::InstantiateDestructorAndVirtualFunctions(ClassTemplateSpecializationSymbol* classTemplateSpecialization, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span)
{
    for (FunctionSymbol* virtualMemberFunction : classTemplateSpecialization->Vmt())
    {
        if (virtualMemberFunction->Parent() == classTemplateSpecialization && !virtualMemberFunction->IsGeneratedFunction())
        {
            if (!Instantiate(virtualMemberFunction, containerScope, currentFunction, span))
            {
                return false;
            }
        }
    }
    if (classTemplateSpecialization->Destructor())
    {
        if (!classTemplateSpecialization->Destructor()->IsGeneratedFunction())
        {
            if (!Instantiate(classTemplateSpecialization->Destructor(), containerScope, currentFunction, span))
            {
                return false;
            }
        }
    }
    return true;
}

} } // namespace cmajor::binder
