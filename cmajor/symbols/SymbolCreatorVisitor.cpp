// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Interface.hpp>

namespace cmajor { namespace symbols {

SymbolCreatorVisitor::SymbolCreatorVisitor(SymbolTable& symbolTable_) : 
    symbolTable(symbolTable_), classInstanceNode(nullptr), classTemplateSpecialization(nullptr), functionIndex(0), leaveFunction(false)
{
}

void SymbolCreatorVisitor::SetClassInstanceNode(ClassNode* classInstanceNode_)
{
    classInstanceNode = classInstanceNode_;
}

void SymbolCreatorVisitor::SetClassTemplateSpecialization(ClassTemplateSpecializationSymbol* classTemplateSpecialization_)
{
    classTemplateSpecialization = classTemplateSpecialization_;
}

void SymbolCreatorVisitor::Visit(CompileUnitNode& compileUnitNode)
{
    compileUnitNode.GlobalNs()->Accept(*this);
}

void SymbolCreatorVisitor::Visit(NamespaceNode& namespaceNode)
{
    symbolTable.BeginNamespace(namespaceNode);
    NodeList<Node>& members = namespaceNode.Members();
    int n = members.Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = members[i];
        member->Accept(*this);
    }
    symbolTable.EndNamespace();
}

void SymbolCreatorVisitor::Visit(FunctionNode& functionNode)
{
    symbolTable.BeginFunction(functionNode, functionIndex++);
    int nt = functionNode.TemplateParameters().Count();
    for (int i = 0; i < nt; ++i)
    {
        symbolTable.AddTemplateParameter(*functionNode.TemplateParameters()[i]);
    }
    int n = functionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = functionNode.Parameters()[i];
        parameterNode->Accept(*this);
    }
    if (nt == 0)
    {
        if (functionNode.Body())
        {
            functionNode.Body()->Accept(*this);
        }
    }
    symbolTable.EndFunction(!leaveFunction);
}

void SymbolCreatorVisitor::Visit(ParameterNode& parameterNode)
{
    symbolTable.AddParameter(parameterNode);
}

void SymbolCreatorVisitor::Visit(ClassNode& classNode)
{
    if (&classNode == classInstanceNode)
    {
        symbolTable.BeginClassTemplateSpecialization(*classInstanceNode, classTemplateSpecialization);
    }
    else
    {
        symbolTable.BeginClass(classNode);
    }
    int nt = classNode.TemplateParameters().Count();
    for (int i = 0; i < nt; ++i)
    {
        symbolTable.AddTemplateParameter(*classNode.TemplateParameters()[i]);
    }
    if (nt == 0)
    {
        int n = classNode.Members().Count();
        for (int i = 0; i < n; ++i)
        {
            Node* member = classNode.Members()[i];
            member->Accept(*this);
        }
    }
    if (&classNode == classInstanceNode)
    {
        symbolTable.EndClassTemplateSpecialization();
    }
    else
    {
        symbolTable.EndClass();
    }
}

void SymbolCreatorVisitor::Visit(StaticConstructorNode& staticConstructorNode)
{
    symbolTable.BeginStaticConstructor(staticConstructorNode, functionIndex++);
    if (staticConstructorNode.Body())
    {
        staticConstructorNode.Body()->Accept(*this);
    }
    symbolTable.EndStaticConstructor(!leaveFunction);
}

void SymbolCreatorVisitor::Visit(ConstructorNode& constructorNode)
{
    symbolTable.BeginConstructor(constructorNode, functionIndex++);
    int n = constructorNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = constructorNode.Parameters()[i];
        parameterNode->Accept(*this);
    }
    if (constructorNode.Body())
    {
        constructorNode.Body()->Accept(*this);
    }
    symbolTable.EndConstructor(!leaveFunction);
}

void SymbolCreatorVisitor::Visit(DestructorNode& destructorNode)
{
    symbolTable.BeginDestructor(destructorNode, functionIndex++);
    if (destructorNode.Body())
    {
        destructorNode.Body()->Accept(*this);
    }
    symbolTable.EndDestructor(!leaveFunction);
}

void SymbolCreatorVisitor::Visit(MemberFunctionNode& memberFunctionNode)
{
    symbolTable.BeginMemberFunction(memberFunctionNode, functionIndex++);
    int n = memberFunctionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = memberFunctionNode.Parameters()[i];
        parameterNode->Accept(*this);
    }
    if (memberFunctionNode.Body())
    {
        memberFunctionNode.Body()->Accept(*this);
    }
    symbolTable.EndMemberFunction(!leaveFunction);
}

void SymbolCreatorVisitor::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    symbolTable.BeginConversionFunction(conversionFunctionNode, functionIndex++);
    if (conversionFunctionNode.Body())
    {
        conversionFunctionNode.Body()->Accept(*this);
    }
    symbolTable.EndConversionFunction(!leaveFunction);
}

void SymbolCreatorVisitor::Visit(MemberVariableNode& memberVariableNode)
{
    symbolTable.AddMemberVariable(memberVariableNode);
}

void SymbolCreatorVisitor::Visit(InterfaceNode& interfaceNode)
{
    symbolTable.BeginInterface(interfaceNode);
    int n = interfaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = interfaceNode.Members()[i];
        member->Accept(*this);
    }
    symbolTable.EndInterface();
}

void SymbolCreatorVisitor::Visit(DelegateNode& delegateNode)
{
    symbolTable.BeginDelegate(delegateNode);
    int n = delegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = delegateNode.Parameters()[i];
        parameterNode->Accept(*this);
    }
    symbolTable.EndDelegate();
}

void SymbolCreatorVisitor::Visit(ClassDelegateNode& classDelegateNode)
{
    symbolTable.BeginClassDelegate(classDelegateNode);
    int n = classDelegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = classDelegateNode.Parameters()[i];
        parameterNode->Accept(*this);
    }
    symbolTable.EndClassDelegate();
}

void SymbolCreatorVisitor::Visit(ConceptNode& conceptNode)
{
    symbolTable.BeginConcept(conceptNode, true);
    int n = conceptNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        IdentifierNode* identifierNode = conceptNode.TypeParameters()[i];
        symbolTable.AddTemplateParameter(*identifierNode);
    }
    symbolTable.EndConcept();
}

void SymbolCreatorVisitor::Visit(CompoundStatementNode& compoundStatementNode)
{
    symbolTable.BeginDeclarationBlock(compoundStatementNode);
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statement = compoundStatementNode.Statements()[i];
        statement->Accept(*this);
    }
    symbolTable.EndDeclarationBlock();
}

void SymbolCreatorVisitor::Visit(IfStatementNode& ifStatementNode)
{
    ifStatementNode.ThenS()->Accept(*this);
    if (ifStatementNode.ElseS())
    {
        ifStatementNode.ElseS()->Accept(*this);
    }
}

void SymbolCreatorVisitor::Visit(WhileStatementNode& whileStatementNode)
{
    whileStatementNode.Statement()->Accept(*this);
}

void SymbolCreatorVisitor::Visit(DoStatementNode& doStatementNode)
{
    doStatementNode.Statement()->Accept(*this);
}

void SymbolCreatorVisitor::Visit(ForStatementNode& forStatementNode)
{
    symbolTable.BeginDeclarationBlock(forStatementNode);
    forStatementNode.InitS()->Accept(*this);
    forStatementNode.ActionS()->Accept(*this);
    symbolTable.EndDeclarationBlock();
}

void SymbolCreatorVisitor::Visit(ConstructionStatementNode& constructionStatementNode)
{
    symbolTable.AddLocalVariable(constructionStatementNode);
}

void SymbolCreatorVisitor::Visit(SwitchStatementNode& switchStatementNode)
{
    int n = switchStatementNode.Cases().Count();
    for (int i = 0; i < n; ++i)
    {
        CaseStatementNode* caseStatementNode = switchStatementNode.Cases()[i];
        caseStatementNode->Accept(*this);
    }
    if (switchStatementNode.Default())
    {
        switchStatementNode.Default()->Accept(*this);
    }
}

void SymbolCreatorVisitor::Visit(CaseStatementNode& caseStatementNode)
{
    int n = caseStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = caseStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
}

void SymbolCreatorVisitor::Visit(DefaultStatementNode& defaultStatementNode)
{
    int n = defaultStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = defaultStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
}

void SymbolCreatorVisitor::Visit(CatchNode& catchNode)
{
    symbolTable.BeginDeclarationBlock(catchNode);
    if (catchNode.Id())
    {
        symbolTable.AddLocalVariable(*catchNode.Id());
    }
    catchNode.CatchBlock()->Accept(*this);
    symbolTable.EndDeclarationBlock();
}

void SymbolCreatorVisitor::Visit(TryStatementNode& tryStatementNode)
{
    tryStatementNode.TryBlock()->Accept(*this);
    int n = tryStatementNode.Catches().Count();
    for (int i = 0; i < n; ++i)
    {
        CatchNode* catchNode = tryStatementNode.Catches()[i];
        catchNode->Accept(*this);
    }
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    conditionalCompilationPartNode.Expr()->Accept(*this);
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    conditionalCompilationDisjunctionNode.Left()->Accept(*this);
    bool left = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationDisjunctionNode.Right()->Accept(*this);
    bool right = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(left || right);
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    conditionalCompilationConjunctionNode.Left()->Accept(*this);
    bool left = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationConjunctionNode.Right()->Accept(*this);
    bool right = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(left && right);
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    conditionalCompilationNotNode.Expr()->Accept(*this);
    bool operand = conditionalCompilationStack.top();
    conditionalCompilationStack.pop();
    conditionalCompilationStack.push(!operand);
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    bool defined = symbolTable.GetModule()->IsSymbolDefined(conditionalCompilationPrimaryNode.Symbol());
    conditionalCompilationStack.push(defined);
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
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

void SymbolCreatorVisitor::Visit(TypedefNode& typedefNode)
{
    symbolTable.AddTypedef(typedefNode);
}

void SymbolCreatorVisitor::Visit(ConstantNode& constantNode) 
{
    symbolTable.AddConstant(constantNode);
}

void SymbolCreatorVisitor::Visit(EnumTypeNode& enumTypeNode) 
{
    symbolTable.BeginEnumType(enumTypeNode);
    int n = enumTypeNode.Constants().Count();
    for (int i = 0; i < n; ++i)
    {
        enumTypeNode.Constants()[i]->Accept(*this);
    }
    symbolTable.EndEnumType();
}

void SymbolCreatorVisitor::Visit(EnumConstantNode& enumConstantNode) 
{
    symbolTable.AddEnumConstant(enumConstantNode);
}

void SymbolCreatorVisitor::Visit(GlobalVariableNode& globalVariableNode)
{
    symbolTable.AddGlobalVariable(globalVariableNode);
}

} } // namespace cmajor::symbols
