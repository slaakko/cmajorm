// =================================
// Copyright (c) 2021 Seppo Laakko
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

void ClassTemplateRepository::ResolveDefaultTemplateArguments(std::vector<TypeSymbol*>& templateArgumentTypes, ClassTypeSymbol* classTemplate, ContainerScope* containerScope, 
    const Span& span, const boost::uuids::uuid& moduleId)
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
        FileScope* fileScope = new FileScope();
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
        FileScope* primaryFileScope = new FileScope();
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
        BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, moduleId, templateParameterSymbol->Name());
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
                throw Exception("too few template arguments", span, moduleId);
            }
            Node* defaultTemplateArgumentNode = classNode->TemplateParameters()[i]->DefaultTemplateArgument();
            if (!defaultTemplateArgumentNode)
            {
                throw Exception("too few template arguments", span, moduleId);
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

void ClassTemplateRepository::BindClassTemplateSpecialization(ClassTemplateSpecializationSymbol* classTemplateSpecialization, ContainerScope* containerScope, 
    const Span& span,  const boost::uuids::uuid& moduleId)
{
    if (classTemplateSpecialization->IsBound()) return;
    if (classTemplateSpecialization->FullName() == U"String<char>")
    {
        int x = 0;
    }
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
    std::unique_ptr<NamespaceNode> globalNs(new NamespaceNode(classNode->GetSpan(), classNode->ModuleId(), new IdentifierNode(classNode->GetSpan(), classNode->ModuleId(), U"")));
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
        FileScope* primaryFileScope = new FileScope();
        primaryFileScope->AddContainerScope(classTemplate->Ns()->GetContainerScope());
        boundCompileUnit.AddFileScope(primaryFileScope);
        fileScopeAdded = true;
        std::u32string fullNsName = classTemplate->Ns()->FullName();
        std::vector<std::u32string> nsComponents = Split(fullNsName, '.');
        for (const std::u32string& nsComponent : nsComponents)
        {
            NamespaceNode* nsNode = new NamespaceNode(classNode->GetSpan(), classNode->ModuleId(), new IdentifierNode(classNode->GetSpan(), classNode->ModuleId(), nsComponent));
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
        throw Exception("wrong number of template arguments", span, moduleId);
    }
    bool templateParameterBinding = false;
    ContainerScope resolveScope;
    resolveScope.SetParentScope(containerScope);
    for (int i = 0; i < n; ++i)
    {
        TemplateParameterSymbol* templateParameter = classTemplate->TemplateParameters()[i];
        BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, moduleId, templateParameter->Name());
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
    typeBinder.SetContainerScope(classTemplateSpecialization->GetContainerScope());
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

bool ClassTemplateRepository::Instantiate(FunctionSymbol* memberFunction, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span, const boost::uuids::uuid& moduleId)
{
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
                    templateParameterMap, boundConstraint, span, moduleId, memberFunction, conceptCheckException))
                {
                    if (conceptCheckException)
                    {
                        throw Exception("concept check of class template specialization '" + ToUtf8(classTemplateSpecialization->FullName()) + "' failed: " + conceptCheckException->Message(), span,
                            moduleId, conceptCheckException->References());
                    }
                    else
                    {
                        throw Exception("concept check of class template specialization '" + ToUtf8(classTemplateSpecialization->FullName()) + "' failed.", span, moduleId);
                    }
                }
            }
        }
        FileScope* fileScope = new FileScope();
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
            functionInstanceNode->SetBodySource(new sngcm::ast::CompoundStatementNode(span, moduleId));
        }
        Assert(functionInstanceNode->BodySource(), "body source expected");
        CloneContext cloneContext;
        functionInstanceNode->SetBody(static_cast<CompoundStatementNode*>(functionInstanceNode->BodySource()->Clone(cloneContext)));
        if (functionInstanceNode->WhereConstraint())
        {
            std::unique_ptr<BoundConstraint> boundConstraint;
            std::unique_ptr<Exception> conceptCheckException;
            FileScope* classTemplateScope = new FileScope();
            classTemplateScope->AddContainerScope(classTemplateSpecialization->GetContainerScope());
            boundCompileUnit.AddFileScope(classTemplateScope);
            if (!CheckConstraint(functionInstanceNode->WhereConstraint(), classTemplate->UsingNodes(), boundCompileUnit, containerScope, currentFunction, classTemplate->TemplateParameters(),
                templateParameterMap, boundConstraint, span, moduleId, memberFunction, conceptCheckException))
            {
                boundCompileUnit.RemoveLastFileScope();
                if (conceptCheckException)
                {
                    std::vector<std::pair<Span, boost::uuids::uuid>> references;
                    references.push_back(std::make_pair(conceptCheckException->Defined(), conceptCheckException->DefinedModuleId()));
                    references.insert(references.end(), conceptCheckException->References().begin(), conceptCheckException->References().end());
                    throw Exception("concept check of class template member function '" + ToUtf8(memberFunction->FullName()) + "' failed: " + conceptCheckException->Message(), span, moduleId, references);
                }
                else
                {
                    throw Exception("concept check of class template template member function '" + ToUtf8(memberFunction->FullName()) + "' failed.", span, moduleId);
                }
            }
            else
            {
                boundCompileUnit.RemoveLastFileScope();
            }
        }
        std::lock_guard<std::recursive_mutex> lock(boundCompileUnit.GetModule().GetLock());
        FunctionSymbol* master = memberFunction;
        master->ResetImmutable();
        memberFunction = master->Copy();
        boundCompileUnit.GetSymbolTable().AddFunctionSymbol(std::unique_ptr<FunctionSymbol>(memberFunction));
        master->SetImmutable();
        symbolTable.SetCurrentCompileUnit(boundCompileUnit.GetCompileUnitNode());
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        symbolTable.BeginContainer(memberFunction);
        symbolTable.MapNode(functionInstanceNode, memberFunction);
        symbolCreatorVisitor.InsertTracer(functionInstanceNode->Body());
        functionInstanceNode->Body()->Accept(symbolCreatorVisitor); 
        symbolTable.EndContainer();
        TypeBinder typeBinder(boundCompileUnit);
        typeBinder.SetContainerScope(memberFunction->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(memberFunction);
        functionInstanceNode->Body()->Accept(typeBinder);
        StatementBinder statementBinder(boundCompileUnit);
        std::unique_ptr<BoundClass> boundClass(new BoundClass(classTemplateSpecialization));
        statementBinder.SetCurrentClass(boundClass.get());
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, memberFunction));
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
        return InstantiateDestructorAndVirtualFunctions(classTemplateSpecialization, containerScope, currentFunction, span, moduleId);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(memberFunction->GetSpan(), memberFunction->SourceModuleId()));
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("could not instantiate member function '" + ToUtf8(memberFunction->FullName()) + "'. Reason: " + ex.Message(), span, moduleId, references);
    }
}

bool ClassTemplateRepository::InstantiateDestructorAndVirtualFunctions(ClassTemplateSpecializationSymbol* classTemplateSpecialization, ContainerScope* containerScope, BoundFunction* currentFunction, 
    const Span& span, const boost::uuids::uuid& moduleId)
{
    for (FunctionSymbol* virtualMemberFunction : classTemplateSpecialization->Vmt())
    {
        if (virtualMemberFunction->Parent() == classTemplateSpecialization && !virtualMemberFunction->IsGeneratedFunction())
        {
            if (!Instantiate(virtualMemberFunction, containerScope, currentFunction, span, moduleId))
            {
                return false;
            }
        }
    }
    if (classTemplateSpecialization->Destructor())
    {
        if (!classTemplateSpecialization->Destructor()->IsGeneratedFunction())
        {
            if (!Instantiate(classTemplateSpecialization->Destructor(), containerScope, currentFunction, span, moduleId))
            {
                return false;
            }
        }
    }
    return true;
}

void ClassTemplateRepository::InstantiateAll(ClassTemplateSpecializationSymbol* classTemplateSpecialization, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span, 
    const boost::uuids::uuid& moduleId)
{
    try
    {
        BindClassTemplateSpecialization(classTemplateSpecialization, containerScope, span, moduleId);
        for (MemberFunctionSymbol* memberFunction : classTemplateSpecialization->MemberFunctions())
        {
            if (!Instantiate(memberFunction, containerScope, currentFunction, span, moduleId))
            {
                throw Exception("instantation of member function '" + ToUtf8(memberFunction->Name()) + "' failed", memberFunction->GetSpan(), memberFunction->SourceModuleId());
            }
        }
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("full instantiation request for class template specialization '" + ToUtf8(classTemplateSpecialization->FullName()) + "' failed. Reason: " + ex.Message(), 
            span, moduleId, references);
    }
}

} } // namespace cmajor::binder
