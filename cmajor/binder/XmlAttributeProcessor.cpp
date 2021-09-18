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
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;
using namespace cmajor::symbols;


XmlAttributeProcessor::XmlAttributeProcessor() : AttributeProcessor(U"xml")
{
    reservedMemberFunctionNames.insert(U"StaticClassName");
    reservedMemberFunctionNames.insert(U"Register");
    reservedMemberFunctionNames.insert(U"Create");
    reservedMemberFunctionNames.insert(U"DestroyObject");
    reservedMemberFunctionNames.insert(U"ObjectId");
    reservedMemberFunctionNames.insert(U"SetObjectId");
    reservedMemberFunctionNames.insert(U"Container");
    reservedMemberFunctionNames.insert(U"SetContainer");
    reservedMemberFunctionNames.insert(U"ClassId");
    reservedMemberFunctionNames.insert(U"ClassName");
    reservedMemberFunctionNames.insert(U"SetObjectXmlAttributes");
    reservedMemberFunctionNames.insert(U"ToXml");
    reservedMemberFunctionNames.insert(U"FromXml");
    reservedMemberVariableNames.insert(U"classId");
    reservedMemberVariableNames.insert(U"objectId");
    reservedMemberVariableNames.insert(U"container");
}

void XmlAttributeProcessor::TypeCheck(AttributeNode* attribute, Symbol* symbol)
{
    switch (symbol->GetSymbolType())
    {
        case SymbolType::classTypeSymbol: case SymbolType::classTemplateSpecializationSymbol: case SymbolType::memberVariableSymbol:
        {
            if (attribute->Value() == U"true" || attribute->Value() == U"false")
            {
                if (attribute->Value() == U"true" && (symbol->GetSymbolType() == SymbolType::classTypeSymbol || symbol->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol))
                {
                    TypeCheckClass(static_cast<ClassTypeSymbol*>(symbol));
                }
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

void XmlAttributeProcessor::TypeCheckClass(ClassTypeSymbol* classType)
{
    for (MemberFunctionSymbol* memberFunction : classType->MemberFunctions())
    {
        if (reservedMemberFunctionNames.find(memberFunction->GroupName()) != reservedMemberFunctionNames.cend())
        {
            throw Exception("member function name '" + ToUtf8(memberFunction->GroupName()) + "' is reserved for the implementation when using the '[xml]' attribute for a class", 
                memberFunction->GetSpan(), memberFunction->SourceModuleId());
        }
    }
    for (MemberVariableSymbol* memberVariable : classType->StaticMemberVariables())
    {
        if (reservedMemberVariableNames.find(memberVariable->Name()) != reservedMemberVariableNames.cend())
        {
            throw Exception("member variable name '" + ToUtf8(memberVariable->Name()) + "' is reserved for the implementation when using the '[xml]' attribute for a class",
                memberVariable->GetSpan(), memberVariable->SourceModuleId());
        }
    }
    for (MemberVariableSymbol* memberVariable : classType->MemberVariables())
    {
        if (reservedMemberVariableNames.find(memberVariable->Name()) != reservedMemberVariableNames.cend())
        {
            throw Exception("member variable name '" + ToUtf8(memberVariable->Name()) + "' is reserved for the implementation when using the '[xml]' attribute for a class",
                memberVariable->GetSpan(), memberVariable->SourceModuleId());
        }
    }
    classType->SetHasXmlAttribute();
}

void XmlAttributeProcessor::GenerateMemberSymbols(AttributeNode* attribute, ClassTypeSymbol* classType, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    for (MemberVariableSymbol* memberVariableSymbol : classType->MemberVariables())
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
        if (reservedMemberVariableNames.find(memberVariableSymbol->Name()) != reservedMemberVariableNames.cend())
        {
            continue;
        }
        TypeSymbol* memberVariableType = memberVariableSymbol->GetType();
        if (memberVariableType->IsClassTypeSymbol())
        {
            ClassTypeSymbol* memberVarClassType = static_cast<ClassTypeSymbol*>(memberVariableType);
            Node* node = boundCompileUnit.GetSymbolTable().GetNodeNoThrow(memberVarClassType);
            if (node)
            {
                BindClass(memberVarClassType, &boundCompileUnit);
            }
        }
    }
}

const int destructorId = 0;
const int staticClassNameId = 1;
const int createId = 2;
const int registerId = 3;
const int destroyObjectId = 4;
const int objectIdId = 5;
const int setObjectIdId = 6;
const int containerId = 7;
const int setContainerId = 8;
const int classIdId = 9;
const int classNameId = 10;
const int setObjectXmlAttributesId = 11;
const int toXmlId = 12;
const int fromXmlId = 13;
const int getPtrsId = 14;

void XmlAttributeProcessor::GenerateSymbols(AttributeNode* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    if (symbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
        if (attribute->Value() == U"true")
        {
            CheckXmlSerializableInterface(classTypeSymbol, boundCompileUnit, containerScope);
            if (!classTypeSymbol->BaseClass())
            {
                CheckVirtualDestructor(classTypeSymbol);
            }
            GenerateMemberSymbols(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateMemberVariableSymbols(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateStaticClassNameSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateCreateFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateRegisterFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateDestroyObjectFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateObjectIdFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateSetObjectIdFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateContainerFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateSetContainerFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateClassIdFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateClassNameFunctionSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateSetObjectXmlAttributesSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateToXmlSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateFromXmlSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
            GenerateGetPtrsSymbol(attribute, classTypeSymbol, boundCompileUnit, containerScope);
        }
    }
}

void XmlAttributeProcessor::CheckXmlSerializableInterface(ClassTypeSymbol* classType, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    const std::vector<InterfaceTypeSymbol*>& interfaces = classType->ImplementedInterfaces();
    bool xmlSerializableInterfaceFound = false;
    for (InterfaceTypeSymbol* intf : interfaces)
    {
        if (intf->FullName() == U"System.Xml.Serialization.XmlSerializable")
        {
            xmlSerializableInterfaceFound = true;
            break;
        }
    }
    if (!xmlSerializableInterfaceFound)
    {
        IdentifierNode xmlSerializableInterfaceNode(classType->GetSpan(), classType->SourceModuleId(), U"System.Xml.Serialization.XmlSerializable");
        TypeSymbol* xmlSerializableType = ResolveType(&xmlSerializableInterfaceNode, boundCompileUnit, containerScope);
        if (xmlSerializableType->GetSymbolType() == SymbolType::interfaceTypeSymbol)
        {
            InterfaceTypeSymbol* xmlSerialiableInterfaceType = static_cast<InterfaceTypeSymbol*>(xmlSerializableType);
            classType->AddImplementedInterface(xmlSerialiableInterfaceType);
        }
    }

}

void XmlAttributeProcessor::CheckVirtualDestructor(ClassTypeSymbol* classTypeSymbol)
{
    if (classTypeSymbol->Destructor())
    {
        if (!classTypeSymbol->Destructor()->IsVirtual())
        {
            throw Exception("destructor of an XML class '" + ToUtf8(classTypeSymbol->Name()) + "' is not virtual",
                classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
        }
    }
    else
    {
        classTypeSymbol->CreateDestructorSymbol();
        classTypeSymbol->Destructor()->SetVirtual();
        auto& m = functionSymbolMap[classTypeSymbol];
        m.push_back(std::make_pair(classTypeSymbol->Destructor(), destructorId));
    }
}

void XmlAttributeProcessor::GenerateImplementation(AttributeNode* attribute, Symbol* symbol, StatementBinder* statementBinder)
{
    if (symbol->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
        auto  it = functionSymbolMap.find(symbol);
        if (it != functionSymbolMap.cend())
        {
            const auto& m = it->second;
            if (m.empty())
            {
                throw Exception("internal error in XML attribute implementation: member function symbol map for class type symbol '" + ToUtf8(classTypeSymbol->FullName()) + "' is empty",
                    classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
            }
            for (const std::pair<FunctionSymbol*, int>& p : m)
            {
                FunctionSymbol* functionSymbol = p.first;
                switch (p.second)
                {
                    case destructorId:
                    {
                        GenerateDestructorImplementation(static_cast<DestructorSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case staticClassNameId:
                    {
                        GenerateStaticClassNameImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case createId:
                    {
                        GenerateCreateImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case registerId:
                    {
                        GenerateRegisterImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case destroyObjectId:
                    {
                        GenerateDestroyObjectImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case objectIdId:
                    {
                        GenerateObjectIdImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case setObjectIdId:
                    {
                        GenerateSetObjectIdImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case containerId:
                    {
                        GenerateContainerImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case setContainerId:
                    {
                        GenerateSetContainerImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case classIdId:
                    {
                        GenerateClassIdImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case classNameId:
                    {
                        GenerateClassNameImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case setObjectXmlAttributesId:
                    {
                        GenerateSetObjectXmlAttributesImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case toXmlId:
                    {
                        GenerateToXmlImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case fromXmlId:
                    {
                        GenerateFromXmlImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    case getPtrsId:
                    {
                        GenerateGetPtrsImplementation(attribute, classTypeSymbol, static_cast<MemberFunctionSymbol*>(functionSymbol), statementBinder);
                        break;
                    }
                    default:
                    {
                        throw Exception("internal error in XML attribute implementation: member function symbol map for class type symbol '" + ToUtf8(classTypeSymbol->FullName()) +
                            "' contains invalid member function id " + std::to_string(p.second), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
                    }
                }
            }
        }
        else
        {
            throw Exception("internal error in XML attribute implementation: member function symbol map for class type symbol '" + ToUtf8(classTypeSymbol->FullName()) + "' not found",
                classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
        }
    }
}

void XmlAttributeProcessor::GenerateDestructorImplementation(DestructorSymbol* destructorSymbol, StatementBinder* statementBinder)
{
    BoundCompileUnit& boundCompileUnit = statementBinder->GetBoundCompileUnit();
    if (!boundCompileUnit.IsGeneratedDestructorInstantiated(destructorSymbol))
    {
        boundCompileUnit.SetGeneratedDestructorInstantiated(destructorSymbol);
        cmajor::binder::GenerateDestructorImplementation(statementBinder->CurrentClass(), destructorSymbol, boundCompileUnit, statementBinder->GetContainerScope(),
            statementBinder->CurrentFunction(), destructorSymbol->GetSpan(), destructorSymbol->SourceModuleId());
    }
}

void XmlAttributeProcessor::GenerateMemberVariableSymbols(AttributeNode* attribute, ClassTypeSymbol* classType, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberVariableSymbol* classIdSymbol = new MemberVariableSymbol(attribute->GetSpan(), attribute->ModuleId(), U"classId");
    classIdSymbol->SetModule(&boundCompileUnit.GetModule());
    classIdSymbol->SetStatic();
    classIdSymbol->SetAccess(SymbolAccess::public_);
    classIdSymbol->SetType(symbolTable.GetTypeByName(U"int"));
    classType->AddMember(classIdSymbol);

    if (!HasXmlBaseClass(classType))
    {
        MemberVariableSymbol* objectIdSymbol = new MemberVariableSymbol(attribute->GetSpan(), attribute->ModuleId(), U"objectId");
        objectIdSymbol->SetModule(&boundCompileUnit.GetModule());
        objectIdSymbol->SetAccess(SymbolAccess::public_);
        objectIdSymbol->SetType(symbolTable.GetTypeByName(U"System.Uuid"));
        classType->AddMember(objectIdSymbol);

        MemberVariableSymbol* containerSymbol = new MemberVariableSymbol(attribute->GetSpan(), attribute->ModuleId(), U"container");
        containerSymbol->SetModule(&boundCompileUnit.GetModule());
        containerSymbol->SetAccess(SymbolAccess::public_);
        containerSymbol->SetType(symbolTable.GetTypeByName(U"System.Xml.Serialization.XmlContainer")->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
        classType->AddMember(containerSymbol);
    }
}

void XmlAttributeProcessor::GenerateStaticClassNameSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    try
    {
        SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
        MemberFunctionSymbol* staticClassNameSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"StaticClassName");
        staticClassNameSymbol->SetModule(&boundCompileUnit.GetModule());
        staticClassNameSymbol->SetGroupName(U"StaticClassName");
        staticClassNameSymbol->SetStatic();
        staticClassNameSymbol->SetAccess(SymbolAccess::public_);

        GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(staticClassNameSymbol);
        TypeSymbol* stringTypeSymbol = symbolTable.GetTypeByName(U"String<char>");
        staticClassNameSymbol->SetReturnType(stringTypeSymbol);

        ParameterSymbol* returnParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@return");
        returnParam->SetParent(staticClassNameSymbol);
        returnParam->SetType(stringTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
        staticClassNameSymbol->SetReturnParam(returnParam);
        classTypeSymbol->AddMember(staticClassNameSymbol);
        staticClassNameSymbol->ComputeName();

        auto& m = functionSymbolMap[classTypeSymbol];
        m.push_back(std::make_pair(staticClassNameSymbol, staticClassNameId));
    }
    catch (const std::exception& ex)
    {
        throw Exception("generation of 'StaticClassName' member function symbol failed: " + std::string(ex.what()), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
    }
}

void XmlAttributeProcessor::GenerateStaticClassNameImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* staticClassNameSymbol,
    StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), staticClassNameSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StringLiteralNode* classNameLiteralNode = new StringLiteralNode(span, moduleId, ToUtf8(classTypeSymbol->FullName()));
        ReturnStatementNode* returnStatementNode(new ReturnStatementNode(span, moduleId, classNameLiteralNode));
        compoundStatementNode.AddStatement(returnStatementNode);

        CompileMemberFunction(staticClassNameSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'StaticClassName' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateCreateFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* createSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"Create");
    createSymbol->SetModule(&boundCompileUnit.GetModule());
    createSymbol->SetGroupName(U"Create");
    createSymbol->SetStatic();
    createSymbol->SetAccess(SymbolAccess::public_);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(createSymbol);

    TypeSymbol* voidPtrTypeSymbol = symbolTable.GetTypeByName(U"void")->AddPointer(attribute->GetSpan(), attribute->ModuleId());
    createSymbol->SetReturnType(voidPtrTypeSymbol);

    classTypeSymbol->AddMember(createSymbol);
    createSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(createSymbol, createId));
}

void XmlAttributeProcessor::GenerateCreateImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* createFunctionSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), createFunctionSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        IdentifierNode* classNode = new IdentifierNode(span, moduleId, classTypeSymbol->FullName());
        NewNode* newNode = new NewNode(span, moduleId, classNode);
        ReturnStatementNode* returnStatementNode(new ReturnStatementNode(span, moduleId, newNode));
        compoundStatementNode.AddStatement(returnStatementNode);

        CompileMemberFunction(createFunctionSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'Create' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateRegisterFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* registerSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"Register");
    registerSymbol->SetModule(&boundCompileUnit.GetModule());
    registerSymbol->SetGroupName(U"Register");
    registerSymbol->SetStatic();
    registerSymbol->SetAccess(SymbolAccess::public_);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(registerSymbol);

    TypeSymbol* voidTypeSymbol = symbolTable.GetTypeByName(U"void");
    registerSymbol->SetReturnType(voidTypeSymbol);

    ParameterSymbol* classIdParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"classId_");
    TypeSymbol* intTypeSymbol = symbolTable.GetTypeByName(U"int");
    classIdParam->SetType(intTypeSymbol);
    registerSymbol->AddMember(classIdParam);
    classTypeSymbol->AddMember(registerSymbol);
    registerSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(registerSymbol, registerId));
}

void XmlAttributeProcessor::GenerateRegisterImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* registerSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), registerSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        AssignmentStatementNode* assignClassIdStatement = new AssignmentStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"classId"), new IdentifierNode(span, moduleId, U"classId_"));
        compoundStatementNode.AddStatement(assignClassIdStatement);

        TemplateIdNode* xmlRegister = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Xml.Serialization.XmlRegister"));
        xmlRegister->AddTemplateArgument(new IdentifierNode(span, moduleId, classTypeSymbol->FullName()));
        InvokeNode* invokeXmlRegisterFunction = new InvokeNode(span, moduleId, xmlRegister);
        invokeXmlRegisterFunction->AddArgument(new IdentifierNode(span, moduleId, U"classId"));
        ExpressionStatementNode* invokeXmlRegisterStatement = new ExpressionStatementNode(span, moduleId, invokeXmlRegisterFunction);
        compoundStatementNode.AddStatement(invokeXmlRegisterStatement);

        CompileMemberFunction(registerSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'Register' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateDestroyObjectFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* destroyObjectSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"DestroyObject");
    destroyObjectSymbol->SetModule(&boundCompileUnit.GetModule());
    destroyObjectSymbol->SetGroupName(U"DestroyObject");
    destroyObjectSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        destroyObjectSymbol->SetOverride();
    }
    else
    {
        destroyObjectSymbol->SetVirtual();
    }
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    destroyObjectSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(destroyObjectSymbol);

    TypeSymbol* voidTypeSymbol = symbolTable.GetTypeByName(U"void");
    destroyObjectSymbol->SetReturnType(voidTypeSymbol);

    classTypeSymbol->AddMember(destroyObjectSymbol);
    destroyObjectSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(destroyObjectSymbol, destroyObjectId));
}

void XmlAttributeProcessor::GenerateDestroyObjectImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* destroyObjectSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), destroyObjectSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StatementNode* statementNode = nullptr;
        if (HasXmlBaseClass(classTypeSymbol))
        {
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"DestroyObject"));
            InvokeNode* baseDestroyObjectCall = new InvokeNode(span, moduleId, arrowNode);
            statementNode = new ExpressionStatementNode(span, moduleId, baseDestroyObjectCall);
        }
        else
        {
            statementNode = new DeleteStatementNode(span, moduleId, new ThisNode(span, moduleId));
        }
        compoundStatementNode.AddStatement(statementNode);

        CompileMemberFunction(destroyObjectSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'DestroyObject' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateObjectIdFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* objectIdSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"ObjectId");
    objectIdSymbol->SetModule(&boundCompileUnit.GetModule());
    objectIdSymbol->SetGroupName(U"ObjectId");
    objectIdSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        objectIdSymbol->SetOverride();
    }
    else
    {
        objectIdSymbol->SetVirtual();
    }
    objectIdSymbol->SetConst();
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    objectIdSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(objectIdSymbol);

    TypeSymbol* uuidTypeSymbol = symbolTable.GetTypeByName(U"System.Uuid");
    objectIdSymbol->SetReturnType(uuidTypeSymbol);

    ParameterSymbol* returnParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@return");
    returnParam->SetParent(objectIdSymbol);
    returnParam->SetType(uuidTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    objectIdSymbol->SetReturnParam(returnParam);

    classTypeSymbol->AddMember(objectIdSymbol);
    objectIdSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(objectIdSymbol, objectIdId));
}

void XmlAttributeProcessor::GenerateObjectIdImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* objectIdSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), objectIdSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StatementNode* statementNode = nullptr;
        if (HasXmlBaseClass(classTypeSymbol))
        {
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"ObjectId"));
            InvokeNode* baseObjectIdCall = new InvokeNode(span, moduleId, arrowNode);
            statementNode = new ReturnStatementNode(span, moduleId, baseObjectIdCall);
        }
        else
        {
            ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"objectId"));
            statementNode = returnStatementNode;
        }
        compoundStatementNode.AddStatement(statementNode);

        CompileMemberFunction(objectIdSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'ObjectId' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateSetObjectIdFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* setObjectIdSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"SetObjectId");
    setObjectIdSymbol->SetModule(&boundCompileUnit.GetModule());
    setObjectIdSymbol->SetGroupName(U"SetObjectId");
    setObjectIdSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        setObjectIdSymbol->SetOverride();
    }
    else
    {
        setObjectIdSymbol->SetVirtual();
    }
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    setObjectIdSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(setObjectIdSymbol);

    TypeSymbol* uuidTypeSymbol = symbolTable.GetTypeByName(U"System.Uuid");
    ParameterSymbol* objectIdParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"objectId_");
    objectIdParam->SetType(uuidTypeSymbol->AddConst(attribute->GetSpan(), attribute->ModuleId())->AddLvalueReference(attribute->GetSpan(), attribute->ModuleId()));
    setObjectIdSymbol->AddMember(objectIdParam);

    setObjectIdSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));

    classTypeSymbol->AddMember(setObjectIdSymbol);
    setObjectIdSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(setObjectIdSymbol, setObjectIdId));
}

bool XmlAttributeProcessor::HasXmlBaseClass(ClassTypeSymbol* classType) const
{
    ClassTypeSymbol* baseClass = classType->BaseClass();
    if (baseClass)
    {
        return baseClass->HasXmlAttribute();
    }
    return false;
}

void XmlAttributeProcessor::GenerateSetObjectIdImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setObjectIdSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), setObjectIdSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StatementNode* statementNode = nullptr;
        if (HasXmlBaseClass(classTypeSymbol))
        {
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"SetObjectId"));
            InvokeNode* baseObjectIdCall = new InvokeNode(span, moduleId, arrowNode);
            baseObjectIdCall->AddArgument(new IdentifierNode(span, moduleId, U"objectId_"));
            statementNode = new ExpressionStatementNode(span, moduleId, baseObjectIdCall);
        }
        else
        {
            AssignmentStatementNode* assignmentStatementNode = new AssignmentStatementNode(span, moduleId, 
                new IdentifierNode(span, moduleId, U"objectId"), 
                new IdentifierNode(span, moduleId, U"objectId_"));
            statementNode = assignmentStatementNode;
        }
        compoundStatementNode.AddStatement(statementNode);

        CompileMemberFunction(setObjectIdSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'SetObjectId' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateContainerFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* containerSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"Container");
    containerSymbol->SetModule(&boundCompileUnit.GetModule());
    containerSymbol->SetGroupName(U"Container");
    containerSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        containerSymbol->SetOverride();
    }
    else
    {
        containerSymbol->SetVirtual();
    }
    containerSymbol->SetConst();
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    containerSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(containerSymbol);

    TypeSymbol* containerTypeSymbol = symbolTable.GetTypeByName(U"System.Xml.Serialization.XmlContainer")->AddPointer(attribute->GetSpan(), attribute->ModuleId());
    containerSymbol->SetReturnType(containerTypeSymbol);
    
    classTypeSymbol->AddMember(containerSymbol);
    containerSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(containerSymbol, containerId));
}

void XmlAttributeProcessor::GenerateContainerImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* containerSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), containerSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StatementNode* statementNode = nullptr;
        if (HasXmlBaseClass(classTypeSymbol))
        {
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"Container"));
            InvokeNode* baseObjectIdCall = new InvokeNode(span, moduleId, arrowNode);
            statementNode = new ReturnStatementNode(span, moduleId, baseObjectIdCall);
        }
        else
        {
            ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"container"));
            statementNode = returnStatementNode;
        }
        compoundStatementNode.AddStatement(statementNode);

        CompileMemberFunction(containerSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'Container' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateSetContainerFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* setContainerSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"SetContainer");
    setContainerSymbol->SetModule(&boundCompileUnit.GetModule());
    setContainerSymbol->SetGroupName(U"SetContainer");
    setContainerSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        setContainerSymbol->SetOverride();
    }
    else
    {
        setContainerSymbol->SetVirtual();
    }
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    setContainerSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(setContainerSymbol);

    TypeSymbol* containerTypeSymbol = symbolTable.GetTypeByName(U"System.Xml.Serialization.XmlContainer");
    ParameterSymbol* containerParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"container_");
    containerParam->SetType(containerTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    setContainerSymbol->AddMember(containerParam);

    setContainerSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));

    classTypeSymbol->AddMember(setContainerSymbol);
    setContainerSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(setContainerSymbol, setContainerId));
}

void XmlAttributeProcessor::GenerateSetContainerImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setContainerSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), setContainerSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StatementNode* statementNode = nullptr;
        if (HasXmlBaseClass(classTypeSymbol))
        {
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"SetContainer"));
            InvokeNode* baseContainerCall = new InvokeNode(span, moduleId, arrowNode);
            baseContainerCall->AddArgument(new IdentifierNode(span, moduleId, U"container_"));
            statementNode = new ExpressionStatementNode(span, moduleId, baseContainerCall);
        }
        else
        {
            AssignmentStatementNode* assignmentStatementNode = new AssignmentStatementNode(span, moduleId,
                new IdentifierNode(span, moduleId, U"container"),
                new IdentifierNode(span, moduleId, U"container_"));
            statementNode = assignmentStatementNode;
        }
        compoundStatementNode.AddStatement(statementNode);

        CompileMemberFunction(setContainerSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'SetContainer' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateClassIdFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* classIdSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"ClassId");
    classIdSymbol->SetModule(&boundCompileUnit.GetModule());
    classIdSymbol->SetGroupName(U"ClassId");
    classIdSymbol->SetAccess(SymbolAccess::public_);
    classIdSymbol->SetConst();
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    classIdSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(classIdSymbol);

    classIdSymbol->SetReturnType(symbolTable.GetTypeByName(U"int"));

    classTypeSymbol->AddMember(classIdSymbol);
    classIdSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(classIdSymbol, classIdId));
}

void XmlAttributeProcessor::GenerateClassIdImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* classIdSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), classIdSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StatementNode* statementNode = nullptr;
        ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"classId"));
        statementNode = returnStatementNode;
        compoundStatementNode.AddStatement(statementNode);

        CompileMemberFunction(classIdSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'ClassId' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateClassNameFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* classNameSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"ClassName");
    classNameSymbol->SetModule(&boundCompileUnit.GetModule());
    classNameSymbol->SetGroupName(U"ClassName");
    classNameSymbol->SetAccess(SymbolAccess::public_);
    classNameSymbol->SetConst();
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    classNameSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(classNameSymbol);

    TypeSymbol* stringTypeSymbol = symbolTable.GetTypeByName(U"String<char>");
    classNameSymbol->SetReturnType(stringTypeSymbol);

    ParameterSymbol* returnParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@return");
    returnParam->SetParent(classNameSymbol);
    returnParam->SetType(stringTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    classNameSymbol->SetReturnParam(returnParam);
    classTypeSymbol->AddMember(classNameSymbol);
    classNameSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(classNameSymbol, classNameId));
}

void XmlAttributeProcessor::GenerateClassNameImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* classNameSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), classNameSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        StatementNode* statementNode = nullptr;
        TypeNameNode* typeNameNode = new TypeNameNode(span, moduleId, new DerefNode(span, moduleId, new ThisNode(span, moduleId)));
        ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, moduleId, typeNameNode);
        statementNode = returnStatementNode;
        compoundStatementNode.AddStatement(statementNode);

        CompileMemberFunction(classNameSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'ClassName' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateSetObjectXmlAttributesSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* setObjectXmlAttributesSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"SetObjectXmlAttributes");
    setObjectXmlAttributesSymbol->SetModule(&boundCompileUnit.GetModule());
    setObjectXmlAttributesSymbol->SetGroupName(U"SetObjectXmlAttributes");
    setObjectXmlAttributesSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        setObjectXmlAttributesSymbol->SetOverride();
    }
    else
    {
        setObjectXmlAttributesSymbol->SetVirtual();
    }
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    setObjectXmlAttributesSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(setObjectXmlAttributesSymbol);

    TypeSymbol* domElementTypeSymbol = symbolTable.GetTypeByName(U"System.Dom.Element");
    ParameterSymbol* elementParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"element");
    elementParam->SetType(domElementTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    setObjectXmlAttributesSymbol->AddMember(elementParam);

    setObjectXmlAttributesSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));

    classTypeSymbol->AddMember(setObjectXmlAttributesSymbol);
    setObjectXmlAttributesSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(setObjectXmlAttributesSymbol, setObjectXmlAttributesId));
}

void XmlAttributeProcessor::GenerateSetObjectXmlAttributesImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setObjectXmlAttributesSymbol,
    StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), setObjectXmlAttributesSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        ArrowNode* setClassIdArrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"element"), new IdentifierNode(span, moduleId, U"SetAttribute"));
        InvokeNode* invokeSetClassIdAttribute = new InvokeNode(span, moduleId, setClassIdArrowNode);
        UStringLiteralNode* classIdLiteralArg = new UStringLiteralNode(span, moduleId, U"classId");
        invokeSetClassIdAttribute->AddArgument(classIdLiteralArg);
        InvokeNode* invokeClassIdToStringFun = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToString"));
        invokeClassIdToStringFun->AddArgument(new IdentifierNode(span, moduleId, U"classId"));
        InvokeNode* invokeClassIdToUtf32Fun = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToUtf32"));
        invokeClassIdToUtf32Fun->AddArgument(invokeClassIdToStringFun);
        invokeSetClassIdAttribute->AddArgument(invokeClassIdToUtf32Fun);
        ExpressionStatementNode* setClassIdAttributeStatementNode = new ExpressionStatementNode(span, moduleId, invokeSetClassIdAttribute);
        compoundStatementNode.AddStatement(setClassIdAttributeStatementNode);

        ArrowNode* setObjectIdArrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"element"), new IdentifierNode(span, moduleId, U"SetAttribute"));
        InvokeNode* invokeSetObjectIdAttribute = new InvokeNode(span, moduleId, setObjectIdArrowNode);
        UStringLiteralNode* objectIdLiteralArg = new UStringLiteralNode(span, moduleId, U"objectId");
        invokeSetObjectIdAttribute->AddArgument(objectIdLiteralArg);
        InvokeNode* invokeObjectIdToStringFun = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToString"));
        invokeObjectIdToStringFun->AddArgument(new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ObjectId")));
        InvokeNode* invokeObjectIdToUtf32Fun = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToUtf32"));
        invokeObjectIdToUtf32Fun->AddArgument(invokeObjectIdToStringFun);
        invokeSetObjectIdAttribute->AddArgument(invokeObjectIdToUtf32Fun);
        ExpressionStatementNode* setObjectIdAttributeStatementNode = new ExpressionStatementNode(span, moduleId, invokeSetObjectIdAttribute);
        compoundStatementNode.AddStatement(setObjectIdAttributeStatementNode);

        ArrowNode* setClassNameArrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"element"), new IdentifierNode(span, moduleId, U"SetAttribute"));
        InvokeNode* invokeSetClassNameAttribute = new InvokeNode(span, moduleId, setClassNameArrowNode);
        UStringLiteralNode* classNameLiteralArg = new UStringLiteralNode(span, moduleId, U"className");
        invokeSetClassNameAttribute->AddArgument(classNameLiteralArg);
        InvokeNode* invokeClassNameToUtf32Fun = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToUtf32"));
        invokeClassNameToUtf32Fun->AddArgument(new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ClassName")));
        invokeSetClassNameAttribute->AddArgument(invokeClassNameToUtf32Fun);
        ExpressionStatementNode* setClassNameAttributeStatementNode = new ExpressionStatementNode(span, moduleId, invokeSetClassNameAttribute);
        compoundStatementNode.AddStatement(setClassNameAttributeStatementNode);

        CompileMemberFunction(setObjectXmlAttributesSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'SetObjectXmlAttributes' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateToXmlSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* toXmlSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"ToXml");
    toXmlSymbol->SetModule(&boundCompileUnit.GetModule());
    toXmlSymbol->SetGroupName(U"ToXml");
    toXmlSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        toXmlSymbol->SetOverride();
    }
    else
    {
        toXmlSymbol->SetVirtual();
    }
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    toXmlSymbol->AddMember(thisParam);

    ParameterSymbol* elementNameParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"elementName");
    elementNameParam->SetType(symbolTable.GetTypeByName(U"String<char>")->AddConst(attribute->GetSpan(), attribute->ModuleId())->AddLvalueReference(attribute->GetSpan(), attribute->ModuleId()));
    toXmlSymbol->AddMember(elementNameParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(toXmlSymbol);

    TypeSymbol* elementTypeSymbol = symbolTable.GetTypeByName(U"System.Dom.Element")->AddPointer(attribute->GetSpan(), attribute->ModuleId());
    toXmlSymbol->SetReturnType(elementTypeSymbol);

    classTypeSymbol->AddMember(toXmlSymbol);
    toXmlSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(toXmlSymbol, toXmlId));
}

void XmlAttributeProcessor::GenerateToXmlImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toXmlSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), toXmlSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        if (HasXmlBaseClass(classTypeSymbol))
        {
            ConstructionStatementNode* constructionStatementNode = new ConstructionStatementNode(span, moduleId, 
                new PointerNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Dom.Element")),
                new IdentifierNode(span, moduleId, U"element"));
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"ToXml"));
            InvokeNode* invokeNode = new InvokeNode(span, moduleId, arrowNode);
            invokeNode->AddArgument(new IdentifierNode(span, moduleId, U"elementName"));
            constructionStatementNode->AddArgument(invokeNode);
            constructionStatementNode->SetAssignment();
            compoundStatementNode.AddStatement(constructionStatementNode);
        }
        else
        {
            ConstructionStatementNode* constructionStatementNode = new ConstructionStatementNode(span, moduleId, 
                new PointerNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Dom.Element")),
                new IdentifierNode(span, moduleId, U"element"));
            NewNode* newElementNode = new NewNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Dom.Element"));
            InvokeNode* toUtf32Node = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToUtf32"));
            toUtf32Node->AddArgument(new IdentifierNode(span, moduleId, U"elementName"));
            newElementNode->AddArgument(toUtf32Node);
            constructionStatementNode->AddArgument(newElementNode);
            constructionStatementNode->SetAssignment();
            compoundStatementNode.AddStatement(constructionStatementNode);

            InvokeNode* invokeSetObjectXmlAttributesNode = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"SetObjectXmlAttributes"));
            invokeSetObjectXmlAttributesNode->AddArgument(new IdentifierNode(span, moduleId, U"element"));
            ExpressionStatementNode* expressionStatement = new ExpressionStatementNode(span, moduleId, invokeSetObjectXmlAttributesNode);
            compoundStatementNode.AddStatement(expressionStatement);
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
            if (reservedMemberVariableNames.find(memberVariableSymbol->Name()) != reservedMemberVariableNames.cend())
            {
                continue; 
            }
            DotNode* toXmlDotNode = new DotNode(span, moduleId,
                new DotNode(span, moduleId,
                    new DotNode(span, moduleId,
                        new IdentifierNode(span, moduleId, U"System"),
                        new IdentifierNode(span, moduleId, U"Xml")),
                    new IdentifierNode(span, moduleId, U"Serialization")),
                    new IdentifierNode(span, moduleId, U"ToXml"));
            InvokeNode* toXmlInvokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), toXmlDotNode);
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
            ArrowNode* appendChildArrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"element"), new IdentifierNode(span, moduleId, U"AppendChild"));
            InvokeNode* appendChildInvokeNode = new InvokeNode(span, moduleId, appendChildArrowNode);
            appendChildInvokeNode->AddArgument(uniquePtrNodeInvokeNode);
            ExpressionStatementNode* appendChildStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), appendChildInvokeNode);
            compoundStatementNode.AddStatement(appendChildStatement);
        }

        ReturnStatementNode* returnStatement = new ReturnStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"element"));
        compoundStatementNode.AddStatement(returnStatement);

        CompileMemberFunction(toXmlSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'ToXml' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateFromXmlSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* fromXmlSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"FromXml");
    fromXmlSymbol->SetModule(&boundCompileUnit.GetModule());
    fromXmlSymbol->SetGroupName(U"FromXml");
    fromXmlSymbol->SetAccess(SymbolAccess::public_);
    if (HasXmlBaseClass(classTypeSymbol))
    {
        fromXmlSymbol->SetOverride();
    }
    else
    {
        fromXmlSymbol->SetVirtual();
    }
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    fromXmlSymbol->AddMember(thisParam);

    ParameterSymbol* elementParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"element");
    elementParam->SetType(symbolTable.GetTypeByName(U"System.Dom.Element")->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    fromXmlSymbol->AddMember(elementParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(fromXmlSymbol);

    fromXmlSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));

    classTypeSymbol->AddMember(fromXmlSymbol);
    fromXmlSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(fromXmlSymbol, fromXmlId));
}

void XmlAttributeProcessor::GenerateFromXmlImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* fromXmlSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), fromXmlSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        if (HasXmlBaseClass(classTypeSymbol))
        {
            InvokeNode* invokeBaseFromXml = new InvokeNode(span, moduleId, new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"FromXml")));
            invokeBaseFromXml->AddArgument(new IdentifierNode(span, moduleId, U"element"));
            ExpressionStatementNode* baseFromXmlStatement = new ExpressionStatementNode(span, moduleId, invokeBaseFromXml);
            compoundStatementNode.AddStatement(baseFromXmlStatement);
        }
        else
        {
            ConstructionStatementNode* constructionStatementNode = new ConstructionStatementNode(span, moduleId,
                new IdentifierNode(span, moduleId, U"System.ustring"),
                new IdentifierNode(span, moduleId, U"objectIdAttr"));
            constructionStatementNode->SetAssignment();
            ArrowNode* arrowNode = new ArrowNode(span, moduleId, new IdentifierNode(span, moduleId, U"element"), new IdentifierNode(span, moduleId, U"GetAttribute"));
            UStringLiteralNode* objectIdLiteral = new UStringLiteralNode(span, moduleId, U"objectId");
            InvokeNode* elementGetAttributeNode = new InvokeNode(span, moduleId, arrowNode);
            elementGetAttributeNode->AddArgument(objectIdLiteral);
            constructionStatementNode->AddArgument(elementGetAttributeNode);
            constructionStatementNode->SetAssignment();
            compoundStatementNode.AddStatement(constructionStatementNode);

            InvokeNode* invokeParseUuid = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ParseUuid"));
            InvokeNode* toUtf8Node = new InvokeNode(span, moduleId, new IdentifierNode(span, moduleId, U"ToUtf8"));
            toUtf8Node->AddArgument(new IdentifierNode(span, moduleId, U"objectIdAttr"));
            invokeParseUuid->AddArgument(toUtf8Node);
            AssignmentStatementNode* assignObjectIdStatement = new AssignmentStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"objectId"), invokeParseUuid);
            DotNode* dotNode = new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"objectIdAttr"), new IdentifierNode(span, moduleId, U"IsEmpty"));
            NotNode* notObjectIdAttrIsEmpty = new NotNode(span, moduleId, new InvokeNode(span, moduleId, dotNode));
            IfStatementNode* ifStatementNode = new IfStatementNode(span, moduleId, notObjectIdAttrIsEmpty, assignObjectIdStatement, nullptr);
            compoundStatementNode.AddStatement(ifStatementNode);
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
            if (reservedMemberVariableNames.find(memberVariableSymbol->Name()) != reservedMemberVariableNames.cend())
            {
                continue;
            }
            DotNode* fromXmlDotNode = new DotNode(span, moduleId,
                new DotNode(span, moduleId,
                    new DotNode(span, moduleId,
                        new IdentifierNode(span, moduleId, U"System"),
                        new IdentifierNode(span, moduleId, U"Xml")),
                    new IdentifierNode(span, moduleId, U"Serialization")),
                new IdentifierNode(span, moduleId, U"FromXml"));
            InvokeNode* fromXmlInvokeNode = new InvokeNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), fromXmlDotNode);
            fromXmlInvokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), U"element"));
            fromXmlInvokeNode->AddArgument(new StringLiteralNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), ToUtf8(memberVariableSymbol->Name())));
            fromXmlInvokeNode->AddArgument(new IdentifierNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), memberVariableSymbol->Name()));
            ExpressionStatementNode* fromXmlStatement = new ExpressionStatementNode(memberVariableSymbol->GetSpan(), memberVariableSymbol->SourceModuleId(), fromXmlInvokeNode);
            compoundStatementNode.AddStatement(fromXmlStatement);
        }

        CompileMemberFunction(fromXmlSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'FromXml' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::GenerateGetPtrsSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    MemberFunctionSymbol* getPtrsSymbol = new MemberFunctionSymbol(attribute->GetSpan(), attribute->ModuleId(), U"GetPtrs");
    getPtrsSymbol->SetModule(&boundCompileUnit.GetModule());
    getPtrsSymbol->SetGroupName(U"GetPtrs");
    getPtrsSymbol->SetAccess(SymbolAccess::public_);
    getPtrsSymbol->SetConst();
    if (HasXmlBaseClass(classTypeSymbol))
    {
        getPtrsSymbol->SetOverride();
    }
    else
    {
        getPtrsSymbol->SetVirtual();
    }
    ParameterSymbol* thisParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"this");
    thisParam->SetType(classTypeSymbol->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    getPtrsSymbol->AddMember(thisParam);

    GetRootModuleForCurrentThread()->GetSymbolTable().SetFunctionIdFor(getPtrsSymbol);

    TypeSymbol* listOfXmlPtrBasePtrs = symbolTable.GetTypeByName(U"List<XmlPtrBase*>");
    getPtrsSymbol->SetReturnType(listOfXmlPtrBasePtrs);

    ParameterSymbol* returnParam = new ParameterSymbol(attribute->GetSpan(), attribute->ModuleId(), U"@return");
    returnParam->SetParent(getPtrsSymbol);
    returnParam->SetType(listOfXmlPtrBasePtrs->AddPointer(attribute->GetSpan(), attribute->ModuleId()));
    getPtrsSymbol->SetReturnParam(returnParam);

    classTypeSymbol->AddMember(getPtrsSymbol);
    getPtrsSymbol->ComputeName();

    auto& m = functionSymbolMap[classTypeSymbol];
    m.push_back(std::make_pair(getPtrsSymbol, getPtrsId));
}

void XmlAttributeProcessor::GenerateGetPtrsImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* getPtrsSymbol, StatementBinder* statementBinder)
{
    try
    {
        FileScope* fileScope = new FileScope();
        statementBinder->GetBoundCompileUnit().AddFileScope(fileScope);
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(&statementBinder->GetBoundCompileUnit(), getPtrsSymbol));
        Span span = attribute->GetSpan();
        boost::uuids::uuid moduleId = attribute->ModuleId();
        MemberFunctionNode memberFunctionNode(span, moduleId);
        CompoundStatementNode compoundStatementNode(span, moduleId);
        compoundStatementNode.SetEndBraceSpan(span);

        if (HasXmlBaseClass(classTypeSymbol))
        {
            TemplateIdNode* ptrsList = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Collections.List"));
            ptrsList->AddTemplateArgument(new PointerNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Xml.Serialization.XmlPtrBase")));
            ConstructionStatementNode* constructPtrsList = new ConstructionStatementNode(span, moduleId, ptrsList, new IdentifierNode(span, moduleId, U"ptrs"));
            constructPtrsList->SetAssignment();
            ArrowNode* arrow = new ArrowNode(span, moduleId, new BaseNode(span, moduleId), new IdentifierNode(span, moduleId, U"GetPtrs"));
            constructPtrsList->AddArgument(new InvokeNode(span, moduleId, arrow));
            compoundStatementNode.AddStatement(constructPtrsList);
        }
        else
        {
            TemplateIdNode* ptrsList = new TemplateIdNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Collections.List"));
            ptrsList->AddTemplateArgument(new PointerNode(span, moduleId, new IdentifierNode(span, moduleId, U"System.Xml.Serialization.XmlPtrBase")));
            ConstructionStatementNode* constructPtrsList = new ConstructionStatementNode(span, moduleId, ptrsList, new IdentifierNode(span, moduleId, U"ptrs"));
            compoundStatementNode.AddStatement(constructPtrsList);
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
            if (reservedMemberVariableNames.find(memberVariableSymbol->Name()) != reservedMemberVariableNames.cend())
            {
                continue;
            }
            TypeSymbol* typeSymbol = memberVariableSymbol->GetType();
            if (typeSymbol->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(typeSymbol);
                ClassTypeSymbol* primaryType = specialization->GetClassTemplate();
                std::u32string primaryTypeGroupName = primaryType->GroupName();
                if (primaryTypeGroupName == U"XmlPtr" || primaryTypeGroupName == U"UniqueXmlPtr")
                {
                    // ptrs.Add(&x)
                    DotNode* addDot = new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"ptrs"), new IdentifierNode(span, moduleId, U"Add"));
                    InvokeNode* invokeAdd = new InvokeNode(span, moduleId, addDot);
                    invokeAdd->AddArgument(new AddrOfNode(span, moduleId, new IdentifierNode(span, moduleId, memberVariableSymbol->Name())));
                    ExpressionStatementNode* addStatement = new ExpressionStatementNode(span, moduleId, invokeAdd);
                    compoundStatementNode.AddStatement(addStatement);
                }
                else if (primaryTypeGroupName == U"List")
                {
                    if (specialization->TemplateArgumentTypes().size() == 1)
                    {
                        TypeSymbol* argType = specialization->TemplateArgumentTypes()[0];
                        if (argType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
                        {
                            ClassTemplateSpecializationSymbol* argSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(argType);
                            ClassTypeSymbol* argPrimaryType = argSpecialization->GetClassTemplate();
                            std::u32string argGroupName = argPrimaryType->GroupName();
                            if (argGroupName == U"XmlPtr" || argGroupName == U"UniqueXmlPtr")
                            {
                                // long n_x = x.Count();
                                // for (long i = 0; i < n_x; ++i)
                                //{
                                //     ptrs.Add(&x[i]);
                                //}
                                ConstructionStatementNode* count = new ConstructionStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"long"), 
                                    new IdentifierNode(span, moduleId, U"n_" + memberVariableSymbol->Name()));
                                count->SetAssignment();
                                DotNode* dotCount = new DotNode(span, moduleId, new IdentifierNode(span, moduleId, memberVariableSymbol->Name()), new IdentifierNode(span, moduleId, U"Count"));
                                InvokeNode* invokeCount = new InvokeNode(span, moduleId, dotCount);
                                count->AddArgument(invokeCount);
                                compoundStatementNode.AddStatement(count);

                                ConstructionStatementNode* constructI = new ConstructionStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"long"),
                                    new IdentifierNode(span, moduleId, U"i"));
                                LessNode* testI = new LessNode(span, moduleId, new IdentifierNode(span, moduleId, U"i"), new IdentifierNode(span, moduleId, U"n_" + memberVariableSymbol->Name()));
                                ExpressionStatementNode* incI = new ExpressionStatementNode(span, moduleId, new PrefixIncrementNode(span, moduleId, new IdentifierNode(span, moduleId, U"i")));
                                DotNode* addDot = new DotNode(span, moduleId, new IdentifierNode(span, moduleId, U"ptrs"), new IdentifierNode(span, moduleId, U"Add"));
                                InvokeNode* invokeAdd = new InvokeNode(span, moduleId, addDot);
                                IndexingNode* indexing = new IndexingNode(span, moduleId, new IdentifierNode(span, moduleId, memberVariableSymbol->Name()),
                                    new IdentifierNode(span, moduleId, U"i"));
                                invokeAdd->AddArgument(new AddrOfNode(span, moduleId, indexing));
                                ExpressionStatementNode* actionS = new ExpressionStatementNode(span, moduleId, invokeAdd);
                                ForStatementNode* forStatement = new ForStatementNode(span, moduleId, constructI, testI, incI, actionS);
                                compoundStatementNode.AddStatement(forStatement);
                            }
                        }
                    }
                }
            }
        }

        ReturnStatementNode* returnStatementNode = new ReturnStatementNode(span, moduleId, new IdentifierNode(span, moduleId, U"ptrs"));
        compoundStatementNode.AddStatement(returnStatementNode);

        CompileMemberFunction(getPtrsSymbol, compoundStatementNode, memberFunctionNode, std::move(boundFunction), statementBinder);
    }
    catch (const Exception& ex)
    {
        std::vector<std::pair<Span, boost::uuids::uuid>> references;
        references.push_back(std::make_pair(ex.Defined(), ex.DefinedModuleId()));
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception("error in XML attribute generation: could not create 'GetPtrs' function for the class '" + ToUtf8(classTypeSymbol->FullName()) + "': " + ex.Message(),
            classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId(), references);
    }
}

void XmlAttributeProcessor::CompileMemberFunction(MemberFunctionSymbol* memberFunctionSymbol, CompoundStatementNode& compoundStatementNode, MemberFunctionNode& memberFunctionNode,
    std::unique_ptr<BoundFunction>&& boundFunction, StatementBinder* statementBinder)
{
    SymbolTable& symbolTable = statementBinder->GetBoundCompileUnit().GetSymbolTable();
    symbolTable.BeginContainer(memberFunctionSymbol);
    SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
    compoundStatementNode.Accept(symbolCreatorVisitor);
    symbolTable.EndContainer();
    TypeBinder typeBinder(statementBinder->GetBoundCompileUnit());
    ContainerScope* containerScope = statementBinder->GetContainerScope();
    typeBinder.SetContainerScope(containerScope);
    statementBinder->SetContainerScope(memberFunctionSymbol->GetContainerScope());
    typeBinder.SetCurrentFunctionSymbol(memberFunctionSymbol);
    compoundStatementNode.Accept(typeBinder);
    BoundFunction* prevFunction = statementBinder->CurrentFunction();
    statementBinder->SetCurrentFunction(boundFunction.get());
    MemberFunctionSymbol* prevMemberFunctionSymbol = statementBinder->CurrentMemberFunctionSymbol();
    MemberFunctionNode* prevMemberFunctionNode = statementBinder->CurrentMemberFunctionNode();
    statementBinder->SetCurrentMemberFunction(memberFunctionSymbol, &memberFunctionNode);
    compoundStatementNode.Accept(*statementBinder);
    statementBinder->SetContainerScope(containerScope);
    BoundStatement* boundStatement = statementBinder->ReleaseStatement();
    Assert(boundStatement->GetBoundNodeType() == BoundNodeType::boundCompoundStatement, "bound compound statement expected");
    BoundCompoundStatement* compoundStatement = static_cast<BoundCompoundStatement*>(boundStatement);
    boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(compoundStatement));
    statementBinder->CurrentClass()->AddMember(std::move(boundFunction));
    statementBinder->SetCurrentMemberFunction(prevMemberFunctionSymbol, prevMemberFunctionNode);
    statementBinder->SetCurrentFunction(prevFunction);
    statementBinder->GetBoundCompileUnit().RemoveLastFileScope();
}

} } // namespace cmajor::binder
