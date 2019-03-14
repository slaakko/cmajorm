// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/BoundNamespace.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/ExpressionBinder.hpp>
#include <cmajor/binder/Access.hpp>
#include <cmajor/binder/OperationRepository.hpp>
#include <cmajor/binder/Evaluator.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/binder/BoundEnum.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/parser/TypeExpr.hpp>
#include <cmajor/ast/Literal.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/ast/BasicType.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::unicode;
using namespace cmajor::parser;

bool IsAlwaysTrue(Node* node, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    std::unique_ptr<Value> value = Evaluate(node, boundCompileUnit.GetSymbolTable().GetTypeByName(U"bool"), containerScope, boundCompileUnit, true, nullptr, node->GetSpan());
    if (value)
    {
        if (value->GetValueType() == ValueType::boolValue)
        {
            BoolValue* boolValue = static_cast<BoolValue*>(value.get());
            return boolValue->GetValue() == true;
        }
    }
    return false;
}

bool TerminatesFunction(StatementNode* statement, bool inForEverLoop, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit)
{
    switch (statement->GetNodeType())
    {
        case NodeType::compoundStatementNode:
        {
            CompoundStatementNode* compoundStatement = static_cast<CompoundStatementNode*>(statement);
            int n = compoundStatement->Statements().Count();
            for (int i = 0; i < n; ++i)
            {
                StatementNode* statement = compoundStatement->Statements()[i];
                if (TerminatesFunction(statement, inForEverLoop, containerScope, boundCompileUnit)) return true;
            }
            break;
        }
        case NodeType::ifStatementNode:
        {
            IfStatementNode* ifStatement = static_cast<IfStatementNode*>(statement);
            if (inForEverLoop || ifStatement->ElseS())
            {
                if (TerminatesFunction(ifStatement->ThenS(), inForEverLoop, containerScope, boundCompileUnit) &&
                    inForEverLoop || (ifStatement->ElseS() && TerminatesFunction(ifStatement->ElseS(), inForEverLoop, containerScope, boundCompileUnit)))
                {
                    return true;
                }
            }
            break;
        }
        case NodeType::whileStatementNode:
        {
            WhileStatementNode* whileStatement = static_cast<WhileStatementNode*>(statement);
            if (IsAlwaysTrue(whileStatement->Condition(), boundCompileUnit, containerScope))
            {
                if (TerminatesFunction(whileStatement->Statement(), true, containerScope, boundCompileUnit)) return true;
            }
            break;
        }
        case NodeType::doStatementNode:
        {
            DoStatementNode* doStatement = static_cast<DoStatementNode*>(statement);
            if (IsAlwaysTrue(doStatement->Condition(), boundCompileUnit, containerScope))
            {
                if (TerminatesFunction(doStatement->Statement(), true, containerScope, boundCompileUnit)) return true;
            }
            break;
        }
        case NodeType::forStatementNode:
        {
            ForStatementNode* forStatement = static_cast<ForStatementNode*>(statement);
            if (!forStatement->Condition() || IsAlwaysTrue(forStatement->Condition(), boundCompileUnit, containerScope))
            {
                if (TerminatesFunction(forStatement->ActionS(), true, containerScope, boundCompileUnit)) return true;
            }
            break;
        }
        default:
        {
            if (statement->IsFunctionTerminatingNode())
            {
                return true;
            }
            break;
        }
    }
    return false;
}

bool TerminatesCase(StatementNode* statementNode)
{
    if (statementNode->GetNodeType() == NodeType::ifStatementNode)
    {
        IfStatementNode* ifStatementNode = static_cast<IfStatementNode*>(statementNode);
        if (ifStatementNode->ElseS())
        {
            if (TerminatesCase(ifStatementNode->ThenS()) && TerminatesCase(ifStatementNode->ElseS()))
            {
                return true;
            }
        }
    }
    else if (statementNode->GetNodeType() == NodeType::compoundStatementNode)
    {
        CompoundStatementNode* compoundStatement = static_cast<CompoundStatementNode*>(statementNode);
        int n = compoundStatement->Statements().Count();
        for (int i = 0; i < n; ++i)
        {
            StatementNode* statementNode = compoundStatement->Statements()[i];
            if (TerminatesCase(statementNode))
            {
                return true;
            }
        }
    }
    else
    {
        return statementNode->IsCaseTerminatingNode();
    }
    return false;
}

bool TerminatesDefault(StatementNode* statementNode)
{
    if (statementNode->GetNodeType() == NodeType::ifStatementNode)
    {
        IfStatementNode* ifStatementNode = static_cast<IfStatementNode*>(statementNode);
        if (ifStatementNode->ElseS())
        {
            if (TerminatesDefault(ifStatementNode->ThenS()) && TerminatesDefault(ifStatementNode->ElseS()))
            {
                return true;
            }
        }
    }
    else if (statementNode->GetNodeType() == NodeType::compoundStatementNode)
    {
        CompoundStatementNode* compoundStatement = static_cast<CompoundStatementNode*>(statementNode);
        int n = compoundStatement->Statements().Count();
        for (int i = 0; i < n; ++i)
        {
            StatementNode* statementNode = compoundStatement->Statements()[i];
            if (TerminatesDefault(statementNode))
            {
                return true;
            }
        }
    }
    else
    {
        return statementNode->IsDefaultTerminatingNode();
    }
    return false;
}

void CheckFunctionReturnPaths(FunctionSymbol* functionSymbol, CompoundStatementNode* bodyNode, const Span& span, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit);

void CheckFunctionReturnPaths(FunctionSymbol* functionSymbol, FunctionNode& functionNode, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit)
{
    CheckFunctionReturnPaths(functionSymbol, functionNode.Body(), functionNode.GetSpan(), containerScope, boundCompileUnit);
}

void CheckFunctionReturnPaths(FunctionSymbol* functionSymbol, CompoundStatementNode* bodyNode, const Span& span, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit)
{
    TypeSymbol* returnType = functionSymbol->ReturnType();
    if (!returnType || returnType->GetSymbolType() == SymbolType::voidTypeSymbol) return;
    if (functionSymbol->IsExternal()) return;
    if (functionSymbol->IsAbstract()) return;
    CompoundStatementNode* body = bodyNode;
    if (body)
    {
        int n = body->Statements().Count();
        for (int i = 0; i < n; ++i)
        {
            StatementNode* statement = body->Statements()[i];
            if (TerminatesFunction(statement, false, containerScope, boundCompileUnit)) return;
        }
        throw Exception(&boundCompileUnit.GetModule(), "not all control paths terminate in return or throw statement", span);
    }
}

StatementBinder::StatementBinder(BoundCompileUnit& boundCompileUnit_) :  
    boundCompileUnit(boundCompileUnit_), symbolTable(boundCompileUnit.GetSymbolTable()), module(&boundCompileUnit.GetModule()), containerScope(nullptr), statement(), compoundLevel(0), insideCatch(false),
    currentClass(nullptr), currentFunction(nullptr), currentStaticConstructorSymbol(nullptr), currentStaticConstructorNode(nullptr), currentConstructorSymbol(nullptr),
    currentConstructorNode(nullptr), currentDestructorSymbol(nullptr), currentDestructorNode(nullptr), currentMemberFunctionSymbol(nullptr), currentMemberFunctionNode(nullptr), 
    switchConditionType(nullptr), currentCaseValueMap(nullptr), currentGotoCaseStatements(nullptr), currentGotoDefaultStatements(nullptr), postfix(false), compilingThrow(false), 
    compilingReleaseExceptionStatement(false)
{
}

void StatementBinder::Visit(CompileUnitNode& compileUnitNode)
{
    compileUnitNode.GlobalNs()->Accept(*this);
}

void StatementBinder::Visit(NamespaceNode& namespaceNode)
{
    std::unique_ptr<BoundNamespace> ns(new BoundNamespace(module, namespaceNode));
    boundCompileUnit.PushNamespace(ns.get());
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&namespaceNode);
    containerScope = symbol->GetContainerScope();
    int n = namespaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = namespaceNode.Members()[i];
        member->Accept(*this);
    }
    containerScope = prevContainerScope;
    boundCompileUnit.PopNamespace();
    boundCompileUnit.AddBoundNode(std::unique_ptr<BoundNode>(ns.release()));
}

void StatementBinder::Visit(EnumTypeNode& enumTypeNode)
{
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&enumTypeNode);
    Assert(symbol->GetSymbolType() == SymbolType::enumTypeSymbol, "enum type symbols expected");
    EnumTypeSymbol* enumTypeSymbol = static_cast<EnumTypeSymbol*>(symbol);
    std::unique_ptr<BoundEnumTypeDefinition> boundEnum(new BoundEnumTypeDefinition(module, enumTypeSymbol));
    boundCompileUnit.AddBoundNode(std::move(boundEnum));
}

void StatementBinder::Visit(ClassNode& classNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&classNode);
    Assert(symbol->GetSymbolType() == SymbolType::classTypeSymbol || symbol->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type symbol expected");
    ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
    if (classTypeSymbol->IsClassTemplate())
    {
        return;
    }
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundClass> boundClass(new BoundClass(module, classTypeSymbol));
    BoundClass* prevClass = currentClass;
    currentClass = boundClass.get();
    int n = classNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* classMember = classNode.Members()[i];
        classMember->Accept(*this);
    }
    boundCompileUnit.GetAttributeBinder()->GenerateImplementation(classNode.GetAttributes(), symbol, this);
    boundCompileUnit.AddBoundNode(std::move(boundClass));
    DestructorSymbol* destructorSymbol = classTypeSymbol->Destructor();
    if (destructorSymbol && destructorSymbol->IsProject() && destructorSymbol->IsGeneratedFunction() && !GetGlobalFlag(GlobalFlags::info))
    {
        GenerateDestructorImplementation(currentClass, destructorSymbol, boundCompileUnit, containerScope, currentFunction, classNode.GetSpan());
    }
    currentClass = prevClass;
    containerScope = prevContainerScope;
}

void StatementBinder::Visit(FunctionNode& functionNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&functionNode);
    Assert(symbol->GetSymbolType() == SymbolType::functionSymbol, "function symbol expected");
    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
    if (functionSymbol->IsFunctionTemplate())
    {
        return;
    }
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, functionSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (functionNode.Body())
    {
        compoundLevel = 0;
        functionNode.Body()->Accept(*this);
        BoundStatement* boundStatement = statement.release();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        CheckFunctionReturnPaths(functionSymbol, functionNode, containerScope, boundCompileUnit);
        boundCompileUnit.AddBoundNode(std::move(boundFunction));
    }
    boundCompileUnit.GetAttributeBinder()->GenerateImplementation(functionNode.GetAttributes(), symbol, this);
    currentFunction = prevFunction;
    containerScope = prevContainerScope;
}

void StatementBinder::Visit(StaticConstructorNode& staticConstructorNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&staticConstructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::staticConstructorSymbol , "static constructor symbol expected");
    StaticConstructorSymbol* staticConstructorSymbol = static_cast<StaticConstructorSymbol*>(symbol);
    StaticConstructorSymbol* prevStaticConstructorSymbol = currentStaticConstructorSymbol;
    currentStaticConstructorSymbol = staticConstructorSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, staticConstructorSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (staticConstructorNode.Body())
    {
        StaticConstructorNode* prevStaticConstructorNode = currentStaticConstructorNode;
        currentStaticConstructorNode = &staticConstructorNode;
        compoundLevel = 0;
        staticConstructorNode.Body()->Accept(*this);
        currentStaticConstructorNode = prevStaticConstructorNode;
        BoundStatement* boundStatement = statement.release();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        CheckFunctionReturnPaths(staticConstructorSymbol, staticConstructorNode, containerScope, boundCompileUnit);
        currentClass->AddMember(std::move(boundFunction));
    }
    boundCompileUnit.GetAttributeBinder()->GenerateImplementation(staticConstructorNode.GetAttributes(), symbol, this);
    currentFunction = prevFunction;
    containerScope = prevContainerScope;
    currentStaticConstructorSymbol = prevStaticConstructorSymbol;
}

void StatementBinder::Visit(ConstructorNode& constructorNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&constructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::constructorSymbol, "constructor symbol expected");
    ConstructorSymbol* constructorSymbol = static_cast<ConstructorSymbol*>(symbol);
    ConstructorSymbol* prevConstructorSymbol = currentConstructorSymbol;
    currentConstructorSymbol = constructorSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, constructorSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (constructorNode.Body())
    {
        ConstructorNode* prevConstructorNode = currentConstructorNode;
        currentConstructorNode = &constructorNode;
        compoundLevel = 0;
        constructorNode.Body()->Accept(*this);
        currentConstructorNode = prevConstructorNode;
        BoundStatement* boundStatement = statement.release();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
    }
    else if (constructorSymbol->IsDefault())
    {
        ConstructorNode* prevConstructorNode = currentConstructorNode;
        currentConstructorNode = &constructorNode;
        std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(module, constructorNode.GetSpan()));
        GenerateClassInitialization(currentConstructorSymbol, currentConstructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, true, constructorNode.GetSpan());
        currentConstructorNode = prevConstructorNode;
        boundFunction->SetBody(std::move(boundCompoundStatement));
    }
    if (boundFunction->Body())
    {
        CheckFunctionReturnPaths(constructorSymbol, constructorNode, containerScope, boundCompileUnit);
        currentClass->AddMember(std::move(boundFunction));
    }
    boundCompileUnit.GetAttributeBinder()->GenerateImplementation(constructorNode.GetAttributes(), symbol, this);
    currentFunction = prevFunction;
    containerScope = prevContainerScope;
    currentConstructorSymbol = prevConstructorSymbol;
}

void StatementBinder::Visit(DestructorNode& destructorNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&destructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::destructorSymbol, "destructor symbol expected");
    DestructorSymbol* destructorSymbol = static_cast<DestructorSymbol*>(symbol);
    DestructorSymbol* prevDestructorSymbol = currentDestructorSymbol;
    currentDestructorSymbol = destructorSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, destructorSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (destructorNode.Body())
    {
        DestructorNode* prevDestructorNode = currentDestructorNode;
        currentDestructorNode = &destructorNode;
        compoundLevel = 0;
        destructorNode.Body()->Accept(*this);
        currentDestructorNode = prevDestructorNode;
        BoundStatement* boundStatement = statement.release();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
    }
    else if (destructorSymbol->IsDefault())
    {
        DestructorNode* prevDestructorNode = currentDestructorNode;
        currentDestructorNode = &destructorNode;
        std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(module, destructorNode.GetSpan()));
        GenerateClassTermination(currentDestructorSymbol, currentDestructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, currentDestructorNode->GetSpan());
        currentDestructorNode = prevDestructorNode;
        boundFunction->SetBody(std::move(boundCompoundStatement));
    }
    if (boundFunction->Body())
    {
        CheckFunctionReturnPaths(destructorSymbol, destructorNode, containerScope, boundCompileUnit);
        currentClass->AddMember(std::move(boundFunction));
    }
    boundCompileUnit.GetAttributeBinder()->GenerateImplementation(destructorNode.GetAttributes(), symbol, this);
    currentFunction = prevFunction;
    containerScope = prevContainerScope;
    currentDestructorSymbol = prevDestructorSymbol;
}

void StatementBinder::Visit(MemberFunctionNode& memberFunctionNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&memberFunctionNode);
    Assert(symbol->GetSymbolType() == SymbolType::memberFunctionSymbol, "member function symbol expected");
    MemberFunctionSymbol* memberFunctionSymbol = static_cast<MemberFunctionSymbol*>(symbol);
    MemberFunctionSymbol* prevMemberFunctionSymbol = currentMemberFunctionSymbol;
    currentMemberFunctionSymbol = memberFunctionSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, memberFunctionSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (memberFunctionNode.Body())
    {
        MemberFunctionNode* prevMemberFunctionNode = currentMemberFunctionNode;
        currentMemberFunctionNode = &memberFunctionNode;
        compoundLevel = 0;
        memberFunctionNode.Body()->Accept(*this);
        currentMemberFunctionNode = prevMemberFunctionNode;
        BoundStatement* boundStatement = statement.release();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
    }
    else if (memberFunctionSymbol->IsDefault())
    {
        Assert(memberFunctionSymbol->GroupName() == U"operator=", "operator= expected");
        MemberFunctionNode* prevMemberFunctionNode = currentMemberFunctionNode;
        currentMemberFunctionNode = &memberFunctionNode;
        std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(module, memberFunctionNode.GetSpan()));
        GenerateClassAssignment(currentMemberFunctionSymbol, currentMemberFunctionNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, true, memberFunctionNode.GetSpan());
        currentMemberFunctionNode = prevMemberFunctionNode;
        boundFunction->SetBody(std::move(boundCompoundStatement));
    }
    if (boundFunction->Body())
    {
        CheckFunctionReturnPaths(memberFunctionSymbol, memberFunctionNode, containerScope, boundCompileUnit);
        currentClass->AddMember(std::move(boundFunction));
    }
    boundCompileUnit.GetAttributeBinder()->GenerateImplementation(memberFunctionNode.GetAttributes(), symbol, this);
    currentFunction = prevFunction;
    containerScope = prevContainerScope;
    currentMemberFunctionSymbol = prevMemberFunctionSymbol;
}

void StatementBinder::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&conversionFunctionNode);
    Assert(symbol->GetSymbolType() == SymbolType::conversionFunctionSymbol, "conversion function symbol expected");
    ConversionFunctionSymbol* conversionFunctionSymbol = static_cast<ConversionFunctionSymbol*>(symbol);
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, conversionFunctionSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (conversionFunctionNode.Body())
    {
        compoundLevel = 0;
        conversionFunctionNode.Body()->Accept(*this);
        BoundStatement* boundStatement = statement.release();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        CheckFunctionReturnPaths(conversionFunctionSymbol, conversionFunctionNode, containerScope, boundCompileUnit);
        currentClass->AddMember(std::move(boundFunction));
    }
    boundCompileUnit.GetAttributeBinder()->GenerateImplementation(conversionFunctionNode.GetAttributes(), symbol, this);
    currentFunction = prevFunction;
    containerScope = prevContainerScope;
}

void StatementBinder::Visit(CompoundStatementNode& compoundStatementNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = symbolTable.GetSymbol(&compoundStatementNode);
    Assert(symbol->GetSymbolType() == SymbolType::declarationBlock, "declaration block expected");
    DeclarationBlock* declarationBlock = static_cast<DeclarationBlock*>(symbol);
    containerScope = declarationBlock->GetContainerScope();
    std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(module, compoundStatementNode.GetSpan(), compoundStatementNode.EndBraceSpan()));
    if (compoundLevel == 0)
    {
        if (GetGlobalFlag(GlobalFlags::profile))
        {
            bool profile = true;
            if (currentFunction->GetFunctionSymbol()->IsProgramMain())
            {
                profile = false;
            }
            else if (currentClass && currentClass->GetClassTypeSymbol()->FullName() == U"System.Runtime.FunctionProfiler")
            {
                profile = false;
            }
            if (profile)
            {
                boost::uuids::uuid functionId = currentFunction->GetFunctionSymbol()->FunctionId();
                symbolTable.MapProfiledFunction(functionId, currentFunction->GetFunctionSymbol()->FullName());
                ConstructionStatementNode constructFunctionProfiler(compoundStatementNode.GetSpan(), new IdentifierNode(compoundStatementNode.GetSpan(), U"System.Runtime.FunctionProfiler"),
                    new IdentifierNode(compoundStatementNode.GetSpan(), U"@functionProfiler"));
                constructFunctionProfiler.AddArgument(new UuidLiteralNode(compoundStatementNode.GetSpan(), functionId)); 
                symbolTable.SetCurrentFunctionSymbol(currentFunction->GetFunctionSymbol());
                symbolTable.BeginContainer(containerScope->Container());
                SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
                constructFunctionProfiler.Accept(symbolCreatorVisitor);
                symbolTable.EndContainer();
                TypeBinder typeBinder(boundCompileUnit);
                typeBinder.SetContainerScope(containerScope);
                typeBinder.SetCurrentFunctionSymbol(currentFunction->GetFunctionSymbol());
                constructFunctionProfiler.Accept(typeBinder);
                constructFunctionProfiler.Accept(*this);
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(ReleaseStatement()));
            }
        }
        if (currentStaticConstructorSymbol && currentStaticConstructorNode)
        {
            GenerateStaticClassInitialization(currentStaticConstructorSymbol, currentStaticConstructorNode, boundCompileUnit, boundCompoundStatement.get(), currentFunction, containerScope, this, 
                boundCompoundStatement->GetSpan());
        }
        else if (currentConstructorSymbol && currentConstructorNode)
        {
            GenerateClassInitialization(currentConstructorSymbol, currentConstructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, false,
                boundCompoundStatement->GetSpan());
        }
        else if (currentMemberFunctionSymbol && currentMemberFunctionSymbol->GroupName() == U"operator=" && currentMemberFunctionNode)
        {
            GenerateClassAssignment(currentMemberFunctionSymbol, currentMemberFunctionNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, false,
                boundCompoundStatement->GetSpan());
        }
        else if (currentMemberFunctionSymbol && currentMemberFunctionSymbol->IsStatic() && currentMemberFunctionNode)
        {
            if (currentClass->GetClassTypeSymbol()->StaticConstructor())
            {
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::unique_ptr<BoundExpression>(
                    new BoundFunctionCall(module, boundCompoundStatement->GetSpan(), currentClass->GetClassTypeSymbol()->StaticConstructor())))));
            }
        }
    }
    ++compoundLevel;
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = compoundStatementNode.Statements()[i];
        statementNode->Accept(*this);
        boundCompoundStatement->AddStatement(std::move(statement));
    }
    --compoundLevel;
    if (compoundLevel == 0 && currentDestructorSymbol && currentDestructorNode)
    {
        GenerateClassTermination(currentDestructorSymbol, currentDestructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, boundCompoundStatement->EndSpan());
    }
    AddStatement(boundCompoundStatement.release());
    if (compoundStatementNode.Label())
    {
        statement->SetLabel(compoundStatementNode.Label()->Label());
    }
    containerScope = prevContainerScope;
}

void StatementBinder::Visit(ReturnStatementNode& returnStatementNode) 
{
    if (returnStatementNode.Expression())
    {
        if (currentFunction->GetFunctionSymbol()->ReturnsClassInterfaceOrClassDelegateByValue())
        {
            std::vector<FunctionScopeLookup> classReturnLookups;
            classReturnLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            classReturnLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, currentFunction->GetFunctionSymbol()->ReturnType()->ClassInterfaceEnumDelegateOrNsScope()));
            classReturnLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> classReturnArgs;
            classReturnArgs.push_back(std::unique_ptr<BoundExpression>(new BoundParameter(module, returnStatementNode.GetSpan(), currentFunction->GetFunctionSymbol()->ReturnParam())));
            TypeSymbol* returnType = currentFunction->GetFunctionSymbol()->ReturnType();
            bool returnClassDelegateType = returnType->GetSymbolType() == SymbolType::classDelegateTypeSymbol;
            std::unique_ptr<BoundExpression> expression = BindExpression(returnStatementNode.Expression(), boundCompileUnit, currentFunction, containerScope, this, false, returnClassDelegateType, 
                returnClassDelegateType);
            bool exceptionCapture = false;
            if (insideCatch && expression->ContainsExceptionCapture())
            {
                exceptionCapture = true;
            }
            if (expression->GetBoundNodeType() == BoundNodeType::boundLocalVariable)
            {
                std::vector<FunctionScopeLookup> rvalueLookups;
                rvalueLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                rvalueLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> rvalueArguments;
                if (returnClassDelegateType && expression->GetType()->BaseType()->GetSymbolType() == SymbolType::functionGroupTypeSymbol)
                {
                    TypeSymbol* exprType = expression->GetType();
                    ArgumentMatch argumentMatch;
                    expression.reset(new BoundConversion(module, std::move(expression), boundCompileUnit.GetConversion(exprType, returnType, containerScope, currentFunction, returnStatementNode.GetSpan(), argumentMatch)));
                }
                rvalueArguments.push_back(std::move(expression));
                std::unique_ptr<BoundExpression> rvalueExpr = ResolveOverload(U"System.Rvalue", containerScope, rvalueLookups, rvalueArguments, boundCompileUnit, currentFunction,
                    returnStatementNode.GetSpan());
                expression = std::move(rvalueExpr);
            }
            classReturnArgs.push_back(std::move(expression));
            std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, classReturnLookups, classReturnArgs, boundCompileUnit, currentFunction,
                returnStatementNode.GetSpan());
            std::unique_ptr<BoundStatement> constructStatement(new BoundExpressionStatement(module, std::move(constructorCall)));
            AddStatement(constructStatement.release());
            std::unique_ptr<BoundFunctionCall> returnFunctionCall;
            std::unique_ptr<BoundStatement> returnStatement(new BoundReturnStatement(module, std::move(returnFunctionCall), returnStatementNode.GetSpan()));
            if (exceptionCapture)
            {
                AddReleaseExceptionStatement(returnStatementNode.GetSpan());
            }
            AddStatement(returnStatement.release());
        }
        else
        {
            TypeSymbol* returnType = currentFunction->GetFunctionSymbol()->ReturnType();
            bool returnDelegateType = false;
            bool returnClassDelegateType = false;
            bool exceptionCapture = false;
            if (returnType)
            {
                returnDelegateType = returnType->GetSymbolType() == SymbolType::delegateTypeSymbol;
                returnClassDelegateType = returnType->GetSymbolType() == SymbolType::classDelegateTypeSymbol;
            }
            if (returnType && returnType->GetSymbolType() != SymbolType::voidTypeSymbol)
            {
                std::vector<std::unique_ptr<BoundExpression>> returnTypeArgs;
                BoundTypeExpression* boundTypeExpression = new BoundTypeExpression(module, returnStatementNode.GetSpan(), returnType);
                returnTypeArgs.push_back(std::unique_ptr<BoundTypeExpression>(boundTypeExpression));
                std::vector<FunctionScopeLookup> functionScopeLookups;
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, returnType->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::unique_ptr<BoundFunctionCall> returnFunctionCall = ResolveOverload(U"@return", containerScope, functionScopeLookups, returnTypeArgs, boundCompileUnit, currentFunction,
                    returnStatementNode.GetSpan());
                std::unique_ptr<BoundExpression> expression = BindExpression(returnStatementNode.Expression(), boundCompileUnit, currentFunction, containerScope, this, false,
                    returnDelegateType || returnClassDelegateType, returnClassDelegateType);
                if (insideCatch && expression->ContainsExceptionCapture())
                {
                    exceptionCapture = true;
                }
                std::vector<std::unique_ptr<BoundExpression>> returnValueArguments;
                returnValueArguments.push_back(std::move(expression));
                FunctionMatch functionMatch(returnFunctionCall->GetFunctionSymbol());
                bool conversionFound = FindConversions(boundCompileUnit, returnFunctionCall->GetFunctionSymbol(), returnValueArguments, functionMatch, ConversionType::implicit_, 
                    containerScope, currentFunction, returnStatementNode.GetSpan());
                if (conversionFound)
                {
                    Assert(!functionMatch.argumentMatches.empty(), "argument match expected");
                    ArgumentMatch argumentMatch = functionMatch.argumentMatches[0];
                    if (argumentMatch.preReferenceConversionFlags != OperationFlags::none)
                    {
                        if (argumentMatch.preReferenceConversionFlags == OperationFlags::addr)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->AddLvalueReference(returnStatementNode.GetSpan());
                            BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(module, std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(addressOfExpression);
                        }
                        else if (argumentMatch.preReferenceConversionFlags == OperationFlags::deref)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->RemoveReference(returnStatementNode.GetSpan());
                            BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(module, std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(dereferenceExpression);
                        }
                    }
                    FunctionSymbol* conversionFun = argumentMatch.conversionFun;
                    if (conversionFun)
                    {
                        if (conversionFun->GetSymbolType() == SymbolType::constructorSymbol)
                        {
                            BoundFunctionCall* constructorCall = new BoundFunctionCall(module, returnStatementNode.GetSpan(), conversionFun);
                            LocalVariableSymbol* temporary = currentFunction->GetFunctionSymbol()->CreateTemporary(conversionFun->ConversionTargetType(), returnStatementNode.GetSpan());
                            constructorCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(new BoundLocalVariable(module, returnStatementNode.GetSpan(), temporary)),
                                conversionFun->ConversionTargetType()->AddPointer(returnStatementNode.GetSpan()))));
                            constructorCall->AddArgument(std::move(returnValueArguments[0]));
                            BoundConstructAndReturnTemporaryExpression* conversion = new BoundConstructAndReturnTemporaryExpression(module, std::unique_ptr<BoundExpression>(constructorCall),
                                std::unique_ptr<BoundExpression>(new BoundLocalVariable(module, returnStatementNode.GetSpan(), temporary)));
                            returnValueArguments[0].reset(conversion);
                        }
                        else
                        {
                            BoundConversion* boundConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(returnValueArguments[0].release()), conversionFun);
                            returnValueArguments[0].reset(boundConversion);
                        }
                    }
                    if (argumentMatch.postReferenceConversionFlags != OperationFlags::none)
                    {
                        if (argumentMatch.postReferenceConversionFlags == OperationFlags::addr)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->AddLvalueReference(returnStatementNode.GetSpan());
                            BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(module, std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(addressOfExpression);
                        }
                        else if (argumentMatch.postReferenceConversionFlags == OperationFlags::deref)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->RemoveReference(returnStatementNode.GetSpan());
                            BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(module, std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(dereferenceExpression);
                        }
                    }
                    returnFunctionCall->SetArguments(std::move(returnValueArguments));
                }
                else
                {
                    throw Exception(module, "no implicit conversion from '" + ToUtf8(returnValueArguments[0]->GetType()->FullName()) + "' to '" + ToUtf8(returnType->FullName()) + "' exists",
                        returnStatementNode.GetSpan(), currentFunction->GetFunctionSymbol()->GetSpan());
                }
                CheckAccess(currentFunction->GetFunctionSymbol(), returnFunctionCall->GetFunctionSymbol());
                if (exceptionCapture)
                {
                    AddReleaseExceptionStatement(returnStatementNode.GetSpan());
                }
                AddStatement(new BoundReturnStatement(module, std::move(returnFunctionCall), returnStatementNode.GetSpan()));
            }
            else
            {
                if (returnType)
                {
                    throw Exception(module, "void function cannot return a value", returnStatementNode.Expression()->GetSpan(), currentFunction->GetFunctionSymbol()->GetSpan());
                }
                else
                {
                    throw Exception(module, "constructor or assignment function cannot return a value", returnStatementNode.Expression()->GetSpan(), currentFunction->GetFunctionSymbol()->GetSpan());
                }
            }
        }
    }
    else
    {
        TypeSymbol* returnType = currentFunction->GetFunctionSymbol()->ReturnType();
        if (!returnType || returnType->GetSymbolType() == SymbolType::voidTypeSymbol)
        {
            std::unique_ptr<BoundFunctionCall> returnFunctionCall;
            AddStatement(new BoundReturnStatement(module, std::move(returnFunctionCall), returnStatementNode.GetSpan()));
        }
        else
        {
            throw Exception(module, "nonvoid function must return a value", returnStatementNode.GetSpan(), currentFunction->GetFunctionSymbol()->GetSpan());
        }
    }
    if (returnStatementNode.Label())
    {
        statement->SetLabel(returnStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(IfStatementNode& ifStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> condition = BindExpression(ifStatementNode.Condition(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(ifStatementNode.GetSpan())))
    {
        throw Exception(module, "condition of an if statement must be a Boolean expression", ifStatementNode.Condition()->GetSpan());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(module, std::move(condition), baseType));
    }
    std::unique_ptr<BoundStatement> s;
    if (statement)
    {
        s = std::move(statement);
    }
    ifStatementNode.ThenS()->Accept(*this);
    BoundStatement* thenS = statement.release();
    BoundStatement* elseS = nullptr;
    if (ifStatementNode.ElseS())
    {
        ifStatementNode.ElseS()->Accept(*this);
        elseS = statement.release();
    }
    if (s)
    {
        AddStatement(s.release());
    }
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(ifStatementNode.GetSpan());
    }
    AddStatement(new BoundIfStatement(module, ifStatementNode.GetSpan(), std::move(condition), std::unique_ptr<BoundStatement>(thenS), std::unique_ptr<BoundStatement>(elseS)));
    if (ifStatementNode.Label())
    {
        statement->SetLabel(ifStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(WhileStatementNode& whileStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> condition = BindExpression(whileStatementNode.Condition(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(whileStatementNode.GetSpan())))
    {
        throw Exception(module, "condition of a while statement must be a Boolean expression", whileStatementNode.Condition()->GetSpan());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(module, std::move(condition), baseType));
    }
    std::unique_ptr<BoundStatement> s;
    if (statement)
    {
        s = std::move(statement);
    }
    whileStatementNode.Statement()->Accept(*this);
    BoundStatement* stmt = statement.release();
    if (s)
    {
        AddStatement(s.release());
    }
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(whileStatementNode.GetSpan());
    }
    AddStatement(new BoundWhileStatement(module, whileStatementNode.GetSpan(), std::move(condition), std::unique_ptr<BoundStatement>(stmt)));
    if (whileStatementNode.Label())
    {
        statement->SetLabel(whileStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(DoStatementNode& doStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> condition = BindExpression(doStatementNode.Condition(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(doStatementNode.GetSpan())))
    {
        throw Exception(module, "condition of a do statement must be a Boolean expression", doStatementNode.Condition()->GetSpan());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(module, std::move(condition), baseType));
    }
    std::unique_ptr<BoundStatement> s;
    if (statement)
    {
        s = std::move(statement);
    }
    doStatementNode.Statement()->Accept(*this);
    BoundStatement* stmt = statement.release();
    if (s)
    {
        AddStatement(s.release());
    }
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(doStatementNode.GetSpan());
    }
    AddStatement(new BoundDoStatement(module, doStatementNode.GetSpan(), std::unique_ptr<BoundStatement>(stmt), std::move(condition)));
    if (doStatementNode.Label())
    {
        statement->SetLabel(doStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(ForStatementNode& forStatementNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&forStatementNode);
    Assert(symbol->GetSymbolType() == SymbolType::declarationBlock, "declaration block expected");
    DeclarationBlock* declarationBlock = static_cast<DeclarationBlock*>(symbol);
    containerScope = declarationBlock->GetContainerScope();
    std::unique_ptr<BoundExpression> condition;
    if (forStatementNode.Condition())
    {
        condition = BindExpression(forStatementNode.Condition(), boundCompileUnit, currentFunction, containerScope, this);
    }
    else
    {
        BooleanLiteralNode trueNode(forStatementNode.GetSpan(), true);
        condition = BindExpression(&trueNode, boundCompileUnit, currentFunction, containerScope, this);
    }
    bool exceptionCapture = false;
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(forStatementNode.GetSpan())))
    {
        throw Exception(module, "condition of a for statement must be a Boolean expression", forStatementNode.Condition()->GetSpan());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(module, std::move(condition), baseType));
    }
    std::unique_ptr<BoundStatement> s;
    if (statement)
    {
        s = std::move(statement);
    }
    forStatementNode.InitS()->Accept(*this);
    BoundStatement* initS = statement.release();
    forStatementNode.LoopS()->Accept(*this);
    BoundStatement* loopS = statement.release();
    forStatementNode.ActionS()->Accept(*this);
    BoundStatement* actionS = statement.release();
    if (s)
    {
        AddStatement(s.release());
    }
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(forStatementNode.GetSpan());
    }
    AddStatement(new BoundForStatement(module, forStatementNode.GetSpan(), std::unique_ptr<BoundStatement>(initS), std::move(condition), std::unique_ptr<BoundStatement>(loopS),
        std::unique_ptr<BoundStatement>(actionS)));
    if (forStatementNode.Label())
    {
        statement->SetLabel(forStatementNode.Label()->Label());
    }
    containerScope = prevContainerScope;
}

void StatementBinder::Visit(BreakStatementNode& breakStatementNode)
{
    const Node* parent = breakStatementNode.Parent();
    const StatementNode* parentStatement = nullptr;
    if (parent && parent->IsStatementNode())
    {
        parentStatement = static_cast<const StatementNode*>(parent);
    }
    while (parentStatement && !parentStatement->IsBreakEnclosingStatementNode())
    {
        parent = parentStatement->Parent();
        if (parent && parent->IsStatementNode())
        {
            parentStatement = static_cast<const StatementNode*>(parent);
        }
        else
        {
            parentStatement = nullptr;
        }
    }
    if (!parentStatement)
    {
        throw Exception(module, "break statement must be enclosed in a while, do, for or switch statement", breakStatementNode.GetSpan());
    }
    AddStatement(new BoundBreakStatement(module, breakStatementNode.GetSpan()));
    if (breakStatementNode.Label())
    {
        statement->SetLabel(breakStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(ContinueStatementNode& continueStatementNode)
{
    const Node* parent = continueStatementNode.Parent();
    const StatementNode* parentStatement = nullptr;
    if (parent && parent->IsStatementNode())
    {
        parentStatement = static_cast<const StatementNode*>(parent);
    }
    while (parentStatement && !parentStatement->IsContinueEnclosingStatementNode())
    {
        parent = parentStatement->Parent();
        if (parent && parent->IsStatementNode())
        {
            parentStatement = static_cast<const StatementNode*>(parent);
        }
        else
        {
            parentStatement = nullptr;
        }
    }
    if (!parentStatement)
    {
        throw Exception(module, "continue statement must be enclosed in a while, do or for statement", continueStatementNode.GetSpan());
    }
    AddStatement(new BoundContinueStatement(module, continueStatementNode.GetSpan()));
    if (continueStatementNode.Label())
    {
        statement->SetLabel(continueStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(GotoStatementNode& gotoStatementNode)
{
    currentFunction->SetHasGotos();
    boundCompileUnit.SetHasGotos();
    AddStatement(new BoundGotoStatement(module, gotoStatementNode.GetSpan(), gotoStatementNode.Target()));
    if (gotoStatementNode.Label())
    {
        statement->SetLabel(gotoStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(ConstructionStatementNode& constructionStatementNode)
{
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&constructionStatementNode);
    Assert(symbol->GetSymbolType() == SymbolType::localVariableSymbol, "local variable symbol expected");
    LocalVariableSymbol* localVariableSymbol = static_cast<LocalVariableSymbol*>(symbol);
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    BoundExpression* localVariable = new BoundLocalVariable(module, constructionStatementNode.GetSpan(), localVariableSymbol);
    arguments.push_back(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(module,
        std::unique_ptr<BoundExpression>(localVariable), localVariable->GetType()->AddPointer(constructionStatementNode.GetSpan()))));
    bool constructDelegateOrClassDelegateType = 
        localVariableSymbol->GetType()->GetSymbolType() == SymbolType::delegateTypeSymbol|| 
        localVariableSymbol->GetType()->GetSymbolType() == SymbolType::classDelegateTypeSymbol;
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, localVariableSymbol->GetType()->ClassInterfaceEnumDelegateOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    bool exceptionCapture = false;
    int n = constructionStatementNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* argumentNode = constructionStatementNode.Arguments()[i];
        std::unique_ptr<BoundExpression> argument = BindExpression(argumentNode, boundCompileUnit, currentFunction, containerScope, this, false, constructDelegateOrClassDelegateType);
        if (insideCatch && argument->ContainsExceptionCapture())
        {
            exceptionCapture = true;
        }
        arguments.push_back(std::move(argument));
    }
    std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, functionScopeLookups, arguments, boundCompileUnit, currentFunction, 
        constructionStatementNode.GetSpan());
    CheckAccess(currentFunction->GetFunctionSymbol(), constructorCall->GetFunctionSymbol());
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(constructionStatementNode.GetSpan());
    }
    AddStatement(new BoundConstructionStatement(module, std::move(constructorCall)));
    if (constructionStatementNode.Label())
    {
        statement->SetLabel(constructionStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(DeleteStatementNode& deleteStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> ptr = BindExpression(deleteStatementNode.Expression(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && ptr->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (GetBackEnd() == BackEnd::llvm)
    {
        if (GetConfig() == "debug")
        {
            std::vector<FunctionScopeLookup> lookups;
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> arguments;
            arguments.push_back(std::move(std::unique_ptr<BoundExpression>(ptr->Clone())));
            std::unique_ptr<BoundFunctionCall> disposeCall = ResolveOverload(U"RtDispose", containerScope, lookups, arguments, boundCompileUnit, currentFunction, deleteStatementNode.GetSpan());
            CheckAccess(currentFunction->GetFunctionSymbol(), disposeCall->GetFunctionSymbol());
            AddStatement(new BoundExpressionStatement(module, std::move(disposeCall)));
        }
    }
    std::unique_ptr<BoundExpression> memFreePtr;
    TypeSymbol* baseType = ptr->GetType()->BaseType();
    if (baseType->HasNontrivialDestructor())
    {
        Assert(baseType->GetSymbolType() == SymbolType::classTypeSymbol || baseType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type expected");
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(baseType);
        std::vector<FunctionScopeLookup> lookups;
        lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
        lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->ClassInterfaceOrNsScope()));
        lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
        std::vector<std::unique_ptr<BoundExpression>> arguments;
        arguments.push_back(std::move(ptr));
        std::unique_ptr<BoundFunctionCall> destructorCall = ResolveOverload(U"@destructor", containerScope, lookups, arguments, boundCompileUnit, currentFunction, deleteStatementNode.GetSpan());
        CheckAccess(currentFunction->GetFunctionSymbol(), destructorCall->GetFunctionSymbol());
        if (destructorCall->GetFunctionSymbol()->IsVirtualAbstractOrOverride())
        {
            destructorCall->SetFlag(BoundExpressionFlags::virtualCall);
        }
        AddStatement(new BoundExpressionStatement(module, std::move(destructorCall)));
        memFreePtr = BindExpression(deleteStatementNode.Expression(), boundCompileUnit, currentFunction, containerScope, this);
        if (insideCatch && memFreePtr->ContainsExceptionCapture())
        {
            exceptionCapture = true;
        }
    }
    else
    {
        memFreePtr = std::move(ptr);
    }
    std::vector<FunctionScopeLookup> lookups;
    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    arguments.push_back(std::move(memFreePtr));
    const char32_t* memFreeFunctionName = U"";
    if (GetBackEnd() == BackEnd::llvm)
    {
        memFreeFunctionName = U"RtMemFree";
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        memFreeFunctionName = U"MemFree";
    }
    std::unique_ptr<BoundFunctionCall> memFreeCall = ResolveOverload(memFreeFunctionName, containerScope, lookups, arguments, boundCompileUnit, currentFunction, deleteStatementNode.GetSpan());
    CheckAccess(currentFunction->GetFunctionSymbol(), memFreeCall->GetFunctionSymbol());
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(deleteStatementNode.GetSpan());
    }
    AddStatement(new BoundExpressionStatement(module, std::move(memFreeCall)));
    if (deleteStatementNode.Label())
    {
        statement->SetLabel(deleteStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(DestroyStatementNode& destroyStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> ptr = BindExpression(destroyStatementNode.Expression(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && ptr->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!ptr->GetType()->IsPointerType())
    {
        throw Exception(module, "destroy statement needs pointer type operand", destroyStatementNode.GetSpan());
    }
    TypeSymbol* pointeeType = ptr->GetType()->RemovePointer(destroyStatementNode.GetSpan());
    if (pointeeType->HasNontrivialDestructor())
    {
        Assert(pointeeType->GetSymbolType() == SymbolType::classTypeSymbol || pointeeType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type expected");
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(pointeeType);
        std::vector<FunctionScopeLookup> lookups;
        lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
        lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->ClassInterfaceOrNsScope()));
        lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
        std::vector<std::unique_ptr<BoundExpression>> arguments;
        arguments.push_back(std::move(ptr));
        std::unique_ptr<BoundFunctionCall> destructorCall = ResolveOverload(U"@destructor", containerScope, lookups, arguments, boundCompileUnit, currentFunction, destroyStatementNode.GetSpan());
        CheckAccess(currentFunction->GetFunctionSymbol(), destructorCall->GetFunctionSymbol());
        if (destructorCall->GetFunctionSymbol()->IsVirtualAbstractOrOverride())
        {
            destructorCall->SetFlag(BoundExpressionFlags::virtualCall);
        }
        if (exceptionCapture)
        {
            AddReleaseExceptionStatement(destroyStatementNode.GetSpan());
        }
        AddStatement(new BoundExpressionStatement(module, std::move(destructorCall)));
    }
    else
    {
        AddStatement(new BoundEmptyStatement(module, destroyStatementNode.GetSpan()));
    }
}

void StatementBinder::Visit(AssignmentStatementNode& assignmentStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> target = BindExpression(assignmentStatementNode.TargetExpr(), boundCompileUnit, currentFunction, containerScope, this, true);
    if (insideCatch && target->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    TypeSymbol* targetPlainType = target->GetType()->PlainType(assignmentStatementNode.GetSpan());
    if (targetPlainType->IsClassTypeSymbol() && target->GetType()->IsReferenceType())
    {
        TypeSymbol* type = target->GetType()->RemoveReference(assignmentStatementNode.GetSpan())->AddPointer(assignmentStatementNode.GetSpan());
        target.reset(new BoundReferenceToPointerExpression(module, std::unique_ptr<BoundExpression>(target.release()), type));
    }
    else
    {
        target.reset(new BoundAddressOfExpression(module, std::move(target), target->GetType()->AddPointer(assignmentStatementNode.GetSpan())));
    }
    TypeSymbol* targetType = target->GetType()->BaseType();
    bool assignDelegateOrClassDelegateType = targetType->GetSymbolType() == SymbolType::delegateTypeSymbol || targetType->GetSymbolType() == SymbolType::classDelegateTypeSymbol;
    std::unique_ptr<BoundExpression> source = BindExpression(assignmentStatementNode.SourceExpr(), boundCompileUnit, currentFunction, containerScope, this, false, assignDelegateOrClassDelegateType);
    if (insideCatch && source->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    arguments.push_back(std::move(target));
    arguments.push_back(std::move(source));
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, targetType->ClassInterfaceEnumDelegateOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    std::unique_ptr<BoundFunctionCall> assignmentCall = ResolveOverload(U"operator=", containerScope, functionScopeLookups, arguments, boundCompileUnit, currentFunction, 
        assignmentStatementNode.GetSpan());
    CheckAccess(currentFunction->GetFunctionSymbol(), assignmentCall->GetFunctionSymbol());
    currentFunction->MoveTemporaryDestructorCallsTo(*assignmentCall);
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(assignmentStatementNode.GetSpan());
    }
    AddStatement(new BoundAssignmentStatement(module, std::move(assignmentCall)));
    if (assignmentStatementNode.Label())
    {
        statement->SetLabel(assignmentStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(ExpressionStatementNode& expressionStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> expression = BindExpression(expressionStatementNode.Expression(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && expression->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(expressionStatementNode.GetSpan());
    }
    AddStatement(new BoundExpressionStatement(module, std::move(expression)));
    if (expressionStatementNode.Label())
    {
        statement->SetLabel(expressionStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(EmptyStatementNode& emptyStatementNode)
{
    AddStatement(new BoundEmptyStatement(module, emptyStatementNode.GetSpan()));
    if (emptyStatementNode.Label())
    {
        statement->SetLabel(emptyStatementNode.Label()->Label());
    }
}

TypeExpr* typeExprGrammar = nullptr;

void StatementBinder::Visit(RangeForStatementNode& rangeForStatementNode)
{
    const Span& span = rangeForStatementNode.GetSpan();
    std::unique_ptr<BoundExpression> container = BindExpression(rangeForStatementNode.Container(), boundCompileUnit, currentFunction, containerScope, this);
    TypeSymbol* plainContainerType = container->GetType()->PlainType(span);
    std::u32string plainContainerTypeFullName = plainContainerType->FullName();
    if (!typeExprGrammar)
    {
        typeExprGrammar = TypeExpr::Create();
    }
    ParsingContext parsingContext;
    std::unique_ptr<Node> containerTypeNode(typeExprGrammar->Parse(&plainContainerTypeFullName[0], &plainContainerTypeFullName[plainContainerTypeFullName.length()], 0, "", &parsingContext));
    std::unique_ptr<IdentifierNode> iteratorTypeNode = nullptr;
    if (container->GetType()->IsConstType())
    {
        iteratorTypeNode.reset(new IdentifierNode(span, U"ConstIterator"));
    }
    else
    {
        iteratorTypeNode.reset(new IdentifierNode(span, U"Iterator"));
    }
    CloneContext cloneContext;
    std::unique_ptr<CompoundStatementNode> compoundStatementNode(new CompoundStatementNode(span));
    if (rangeForStatementNode.Action()->GetNodeType() == NodeType::compoundStatementNode)
    {
        CompoundStatementNode* action = static_cast<CompoundStatementNode*>(rangeForStatementNode.Action());
        compoundStatementNode->SetEndBraceSpan(action->EndBraceSpan());
    }
    else
    {
        compoundStatementNode->SetEndBraceSpan(span);
    }
    compoundStatementNode->SetParent(rangeForStatementNode.Parent());
    ConstructionStatementNode* constructEndIteratorStatement = new ConstructionStatementNode(span, 
        new DotNode(span, containerTypeNode->Clone(cloneContext), static_cast<IdentifierNode*>(iteratorTypeNode->Clone(cloneContext))), new IdentifierNode(span, U"@end"));
    if (container->GetType()->IsConstType())
    {
        constructEndIteratorStatement->AddArgument(new InvokeNode(span, new DotNode(span, rangeForStatementNode.Container()->Clone(cloneContext), new IdentifierNode(span, U"CEnd"))));
    }
    else
    {
        constructEndIteratorStatement->AddArgument(new InvokeNode(span, new DotNode(span, rangeForStatementNode.Container()->Clone(cloneContext), new IdentifierNode(span, U"End"))));
    }
    compoundStatementNode->AddStatement(constructEndIteratorStatement);
    ConstructionStatementNode* constructIteratorStatement = new ConstructionStatementNode(span, 
        new DotNode(span, containerTypeNode->Clone(cloneContext), static_cast<IdentifierNode*>(iteratorTypeNode->Clone(cloneContext))), new IdentifierNode(span, U"@it"));
    if (container->GetType()->IsConstType())
    {
        constructIteratorStatement->AddArgument(new InvokeNode(span, new DotNode(span, rangeForStatementNode.Container()->Clone(cloneContext), new IdentifierNode(span, U"CBegin"))));
    }
    else
    {
        constructIteratorStatement->AddArgument(new InvokeNode(span, new DotNode(span, rangeForStatementNode.Container()->Clone(cloneContext), new IdentifierNode(span, U"Begin"))));
    }
    Node* itNotEndCond = new NotEqualNode(span, new IdentifierNode(span, U"@it"), new IdentifierNode(span, U"@end"));
    StatementNode* incrementItStatement = new ExpressionStatementNode(span, new PrefixIncrementNode(span, new IdentifierNode(span, U"@it")));
    CompoundStatementNode* actionStatement = new CompoundStatementNode(span);
    if (rangeForStatementNode.Action()->GetNodeType() == NodeType::compoundStatementNode)
    {
        CompoundStatementNode* action = static_cast<CompoundStatementNode*>(rangeForStatementNode.Action());
        actionStatement->SetEndBraceSpan(action->EndBraceSpan());
    }
    else
    {
        actionStatement->SetEndBraceSpan(span);
    }
    ConstructionStatementNode* constructLoopVarStatement = new ConstructionStatementNode(span,
        rangeForStatementNode.TypeExpr()->Clone(cloneContext), static_cast<IdentifierNode*>(rangeForStatementNode.Id()->Clone(cloneContext)));
    constructLoopVarStatement->AddArgument(new DerefNode(span, new IdentifierNode(span, U"@it")));
    actionStatement->AddStatement(constructLoopVarStatement);
    actionStatement->AddStatement(static_cast<StatementNode*>(rangeForStatementNode.Action()->Clone(cloneContext)));
    ForStatementNode* forStatement = new ForStatementNode(span, constructIteratorStatement, itNotEndCond, incrementItStatement, actionStatement);
    compoundStatementNode->AddStatement(forStatement);

    symbolTable.BeginContainer(containerScope->Container());
    SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
    compoundStatementNode->Accept(symbolCreatorVisitor);
    symbolTable.EndContainer();
    TypeBinder typeBinder(boundCompileUnit);
    typeBinder.SetContainerScope(containerScope);
    typeBinder.SetCurrentFunctionSymbol(currentFunction->GetFunctionSymbol());
    compoundStatementNode->Accept(typeBinder);
    compoundStatementNode->Accept(*this);
}

void StatementBinder::Visit(SwitchStatementNode& switchStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> condition = BindExpression(switchStatementNode.Condition(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    TypeSymbol* conditionType = condition->GetType();
    if (conditionType->IsSwitchConditionType())
    {
        if (conditionType->GetSymbolType() == SymbolType::enumTypeSymbol)
        {
            EnumTypeSymbol* enumType = static_cast<EnumTypeSymbol*>(conditionType);
            conditionType = enumType->UnderlyingType();
        }
        TypeSymbol* prevSwitchConditionType = switchConditionType;
        switchConditionType = conditionType;
        std::unordered_map<IntegralValue, CaseStatementNode*, IntegralValueHash>* prevCaseValueMap = currentCaseValueMap;
        std::unordered_map<IntegralValue, CaseStatementNode*, IntegralValueHash> caseValueMap;
        currentCaseValueMap = &caseValueMap;
        std::vector<std::pair<BoundGotoCaseStatement*, IntegralValue>>* prevGotoCaseStatements = currentGotoCaseStatements;
        std::vector<std::pair<BoundGotoCaseStatement*, IntegralValue>> gotoCaseStatements;
        currentGotoCaseStatements = &gotoCaseStatements;
        std::vector<BoundGotoDefaultStatement*>* prevGotoDefaultStatements = currentGotoDefaultStatements;
        std::vector<BoundGotoDefaultStatement*> gotoDefaultStatements;
        currentGotoDefaultStatements = &gotoDefaultStatements;
        std::unique_ptr<BoundSwitchStatement> boundSwitchStatement(new BoundSwitchStatement(module, switchStatementNode.GetSpan(), std::move(condition)));
        int n = switchStatementNode.Cases().Count();
        for (int i = 0; i < n; ++i)
        {
            CaseStatementNode* caseS = switchStatementNode.Cases()[i];
            caseS->Accept(*this);
            Assert(statement->GetBoundNodeType() == BoundNodeType::boundCaseStatement, "case statement expected");
            boundSwitchStatement->AddCaseStatement(std::unique_ptr<BoundCaseStatement>(static_cast<BoundCaseStatement*>(statement.release())));
        }
        if (switchStatementNode.Default())
        {
            switchStatementNode.Default()->Accept(*this);
            Assert(statement->GetBoundNodeType() == BoundNodeType::boundDefaultStatement, "default statement expected");
            boundSwitchStatement->SetDefaultStatement(std::unique_ptr<BoundDefaultStatement>(static_cast<BoundDefaultStatement*>(statement.release())));
        }
        for (const std::pair<BoundGotoCaseStatement*, IntegralValue>& p : gotoCaseStatements)
        {
            BoundGotoCaseStatement* gotoCaseStatement = p.first;
            IntegralValue integralCaseValue = p.second;
            auto it = caseValueMap.find(integralCaseValue);
            if (it == caseValueMap.cend())
            {
                throw Exception(module, "case not found", gotoCaseStatement->GetSpan());
            }
        }
        if (!gotoDefaultStatements.empty() && !switchStatementNode.Default())
        {
            throw Exception(module, "switch does not have a default statement", gotoDefaultStatements.front()->GetSpan());
        }
        currentGotoCaseStatements = prevGotoCaseStatements;
        currentGotoDefaultStatements = prevGotoDefaultStatements;
        currentCaseValueMap = prevCaseValueMap;
        if (exceptionCapture)
        {
            AddReleaseExceptionStatement(switchStatementNode.GetSpan());
        }
        AddStatement(boundSwitchStatement.release());
        if (switchStatementNode.Label())
        {
            statement->SetLabel(switchStatementNode.Label()->Label());
        }
        switchConditionType = prevSwitchConditionType;
    }
    else
    {
        throw Exception(module, "switch statement condition must be of integer, character, enumerated or Boolean type", switchStatementNode.Condition()->GetSpan());
    }
}

void StatementBinder::Visit(CaseStatementNode& caseStatementNode)
{
    std::unique_ptr<BoundCaseStatement> boundCaseStatement(new BoundCaseStatement(module, caseStatementNode.GetSpan()));
    bool terminated = false;
    int n = caseStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = caseStatementNode.Statements()[i];
        if (TerminatesCase(statementNode))
        {
            terminated = true;
        }
        statementNode->Accept(*this);
        boundCaseStatement->AddStatement(std::move(statement));
    }
    if (!terminated)
    {
        throw Exception(module, "case must end in break, continue, return, throw, goto, goto case or goto default statement", caseStatementNode.GetSpan());
    }
    int ne = caseStatementNode.CaseExprs().Count();
    for (int i = 0; i < ne; ++i)
    {
        Node* caseExprNode = caseStatementNode.CaseExprs()[i];
        std::unique_ptr<Value> caseValue = Evaluate(caseExprNode, switchConditionType, containerScope, boundCompileUnit, false, currentFunction, caseExprNode->GetSpan());
        IntegralValue integralCaseValue(caseValue.get());
        Assert(currentCaseValueMap, "current case value map not set");
        auto it = currentCaseValueMap->find(integralCaseValue);
        if (it != currentCaseValueMap->cend())
        {
            throw Exception(module, "case value already used", caseExprNode->GetSpan());
        }
        (*currentCaseValueMap)[integralCaseValue] = &caseStatementNode;
        boundCaseStatement->AddCaseValue(std::move(caseValue));
    }
    AddStatement(boundCaseStatement.release());
    if (caseStatementNode.Label())
    {
        statement->SetLabel(caseStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(DefaultStatementNode& defaultStatementNode)
{
    std::unique_ptr<BoundDefaultStatement> boundDefaultStatement(new BoundDefaultStatement(module, defaultStatementNode.GetSpan()));
    bool terminated = false;
    int n = defaultStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = defaultStatementNode.Statements()[i];
        if (TerminatesDefault(statementNode))
        {
            terminated = true;
        }
        statementNode->Accept(*this);
        boundDefaultStatement->AddStatement(std::move(statement));
    }
    if (!terminated)
    {
        throw Exception(module, "default must end in break, continue, return, throw, goto, or goto case statement", defaultStatementNode.GetSpan());
    }
    AddStatement(boundDefaultStatement.release());
    if (defaultStatementNode.Label())
    {
        statement->SetLabel(defaultStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(GotoCaseStatementNode& gotoCaseStatementNode)
{
    const Node* parent = gotoCaseStatementNode.Parent();
    while (parent && parent->GetNodeType() != NodeType::caseStatementNode && parent->GetNodeType() != NodeType::defaultStatementNode)
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Exception(module, "goto case statement must be enclosed in a case or default statement", gotoCaseStatementNode.GetSpan());
    }
    Node* caseExprNode = gotoCaseStatementNode.CaseExpr();
    std::unique_ptr<Value> caseValue = Evaluate(caseExprNode, switchConditionType, containerScope, boundCompileUnit, false, currentFunction, gotoCaseStatementNode.GetSpan());
    Value* caseValuePtr = caseValue.get();
    BoundGotoCaseStatement* boundGotoCaseStatement = new BoundGotoCaseStatement(module, gotoCaseStatementNode.GetSpan(), std::move(caseValue));
    Assert(currentGotoCaseStatements, "current goto case statement vector not set");
    currentGotoCaseStatements->push_back(std::make_pair(boundGotoCaseStatement, IntegralValue(caseValuePtr)));
    AddStatement(boundGotoCaseStatement);
    if (gotoCaseStatementNode.Label())
    {
        statement->SetLabel(gotoCaseStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    const Node* parent = gotoDefaultStatementNode.Parent();
    while (parent && parent->GetNodeType() != NodeType::caseStatementNode)
    {
        parent = parent->Parent();
    }
    if (!parent)
    {
        throw Exception(module, "goto default statement must be enclosed in a case statement", gotoDefaultStatementNode.GetSpan());
    }
    BoundGotoDefaultStatement* boundGotoDefaultStatement = new BoundGotoDefaultStatement(module, gotoDefaultStatementNode.GetSpan());
    Assert(currentGotoDefaultStatements, "current goto default statement vector not set");
    currentGotoDefaultStatements->push_back(boundGotoDefaultStatement);
    AddStatement(boundGotoDefaultStatement);
    if (gotoDefaultStatementNode.Label())
    {
        statement->SetLabel(gotoDefaultStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(ThrowStatementNode& throwStatementNode)
{
    bool prevCompilingThrow = compilingThrow;
    compilingThrow = true;
    if (currentFunction->GetFunctionSymbol()->DontThrow() && !currentFunction->GetFunctionSymbol()->HasTry())
    {
        throw Exception(module, "a nothrow function cannot contain a throw statement unless it handles exceptions", throwStatementNode.GetSpan(), currentFunction->GetFunctionSymbol()->GetSpan());
    }
    Span span = throwStatementNode.GetSpan();
    Node* exceptionExprNode = throwStatementNode.Expression();
    if (exceptionExprNode)
    {
        std::unique_ptr<BoundExpression> boundExceptionExpr = BindExpression(exceptionExprNode, boundCompileUnit, currentFunction, containerScope, this);
        if (boundExceptionExpr->GetType()->PlainType(span)->IsClassTypeSymbol())
        {
            ClassTypeSymbol* exceptionClassType = static_cast<ClassTypeSymbol*>(boundExceptionExpr->GetType()->PlainType(span));
            IdentifierNode systemExceptionNode(throwStatementNode.GetSpan(), U"System.Exception");
            TypeSymbol* systemExceptionType = ResolveType(&systemExceptionNode, boundCompileUnit, containerScope);
            Assert(systemExceptionType->IsClassTypeSymbol(), "System.Exception not of class type");
            ClassTypeSymbol* systemExceptionClassType = static_cast<ClassTypeSymbol*>(systemExceptionType);
            if (exceptionClassType == systemExceptionClassType || exceptionClassType->HasBaseClass(systemExceptionClassType))
            {
                NewNode* newNode = new NewNode(span, new IdentifierNode(span, exceptionClassType->FullName()));
                CloneContext cloneContext;
                newNode->AddArgument(throwStatementNode.Expression()->Clone(cloneContext));
                InvokeNode invokeNode(span, new IdentifierNode(span, U"RtThrowException"));
                invokeNode.AddArgument(newNode);
                invokeNode.AddArgument(new UuidLiteralNode(span, exceptionClassType->TypeId())); 
                std::unique_ptr<BoundExpression> throwCallExpr = BindExpression(&invokeNode, boundCompileUnit, currentFunction, containerScope, this);
                AddStatement(new BoundThrowStatement(module, span, std::move(throwCallExpr)));
            }
            else
            {
                throw Exception(module, "exception class must be derived from System.Exception class", throwStatementNode.GetSpan());
            }
        }
        else
        {
            throw Exception(module, "exception not of class type", throwStatementNode.GetSpan());
        }
    }
    else
    {
        if (insideCatch)
        {
            InvokeNode invokeNode(span, new DotNode(span, new IdentifierNode(span, U"@exPtr"), new IdentifierNode(span, U"Release")));
            std::unique_ptr<BoundExpression> releaseCall = BindExpression(&invokeNode, boundCompileUnit, currentFunction, containerScope, this);
            AddStatement(new BoundRethrowStatement(module, span, std::move(releaseCall)));
        }
        else
        {
            throw Exception(module, "rethrow must occur inside a catch clause", throwStatementNode.GetSpan());
        }
    }
    if (throwStatementNode.Label())
    {
        statement->SetLabel(throwStatementNode.Label()->Label());
    }
    compilingThrow = prevCompilingThrow;
}

void StatementBinder::Visit(TryStatementNode& tryStatementNode)
{
    BoundTryStatement* boundTryStatement = new BoundTryStatement(module, tryStatementNode.GetSpan());
    tryStatementNode.TryBlock()->Accept(*this);
    boundTryStatement->SetTryBlock(std::move(statement));
    int n = tryStatementNode.Catches().Count();
    for (int i = 0; i < n; ++i)
    {
        CatchNode* catchNode = tryStatementNode.Catches()[i];
        catchNode->Accept(*this);
        BoundStatement* s = statement.release();
        Assert(s->GetBoundNodeType() == BoundNodeType::boundCatchStatement, "catch statement expected");
        BoundCatchStatement* catchStatement = static_cast<BoundCatchStatement*>(s);
        boundTryStatement->AddCatch(std::unique_ptr<BoundCatchStatement>(catchStatement));
    }
    AddStatement(boundTryStatement);
    if (tryStatementNode.Label())
    {
        statement->SetLabel(tryStatementNode.Label()->Label());
    }
}

void StatementBinder::Visit(CatchNode& catchNode)
{
    bool prevInsideCatch = insideCatch;
    insideCatch = true;
    Span span = catchNode.GetSpan();
    std::unique_ptr<BoundCatchStatement> boundCatchStatement(new BoundCatchStatement(module, catchNode.GetSpan()));
    TypeSymbol* catchedType = ResolveType(catchNode.TypeExpr(), boundCompileUnit, containerScope);
    boundCatchStatement->SetCatchedType(catchedType);
    boundCatchStatement->SetCatchedTypeUuidId(boundCompileUnit.Install(catchedType->BaseType()->TypeId()));
    LocalVariableSymbol* catchVar = nullptr;
    if (catchNode.Id())
    {
        Symbol* symbol = symbolTable.GetSymbol(catchNode.Id());
        Assert(symbol->GetSymbolType() == SymbolType::localVariableSymbol, "local variable symbol expected");
        catchVar = static_cast<LocalVariableSymbol*>(symbol);
        boundCatchStatement->SetCatchVar(catchVar);
        currentFunction->GetFunctionSymbol()->AddLocalVariable(catchVar);
    }
    CompoundStatementNode handlerBlock(span);
    handlerBlock.SetEndBraceSpan(catchNode.CatchBlock()->EndBraceSpan());
    handlerBlock.SetParent(catchNode.Parent());
    ConstructionStatementNode* getExceptionAddr = new ConstructionStatementNode(span, new PointerNode(span, new IdentifierNode(span, U"void")), new IdentifierNode(span, U"@exceptionAddr"));
    getExceptionAddr->AddArgument(new InvokeNode(span, new IdentifierNode(span, U"RtGetException")));
    handlerBlock.AddStatement(getExceptionAddr);
    PointerNode exceptionPtrTypeNode(span, new IdentifierNode(span, catchedType->BaseType()->FullName()));
    CloneContext cloneContext;
    ConstructionStatementNode* constructExceptionPtr = new ConstructionStatementNode(span, exceptionPtrTypeNode.Clone(cloneContext), new IdentifierNode(span, U"@exceptionPtr"));
    constructExceptionPtr->AddArgument(new CastNode(span, exceptionPtrTypeNode.Clone(cloneContext), new IdentifierNode(span, U"@exceptionAddr")));
    handlerBlock.AddStatement(constructExceptionPtr);
    TemplateIdNode* uniquePtrNode = new TemplateIdNode(span, new IdentifierNode(span, U"UniquePtr"));
    uniquePtrNode->AddTemplateArgument(new IdentifierNode(span, catchedType->BaseType()->FullName()));
    ConstructionStatementNode* constructUniquePtrException = new ConstructionStatementNode(span, uniquePtrNode, new IdentifierNode(span, U"@exPtr"));
    constructUniquePtrException->AddArgument(new IdentifierNode(span, U"@exceptionPtr"));
    handlerBlock.AddStatement(constructUniquePtrException);
    if (catchVar)
    {
        ConstructionStatementNode* setExceptionVar = new ConstructionStatementNode(span, catchNode.TypeExpr()->Clone(cloneContext), static_cast<IdentifierNode*>(catchNode.Id()->Clone(cloneContext)));
        setExceptionVar->AddArgument(new DerefNode(span, new IdentifierNode(span, U"@exPtr")));
        handlerBlock.AddStatement(setExceptionVar);
    }
    handlerBlock.AddStatement(static_cast<StatementNode*>(catchNode.CatchBlock()->Clone(cloneContext)));
    symbolTable.BeginContainer(containerScope->Container());
    SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
    handlerBlock.Accept(symbolCreatorVisitor);
    symbolTable.EndContainer();
    TypeBinder typeBinder(boundCompileUnit);
    typeBinder.SetContainerScope(containerScope);
    typeBinder.SetCurrentFunctionSymbol(currentFunction->GetFunctionSymbol());
    handlerBlock.Accept(typeBinder);
    handlerBlock.Accept(*this);
    boundCatchStatement->SetCatchBlock(std::move(statement));
    AddStatement(boundCatchStatement.release());
    insideCatch = prevInsideCatch;
}

void StatementBinder::Visit(AssertStatementNode& assertStatementNode)
{
    if (!module)
    {
        throw std::runtime_error("module not set");
    }
    bool unitTesting = GetGlobalFlag(GlobalFlags::unitTest) && InUnitTest();
    bool unitTestAssertion = false;
    if (unitTesting)
    {
        FunctionSymbol* functionSymbol = currentFunction->GetFunctionSymbol();
        Node* node = symbolTable.GetNodeNoThrow(functionSymbol);
        if (node)
        {
            if (node->GetNodeType() == NodeType::functionNode)
            {
                FunctionNode* functionNode = static_cast<FunctionNode*>(node);
                if ((functionNode->GetSpecifiers() & Specifiers::unit_test_) != Specifiers::none)
                {
                    unitTestAssertion = true;
                }
            }
        }
    }
    if (unitTestAssertion)
    {
        int32_t assertionLineNumber = assertStatementNode.GetSpan().LineNumber();
        int32_t assertionIndex = GetNextUnitTestAssertionNumber();
        AddAssertionLineNumber(assertionLineNumber);
        InvokeNode* invokeSetUnitTestAssertionResult = new InvokeNode(assertStatementNode.GetSpan(), new IdentifierNode(assertStatementNode.GetSpan(), U"RtSetUnitTestAssertionResult"));
        invokeSetUnitTestAssertionResult->AddArgument(new IntLiteralNode(assertStatementNode.GetSpan(), assertionIndex));
        CloneContext cloneContext;
        invokeSetUnitTestAssertionResult->AddArgument(assertStatementNode.AssertExpr()->Clone(cloneContext));
        invokeSetUnitTestAssertionResult->AddArgument(new IntLiteralNode(assertStatementNode.GetSpan(), assertionLineNumber));
        ExpressionStatementNode setUnitTestAssertionResult(assertStatementNode.GetSpan(), invokeSetUnitTestAssertionResult);
        symbolTable.BeginContainer(containerScope->Container());
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        setUnitTestAssertionResult.Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(boundCompileUnit);
        typeBinder.SetContainerScope(containerScope);
        typeBinder.SetCurrentFunctionSymbol(currentFunction->GetFunctionSymbol());
        setUnitTestAssertionResult.Accept(typeBinder);
        setUnitTestAssertionResult.Accept(*this);
    }
    else
    {
        if (GetGlobalFlag(GlobalFlags::release))
        {
            AddStatement(new BoundEmptyStatement(module, assertStatementNode.GetSpan()));
        }
        else
        {
            std::vector<FunctionScopeLookup> lookups;
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, symbolTable.GlobalNs().GetContainerScope()));
            std::vector<std::unique_ptr<BoundExpression>> arguments;
            TypeSymbol* constCharPtrType = symbolTable.GetTypeByName(U"char")->AddConst(assertStatementNode.GetSpan())->AddPointer(assertStatementNode.GetSpan());
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(module, std::unique_ptr<Value>(new StringValue(assertStatementNode.GetSpan(),
                boundCompileUnit.Install(assertStatementNode.AssertExpr()->ToString()))), constCharPtrType)));
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(module, std::unique_ptr<Value>(new StringValue(assertStatementNode.GetSpan(),
                boundCompileUnit.Install(ToUtf8(currentFunction->GetFunctionSymbol()->FullName())))), constCharPtrType)));
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(module, std::unique_ptr<Value>(new StringValue(assertStatementNode.GetSpan(),
                boundCompileUnit.Install(module->GetFilePath(assertStatementNode.GetSpan().FileIndex())))), constCharPtrType)));
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(module, std::unique_ptr<Value>(new IntValue(assertStatementNode.GetSpan(),
                assertStatementNode.GetSpan().LineNumber())), symbolTable.GetTypeByName(U"int"))));
            std::unique_ptr<BoundExpression> assertExpression = BindExpression(assertStatementNode.AssertExpr(), boundCompileUnit, currentFunction, containerScope, this);
            const char32_t* failAssertionFunctionName = U"";
            if (GetBackEnd() == BackEnd::llvm)
            {
                failAssertionFunctionName = U"RtFailAssertion";
            }
            else if (GetBackEnd() == BackEnd::cmsx)
            {
                failAssertionFunctionName = U"System.FailAssertion";
            }
            std::unique_ptr<BoundStatement> ifStatement(new BoundIfStatement(module, assertStatementNode.GetSpan(), std::move(assertExpression),
                std::unique_ptr<BoundStatement>(new BoundEmptyStatement(module, assertStatementNode.GetSpan())),
                std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, ResolveOverload(failAssertionFunctionName, containerScope, lookups, arguments, boundCompileUnit, currentFunction,
                    assertStatementNode.GetSpan())))));
            AddStatement(ifStatement.release());
        }
    }
}

void StatementBinder::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    conditionalCompilationPartNode.Expr()->Accept(*this);
}

void StatementBinder::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    conditionalCompilationDisjunctionNode.Left()->Accept(*this);
    bool left = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationDisjunctionNode.Right()->Accept(*this);
    bool right = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(left || right);
}

void StatementBinder::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    conditionalCompilationConjunctionNode.Left()->Accept(*this);
    bool left = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationConjunctionNode.Right()->Accept(*this);
    bool right = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(left && right);
}

void StatementBinder::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    conditionalCompilationNotNode.Expr()->Accept(*this);
    bool operand = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(!operand);
}

void StatementBinder::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    bool defined = module->IsSymbolDefined(conditionalCompilationPrimaryNode.Symbol());
    conditionalCompilationStack.push(defined);
}

void StatementBinder::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    conditionalCompilationStatementNode.IfPart()->Accept(*this);
    bool defined = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    if (defined)
    {
        int n = conditionalCompilationStatementNode.IfPart()->Statements().Count();
        if (n > 0)
        {
            for (int i = 0; i < n; ++i)
            {
                StatementNode* statement = conditionalCompilationStatementNode.IfPart()->Statements()[i];
                statement->Accept(*this);
            }
        }
        else
        {
            AddStatement(new BoundEmptyStatement(module, conditionalCompilationStatementNode.GetSpan()));
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
                if (n > 0)
                {
                    for (int i = 0; i < n; ++i)
                    {
                        StatementNode* statement = elifPart->Statements()[i];
                        statement->Accept(*this);
                    }
                }
                else
                {
                    AddStatement(new BoundEmptyStatement(module, conditionalCompilationStatementNode.GetSpan()));
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
                if (n > 0)
                {
                    for (int i = 0; i < n; ++i)
                    {
                        StatementNode* statement = elsePart->Statements()[i];
                        statement->Accept(*this);
                    }
                }
                else
                {
                    AddStatement(new BoundEmptyStatement(module, conditionalCompilationStatementNode.GetSpan()));
                }
            }
            else
            {
                AddStatement(new BoundEmptyStatement(module, conditionalCompilationStatementNode.GetSpan()));
            }
        }
    }
}

void StatementBinder::CompileStatement(Node* statementNode, bool setPostfix)
{
    bool prevPostfix = postfix;
    postfix = setPostfix;
    statementNode->Accept(*this);
    postfix = prevPostfix;
}

void StatementBinder::SetCurrentConstructor(ConstructorSymbol* currentConstructorSymbol_, ConstructorNode* currentConstructorNode_)
{
    currentConstructorSymbol = currentConstructorSymbol_;
    currentConstructorNode = currentConstructorNode_;
}

void StatementBinder::SetCurrentDestructor(DestructorSymbol* currentDestructorSymbol_, DestructorNode* currentDestructorNode_)
{
    currentDestructorSymbol = currentDestructorSymbol_;
    currentDestructorNode = currentDestructorNode_;
}

void StatementBinder::SetCurrentMemberFunction(MemberFunctionSymbol* currentMemberFunctionSymbol_, MemberFunctionNode* currentMemberFunctionNode_)
{
    currentMemberFunctionSymbol = currentMemberFunctionSymbol_;
    currentMemberFunctionNode = currentMemberFunctionNode_;
}

void StatementBinder::AddReleaseExceptionStatement(const Span& span)
{
    if (insideCatch && !compilingReleaseExceptionStatement)
    {
        compilingReleaseExceptionStatement = true;
        InvokeNode* invokeNode(new InvokeNode(span, new DotNode(span, new IdentifierNode(span, U"@exPtr"), new IdentifierNode(span, U"Release"))));
        ExpressionStatementNode releaseExceptionStatement(span, invokeNode);
        CompileStatement(&releaseExceptionStatement, true);
        compilingReleaseExceptionStatement = false;
    }
}

void StatementBinder::AddStatement(BoundStatement* boundStatement)
{
    if (postfix)
    {
        boundStatement->SetPostfix();
    }
    if (statement)
    {
        if (statement->Postfix())
        {
            BoundSequenceStatement* sequenceStatement = new BoundSequenceStatement(module, boundStatement->GetSpan(), std::unique_ptr<BoundStatement>(boundStatement), std::move(statement));
            boundStatement = sequenceStatement;
        }
        else
        {
            BoundSequenceStatement* sequenceStatement = new BoundSequenceStatement(module, boundStatement->GetSpan(), std::move(statement), std::unique_ptr<BoundStatement>(boundStatement));
            boundStatement = sequenceStatement;
        }
        if (postfix)
        {
            boundStatement->SetPostfix();
        }
    }
    statement.reset(boundStatement);
}

} } // namespace cmajor::binder
