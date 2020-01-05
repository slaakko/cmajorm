// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/Uuid.hpp>
#include <soulng/util/Prime.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

int32_t GetClassIdVmtIndexOffset()
{
    return 0;
}

int32_t GetTypeIdVmtIndexOffset()
{
    return 1;
}

int32_t GetClassNameVmtIndexOffset()
{
    if (GetBackEnd() == BackEnd::llvm)
    {
        return 3;
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int32_t GetImtsVmtIndexOffset()
{
    if (GetBackEnd() == BackEnd::llvm)
    {
        return 4;
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

int32_t GetFunctionVmtIndexOffset()
{
    if (GetBackEnd() == BackEnd::llvm)
    {
        return 5;
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

ClassGroupTypeSymbol::ClassGroupTypeSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(SymbolType::classGroupTypeSymbol, span_, name_)
{
}

void* ClassGroupTypeSymbol::IrType(Emitter& emitter)
{
    Assert(false, "tried to get ir type of class group");
    return nullptr;
}

void* ClassGroupTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    Assert(false, "tried to create default ir value of class group");
    return nullptr;
}

void ClassGroupTypeSymbol::AddClass(ClassTypeSymbol* classTypeSymbol)
{
    for (int arity = classTypeSymbol->MinArity(); arity <= classTypeSymbol->MaxArity(); ++arity)
    {
        if (arityClassMap.find(arity) != arityClassMap.cend())
        {
            throw Exception(GetRootModuleForCurrentThread(), "already has class with arity " + std::to_string(arity) + " in class group '" + ToUtf8(Name()) + "'", GetSpan(), classTypeSymbol->GetSpan());
        }
        arityClassMap[arity] = classTypeSymbol;
    }
}

ClassTypeSymbol* ClassGroupTypeSymbol::GetClass(int arity) const
{
    auto it = arityClassMap.find(arity);
    if (it != arityClassMap.cend())
    {
        ClassTypeSymbol* classTypeSymbol = it->second;
        return classTypeSymbol;
    }
    return nullptr;
}

bool ClassGroupTypeSymbol::HasProjectMembers() const
{
    for (const auto& p : arityClassMap)
    {
        ClassTypeSymbol* cls = p.second;
        if (cls->IsProject())
        {
            return true;
        }
    }
    return false;
}

void ClassGroupTypeSymbol::AppendChildElements(sngxml::dom::Element* element, TypeMap& typeMap) const
{
    for (const auto& p : arityClassMap)
    {
        ClassTypeSymbol* cls = p.second;
        std::unique_ptr<sngxml::dom::Element> classElement = cls->ToDomElement(typeMap);
        if (cls->IsProject())
        {
            element->AppendChild(std::unique_ptr<sngxml::dom::Node>(classElement.release()));
        }
    }
}

void ClassGroupTypeSymbol::Check()
{
    TypeSymbol::Check();
    for (const auto& p : arityClassMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException(GetRootModuleForCurrentThread(), "class group type symbol has no class type symbol", GetSpan());
        }
    }
}

ClassTypeSymbol::ClassTypeSymbol(const Span& span_, const std::u32string& name_) : 
    TypeSymbol(SymbolType::classTypeSymbol, span_, name_), 
    minArity(0), baseClass(), flags(ClassTypeSymbolFlags::none), implementedInterfaces(), templateParameters(), memberVariables(), staticMemberVariables(),
    staticConstructor(nullptr), defaultConstructor(nullptr), copyConstructor(nullptr), moveConstructor(nullptr), copyAssignment(nullptr), moveAssignment(nullptr), 
    constructors(), destructor(nullptr), memberFunctions(), vmtPtrIndex(-1), prototype(nullptr)
{
}

ClassTypeSymbol::ClassTypeSymbol(SymbolType symbolType_, const Span& span_, const std::u32string& name_) :
    TypeSymbol(symbolType_, span_, name_),
    minArity(0), baseClass(), flags(ClassTypeSymbolFlags::none), implementedInterfaces(), templateParameters(), memberVariables(), staticMemberVariables(),
    staticConstructor(nullptr), defaultConstructor(nullptr), copyConstructor(nullptr), moveConstructor(nullptr), copyAssignment(nullptr), moveAssignment(nullptr), 
    constructors(), destructor(nullptr), memberFunctions(), vmtPtrIndex(-1), prototype(nullptr)
{
}

void ClassTypeSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(groupName);
    writer.GetBinaryWriter().Write(static_cast<uint8_t>(flags));
    writer.GetBinaryWriter().Write(static_cast<int32_t>(minArity));
    if (IsClassTemplate())
    {
        usingNodes.Write(writer.GetAstWriter());
        Node* node = GetRootModuleForCurrentThread()->GetSymbolTable().GetNode(this);
        writer.GetAstWriter().Write(node);
        bool hasPrototype = prototype != nullptr;
        writer.GetBinaryWriter().Write(hasPrototype);
        if (hasPrototype)
        {
            writer.GetBinaryWriter().Write(prototype->TypeId());
        }
    }
    boost::uuids::uuid baseClassId = boost::uuids::nil_generator()();
    if (baseClass)
    {
        baseClassId = baseClass->TypeId();
    }
    writer.GetBinaryWriter().Write(baseClassId);
    uint32_t n = uint32_t(implementedInterfaces.size());
    writer.GetBinaryWriter().WriteULEB128UInt(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        InterfaceTypeSymbol* intf = implementedInterfaces[i];
        const boost::uuids::uuid& intfTypeId = intf->TypeId();
        writer.GetBinaryWriter().Write(intfTypeId);
    }
    bool hasDefaultConstructor = defaultConstructor != nullptr && !defaultConstructor->IsGeneratedFunction();
    writer.GetBinaryWriter().Write(hasDefaultConstructor);
    if (hasDefaultConstructor)
    {
        writer.GetBinaryWriter().Write(defaultConstructor->FunctionId());
    }
    bool hasCopyConstructor = copyConstructor != nullptr && !copyConstructor->IsGeneratedFunction();
    writer.GetBinaryWriter().Write(hasCopyConstructor);
    if (hasCopyConstructor)
    {
        writer.GetBinaryWriter().Write(copyConstructor->FunctionId());
    }
    bool hasMoveConstructor = moveConstructor != nullptr && !moveConstructor->IsGeneratedFunction();
    writer.GetBinaryWriter().Write(hasMoveConstructor);
    if (hasMoveConstructor)
    {
        writer.GetBinaryWriter().Write(moveConstructor->FunctionId());
    }
    bool hasCopyAssignment = copyAssignment != nullptr && !copyAssignment->IsGeneratedFunction();
    writer.GetBinaryWriter().Write(hasCopyAssignment);
    if (hasCopyAssignment)
    {
        writer.GetBinaryWriter().Write(copyAssignment->FunctionId());
    }
    bool hasMoveAssignment = moveAssignment != nullptr && !moveAssignment->IsGeneratedFunction();
    writer.GetBinaryWriter().Write(hasMoveAssignment);
    if (hasMoveAssignment)
    {
        writer.GetBinaryWriter().Write(moveAssignment->FunctionId());
    }
    uint32_t vmtSize = vmt.size();
    writer.GetBinaryWriter().WriteULEB128UInt(vmtSize);
    writer.GetBinaryWriter().Write(vmtPtrIndex);
    bool hasConstraint = constraint != nullptr;
    writer.GetBinaryWriter().Write(hasConstraint);
    if (hasConstraint)
    {
        writer.GetAstWriter().Write(constraint.get());
    }
    uint32_t oln = objectLayout.size();
    writer.GetBinaryWriter().WriteULEB128UInt(oln);
    for (TypeSymbol* type : objectLayout)
    {
        writer.GetBinaryWriter().Write(type->TypeId());
    }
    uint32_t sln = staticLayout.size();
    writer.GetBinaryWriter().WriteULEB128UInt(sln);
    for (TypeSymbol* type : staticLayout)
    {
        writer.GetBinaryWriter().Write(type->TypeId());
    }
}

const int prototypeIndex = 1000000;
const int defaultConstructorIndex = 1000001;
const int copyConstructorIndex = 1000002;
const int moveConstructorIndex = 1000003;
const int copyAssignmentIndex = 1000004;
const int moveAssignmentIndex = 1000005;
const int objectLayoutIndex = 0x10000;
const int staticLayoutIndex = 0x20000;
const int maxObjectLayoutSize = staticLayoutIndex - objectLayoutIndex;
const int maxStaticLayoutSize = maxObjectLayoutSize;

void ClassTypeSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    groupName = reader.GetBinaryReader().ReadUtf32String();
    flags = static_cast<ClassTypeSymbolFlags>(reader.GetBinaryReader().ReadByte());
    minArity = reader.GetBinaryReader().ReadInt();
    if (IsClassTemplate())
    {
        usingNodes.Read(reader.GetAstReader());
        Node* node = reader.GetAstReader().ReadNode();
        Assert(node->GetNodeType() == NodeType::classNode, "class node expected");
        ClassNode* clsNode = static_cast<ClassNode*>(node);
        classNode.reset(clsNode);
        bool hasPrototype = reader.GetBinaryReader().ReadBool();
        if (hasPrototype)
        {
            boost::uuids::uuid prototypeId;
            reader.GetBinaryReader().ReadUuid(prototypeId);
            reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, prototypeId, prototypeIndex);
        }
    }
    boost::uuids::uuid baseClassId;
    reader.GetBinaryReader().ReadUuid(baseClassId);
    if (!baseClassId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, baseClassId, 0);
    }
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    implementedInterfaces.resize(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        boost::uuids::uuid intfTypeId;
        reader.GetBinaryReader().ReadUuid(intfTypeId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, intfTypeId, 1 + i);
    }
    bool hasDefaultConstructor = reader.GetBinaryReader().ReadBool();
    if (hasDefaultConstructor)
    {
        boost::uuids::uuid defaultConstructorId;
        reader.GetBinaryReader().ReadUuid(defaultConstructorId);
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, defaultConstructorId, defaultConstructorIndex);
    }
    bool hasCopyConstructor = reader.GetBinaryReader().ReadBool();
    if (hasCopyConstructor)
    {
        boost::uuids::uuid copyConstructorId;
        reader.GetBinaryReader().ReadUuid(copyConstructorId);
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, copyConstructorId, copyConstructorIndex);
    }
    bool hasMoveConstructor = reader.GetBinaryReader().ReadBool();
    if (hasMoveConstructor)
    {
        boost::uuids::uuid moveConstructorId;
        reader.GetBinaryReader().ReadUuid(moveConstructorId);
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, moveConstructorId, moveConstructorIndex);
    }
    bool hasCopyAssignment = reader.GetBinaryReader().ReadBool();
    if (hasCopyAssignment)
    {
        boost::uuids::uuid copyAssignmentId;
        reader.GetBinaryReader().ReadUuid(copyAssignmentId);
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, copyAssignmentId, copyAssignmentIndex);
    }
    bool hasMoveAssignment = reader.GetBinaryReader().ReadBool();
    if (hasMoveAssignment)
    {
        boost::uuids::uuid moveAssignmentId;
        reader.GetBinaryReader().ReadUuid(moveAssignmentId);
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, moveAssignmentId, moveAssignmentIndex);
    }
    uint32_t vmtSize = reader.GetBinaryReader().ReadULEB128UInt();
    vmt.resize(vmtSize);
    if (destructor)
    {
        if (destructor->VmtIndex() != -1)
        {
            Assert(destructor->VmtIndex() < vmt.size(), "invalid destructor vmt index");
            vmt[destructor->VmtIndex()] = destructor;
        }
    }
    for (FunctionSymbol* memberFunction : memberFunctions)
    {
        if (memberFunction->VmtIndex() != -1)
        {
            Assert(memberFunction->VmtIndex() < vmt.size(), "invalid member function vmt index");
            vmt[memberFunction->VmtIndex()] = memberFunction;
        }
    }
    vmtPtrIndex = reader.GetBinaryReader().ReadInt();
    bool hasConstraint = reader.GetBinaryReader().ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.GetAstReader().ReadConstraintNode());
    }
    uint32_t oln = reader.GetBinaryReader().ReadULEB128UInt();
    objectLayout.resize(oln);
    for (uint32_t i = 0; i < oln; ++i)
    {
        boost::uuids::uuid typeId;
        reader.GetBinaryReader().ReadUuid(typeId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, objectLayoutIndex + i);
    }
    uint32_t sln = reader.GetBinaryReader().ReadULEB128UInt();
    staticLayout.resize(sln);
    for (uint32_t i = 0; i < sln; ++i)
    {
        boost::uuids::uuid typeId;
        reader.GetBinaryReader().ReadUuid(typeId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, staticLayoutIndex + i);
    }
    if (IsPolymorphic() && !IsPrototypeTemplateSpecialization())
    {
        reader.GetSymbolTable()->AddPolymorphicClass(this);
    }
    if (StaticConstructor())
    {
        reader.GetSymbolTable()->AddClassHavingStaticConstructor(this);
    }
}

void ClassTypeSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 0)
    {
        Assert(typeSymbol->GetSymbolType() == SymbolType::classTypeSymbol || typeSymbol->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type symbol expected");
        baseClass = static_cast<ClassTypeSymbol*>(typeSymbol);
    }
    else if (index >= 1)
    {
        if (index == prototypeIndex)
        {
            Assert(typeSymbol->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class template specialization expected");
            SetPrototype(static_cast<ClassTemplateSpecializationSymbol*>(typeSymbol));
        }
        else if (index < objectLayoutIndex)
        {
            Assert(typeSymbol->GetSymbolType() == SymbolType::interfaceTypeSymbol, "interface type symbol expected");
            InterfaceTypeSymbol* interfaceTypeSymbol = static_cast<InterfaceTypeSymbol*>(typeSymbol);
            implementedInterfaces[index - 1] = interfaceTypeSymbol;
        }
        else if (index >= objectLayoutIndex && index < objectLayoutIndex + maxObjectLayoutSize)
        {
            objectLayout[index - objectLayoutIndex] = typeSymbol;
        }
        else if (index >= staticLayoutIndex && index < staticLayoutIndex + maxStaticLayoutSize)
        {
            staticLayout[index - staticLayoutIndex] = typeSymbol;
        }
        else
        {
            throw std::runtime_error("internal error: invalid class type emplace type index");
        }
    }
}

void ClassTypeSymbol::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    switch (index)
    {
        case defaultConstructorIndex: defaultConstructor = static_cast<ConstructorSymbol*>(functionSymbol); break;
        case copyConstructorIndex: copyConstructor = static_cast<ConstructorSymbol*>(functionSymbol); break;
        case moveConstructorIndex: moveConstructor = static_cast<ConstructorSymbol*>(functionSymbol); break;
        case copyAssignmentIndex: copyAssignment = static_cast<MemberFunctionSymbol*>(functionSymbol); break;
        case moveAssignmentIndex: moveAssignment = static_cast<MemberFunctionSymbol*>(functionSymbol); break;
        default:
        {
            throw std::runtime_error("internal error: invalid class type emplace function index");        
        }
    }
}

void ClassTypeSymbol::AddMember(Symbol* member)
{
    TypeSymbol::AddMember(member);
    switch (member->GetSymbolType())
    {
        case SymbolType::templateParameterSymbol:
        {
            templateParameters.push_back(static_cast<TemplateParameterSymbol*>(member));
            break;
        }
        case SymbolType::memberVariableSymbol:
        {
            if (member->IsStatic())
            {
                staticMemberVariables.push_back(static_cast<MemberVariableSymbol*>(member));
            }
            else
            {
                memberVariables.push_back(static_cast<MemberVariableSymbol*>(member));
            }
            break;
        }
        case SymbolType::staticConstructorSymbol:
        {
            if (staticConstructor)
            {
                throw Exception(GetRootModuleForCurrentThread(), "already has a static constructor", member->GetSpan(), staticConstructor->GetSpan());
            }
            else
            {
                staticConstructor = static_cast<StaticConstructorSymbol*>(member);
            }
            break;
        }
        case SymbolType::constructorSymbol:
        {
            ConstructorSymbol* constructor = static_cast<ConstructorSymbol*>(member);
            constructors.push_back(constructor);
            break;
        }
        case SymbolType::destructorSymbol:
        {
            if (destructor)
            {
                throw Exception(GetRootModuleForCurrentThread(), "already has a destructor", member->GetSpan(), destructor->GetSpan());
            }
            else
            {
                destructor = static_cast<DestructorSymbol*>(member);
            }
            break;
        }
        case SymbolType::memberFunctionSymbol:
        {
            memberFunctions.push_back(static_cast<MemberFunctionSymbol*>(member));
            break;
        }
    }
}

std::string ClassTypeSymbol::GetSpecifierStr() const
{
    std::string specifierStr;
    if (IsAbstract())
    {
        specifierStr.append("abstract");
    }
    std::string baseSpecifierStr = TypeSymbol::GetSpecifierStr();
    if (!baseSpecifierStr.empty())
    {
        specifierStr.append(1, ' ');
    }
    specifierStr.append(baseSpecifierStr);
    return specifierStr;
}

bool ClassTypeSymbol::HasNontrivialDestructor() const
{
    if (destructor || IsPolymorphic()) return true;
    if (baseClass && baseClass->Destructor()) return true;
    int n = memberVariables.size();
    for (int i = 0; i < n; ++i)
    {
        MemberVariableSymbol* memberVariable = memberVariables[i];
        if (memberVariable->GetType()->HasNontrivialDestructor())
        {
            return true;
        }
    }
    return false;
}

void ClassTypeSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject())
    {
        if (prototype)
        {
            prototype->SetParent(Ns());
            if (Constraint())
            {
                CloneContext cloneContext;
                prototype->SetConstraint(static_cast<ConstraintNode*>(Constraint()->Clone(cloneContext)));
            }
            collector->AddClass(prototype);
        }
        else
        {
            if (Access() == SymbolAccess::public_)
            {
                collector->AddClass(this);
            }
        }
    }
}

void ClassTypeSymbol::CollectMembers(SymbolCollector* collector)
{
    TypeSymbol::Accept(collector);
}

void ClassTypeSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("group name: " + ToUtf8(groupName));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    if (baseClass)
    {
        formatter.WriteLine("base class: " + ToUtf8(baseClass->FullName()));
    }
    if (!implementedInterfaces.empty())
    {
        formatter.WriteLine("implemented interfaces:");
        formatter.IncIndent();
        for (InterfaceTypeSymbol* interface : implementedInterfaces)
        {
            formatter.WriteLine(ToUtf8(interface->FullName()));
        }
        formatter.DecIndent();
    }
    if (constraint)
    {
        formatter.WriteLine("constraint: " + constraint->ToString());
    }
    formatter.WriteLine("typeid: " + boost::uuids::to_string(TypeId()));
/*
    if (IsPolymorphic())
    {
        formatter.WriteLine("vmt object name: " + VmtObjectName());
    }
*/
/*
    if (!staticLayout.empty())
    {
        formatter.WriteLine("static object name: " + StaticObjectName());
    }
*/
    formatter.IncIndent();
    SymbolCollector collector;
    TypeSymbol::Accept(&collector);
    if (!collector.Functions().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("MEMBER FUNCTIONS");
        for (FunctionSymbol* function : collector.Functions())
        {
            formatter.WriteLine();
            function->Dump(formatter);
        }
    }
    if (!collector.Classes().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CLASSES");
        for (ClassTypeSymbol* class_ : collector.Classes())
        {
            formatter.WriteLine();
            class_->Dump(formatter);
        }
    }
    if (!collector.Interfaces().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("INTERFACES");
        for (InterfaceTypeSymbol* interface : collector.Interfaces())
        {
            formatter.WriteLine();
            interface->Dump(formatter);
        }
    }
    if (!collector.Typedefs().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("TYPEDEFS");
        for (TypedefSymbol* typedef_ : collector.Typedefs())
        {
            formatter.WriteLine();
            typedef_->Dump(formatter);
        }
    }
    if (!collector.Concepts().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CONCEPTS");
        for (ConceptSymbol* concept_ : collector.Concepts())
        {
            formatter.WriteLine();
            concept_->Dump(formatter);
        }
    }
    if (!collector.Constants().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CONSTANTS");
        for (ConstantSymbol* constant : collector.Constants())
        {
            formatter.WriteLine();
            constant->Dump(formatter);
        }
    }
    if (!collector.Delegates().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("DELEGATES");
        for (DelegateTypeSymbol* delegate_ : collector.Delegates())
        {
            formatter.WriteLine();
            delegate_->Dump(formatter);
        }
    }
    if (!collector.ClassDelegates().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CLASS DELEGATES");
        for (ClassDelegateTypeSymbol* classDelegate : collector.ClassDelegates())
        {
            formatter.WriteLine();
            classDelegate->Dump(formatter);
        }
    }
    if (!collector.EnumeratedTypes().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("ENUMERATED TYPES");
        for (EnumTypeSymbol* enumeratedType : collector.EnumeratedTypes())
        {
            formatter.WriteLine();
            enumeratedType->Dump(formatter);
        }
    }
    if (!collector.MemberVariables().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("MEMBER VARIABLES");
        for (MemberVariableSymbol* memberVariable : collector.MemberVariables())
        {
            formatter.WriteLine();
            memberVariable->Dump(formatter);
        }
    }
    formatter.DecIndent();
}

bool ClassTypeSymbol::IsRecursive(TypeSymbol* type, std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>>& tested) 
{
    if (tested.find(TypeId()) != tested.cend())
    {
        bool equal = TypesEqual(type, this);
        return equal;
    }
    tested.insert(TypeId());
    if (TypeSymbol::IsRecursive(type, tested))
    {
        return true;
    }
    if (baseClass && baseClass->IsRecursive(type, tested))
    {
        return true;
    }
    for (MemberVariableSymbol* memberVariable : memberVariables)
    {
        if (memberVariable->GetType()->IsRecursive(type, tested))
        {
            return true;
        }
    }
    return false;
}

void ClassTypeSymbol::CreateDestructorSymbol()
{
    if (!destructor)
    {
        DestructorSymbol* destructorSymbol = new DestructorSymbol(GetSpan(), U"@destructor");
        destructorSymbol->SetModule(GetModule());
        GetModule()->GetSymbolTable().SetFunctionIdFor(destructorSymbol);
        destructorSymbol->SetGenerated();
        ParameterSymbol* thisParam = new ParameterSymbol(GetSpan(), U"this");
        thisParam->SetType(AddPointer(GetSpan()));
        destructorSymbol->SetAccess(SymbolAccess::public_);
        destructorSymbol->AddMember(thisParam);
        AddMember(destructorSymbol);
        Assert(destructor, "destructor expected");
        if (GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            destructor->SetLinkOnceOdrLinkage();
        }
        destructor->ComputeName();
    }
}

void ClassTypeSymbol::SetGroupName(const std::u32string& groupName_)
{
    groupName = groupName_;
}

void ClassTypeSymbol::ComputeMinArity()
{
    bool defaultHit = false;
    int n = templateParameters.size();
    for (int i = 0; i < n; ++i)
    {
        TemplateParameterSymbol* templateParameter = templateParameters[i];
        if (templateParameter->HasDefault())
        {
            defaultHit = true;
            break;
        }
        minArity = i;
    }
    if (!defaultHit)
    {
        minArity = n;
    }
}

bool ClassTypeSymbol::HasBaseClass(ClassTypeSymbol* cls) const
{
    if (!baseClass) return false;
    if (TypesEqual(baseClass, cls) || baseClass->HasBaseClass(cls)) return true;
    return false;
}

bool ClassTypeSymbol::HasBaseClass(ClassTypeSymbol* cls, uint8_t& distance) const
{
    if (!baseClass) return false;
    ++distance;
    if (TypesEqual(baseClass, cls)) return true;
    return baseClass->HasBaseClass(cls, distance);
}

void ClassTypeSymbol::AddImplementedInterface(InterfaceTypeSymbol* interfaceTypeSymbol)
{
    int n = implementedInterfaces.size();
    for (int i = 0; i < n; ++i)
    {
        if (implementedInterfaces[i] == interfaceTypeSymbol)
        {
            throw Exception(GetRootModuleForCurrentThread(), "class cannot implement an interface more than once", GetSpan(), interfaceTypeSymbol->GetSpan());
        }
    }
    implementedInterfaces.push_back(interfaceTypeSymbol);
}

void ClassTypeSymbol::CloneUsingNodes(const std::vector<Node*>& usingNodes_)
{
    CloneContext cloneContext;
    for (Node* usingNode : usingNodes_)
    {
        usingNodes.Add(usingNode->Clone(cloneContext));
    }
}

void ClassTypeSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        SetStatic();
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        SetAbstract();
        SetPolymorphic();
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be nothrow", GetSpan());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be throw", GetSpan());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "class type symbol cannot be unit_test", GetSpan());
    }
}

void ClassTypeSymbol::ComputeName()
{
    std::u32string name = Name();
    if (IsClassTemplate())
    {
        name.append(1, '<');
        bool first = true;
        for (TemplateParameterSymbol* templateParameter : templateParameters)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                name.append(U", ");
            }
            name.append(templateParameter->Name());
        }
        name.append(1, '>');
    }
    SetName(name);
    ComputeMangledName();
}

void ClassTypeSymbol::ComputeMangledName()
{
    std::u32string mangledName = ToUtf32(TypeString());
    mangledName.append(1, U'_').append(SimpleName());
    std::string constraintStr;
    if (constraint)
    {
        constraintStr = " " + constraint->ToString();
    }
    mangledName.append(1, U'_').append(ToUtf32(GetSha1MessageDigest(ToUtf8(FullNameWithSpecifiers()) + constraintStr)));
    SetMangledName(mangledName);
}

void ClassTypeSymbol::SetSpecialMemberFunctions()
{
    int nc = constructors.size();
    for (int i = 0; i < nc; ++i)
    {
        ConstructorSymbol* constructor = constructors[i];
        if (constructor->IsDefaultConstructor())
        {
            defaultConstructor = constructor;
        }
        else if (constructor->IsCopyConstructor())
        {
            copyConstructor = constructor;
        }
        else if (constructor->IsMoveConstructor())
        {
            moveConstructor = constructor;
        }
    }
    int nm = memberFunctions.size();
    for (int i = 0; i < nm; ++i)
    {
        MemberFunctionSymbol* memberFunction = memberFunctions[i];
        if (memberFunction->IsCopyAssignment())
        {
            copyAssignment = memberFunction;
        }
        else if (memberFunction->IsMoveAssignment())
        {
            moveAssignment = memberFunction;
        }
    }
}

bool ClassTypeSymbol::IsLiteralClassType() const
{
    if (IsPolymorphic()) return false;
    if (HasNontrivialDestructor()) return false;
    if (!StaticMemberVariables().empty()) return false;
    if (BaseClass()) return false;
    return true;
}

void ClassTypeSymbol::SetInitializedVar(MemberVariableSymbol* initializedVar_) 
{ 
    initializedVar.reset(initializedVar_); 
}

void ClassTypeSymbol::InitVmt()
{
    if (IsVmtInitialized()) return;
    SetVmtInitialized();
    if (baseClass)
    {
        baseClass->InitVmt();
        if (baseClass->IsPolymorphic())
        {
            SetPolymorphic();
        }
    }
    if (destructor && (destructor->IsVirtual() || destructor->IsOverride()))
    {
        SetPolymorphic();
    }
    for (MemberFunctionSymbol* memberFunction : memberFunctions)
    {
        if (memberFunction->IsVirtualAbstractOrOverride())
        {
            SetPolymorphic();
            break;
        }
    }
    if (!implementedInterfaces.empty())
    {
        SetPolymorphic();
    }
    if (IsPolymorphic())
    {
        CreateDestructorSymbol();
        if (baseClass && baseClass->IsPolymorphic())
        {
            destructor->SetOverride();
        }
        else
        {
            destructor->SetVirtual();
        }
    }
    if (IsPolymorphic())
    {
        if (!baseClass || !baseClass->IsPolymorphic())
        {
            if (baseClass)
            {
                vmtPtrIndex = 1;
            }
            else
            {
                vmtPtrIndex = 0;
            }
        }
        InitVmt(vmt);
        for (FunctionSymbol* virtualFunction : vmt)
        {
            if (virtualFunction->IsAbstract())
            {
                if (!IsAbstract())
                {
                    throw Exception(GetRootModuleForCurrentThread(), "class containing abstract member functions must be declared abstract", GetSpan(), virtualFunction->GetSpan());
                }
            }
        }
    }
}

bool Overrides(FunctionSymbol* f, FunctionSymbol* g)
{
    if (f->GroupName() == g->GroupName())
    {
        int n = f->Parameters().size();
        if (n == g->Parameters().size())
        {
            for (int i = 1; i < n; ++i)
            {
                ParameterSymbol* p = f->Parameters()[i];
                ParameterSymbol* q = g->Parameters()[i];
                if (!TypesEqual(p->GetType(), q->GetType())) return false;
            }
            return true;
        }
    }
    return false;
}

void ClassTypeSymbol::InitVmt(std::vector<FunctionSymbol*>& vmtToInit)
{
    if (!IsPolymorphic()) return;
    if (baseClass)
    {
        baseClass->InitVmt(vmtToInit);
    }
    std::vector<FunctionSymbol*> virtualFunctions;
    if (destructor)
    {
        if (destructor->IsVirtual() || destructor->IsOverride())
        {
            virtualFunctions.push_back(destructor);
        }
    }
    for (FunctionSymbol* memberFunction : memberFunctions)
    {
        if (memberFunction->IsVirtualAbstractOrOverride())
        {
            virtualFunctions.push_back(memberFunction);
        }
    }
    int n = virtualFunctions.size();
    for (int i = 0; i < n; ++i)
    {
        FunctionSymbol* f = virtualFunctions[i];
        bool found = false;
        int m = vmtToInit.size();
        for (int j = 0; j < m; ++j)
        {
            FunctionSymbol* v = vmtToInit[j];
            if (Overrides(f, v))
            {
                if (!f->IsOverride())
                {
                    throw Exception(GetRootModuleForCurrentThread(), "overriding function should be declared with override specifier", f->GetSpan());
                }
                if (f->DontThrow() != v->DontThrow())
                {
                    throw Exception(GetRootModuleForCurrentThread(), "overriding function has conflicting nothrow specification compared to the base class virtual function", f->GetSpan(), v->GetSpan());
                }
                f->SetVmtIndex(j);
                vmtToInit[j] = f;
                found = true;
                break;
            }
        }
        if (!found)
        {
            if (f->IsOverride())
            {
                throw Exception(GetRootModuleForCurrentThread(), "no suitable function to override ('" + ToUtf8(f->FullName()) + "')", f->GetSpan());
            }
            f->SetVmtIndex(m);
            vmtToInit.push_back(f);
        }
    }
}

bool Implements(MemberFunctionSymbol* classMemFun, MemberFunctionSymbol* intfMemFun)
{
    if (classMemFun->GroupName() != intfMemFun->GroupName()) return false;
    if (!classMemFun->ReturnType() || !intfMemFun->ReturnType()) return false;
    if (classMemFun->ReturnType() != intfMemFun->ReturnType()) return false;
    int n = classMemFun->Parameters().size();
    if (n != intfMemFun->Parameters().size()) return false;
    for (int i = 1; i < n; ++i)
    {
        TypeSymbol* classMemFunParamType = classMemFun->Parameters()[i]->GetType();
        TypeSymbol* intfMemFunParamType = intfMemFun->Parameters()[i]->GetType();
        if (!TypesEqual(classMemFunParamType, intfMemFunParamType)) return false;
    }
    return true;
}

void ClassTypeSymbol::InitImts()
{
    if (IsImtsInitialized()) return;
    SetImtsInitialized();
    int n = implementedInterfaces.size();
    if (n == 0) return;
    imts.resize(n);
    for (int32_t i = 0; i < n; ++i)
    {
        std::vector<FunctionSymbol*>& imt = imts[i];
        InterfaceTypeSymbol* intf = implementedInterfaces[i];
        int q = intf->MemberFunctions().size();
        imt.resize(q);
    }
    int m = memberFunctions.size();
    for (int j = 0; j < m; ++j)
    {
        MemberFunctionSymbol* classMemFun = memberFunctions[j];
        for (int32_t i = 0; i < n; ++i)
        {
            std::vector<FunctionSymbol*>& imt = imts[i];
            InterfaceTypeSymbol* intf = implementedInterfaces[i];
            int q = intf->MemberFunctions().size();
            for (int k = 0; k < q; ++k)
            {
                MemberFunctionSymbol* intfMemFun = intf->MemberFunctions()[k];
                if (Implements(classMemFun, intfMemFun))
                {
                    imt[intfMemFun->ImtIndex()] = classMemFun;
                    break;
                }
            }
        }
    }
    for (int i = 0; i < n; ++i)
    {
        InterfaceTypeSymbol* intf = implementedInterfaces[i];
        std::vector<FunctionSymbol*>& imt = imts[i];
        int m = imt.size();
        for (int j = 0; j < m; ++j)
        {
            if (!imt[j])
            {
                MemberFunctionSymbol* intfMemFun = intf->MemberFunctions()[j];
                throw Exception(GetRootModuleForCurrentThread(), "class '" + ToUtf8(FullName()) + "' does not implement interface '" + ToUtf8(intf->FullName()) + "' because implementation of interface function '" +
                    ToUtf8(intfMemFun->FullName()) + "' is missing", GetSpan(), intfMemFun->GetSpan());
            }
        }
    }
}

void ClassTypeSymbol::CreateLayouts()
{
    if (IsLayoutsComputed()) return;
    SetLayoutsComputed();
    if (baseClass)
    {
        objectLayout.push_back(baseClass);
    }
    else
    {
        if (IsPolymorphic())
        {
            vmtPtrIndex = objectLayout.size();
            objectLayout.push_back(GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"void")->AddPointer(GetSpan()));
        }
        else if (memberVariables.empty())
        {
            objectLayout.push_back(GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"byte"));
        }
    }
    int n = memberVariables.size();
    for (int i = 0; i < n; ++i)
    {
        MemberVariableSymbol* memberVariable = memberVariables[i];
        memberVariable->SetLayoutIndex(objectLayout.size());
        objectLayout.push_back(memberVariable->GetType());
    }
    if (!staticMemberVariables.empty() || StaticConstructor())
    {
        MemberVariableSymbol* initVar = new MemberVariableSymbol(GetSpan(), U"@initialized");
        initVar->SetParent(this);
        initVar->SetStatic();
        initVar->SetType(GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"bool"));
        initVar->SetLayoutIndex(0);
        SetInitializedVar(initVar);
        staticLayout.push_back(GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"bool"));
        int ns = staticMemberVariables.size();
        for (int i = 0; i < ns; ++i)
        {
            MemberVariableSymbol* staticMemberVariable = staticMemberVariables[i];
            staticMemberVariable->SetLayoutIndex(staticLayout.size());
            staticLayout.push_back(staticMemberVariable->GetType());
        }
    }
}

bool ClassTypeSymbol::IsRecursive()
{
    if (RecursiveComputed())
    {
        return Recursive();
    }
    else
    {
        int n = objectLayout.size();
        for (int i = 0; i < n; ++i)
        {
            TypeSymbol* elementType = objectLayout[i];
            std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>> tested;
            if (elementType->IsRecursive(this, tested))
            {
                SetRecursive();
                SetRecursiveComputed();
                return true;
            }
        }
        SetRecursiveComputed();
        return false;
    }
}

void* ClassTypeSymbol::IrType(Emitter& emitter)
{
    if (!IsBound())
    {
        throw Exception(GetRootModuleForCurrentThread(), "class '" + ToUtf8(Name()) + "' not bound", GetSpan());
    }
    void* localIrType = emitter.GetIrTypeByTypeId(TypeId());
    if (!localIrType)
    {
        std::vector<void*> elementTypes;
        int n = objectLayout.size();
        if (!IsRecursive())
        {
            for (int i = 0; i < n; ++i)
            {
                TypeSymbol* elementType = objectLayout[i];
                elementTypes.push_back(elementType->IrType(emitter));
            }
            localIrType = emitter.GetIrTypeForClassType(elementTypes);
            emitter.SetIrTypeByTypeId(TypeId(), localIrType);
        }
        else
        {

            void* forwardDeclaredType = emitter.CreateFwdIrTypeForClassType();
            localIrType = forwardDeclaredType;
            emitter.SetIrTypeByTypeId(TypeId(), localIrType);
            for (int i = 0; i < n; ++i)
            {
                TypeSymbol* elementType = objectLayout[i];
                elementTypes.push_back(elementType->IrType(emitter));
            }
            emitter.SetFwdIrTypeBody(forwardDeclaredType, elementTypes);
        }
    }
    return localIrType;
}

void* ClassTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    void* irType = IrType(emitter);
    std::vector<void*> arrayOfDefaults;
    for (TypeSymbol* type : objectLayout)
    {
        arrayOfDefaults.push_back(type->CreateDefaultIrValue(emitter));
    }
    return emitter.CreateDefaultIrValueForStruct(irType, arrayOfDefaults);
}

void* ClassTypeSymbol::CreateDIType(Emitter& emitter)
{
    void* baseClassDIType = nullptr;
    if (baseClass)
    {
        baseClassDIType = baseClass->GetDIType(emitter);
    }
    void* vtableHolderClass = nullptr;
    if (IsPolymorphic() && VmtPtrHolderClass())
    {
        vtableHolderClass = VmtPtrHolderClass()->CreateDIForwardDeclaration(emitter);
        emitter.MapFwdDeclaration(vtableHolderClass, VmtPtrHolderClass()->TypeId());
    }
    std::vector<void*> elements;
    for (MemberVariableSymbol* memberVariable : memberVariables)
    {
        uint64_t offsetInBits = emitter.GetOffsetInBits(IrType(emitter), memberVariable->LayoutIndex());
        elements.push_back(memberVariable->GetDIMemberType(emitter, offsetInBits));
    }
    Span classSpan = GetSpan();
    if (GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(this);
        classSpan = specialization->GetClassTemplate()->GetSpan();
    }
    return emitter.CreateDITypeForClassType(IrType(emitter), elements, classSpan, ToUtf8(Name()), vtableHolderClass, ToUtf8(MangledName()), baseClassDIType);
}

void* ClassTypeSymbol::CreateDIForwardDeclaration(Emitter& emitter)
{
    Span classSpan = GetSpan();
    if (GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(this);
        classSpan = specialization->GetClassTemplate()->GetSpan();
    }
    return emitter.CreateIrDIForwardDeclaration(IrType(emitter), ToUtf8(Name()), ToUtf8(MangledName()), classSpan);
}

std::string ClassTypeSymbol::VmtObjectNameStr()
{
    return "vmt_" + ToUtf8(SimpleName()) + "_" + GetSha1MessageDigest(ToUtf8(FullNameWithSpecifiers()));;
}

std::string ClassTypeSymbol::VmtObjectName(Emitter& emitter)
{
    std::string localVmtObjectName = emitter.GetVmtObjectName(this);
    if (localVmtObjectName.empty())
    {
        localVmtObjectName = VmtObjectNameStr();
        emitter.SetVmtObjectName(this, localVmtObjectName);
    }
    return localVmtObjectName;
}

std::string ClassTypeSymbol::ImtArrayObjectName(Emitter& emitter)
{
    std::string localImtArrayObjectName = emitter.GetImtArrayObjectName(this);
    if (localImtArrayObjectName.empty())
    {
        localImtArrayObjectName = "imts_" + ToUtf8(SimpleName()) + "_" + GetSha1MessageDigest(ToUtf8(FullNameWithSpecifiers()));
        emitter.SetImtArrayObjectName(this, localImtArrayObjectName);
    }
    return localImtArrayObjectName;
}

std::string ClassTypeSymbol::ImtObjectName(int index)
{ 
    return "imt_" + std::to_string(index) + "_" + ToUtf8(SimpleName()) + "_" + GetSha1MessageDigest(ToUtf8(FullNameWithSpecifiers()));
}

ClassTypeSymbol* ClassTypeSymbol::VmtPtrHolderClass() 
{
    if (!IsPolymorphic())
    {
        throw Exception(GetRootModuleForCurrentThread(), "nonpolymorphic class does not contain a vmt ptr", GetSpan());
    }
    if (vmtPtrIndex != -1)
    {
        return this;
    }
    else
    {
        if (baseClass)
        {
            return baseClass->VmtPtrHolderClass();
        }
        else
        {
            throw Exception(GetRootModuleForCurrentThread(), "vmt ptr holder class not found", GetSpan());
        }
    }
}

void* ClassTypeSymbol::VmtPtrType(Emitter& emitter)
{
    void* vmtArrayType = emitter.GetIrTypeForArrayType(emitter.GetIrTypeForVoidPtrType(), vmt.size() + GetFunctionVmtIndexOffset());
    void* vmtPtrType = emitter.GetIrTypeForPtrType(vmtArrayType);
    return vmtPtrType;
}

void* ClassTypeSymbol::CreateImt(Emitter& emitter, int index)
{
    std::vector<FunctionSymbol*>& imt = imts[index];
    std::string imtObjectName = ImtObjectName(index);
    void* imtType = emitter.GetIrTypeForArrayType(emitter.GetIrTypeForVoidPtrType(), imt.size());
    void* imtObject = emitter.GetOrInsertGlobal(imtObjectName, imtType);
    void* comdat = emitter.GetOrInsertAnyComdat(imtObjectName, imtObject);
    std::vector<void*> irImt;
    int n = imt.size();
    for (int i = 0; i < n; ++i)
    {
        FunctionSymbol* memFun = imt[i];
        void* interfaceFun = emitter.GetOrInsertFunction(ToUtf8(memFun->MangledName()), memFun->IrType(emitter));
        irImt.push_back(emitter.CreateBitCast(interfaceFun, emitter.GetIrTypeForVoidPtrType()));
    }
    emitter.SetInitializer(imtObject, emitter.CreateIrValueForConstantArray(imtType, irImt, std::string()));
    return imtObject;
}

void* ClassTypeSymbol::CreateImts(Emitter& emitter)
{
    std::string imtArrayObjectName = ImtArrayObjectName(emitter);
    void* imtsArrayType = emitter.GetIrTypeForArrayType(emitter.GetIrTypeForVoidPtrType(), implementedInterfaces.size());
    void* imtsArrayObject = emitter.GetOrInsertGlobal(imtArrayObjectName, imtsArrayType);
    void* comdat = emitter.GetOrInsertAnyComdat(imtArrayObjectName, imtsArrayObject);
    std::vector<void*> imtsArray;
    int n = imts.size();
    for (int i = 0; i < n; ++i)
    {
        void* irImt = CreateImt(emitter, i);
        imtsArray.push_back(emitter.CreateBitCast(irImt, emitter.GetIrTypeForVoidPtrType()));
    }
    emitter.SetInitializer(imtsArrayObject, emitter.CreateIrValueForConstantArray(imtsArrayType, imtsArray, std::string()));
    return imtsArrayObject;
}

void* ClassTypeSymbol::VmtObject(Emitter& emitter, bool create)
{
    if (!IsPolymorphic()) return nullptr;
    void* localVmtObjectType = emitter.GetVmtObjectType(this);
    if (!localVmtObjectType)
    {
        localVmtObjectType = emitter.GetIrTypeForArrayType(emitter.GetIrTypeForVoidPtrType(), vmt.size() + GetFunctionVmtIndexOffset());
        emitter.SetVmtObjectType(this, localVmtObjectType);
    }
    void* className = nullptr;
    if (!emitter.IsVmtObjectCreated(this) && create)
    {
        className = emitter.CreateGlobalStringPtr(ToUtf8(FullName()));
    }
    void* vmtObject = emitter.GetOrInsertGlobal(VmtObjectName(emitter), localVmtObjectType);
    if (!emitter.IsVmtObjectCreated(this) && create)
    {
        emitter.SetVmtObjectCreated(this);
        std::string vmtObjectName = VmtObjectName(emitter);
        void* comdat = emitter.GetOrInsertAnyComdat(vmtObjectName, vmtObject);
        std::vector<void*> vmtArray;
        if (GetBackEnd() == BackEnd::llvm)
        {
            vmtArray.push_back(emitter.CreateDefaultIrValueForVoidPtrType()); // 64-bit class id, initially 0, dynamically initialized
            uint64_t typeId1 = 0;
            uint64_t typeId2 = 0;
            UuidToInts(TypeId(), typeId1, typeId2);
            //      16-byte type id:
            vmtArray.push_back(emitter.CreateIntToPtr(emitter.CreateIrValueForULong(typeId1), emitter.GetIrTypeForVoidPtrType()));
            vmtArray.push_back(emitter.CreateIntToPtr(emitter.CreateIrValueForULong(typeId2), emitter.GetIrTypeForVoidPtrType()));
            vmtArray.push_back(emitter.CreateBitCast(className, emitter.GetIrTypeForVoidPtrType())); // class name pointer
            if (!implementedInterfaces.empty())
            {
                void* itabsArrayObject = CreateImts(emitter);
                vmtArray.push_back(emitter.CreateBitCast(itabsArrayObject, emitter.GetIrTypeForVoidPtrType())); // interface method table pointer
            }
            else
            {
                vmtArray.push_back(emitter.CreateDefaultIrValueForVoidPtrType());
            }
            //      virtual method table:
            int n = vmt.size();
            for (int i = 0; i < n; ++i)
            {
                FunctionSymbol* virtualFunction = vmt[i];
                if (!virtualFunction || virtualFunction->IsAbstract())
                {
                    vmtArray.push_back(emitter.CreateDefaultIrValueForVoidPtrType());
                }
                else
                {
                    void* functionObject = emitter.GetOrInsertFunction(ToUtf8(virtualFunction->MangledName()), virtualFunction->IrType(emitter));
                    vmtArray.push_back(emitter.CreateBitCast(functionObject, emitter.GetIrTypeForVoidPtrType()));
                }
            }
        }
        else if (GetBackEnd() == BackEnd::cmsx)
        {
            uint64_t typeId1 = 0;
            uint64_t typeId2 = 0;
            std::string typeId;
            for (uint8_t x : TypeId())
            {
                typeId.append(soulng::util::ToHexString(x));
            }
            vmtArray.push_back(emitter.GetClsIdValue(typeId)); // 64-bit class id
            vmtArray.push_back(emitter.GetConversionValue(emitter.GetIrTypeForVoidPtrType(), className)); // class name pointer
            if (!implementedInterfaces.empty())
            {
                void* itabsArrayObject = CreateImts(emitter);
                vmtArray.push_back(emitter.GetConversionValue(emitter.GetIrTypeForVoidPtrType(), itabsArrayObject)); // interface method table pointer
            }
            else
            {
                vmtArray.push_back(emitter.CreateDefaultIrValueForVoidPtrType());
            }
            //      virtual method table:
            int n = vmt.size();
            for (int i = 0; i < n; ++i)
            {
                FunctionSymbol* virtualFunction = vmt[i];
                if (!virtualFunction || virtualFunction->IsAbstract())
                {
                    vmtArray.push_back(emitter.CreateDefaultIrValueForVoidPtrType());
                }
                else
                {
                    void* functionObject = emitter.GetOrInsertFunction(ToUtf8(virtualFunction->MangledName()), virtualFunction->IrType(emitter));
                    vmtArray.push_back(emitter.GetConversionValue(emitter.GetIrTypeForVoidPtrType(), functionObject));
                }
            }
        }
        void* initializer = emitter.CreateIrValueForConstantArray(localVmtObjectType, vmtArray, std::string());
        emitter.SetInitializer(vmtObject, initializer);
    }
    return vmtObject;
}

void* ClassTypeSymbol::StaticObject(Emitter& emitter, bool create)
{
    if (staticLayout.empty()) return nullptr;
    void* staticObject = emitter.GetOrInsertGlobal(StaticObjectName(emitter), StaticObjectType(emitter));
    if (!emitter.IsStaticObjectCreated(this) && create)
    {
        emitter.SetStaticObjectCreated(this);
        void* staticObjectGlobal = staticObject;
        std::vector<void*> arrayOfStatics;
        for (TypeSymbol* type : staticLayout)
        {
            arrayOfStatics.push_back(type->CreateDefaultIrValue(emitter));
        }
        emitter.SetInitializer(staticObjectGlobal, emitter.CreateIrValueForConstantStruct(StaticObjectType(emitter), arrayOfStatics));
    }
    return staticObject;
}

void* ClassTypeSymbol::StaticObjectType(Emitter& emitter)
{
    void* localStaticObjectType = emitter.GetStaticObjectType(this);
    if (!localStaticObjectType)
    {
        std::vector<void*> elementTypes;
        int n = staticLayout.size();
        for (int i = 0; i < n; ++i)
        {
            void* elementType = staticLayout[i]->IrType(emitter);
            elementTypes.push_back(elementType);
        }
        localStaticObjectType = emitter.GetIrTypeForStructType(elementTypes);
    }
    emitter.SetStaticObjectType(this, localStaticObjectType);
    return localStaticObjectType;
}

std::string ClassTypeSymbol::StaticObjectName(Emitter& emitter)
{
    std::string localStaticObjectName = emitter.GetStaticObjectName(this);
    if (localStaticObjectName.empty())
    {
        localStaticObjectName = "statics_" + ToUtf8(SimpleName()) + "_" + GetSha1MessageDigest(ToUtf8(FullNameWithSpecifiers()));
        emitter.SetStaticObjectName(this, localStaticObjectName);
    }
    return localStaticObjectName;
}

ValueType ClassTypeSymbol::GetValueType() const
{
    return ValueType::structuredValue;
}

Value* ClassTypeSymbol::MakeValue() const
{
    std::vector<std::unique_ptr<Value>> memberValues;
    return new StructuredValue(GetSpan(), const_cast<TypeSymbol*>(static_cast<const TypeSymbol*>(this)), std::move(memberValues));
}

std::u32string ClassTypeSymbol::Id() const
{
    if (prototype)
    {
        return prototype->Id();
    }
    else
    {
        return MangledName();
    }
}

void ClassTypeSymbol::Check()
{
    TypeSymbol::Check();
    if (groupName.empty())
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "class type symbol has empty group name", GetSpan());
    }
}

struct ClassInfo
{
    ClassInfo() : cls(nullptr), baseClassInfo(nullptr), level(0), key(0), id(0) {}
    ClassInfo(ClassTypeSymbol* cls_) : cls(cls_), baseClassInfo(nullptr), level(0), key(0), id(0) {}
    ClassTypeSymbol* cls;
    ClassInfo* baseClassInfo;
    int level;
    uint64_t key;
    uint64_t id;
};

void ResolveBaseClasses(std::unordered_map<boost::uuids::uuid, ClassInfo, boost::hash<boost::uuids::uuid>>& classIdMap)
{
    for (auto& p : classIdMap)
    {
        ClassInfo& info = p.second;
        ClassTypeSymbol* baseClass = info.cls->BaseClass();
        if (baseClass)
        {
            auto it = classIdMap.find(baseClass->TypeId());
            if (it != classIdMap.cend())
            {
                ClassInfo* baseClassInfo = &it->second;
                info.baseClassInfo = baseClassInfo;
            }
            else
            {
                throw Exception(GetRootModuleForCurrentThread(), "internal error: could not resolve base class info for class '" + ToUtf8(info.cls->FullName()) + "'",
                    info.cls->GetSpan());
            }
        }
    }
}

int NumberOfAncestors(ClassInfo& cls)
{
    int numAncestors = 0;
    ClassInfo* baseClass = cls.baseClassInfo;
    while (baseClass)
    {
        ++numAncestors;
        baseClass = baseClass->baseClassInfo;
    }
    return numAncestors;
}

void AssignLevels(std::unordered_map<boost::uuids::uuid, ClassInfo, boost::hash<boost::uuids::uuid>>& classIdMap)
{
    for (auto& p : classIdMap)
    {
        ClassInfo& cls = p.second;
        cls.level = NumberOfAncestors(cls);
    }
}

struct PriorityGreater
{
    bool operator()(ClassInfo* left, ClassInfo* right) const
    {
        if (left->level < right->level) return true;
        return false;
    }
};

std::vector<ClassInfo*> GetClassesByPriority(std::unordered_map<boost::uuids::uuid, ClassInfo, boost::hash<boost::uuids::uuid>>& classIdMap)
{
    std::vector<ClassInfo*> classesByPriority;
    for (auto& p : classIdMap)
    {
        ClassInfo* cls = &p.second;
        classesByPriority.push_back(cls);
    }
    std::sort(classesByPriority.begin(), classesByPriority.end(), PriorityGreater());
    return classesByPriority;
}

void AssignKeys(std::vector<ClassInfo*>& classesByPriority)
{
    uint64_t key = 2;
    for (ClassInfo* cls : classesByPriority)
    {
        cls->key = key;
        key = NextPrime(key + 1);
        if (key < cls->key)
        {
            throw std::runtime_error("error assigning class key for class " + ToUtf8(cls->cls->FullName()) + ": overflow, too many polymorphic classes");
        }
    }
}

uint64_t ComputeClassId(ClassInfo* cls)
{
    uint64_t classId = cls->key;
    ClassInfo* baseClass = cls->baseClassInfo;
    while (baseClass)
    {
        uint64_t key = baseClass->key;
        uint64_t product = classId * key;
        if (product / classId != key || product % key != 0)
        {
            throw std::runtime_error("error computing class key for class " + ToUtf8(cls->cls->FullName()) + ": overflow, too many polymorphic classes");
        }
        classId = product;
        baseClass = baseClass->baseClassInfo;
    }
    if (classId == 0)
    {
        throw std::runtime_error("internal error assigning class id's: invalid resulting class id 0");
    }
    return classId;
}

void AssignClassIds(std::vector<ClassInfo*>& classesByPriority)
{
    for (ClassInfo* cls : classesByPriority)
    {
        cls->id = ComputeClassId(cls);
    }
}

ConstantNode* MakePolymorphicClassArray(const std::unordered_set<ClassTypeSymbol*>& polymorphicClasses, const std::u32string& arrayName)
{
    std::unordered_map<boost::uuids::uuid, ClassInfo, boost::hash<boost::uuids::uuid>> classIdMap;
    for (ClassTypeSymbol* cls : polymorphicClasses)
    {
        classIdMap[cls->TypeId()] = ClassInfo(cls);
    }
    ResolveBaseClasses(classIdMap);
    AssignLevels(classIdMap);
    std::vector<ClassInfo*> classesByPriority = GetClassesByPriority(classIdMap);
    AssignKeys(classesByPriority);
    AssignClassIds(classesByPriority);
    ArrayLiteralNode* polymorphicClassArrayLiteral = new ArrayLiteralNode(Span());
    for (ClassInfo* info : classesByPriority)
    {
        const boost::uuids::uuid& typeId = info->cls->TypeId();
        uint64_t typeId1 = 0;
        uint64_t typeId2 = 0;
        UuidToInts(typeId, typeId1, typeId2);
        polymorphicClassArrayLiteral->AddValue(new ULongLiteralNode(Span(), typeId1));
        polymorphicClassArrayLiteral->AddValue(new ULongLiteralNode(Span(), typeId2));
        polymorphicClassArrayLiteral->AddValue(new ULongLiteralNode(Span(), info->id));
    }
    uint64_t arrayLength = polymorphicClassArrayLiteral->Values().Count();
    ConstantNode* polymorphicClassArray = new ConstantNode(Span(), Specifiers::internal_, new ArrayNode(Span(), new ULongNode(Span()),
        CreateIntegerLiteralNode(Span(), arrayLength, false)), new IdentifierNode(Span(), arrayName), polymorphicClassArrayLiteral);
    return polymorphicClassArray;
}

ConstantNode* MakeStaticClassArray(const std::unordered_set<ClassTypeSymbol*>& classesHavingStaticConstructor, const std::u32string& arrayName)
{
    std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>> staticTypeIdSet;
    for (ClassTypeSymbol* cls : classesHavingStaticConstructor)
    {
        staticTypeIdSet.insert(cls->TypeId());
    }
    ArrayLiteralNode* staticTypeIdArrayLiteral = new ArrayLiteralNode(Span());
    for (const boost::uuids::uuid& typeId : staticTypeIdSet)
    {
        uint64_t typeId1 = 0;
        uint64_t typeId2 = 0;
        UuidToInts(typeId, typeId1, typeId2);
        staticTypeIdArrayLiteral->AddValue(new ULongLiteralNode(Span(), typeId1));
        staticTypeIdArrayLiteral->AddValue(new ULongLiteralNode(Span(), typeId2));
    }
    uint64_t arrayLength = staticTypeIdArrayLiteral->Values().Count();
    ConstantNode* staticClassIdArray = new ConstantNode(Span(), Specifiers::internal_, new ArrayNode(Span(), new ULongNode(Span()),
        CreateIntegerLiteralNode(Span(), arrayLength, false)), new IdentifierNode(Span(), arrayName), staticTypeIdArrayLiteral);
    return staticClassIdArray;
}

#define CLASS_ID_FILE_DEBUG 0

void MakeClassIdFile(const std::unordered_set<ClassTypeSymbol*>& polymorphicClasses, const std::string& classIdFileName)
{
    soulng::util::BinaryWriter binaryWriter(classIdFileName);
    std::unordered_map<boost::uuids::uuid, ClassInfo, boost::hash<boost::uuids::uuid>> classIdMap;
    for (ClassTypeSymbol* cls : polymorphicClasses)
    {
        classIdMap[cls->TypeId()] = ClassInfo(cls);
    }
    ResolveBaseClasses(classIdMap);
    AssignLevels(classIdMap);
    std::vector<ClassInfo*> classesByPriority = GetClassesByPriority(classIdMap);
    AssignKeys(classesByPriority);
    AssignClassIds(classesByPriority);
    uint64_t n = classesByPriority.size();
#if (CLASS_ID_FILE_DEBUG)
    std::cerr << n << " classes" << std::endl;
#endif
    binaryWriter.Write(n);
    uint64_t i = 0;
    for (ClassInfo* info : classesByPriority)
    {
        const boost::uuids::uuid& typeId = info->cls->TypeId();
        binaryWriter.Write(typeId);
        binaryWriter.Write(info->id);
#if (CLASS_ID_FILE_DEBUG)
        std::cerr << "class" << i << std::endl;
#endif
        ++i;
    }
}

} } // namespace cmajor::symbols
