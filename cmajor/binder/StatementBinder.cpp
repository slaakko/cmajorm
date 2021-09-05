// =================================
// Copyright (c) 2021 Seppo Laakko
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
#include <sngcm/cmparser/TypeExpr.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Statement.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

bool IsAlwaysTrue(Node* node, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    std::unique_ptr<Value> value = Evaluate(node, boundCompileUnit.GetSymbolTable().GetTypeByName(U"bool"), containerScope, boundCompileUnit, true, nullptr, node->GetSpan(), node->ModuleId());
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

void CheckFunctionReturnPaths(FunctionSymbol* functionSymbol, CompoundStatementNode* bodyNode, const Span& span, const boost::uuids::uuid& moduleId, 
    ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit);

void CheckFunctionReturnPaths(FunctionSymbol* functionSymbol, FunctionNode& functionNode, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit)
{
    CheckFunctionReturnPaths(functionSymbol, functionNode.Body(), functionNode.GetSpan(), functionNode.ModuleId(), containerScope, boundCompileUnit);
}

void CheckFunctionReturnPaths(FunctionSymbol* functionSymbol, CompoundStatementNode* bodyNode, const Span& span, const boost::uuids::uuid& moduleId,
    ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit)
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
        throw Exception("not all control paths terminate in return or throw statement", span, moduleId);
    }
}

StatementBinder::StatementBinder(BoundCompileUnit& boundCompileUnit_) :  
    boundCompileUnit(boundCompileUnit_), symbolTable(boundCompileUnit.GetSymbolTable()), module(&boundCompileUnit.GetModule()), containerScope(nullptr), statement(), compoundLevel(0), insideCatch(false),
    currentClass(nullptr), currentFunction(nullptr), currentStaticConstructorSymbol(nullptr), currentStaticConstructorNode(nullptr), currentConstructorSymbol(nullptr),
    currentConstructorNode(nullptr), currentDestructorSymbol(nullptr), currentDestructorNode(nullptr), currentMemberFunctionSymbol(nullptr), currentMemberFunctionNode(nullptr), 
    switchConditionType(nullptr), currentCaseValueMap(nullptr), currentGotoCaseStatements(nullptr), currentGotoDefaultStatements(nullptr), postfix(false), compilingThrow(false), 
    compilingReleaseExceptionStatement(false), dontCheckDuplicateFunctionSymbols(false)
{
}

void StatementBinder::Visit(CompileUnitNode& compileUnitNode)
{
    if (compileUnitNode.IsSynthesizedUnit())
    {
        dontCheckDuplicateFunctionSymbols = true;
    }
    compileUnitNode.GlobalNs()->Accept(*this);
    dontCheckDuplicateFunctionSymbols = false;
    Symbol* symbol = symbolTable.GlobalNs().GetContainerScope()->Lookup(U"System.Runtime.AddCompileUnitFunction");
    if (symbol)
    {
        if (symbol->GetSymbolType() == SymbolType::functionGroupSymbol)
        {
            FunctionGroupSymbol* functionGroup = static_cast<FunctionGroupSymbol*>(symbol);
            FunctionSymbol* systemRuntimeAddCompileUnitFunctionSymbol = functionGroup->GetFunction();
            boundCompileUnit.SetSystemRuntimeAddCompileUnitFunctionSymbol(systemRuntimeAddCompileUnitFunctionSymbol);
        }
    }
    FunctionSymbol* initCompileUnitSymbol = boundCompileUnit.GetInitCompileUnitFunctionSymbol();
    if (initCompileUnitSymbol == nullptr)
    {
        boundCompileUnit.GenerateCompileUnitInitialization();
    }
}

struct NamespaceVisitor
{
    NamespaceVisitor(BoundCompileUnit* cu_, BoundNamespace* ns_) : cu(cu_), ns(ns_)
    {
        cu->PushNamespace(ns);
    }
    ~NamespaceVisitor()
    {
        cu->PopNamespace();
    }
    BoundCompileUnit* cu;
    BoundNamespace* ns;
};

void StatementBinder::Visit(NamespaceNode& namespaceNode)
{
    std::unique_ptr<BoundNamespace> ns(new BoundNamespace(namespaceNode));
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
    std::unique_ptr<BoundEnumTypeDefinition> boundEnum(new BoundEnumTypeDefinition(enumTypeSymbol));
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
    std::unique_ptr<BoundClass> boundClass(new BoundClass(classTypeSymbol));
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
    if (destructorSymbol && destructorSymbol->IsGeneratedFunction() && !GetGlobalFlag(GlobalFlags::info))
    {
        if (!boundCompileUnit.IsGeneratedDestructorInstantiated(destructorSymbol))
        {
            boundCompileUnit.SetGeneratedDestructorInstantiated(destructorSymbol);
            GenerateDestructorImplementation(currentClass, destructorSymbol, boundCompileUnit, containerScope, currentFunction, classNode.GetSpan(), classNode.ModuleId());
        }
    }
    currentClass = prevClass;
    containerScope = prevContainerScope;
}

void StatementBinder::Visit(MemberVariableNode& memberVariableNode)
{
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&memberVariableNode);
    Assert(symbol->GetSymbolType() == SymbolType::memberVariableSymbol, "member variable symbol expected");
    MemberVariableSymbol* memberVariableSymbol = static_cast<MemberVariableSymbol*>(symbol);
    TypeSymbol* typeSymbol = memberVariableSymbol->GetType();
    if (typeSymbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(typeSymbol);
        DestructorSymbol* destructorSymbol = classType->Destructor();
        if (destructorSymbol && destructorSymbol->IsGeneratedFunction() && !GetGlobalFlag(GlobalFlags::info))
        {
            if (!boundCompileUnit.IsGeneratedDestructorInstantiated(destructorSymbol))
            {
                boundCompileUnit.SetGeneratedDestructorInstantiated(destructorSymbol);
                std::unique_ptr<BoundClass> boundClass(new BoundClass(classType));
                GenerateDestructorImplementation(boundClass.get(), destructorSymbol, boundCompileUnit, containerScope, currentFunction, memberVariableNode.GetSpan(), memberVariableNode.ModuleId());
                boundCompileUnit.AddBoundNode(std::move(boundClass));
            }
        }
    }
}

void StatementBinder::Visit(FunctionNode& functionNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&functionNode);
    Assert(symbol->GetSymbolType() == SymbolType::functionSymbol, "function symbol expected");
    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
    if (!dontCheckDuplicateFunctionSymbols)
    {
        functionSymbol->FunctionGroup()->CheckDuplicateFunctionSymbols();
    }
    if (functionSymbol->IsFunctionTemplate())
    {
        return;
    }
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, functionSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (functionNode.Body())
    {
        GenerateEnterAndExitFunctionCode(currentFunction);
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

void StatementBinder::Visit(FullInstantiationRequestNode& fullInstantiationRequestNode)
{
    TypeSymbol* type = ResolveType(fullInstantiationRequestNode.TemplateId(), boundCompileUnit, containerScope);
    if (type->GetSymbolType() != SymbolType::classTemplateSpecializationSymbol)
    {
        throw Exception("full instantiation request expects subject template identifier to be a class template specialization", 
            fullInstantiationRequestNode.GetSpan(),
            fullInstantiationRequestNode.ModuleId());
    }
    ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
    LogMessage(module->LogStreamId(), "generating full instantation of '" + ToUtf8(specialization->FullName()) + "'");
    GetBoundCompileUnit().GetClassTemplateRepository().InstantiateAll(specialization, containerScope, currentFunction, fullInstantiationRequestNode.GetSpan(), fullInstantiationRequestNode.ModuleId());
    specialization->SetHasFullInstantiation();
}

void StatementBinder::Visit(StaticConstructorNode& staticConstructorNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&staticConstructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::staticConstructorSymbol , "static constructor symbol expected");
    StaticConstructorSymbol* staticConstructorSymbol = static_cast<StaticConstructorSymbol*>(symbol);
    if (!dontCheckDuplicateFunctionSymbols)
    {
        staticConstructorSymbol->FunctionGroup()->CheckDuplicateFunctionSymbols();
    }
    StaticConstructorSymbol* prevStaticConstructorSymbol = currentStaticConstructorSymbol;
    currentStaticConstructorSymbol = staticConstructorSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, staticConstructorSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (staticConstructorNode.Body())
    {
        GenerateEnterAndExitFunctionCode(currentFunction);
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

void StatementBinder::GenerateEnterAndExitFunctionCode(BoundFunction* boundFunction)
{
    if (cmajor::symbols::GetBackEnd() == cmajor::symbols::BackEnd::cmsx) return;
    Span span;
    boost::uuids::uuid moduleId = boost::uuids::nil_uuid();
    if (boundFunction->GetFunctionSymbol()->DontThrow()) return;
    TypeSymbol* systemRuntimeUnwindInfoSymbol = boundCompileUnit.GetSystemRuntimeUnwindInfoSymbol();
    if (systemRuntimeUnwindInfoSymbol == nullptr)
    {
        sngcm::ast::IdentifierNode systemRuntimeUnwindInfoNode(span, moduleId, U"System.Runtime.UnwindInfo");
        systemRuntimeUnwindInfoSymbol = ResolveType(&systemRuntimeUnwindInfoNode, boundCompileUnit, containerScope);
        boundCompileUnit.SetSystemRuntimeUnwindInfoSymbol(systemRuntimeUnwindInfoSymbol);
    }
    FunctionSymbol* initUnwindSymbol = boundCompileUnit.GetInitUnwindInfoFunctionSymbol();
    if (initUnwindSymbol == nullptr)
    {
        boundCompileUnit.GenerateInitUnwindInfoFunctionSymbol();
    }
    LocalVariableSymbol* prevUnwindInfoVariableSymbol = new LocalVariableSymbol(span, moduleId, U"@prevUnwindInfo");
    containerScope->Install(prevUnwindInfoVariableSymbol);
    prevUnwindInfoVariableSymbol->SetType(systemRuntimeUnwindInfoSymbol->AddPointer(span, moduleId));
    boundFunction->GetFunctionSymbol()->SetPrevUnwindInfoVar(prevUnwindInfoVariableSymbol);
    sngcm::ast::IdentifierNode* prevUnwindInfoNode1 = new IdentifierNode(span, moduleId, U"@prevUnwindInfo");
    symbolTable.MapSymbol(prevUnwindInfoNode1, prevUnwindInfoVariableSymbol);
    symbolTable.MapNode(prevUnwindInfoNode1, prevUnwindInfoVariableSymbol);
    sngcm::ast::InvokeNode* pushUnwindInfo = new sngcm::ast::InvokeNode(span, moduleId, new sngcm::ast::IdentifierNode(span, moduleId, U"RtPushUnwindInfo"));
    LocalVariableSymbol* unwindInfoVariableSymbol = new LocalVariableSymbol(span, moduleId, U"@unwindInfo");
    containerScope->Install(unwindInfoVariableSymbol);
    unwindInfoVariableSymbol->SetType(systemRuntimeUnwindInfoSymbol);
    boundFunction->GetFunctionSymbol()->SetUnwindInfoVar(unwindInfoVariableSymbol);
    sngcm::ast::IdentifierNode* unwindInfoNode1 = new IdentifierNode(span, moduleId, U"@unwindInfo");
    symbolTable.MapSymbol(unwindInfoNode1, unwindInfoVariableSymbol);
    symbolTable.MapNode(unwindInfoNode1, unwindInfoVariableSymbol);
    pushUnwindInfo->AddArgument(new sngcm::ast::CastNode(span, moduleId, new sngcm::ast::PointerNode(span, moduleId, new sngcm::ast::VoidNode(span, moduleId)), new AddrOfNode(span, moduleId, unwindInfoNode1)));
    sngcm::ast::AssignmentStatementNode assignUnwindInfo(span, moduleId, prevUnwindInfoNode1,
        new sngcm::ast::CastNode(span, moduleId, new PointerNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Runtime.UnwindInfo")), pushUnwindInfo));
    assignUnwindInfo.Accept(*this);
    std::unique_ptr<BoundStatement> pushUnwindInfoStatement(statement.release());

    sngcm::ast::IdentifierNode* prevUnwindInfoNode2 = new IdentifierNode(span, moduleId, U"@prevUnwindInfo");
    symbolTable.MapSymbol(prevUnwindInfoNode2, prevUnwindInfoVariableSymbol);
    symbolTable.MapNode(prevUnwindInfoNode2, prevUnwindInfoVariableSymbol);
    sngcm::ast::IdentifierNode* unwindInfoNode2 = new IdentifierNode(span, moduleId, U"@unwindInfo");
    symbolTable.MapSymbol(unwindInfoNode2, unwindInfoVariableSymbol);
    symbolTable.MapNode(unwindInfoNode2, unwindInfoVariableSymbol);
    sngcm::ast::AssignmentStatementNode assignUnwindInfoNext(span, moduleId, new sngcm::ast::DotNode(span, moduleId, unwindInfoNode2, new IdentifierNode(span, moduleId, U"next")), prevUnwindInfoNode2);
    assignUnwindInfoNext.Accept(*this);
    std::unique_ptr<BoundStatement> assignUnwindInfoNextStatement(statement.release());

    sngcm::ast::IdentifierNode* unwindInfoNode3 = new IdentifierNode(span, moduleId, U"@unwindInfo");
    symbolTable.MapSymbol(unwindInfoNode3, unwindInfoVariableSymbol);
    symbolTable.MapNode(unwindInfoNode3, unwindInfoVariableSymbol);
    sngcm::ast::FunctionPtrNode* functionPtrNode = new sngcm::ast::FunctionPtrNode(span, moduleId);
    BoundFunctionPtr* boundFunctionPtr = new BoundFunctionPtr(span, moduleId, boundFunction->GetFunctionSymbol(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span, moduleId));
    BoundBitCast* boundBitCast = new BoundBitCast(std::unique_ptr<BoundExpression>(boundFunctionPtr), boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span, moduleId));
    std::unique_ptr<BoundExpression> boundFunctionPtrAsVoidPtr(boundBitCast);
    functionPtrNode->SetBoundExpression(boundFunctionPtrAsVoidPtr.get());
    sngcm::ast::AssignmentStatementNode assignFunctionPtr(span, moduleId, new sngcm::ast::DotNode(span, moduleId, unwindInfoNode3, new IdentifierNode(span, moduleId, U"function")),
        new sngcm::ast::CastNode(span, moduleId, new sngcm::ast::PointerNode(span, moduleId, new sngcm::ast::VoidNode(span, moduleId)), functionPtrNode));
    assignFunctionPtr.Accept(*this);
    std::unique_ptr<BoundStatement> assignFunctionPtrStatement(statement.release());

    sngcm::ast::IdentifierNode* unwindInfoNode4 = new IdentifierNode(span, moduleId, U"@unwindInfo");
    symbolTable.MapSymbol(unwindInfoNode4, unwindInfoVariableSymbol);
    symbolTable.MapNode(unwindInfoNode4, unwindInfoVariableSymbol);
    sngcm::ast::AssignmentStatementNode assignUnwindInfoLine(span, moduleId, new sngcm::ast::DotNode(span, moduleId, unwindInfoNode4, new IdentifierNode(span, moduleId, U"line")), 
        new IntLiteralNode(span, moduleId, 0));
    assignUnwindInfoLine.Accept(*this);
    std::unique_ptr<BoundStatement> assignUnwindInfoLineStatement(statement.release());

    sngcm::ast::IdentifierNode* prevUnwindInfoNode3 = new IdentifierNode(span, moduleId, U"@prevUnwindInfo");
    symbolTable.MapSymbol(prevUnwindInfoNode3, prevUnwindInfoVariableSymbol);
    symbolTable.MapNode(prevUnwindInfoNode3, prevUnwindInfoVariableSymbol);
    sngcm::ast::InvokeNode* setPrevUnwindInfoListPtr = new sngcm::ast::InvokeNode(span, moduleId, new sngcm::ast::IdentifierNode(span, moduleId, U"RtPopUnwindInfo"));
    setPrevUnwindInfoListPtr->AddArgument(new CastNode(span, moduleId, new PointerNode(span, moduleId, new VoidNode(span, moduleId)), prevUnwindInfoNode3));
    sngcm::ast::ExpressionStatementNode setPrevUnwindInfoList(span, moduleId, setPrevUnwindInfoListPtr);
    setPrevUnwindInfoList.Accept(*this);
    std::unique_ptr<BoundStatement> setPrevUnwindInfoListStatement(statement.release());

    std::vector<std::unique_ptr<BoundStatement>> enterCode;
    enterCode.push_back(std::move(pushUnwindInfoStatement));
    enterCode.push_back(std::move(assignUnwindInfoNextStatement));
    enterCode.push_back(std::move(assignFunctionPtrStatement));
    enterCode.push_back(std::move(assignUnwindInfoLineStatement));
    boundFunction->SetEnterCode(std::move(enterCode));

    std::unique_ptr<BoundStatement> setLineCode;
    sngcm::ast::IdentifierNode* unwindInfoNode5 = new IdentifierNode(span, moduleId, U"@unwindInfo");
    symbolTable.MapSymbol(unwindInfoNode5, unwindInfoVariableSymbol);
    sngcm::ast::AssignmentStatementNode setUnwindInfoLine(span, moduleId, new sngcm::ast::DotNode(span, moduleId, unwindInfoNode5, new IdentifierNode(span, moduleId, U"line")), 
        new IntLiteralNode(span, moduleId, 0));
    setUnwindInfoLine.Accept(*this);
    std::unique_ptr<BoundStatement> setLineStatement(statement.release());
    boundFunction->SetLineCode(std::move(setLineStatement));

    std::vector<std::unique_ptr<BoundStatement>> exitCode;
    exitCode.push_back(std::move(setPrevUnwindInfoListStatement));
    boundFunction->SetExitCode(std::move(exitCode));
}

void StatementBinder::Visit(ConstructorNode& constructorNode)
{
    ContainerScope* prevContainerScope = containerScope;
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&constructorNode);
    Assert(symbol->GetSymbolType() == SymbolType::constructorSymbol, "constructor symbol expected");
    ConstructorSymbol* constructorSymbol = static_cast<ConstructorSymbol*>(symbol);
    if (!dontCheckDuplicateFunctionSymbols)
    {
        constructorSymbol->FunctionGroup()->CheckDuplicateFunctionSymbols();
    }
    ConstructorSymbol* prevConstructorSymbol = currentConstructorSymbol;
    currentConstructorSymbol = constructorSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, constructorSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (constructorNode.Body())
    {
        GenerateEnterAndExitFunctionCode(currentFunction);
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
        std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(constructorNode.GetSpan(), constructorNode.ModuleId()));
        GenerateClassInitialization(currentConstructorSymbol, currentConstructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, true, 
            constructorNode.GetSpan(), constructorNode.ModuleId());
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
    if (!dontCheckDuplicateFunctionSymbols)
    {
        destructorSymbol->FunctionGroup()->CheckDuplicateFunctionSymbols();
    }
    DestructorSymbol* prevDestructorSymbol = currentDestructorSymbol;
    currentDestructorSymbol = destructorSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, destructorSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (destructorNode.Body())
    {
        GenerateEnterAndExitFunctionCode(currentFunction);
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
        std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(destructorNode.GetSpan(), destructorNode.ModuleId()));
        GenerateClassTermination(currentDestructorSymbol, currentDestructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, 
            currentDestructorNode->GetSpan(), currentDestructorNode->ModuleId());
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
    if (!dontCheckDuplicateFunctionSymbols)
    {
        memberFunctionSymbol->FunctionGroup()->CheckDuplicateFunctionSymbols();
    }
    MemberFunctionSymbol* prevMemberFunctionSymbol = currentMemberFunctionSymbol;
    currentMemberFunctionSymbol = memberFunctionSymbol;
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, memberFunctionSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (memberFunctionNode.Body())
    {
        GenerateEnterAndExitFunctionCode(currentFunction);
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
        std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId()));
        GenerateClassAssignment(currentMemberFunctionSymbol, currentMemberFunctionNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, true, 
            memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId());
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
    if (!dontCheckDuplicateFunctionSymbols)
    {
        conversionFunctionSymbol->FunctionGroup()->CheckDuplicateFunctionSymbols();
    }
    containerScope = symbol->GetContainerScope();
    std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&boundCompileUnit, conversionFunctionSymbol));
    BoundFunction* prevFunction = currentFunction;
    currentFunction = boundFunction.get();
    if (conversionFunctionNode.Body())
    {
        GenerateEnterAndExitFunctionCode(currentFunction);
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
    std::unique_ptr<BoundCompoundStatement> boundCompoundStatement(new BoundCompoundStatement(compoundStatementNode.BeginBraceSpan(), compoundStatementNode.EndBraceSpan(), compoundStatementNode.ModuleId()));
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
                ConstructionStatementNode constructFunctionProfiler(compoundStatementNode.GetSpan(), compoundStatementNode.ModuleId(), 
                    new IdentifierNode(compoundStatementNode.GetSpan(), compoundStatementNode.ModuleId(), U"System.Runtime.FunctionProfiler"),
                    new IdentifierNode(compoundStatementNode.GetSpan(), compoundStatementNode.ModuleId(), U"@functionProfiler"));
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
                boundCompoundStatement->GetSpan(), boundCompoundStatement->ModuleId());
        }
        else if (currentConstructorSymbol && currentConstructorNode)
        {
            GenerateClassInitialization(currentConstructorSymbol, currentConstructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, false,
                boundCompoundStatement->GetSpan(), boundCompoundStatement->ModuleId());
        }
        else if (currentMemberFunctionSymbol && currentMemberFunctionSymbol->GroupName() == U"operator=" && currentMemberFunctionNode)
        {
            GenerateClassAssignment(currentMemberFunctionSymbol, currentMemberFunctionNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, false,
                boundCompoundStatement->GetSpan(), boundCompoundStatement->ModuleId());
        }
        else if (currentMemberFunctionSymbol && currentMemberFunctionSymbol->IsStatic() && currentMemberFunctionNode)
        {
            if (currentClass->GetClassTypeSymbol()->StaticConstructor())
            {
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundInitializationStatement(std::unique_ptr<BoundExpression>(
                    new BoundFunctionCall(boundCompoundStatement->GetSpan(), boundCompoundStatement->ModuleId(), currentClass->GetClassTypeSymbol()->StaticConstructor())))));
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
        GenerateClassTermination(currentDestructorSymbol, currentDestructorNode, boundCompoundStatement.get(), currentFunction, boundCompileUnit, containerScope, this, 
            boundCompoundStatement->EndSpan(), boundCompoundStatement->ModuleId());
    }
    AddStatement(boundCompoundStatement.release());
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
            classReturnArgs.push_back(std::unique_ptr<BoundExpression>(new BoundParameter(returnStatementNode.GetSpan(), returnStatementNode.ModuleId(), currentFunction->GetFunctionSymbol()->ReturnParam())));
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
                    expression.reset(new BoundConversion(std::move(expression), 
                        boundCompileUnit.GetConversion(exprType, returnType, containerScope, currentFunction, returnStatementNode.GetSpan(), returnStatementNode.ModuleId(), argumentMatch)));
                }
                rvalueArguments.push_back(std::move(expression));
                std::unique_ptr<BoundExpression> rvalueExpr = ResolveOverload(U"System.Rvalue", containerScope, rvalueLookups, rvalueArguments, boundCompileUnit, currentFunction,
                    returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                expression = std::move(rvalueExpr);
            }
            classReturnArgs.push_back(std::move(expression));
            std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, classReturnLookups, classReturnArgs, boundCompileUnit, currentFunction,
                returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
            std::unique_ptr<BoundStatement> constructStatement(new BoundInitializationStatement(std::move(constructorCall)));
            AddStatement(constructStatement.release());
            std::unique_ptr<BoundFunctionCall> returnFunctionCall;
            std::unique_ptr<BoundStatement> returnStatement(new BoundReturnStatement(std::move(returnFunctionCall), returnStatementNode.GetSpan(), returnStatementNode.ModuleId()));
            if (exceptionCapture)
            {
                AddReleaseExceptionStatement(returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
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
                BoundTypeExpression* boundTypeExpression = new BoundTypeExpression(returnStatementNode.GetSpan(), returnStatementNode.ModuleId(), returnType);
                returnTypeArgs.push_back(std::unique_ptr<BoundTypeExpression>(boundTypeExpression));
                std::vector<FunctionScopeLookup> functionScopeLookups;
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, returnType->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::unique_ptr<BoundFunctionCall> returnFunctionCall = ResolveOverload(U"@return", containerScope, functionScopeLookups, returnTypeArgs, boundCompileUnit, currentFunction,
                    returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
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
                    containerScope, currentFunction, returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                if (conversionFound)
                {
                    Assert(!functionMatch.argumentMatches.empty(), "argument match expected");
                    ArgumentMatch argumentMatch = functionMatch.argumentMatches[0];
                    if (argumentMatch.preReferenceConversionFlags != OperationFlags::none)
                    {
                        if (argumentMatch.preReferenceConversionFlags == OperationFlags::addr)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->AddLvalueReference(returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                            BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(addressOfExpression);
                        }
                        else if (argumentMatch.preReferenceConversionFlags == OperationFlags::deref)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->RemoveReference(returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                            BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(dereferenceExpression);
                        }
                    }
                    FunctionSymbol* conversionFun = argumentMatch.conversionFun;
                    if (conversionFun)
                    {
                        if (conversionFun->GetSymbolType() == SymbolType::constructorSymbol)
                        {
                            BoundFunctionCall* constructorCall = new BoundFunctionCall(returnStatementNode.GetSpan(), returnStatementNode.ModuleId(), conversionFun);
                            LocalVariableSymbol* temporary = currentFunction->GetFunctionSymbol()->CreateTemporary(conversionFun->ConversionTargetType(), 
                                returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                            constructorCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(returnStatementNode.GetSpan(), returnStatementNode.ModuleId(), temporary)),
                                conversionFun->ConversionTargetType()->AddPointer(returnStatementNode.GetSpan(), returnStatementNode.ModuleId()))));
                            constructorCall->AddArgument(std::move(returnValueArguments[0]));
                            BoundConstructAndReturnTemporaryExpression* conversion = new BoundConstructAndReturnTemporaryExpression(std::unique_ptr<BoundExpression>(constructorCall),
                                std::unique_ptr<BoundExpression>(new BoundLocalVariable(returnStatementNode.GetSpan(), returnStatementNode.ModuleId(),temporary)));
                            returnValueArguments[0].reset(conversion);
                        }
                        else
                        {
                            BoundConversion* boundConversion = new BoundConversion(std::unique_ptr<BoundExpression>(returnValueArguments[0].release()), conversionFun);
                            returnValueArguments[0].reset(boundConversion);
                        }
                    }
                    if (argumentMatch.postReferenceConversionFlags != OperationFlags::none)
                    {
                        if (argumentMatch.postReferenceConversionFlags == OperationFlags::addr)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->AddLvalueReference(returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                            BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(addressOfExpression);
                        }
                        else if (argumentMatch.postReferenceConversionFlags == OperationFlags::deref)
                        {
                            TypeSymbol* type = returnValueArguments[0]->GetType()->RemoveReference(returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                            BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(std::move(returnValueArguments[0]), type);
                            returnValueArguments[0].reset(dereferenceExpression);
                        }
                    }
                    returnFunctionCall->SetArguments(std::move(returnValueArguments));
                }
                else
                {
                    throw Exception("no implicit conversion from '" + ToUtf8(returnValueArguments[0]->GetType()->FullName()) + "' to '" + ToUtf8(returnType->FullName()) + "' exists",
                        returnStatementNode.GetSpan(), returnStatementNode.ModuleId(), currentFunction->GetFunctionSymbol()->GetSpan(), currentFunction->GetFunctionSymbol()->SourceModuleId());
                }
                CheckAccess(currentFunction->GetFunctionSymbol(), returnFunctionCall->GetFunctionSymbol());
                if (exceptionCapture)
                {
                    AddReleaseExceptionStatement(returnStatementNode.GetSpan(), returnStatementNode.ModuleId());
                }
                AddStatement(new BoundReturnStatement(std::move(returnFunctionCall), returnStatementNode.GetSpan(), returnStatementNode.ModuleId()));
            }
            else
            {
                if (returnType)
                {
                    throw Exception("void function cannot return a value", returnStatementNode.Expression()->GetSpan(), returnStatementNode.Expression()->ModuleId(), 
                        currentFunction->GetFunctionSymbol()->GetSpan(), currentFunction->GetFunctionSymbol()->SourceModuleId());
                }
                else
                {
                    throw Exception("constructor or assignment function cannot return a value", returnStatementNode.Expression()->GetSpan(), returnStatementNode.Expression()->ModuleId(), 
                        currentFunction->GetFunctionSymbol()->GetSpan(), currentFunction->GetFunctionSymbol()->SourceModuleId());
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
            AddStatement(new BoundReturnStatement(std::move(returnFunctionCall), returnStatementNode.GetSpan(), returnStatementNode.ModuleId()));
        }
        else
        {
            throw Exception("nonvoid function must return a value", returnStatementNode.GetSpan(), returnStatementNode.ModuleId(), 
                currentFunction->GetFunctionSymbol()->GetSpan(), currentFunction->GetFunctionSymbol()->SourceModuleId());
        }
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
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(ifStatementNode.GetSpan(), ifStatementNode.ModuleId())))
    {
        throw Exception("condition of an if statement must be a Boolean expression", ifStatementNode.Condition()->GetSpan(), ifStatementNode.Condition()->ModuleId());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(std::move(condition), baseType));
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
        AddReleaseExceptionStatement(ifStatementNode.GetSpan(), ifStatementNode.ModuleId());
    }
    AddStatement(new BoundIfStatement(ifStatementNode.GetSpan(), ifStatementNode.ModuleId(), std::move(condition), std::unique_ptr<BoundStatement>(thenS), std::unique_ptr<BoundStatement>(elseS)));
}

void StatementBinder::Visit(WhileStatementNode& whileStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> condition = BindExpression(whileStatementNode.Condition(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(whileStatementNode.GetSpan(), whileStatementNode.ModuleId())))
    {
        throw Exception("condition of a while statement must be a Boolean expression", whileStatementNode.Condition()->GetSpan(), whileStatementNode.Condition()->ModuleId());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(std::move(condition), baseType));
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
        AddReleaseExceptionStatement(whileStatementNode.GetSpan(), whileStatementNode.ModuleId());
    }
    AddStatement(new BoundWhileStatement(whileStatementNode.GetSpan(), whileStatementNode.ModuleId(), std::move(condition), std::unique_ptr<BoundStatement>(stmt)));
}

void StatementBinder::Visit(DoStatementNode& doStatementNode)
{
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> condition = BindExpression(doStatementNode.Condition(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(doStatementNode.GetSpan(), doStatementNode.ModuleId())))
    {
        throw Exception("condition of a do statement must be a Boolean expression", doStatementNode.Condition()->GetSpan(), doStatementNode.Condition()->ModuleId());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(std::move(condition), baseType));
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
        AddReleaseExceptionStatement(doStatementNode.GetSpan(), doStatementNode.ModuleId());
    }
    AddStatement(new BoundDoStatement(doStatementNode.GetSpan(), doStatementNode.ModuleId(), std::unique_ptr<BoundStatement>(stmt), std::move(condition)));
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
        BooleanLiteralNode trueNode(forStatementNode.GetSpan(), forStatementNode.ModuleId(), true);
        condition = BindExpression(&trueNode, boundCompileUnit, currentFunction, containerScope, this);
    }
    bool exceptionCapture = false;
    if (insideCatch && condition->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (!TypesEqual(symbolTable.GetTypeByName(U"bool"), condition->GetType()->PlainType(forStatementNode.GetSpan(), forStatementNode.ModuleId())))
    {
        throw Exception("condition of a for statement must be a Boolean expression", forStatementNode.Condition()->GetSpan(), forStatementNode.Condition()->ModuleId());
    }
    if (condition->GetType()->IsReferenceType())
    {
        TypeSymbol* baseType = condition->GetType()->BaseType();
        condition.reset(new BoundDereferenceExpression(std::move(condition), baseType));
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
    loopS->SetForLoopStatementNode();
    forStatementNode.ActionS()->Accept(*this);
    BoundStatement* actionS = statement.release();
    if (s)
    {
        AddStatement(s.release());
    }
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(forStatementNode.GetSpan(), forStatementNode.ModuleId());
    }
    AddStatement(new BoundForStatement(forStatementNode.GetSpan(), forStatementNode.ModuleId(), std::unique_ptr<BoundStatement>(initS), std::move(condition), std::unique_ptr<BoundStatement>(loopS),
        std::unique_ptr<BoundStatement>(actionS)));
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
        throw Exception("break statement must be enclosed in a while, do, for or switch statement", breakStatementNode.GetSpan(), breakStatementNode.ModuleId());
    }
    AddStatement(new BoundBreakStatement(breakStatementNode.GetSpan(), breakStatementNode.ModuleId()));
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
        throw Exception("continue statement must be enclosed in a while, do or for statement", continueStatementNode.GetSpan(), continueStatementNode.ModuleId());
    }
    AddStatement(new BoundContinueStatement(continueStatementNode.GetSpan(), continueStatementNode.ModuleId()));
}

void StatementBinder::Visit(GotoStatementNode& gotoStatementNode)
{
    currentFunction->SetHasGotos();
    boundCompileUnit.SetHasGotos();
    AddStatement(new BoundGotoStatement(gotoStatementNode.GetSpan(), gotoStatementNode.ModuleId(), gotoStatementNode.Target()));
}

void StatementBinder::Visit(ConstructionStatementNode& constructionStatementNode)
{
    Symbol* symbol = boundCompileUnit.GetSymbolTable().GetSymbol(&constructionStatementNode);
    Assert(symbol->GetSymbolType() == SymbolType::localVariableSymbol, "local variable symbol expected");
    LocalVariableSymbol* localVariableSymbol = static_cast<LocalVariableSymbol*>(symbol);
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    BoundExpression* localVariable = new BoundLocalVariable(constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId(), localVariableSymbol);
    arguments.push_back(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(localVariable), 
        localVariable->GetType()->AddPointer(constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId()))));
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
        constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
    FunctionSymbol* functionSymbol = constructorCall->GetFunctionSymbol();
    CheckAccess(currentFunction->GetFunctionSymbol(), functionSymbol);
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
    }
    if (functionSymbol->Parent()->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(functionSymbol->Parent());
        if (classType->Destructor() && classType->Destructor()->IsGeneratedFunction() && !GetGlobalFlag(GlobalFlags::info))
        {
            if (!boundCompileUnit.IsGeneratedDestructorInstantiated(classType->Destructor()))
            {
                boundCompileUnit.SetGeneratedDestructorInstantiated(classType->Destructor());
                std::unique_ptr<BoundClass> boundClass(new BoundClass(classType));
                GenerateDestructorImplementation(boundClass.get(), classType->Destructor(), boundCompileUnit, containerScope, currentFunction,
                    constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
                boundCompileUnit.AddBoundNode(std::move(boundClass));
            }
        }
    }
    BoundConstructionStatement* boundConstructionStatement = new BoundConstructionStatement(std::move(constructorCall), constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
    boundConstructionStatement->SetLocalVariable(localVariableSymbol);
    AddStatement(boundConstructionStatement);
}

void StatementBinder::Visit(DeleteStatementNode& deleteStatementNode)
{
    Span span;
    boost::uuids::uuid moduleId = boost::uuids::nil_uuid();
    if (GetBackEnd() == BackEnd::llvm)
    {
        span = deleteStatementNode.GetSpan();
        moduleId = deleteStatementNode.ModuleId();
    }
    bool exceptionCapture = false;
    std::unique_ptr<BoundExpression> ptr = BindExpression(deleteStatementNode.Expression(), boundCompileUnit, currentFunction, containerScope, this);
    if (insideCatch && ptr->ContainsExceptionCapture())
    {
        exceptionCapture = true;
    }
    if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
    {
        if (GetConfig() == "debug")
        {
            std::vector<FunctionScopeLookup> lookups;
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> arguments;
            arguments.push_back(std::move(std::unique_ptr<BoundExpression>(ptr->Clone())));
            std::unique_ptr<BoundFunctionCall> disposeCall = ResolveOverload(U"RtDispose", containerScope, lookups, arguments, boundCompileUnit, currentFunction, span, moduleId);
            CheckAccess(currentFunction->GetFunctionSymbol(), disposeCall->GetFunctionSymbol());
            AddStatement(new BoundExpressionStatement(std::move(disposeCall), span, moduleId));
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
        std::unique_ptr<BoundFunctionCall> destructorCall = ResolveOverload(U"@destructor", containerScope, lookups, arguments, boundCompileUnit, currentFunction, span, moduleId);
        CheckAccess(currentFunction->GetFunctionSymbol(), destructorCall->GetFunctionSymbol());
        if (destructorCall->GetFunctionSymbol()->IsVirtualAbstractOrOverride())
        {
            destructorCall->SetFlag(BoundExpressionFlags::virtualCall);
        }
        AddStatement(new BoundExpressionStatement(std::move(destructorCall), span, moduleId));
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
    if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
    {
        memFreeFunctionName = U"RtMemFree";
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        memFreeFunctionName = U"MemFree";
    }
    std::unique_ptr<BoundFunctionCall> memFreeCall = ResolveOverload(memFreeFunctionName, containerScope, lookups, arguments, boundCompileUnit, currentFunction, span, moduleId);
    CheckAccess(currentFunction->GetFunctionSymbol(), memFreeCall->GetFunctionSymbol());
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(deleteStatementNode.GetSpan(), deleteStatementNode.ModuleId());
    }
    AddStatement(new BoundExpressionStatement(std::move(memFreeCall), deleteStatementNode.GetSpan(), deleteStatementNode.ModuleId()));
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
        throw Exception("destroy statement needs pointer type operand", destroyStatementNode.GetSpan(), destroyStatementNode.ModuleId());
    }
    TypeSymbol* pointeeType = ptr->GetType()->RemovePointer(destroyStatementNode.GetSpan(), destroyStatementNode.ModuleId());
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
        std::unique_ptr<BoundFunctionCall> destructorCall = ResolveOverload(U"@destructor", containerScope, lookups, arguments, boundCompileUnit, currentFunction, 
            destroyStatementNode.GetSpan(), destroyStatementNode.ModuleId());
        CheckAccess(currentFunction->GetFunctionSymbol(), destructorCall->GetFunctionSymbol());
        if (destructorCall->GetFunctionSymbol()->IsVirtualAbstractOrOverride())
        {
            destructorCall->SetFlag(BoundExpressionFlags::virtualCall);
        }
        if (exceptionCapture)
        {
            AddReleaseExceptionStatement(destroyStatementNode.GetSpan(), destroyStatementNode.ModuleId());
        }
        AddStatement(new BoundExpressionStatement(std::move(destructorCall), destroyStatementNode.GetSpan(), destroyStatementNode.ModuleId()));
    }
    else
    {
        AddStatement(new BoundEmptyStatement(destroyStatementNode.GetSpan(), destroyStatementNode.ModuleId()));
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
    TypeSymbol* targetPlainType = target->GetType()->PlainType(assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
    if ((targetPlainType->IsClassTypeSymbol() || targetPlainType->IsArrayType()) && target->GetType()->IsReferenceType())
    {
        TypeSymbol* type = target->GetType()->RemoveReference(assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId())->AddPointer(
            assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
        target.reset(new BoundReferenceToPointerExpression(std::unique_ptr<BoundExpression>(target.release()), type));
    }
    else if (targetPlainType->IsPointerType() && target->GetType()->IsReferenceType())
    {
        TypeSymbol* derefType = target->GetType()->RemoveReference(assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
        TypeSymbol* addrOfType = derefType->AddPointer(assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
        target.reset(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(
            new BoundDereferenceExpression(std::unique_ptr<BoundExpression>(target.release()), derefType)), addrOfType));
    }
    else
    {
        target.reset(new BoundAddressOfExpression(std::move(target), target->GetType()->AddPointer(assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId())));
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
        assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
    CheckAccess(currentFunction->GetFunctionSymbol(), assignmentCall->GetFunctionSymbol());
    currentFunction->MoveTemporaryDestructorCallsTo(*assignmentCall);
    if (exceptionCapture)
    {
        AddReleaseExceptionStatement(assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId());
    }
    AddStatement(new BoundAssignmentStatement(std::move(assignmentCall), assignmentStatementNode.GetSpan(), assignmentStatementNode.ModuleId()));
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
        AddReleaseExceptionStatement(expressionStatementNode.GetSpan(), expressionStatementNode.ModuleId());
    }
    AddStatement(new BoundExpressionStatement(std::move(expression), expressionStatementNode.GetSpan(), expressionStatementNode.ModuleId()));
}

void StatementBinder::Visit(EmptyStatementNode& emptyStatementNode)
{
    AddStatement(new BoundEmptyStatement(emptyStatementNode.GetSpan(), emptyStatementNode.ModuleId()));
}

void StatementBinder::Visit(RangeForStatementNode& rangeForStatementNode)
{
    Span span;
    boost::uuids::uuid moduleId = boost::uuids::nil_uuid();
    if (GetBackEnd() == BackEnd::llvm)
    {
        span = rangeForStatementNode.GetSpan();
        moduleId = rangeForStatementNode.ModuleId();
    }
    Span actionBeginBraceSpan;
    Span actionEndBraceSpan;
    if (rangeForStatementNode.Action()->GetNodeType() == NodeType::compoundStatementNode)
    {
        CompoundStatementNode* action = static_cast<CompoundStatementNode*>(rangeForStatementNode.Action());
        actionBeginBraceSpan = action->BeginBraceSpan();
        actionEndBraceSpan = action->EndBraceSpan();
    }
    else
    {
        actionBeginBraceSpan = rangeForStatementNode.Action()->GetSpan();
        actionEndBraceSpan = rangeForStatementNode.Action()->GetSpan();
    }
    Span initSpan = rangeForStatementNode.TypeExpr()->GetSpan();
    initSpan.end = rangeForStatementNode.Id()->GetSpan().end;
    Span containerSpan = rangeForStatementNode.Container()->GetSpan();
    Span colonSpan = rangeForStatementNode.ColonSpan();
    std::unique_ptr<BoundExpression> container = BindExpression(rangeForStatementNode.Container(), boundCompileUnit, currentFunction, containerScope, this);
    TypeSymbol* plainContainerType = container->GetType()->PlainType(span, moduleId);
    std::u32string plainContainerTypeFullName = plainContainerType->FullName();
    ParsingContext parsingContext;
    CmajorLexer cmajorLexer(plainContainerTypeFullName + U"\n", "", 0);
    cmajorLexer.SetSeparatorChar('\n');
    std::unique_ptr<Node> containerTypeNode(TypeExprParser::Parse(cmajorLexer, &moduleId, &parsingContext));
    std::unique_ptr<IdentifierNode> iteratorTypeNode = nullptr;
    if (container->GetType()->IsConstType())
    {
        iteratorTypeNode.reset(new IdentifierNode(span, moduleId, U"ConstIterator"));
    }
    else
    {
        iteratorTypeNode.reset(new IdentifierNode(span, moduleId, U"Iterator"));
    }
    CloneContext cloneContext;
    std::unique_ptr<CompoundStatementNode> compoundStatementNode(new CompoundStatementNode(span, moduleId));
    if (rangeForStatementNode.Action()->GetNodeType() == NodeType::compoundStatementNode)
    {
        CompoundStatementNode* action = static_cast<CompoundStatementNode*>(rangeForStatementNode.Action());
        compoundStatementNode->SetBeginBraceSpan(span);
        compoundStatementNode->SetEndBraceSpan(span);
    }
    else
    {
        compoundStatementNode->SetBeginBraceSpan(span);
        compoundStatementNode->SetEndBraceSpan(span);
    }
    compoundStatementNode->SetParent(rangeForStatementNode.Parent());
    ConstructionStatementNode* constructEndIteratorStatement = new ConstructionStatementNode(span, moduleId, 
        new DotNode(span, moduleId, containerTypeNode->Clone(cloneContext), static_cast<IdentifierNode*>(iteratorTypeNode->Clone(cloneContext))), new IdentifierNode(span, moduleId, U"@end"));
    if (container->GetType()->IsConstType())
    {
        constructEndIteratorStatement->AddArgument(new InvokeNode(span, moduleId, new DotNode(span, moduleId, rangeForStatementNode.Container()->Clone(cloneContext), 
            new IdentifierNode(span, moduleId, U"CEnd"))));
    }
    else
    {
        constructEndIteratorStatement->AddArgument(new InvokeNode(span, moduleId, new DotNode(span, moduleId, rangeForStatementNode.Container()->Clone(cloneContext), 
            new IdentifierNode(span, moduleId, U"End"))));
    }
    compoundStatementNode->AddStatement(constructEndIteratorStatement);
    ConstructionStatementNode* constructIteratorStatement = new ConstructionStatementNode(initSpan, moduleId, 
        new DotNode(span, moduleId, containerTypeNode->Clone(cloneContext), static_cast<IdentifierNode*>(iteratorTypeNode->Clone(cloneContext))), new IdentifierNode(span, moduleId, U"@it"));
    if (container->GetType()->IsConstType())
    {
        constructIteratorStatement->AddArgument(new InvokeNode(span, moduleId, new DotNode(span, moduleId, rangeForStatementNode.Container()->Clone(cloneContext), 
            new IdentifierNode(span, moduleId, U"CBegin"))));
    }
    else
    {
        constructIteratorStatement->AddArgument(new InvokeNode(span, moduleId, new DotNode(span, moduleId, rangeForStatementNode.Container()->Clone(cloneContext), 
            new IdentifierNode(span, moduleId, U"Begin"))));
    }
    Node* itNotEndCond = new NotEqualNode(colonSpan, moduleId, new IdentifierNode(colonSpan, moduleId, U"@it"), new IdentifierNode(colonSpan, moduleId, U"@end"));
    StatementNode* incrementItStatement = new ExpressionStatementNode(containerSpan, moduleId, new PrefixIncrementNode(containerSpan, moduleId, new IdentifierNode(containerSpan, moduleId, U"@it")));
    CompoundStatementNode* actionStatement = new CompoundStatementNode(actionBeginBraceSpan, moduleId);
    actionStatement->SetBeginBraceSpan(actionBeginBraceSpan);
    actionStatement->SetEndBraceSpan(actionEndBraceSpan);
    ConstructionStatementNode* constructLoopVarStatement = new ConstructionStatementNode(span, moduleId,
        rangeForStatementNode.TypeExpr()->Clone(cloneContext), static_cast<IdentifierNode*>(rangeForStatementNode.Id()->Clone(cloneContext)));
    constructLoopVarStatement->AddArgument(new DerefNode(span, moduleId, new IdentifierNode(span, moduleId, U"@it")));
    actionStatement->AddStatement(constructLoopVarStatement);
    actionStatement->AddStatement(static_cast<StatementNode*>(rangeForStatementNode.Action()->Clone(cloneContext)));
    ForStatementNode* forStatement = new ForStatementNode(span, moduleId, constructIteratorStatement, itNotEndCond, incrementItStatement, actionStatement);
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
        std::unique_ptr<BoundSwitchStatement> boundSwitchStatement(new BoundSwitchStatement(switchStatementNode.GetSpan(), switchStatementNode.ModuleId(), std::move(condition)));
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
                throw Exception("case not found", gotoCaseStatement->GetSpan(), gotoCaseStatement->ModuleId());
            }
        }
        if (!gotoDefaultStatements.empty() && !switchStatementNode.Default())
        {
            throw Exception("switch does not have a default statement", gotoDefaultStatements.front()->GetSpan(), gotoDefaultStatements.front()->ModuleId());
        }
        currentGotoCaseStatements = prevGotoCaseStatements;
        currentGotoDefaultStatements = prevGotoDefaultStatements;
        currentCaseValueMap = prevCaseValueMap;
        if (exceptionCapture)
        {
            AddReleaseExceptionStatement(switchStatementNode.GetSpan(), switchStatementNode.ModuleId());
        }
        AddStatement(boundSwitchStatement.release());
        switchConditionType = prevSwitchConditionType;
    }
    else
    {
        throw Exception("switch statement condition must be of integer, character, enumerated or Boolean type", switchStatementNode.Condition()->GetSpan(), switchStatementNode.Condition()->ModuleId());
    }
}

void StatementBinder::Visit(CaseStatementNode& caseStatementNode)
{
    std::unique_ptr<BoundCaseStatement> boundCaseStatement(new BoundCaseStatement(caseStatementNode.GetSpan(), caseStatementNode.ModuleId()));
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
        throw Exception("case must end in break, continue, return, throw, goto, goto case or goto default statement", caseStatementNode.GetSpan(), caseStatementNode.ModuleId());
    }
    int ne = caseStatementNode.CaseExprs().Count();
    for (int i = 0; i < ne; ++i)
    {
        Node* caseExprNode = caseStatementNode.CaseExprs()[i];
        std::unique_ptr<Value> caseValue = Evaluate(caseExprNode, switchConditionType, containerScope, boundCompileUnit, false, currentFunction, caseExprNode->GetSpan(), caseExprNode->ModuleId());
        IntegralValue integralCaseValue(caseValue.get());
        Assert(currentCaseValueMap, "current case value map not set");
        auto it = currentCaseValueMap->find(integralCaseValue);
        if (it != currentCaseValueMap->cend())
        {
            throw Exception("case value already used", caseExprNode->GetSpan(), caseExprNode->ModuleId());
        }
        (*currentCaseValueMap)[integralCaseValue] = &caseStatementNode;
        boundCaseStatement->AddCaseValue(std::move(caseValue));
    }
    AddStatement(boundCaseStatement.release());
}

void StatementBinder::Visit(DefaultStatementNode& defaultStatementNode)
{
    std::unique_ptr<BoundDefaultStatement> boundDefaultStatement(new BoundDefaultStatement(defaultStatementNode.GetSpan(), defaultStatementNode.ModuleId()));
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
        throw Exception("default must end in break, continue, return, throw, goto, or goto case statement", defaultStatementNode.GetSpan(), defaultStatementNode.ModuleId());
    }
    AddStatement(boundDefaultStatement.release());
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
        throw Exception("goto case statement must be enclosed in a case or default statement", gotoCaseStatementNode.GetSpan(), gotoCaseStatementNode.ModuleId());
    }
    Node* caseExprNode = gotoCaseStatementNode.CaseExpr();
    std::unique_ptr<Value> caseValue = Evaluate(caseExprNode, switchConditionType, containerScope, boundCompileUnit, false, currentFunction, gotoCaseStatementNode.GetSpan(), gotoCaseStatementNode.ModuleId());
    Value* caseValuePtr = caseValue.get();
    BoundGotoCaseStatement* boundGotoCaseStatement = new BoundGotoCaseStatement(gotoCaseStatementNode.GetSpan(), gotoCaseStatementNode.ModuleId(), std::move(caseValue));
    Assert(currentGotoCaseStatements, "current goto case statement vector not set");
    currentGotoCaseStatements->push_back(std::make_pair(boundGotoCaseStatement, IntegralValue(caseValuePtr)));
    AddStatement(boundGotoCaseStatement);
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
        throw Exception("goto default statement must be enclosed in a case statement", gotoDefaultStatementNode.GetSpan(), gotoDefaultStatementNode.ModuleId());
    }
    BoundGotoDefaultStatement* boundGotoDefaultStatement = new BoundGotoDefaultStatement(gotoDefaultStatementNode.GetSpan(), gotoDefaultStatementNode.ModuleId());
    Assert(currentGotoDefaultStatements, "current goto default statement vector not set");
    currentGotoDefaultStatements->push_back(boundGotoDefaultStatement);
    AddStatement(boundGotoDefaultStatement);
}

void StatementBinder::Visit(ThrowStatementNode& throwStatementNode)
{
    bool prevCompilingThrow = compilingThrow;
    compilingThrow = true;
    if (currentFunction->GetFunctionSymbol()->DontThrow() && !currentFunction->GetFunctionSymbol()->HasTry())
    {
        throw Exception("a nothrow function cannot contain a throw statement unless it handles exceptions", 
            throwStatementNode.GetSpan(), throwStatementNode.ModuleId(), currentFunction->GetFunctionSymbol()->GetSpan(), currentFunction->GetFunctionSymbol()->SourceModuleId());
    }
    Span span = throwStatementNode.GetSpan();
    boost::uuids::uuid moduleId = throwStatementNode.ModuleId();
    Node* exceptionExprNode = throwStatementNode.Expression();
    if (exceptionExprNode)
    {
        std::unique_ptr<BoundExpression> boundExceptionExpr = BindExpression(exceptionExprNode, boundCompileUnit, currentFunction, containerScope, this);
        if (boundExceptionExpr->GetType()->PlainType(span, moduleId)->IsClassTypeSymbol())
        {
            ClassTypeSymbol* exceptionClassType = static_cast<ClassTypeSymbol*>(boundExceptionExpr->GetType()->PlainType(span, moduleId));
            IdentifierNode systemExceptionNode(span, moduleId, U"System.Exception");
            TypeSymbol* systemExceptionType = ResolveType(&systemExceptionNode, boundCompileUnit, containerScope);
            Assert(systemExceptionType->IsClassTypeSymbol(), "System.Exception not of class type");
            ClassTypeSymbol* systemExceptionClassType = static_cast<ClassTypeSymbol*>(systemExceptionType);
            if (exceptionClassType == systemExceptionClassType || exceptionClassType->HasBaseClass(systemExceptionClassType))
            {
                NewNode* newNode = new NewNode(span, moduleId, new IdentifierNode(span, moduleId, exceptionClassType->FullName()));
                CloneContext cloneContext;
                newNode->AddArgument(throwStatementNode.Expression()->Clone(cloneContext));
                if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
                {
                    InvokeNode invokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"RtThrowException"));
                    invokeNode.AddArgument(newNode);
                    invokeNode.AddArgument(new UuidLiteralNode(span, moduleId, exceptionClassType->TypeId()));
                    std::unique_ptr<BoundExpression> throwCallExpr = BindExpression(&invokeNode, boundCompileUnit, currentFunction, containerScope, this);
                    AddStatement(new BoundThrowStatement(span, moduleId, std::move(throwCallExpr)));
                }
                else if (GetBackEnd() == BackEnd::cmsx)
                {
                    InvokeNode invokeNode(span, moduleId, new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"System"), new IdentifierNode(span, moduleId, U"Throw")));
                    invokeNode.AddArgument(newNode);
                    std::unique_ptr<BoundExpression> throwCallExpr = BindExpression(&invokeNode, boundCompileUnit, currentFunction, containerScope, this);
                    AddStatement(new BoundThrowStatement(span, moduleId, std::move(throwCallExpr)));
                }
            }
            else
            {
                throw Exception("exception class must be derived from System.Exception class", throwStatementNode.GetSpan(), throwStatementNode.ModuleId());
            }
        }
        else
        {
            throw Exception("exception not of class type", throwStatementNode.GetSpan(), throwStatementNode.ModuleId());
        }
    }
    else
    {
        if (insideCatch)
        {
            InvokeNode invokeNode(span, moduleId, new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"@exPtr"), new IdentifierNode(span, moduleId, U"Release")));
            std::unique_ptr<BoundExpression> releaseCall = BindExpression(&invokeNode, boundCompileUnit, currentFunction, containerScope, this);
            AddStatement(new BoundRethrowStatement(span, moduleId, std::move(releaseCall)));
        }
        else
        {
            throw Exception("rethrow must occur inside a catch clause", throwStatementNode.GetSpan(), throwStatementNode.ModuleId());
        }
    }
    compilingThrow = prevCompilingThrow;
}

void StatementBinder::Visit(TryStatementNode& tryStatementNode)
{
    BoundTryStatement* boundTryStatement = new BoundTryStatement(tryStatementNode.GetSpan(), tryStatementNode.ModuleId());
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
}

void StatementBinder::Visit(CatchNode& catchNode)
{
    bool prevInsideCatch = insideCatch;
    insideCatch = true;
    Span span;
    boost::uuids::uuid moduleId = boost::uuids::nil_uuid();
    if (GetBackEnd() == BackEnd::llvm)
    {
        span = catchNode.GetSpan();
        moduleId = catchNode.ModuleId();
    }
    std::unique_ptr<BoundCatchStatement> boundCatchStatement(new BoundCatchStatement(span, moduleId));
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
    CompoundStatementNode handlerBlock(catchNode.CatchBlock()->BeginBraceSpan(), moduleId);
    handlerBlock.SetBeginBraceSpan(catchNode.CatchBlock()->BeginBraceSpan());
    handlerBlock.SetEndBraceSpan(catchNode.CatchBlock()->EndBraceSpan());
    handlerBlock.SetParent(catchNode.Parent());
    if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
    {
        ConstructionStatementNode* getExceptionAddr = new ConstructionStatementNode(span, moduleId, new PointerNode(span, moduleId, new IdentifierNode(span, moduleId, U"void")), 
            new IdentifierNode(span, moduleId, U"@exceptionAddr"));
        getExceptionAddr->AddArgument(new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"RtGetException")));
        handlerBlock.AddStatement(getExceptionAddr);
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        ConstructionStatementNode* getExceptionAddr = new ConstructionStatementNode(span, moduleId, new PointerNode(span, moduleId, new IdentifierNode(span, moduleId, U"void")), 
            new IdentifierNode(span, moduleId, U"@exceptionAddr"));
        getExceptionAddr->AddArgument(new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"do_catch")));
        handlerBlock.AddStatement(getExceptionAddr);
    }
    PointerNode exceptionPtrTypeNode(span, moduleId, new IdentifierNode(span, moduleId, catchedType->BaseType()->FullName()));
    CloneContext cloneContext;
    ConstructionStatementNode* constructExceptionPtr = new ConstructionStatementNode(span, moduleId, exceptionPtrTypeNode.Clone(cloneContext), 
        new IdentifierNode(span, moduleId, U"@exceptionPtr"));
    constructExceptionPtr->AddArgument(new CastNode(span, moduleId, exceptionPtrTypeNode.Clone(cloneContext), new IdentifierNode(span, moduleId, U"@exceptionAddr")));
    handlerBlock.AddStatement(constructExceptionPtr);
    TemplateIdNode* uniquePtrNode = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"UniquePtr"));
    uniquePtrNode->AddTemplateArgument(new IdentifierNode(span, moduleId, catchedType->BaseType()->FullName()));
    ConstructionStatementNode* constructUniquePtrException = new ConstructionStatementNode(span, moduleId, uniquePtrNode, new IdentifierNode(span, moduleId, U"@exPtr"));
    constructUniquePtrException->AddArgument(new IdentifierNode(span, moduleId, U"@exceptionPtr"));
    handlerBlock.AddStatement(constructUniquePtrException);
    if (catchVar)
    {
        ConstructionStatementNode* setExceptionVar = new ConstructionStatementNode(span, moduleId, catchNode.TypeExpr()->Clone(cloneContext), static_cast<IdentifierNode*>(catchNode.Id()->Clone(cloneContext)));
        setExceptionVar->AddArgument(new DerefNode(span, moduleId, new IdentifierNode(span, moduleId, U"@exPtr")));
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
        int32_t assertionLineNumber = assertStatementNode.GetSpan().line;
        int32_t assertionIndex = GetNextUnitTestAssertionNumber();
        AddAssertionLineNumber(assertionLineNumber);
        InvokeNode* invokeSetUnitTestAssertionResult = new InvokeNode(assertStatementNode.GetSpan(), assertStatementNode.ModuleId(),
            new IdentifierNode(assertStatementNode.GetSpan(), assertStatementNode.ModuleId(), U"RtSetUnitTestAssertionResult"));
        invokeSetUnitTestAssertionResult->AddArgument(new IntLiteralNode(assertStatementNode.GetSpan(), assertStatementNode.ModuleId(), assertionIndex));
        CloneContext cloneContext;
        invokeSetUnitTestAssertionResult->AddArgument(assertStatementNode.AssertExpr()->Clone(cloneContext));
        invokeSetUnitTestAssertionResult->AddArgument(new IntLiteralNode(assertStatementNode.GetSpan(), assertStatementNode.ModuleId(), assertionLineNumber));
        ExpressionStatementNode setUnitTestAssertionResult(assertStatementNode.GetSpan(), assertStatementNode.ModuleId(), invokeSetUnitTestAssertionResult);
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
            AddStatement(new BoundEmptyStatement(assertStatementNode.GetSpan(), assertStatementNode.ModuleId()));
        }
        else
        {
            std::vector<FunctionScopeLookup> lookups;
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, symbolTable.GlobalNs().GetContainerScope()));
            std::vector<std::unique_ptr<BoundExpression>> arguments;
            TypeSymbol* constCharPtrType = symbolTable.GetTypeByName(U"char")->AddConst(Span(), boost::uuids::nil_uuid())->AddPointer(Span(), boost::uuids::nil_uuid());
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(std::unique_ptr<Value>(new StringValue(Span(), boost::uuids::nil_uuid(),
                boundCompileUnit.Install(assertStatementNode.AssertExpr()->ToString()), assertStatementNode.AssertExpr()->ToString())), constCharPtrType)));
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(std::unique_ptr<Value>(new StringValue(Span(), boost::uuids::nil_uuid(),
                boundCompileUnit.Install(ToUtf8(currentFunction->GetFunctionSymbol()->FullName())), ToUtf8(currentFunction->GetFunctionSymbol()->FullName()))), constCharPtrType)));
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(std::unique_ptr<Value>(new StringValue(Span(), boost::uuids::nil_uuid(),
                boundCompileUnit.Install(GetSourceFilePath(assertStatementNode.GetSpan().fileIndex, assertStatementNode.ModuleId())), 
                GetSourceFilePath(assertStatementNode.GetSpan().fileIndex, assertStatementNode.ModuleId()))), constCharPtrType)));
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(std::unique_ptr<Value>(new IntValue(Span(), boost::uuids::nil_uuid(),
                assertStatementNode.GetSpan().line)), symbolTable.GetTypeByName(U"int"))));
            std::unique_ptr<BoundExpression> assertExpression = BindExpression(assertStatementNode.AssertExpr(), boundCompileUnit, currentFunction, containerScope, this);
            const char32_t* failAssertionFunctionName = U"";
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
            {
                failAssertionFunctionName = U"RtFailAssertion";
            }
            else if (GetBackEnd() == BackEnd::cmsx)
            {
                failAssertionFunctionName = U"System.FailAssertion";
            }
            std::unique_ptr<BoundStatement> emptyStatement(new BoundEmptyStatement(Span(), boost::uuids::nil_uuid()));
            emptyStatement->SetIgnoreNode();
            std::unique_ptr<BoundStatement> failAssertionStatement(
                new BoundExpressionStatement(ResolveOverload(failAssertionFunctionName, containerScope, lookups, arguments,
                    boundCompileUnit, currentFunction, Span(), boost::uuids::nil_uuid()), Span(), boost::uuids::nil_uuid()));
            failAssertionStatement->SetIgnoreNode();
            std::unique_ptr<BoundStatement> ifStatement(new BoundIfStatement(assertStatementNode.GetSpan(), assertStatementNode.ModuleId(), std::move(assertExpression),
                std::unique_ptr<BoundStatement>(emptyStatement.release()),
                std::unique_ptr<BoundStatement>(failAssertionStatement.release())));
            ifStatement->SetAssertNode();
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
            AddStatement(new BoundEmptyStatement(conditionalCompilationStatementNode.GetSpan(), conditionalCompilationStatementNode.ModuleId()));
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
                    AddStatement(new BoundEmptyStatement(conditionalCompilationStatementNode.GetSpan(), conditionalCompilationStatementNode.ModuleId()));
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
                    AddStatement(new BoundEmptyStatement(conditionalCompilationStatementNode.GetSpan(), conditionalCompilationStatementNode.ModuleId()));
                }
            }
            else
            {
                AddStatement(new BoundEmptyStatement(conditionalCompilationStatementNode.GetSpan(), conditionalCompilationStatementNode.ModuleId()));
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

void StatementBinder::AddReleaseExceptionStatement(const Span& span, const boost::uuids::uuid& moduleId)
{
    if (insideCatch && !compilingReleaseExceptionStatement)
    {
        compilingReleaseExceptionStatement = true;
        InvokeNode* invokeNode(new InvokeNode(span, moduleId, new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"@exPtr"), new IdentifierNode(span, moduleId, U"Release"))));
        ExpressionStatementNode releaseExceptionStatement(span, moduleId, invokeNode);
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
            BoundSequenceStatement* sequenceStatement = new BoundSequenceStatement(boundStatement->GetSpan(), boundStatement->ModuleId(), std::unique_ptr<BoundStatement>(boundStatement), std::move(statement));
            boundStatement = sequenceStatement;
        }
        else
        {
            BoundSequenceStatement* sequenceStatement = new BoundSequenceStatement(boundStatement->GetSpan(), boundStatement->ModuleId(), std::move(statement), std::unique_ptr<BoundStatement>(boundStatement));
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
