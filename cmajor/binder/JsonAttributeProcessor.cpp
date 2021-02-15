// =================================
// Copyright (c) 2021 Seppo Laakko
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

void JsonAttributeProcessor::TypeCheck(AttributeNode* attribute, Symbol* symbol)
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
                        AttributesNode* baseClassAttributes = baseClass->GetAttributes();
                        if (baseClassAttributes)
                        {
                            AttributeNode* jsonAttribute = baseClassAttributes->GetAttribute(U"json");
                            if (jsonAttribute)
                            {
                                baseClassHasJsonAttribute = true;
                            }
                        }
                        if (!baseClassHasJsonAttribute)
                        {
                            Warning warning(module->GetCurrentProjectName(), "base class '" + ToUtf8(baseClass->FullName()) + "' of json-attributed class '" + 
                                ToUtf8(classTypeSymbol->FullName()) + "' does not explicitly declare 'json' attribute value to \"true\" or \"false\"");
                            warning.SetDefined(classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
                            std::vector<std::pair<Span, boost::uuids::uuid>> references;
                            references.push_back(std::make_pair(baseClass->GetSpan(), baseClass->SourceModuleId()));
                            warning.SetReferences(references);
                            module->WarningCollection().AddWarning(warning);
                        }
                    }
                }
                return;
            }
            else
            {
                throw Exception("unknown attribute value '" + ToUtf8(attribute->Value()) + "' for attribute '" + ToUtf8(attribute->Name()) + "'", attribute->GetSpan(), attribute->ModuleId());
            }
        }
    }
    AttributeProcessor::TypeCheck(attribute, symbol);
}

void JsonAttributeProcessor::GenerateSymbols(AttributeNode* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    if (symbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
        if (attribute->Value() == U"true")
        {
            GenerateMemberVariableJsonFieldNames(classTypeSymbol);
            std::map<std::u32string, AttributeNode*> memberVariableFieldNames;
            CheckMemberVariableJsonFieldNames(classTypeSymbol, memberVariableFieldNames);
            GenerateJsonCreatorFunctionSymbol(attribute, classTypeSymbol);
            GenerateJsonConstructorSymbol(attribute, classTypeSymbol);
            GenerateToJsonJsonObjectSymbol(attribute, classTypeSymbol);
            GenerateToJsonSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
        }
    }
}

void JsonAttributeProcessor::CheckMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol, std::map<std::u32string, AttributeNode*>& memberVariableFieldNames)
{
    if (classTypeSymbol->BaseClass())
    {
        AttributesNode* attributes = classTypeSymbol->BaseClass()->GetAttributes();
        AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
        if (jsonAttribute && jsonAttribute->Value() != U"false")
        {
            CheckMemberVariableJsonFieldNames(classTypeSymbol->BaseClass(), memberVariableFieldNames);
        }
    }
    for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        AttributesNode* attributes = memberVariableSymbol->GetAttributes();
        if (attributes)
        {
            AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
            if (jsonAttribute)
            {
                if (jsonAttribute->Value() == U"false")
                {
                    continue;
                }
            }
            AttributeNode* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
            if (jsonFieldNameAttribute)
            {
                auto it = memberVariableFieldNames.find(jsonFieldNameAttribute->Value());
                if (it != memberVariableFieldNames.cend())
                {
                    AttributeNode* prev = it->second;
                    throw Exception("error in JSON field name generation: 'jsonFieldName' attribute not unique among member variable names of the current class and its base classes",
                        jsonFieldNameAttribute->GetSpan(), jsonFieldNameAttribute->ModuleId(), prev->GetSpan(), prev->ModuleId());
                }
                memberVariableFieldNames[jsonFieldNameAttribute->Value()] = jsonFieldNameAttribute;
            }
            else
            {
                throw Exception("internal error in JSON field name generation: 'jsonFieldName' attribute not found", memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId());
            }
        }
        else
        {
            throw Exception("internal error in JSON field name generation: attributes not found", memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId());
        }
    }
}

void JsonAttributeProcessor::GenerateMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol)
{
    for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
    {
        bool hasJsonFieldAttribute = false;
        AttributesNode* attributes = memberVariableSymbol->GetAttributes();
        if (attributes)
        {
            AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
            if (jsonAttribute)
            {
                if (jsonAttribute->Value() == U"false")
                {
                    continue;
                }
            }
            AttributeNode* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
            if (jsonFieldNameAttribute)
            {
                hasJsonFieldAttribute = true;
            }
        }
        else
        {
            memberVariableSymbol->SetAttributes(std::unique_ptr<AttributesNode>(new AttributesNode(classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId())));
            attributes = memberVariableSymbol->GetAttributes();
        }
        if (!hasJsonFieldAttribute)
        {
            attributes->AddAttribute(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), U"jsonFieldName", memberVariableSymbol->Name());
        }
    }
}

void JsonAttributeProcessor::GenerateJsonCreatorFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol)
{
    MemberFunctionSymbol* jsonCreatorFunctionSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"Create");
    jsonCreatorFunctionSymbol->SetGroupName(U"Create");
    jsonCreatorFunctionSymbol->SetModule(module);
    module->GetSymbolTable().SetFunctionIdFor(jsonCreatorFunctionSymbol);
    jsonCreatorFunctionSymbol->SetAccess(SymbolAccess::public_);
    jsonCreatorFunctionSymbol->SetStatic();
    Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
    if (!jsonValue || jsonValue->GetSymbolType() != SymbolType::classGroupTypeSymbol)
    {
        throw Exception("System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), attribute->ModuleId(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
    }
    ClassGroupTypeSymbol* jsonValueGroup = static_cast<ClassGroupTypeSymbol*>(jsonValue);
    ClassTypeSymbol* jsonValueClass = jsonValueGroup->GetClass(0);
    if (!jsonValueClass)
    {
        throw Exception("System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), attribute->ModuleId(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
    }
    ParameterSymbol* jsonValueParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@value");
    jsonValueParam->SetType(jsonValueClass->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    jsonCreatorFunctionSymbol->AddMember(jsonValueParam);
    jsonCreatorFunctionSymbol->SetReturnType(module->GetSymbolTable().GetTypeByName(U"void")->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    classTypeSymbol->AddMember(jsonCreatorFunctionSymbol);
    jsonCreatorFunctionSymbol->ComputeName();
    jsonCreatorMap[classTypeSymbol] = jsonCreatorFunctionSymbol;
}

void JsonAttributeProcessor::GenerateJsonConstructorSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol)
{
    ConstructorSymbol* jsonConstructorSymbol = new ConstructorSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@constructor");
    jsonConstructorSymbol->SetModule(module);
    module->GetSymbolTable().SetFunctionIdFor(jsonConstructorSymbol);
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    ParameterSymbol* jsonValueParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@value");
    Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
    if (!jsonValue || jsonValue->GetSymbolType() != SymbolType::classGroupTypeSymbol)
    {
        throw Exception("System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), attribute->ModuleId(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
    }
    ClassGroupTypeSymbol* jsonValueGroup = static_cast<ClassGroupTypeSymbol*>(jsonValue);
    ClassTypeSymbol* jsonValueClass = jsonValueGroup->GetClass(0);
    if (!jsonValueClass)
    {
        throw Exception("System.Json.JsonValue class not found from the symbol table", attribute->GetSpan(), attribute->ModuleId(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
    }
    jsonValueParam->SetType(jsonValueClass->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    jsonConstructorSymbol->SetAccess(SymbolAccess::public_);
    jsonConstructorSymbol->SetExplicit();
    jsonConstructorSymbol->AddMember(thisParam);
    jsonConstructorSymbol->AddMember(jsonValueParam);
    classTypeSymbol->AddMember(jsonConstructorSymbol);
    jsonConstructorSymbol->ComputeName();
    jsonConstructorMap[classTypeSymbol] = jsonConstructorSymbol;
}

void JsonAttributeProcessor::GenerateToJsonJsonObjectSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol)
{
    MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"ToJson");
    toJsonJsonObjectMemberFunctionSymbol->SetModule(module);
    toJsonJsonObjectMemberFunctionSymbol->SetGroupName(U"ToJson");
    ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
    bool jsonBase = false;
    if (baseClass)
    {
        AttributesNode* attributes = baseClass->GetAttributes();
        if (attributes)
        {
            AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
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
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    ParameterSymbol* jsonObjectParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@object");
    Symbol* jsonObject = GetRootModuleForCurrentThread()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonObject");
    if (!jsonObject || jsonObject->GetSymbolType() != SymbolType::classGroupTypeSymbol)
    {
        throw Exception("System.Json.JsonObject class not found from the symbol table", attribute->GetSpan(), attribute->ModuleId(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
    }
    ClassGroupTypeSymbol* jsonObjectGroup = static_cast<ClassGroupTypeSymbol*>(jsonObject);
    ClassTypeSymbol* jsonObjectClass = jsonObjectGroup->GetClass(0);
    if (!jsonObjectClass)
    {
        throw Exception("System.Json.JsonObject class not found from the symbol table", attribute->GetSpan(), attribute->ModuleId(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
    }
    jsonObjectParam->SetType(jsonObjectClass->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    toJsonJsonObjectMemberFunctionSymbol->SetReturnType(module->GetSymbolTable().GetTypeByName(U"void"));
    toJsonJsonObjectMemberFunctionSymbol->SetAccess(SymbolAccess::public_);
    toJsonJsonObjectMemberFunctionSymbol->AddMember(thisParam);
    toJsonJsonObjectMemberFunctionSymbol->AddMember(jsonObjectParam);
    classTypeSymbol->AddMember(toJsonJsonObjectMemberFunctionSymbol);
    toJsonJsonObjectMemberFunctionSymbol->ComputeName();
    toJsonJsonObjectMemberFunctionSymbolMap[classTypeSymbol] = toJsonJsonObjectMemberFunctionSymbol;
}

void JsonAttributeProcessor::GenerateToJsonSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    MemberFunctionSymbol* toJsonMemberFunctionSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"ToJson");
    toJsonMemberFunctionSymbol->SetModule(module);
    toJsonMemberFunctionSymbol->SetGroupName(U"ToJson");
    ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
    bool jsonBase = false;
    if (baseClass)
    {
        AttributesNode* attributes = baseClass->GetAttributes();
        if (attributes)
        {
            AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
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
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    TemplateIdNode templateId(attribute->GetSpan(), attribute->ModuleId(), new IdentifierNode(attribute->GetSpan(), attribute->ModuleId(), U"System.UniquePtr"));
    templateId.AddTemplateArgument(new IdentifierNode(attribute->GetSpan(), attribute->ModuleId(), U"System.Json.JsonValue"));
    TypeSymbol* uniquePtrJsonValueType = ResolveType(&templateId, boundCompileUnit, containerScope);
    toJsonMemberFunctionSymbol->SetReturnType(uniquePtrJsonValueType);
    ParameterSymbol* returnParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@return");
    returnParam->SetParent(toJsonMemberFunctionSymbol);
    returnParam->SetType(uniquePtrJsonValueType->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    toJsonMemberFunctionSymbol->SetReturnParam(returnParam);
    toJsonMemberFunctionSymbol->SetAccess(SymbolAccess::public_);
    toJsonMemberFunctionSymbol->AddMember(thisParam);
    classTypeSymbol->AddMember(toJsonMemberFunctionSymbol);
    toJsonMemberFunctionSymbol->ComputeName();
    toJsonObjectMemberFunctionSymbolMap[classTypeSymbol] = toJsonMemberFunctionSymbol;
}

void JsonAttributeProcessor::GenerateImplementation(AttributeNode* attribute, Symbol* symbol, StatementBinder* statementBinder)
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
            throw Exception("internal error in JSON attribute implementation: constructor symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan(), attribute->ModuleId());
        }
        auto it1 = jsonCreatorMap.find(symbol);
        if (it1 != jsonCreatorMap.cend())
        {
            MemberFunctionSymbol* jsonCreatorSymbol = it1->second;
            GenerateJsonCreatorImplementation(attribute, classTypeSymbol, jsonCreatorSymbol, statementBinder);
        }
        else
        {
            throw Exception("internal error in JSON attribute implementation: Creator function symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan(), attribute->ModuleId());
        }
        auto it2 = toJsonJsonObjectMemberFunctionSymbolMap.find(symbol);
        if (it2 != toJsonJsonObjectMemberFunctionSymbolMap.cend())
        {
            MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol = it2->second;
            GenerateToJsonJsonObjectImplementation(attribute, classTypeSymbol, toJsonJsonObjectMemberFunctionSymbol, statementBinder);
        }
        else
        {
            throw Exception("internal error in JSON attribute implementation: member function 'ToJson' symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan(), attribute->ModuleId());
        }
        auto it3 = toJsonObjectMemberFunctionSymbolMap.find(symbol);
        if (it3 != toJsonObjectMemberFunctionSymbolMap.cend())
        {
            MemberFunctionSymbol* toJsonMemberFunctionSymbol = it3->second;
            GenerateToJsonImplementation(attribute, classTypeSymbol, toJsonMemberFunctionSymbol, statementBinder);
        }
        else
        {
            throw Exception("internal error in JSON attribute implementation: member function 'ToJson' symbol for symbol '" + ToUtf8(symbol->FullName()) + "' not found", attribute->GetSpan(), attribute->ModuleId());
        }
        SymbolTable& symbolTable = module->GetSymbolTable();
        symbolTable.AddJsonClass(classTypeSymbol->FullName());
    }
}

void JsonAttributeProcessor::GenerateJsonCreatorImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* jsonCreatorFunctionSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
        if (jsonValue)
        {
            fileScope->AddContainerScope(jsonValue->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), jsonCreatorFunctionSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);
        NewNode* newNode = new NewNode(span, moduleId, new IdentifierNode(span, moduleId, classTypeSymbol->FullName()));
        newNode->AddArgument(new IdentifierNode(span, moduleId, U"@value"));
        ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, moduleId, newNode);
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
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in JSON attribute generation: could not create JSON Create() function for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void JsonAttributeProcessor::GenerateJsonConstructorImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, ConstructorSymbol* jsonConstructorSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        Symbol* jsonValue = classTypeSymbol->GetModule()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonValue");
        if (jsonValue)
        {
            fileScope->AddContainerScope(jsonValue->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), jsonConstructorSymbol));
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
                AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"true")
                    {
                        BaseInitializerNode* baseInitializer = new BaseInitializerNode(span, moduleId);
                        baseInitializer->AddArgument(new IdentifierNode(span, moduleId, U"@value"));
                        constructorNode.AddInitializer(baseInitializer);
                    }
                }
            }
        }
        for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
        {
            std::u32string jsonFieldName = memberVariableSymbol->Name();
            AttributesNode* attributes = memberVariableSymbol->GetAttributes();
            if (attributes)
            {
                AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"false")
                    {
                        continue;
                    }
                }
                AttributeNode* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
                if (jsonFieldNameAttribute)
                {
                    jsonFieldName = jsonFieldNameAttribute->Value();
                }
            }
            InvokeNode* invokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), new IdentifierNode(span, moduleId, U"FromJson"));
            invokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), U"@value"));
            invokeNode->AddArgument(new UStringLiteralNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), jsonFieldName));
            invokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), memberVariableSymbol->Name()));
            ExpressionStatementNode* fromJsonStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), invokeNode);
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
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in JSON attribute generation: could not create JSON constructor for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void JsonAttributeProcessor::GenerateToJsonJsonObjectImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        Symbol* jsonObject = GetRootModuleForCurrentThread()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonObject");
        if (jsonObject)
        {
            fileScope->AddContainerScope(jsonObject->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), toJsonJsonObjectMemberFunctionSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);
        ClassTypeSymbol* baseClass = classTypeSymbol->BaseClass();
        if (baseClass)
        {
            AttributesNode* attributes = baseClass->GetAttributes();
            if (attributes)
            {
                AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"true")
                    {
                        BaseNode* baseNode = new BaseNode(span, moduleId);
                        ArrowNode* arrowNode = new ArrowNode(span, moduleId, baseNode, new IdentifierNode(span, moduleId, U"ToJson"));
                        InvokeNode* toJsonInvokeNode = new InvokeNode(span, moduleId, arrowNode);
                        toJsonInvokeNode->AddArgument(new IdentifierNode(span, moduleId, U"@object"));
                        ExpressionStatementNode* toJsonStatement = new ExpressionStatementNode(span, moduleId, toJsonInvokeNode);
                        compoundStatementNode.AddStatement(toJsonStatement);
                    }
                }
            }
        }
        for (MemberVariableSymbol* memberVariableSymbol : classTypeSymbol->MemberVariables())
        {
            std::u32string jsonFieldName = memberVariableSymbol->Name();
            AttributesNode* attributes = memberVariableSymbol->GetAttributes();
            if (attributes)
            {
                AttributeNode* jsonAttribute = attributes->GetAttribute(U"json");
                if (jsonAttribute)
                {
                    if (jsonAttribute->Value() == U"false")
                    {
                        continue;
                    }
                }
                AttributeNode* jsonFieldNameAttribute = attributes->GetAttribute(U"jsonFieldName");
                if (jsonFieldNameAttribute)
                {
                    jsonFieldName = jsonFieldNameAttribute->Value();
                }
            }
            InvokeNode* toJsonInvokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), new IdentifierNode(span, moduleId, U"ToJson"));
            toJsonInvokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), memberVariableSymbol->Name()));
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"@object"), new IdentifierNode(span, moduleId, U"AddField"));
            InvokeNode* addFieldInvokeNode = new InvokeNode(span, moduleId, arrowNode);
            addFieldInvokeNode->AddArgument(new UStringLiteralNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), jsonFieldName));
            addFieldInvokeNode->AddArgument(toJsonInvokeNode);
            ExpressionStatementNode* addFieldStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), addFieldInvokeNode);
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
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in JSON attribute generation: could not create 'void ToJson(JsobObject*)' member function for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void JsonAttributeProcessor::GenerateToJsonImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonMemberFunctionSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        Symbol* jsonObject = GetRootModuleForCurrentThread()->GetSymbolTable().GlobalNs().GetContainerScope()->Lookup(U"System.Json.JsonObject");
        if (jsonObject)
        {
            fileScope->AddContainerScope(jsonObject->Ns()->GetContainerScope());
        }
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), toJsonMemberFunctionSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);
        TemplateIdNode* uniquePtrJsonObject = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"UniquePtr"));
        uniquePtrJsonObject->AddTemplateArgument(new IdentifierNode(span, moduleId, U"JsonObject"));
        ConstructionStatementNode* constructJsonObjectStatement = new ConstructionStatementNode(span, moduleId, uniquePtrJsonObject, new IdentifierNode(span, moduleId, U"@object"));
        constructJsonObjectStatement->AddArgument(new NewNode(span, moduleId, new IdentifierNode(span, moduleId, U"JsonObject")));
        compoundStatementNode.AddStatement(constructJsonObjectStatement);
        InvokeNode* invokeToJson = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToJson"));
        invokeToJson->AddArgument(new InvokeNode(span, moduleId, new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"@object"), new IdentifierNode(span, moduleId, U"Get"))));
        ExpressionStatementNode* callToJsonStatement = new ExpressionStatementNode(span, moduleId, invokeToJson);
        compoundStatementNode.AddStatement(callToJsonStatement);
        TemplateIdNode* uniquePtrJsonValue = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"UniquePtr"));
        uniquePtrJsonValue->AddTemplateArgument(new IdentifierNode(span, moduleId, U"JsonValue"));
        InvokeNode* invokeJsonValue = new InvokeNode(span, moduleId, uniquePtrJsonValue);
        invokeJsonValue->AddArgument(new InvokeNode(span, moduleId, new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"@object"), new IdentifierNode(span, moduleId, U"Release"))));
        ReturnStatementNode* returnStatement = new ReturnStatementNode(span, moduleId, invokeJsonValue);
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
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in JSON attribute generation: could not create 'UniquePtr<JsonValue> ToJson()' member function for class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

JsonFieldNameAttributeProcessor::JsonFieldNameAttributeProcessor() : AttributeProcessor(U"jsonFieldName")
{
}

void JsonFieldNameAttributeProcessor::TypeCheck(AttributeNode* attribute, Symbol* symbol)
{
    if (symbol->GetSymbolType() == SymbolType::memberVariableSymbol)
    {
        if (attribute->Value().empty())
        {
            throw Exception("attribute value '" + ToUtf8(attribute->Value()) + "' for attribute '" + ToUtf8(attribute->Name()) + "' cannot be empty string", attribute->GetSpan(), attribute->ModuleId());
        }
        return;
    }
    AttributeProcessor::TypeCheck(attribute, symbol);
}    

} } // namespace cmajor::binder
