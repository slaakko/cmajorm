// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/Sources.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/ConstantSymbol.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Interface.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Literal.hpp>

namespace cmajor { namespace symbols {

SymbolCreatorVisitor::SymbolCreatorVisitor(SymbolTable& symbolTable_) : 
    symbolTable(symbolTable_), classInstanceNode(nullptr), classTemplateSpecialization(nullptr), functionIndex(0), leaveFunction(false), editMode(false), level(0), source(nullptr)
{
    symbolTable.ResetCursorContainer();
    symbolTable.ResetAxiomNumber();
    symbolTable.ResetAliasNodesAndNamespaceImports();
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
    try
    {
        compileUnitNode.GlobalNs()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(NamespaceNode& namespaceNode)
{
    try
    {
        symbolTable.BeginNamespace(namespaceNode);
        if (namespaceNode.Id())
        {
            namespaceNode.Id()->Accept(*this);
        }
        NodeList<Node>& members = namespaceNode.Members();
        int n = members.Count();
        for (int i = 0; i < n; ++i)
        {
            Node* member = members[i];
            member->Accept(*this);
        }
        symbolTable.EndNamespace();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AliasNode& aliasNode)
{
    try
    {
        symbolTable.SetCursorContainer(aliasNode);
        aliasNode.Id()->Accept(*this);
        aliasNode.Qid()->Accept(*this);
        if (editMode)
        {
            symbolTable.AddAliasNode(&aliasNode);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(NamespaceImportNode& namespaceImportNode)
{
    try
    {
        symbolTable.SetCursorContainer(namespaceImportNode);
        namespaceImportNode.Ns()->Accept(*this);
        if (editMode)
        {
            symbolTable.AddNamespaceImport(&namespaceImportNode);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(IdentifierNode& identifierNode)
{
    symbolTable.SetCursorContainer(identifierNode);
}

void SymbolCreatorVisitor::Visit(CursorIdNode& cursorIdNode)
{
    symbolTable.SetCursorContainer(cursorIdNode);
}

void SymbolCreatorVisitor::Visit(TemplateIdNode& templateIdNode)
{
    try
    {
        symbolTable.SetCursorContainer(templateIdNode);
        templateIdNode.Primary()->Accept(*this);
        const NodeList<Node>& args = templateIdNode.TemplateArguments();
        int n = args.Count();
        for (int i = 0; i < n; ++i)
        {
            Node* arg = args[i];
            arg->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(FunctionNode& functionNode)
{
    try
    {
        symbolTable.BeginFunction(functionNode, functionIndex++);
        ++level;
        if (functionNode.ReturnTypeExpr())
        {
            functionNode.ReturnTypeExpr()->Accept(*this);
        }
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
        if (functionNode.WhereConstraint())
        {
            functionNode.WhereConstraint()->Accept(*this);
        }
        if (nt == 0 || editMode)
        {
            if (functionNode.Body())
            {
                functionNode.Body()->Accept(*this);
            }
        }
        --level;
        if (level == 0 && source)
        {
            source->AddSymbol(symbolTable.Container());
        }
        symbolTable.EndFunction(!leaveFunction);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(FunctionPtrNode& functionPtrNode) 
{
    symbolTable.SetCursorContainer(functionPtrNode);
}

void SymbolCreatorVisitor::Visit(ClassNode& classNode)
{
    try
    {
        if (&classNode == classInstanceNode)
        {
            symbolTable.BeginClassTemplateSpecialization(*classInstanceNode, classTemplateSpecialization);
        }
        else
        {
            symbolTable.BeginClass(classNode);
        }
        ++level;
        classNode.Id()->Accept(*this);
        int nt = classNode.TemplateParameters().Count();
        for (int i = 0; i < nt; ++i)
        {
            symbolTable.AddTemplateParameter(*classNode.TemplateParameters()[i]);
        }
        int nb = classNode.BaseClassOrInterfaces().Count();
        for (int i = 0; i < nb; ++i)
        {
            classNode.BaseClassOrInterfaces()[i]->Accept(*this);
        }
        if (nt == 0 || editMode)
        {
            int n = classNode.Members().Count();
            for (int i = 0; i < n; ++i)
            {
                Node* member = classNode.Members()[i];
                member->Accept(*this);
            }
        }
        --level;
        if (level == 0 && source)
        {
            source->AddSymbol(symbolTable.CurrentClass());
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
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ThisInitializerNode& thisInitializerNode) 
{
    try
    {
        symbolTable.SetCursorContainer(thisInitializerNode);
        int n = thisInitializerNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            thisInitializerNode.Arguments()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(BaseInitializerNode& baseInitializerNode) 
{
    try
    {
        symbolTable.SetCursorContainer(baseInitializerNode);
        int n = baseInitializerNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            baseInitializerNode.Arguments()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(MemberInitializerNode& memberInitializerNode) 
{
    try
    {
        symbolTable.SetCursorContainer(memberInitializerNode);
        memberInitializerNode.MemberId()->Accept(*this);
        int n = memberInitializerNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            memberInitializerNode.Arguments()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(StaticConstructorNode& staticConstructorNode)
{
    try
    {
        symbolTable.BeginStaticConstructor(staticConstructorNode, functionIndex++);
        ++level;
        int ni = staticConstructorNode.Initializers().Count();
        for (int i = 0; i < ni; ++i)
        {
            staticConstructorNode.Initializers()[i]->Accept(*this);
        }
        if (staticConstructorNode.WhereConstraint())
        {
            staticConstructorNode.WhereConstraint()->Accept(*this);
        }
        if (staticConstructorNode.Body())
        {
            InsertTracer(staticConstructorNode.Body());
            staticConstructorNode.Body()->Accept(*this);
        }
        --level;
        symbolTable.EndStaticConstructor(!leaveFunction);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConstructorNode& constructorNode)
{
    try
    {
        symbolTable.BeginConstructor(constructorNode, functionIndex++);
        ++level;
        int ni = constructorNode.Initializers().Count();
        for (int i = 0; i < ni; ++i)
        {
            constructorNode.Initializers()[i]->Accept(*this);
        }
        if (constructorNode.WhereConstraint())
        {
            constructorNode.WhereConstraint()->Accept(*this);
        }
        int n = constructorNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            ParameterNode* parameterNode = constructorNode.Parameters()[i];
            parameterNode->Accept(*this);
        }
        if (constructorNode.Body())
        {
            InsertTracer(constructorNode.Body());
            constructorNode.Body()->Accept(*this);
        }
        --level;
        symbolTable.EndConstructor(!leaveFunction);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DestructorNode& destructorNode)
{
    try
    {
        symbolTable.BeginDestructor(destructorNode, functionIndex++);
        ++level;
        if (destructorNode.Body())
        {
            InsertTracer(destructorNode.Body());
            destructorNode.Body()->Accept(*this);
        }
        --level;
        symbolTable.EndDestructor(!leaveFunction);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(MemberFunctionNode& memberFunctionNode)
{
    try
    {
        symbolTable.BeginMemberFunction(memberFunctionNode, functionIndex++);
        ++level;
        if (memberFunctionNode.WhereConstraint())
        {
            memberFunctionNode.WhereConstraint()->Accept(*this);
        }
        if (memberFunctionNode.ReturnTypeExpr())
        {
            memberFunctionNode.ReturnTypeExpr()->Accept(*this);
        }
        int n = memberFunctionNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            ParameterNode* parameterNode = memberFunctionNode.Parameters()[i];
            parameterNode->Accept(*this);
        }
        if (memberFunctionNode.Body())
        {
            InsertTracer(memberFunctionNode.Body());
            memberFunctionNode.Body()->Accept(*this);
        }
        --level;
        symbolTable.EndMemberFunction(!leaveFunction);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    try
    {
        symbolTable.BeginConversionFunction(conversionFunctionNode, functionIndex++);
        ++level;
        if (conversionFunctionNode.WhereConstraint())
        {
            conversionFunctionNode.WhereConstraint()->Accept(*this);
        }
        if (conversionFunctionNode.ReturnTypeExpr())
        {
            conversionFunctionNode.ReturnTypeExpr()->Accept(*this);
        }
        if (conversionFunctionNode.Body())
        {
            InsertTracer(conversionFunctionNode.Body());
            conversionFunctionNode.Body()->Accept(*this);
        }
        --level;
        symbolTable.EndConversionFunction(!leaveFunction);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(MemberVariableNode& memberVariableNode)
{
    try
    {
        symbolTable.AddMemberVariable(memberVariableNode);
        memberVariableNode.TypeExpr()->Accept(*this);
        memberVariableNode.Id()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(InterfaceNode& interfaceNode)
{
    try
    {
        symbolTable.BeginInterface(interfaceNode);
        ++level;
        interfaceNode.Id()->Accept(*this);
        int n = interfaceNode.Members().Count();
        for (int i = 0; i < n; ++i)
        {
            Node* member = interfaceNode.Members()[i];
            member->Accept(*this);
        }
        --level;
        if (level == 0 && source)
        {
            source->AddSymbol(symbolTable.CurrentInterface());
        }
        symbolTable.EndInterface();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DelegateNode& delegateNode)
{
    try
    {
        symbolTable.BeginDelegate(delegateNode);
        ++level;
        delegateNode.ReturnTypeExpr()->Accept(*this);
        delegateNode.Id()->Accept(*this);
        int n = delegateNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            ParameterNode* parameterNode = delegateNode.Parameters()[i];
            parameterNode->Accept(*this);
        }
        --level;
        if (level == 0 && source)
        {
            source->AddSymbol(symbolTable.Container());
        }
        symbolTable.EndDelegate();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ClassDelegateNode& classDelegateNode)
{
    try
    {
        symbolTable.BeginClassDelegate(classDelegateNode);
        ++level;
        classDelegateNode.ReturnTypeExpr()->Accept(*this);
        classDelegateNode.Id()->Accept(*this);
        int n = classDelegateNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            ParameterNode* parameterNode = classDelegateNode.Parameters()[i];
            parameterNode->Accept(*this);
        }
        --level;
        if (level == 0 && source)
        {
            source->AddSymbol(symbolTable.Container());
        }
        symbolTable.EndClassDelegate();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(parenthesizedConstraintNode);
        parenthesizedConstraintNode.Constraint()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(disjunctiveConstraintNode);
        disjunctiveConstraintNode.Left()->Accept(*this);
        disjunctiveConstraintNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(conjunctiveConstraintNode);
        conjunctiveConstraintNode.Left()->Accept(*this);
        conjunctiveConstraintNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(WhereConstraintNode& whereConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(whereConstraintNode);
        whereConstraintNode.Constraint()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(PredicateConstraintNode& predicateConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(predicateConstraintNode);
        predicateConstraintNode.InvokeExpr()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(IsConstraintNode& isConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(isConstraintNode);
        isConstraintNode.TypeExpr()->Accept(*this);
        isConstraintNode.ConceptOrTypeName()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(MultiParamConstraintNode& multiParamConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(multiParamConstraintNode);
        multiParamConstraintNode.ConceptId()->Accept(*this);
        int n = multiParamConstraintNode.TypeExprs().Count();
        for (int i = 0; i < n; ++i)
        {
            multiParamConstraintNode.TypeExprs()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(TypeNameConstraintNode& typeNameConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(typeNameConstraintNode);
        typeNameConstraintNode.TypeId()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConstructorConstraintNode& constructorConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(constructorConstraintNode);
        constructorConstraintNode.TypeParamId()->Accept(*this);
        int n = constructorConstraintNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            constructorConstraintNode.Parameters()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DestructorConstraintNode& destructorConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(destructorConstraintNode);
        destructorConstraintNode.TypeParamId()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(memberFunctionConstraintNode);
        memberFunctionConstraintNode.ReturnTypeExpr()->Accept(*this);
        memberFunctionConstraintNode.TypeParamId()->Accept(*this);
        int n = memberFunctionConstraintNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            memberFunctionConstraintNode.Parameters()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(FunctionConstraintNode& functionConstraintNode) 
{
    try
    {
        symbolTable.SetCursorContainer(functionConstraintNode);
        functionConstraintNode.ReturnTypeExpr()->Accept(*this);
        int n = functionConstraintNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            functionConstraintNode.Parameters()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AxiomStatementNode& axiomStatementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(axiomStatementNode);
        axiomStatementNode.Expression()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AxiomNode& axiomNode) 
{
    try
    {
        symbolTable.SetCursorContainer(axiomNode);
        symbolTable.BeginAxiom(axiomNode);
        ++level;
        if (axiomNode.Id())
        {
            axiomNode.Id()->Accept(*this);
        }
        int n = axiomNode.Parameters().Count();
        for (int i = 0; i < n; ++i)
        {
            axiomNode.Parameters()[i]->Accept(*this);
        }
        int ns = axiomNode.Statements().Count();
        for (int i = 0; i < ns; ++i)
        {
            axiomNode.Statements()[i]->Accept(*this);
        }
        --level;
        symbolTable.EndAxiom();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConceptIdNode& conceptIdNode) 
{
    try
    {
        symbolTable.SetCursorContainer(conceptIdNode);
        conceptIdNode.Id()->Accept(*this);
        int n = conceptIdNode.TypeParameters().Count();
        for (int i = 0; i < n; ++i)
        {
            conceptIdNode.TypeParameters()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(SameConstraintNode& sameConstraintNode) 
{
    symbolTable.SetCursorContainer(sameConstraintNode);
}

void SymbolCreatorVisitor::Visit(DerivedConstraintNode& derivedConstraintNode) 
{
    symbolTable.SetCursorContainer(derivedConstraintNode);
}

void SymbolCreatorVisitor::Visit(ConvertibleConstraintNode& convertibleConstraintNode) 
{
    symbolTable.SetCursorContainer(convertibleConstraintNode);
}

void SymbolCreatorVisitor::Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode) 
{
    symbolTable.SetCursorContainer(explicitlyConvertibleConstraintNode);
}

void SymbolCreatorVisitor::Visit(CommonConstraintNode& commonConstraintNode) 
{
    symbolTable.SetCursorContainer(commonConstraintNode);
}

void SymbolCreatorVisitor::Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode) 
{
    symbolTable.SetCursorContainer(nonreferenceTypeConstraintNode);
}

void SymbolCreatorVisitor::Visit(ConceptNode& conceptNode)
{
    try
    {
        symbolTable.BeginConcept(conceptNode, true);
        ++level;
        conceptNode.Id()->Accept(*this);
        int n = conceptNode.TypeParameters().Count();
        for (int i = 0; i < n; ++i)
        {
            IdentifierNode* identifierNode = conceptNode.TypeParameters()[i];
            symbolTable.AddTemplateParameter(*identifierNode);
        }
        if (conceptNode.Refinement())
        {
            conceptNode.Refinement()->Accept(*this);
        }
        int nc = conceptNode.Constraints().Count();
        for (int i = 0; i < nc; ++i)
        {
            conceptNode.Constraints()[i]->Accept(*this);
        }
        int na = conceptNode.Axioms().Count();
        for (int i = 0; i < na; ++i)
        {
            conceptNode.Axioms()[i]->Accept(*this);
        }
        --level;
        if (level == 0 && source)
        {
            source->AddSymbol(symbolTable.Container());
        }
        symbolTable.EndConcept();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(LabelNode& labelNode) 
{
    symbolTable.SetCursorContainer(labelNode);
}

void SymbolCreatorVisitor::Visit(LabeledStatementNode& labeledStatementNode) 
{
    symbolTable.SetCursorContainer(labeledStatementNode);
    labeledStatementNode.Label()->Accept(*this);
}

void SymbolCreatorVisitor::Visit(CompoundStatementNode& compoundStatementNode)
{
    try
    {
        symbolTable.BeginDeclarationBlock(compoundStatementNode);
        ++level;
        int n = compoundStatementNode.Statements().Count();
        for (int i = 0; i < n; ++i)
        {
            StatementNode* statement = compoundStatementNode.Statements()[i];
            statement->Accept(*this);
        }
        --level;
        symbolTable.EndDeclarationBlock();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ReturnStatementNode& returnStatementNode)
{
    try
    {
        symbolTable.SetCursorContainer(returnStatementNode);
        if (returnStatementNode.Expression())
        {
            returnStatementNode.Expression()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(IfStatementNode& ifStatementNode)
{
    try
    {
        ifStatementNode.Condition()->Accept(*this);
        ifStatementNode.ThenS()->Accept(*this);
        if (ifStatementNode.ElseS())
        {
            ifStatementNode.ElseS()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(WhileStatementNode& whileStatementNode)
{
    try
    {
        whileStatementNode.Condition()->Accept(*this);
        whileStatementNode.Statement()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DoStatementNode& doStatementNode)
{
    try
    {
        doStatementNode.Statement()->Accept(*this);
        if (doStatementNode.Condition())
        {
            doStatementNode.Condition()->Accept(*this);
        }
        else
        {
            throw Exception("condition expected", doStatementNode.GetSpan(), doStatementNode.ModuleId());
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ForStatementNode& forStatementNode)
{
    try
    {
        symbolTable.BeginDeclarationBlock(forStatementNode);
        ++level;
        forStatementNode.InitS()->Accept(*this);
        if (forStatementNode.Condition())
        {
            forStatementNode.Condition()->Accept(*this);
        }
        else
        {
            throw Exception("condition expected", forStatementNode.GetSpan(), forStatementNode.ModuleId());
        }
        if (forStatementNode.LoopS())
        {
            forStatementNode.LoopS()->Accept(*this);
        }
        else
        {
            throw Exception("loop expression expected", forStatementNode.GetSpan(), forStatementNode.ModuleId());
        }
        if (forStatementNode.ActionS())
        {
            forStatementNode.ActionS()->Accept(*this);
        }
        else
        {
            throw Exception("action expected", forStatementNode.GetSpan(), forStatementNode.ModuleId());
        }
        --level;
        symbolTable.EndDeclarationBlock();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(BreakStatementNode& breakStatementNode) 
{
    symbolTable.SetCursorContainer(breakStatementNode);
}

void SymbolCreatorVisitor::Visit(ContinueStatementNode& continueStatementNode) 
{
    symbolTable.SetCursorContainer(continueStatementNode);
}

void SymbolCreatorVisitor::Visit(GotoStatementNode& gotoStatementNode) 
{
    symbolTable.SetCursorContainer(gotoStatementNode);
}

void SymbolCreatorVisitor::Visit(ConstructionStatementNode& constructionStatementNode)
{
    try
    {
        symbolTable.AddLocalVariable(constructionStatementNode);
        constructionStatementNode.TypeExpr()->Accept(*this);
        if (constructionStatementNode.Id())
        {
            constructionStatementNode.Id()->Accept(*this);
        }
        else
        {
            throw Exception("identifier expected", constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId());
        }
        int n = constructionStatementNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            constructionStatementNode.Arguments()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }

}

void SymbolCreatorVisitor::Visit(DeleteStatementNode& deleteStatementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(deleteStatementNode);
        deleteStatementNode.Expression()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DestroyStatementNode& destroyStatementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(destroyStatementNode);
        destroyStatementNode.Expression()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AssignmentStatementNode& assignmentStatementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(assignmentStatementNode);
        assignmentStatementNode.TargetExpr()->Accept(*this);
        assignmentStatementNode.SourceExpr()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ExpressionStatementNode& expressionStatementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(expressionStatementNode);
        expressionStatementNode.Expression()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(EmptyStatementNode& emptyStatementNode) 
{
    symbolTable.SetCursorContainer(emptyStatementNode);
}

void SymbolCreatorVisitor::Visit(RangeForStatementNode& rangeForStatementNode) 
{
    try
    {
        if (editMode)
        {
            symbolTable.SetCursorContainer(rangeForStatementNode);
            rangeForStatementNode.TypeExpr()->Accept(*this);
            rangeForStatementNode.Id()->Accept(*this);
            rangeForStatementNode.Container()->Accept(*this);
            rangeForStatementNode.Action()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(SwitchStatementNode& switchStatementNode)
{
    try
    {
        symbolTable.SetCursorContainer(switchStatementNode);
        switchStatementNode.Condition()->Accept(*this);
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
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(CaseStatementNode& caseStatementNode)
{
    try
    {
        symbolTable.SetCursorContainer(caseStatementNode);
        int nc = caseStatementNode.CaseExprs().Count();
        for (int i = 0; i < nc; ++i)
        {
            caseStatementNode.CaseExprs()[i]->Accept(*this);
        }
        int n = caseStatementNode.Statements().Count();
        for (int i = 0; i < n; ++i)
        {
            StatementNode* statementNode = caseStatementNode.Statements()[i];
            statementNode->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DefaultStatementNode& defaultStatementNode)
{
    try
    {
        symbolTable.SetCursorContainer(defaultStatementNode);
        int n = defaultStatementNode.Statements().Count();
        for (int i = 0; i < n; ++i)
        {
            StatementNode* statementNode = defaultStatementNode.Statements()[i];
            statementNode->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(GotoCaseStatementNode& gotoCaseStatementNode) 
{
    symbolTable.SetCursorContainer(gotoCaseStatementNode);
}

void SymbolCreatorVisitor::Visit(GotoDefaultStatementNode& gotoDefaultStatementNode) 
{
    symbolTable.SetCursorContainer(gotoDefaultStatementNode);
}

void SymbolCreatorVisitor::Visit(ThrowStatementNode& throwStatementNode) 
{
    symbolTable.SetCursorContainer(throwStatementNode);
}

void SymbolCreatorVisitor::Visit(TryStatementNode& tryStatementNode)
{
    try
    {
        symbolTable.SetCursorContainer(tryStatementNode);
        tryStatementNode.TryBlock()->Accept(*this);
        int n = tryStatementNode.Catches().Count();
        for (int i = 0; i < n; ++i)
        {
            CatchNode* catchNode = tryStatementNode.Catches()[i];
            catchNode->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(CatchNode& catchNode)
{
    try
    {
        symbolTable.BeginDeclarationBlock(catchNode);
        if (catchNode.Id())
        {
            symbolTable.AddLocalVariable(*catchNode.Id());
        }
        catchNode.CatchBlock()->Accept(*this);
        symbolTable.EndDeclarationBlock();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AssertStatementNode& assertStatementNode)
{
    try
    {
        symbolTable.SetCursorContainer(assertStatementNode);
        assertStatementNode.AssertExpr()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    try
    {
        symbolTable.SetCursorContainer(conditionalCompilationPartNode);
        conditionalCompilationPartNode.Expr()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    try
    {
        symbolTable.SetCursorContainer(conditionalCompilationDisjunctionNode);
        conditionalCompilationDisjunctionNode.Left()->Accept(*this);
        bool left = conditionalCompilationStack.top();
        conditionalCompilationStack.pop();
        conditionalCompilationDisjunctionNode.Right()->Accept(*this);
        bool right = conditionalCompilationStack.top();
        conditionalCompilationStack.pop();
        conditionalCompilationStack.push(left || right);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    try
    {
        symbolTable.SetCursorContainer(conditionalCompilationConjunctionNode);
        conditionalCompilationConjunctionNode.Left()->Accept(*this);
        bool left = conditionalCompilationStack.top();
        conditionalCompilationStack.pop();
        conditionalCompilationConjunctionNode.Right()->Accept(*this);
        bool right = conditionalCompilationStack.top();
        conditionalCompilationStack.pop();
        conditionalCompilationStack.push(left && right);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    try
    {
        symbolTable.SetCursorContainer(conditionalCompilationNotNode);
        conditionalCompilationNotNode.Expr()->Accept(*this);
        bool operand = conditionalCompilationStack.top();
        conditionalCompilationStack.pop();
        conditionalCompilationStack.push(!operand);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    try
    {
        symbolTable.SetCursorContainer(conditionalCompilationPrimaryNode);
        bool defined = symbolTable.GetModule()->IsSymbolDefined(conditionalCompilationPrimaryNode.Symbol());
        conditionalCompilationStack.push(defined);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    try
    {
        symbolTable.SetCursorContainer(conditionalCompilationStatementNode);
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
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(TypedefNode& typedefNode)
{
    TypedefSymbol* symbol = symbolTable.AddTypedef(typedefNode);
    if (level == 0 && source)
    {
        source->AddSymbol(symbol);
    }
}

void SymbolCreatorVisitor::Visit(ConstantNode& constantNode) 
{
    ConstantSymbol* symbol = symbolTable.AddConstant(constantNode);
    if (level == 0 && source)
    {
        source->AddSymbol(symbol);
    }
}

void SymbolCreatorVisitor::Visit(EnumTypeNode& enumTypeNode) 
{
    try
    {
        symbolTable.BeginEnumType(enumTypeNode);
        ++level;
        enumTypeNode.Id()->Accept(*this);
        if (enumTypeNode.GetUnderlyingType())
        {
            enumTypeNode.GetUnderlyingType()->Accept(*this);
        }
        int n = enumTypeNode.Constants().Count();
        for (int i = 0; i < n; ++i)
        {
            enumTypeNode.Constants()[i]->Accept(*this);
        }
        --level;
        if (level == 0 && source)
        {
            source->AddSymbol(symbolTable.Container());
        }
        symbolTable.EndEnumType();
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(EnumConstantNode& enumConstantNode) 
{
    try
    {
        symbolTable.AddEnumConstant(enumConstantNode);
        enumConstantNode.Id()->Accept(*this);
        if (enumConstantNode.GetValue())
        {
            enumConstantNode.GetValue()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(GlobalVariableNode& globalVariableNode)
{
    try
    {
        GlobalVariableSymbol* symbol = symbolTable.AddGlobalVariable(globalVariableNode);
        if (level == 0 && source)
        {
            source->AddSymbol(symbol);
        }
        globalVariableNode.TypeExpr()->Accept(*this);
        globalVariableNode.Id()->Accept(*this);
        if (globalVariableNode.Initializer())
        {
            globalVariableNode.Initializer()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ParameterNode& parameterNode)
{
    try
    {
        switch (parameterNode.Parent()->GetNodeType())
        {
        case NodeType::functionNode:
        case NodeType::constructorNode:
        case NodeType::memberFunctionNode:
        case NodeType::delegateNode:
        case NodeType::classDelegateNode:
        {
            symbolTable.AddParameter(parameterNode);
            break;
        }
        }
        parameterNode.TypeExpr()->Accept(*this);
        if (parameterNode.Id())
        {
            parameterNode.Id()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(TemplateParameterNode& templateParameterNode) 
{
    try
    {
        symbolTable.SetCursorContainer(templateParameterNode);
        templateParameterNode.Id()->Accept(*this);
        if (templateParameterNode.DefaultTemplateArgument())
        {
            templateParameterNode.DefaultTemplateArgument()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConstNode& constNode) 
{
    try
    {
        symbolTable.SetCursorContainer(constNode);
        constNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(LValueRefNode& lvalueRefNode) 
{
    try
    {
        symbolTable.SetCursorContainer(lvalueRefNode);
        lvalueRefNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(RValueRefNode& rvalueRefNode) 
{
    try
    {
        symbolTable.SetCursorContainer(rvalueRefNode);
        rvalueRefNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(PointerNode& pointerNode) 
{
    try
    {
        symbolTable.SetCursorContainer(pointerNode);
        pointerNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ArrayNode& arrayNode) 
{
    try
    {
        symbolTable.SetCursorContainer(arrayNode);
        arrayNode.Subject()->Accept(*this);
        if (arrayNode.Size())
        {
            arrayNode.Size()->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DotNode& dotNode) 
{
    try
    {
        symbolTable.SetCursorContainer(dotNode);
        dotNode.Subject()->Accept(*this);
        dotNode.MemberId()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ArrowNode& arrowNode) 
{
    try
    {
        symbolTable.SetCursorContainer(arrowNode);
        arrowNode.Subject()->Accept(*this);
        arrowNode.MemberId()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(EquivalenceNode& equivalenceNode) 
{
    try
    {
        symbolTable.SetCursorContainer(equivalenceNode);
        equivalenceNode.Left()->Accept(*this);
        equivalenceNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ImplicationNode& implicationNode) 
{
    try
    {
        symbolTable.SetCursorContainer(implicationNode);
        implicationNode.Left()->Accept(*this);
        implicationNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DisjunctionNode& disjunctionNode) 
{
    try
    {
        symbolTable.SetCursorContainer(disjunctionNode);
        disjunctionNode.Left()->Accept(*this);
        disjunctionNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConjunctionNode& conjunctionNode) 
{
    try
    {
        symbolTable.SetCursorContainer(conjunctionNode);
        conjunctionNode.Left()->Accept(*this);
        conjunctionNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(BitOrNode& bitOrNode) 
{
    try
    {
        symbolTable.SetCursorContainer(bitOrNode);
        bitOrNode.Left()->Accept(*this);
        bitOrNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(BitXorNode& bitXorNode) 
{
    try
    {
        symbolTable.SetCursorContainer(bitXorNode);
        bitXorNode.Left()->Accept(*this);
        bitXorNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(BitAndNode& bitAndNode) 
{
    try
    {
        symbolTable.SetCursorContainer(bitAndNode);
        bitAndNode.Left()->Accept(*this);
        bitAndNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(EqualNode& equalNode) 
{
    try
    {
        symbolTable.SetCursorContainer(equalNode);
        equalNode.Left()->Accept(*this);
        equalNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(NotEqualNode& notEqualNode) 
{
    try
    {
        symbolTable.SetCursorContainer(notEqualNode);
        notEqualNode.Left()->Accept(*this);
        notEqualNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(LessNode& lessNode) 
{
    try
    {
        symbolTable.SetCursorContainer(lessNode);
        lessNode.Left()->Accept(*this);
        lessNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(GreaterNode& greaterNode) 
{
    try
    {
        symbolTable.SetCursorContainer(greaterNode);
        greaterNode.Left()->Accept(*this);
        greaterNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(LessOrEqualNode& lessOrEqualNode) 
{
    try
    {
        symbolTable.SetCursorContainer(lessOrEqualNode);
        lessOrEqualNode.Left()->Accept(*this);
        lessOrEqualNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(GreaterOrEqualNode& greaterOrEqualNode) 
{
    try
    {
        symbolTable.SetCursorContainer(greaterOrEqualNode);
        greaterOrEqualNode.Left()->Accept(*this);
        greaterOrEqualNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ShiftLeftNode& shiftLeftNode) 
{
    try
    {
        symbolTable.SetCursorContainer(shiftLeftNode);
        shiftLeftNode.Left()->Accept(*this);
        shiftLeftNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ShiftRightNode& shiftRightNode) 
{
    try
    {
        symbolTable.SetCursorContainer(shiftRightNode);
        shiftRightNode.Left()->Accept(*this);
        shiftRightNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AddNode& addNode) 
{
    try
    {
        symbolTable.SetCursorContainer(addNode);
        addNode.Left()->Accept(*this);
        addNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(SubNode& subNode) 
{
    try
    {
        symbolTable.SetCursorContainer(subNode);
        subNode.Left()->Accept(*this);
        subNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(MulNode& mulNode) 
{
    try
    {
        symbolTable.SetCursorContainer(mulNode);
        mulNode.Left()->Accept(*this);
        mulNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DivNode& divNode) 
{
    try
    {
        symbolTable.SetCursorContainer(divNode);
        divNode.Left()->Accept(*this);
        divNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(RemNode& remNode) 
{
    try
    {
        symbolTable.SetCursorContainer(remNode);
        remNode.Left()->Accept(*this);
        remNode.Right()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(NotNode& notNode) 
{
    try
    {
        symbolTable.SetCursorContainer(notNode);
        notNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(UnaryPlusNode& unaryPlusNode) 
{
    try
    {
        symbolTable.SetCursorContainer(unaryPlusNode);
        unaryPlusNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(UnaryMinusNode& unaryMinusNode) 
{
    try
    {
        symbolTable.SetCursorContainer(unaryMinusNode);
        unaryMinusNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(PrefixIncrementNode& prefixIncrementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(prefixIncrementNode);
        prefixIncrementNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(PrefixDecrementNode& prefixDecrementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(prefixDecrementNode);
        prefixDecrementNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(DerefNode& derefNode) 
{
    try
    {
        symbolTable.SetCursorContainer(derefNode);
        derefNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AddrOfNode& addrOfNode) 
{
    try
    {
        symbolTable.SetCursorContainer(addrOfNode);
        addrOfNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ComplementNode& complementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(complementNode);
        complementNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(IsNode& isNode) 
{
    try
    {
        symbolTable.SetCursorContainer(isNode);
        isNode.Expr()->Accept(*this);
        if (isNode.TargetTypeExpr())
        {
            isNode.TargetTypeExpr()->Accept(*this);
        }
        else
        {
            throw Exception("target type expected", isNode.GetSpan(), isNode.ModuleId());
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(AsNode& asNode) 
{
    try
    {
        symbolTable.SetCursorContainer(asNode);
        asNode.Expr()->Accept(*this);
        if (asNode.TargetTypeExpr())
        {
            asNode.TargetTypeExpr()->Accept(*this);
        }
        else
        {
            throw Exception("target type expected", asNode.GetSpan(), asNode.ModuleId());
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(IndexingNode& indexingNode) 
{
    try
    {
        symbolTable.SetCursorContainer(indexingNode);
        indexingNode.Subject()->Accept(*this);
        if (indexingNode.Index())
        {
            indexingNode.Index()->Accept(*this);
        }
        else
        {
            throw Exception("index expected", indexingNode.GetSpan(), indexingNode.ModuleId());
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(InvokeNode& invokeNode) 
{
    try
    {
        symbolTable.SetCursorContainer(invokeNode);
        invokeNode.Subject()->Accept(*this);
        int n = invokeNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            invokeNode.Arguments()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(PostfixIncrementNode& postfixIncrementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(postfixIncrementNode);
        postfixIncrementNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(PostfixDecrementNode& postfixDecrementNode) 
{
    try
    {
        symbolTable.SetCursorContainer(postfixDecrementNode);
        postfixDecrementNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(SizeOfNode& sizeOfNode) 
{
    try
    {
        symbolTable.SetCursorContainer(sizeOfNode);
        sizeOfNode.Expression()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(TypeNameNode& typeNameNode) 
{
    try
    {
        symbolTable.SetCursorContainer(typeNameNode);
        typeNameNode.Expression()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(TypeIdNode& typeIdNode) 
{
    try
    {
        symbolTable.SetCursorContainer(typeIdNode);
        typeIdNode.Expression()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(CastNode& castNode) 
{
    try
    {
        symbolTable.SetCursorContainer(castNode);
        castNode.TargetTypeExpr()->Accept(*this);
        castNode.SourceExpr()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ConstructNode& constructNode) 
{
    try
    {
        symbolTable.SetCursorContainer(constructNode);
        constructNode.TypeExpr()->Accept(*this);
        int n = constructNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            constructNode.Arguments()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(NewNode& newNode) 
{
    try
    {
        symbolTable.SetCursorContainer(newNode);
        newNode.TypeExpr()->Accept(*this);
        int n = newNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            newNode.Arguments()[i]->Accept(*this);
        }
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(ThisNode& thisNode) 
{
    symbolTable.SetCursorContainer(thisNode);
}

void SymbolCreatorVisitor::Visit(BaseNode& baseNode) 
{
    symbolTable.SetCursorContainer(baseNode);
}

void SymbolCreatorVisitor::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode) 
{
    try
    {
        symbolTable.SetCursorContainer(parenthesizedExpressionNode);
        parenthesizedExpressionNode.Subject()->Accept(*this);
    }
    catch (const Exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.Message());
        }
        else
        {
            throw;
        }
    }
    catch (const std::exception& ex)
    {
        if (editMode)
        {
            errors.push_back(ex.what());
        }
        else
        {
            throw;
        }
    }
}

void SymbolCreatorVisitor::Visit(CommentNode& commentNode) 
{
    symbolTable.SetCursorContainer(commentNode);
}

void SymbolCreatorVisitor::Visit(BoolNode& boolNode) 
{
    symbolTable.SetCursorContainer(boolNode); 
}

void SymbolCreatorVisitor::Visit(SByteNode& sbyteNode)
{
    symbolTable.SetCursorContainer(sbyteNode);
}

void SymbolCreatorVisitor::Visit(ByteNode& byteNode)
{
    symbolTable.SetCursorContainer(byteNode);
}

void SymbolCreatorVisitor::Visit(ShortNode& shortNode)
{
    symbolTable.SetCursorContainer(shortNode);
}

void SymbolCreatorVisitor::Visit(UShortNode& ushortNode)
{
    symbolTable.SetCursorContainer(ushortNode);
}

void SymbolCreatorVisitor::Visit(IntNode& intNode)
{
    symbolTable.SetCursorContainer(intNode);
}

void SymbolCreatorVisitor::Visit(UIntNode& uintNode)
{
    symbolTable.SetCursorContainer(uintNode);
}

void SymbolCreatorVisitor::Visit(LongNode& longNode) 
{
    symbolTable.SetCursorContainer(longNode);
}

void SymbolCreatorVisitor::Visit(ULongNode& ulongNode)
{
    symbolTable.SetCursorContainer(ulongNode);
}

void SymbolCreatorVisitor::Visit(FloatNode& floatNode)
{
    symbolTable.SetCursorContainer(floatNode);
}

void SymbolCreatorVisitor::Visit(DoubleNode& doubleNode)
{
    symbolTable.SetCursorContainer(doubleNode);
}

void SymbolCreatorVisitor::Visit(CharNode& charNode)
{
    symbolTable.SetCursorContainer(charNode);
}

void SymbolCreatorVisitor::Visit(WCharNode& wcharNode)
{
    symbolTable.SetCursorContainer(wcharNode);
}

void SymbolCreatorVisitor::Visit(UCharNode& ucharNode)
{
    symbolTable.SetCursorContainer(ucharNode);
}

void SymbolCreatorVisitor::Visit(VoidNode& voidNode)
{
    symbolTable.SetCursorContainer(voidNode);
}

void SymbolCreatorVisitor::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    symbolTable.SetCursorContainer(booleanLiteralNode);
}

void SymbolCreatorVisitor::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    symbolTable.SetCursorContainer(sbyteLiteralNode);
}

void SymbolCreatorVisitor::Visit(ByteLiteralNode& byteLiteralNode)
{
    symbolTable.SetCursorContainer(byteLiteralNode);
}

void SymbolCreatorVisitor::Visit(ShortLiteralNode& shortLiteralNode)
{
    symbolTable.SetCursorContainer(shortLiteralNode);
}

void SymbolCreatorVisitor::Visit(UShortLiteralNode& ushortLiteralNode)
{
    symbolTable.SetCursorContainer(ushortLiteralNode);
}

void SymbolCreatorVisitor::Visit(IntLiteralNode& intLiteralNode)
{
    symbolTable.SetCursorContainer(intLiteralNode);
}

void SymbolCreatorVisitor::Visit(UIntLiteralNode& uintLiteralNode)
{
    symbolTable.SetCursorContainer(uintLiteralNode);
}

void SymbolCreatorVisitor::Visit(LongLiteralNode& longLiteralNode)
{
    symbolTable.SetCursorContainer(longLiteralNode);
}

void SymbolCreatorVisitor::Visit(ULongLiteralNode& ulongLiteralNode)
{
    symbolTable.SetCursorContainer(ulongLiteralNode);
}

void SymbolCreatorVisitor::Visit(FloatLiteralNode& floatLiteralNode)
{
    symbolTable.SetCursorContainer(floatLiteralNode);
}

void SymbolCreatorVisitor::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    symbolTable.SetCursorContainer(doubleLiteralNode);
}

void SymbolCreatorVisitor::Visit(CharLiteralNode& charLiteralNode)
{
    symbolTable.SetCursorContainer(charLiteralNode);
}

void SymbolCreatorVisitor::Visit(WCharLiteralNode& wcharLiteralNode)
{
    symbolTable.SetCursorContainer(wcharLiteralNode);
}

void SymbolCreatorVisitor::Visit(UCharLiteralNode& ucharLiteralNode)
{
    symbolTable.SetCursorContainer(ucharLiteralNode);
}

void SymbolCreatorVisitor::Visit(StringLiteralNode& stringLiteralNode)
{
    symbolTable.SetCursorContainer(stringLiteralNode);
}

void SymbolCreatorVisitor::Visit(WStringLiteralNode& wstringLiteralNode)
{
    symbolTable.SetCursorContainer(wstringLiteralNode);
}

void SymbolCreatorVisitor::Visit(UStringLiteralNode& ustringLiteralNode)
{
    symbolTable.SetCursorContainer(ustringLiteralNode);
}

void SymbolCreatorVisitor::Visit(NullLiteralNode& nullLiteralNode)
{
    symbolTable.SetCursorContainer(nullLiteralNode);
}

void SymbolCreatorVisitor::Visit(ArrayLiteralNode& arrayLiteralNode)
{
    symbolTable.SetCursorContainer(arrayLiteralNode);
}

void SymbolCreatorVisitor::Visit(StructuredLiteralNode& structuredLiteralNode)
{
    symbolTable.SetCursorContainer(structuredLiteralNode);
}

void SymbolCreatorVisitor::Visit(UuidLiteralNode& uuidLiteralNode)
{
    symbolTable.SetCursorContainer(uuidLiteralNode);
}

void SymbolCreatorVisitor::InsertTracer(CompoundStatementNode* body)
{
    if (!GetGlobalFlag(GlobalFlags::trace)) return;
    Module* rootModule = GetRootModuleForCurrentThread();
    if (!rootModule) return;
    if (rootModule->IsCore()) return;
    if (rootModule->Name() == U"System.Runtime") return;
    if (rootModule->Name() == U"System.Parsing") return;
    if (body->TracerInserted()) return;
    body->SetTracerInserted();
    Span span = body->GetSpan();
    boost::uuids::uuid moduleId = body->ModuleId();
    DotNode* typeExprNode = new DotNode(span, moduleId, new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"System"), new IdentifierNode(span, moduleId, U"Runtime")),
        new IdentifierNode(span, moduleId, U"Tracer"));
    ConstructionStatementNode* constructTracer(new ConstructionStatementNode(span, moduleId, typeExprNode, new IdentifierNode(span, moduleId, U"@tracer")));
    constructTracer->AddArgument(new IntLiteralNode(span, moduleId, -1));
    body->Statements().Insert(0, constructTracer);
}

} } // namespace cmajor::symbols
