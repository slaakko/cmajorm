// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/JsonAttributeProcessor.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Literal.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;
using namespace cmajor::symbols;

JsonAttributeProcessor::JsonAttributeProcessor(Module* module_) : AttributeProcessor(U"json"), module(module_)
{
}

void JsonAttributeProcessor::TypeCheck(Attribute* attribute, Symbol* symbol)
{
    switch (symbol->GetSymbolType())
    {
        case SymbolType::classTypeSymbol: case SymbolType::classTemplateSpecializationSymbol: case SymbolType::memberVariableSymbol:
        {
            if (attribute->Value() == U"true" || attribute->Value() == U"false")
            {
                if (symbol->IsClassTypeSymbol())
                {
                    ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
                    ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
                    if (baseClass)
                    {
                        bool baseClassHasJsonAttribute = false;
                        Attributes* baseClassAttributes = baseClass->GetAttributes();
                        if (baseClassAttributes)
                        {
                            Attribute* jsonAttribute = baseClassAttributes->GetAttribute(U"json");
                            if (jsonAttribute)
                            {
                                baseClassHasJsonAttribute = true;
                            }
                        }
                        if (!baseClassHasJsonAttribute)
                        {
                            Warning warning(module->GetCurrentProjectName(), "base class '" + ToUtf8(baseClass->FullName()) + "' of json-attributed class '" + 
                                ToUtf8(classTypeSymbol->FullName()) + "' does not explicitly declare 'json' attribute value to \"true\" or \"false\"");
                            warning.SetModule(GetRootModuleForCurrentThread());
                            warning.SetDefined(classTypeSymbol->GetSpan());
                            std::vector<Span> references;
                            references.push_back(baseClass->GetSpan());
                            warning.SetReferences(references);
                            module->WarningCollection().AddWarning(warning);
                        }
                    }
                }
                return;
            }
            else
            {
                throw Exception(module, "unknown attribute value '" + ToUtf8(attribute->Value()) + "' for attribute '" + ToUtf8(attribute->Name()) + "'", attribute->GetSpan());
            }
        }
    }
    AttributeProcessor::TypeCheck(attribute, symbol);
}

void JsonAttributeProcessor::GenerateSymbols(Attribute* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    if (symbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
        if (attribute->Value() == U"true")
        {
            GenerateMemberVariableJsonFieldNames(classTypeSymbol);
            std::map<std::u32string, Attribute*> memberVariableFieldNames;
            CheckMemberVariableJsonFieldNames(classTypeSymbol, memberVariableFieldNames);
            GenerateJsonCreatorFunctionSymbol(attribute, classTypeSymbol);
            GenerateJsonConstructorSymbol(attribute, classTypeSymbol);
            GenerateToJsonJsonObjectSymbol(attribute, classTypeSymbol);
            GenerateToJsonSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
        }
    }
}

void JsonAttributeProcessor::CheckMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol, std::map<std::u32string, Attribute*>& memberVariableFieldNames)
{
    if (classTypeSymbol->BaseClass())
    {
        CheckMemberVariableJsonFieldNames(classTypeSymbol->BaseClass(), memberVariableFieldNames);
    }
    for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        Attributes* attributes = memberVariableSymbol->GetAttributes();
        if (attributes)
        {
            Attribute* jsonAttribute = attributes->GetAttribute(U"json");
            if (jsonAttribute)
            {
                if (jsonAttribute->Value() == U"false")
                {
                    continue;
                }
            }
            Attribute* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
            if (jsonFieldNameAttribute)
            {
                auto it = memberVariableFieldNames.find(jsonFieldNameAttribute->Value());
                if (it != memberVariableFieldNames.cend())
                {
                    Attribute* prev = it->second;
                    throw Exception(module, "error in JSON field name generation: 'jsonFieldName' attribute not unique among member variable names of the current class and its base classes",
                        jsonFieldNameAttribute->GetSpan(), prev->GetSpan());
                }
                memberVariableFieldNames[jsonFieldNameAttribute->Value()] = jsonFieldNameAttribute;
            }
            else
            {
                throw Exception(module, "internal error in JSON field name generation: 'jsonFieldName' attribute not found", memberVariableSymbol->GetSpan());
            }
        }
        else
        {
            throw Exception(module, "internal error in JSON field name generation: attributes not found", memberVariableSymbol->GetSpan());
        }
    }
}

void JsonAttributeProcessor::GenerateMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol)
{
    for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        bool hasJsonFieldAttribute = false;
        Attributes* attributes = memberVariableSymbol->GetAttributes();
        if (attributes)
        {
            Attribute* jsonAttribute = attributes->GetAttribute(U"json");
            if (jsonAttribute)
            {
                if (jsonAttribute->Value() == U"false")
                {
                    continue;
                }
            }
            Attribute* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
            if (jsonFieldNameAttribute)
            {
                hasJsonFieldAttribute = true;
            }
        }
        else
        {
            memberVariableSymbol->SetAttributes(std::unique_ptr<Attributes>(new Attributes()));
            attributes = memberVariableSymbol->GetAttributes();
        }
        if (!hasJsonFieldAttribute)
        {
            attributes->AddAttribute(memberVariableSymbol->GetSpan(), U"jsonFieldName", memberVariableSymbol->Name());
        }
    }
}

void JsonAttributeProcessor::GenerateJsonCreatorFunctionSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol)
{
    MemberFunctionSymbol* jsonCreatorFunctionSymbol = new MemberFunctionSymbol(attribute->GetSpan(), U"Create");
    jsonCreatorFunctionSymbol->SetGroupName(U"Create");
    jsonCreatorFunctionSymbol->SetModule(module);
    module->GetSymbolTable().SetFunctionIdFor(jsonCreatorFunctionSymbol);
    jsonCreatorFunctionSymbol->SetAccess(SymbolAccess::public_);
    jsonCreatorFunctionSymbol->SetStatic();
    Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
    if (!jsonValue || jsonValue->GetSymbolType() != SymbolType::classGroupTypeSymbol)
    {
        throw Exception(module, "System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), classTypeSymbol->GetSpan());
    }
    ClassGroupTypeSymbol* jsonValueGroup = static_cast<ClassGroupTypeSymbol*>(jsonValue);
    ClassTypeSymbol* jsonValueClass = jsonValueGroup->GetClass(0);
    if (!jsonValueClass)
    {
        throw Exception(module, "System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), classTypeSymbol->GetSpan());
    }
    ParameterSymbol* jsonValueParam = new ParameterSymbol(attribute->GetSpan(), U"@value");
    jsonValueParam->SetType(jsonValueClass->AddPointer(attribute->GetSpan()));
    jsonCreatorFunctionSymbol->AddMember(jsonValueParam);
    jsonCreatorFunctionSymbol->SetReturnType(module->GetSymbolTable().GetTypeByName(U"void")->AddPointer(attribute->GetSpan()));
    classTypeSymbol->AddMember(jsonCreatorFunctionSymbol);
    jsonCreatorFunctionSymbol->ComputeName();
    jsonCreatorMap[classTypeSymbol] = jsonCreatorFunctionSymbol;
}

void JsonAttributeProcessor::GenerateJsonConstructorSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol)
{
    ConstructorSymbol* jsonConstructorSymbol = new ConstructorSymbol(attribute->GetSpan(), U"@constructor");
    jsonConstructorSymbol->SetModule(module);
    module->GetSymbolTable().SetFunctionIdFor(jsonConstructorSymbol);
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan()));
    ParameterSymbol* jsonValueParam = new ParameterSymbol(attribute->GetSpan(), U"@value");
    Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
    if (!jsonValue || jsonValue->GetSymbolType() != SymbolType::classGroupTypeSymbol)
    {
        throw Exception(module, "System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), classTypeSymbol->GetSpan());
    }
    ClassGroupTypeSymbol* jsonValueGroup = static_cast<ClassGroupTypeSymbol*>(jsonValue);
    ClassTypeSymbol* jsonValueClass = jsonValueGroup->GetClass(0);
    if (!jsonValueClass)
    {
        throw Exception(module, "System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), classTypeSymbol->GetSpan());
    }
    jsonValueParam->SetType(jsonValueClass->AddPointer(attribute->GetSpan()));
    jsonConstructorSymbol->SetAccess(SymbolAccess::public_);
    jsonConstructorSymbol->SetExplicit();
    jsonConstructorSymbol->AddMember(thisParam);
    jsonConstructorSymbol->AddMember(jsonValueParam);
    classTypeSymbol->AddMember(jsonConstructorSymbol);
    jsonConstructorSymbol->ComputeName();
    jsonConstructorMap[classTypeSymbol] = jsonConstructorSymbol;
}

void JsonAttributeProcessor::GenerateToJsonJsonObjectSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol)
{
    MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol = new MemberFunctionSymbol(attribute->GetSpan(), U"ToJson");
    toJsonJsonObjectMemberFunctionSymbol->SetModule(module);
    toJsonJsonObjectMemberFunctionSymbol->SetGroupName(U"ToJson");
    ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
    bool jsonBase = false;
    if (baseClass)
    {
        Attributes* attributes = baseClass->GetAttributes();
        if (attributes)
        {
            Attribute* jsonAttribute = attributes->GetAttribute(U"json");
            if (jsonAttribute)
            {
                if (jsonAttribute->Value() == U"true")
                {
                    jsonBase = true;
                }
            }
        }
    }
    if (!jsonBase)
    {
        toJsonJsonObjectMemberFunctionSymbol->SetVirtual();
    }
    else
    {
        toJsonJsonObjectMemberFunctionSymbol->SetOverride();
    }
    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(toJsonJsonObjectMemberFunctionSymbol);
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan()));
    ParameterSymbol* jsonObjectParam = new ParameterSymbol(attribute->GetSpan(), U"@object");
    Symbol* jsonObject = GetRootModuleForCurrentThread()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonObject");
    if (!jsonObject || jsonObject->GetSymbolType() != SymbolType::classGroupTypeSymbol)
    {
        throw Exception(module, "System.Json.JsonObject class not found from the symbol table", attribute->GetSpan(), classTypeSymbol->GetSpan());
    }
    ClassGroupTypeSymbol* jsonObjectGroup = static_cast<ClassGroupTypeSymbol*>(jsonObject);
    ClassTypeSymbol* jsonObjectClass = jsonObjectGroup->GetClass(0);
    if (!jsonObjectClass)
    {
        throw Exception(module, "System.Json.JsonObject class not found from the symbol table", attribute->GetSpan(), classTypeSymbol->GetSpan());
    }
    jsonObjectParam->SetType(jsonObjectClass->AddPointer(attribute->GetSpan()));
    toJsonJsonObjectMemberFunctionSymbol->SetReturnType(module->GetSymbolTable().GetTypeByName(U"void"));
    toJsonJsonObjectMemberFunctionSymbol->SetAccess(SymbolAccess::public_);
    toJsonJsonObjectMemberFunctionSymbol->AddMember(thisParam);
    toJsonJsonObjectMemberFunctionSymbol->AddMember(jsonObjectParam);
    classTypeSymbol->AddMember(toJsonJsonObjectMemberFunctionSymbol);
    toJsonJsonObjectMemberFunctionSymbol->ComputeName();
    toJsonJsonObjectMemberFunctionSymbolMap[classTypeSymbol] = toJsonJsonObjectMemberFunctionSymbol;
}

void JsonAttributeProcessor::GenerateToJsonSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    MemberFunctionSymbol* toJsonMemberFunctionSymbol = new MemberFunctionSymbol(attribute->GetSpan(), U"ToJson");
    toJsonMemberFunctionSymbol->SetModule(module);
    toJsonMemberFunctionSymbol->SetGroupName(U"ToJson");
    ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
    bool jsonBase = false;
    if (baseClass)
    {
        Attributes* attributes = baseClass->GetAttributes();
        if (attributes)
        {
            Attribute* jsonAttribute = attributes->GetAttribute(U"json");
            if (jsonAttribute)
            {
                if (jsonAttribute->Value() == U"true")
                {
                    jsonBase = true;
                }
            }
        }
    }
    if (!jsonBase)
    {
        toJsonMemberFunctionSymbol->SetVirtual();
    }
    else
    {
        toJsonMemberFunctionSymbol->SetOverride();
    }
    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(toJsonMemberFunctionSymbol);
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan()));
    TemplateIdNode templateId(attribute->GetSpan(), new IdentifierNode(attribute->GetSpan(), U"System.UniquePtr"));
    templateId.AddTemplateArgument(new IdentifierNode(attribute->GetSpan(), U"System.Json.JsonValue"));
    TypeSymbol* uniquePtrJsonValueType = ResolveType(&templateId, boundCompileUnit, containerScope);
    toJsonMemberFunctionSymbol->SetReturnType(uniquePtrJsonValueType);
    ParameterSymbol* returnParam = new ParameterSymbol(attribute->GetSpan(), U"@return");
    returnParam->SetParent(toJsonMemberFunctionSymbol);
    returnParam->SetType(uniquePtrJsonValueType->AddPointer(attribute->GetSpan()));
    toJsonMemberFunctionSymbol->SetReturnParam(returnParam);
    toJsonMemberFunctionSymbol->SetAccess(SymbolAccess::public_);
    toJsonMemberFunctionSymbol->AddMember(thisParam);
    classTypeSymbol->AddMember(toJsonMemberFunctionSymbol);
    toJsonMemberFunctionSymbol->ComputeName();
    toJsonObjectMemberFunctionSymbolMap[classTypeSymbol] = toJsonMemberFunctionSymbol;
}

void JsonAttributeProcessor::GenerateImplementation(Attribute* attribute, Symbol* symbol, StatementBinder* statementBinder)
{
    if (symbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
        auto it = jsonConstructorMap.find(symbol);
        if (it != jsonConstructorMap.cend())
        {
            ConstructorSymbol* jsonConstructorSymbol = it->second;
            GenerateJsonConstructorImplementation(attribute, classTypeSymbol, jsonConstructorSymbol, statementBinder);
        }
        else
        {
            throw Exception(module, "internal error in JSON attribute implementation: constructor symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan());
        }
        auto it1 = jsonCreatorMap.find(symbol);
        if (it1 != jsonCreatorMap.cend())
        {
            MemberFunctionSymbol* jsonCreatorSymbol = it1->second;
            GenerateJsonCreatorImplementation(attribute, classTypeSymbol, jsonCreatorSymbol, statementBinder);
        }
        else
        {
            throw Exception(module, "internal error in JSON attribute implementation: Creator function symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan());
        }
        auto it2 = toJsonJsonObjectMemberFunctionSymbolMap.find(symbol);
        if (it2 != toJsonJsonObjectMemberFunctionSymbolMap.cend())
        {
            MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol = it2->second;
            GenerateToJsonJsonObjectImplementation(attribute, classTypeSymbol, toJsonJsonObjectMemberFunctionSymbol, statementBinder);
        }
        else
        {
            throw Exception(module, "internal error in JSON attribute implementation: member function 'ToJson' symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan());
        }
        auto it3 = toJsonObjectMemberFunctionSymbolMap.find(symbol);
        if (it3 != toJsonObjectMemberFunctionSymbolMap.cend())
        {
            MemberFunctionSymbol* toJsonMemberFunctionSymbol = it3->second;
            GenerateToJsonImplementation(attribute, classTypeSymbol, toJsonMemberFunctionSymbol, statementBinder);
        }
        else
        {
            throw Exception(module, "internal error in JSON attribute implementation: member function 'ToJson' symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan());
        }
        SymbolTable& symbolTable = module->GetSymbolTable();
        symbolTable.AddJsonClass(classTypeSymbol->FullName());
    }
}

void JsonAttributeProcessor::GenerateJsonCreatorImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* jsonCreatorFunctionSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope(&statementBinder->GetBoundCompileUnit().GetModule());
        Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
        if (jsonValue)
        {
            fileScope->AddContainerScope(jsonValue->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, jsonCreatorFunctionSymbol));
        const Span& span = attribute->GetSpan();
        CompoundStatementNode compoundStatementNode(span);
        compoundStatementNode.SetEndBraceSpan(span);
        NewNode* newNode = new NewNode(span, new IdentifierNode(span, classTypeSymbol->FullName()));
        newNode->AddArgument(new IdentifierNode(span, U"@value"));
        ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, newNode);
        compoundStatementNode.AddStatement(returnStatementNode);
        SymbolTable& symbolTable = statementBinder->GetBoundCompileUnit().GetSymbolTable();
        symbolTable.BeginContainer(jsonCreatorFunctionSymbol);
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        compoundStatementNode.Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(statementBinder->GetBoundCompileUnit());
        ContainerScope* containerScope = statementBinder->GetContainerScope();
        typeBinder.SetContainerScope(containerScope);
        statementBinder->SetContainerScope(jsonCreatorFunctionSymbol->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(jsonCreatorFunctionSymbol);
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
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "error in JSON attribute generation: could not create JSON Create() function for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), references);
    }
}

void JsonAttributeProcessor::GenerateJsonConstructorImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, ConstructorSymbol* jsonConstructorSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope(&statementBinder->GetBoundCompileUnit().GetModule());
        Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
        if (jsonValue)
        {
            fileScope->AddContainerScope(jsonValue->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, jsonConstructorSymbol));
        const Span& span = attribute->GetSpan();
        ConstructorNode constructorNode(span);
        CompoundStatementNode compoundStatementNode(span);
        compoundStatementNode.SetEndBraceSpan(span);
        ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
        if (baseClass)
        {
            Attributes* attributes = baseClass->GetAttributes();
            if (attributes)
            {
                Attribute* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"true")
                    {
                        BaseInitializerNode* baseInitializer = new BaseInitializerNode(span);
                        baseInitializer->AddArgument(new IdentifierNode(span, U"@value"));
                        constructorNode.AddInitializer(baseInitializer);
                    }
                }
            }
        }
        for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
        {
            std::u32string jsonFieldName = memberVariableSymbol->Name();
            Attributes* attributes = memberVariableSymbol->GetAttributes();
            if (attributes)
            {
                Attribute* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"false")
                    {
                        continue;
                    }
                }
                Attribute* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
                if (jsonFieldNameAttribute)
                {
                    jsonFieldName = jsonFieldNameAttribute->Value();
                }
            }
            InvokeNode* invokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), new IdentifierNode(span, U"FromJson"));
            invokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), U"@value"));
            invokeNode->AddArgument(new UStringLiteralNode(memberVariableSymbol->GetSpan(), jsonFieldName));
            invokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->Name()));
            ExpressionStatementNode* fromJsonStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), invokeNode);
            compoundStatementNode.AddStatement(fromJsonStatement);
        }
        SymbolTable& symbolTable = statementBinder->GetBoundCompileUnit().GetSymbolTable();
        symbolTable.BeginContainer(jsonConstructorSymbol);
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        compoundStatementNode.Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(statementBinder->GetBoundCompileUnit());
        ContainerScope* containerScope = statementBinder->GetContainerScope();
        typeBinder.SetContainerScope(containerScope);
        statementBinder->SetContainerScope(jsonConstructorSymbol->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(jsonConstructorSymbol);
        compoundStatementNode.Accept(typeBinder);
        BoundFunction* prevFunction = statementBinder->CurrentFunction();
        statementBinder->SetCurrentFunction(boundFunction.get());
        ConstructorSymbol* prevConstructorSymbol = statementBinder->CurrentConstructorSymbol();
        ConstructorNode* prevConstructorNode = statementBinder->CurrentConstructorNode();
        statementBinder->SetCurrentConstructor(jsonConstructorSymbol, &constructorNode);
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
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "error in JSON attribute generation: could not create JSON constructor for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), references);
    }
}

void JsonAttributeProcessor::GenerateToJsonJsonObjectImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope(&statementBinder->GetBoundCompileUnit().GetModule());
        Symbol* jsonObject = GetRootModuleForCurrentThread()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonObject");
        if (jsonObject)
        {
            fileScope->AddContainerScope(jsonObject->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, toJsonJsonObjectMemberFunctionSymbol));
        const Span& span = attribute->GetSpan();
        CompoundStatementNode compoundStatementNode(span);
        compoundStatementNode.SetEndBraceSpan(span);
        ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
        if (baseClass)
        {
            Attributes* attributes = baseClass->GetAttributes();
            if (attributes)
            {
                Attribute* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"true")
                    {
                        BaseNode* baseNode = new BaseNode(span);
                        ArrowNode* arrowNode = new ArrowNode(span, baseNode, new IdentifierNode(span, U"ToJson"));
                        InvokeNode* toJsonInvokeNode = new InvokeNode(span, arrowNode);
                        toJsonInvokeNode->AddArgument(new IdentifierNode(span, U"@object"));
                        ExpressionStatementNode* toJsonStatement = new ExpressionStatementNode(span, toJsonInvokeNode);
                        compoundStatementNode.AddStatement(toJsonStatement);
                    }
                }
            }
        }
        for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
        {
            std::u32string jsonFieldName = memberVariableSymbol->Name();
            Attributes* attributes = memberVariableSymbol->GetAttributes();
            if (attributes)
            {
                Attribute* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"false")
                    {
                        continue;
                    }
                }
                Attribute* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
                if (jsonFieldNameAttribute)
                {
                    jsonFieldName = jsonFieldNameAttribute->Value();
                }
            }
            InvokeNode* toJsonInvokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), new IdentifierNode(span, U"ToJson"));
            toJsonInvokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->Name()));
            ArrowNode* arrowNode = new ArrowNode(span, new IdentifierNode(span, U"@object"), new IdentifierNode(span, U"AddField"));
            InvokeNode* addFieldInvokeNode = new InvokeNode(span, arrowNode);
            addFieldInvokeNode->AddArgument(new UStringLiteralNode(memberVariableSymbol->GetSpan(), jsonFieldName));
            addFieldInvokeNode->AddArgument(toJsonInvokeNode);
            ExpressionStatementNode* addFieldStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), addFieldInvokeNode);
            compoundStatementNode.AddStatement(addFieldStatement);
        }
        SymbolTable& symbolTable = statementBinder->GetBoundCompileUnit().GetSymbolTable();
        symbolTable.BeginContainer(toJsonJsonObjectMemberFunctionSymbol);
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        compoundStatementNode.Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(statementBinder->GetBoundCompileUnit());
        ContainerScope* containerScope = statementBinder->GetContainerScope();
        typeBinder.SetContainerScope(containerScope);
        statementBinder->SetContainerScope(toJsonJsonObjectMemberFunctionSymbol->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(toJsonJsonObjectMemberFunctionSymbol);
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
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "error in JSON attribute generation: could not create 'void ToJson(JsobObject*)' member function for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), references);
    }
}

void JsonAttributeProcessor::GenerateToJsonImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonMemberFunctionSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope(&statementBinder->GetBoundCompileUnit().GetModule());
        Symbol* jsonObject = GetRootModuleForCurrentThread()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonObject");
        if (jsonObject)
        {
            fileScope->AddContainerScope(jsonObject->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, toJsonMemberFunctionSymbol));
        const Span& span = attribute->GetSpan();
        CompoundStatementNode compoundStatementNode(span);
        compoundStatementNode.SetEndBraceSpan(span);
        TemplateIdNode* uniquePtrJsonObject = new TemplateIdNode(span, new IdentifierNode(span, U"UniquePtr"));
        uniquePtrJsonObject->AddTemplateArgument(new IdentifierNode(span, U"JsonObject"));
        ConstructionStatementNode* constructJsonObjectStatement = new ConstructionStatementNode(span, uniquePtrJsonObject, new IdentifierNode(span, U"@object"));
        constructJsonObjectStatement->AddArgument(new NewNode(span, new IdentifierNode(span, U"JsonObject")));
        compoundStatementNode.AddStatement(constructJsonObjectStatement);
        InvokeNode* invokeToJson = new InvokeNode(span, new IdentifierNode(span, U"ToJson"));
        invokeToJson->AddArgument(new InvokeNode(span, new DotNode(span, new IdentifierNode(span, U"@object"), new IdentifierNode(span, U"Get"))));
        ExpressionStatementNode* callToJsonStatement = new ExpressionStatementNode(span, invokeToJson);
        compoundStatementNode.AddStatement(callToJsonStatement);
        TemplateIdNode* uniquePtrJsonValue = new TemplateIdNode(span, new IdentifierNode(span, U"UniquePtr"));
        uniquePtrJsonValue->AddTemplateArgument(new IdentifierNode(span, U"JsonValue"));
        InvokeNode* invokeJsonValue = new InvokeNode(span, uniquePtrJsonValue);
        invokeJsonValue->AddArgument(new InvokeNode(span, new DotNode(span, new IdentifierNode(span, U"@object"), new IdentifierNode(span, U"Release"))));
        ReturnStatementNode* returnStatement = new ReturnStatementNode(span, invokeJsonValue);
        compoundStatementNode.AddStatement(returnStatement);
        SymbolTable& symbolTable = statementBinder->GetBoundCompileUnit().GetSymbolTable();
        symbolTable.BeginContainer(toJsonMemberFunctionSymbol);
        SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
        compoundStatementNode.Accept(symbolCreatorVisitor);
        symbolTable.EndContainer();
        TypeBinder typeBinder(statementBinder->GetBoundCompileUnit());
        ContainerScope* containerScope = statementBinder->GetContainerScope();
        typeBinder.SetContainerScope(containerScope);
        statementBinder->SetContainerScope(toJsonMemberFunctionSymbol->GetContainerScope());
        typeBinder.SetCurrentFunctionSymbol(toJsonMemberFunctionSymbol);
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
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "error in JSON attribute generation: could not create 'UniquePtr<JsonValue> ToJson()' member function for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), references);
    }
}

JsonFieldNameAttributeProcessor::JsonFieldNameAttributeProcessor(Module* module_) : AttributeProcessor(U"jsonFieldName"), module(module_)
{
}

void JsonFieldNameAttributeProcessor::TypeCheck(Attribute* attribute, Symbol* symbol)
{
    if (symbol->GetSymbolType() == SymbolType::memberVariableSymbol)
    {
        if (attribute->Value().empty())
        {
            throw Exception(module, "attribute value '" + ToUtf8(attribute->Value()) + "' for attribute '" + ToUtf8(attribute->Name()) + "' cannot be empty string", attribute->GetSpan());
        }
        return;
    }
    AttributeProcessor::TypeCheck(attribute, symbol);
}    

} } // namespace cmajor::binder
