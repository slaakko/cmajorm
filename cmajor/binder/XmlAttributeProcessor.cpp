// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/XmlAttributeProcessor.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Literal.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;
using namespace cmajor::symbols;

XmlAttributeProcessor::XmlAttributeProcessor() : AttributeProcessor(U"xml")
{
}

void XmlAttributeProcessor::TypeCheck(AttributeNode* attribute, Symbol* symbol)
{
    switch (symbol->GetSymbolType())
    {
        case SymbolType::classTypeSymbol: case SymbolType::classTemplateSpecializationSymbol: case SymbolType::memberVariableSymbol:
        {
            if (attribute->Value() == U"true" || attribute->Value() == U"false")
            {
                return;
            }
            else
            {
                throw Exception("unknown attribute value '" + ToUtf8(attribute->Value()) + "' for attribute '" + ToUtf8(attribute->Name()) + "'", attribute->GetSpan(), attribute->ModuleId());
            }
            break;
        }
    }
    AttributeProcessor::TypeCheck(attribute, symbol);
}

void XmlAttributeProcessor::GenerateSymbols(AttributeNode* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    if (symbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
        if (attribute->Value() == U"true")
        {
            GenerateToXmlSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateSystemDomElementConstructorSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
        }
    }
}

void XmlAttributeProcessor::GenerateImplementation(AttributeNode* attribute, Symbol* symbol, StatementBinder* statementBinder)
{
    if (symbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
        auto toXmlIt = toXmlMemberFunctionSymbolMap.find(symbol);
        if (toXmlIt != toXmlMemberFunctionSymbolMap.cend())
        {
            MemberFunctionSymbol* toXmlMemberFunctionSymbol = toXmlIt->second;
            GenerateToXmlMemberFunctionImplementation(attribute, classTypeSymbol, toXmlMemberFunctionSymbol, statementBinder);
        }
        else
        {
            throw Exception("internal error in XML attribute implementation: member function 'ToXml' symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan(), attribute->ModuleId());
        }
        auto contructorIt = constructorSymbolMap.find(symbol);
        if (contructorIt != constructorSymbolMap.cend())
        {
            ConstructorSymbol* constructorSymbol = contructorIt->second;
            GenerateSystemDomElementConstructorImplementation(attribute, classTypeSymbol, constructorSymbol, statementBinder);
        }
        else
        {
            throw Exception("internal error in XML attribute implementation: constructor symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan(), attribute->ModuleId());
        }
    }
}

void XmlAttributeProcessor::GenerateToXmlSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    MemberFunctionSymbol* toXmlMemberFunctionSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"ToXml");
    toXmlMemberFunctionSymbol->SetModule(&boundCompileUnit.GetModule());
    toXmlMemberFunctionSymbol->SetGroupName(U"ToXml");
    if (classTypeSymbol->BaseClass())
    {
        toXmlMemberFunctionSymbol->SetOverride();
    }
    else if (classTypeSymbol->Destructor() && classTypeSymbol->Destructor()->IsVirtual())
    {
        toXmlMemberFunctionSymbol->SetVirtual();
    }
    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(toXmlMemberFunctionSymbol);
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    ParameterSymbol* fieldNameParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"fieldName");
    IdentifierNode* stringIdNode(new IdentifierNode(attribute->GetSpan(), attribute->ModuleId(), U"System.string"));
    Node* refNode(new LValueRefNode(attribute->GetSpan(), attribute->ModuleId(), stringIdNode));
    std::unique_ptr<Node> constNode(new ConstNode(attribute->GetSpan(), attribute->ModuleId(), refNode));
    TypeSymbol* stringType = ResolveType(constNode.get(), boundCompileUnit, containerScope);
    fieldNameParam->SetType(stringType);
    TemplateIdNode templateId(attribute->GetSpan(), attribute->ModuleId(), new IdentifierNode(attribute->GetSpan(), attribute->ModuleId(), U"System.UniquePtr"));
    templateId.AddTemplateArgument(new IdentifierNode(attribute->GetSpan(), attribute->ModuleId(), U"System.Dom.Element"));
    TypeSymbol* uniquePtrElementType = ResolveType(&templateId, boundCompileUnit, containerScope);
    toXmlMemberFunctionSymbol->SetReturnType(uniquePtrElementType);
    ParameterSymbol* returnParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@return");
    returnParam->SetParent(toXmlMemberFunctionSymbol);
    returnParam->SetType(uniquePtrElementType->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    toXmlMemberFunctionSymbol->SetReturnParam(returnParam);
    toXmlMemberFunctionSymbol->SetAccess(SymbolAccess::public_);
    toXmlMemberFunctionSymbol->AddMember(thisParam);
    toXmlMemberFunctionSymbol->AddMember(fieldNameParam);
    classTypeSymbol->AddMember(toXmlMemberFunctionSymbol);
    toXmlMemberFunctionSymbol->ComputeName();
    toXmlMemberFunctionSymbolMap[classTypeSymbol] = toXmlMemberFunctionSymbol;
}

void XmlAttributeProcessor::GenerateSystemDomElementConstructorSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    ConstructorSymbol* constructorSymbol = new ConstructorSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@constructor");
    constructorSymbol->SetModule(&boundCompileUnit.GetModule());
    constructorSymbol->SetGroupName(U"@constructor");
    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(constructorSymbol);
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    ParameterSymbol* elementParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@element");
    std::unique_ptr<PointerNode> pointertNode(new PointerNode(attribute->GetSpan(), attribute->ModuleId(), new IdentifierNode(attribute->GetSpan(), attribute->ModuleId(), U"System.Dom.Element")));
    TypeSymbol* elementType = ResolveType(pointertNode.get(), boundCompileUnit, containerScope);
    elementParam->SetType(elementType);
    constructorSymbol->SetAccess(SymbolAccess::public_);
    constructorSymbol->AddMember(thisParam);
    constructorSymbol->AddMember(elementParam);
    classTypeSymbol->AddMember(constructorSymbol);
    constructorSymbol->ComputeName();
    constructorSymbolMap[classTypeSymbol] = constructorSymbol;
}

void XmlAttributeProcessor::GenerateToXmlMemberFunctionImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toXmlMemberFunctionSymbol,
    StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        Symbol* systemDomElement = GetRootModuleForCurrentThread()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Dom.Element");
        if (systemDomElement)
        {
            fileScope->AddContainerScope(systemDomElement->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), toXmlMemberFunctionSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        TemplateIdNode* uniquePtrSystemDomElement = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.UniquePtr"));
        uniquePtrSystemDomElement->AddTemplateArgument(new IdentifierNode(span, moduleId, U"System.Dom.Element"));
        ConstructionStatementNode* constructSystemDomElementStatement = new ConstructionStatementNode(span, moduleId, uniquePtrSystemDomElement, new IdentifierNode(span, moduleId, U"@element"));
        NewNode* newNode = new NewNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Dom.Element"));
        InvokeNode* toUtf32Node = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.ToUtf32"));
        toUtf32Node->AddArgument(new IdentifierNode(span, moduleId, U"fieldName"));
        newNode->AddArgument(toUtf32Node);
        constructSystemDomElementStatement->AddArgument(newNode);
        compoundStatementNode.AddStatement(constructSystemDomElementStatement);

        if (classTypeSymbol->BaseClass())
        {
            ClassTypeSymbol* baseClassSymbol = classTypeSymbol->BaseClass();
            AttributesNode* attributes = baseClassSymbol->GetAttributes();
            if (attributes)
            {
                AttributeNode* xmlAttribute = attributes->GetAttribute(U"xml");
                if (xmlAttribute)
                {
                    if (xmlAttribute->Value() == U"true")
                    {
                        Span span = classTypeSymbol->BaseClass()->GetSpan();
                        boost::uuids::uuid moduleId = classTypeSymbol->SourceModuleId();
                        BaseNode* baseNode = new BaseNode(span, moduleId);
                        ArrowNode* arrowNode = new ArrowNode(span, moduleId, baseNode, new IdentifierNode(span, moduleId, U"ToXml"));
                        InvokeNode* toXmlInvokeNode = new InvokeNode(span, moduleId, arrowNode);
                        toXmlInvokeNode->AddArgument(new StringLiteralNode(span, moduleId, "base"));
                        DotNode* dotNode = new DotNode(span, moduleId, toXmlInvokeNode, new IdentifierNode(span, moduleId, U"Release"));
                        InvokeNode* dotInvokeNode = new InvokeNode(span, moduleId, dotNode);
                        TemplateIdNode* uniquePtrNode = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.UniquePtr"));
                        uniquePtrNode->AddTemplateArgument(new IdentifierNode(span, moduleId, U"System.Dom.Node"));
                        InvokeNode* uniquePtrNodeInvokeNode = new InvokeNode(span, moduleId, uniquePtrNode);
                        uniquePtrNodeInvokeNode->AddArgument(dotInvokeNode);
                        ArrowNode* appendChildArrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"@element"), new IdentifierNode(span, moduleId, U"AppendChild"));
                        InvokeNode* appendChildInvokeNode = new InvokeNode(span, moduleId, appendChildArrowNode);
                        appendChildInvokeNode->AddArgument(uniquePtrNodeInvokeNode);
                        ExpressionStatementNode* appendChildStatement = new ExpressionStatementNode(span, moduleId, appendChildInvokeNode);
                        compoundStatementNode.AddStatement(appendChildStatement);
                    }
                }
            }
        }

        for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
        {
            AttributesNode* attributes = memberVariableSymbol->GetAttributes();
            if (attributes)
            {
                AttributeNode* xmlAttribute = attributes->GetAttribute(U"xml");
                if (xmlAttribute)
                {
                    if (xmlAttribute->Value() == U"false")
                    {
                        continue;
                    }
                }
            }
            InvokeNode* toXmlInvokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), new IdentifierNode(span, moduleId, U"ToXml"));
            toXmlInvokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), memberVariableSymbol->Name()));
            toXmlInvokeNode->AddArgument(new StringLiteralNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), ToUtf8(memberVariableSymbol->Name())));
            DotNode* dotNode = new DotNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), toXmlInvokeNode, new IdentifierNode(
                memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), U"Release"));
            InvokeNode* dotInvokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), dotNode);
            TemplateIdNode* uniquePtrNode = new TemplateIdNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(),
                new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), U"System.UniquePtr"));
            uniquePtrNode->AddTemplateArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(),
                U"System.Dom.Node"));
            InvokeNode* uniquePtrNodeInvokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), uniquePtrNode);
            uniquePtrNodeInvokeNode->AddArgument(dotInvokeNode);
            ArrowNode* appendChildArrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"@element"), new IdentifierNode(span, moduleId, U"AppendChild"));
            InvokeNode* appendChildInvokeNode = new InvokeNode(span, moduleId, appendChildArrowNode);
            appendChildInvokeNode->AddArgument(uniquePtrNodeInvokeNode);
            ExpressionStatementNode* appendChildStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), appendChildInvokeNode);
            compoundStatementNode.AddStatement(appendChildStatement);
        }

        TemplateIdNode* uniquePtrNode = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.UniquePtr"));
        uniquePtrNode->AddTemplateArgument(new IdentifierNode(span, moduleId, U"System.Dom.Element"));
        InvokeNode* invokeReleaseUniquePtrNode = new InvokeNode(span, moduleId, uniquePtrNode);
        invokeReleaseUniquePtrNode->AddArgument(new InvokeNode(span, moduleId, new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"@element"), 
            new IdentifierNode(span, moduleId, U"Release"))));
        ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, moduleId, invokeReleaseUniquePtrNode);
        compoundStatementNode.AddStatement(returnStatementNode);

        SymbolTable& symbolTable = statementBinder->GetBoundCompileUnit().GetSymbolTable();
        symbolTable.BeginContainer(toXmlMemberFunctionSymbol);
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        compoundStatementNode.Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(statementBinder->GetBoundCompileUnit());
        ContainerScope* containerScope = statementBinder->GetContainerScope();
        typeBinder.SetContainerScope(containerScope);
        statementBinder->SetContainerScope(toXmlMemberFunctionSymbol->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(toXmlMemberFunctionSymbol);
        compoundStatementNode.Accept(typeBinder);
        BoundFunction* prevFunction = statementBinder->CurrentFunction();
        statementBinder->SetCurrentFunction(boundFunction.get());
        compoundStatementNode.Accept(*statementBinder);
        statementBinder->SetContainerScope(containerScope);
        BoundStatement* boundStatement = statementBinder->ReleaseStatement();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        statementBinder->CurrentClass()->AddMember(std::move(boundFunction));
        statementBinder->SetCurrentFunction(prevFunction);
        statementBinder->GetBoundCompileUnit().RemoveLastFileScope();
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'UniquePtr<System.Dom.Element> ToXml(const string& fieldName)' member function for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateSystemDomElementConstructorImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, ConstructorSymbol* constructorSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), constructorSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        ConstructorNode constructorNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
        if (baseClass)
        {
            AttributesNode* attributes = baseClass->GetAttributes();
            if (attributes)
            {
                AttributeNode* xmlAttribute = attributes->GetAttribute(U"xml");
                if (xmlAttribute)
                {
                    if (xmlAttribute->Value() == U"true")
                    {
                        Span span = baseClass->GetSpan();
                        boost::uuids::uuid moduleId = classTypeSymbol->SourceModuleId();
                        InvokeNode* invokeNode = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"GetXmlFieldElement"));
                        invokeNode->AddArgument(new StringLiteralNode(span, moduleId, "base"));
                        invokeNode->AddArgument(new IdentifierNode(span, moduleId, U"@element"));
                        BaseInitializerNode* baseInitializer = new BaseInitializerNode(span, moduleId);
                        baseInitializer->AddArgument(invokeNode);
                        constructorNode.AddInitializer(baseInitializer);
                    }
                }
            }
        }

        for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
        {
            AttributesNode* attributes = memberVariableSymbol->GetAttributes();
            if (attributes)
            {
                AttributeNode* xmlAttribute = attributes->GetAttribute(U"xml");
                if (xmlAttribute)
                {
                    if (xmlAttribute->Value() == U"false")
                    {
                        continue;
                    }
                }
            }
            InvokeNode* invokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), new IdentifierNode(span, moduleId, U"FromXml"));
            invokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), U"@element"));
            invokeNode->AddArgument(new StringLiteralNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), ToUtf8(memberVariableSymbol->Name())));
            invokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), memberVariableSymbol->Name()));
            ExpressionStatementNode* fromXmlStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), invokeNode);
            compoundStatementNode.AddStatement(fromXmlStatement);
        }

        SymbolTable& symbolTable = statementBinder->GetBoundCompileUnit().GetSymbolTable();
        symbolTable.BeginContainer(constructorSymbol);
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        compoundStatementNode.Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(statementBinder->GetBoundCompileUnit());
        ContainerScope* containerScope = statementBinder->GetContainerScope();
        typeBinder.SetContainerScope(containerScope);
        statementBinder->SetContainerScope(constructorSymbol->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(constructorSymbol);
        compoundStatementNode.Accept(typeBinder);
        BoundFunction* prevFunction = statementBinder->CurrentFunction();
        statementBinder->SetCurrentFunction(boundFunction.get());
        ConstructorSymbol* prevConstructorSymbol = statementBinder->CurrentConstructorSymbol();
        ConstructorNode* prevConstructorNode = statementBinder->CurrentConstructorNode();
        statementBinder->SetCurrentConstructor(constructorSymbol, &constructorNode);
        compoundStatementNode.Accept(*statementBinder);
        statementBinder->SetContainerScope(containerScope);
        BoundStatement* boundStatement = statementBinder->ReleaseStatement();
        Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
        BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
        statementBinder->CurrentClass()->AddMember(std::move(boundFunction));
        statementBinder->SetCurrentConstructor(prevConstructorSymbol, prevConstructorNode);
        statementBinder->SetCurrentFunction(prevFunction);
        statementBinder->GetBoundCompileUnit().RemoveLastFileScope();
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create constructor taking System.Dom.Element* for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}
} } // namespace cmajor::binder
