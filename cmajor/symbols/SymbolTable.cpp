// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/BasicTypeSymbol.hpp>
#include <cmajor/symbols/BasicTypeOperation.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/DelegateSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/ConstantSymbol.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/symbols/StringFunctions.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Time.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

bool operator==(const ClassTemplateSpecializationKey& left, const ClassTemplateSpecializationKey& right)
{
    if (!TypesEqual(left.classTemplate, right.classTemplate)) return false;
    int n = left.templateArgumentTypes.size();
    if (n != right.templateArgumentTypes.size()) return false;
    for (int i = 0; i < n; ++i)
    {
        if (!TypesEqual(left.templateArgumentTypes[i], right.templateArgumentTypes[i])) return false;
    }
    return true;
}

bool operator!=(const ClassTemplateSpecializationKey& left, const ClassTemplateSpecializationKey& right)
{
    return !(left == right);
}

bool operator==(const ArrayKey& left, const ArrayKey& right)
{
    if (!TypesEqual(left.elementType, right.elementType)) return false;
    if (left.size != right.size) return false;
    return true;
}

bool operator!=(const ArrayKey& left, const ArrayKey& right)
{
    return !(left == right);
}

void MapIdentifierToSymbolDefinition(IdentifierNode* identifierNode, Symbol* symbol)
{
    if (identifierNode->IsInternal()) return;
    if (!identifierNode->GetSpan().Valid()) return;
    if (identifierNode->ModuleId().is_nil()) return;
    if (symbol->SourceModuleId().is_nil()) return;
    if (!symbol->GetSpan().Valid()) return;
    Module* module = GetModuleById(identifierNode->ModuleId());
    if (!module) return;
    if (module->IsImmutable()) return;
    module->GetSymbolTable().MapIdentifierToSymbolDefinition(identifierNode, module, symbol);
}

SymbolTable::SymbolTable(Module* module_) : 
    module(module_), globalNs(Span(), boost::uuids::nil_uuid(), std::u32string()), currentCompileUnit(nullptr), container(&globalNs), currentClass(nullptr), currentInterface(nullptr), 
    mainFunctionSymbol(nullptr), currentFunctionSymbol(nullptr), parameterIndex(0), declarationBlockIndex(0), conversionTable(ConversionTable::Owner::symbolTable, module), 
    numSpecializationsNew(0), numSpecializationsCopied(0), createdFunctionSymbol(nullptr)
{
    globalNs.SetModule(module);
}

SymbolTable::~SymbolTable()
{
}

void SymbolTable::Write(SymbolWriter& writer)
{
    if (module->Name() == U"System.Core") // System.Core is special
    {
        uint32_t numDerivationIds = derivationIds.size();
        writer.GetBinaryWriter().WriteULEB128UInt(numDerivationIds);
        for (uint32_t i = 0; i < numDerivationIds; ++i)
        {
            writer.GetBinaryWriter().Write(derivationIds[i]);
        }
        uint32_t numPositionIds = positionIds.size();
        writer.GetBinaryWriter().WriteULEB128UInt(numPositionIds);
        for (uint32_t i = 0; i < numPositionIds; ++i)
        {
            writer.GetBinaryWriter().Write(positionIds[i]);
        }
    }
    globalNs.Write(writer);
    WriteSymbolDefinitionMap(writer);
    std::vector<ArrayTypeSymbol*> exportedArrayTypes;
    for (const std::unique_ptr<ArrayTypeSymbol>& arrayType : arrayTypes)
    {
        if (arrayType->IsProject())
        {
            exportedArrayTypes.push_back(arrayType.get());
        }
    }
    uint32_t na = exportedArrayTypes.size();
    writer.GetBinaryWriter().WriteULEB128UInt(na);
    for (ArrayTypeSymbol* exportedArrayType : exportedArrayTypes)
    {
        writer.Write(exportedArrayType);
    }
    std::vector<TypeSymbol*> exportedDerivedTypes;
    std::unordered_map<boost::uuids::uuid, TypeSymbol*, boost::hash<boost::uuids::uuid>> derivedTypeMap;
    for (const auto& derivedType : derivedTypes)
    {
        if (derivedType->IsProject())
        {
            derivedTypeMap[derivedType->TypeId()] = derivedType.get(); // last wins
        }
    }
    for (const auto& p : derivedTypeMap)
    {
        exportedDerivedTypes.push_back(p.second);
    }
    uint32_t ned = exportedDerivedTypes.size();
    writer.GetBinaryWriter().WriteULEB128UInt(ned);
    for (TypeSymbol* exportedDerivedType : exportedDerivedTypes)
    {
        writer.Write(exportedDerivedType);
    }
    std::vector<TypeSymbol*> exportedClassTemplateSpecializations;
    std::unordered_map<boost::uuids::uuid, TypeSymbol*, boost::hash<boost::uuids::uuid>> specializationMap;
    for (const auto& classTemplateSpecialization : classTemplateSpecializations)
    {
        if (classTemplateSpecialization->IsProject())
        {
            specializationMap[classTemplateSpecialization->TypeId()] = classTemplateSpecialization.get(); // last wins
        }
    }
    for (const auto& p : specializationMap)
    {
        exportedClassTemplateSpecializations.push_back(p.second);
    }
    uint32_t nec = exportedClassTemplateSpecializations.size();
    writer.GetBinaryWriter().WriteULEB128UInt(nec);
    for (TypeSymbol* classTemplateSpecialization : exportedClassTemplateSpecializations)
    {
        writer.Write(classTemplateSpecialization);
    }
    uint32_t nj = jsonClasses.size();
    writer.GetBinaryWriter().WriteULEB128UInt(nj);
    for (const std::u32string& jsonClass : jsonClasses)
    {
        writer.GetBinaryWriter().Write(jsonClass);
    }
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        uint32_t n = profiledFunctionNameMap.size();
        writer.GetBinaryWriter().WriteULEB128UInt(n);
        for (const auto& p : profiledFunctionNameMap)
        {
            writer.GetBinaryWriter().Write(p.first);
            writer.GetBinaryWriter().Write(p.second);
        }
    }
}

void SymbolTable::Read(SymbolReader& reader)
{
    reader.SetSymbolTable(this);
    if (module->Name() == U"System.Core") // System.Core is special
    {
        uint32_t numDerivationIds = reader.GetBinaryReader().ReadULEB128UInt();
        for (uint32_t i = 0; i < numDerivationIds; ++i)
        {
            boost::uuids::uuid derivationId;
            reader.GetBinaryReader().ReadUuid(derivationId);
            derivationIds.push_back(derivationId);
        }
        uint32_t numPositionIds = reader.GetBinaryReader().ReadULEB128UInt();
        for (uint32_t i = 0; i < numPositionIds; ++i)
        {
            boost::uuids::uuid positionId;
            reader.GetBinaryReader().ReadUuid(positionId);
            positionIds.push_back(positionId);
        }
    }
    globalNs.Read(reader);
    ReadSymbolDefinitionMap(reader);
    uint32_t na = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < na; ++i)
    {
        ArrayTypeSymbol* arrayTypeSymbol = reader.ReadArrayTypeSymbol(&globalNs);
        arrayTypes.push_back(std::unique_ptr<ArrayTypeSymbol>(arrayTypeSymbol));
        reader.AddArrayType(arrayTypeSymbol);
    }
    uint32_t nd = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < nd; ++i)
    {
        DerivedTypeSymbol* derivedTypeSymbol = reader.ReadDerivedTypeSymbol(&globalNs);
        derivedTypes.push_back(std::unique_ptr<DerivedTypeSymbol>(derivedTypeSymbol));
        reader.AddDerivedType(derivedTypeSymbol);
    }
    uint32_t nc = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < nc; ++i)
    {
        ClassTemplateSpecializationSymbol* classTemplateSpecialization = reader.ReadClassTemplateSpecializationSymbol(&globalNs);
        classTemplateSpecializations.push_back(std::unique_ptr<ClassTemplateSpecializationSymbol>(classTemplateSpecialization));
        reader.AddClassTemplateSpecialization(classTemplateSpecialization);
    }
    uint32_t nj = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < nj; ++i)
    {
        std::u32string jsonClass = reader.GetBinaryReader().ReadUtf32String();
        jsonClasses.insert(jsonClass);
    }
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
        for (uint32_t i = 0; i < n; ++i)
        {
            boost::uuids::uuid functionId;
            reader.GetBinaryReader().ReadUuid(functionId);
            std::u32string profiledFunctionName = reader.GetBinaryReader().ReadUtf32String();
            MapProfiledFunction(functionId, profiledFunctionName);
        }
    }
}

void SymbolTable::Import(const SymbolTable& symbolTable)
{
    globalNs.Import(const_cast<NamespaceSymbol*>(&symbolTable.globalNs), *this);
    for (const auto& pair : symbolTable.typeIdMap)
    {
        Symbol* typeOrConcept = pair.second;
        if (typeOrConcept->IsTypeSymbol())
        {
            TypeSymbol* type = static_cast<TypeSymbol*>(typeOrConcept);
            typeIdMap[type->TypeId()] = type;
            typeNameMap[type->FullName()] = type;
        }
        else if (typeOrConcept->GetSymbolType() == SymbolType::conceptSymbol)
        {
            ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(typeOrConcept);
            typeIdMap[conceptSymbol->TypeId()] = conceptSymbol;
        }
        else
        {
            Assert(false, "type or concept symbol expected");
        }
    }
    for (const auto& pair : symbolTable.functionIdMap)
    {
        FunctionSymbol* function = pair.second;
        functionIdMap[function->FunctionId()] = function;
    }
    for (const auto& p : symbolTable.derivedTypeMap)
    {
        derivedTypeMap[p.first] = p.second;
    }
    for (const auto& p : symbolTable.classTemplateSpecializationMap)
    {
        classTemplateSpecializationMap[p.first] = p.second;
    }
    for (const auto& p : symbolTable.arrayTypeMap)
    {
        arrayTypeMap[p.first] = p.second;
    }
    conversionTable.Add(symbolTable.conversionTable);
    for (ClassTypeSymbol* polymorphicClass : symbolTable.PolymorphicClasses())
    {
        AddPolymorphicClass(polymorphicClass);
    }
    for (ClassTypeSymbol* classHavingStaticConstructor : symbolTable.ClassesHavingStaticConstructor())
    {
        AddClassHavingStaticConstructor(classHavingStaticConstructor);
    }
    for (const std::u32string& jsonClass : symbolTable.JsonClasses())
    {
        AddJsonClass(jsonClass);
    }
    if (GetGlobalFlag(GlobalFlags::profile))
    {
        for (const auto& p : symbolTable.profiledFunctionNameMap)
        {
            const boost::uuids::uuid& functionId = p.first;
            const std::u32string& profiledFunctionName = p.second;
            MapProfiledFunction(functionId, profiledFunctionName);
        }
    }
}

void SymbolTable::FinishRead(const std::vector<ArrayTypeSymbol*>& arrayTypes, const std::vector<DerivedTypeSymbol*>& derivedTypes,
    const std::vector<ClassTemplateSpecializationSymbol*>& classTemplateSpecializations, const std::vector<TypeOrConceptRequest>& typeAndConceptRequests, 
    const std::vector<FunctionRequest>& functionRequests, std::vector<FunctionSymbol*>& conversions)
{
    ProcessTypeConceptAndFunctionRequests(typeAndConceptRequests, functionRequests);
    for (FunctionSymbol* conversion : conversions)
    {
        AddConversion(conversion, module);
    }
    for (ArrayTypeSymbol* arrayTypeSymbol : arrayTypes)
    {
        ArrayKey key(arrayTypeSymbol->ElementType(), arrayTypeSymbol->Size());
        arrayTypeMap[key] = arrayTypeSymbol;
    }
    for (DerivedTypeSymbol* derivedTypeSymbol : derivedTypes)
    {
        std::vector<DerivedTypeSymbol*>& derivedTypeVec = derivedTypeMap[derivedTypeSymbol->BaseType()->TypeId()];
        int n = derivedTypeVec.size();
        bool found = false;
        for (int i = 0; i < n; ++i)
        {
            DerivedTypeSymbol* prevDerivedTypeSymbol = derivedTypeVec[i];
            if (prevDerivedTypeSymbol->DerivationRec() == derivedTypeSymbol->DerivationRec())
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            derivedTypeVec.push_back(derivedTypeSymbol);
        }
    }
    AddClassTemplateSpecializationsToClassTemplateSpecializationMap(classTemplateSpecializations);
}

void SymbolTable::Clear()
{
    globalNs.Clear();
    typeIdMap.clear();
    functionIdMap.clear();
    typeNameMap.clear();
    profiledFunctionNameMap.clear();
}

void SymbolTable::BeginContainer(ContainerSymbol* container_)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, container_->GetSpan(), Span());
    }
#endif
    containerStack.push(container);
    container = container_;
}

void SymbolTable::EndContainer()
{
    container = containerStack.top();
    containerStack.pop();
}

void SymbolTable::MapNs(NamespaceSymbol* fromNs, NamespaceSymbol* toNs)
{
    nsMap[fromNs] = toNs;
}

NamespaceSymbol* SymbolTable::GetMappedNs(NamespaceSymbol* fromNs) const
{
    auto it = nsMap.find(fromNs);
    if (it != nsMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

NamespaceSymbol* SymbolTable::BeginNamespace(NamespaceNode& namespaceNode)
{
    std::u32string nsName = namespaceNode.Id()->Str();
    NamespaceSymbol* ns = BeginNamespace(nsName, namespaceNode.GetSpan(), namespaceNode.ModuleId());
    MapNode(&namespaceNode, container);
    return ns;
}

NamespaceSymbol* SymbolTable::BeginNamespace(const std::u32string& namespaceName, const Span& span, const boost::uuids::uuid& sourceModuleId)
{
    if (namespaceName.empty())
    {
        if (!globalNs.GetSpan().Valid())
        {
            globalNs.SetSpan(span);
        }
        BeginContainer(&globalNs);
        return &globalNs;
    }
    else
    { 
        Symbol* symbol = container->GetContainerScope()->Lookup(namespaceName);
        if (symbol)
        {
            if (symbol->GetSymbolType() == SymbolType::namespaceSymbol)
            {
                NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(symbol);
                BeginContainer(ns);
                return ns;
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(symbol->Name()) + "' does not denote a namespace", symbol->GetSpan(), symbol->SourceModuleId());
            }
        }
        else
        {
            NamespaceSymbol* ns = container->GetContainerScope()->CreateNamespace(namespaceName, span, sourceModuleId);
            BeginContainer(ns);
            return ns;
        }
    }
}

void SymbolTable::EndNamespace()
{
    EndContainer();
}

void SymbolTable::BeginFunction(FunctionNode& functionNode, int32_t functionIndex)
{
    FunctionSymbol* functionSymbol = new FunctionSymbol(functionNode.GetSpan(), functionNode.ModuleId(), functionNode.GroupId());
    SetFunctionIdFor(functionSymbol);
    functionSymbol->SetIndex(functionIndex);
    if ((functionNode.GetSpecifiers() & Specifiers::constexpr_) != Specifiers::none)
    {
        functionSymbol->SetConstExpr();
    }
    functionSymbol->SetHasSource();
    functionSymbol->SetCompileUnit(currentCompileUnit);
    functionSymbol->SetModule(module);
    functionSymbol->SetGroupName(functionNode.GroupId());
    if (functionNode.WhereConstraint())
    {
        CloneContext cloneContext;
        functionSymbol->SetConstraint(static_cast<WhereConstraintNode*>(functionNode.WhereConstraint()->Clone(cloneContext)));
    }
    if (functionSymbol->GroupName() == U"main" || functionSymbol->GroupName() == U"wmain" || functionSymbol->GroupName() == U"WinMain" || functionSymbol->GroupName() == U"wWinMain")
    {
        if (functionNode.IsProgramMain())
        {
            functionSymbol->SetCDecl();
            functionSymbol->SetProgramMain();
        }
        else
        {
            if (mainFunctionSymbol)
            {
                throw Exception("already has main function", functionNode.GetSpan(), functionNode.ModuleId(), mainFunctionSymbol->GetSpan(), mainFunctionSymbol->SourceModuleId());
            }
            else
            {
                mainFunctionSymbol = functionSymbol;
            }
        }
    }
    MapNode(&functionNode, functionSymbol);
    BeginContainer(functionSymbol);
    parameterIndex = 0;
    ResetDeclarationBlockIndex();
}

void SymbolTable::EndFunction(bool addMember)
{
    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(container);
    EndContainer();
    if (addMember)
    {
        container->AddMember(functionSymbol);
    }
    else
    {
        createdFunctionSymbol = functionSymbol;
    }
}

void SymbolTable::AddParameter(ParameterNode& parameterNode)
{
    bool artificialId = false;
    std::u32string parameterName = ToUtf32("@p" + std::to_string(parameterIndex));
    if (parameterNode.Id())
    {
        parameterName = parameterNode.Id()->Str();
    }
    else
    {
        parameterNode.SetId(new IdentifierNode(parameterNode.GetSpan(), parameterNode.ModuleId(), parameterName));
        artificialId = true;
    }
    ParameterSymbol* parameterSymbol = new ParameterSymbol(parameterNode.GetSpan(), parameterNode.ModuleId(), parameterName);
    parameterSymbol->SetCompileUnit(currentCompileUnit);
    MapNode(&parameterNode, parameterSymbol);
    container->AddMember(parameterSymbol);
    ++parameterIndex;
    if (artificialId)
    {
        parameterSymbol->SetArtificialName();
    }
}

void SymbolTable::BeginClass(ClassNode& classNode)
{
    ClassTypeSymbol* classTypeSymbol = new ClassTypeSymbol(classNode.GetSpan(), classNode.ModuleId(), classNode.Id()->Str());
    classTypeSymbol->SetGroupName(classNode.Id()->Str());
    currentClassStack.push(currentClass);
    currentClass = classTypeSymbol;
    classTypeSymbol->SetCompileUnit(currentCompileUnit);
    classTypeSymbol->SetModule(module);
    MapNode(&classNode, classTypeSymbol);
    SetTypeIdFor(classTypeSymbol);
    BeginContainer(classTypeSymbol);
}

void SymbolTable::EndClass()
{
    ClassTypeSymbol* classTypeSymbol = currentClass;
    currentClass = currentClassStack.top();
    currentClassStack.pop();
    EndContainer();
    classTypeSymbol->ComputeMinArity();
    container->AddMember(classTypeSymbol);
}

void SymbolTable::BeginClassTemplateSpecialization(ClassNode& classInstanceNode, ClassTemplateSpecializationSymbol* classTemplateSpecialization)
{
    currentClassStack.push(currentClass);
    currentClass = classTemplateSpecialization;
    MapNode(&classInstanceNode, classTemplateSpecialization);
    if (classTemplateSpecialization->TypeIdNotSet())
    {
        SetTypeIdFor(classTemplateSpecialization);
    }
    BeginContainer(classTemplateSpecialization);
}

void SymbolTable::EndClassTemplateSpecialization()
{
    EndContainer();
    currentClass = currentClassStack.top();
    currentClassStack.pop();
}

void SymbolTable::AddTemplateParameter(TemplateParameterNode& templateParameterNode)
{
    TemplateParameterSymbol* templateParameterSymbol = new TemplateParameterSymbol(templateParameterNode.GetSpan(), templateParameterNode.ModuleId(), templateParameterNode.Id()->Str());
    if (templateParameterNode.DefaultTemplateArgument())
    {
        templateParameterSymbol->SetHasDefault();
    }
    templateParameterSymbol->SetCompileUnit(currentCompileUnit);
    templateParameterSymbol->SetModule(module);
    SetTypeIdFor(templateParameterSymbol);
    MapNode(&templateParameterNode, templateParameterSymbol);
    container->AddMember(templateParameterSymbol);
}

void SymbolTable::AddTemplateParameter(IdentifierNode& identifierNode)
{
    TemplateParameterSymbol* templateParameterSymbol = new TemplateParameterSymbol(identifierNode.GetSpan(), identifierNode.ModuleId(), identifierNode.Str());
    templateParameterSymbol->SetCompileUnit(currentCompileUnit);
    templateParameterSymbol->SetModule(module);
    SetTypeIdFor(templateParameterSymbol);
    MapNode(&identifierNode, templateParameterSymbol);
    container->AddMember(templateParameterSymbol);
}

void SymbolTable::BeginInterface(InterfaceNode& interfaceNode)
{
    InterfaceTypeSymbol* interfaceTypeSymbol = new InterfaceTypeSymbol(interfaceNode.GetSpan(), interfaceNode.ModuleId(), interfaceNode.Id()->Str());
    currentInterfaceStack.push(currentInterface);
    currentInterface = interfaceTypeSymbol;
    interfaceTypeSymbol->SetCompileUnit(currentCompileUnit);
    interfaceTypeSymbol->SetModule(module);
    MapNode(&interfaceNode, interfaceTypeSymbol);
    SetTypeIdFor(interfaceTypeSymbol);
    container->AddMember(interfaceTypeSymbol);
    BeginContainer(interfaceTypeSymbol);
}

void SymbolTable::EndInterface()
{
    currentInterface = currentInterfaceStack.top();
    currentInterfaceStack.pop();
    EndContainer();
}

void SymbolTable::BeginStaticConstructor(StaticConstructorNode& staticConstructorNode, int32_t functionIndex)
{
    StaticConstructorSymbol* staticConstructorSymbol = new StaticConstructorSymbol(staticConstructorNode.GetSpan(), staticConstructorNode.ModuleId(), U"@static_constructor");
    staticConstructorSymbol->SetIndex(functionIndex);
    SetFunctionIdFor(staticConstructorSymbol);
    staticConstructorSymbol->SetHasSource();
    staticConstructorSymbol->SetCompileUnit(currentCompileUnit);
    staticConstructorSymbol->SetModule(module);
    if (staticConstructorNode.WhereConstraint())
    {
        CloneContext cloneContext;
        staticConstructorSymbol->SetConstraint(static_cast<WhereConstraintNode*>(staticConstructorNode.WhereConstraint()->Clone(cloneContext)));
    }
    MapNode(&staticConstructorNode, staticConstructorSymbol);
    BeginContainer(staticConstructorSymbol);
    ResetDeclarationBlockIndex();
}

void SymbolTable::EndStaticConstructor(bool addMember)
{
    StaticConstructorSymbol* staticConstructorSymbol = static_cast<StaticConstructorSymbol*>(container);
    EndContainer();
    if (addMember)
    {
        container->AddMember(staticConstructorSymbol);
    }
    else
    {
        createdFunctionSymbol = staticConstructorSymbol;
    }
}

void SymbolTable::BeginConstructor(ConstructorNode& constructorNode, int32_t functionIndex)
{
    ConstructorSymbol* constructorSymbol = new ConstructorSymbol(constructorNode.GetSpan(), constructorNode.ModuleId(), U"@constructor");
    constructorSymbol->SetIndex(functionIndex);
    SetFunctionIdFor(constructorSymbol);
    if ((constructorNode.GetSpecifiers() & Specifiers::constexpr_) != Specifiers::none)
    {
        constructorSymbol->SetConstExpr();
    }
    constructorSymbol->SetHasSource();
    constructorSymbol->SetCompileUnit(currentCompileUnit);
    constructorSymbol->SetModule(module);
    if (constructorNode.WhereConstraint())
    {
        CloneContext cloneContext;
        constructorSymbol->SetConstraint(static_cast<WhereConstraintNode*>(constructorNode.WhereConstraint()->Clone(cloneContext)));
    }
    MapNode(&constructorNode, constructorSymbol);
    BeginContainer(constructorSymbol);
    parameterIndex = 0;
    ResetDeclarationBlockIndex();
    ParameterSymbol* thisParam = new ParameterSymbol(constructorNode.GetSpan(), constructorNode.ModuleId(), U"this");
    TypeSymbol* thisParamType = nullptr;
    if (currentClass)
    {
        thisParamType = currentClass->AddPointer(constructorNode.GetSpan(), constructorNode.ModuleId());
        thisParam->SetType(thisParamType);
        thisParam->SetBound();
        constructorSymbol->AddMember(thisParam);
    }
    else if (currentInterface)
    {
        throw Exception("interface type cannot have a constructor", constructorNode.GetSpan(), constructorNode.ModuleId());
    }
}

void SymbolTable::EndConstructor(bool addMember)
{
    ConstructorSymbol* constructorSymbol = static_cast<ConstructorSymbol*>(container);
    EndContainer();
    if (addMember)
    {
        container->AddMember(constructorSymbol);
    }
    else
    {
        createdFunctionSymbol = constructorSymbol;
    }
}

void SymbolTable::BeginDestructor(DestructorNode& destructorNode, int32_t functionIndex)
{
    DestructorSymbol* destructorSymbol = new DestructorSymbol(destructorNode.GetSpan(), destructorNode.ModuleId(), U"@destructor");
    destructorSymbol->SetIndex(functionIndex);
    SetFunctionIdFor(destructorSymbol);
    destructorSymbol->SetHasSource();
    destructorSymbol->SetCompileUnit(currentCompileUnit);
    destructorSymbol->SetModule(module);
    if (destructorNode.WhereConstraint())
    {
        CloneContext cloneContext;
        destructorSymbol->SetConstraint(static_cast<WhereConstraintNode*>(destructorNode.WhereConstraint()->Clone(cloneContext)));
    }
    MapNode(&destructorNode, destructorSymbol);
    BeginContainer(destructorSymbol);
    ResetDeclarationBlockIndex();
    ParameterSymbol* thisParam = new ParameterSymbol(destructorNode.GetSpan(), destructorNode.ModuleId(), U"this");
    TypeSymbol* thisParamType = nullptr;
    if (currentClass)
    {
        thisParamType = currentClass->AddPointer(destructorNode.GetSpan(), destructorNode.ModuleId());
        thisParam->SetType(thisParamType);
        thisParam->SetBound();
        destructorSymbol->AddMember(thisParam);
    }
    else if (currentInterface)
    {
        throw Exception("interface type cannot have a destructor", destructorNode.GetSpan(), destructorNode.ModuleId());
    }
}

void SymbolTable::EndDestructor(bool addMember)
{
    DestructorSymbol* destructorSymbol = static_cast<DestructorSymbol*>(container);
    EndContainer();
    if (addMember)
    {
        container->AddMember(destructorSymbol);
    }
    else
    {
        createdFunctionSymbol = destructorSymbol;
    }
}

void SymbolTable::BeginMemberFunction(MemberFunctionNode& memberFunctionNode, int32_t functionIndex)
{
    MemberFunctionSymbol* memberFunctionSymbol = new MemberFunctionSymbol(memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId(), memberFunctionNode.GroupId());
    memberFunctionSymbol->SetIndex(functionIndex);
    SetFunctionIdFor(memberFunctionSymbol);
    if ((memberFunctionNode.GetSpecifiers() & Specifiers::constexpr_) != Specifiers::none)
    {
        memberFunctionSymbol->SetConstExpr();
    }
    memberFunctionSymbol->SetHasSource();
    memberFunctionSymbol->SetCompileUnit(currentCompileUnit);
    memberFunctionSymbol->SetModule(module);
    memberFunctionSymbol->SetGroupName(memberFunctionNode.GroupId());
    if (memberFunctionNode.WhereConstraint())
    {
        CloneContext cloneContext;
        memberFunctionSymbol->SetConstraint(static_cast<WhereConstraintNode*>(memberFunctionNode.WhereConstraint()->Clone(cloneContext)));
    }
    MapNode(&memberFunctionNode, memberFunctionSymbol);
    BeginContainer(memberFunctionSymbol);
    parameterIndex = 0;
    ResetDeclarationBlockIndex();
    if ((memberFunctionNode.GetSpecifiers() & Specifiers::static_) == Specifiers::none)
    {
        ParameterSymbol* thisParam = new ParameterSymbol(memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId(), U"this");
        TypeSymbol* thisParamType = nullptr;
        if (currentClass)
        {
            if (memberFunctionNode.IsConst())
            {
                thisParamType = currentClass->AddConst(memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId())->AddPointer(memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId());
            }
            else
            {
                thisParamType = currentClass->AddPointer(memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId());
            }
        }
        else if (currentInterface)
        {
            thisParamType = currentInterface->AddPointer(memberFunctionNode.GetSpan(), memberFunctionNode.ModuleId());
        }
        else
        {
            Assert(false, "class or interface expected");
        }
        thisParam->SetType(thisParamType);
        thisParam->SetBound();
        memberFunctionSymbol->AddMember(thisParam);
    }
}

void SymbolTable::EndMemberFunction(bool addMember)
{
    MemberFunctionSymbol* memberFunctionSymbol = static_cast<MemberFunctionSymbol*>(container);
    EndContainer();
    if (addMember)
    {
        container->AddMember(memberFunctionSymbol);
    }
    else
    {
        createdFunctionSymbol = memberFunctionSymbol;
    }
}

void SymbolTable::BeginConversionFunction(ConversionFunctionNode& conversionFunctionNode, int32_t functionIndex)
{
    ConversionFunctionSymbol* conversionFunctionSymbol = new ConversionFunctionSymbol(conversionFunctionNode.GetSpan(), conversionFunctionNode.ModuleId(), U"@conversion");
    conversionFunctionSymbol->SetIndex(functionIndex);
    SetFunctionIdFor(conversionFunctionSymbol);
    if ((conversionFunctionNode.GetSpecifiers() & Specifiers::constexpr_) != Specifiers::none)
    {
        conversionFunctionSymbol->SetConstExpr();
    }
    conversionFunctionSymbol->SetHasSource();
    conversionFunctionSymbol->SetCompileUnit(currentCompileUnit);
    conversionFunctionSymbol->SetModule(module);
    conversionFunctionSymbol->SetGroupName(U"@operator_conv");
    if (conversionFunctionNode.WhereConstraint())
    {
        CloneContext cloneContext;
        conversionFunctionSymbol->SetConstraint(static_cast<WhereConstraintNode*>(conversionFunctionNode.WhereConstraint()->Clone(cloneContext)));
    }
    MapNode(&conversionFunctionNode, conversionFunctionSymbol);
    BeginContainer(conversionFunctionSymbol);
    ResetDeclarationBlockIndex();
    ParameterSymbol* thisParam = new ParameterSymbol(conversionFunctionNode.GetSpan(), conversionFunctionNode.ModuleId(), U"this");
    TypeSymbol* thisParamType = nullptr;
    if (conversionFunctionNode.IsConst())
    {
        thisParamType = currentClass->AddConst(conversionFunctionNode.GetSpan(), conversionFunctionNode.ModuleId())->AddPointer(conversionFunctionNode.GetSpan(), conversionFunctionNode.ModuleId());
    }
    else
    {
        thisParamType = currentClass->AddPointer(conversionFunctionNode.GetSpan(), conversionFunctionNode.ModuleId());
    }
    thisParam->SetType(thisParamType);
    thisParam->SetBound();
    conversionFunctionSymbol->AddMember(thisParam);
}

void SymbolTable::EndConversionFunction(bool addMember)
{
    ConversionFunctionSymbol* conversionFunctionSymbol = static_cast<ConversionFunctionSymbol*>(container);
    EndContainer();
    if (addMember)
    {
        container->AddMember(conversionFunctionSymbol);
    }
    else
    {
        createdFunctionSymbol = conversionFunctionSymbol;
    }
}

void SymbolTable::AddMemberVariable(MemberVariableNode& memberVariableNode)
{
    MemberVariableSymbol* memberVariableSymbol = new MemberVariableSymbol(memberVariableNode.GetSpan(), memberVariableNode.ModuleId(), memberVariableNode.Id()->Str());
    if ((memberVariableNode.GetSpecifiers() & Specifiers::static_) != Specifiers::none)
    {
        memberVariableSymbol->SetStatic();
    }
    memberVariableSymbol->SetCompileUnit(currentCompileUnit);
    MapNode(&memberVariableNode, memberVariableSymbol);
    container->AddMember(memberVariableSymbol);
}

void SymbolTable::BeginDelegate(DelegateNode& delegateNode)
{
    DelegateTypeSymbol* delegateTypeSymbol = new DelegateTypeSymbol(delegateNode.GetSpan(), delegateNode.ModuleId(), delegateNode.Id()->Str());
    delegateTypeSymbol->SetCompileUnit(currentCompileUnit);
    delegateTypeSymbol->SetModule(module);
    MapNode(&delegateNode, delegateTypeSymbol);
    SetTypeIdFor(delegateTypeSymbol);
    container->AddMember(delegateTypeSymbol);
    BeginContainer(delegateTypeSymbol);
    parameterIndex = 0;
}

void SymbolTable::EndDelegate()
{
    EndContainer();
}

void SymbolTable::BeginClassDelegate(ClassDelegateNode& classDelegateNode)
{
    ClassDelegateTypeSymbol* classDelegateTypeSymbol = new ClassDelegateTypeSymbol(classDelegateNode.GetSpan(), classDelegateNode.ModuleId(), classDelegateNode.Id()->Str());
    classDelegateTypeSymbol->SetCompileUnit(currentCompileUnit);
    classDelegateTypeSymbol->SetModule(module);
    MapNode(&classDelegateNode, classDelegateTypeSymbol);
    SetTypeIdFor(classDelegateTypeSymbol);
    container->AddMember(classDelegateTypeSymbol);
    BeginContainer(classDelegateTypeSymbol);
    parameterIndex = 0;
}

void SymbolTable::EndClassDelegate()
{
    EndContainer();
}

void SymbolTable::BeginConcept(ConceptNode& conceptNode, bool hasSource)
{
    ConceptSymbol* conceptSymbol = new ConceptSymbol(conceptNode.GetSpan(), conceptNode.ModuleId(), conceptNode.Id()->Str());
    if (hasSource)
    {
        conceptSymbol->SetHasSource();
    }
    conceptSymbol->SetGroupName(conceptNode.Id()->Str());
    conceptSymbol->SetCompileUnit(currentCompileUnit);
    conceptSymbol->SetModule(module);
    MapNode(&conceptNode, conceptSymbol);
    SetTypeIdFor(conceptSymbol);
    BeginContainer(conceptSymbol);
}

void SymbolTable::EndConcept()
{
    ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(container);
    EndContainer();
    container->AddMember(conceptSymbol);
}

void SymbolTable::BeginDeclarationBlock(Node& node)
{
    DeclarationBlock* declarationBlock = new DeclarationBlock(node.GetSpan(), node.ModuleId(), U"@locals" + ToUtf32(std::to_string(GetNextDeclarationBlockIndex())));
    declarationBlock->SetCompileUnit(currentCompileUnit);
    declarationBlock->SetModule(module);
    MapNode(&node, declarationBlock);
    container->AddMember(declarationBlock);
    BeginContainer(declarationBlock);
}

void SymbolTable::EndDeclarationBlock()
{
    EndContainer();
}

void SymbolTable::AddLocalVariable(ConstructionStatementNode& constructionStatementNode)
{
    if (GetGlobalFlag(GlobalFlags::profile) && constructionStatementNode.Id()->Str() == U"@functionProfiler")
    {
        for (LocalVariableSymbol* localVariableSymbol : currentFunctionSymbol->LocalVariables())
        {
            if (localVariableSymbol->Name() == U"@functionProfiler")
            {
                MapNode(&constructionStatementNode, localVariableSymbol);
                return;
            }
        }
    }
    LocalVariableSymbol* localVariableSymbol = new LocalVariableSymbol(constructionStatementNode.GetSpan(), constructionStatementNode.ModuleId(), constructionStatementNode.Id()->Str());
    localVariableSymbol->SetCompileUnit(currentCompileUnit);
    localVariableSymbol->SetModule(module);
    MapNode(&constructionStatementNode, localVariableSymbol);
    container->AddMember(localVariableSymbol);
}

void SymbolTable::AddLocalVariable(IdentifierNode& identifierNode)
{
    LocalVariableSymbol* localVariableSymbol = new LocalVariableSymbol(identifierNode.GetSpan(), identifierNode.ModuleId(), identifierNode.Str());
    localVariableSymbol->SetCompileUnit(currentCompileUnit);
    localVariableSymbol->SetModule(module);
    MapNode(&identifierNode, localVariableSymbol);
    container->AddMember(localVariableSymbol);
}

void SymbolTable::AddTypedef(TypedefNode& typedefNode)
{
    TypedefSymbol* typedefSymbol = new TypedefSymbol(typedefNode.GetSpan(), typedefNode.ModuleId(), typedefNode.Id()->Str());
    typedefSymbol->SetCompileUnit(currentCompileUnit);
    typedefSymbol->SetModule(module);
    MapNode(&typedefNode, typedefSymbol);
    container->AddMember(typedefSymbol);
}

void SymbolTable::AddConstant(ConstantNode& constantNode)
{
    ConstantSymbol* constantSymbol = new ConstantSymbol(constantNode.GetSpan(), constantNode.ModuleId(), constantNode.Id()->Str());
    constantSymbol->SetCompileUnit(currentCompileUnit);
    constantSymbol->SetModule(module);
    constantSymbol->SetStrValue(constantNode.StrValue());
    MapNode(&constantNode, constantSymbol);
    container->AddMember(constantSymbol);
}

void SymbolTable::AddGlobalVariable(GlobalVariableNode& globalVariableNode)
{
    GlobalVariableSymbol* globalVariableSymbol = new GlobalVariableSymbol(globalVariableNode.GetSpan(), globalVariableNode.ModuleId(), globalVariableNode.Id()->Str(), globalVariableNode.CompileUnit()->Id(),
        globalVariableNode.CompileUnit()->FilePath());
    globalVariableSymbol->SetSpecifiers(globalVariableNode.GetSpecifiers());
    globalVariableSymbol->SetCompileUnit(currentCompileUnit);
    globalVariableSymbol->SetModule(module);
    MapNode(&globalVariableNode, globalVariableSymbol);
    container->AddMember(globalVariableSymbol);
}

void SymbolTable::BeginEnumType(EnumTypeNode& enumTypeNode)
{
    EnumTypeSymbol* enumTypeSymbol = new EnumTypeSymbol(enumTypeNode.GetSpan(), enumTypeNode.ModuleId(), enumTypeNode.Id()->Str());
    enumTypeSymbol->SetCompileUnit(currentCompileUnit);
    enumTypeSymbol->SetModule(module);
    MapNode(&enumTypeNode, enumTypeSymbol);
    SetTypeIdFor(enumTypeSymbol);
    container->AddMember(enumTypeSymbol);
    BeginContainer(enumTypeSymbol);
}

void SymbolTable::EndEnumType()
{
    EndContainer();
}

void SymbolTable::AddEnumConstant(EnumConstantNode& enumConstantNode)
{
    EnumConstantSymbol* enumConstantSymbol = new EnumConstantSymbol(enumConstantNode.GetSpan(), enumConstantNode.ModuleId(), enumConstantNode.Id()->Str());
    enumConstantSymbol->SetCompileUnit(currentCompileUnit);
    enumConstantSymbol->SetModule(module);
    enumConstantSymbol->SetStrValue(enumConstantNode.StrValue());
    MapNode(&enumConstantNode, enumConstantSymbol);
    container->AddMember(enumConstantSymbol);
}

void SymbolTable::AddTypeSymbolToGlobalScope(TypeSymbol* typeSymbol)
{
    typeSymbol->SetModule(module);
    globalNs.AddMember(typeSymbol);
    SetTypeIdFor(typeSymbol);
    typeNameMap[typeSymbol->FullName()] = typeSymbol;
}

void SymbolTable::AddFunctionSymbolToGlobalScope(FunctionSymbol* functionSymbol)
{
    SetFunctionIdFor(functionSymbol);
    functionSymbol->SetModule(module);
    globalNs.AddMember(functionSymbol);
    if (functionSymbol->IsConversion())
    {
        conversionTable.AddConversion(functionSymbol);
    }
}

void SymbolTable::MapNode(Node* node, Symbol* symbol)
{
    nodeSymbolMap[node] = symbol;
    symbolNodeMap[symbol] = node;
}

Symbol* SymbolTable::GetSymbolNoThrow(Node* node) const
{
    auto it = nodeSymbolMap.find(node);
    if (it != nodeSymbolMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Symbol* SymbolTable::GetSymbol(Node* node) const
{
    Symbol* symbol = GetSymbolNoThrow(node);
    if (symbol)
    {
        return symbol;
    }
    else
    {
        throw std::runtime_error("symbol for node not found");
    }
}

Node* SymbolTable::GetNodeNoThrow(Symbol* symbol) const
{
    auto it = symbolNodeMap.find(symbol);
    if (it != symbolNodeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Node* SymbolTable::GetNode(Symbol* symbol) const
{
    Node* node = GetNodeNoThrow(symbol);
    if (node)
    {
        return node;
    }
    else
    {
        throw std::runtime_error("node for symbol not found");
    }
}

void SymbolTable::AddTypeOrConceptSymbolToTypeIdMap(Symbol* typeOrConceptSymbol)
{
    if (typeOrConceptSymbol->IsTypeSymbol())
    {
        TypeSymbol* typeSymbol = static_cast<TypeSymbol*>(typeOrConceptSymbol);
        typeIdMap[typeSymbol->TypeId()] = typeSymbol;
    }
    else if (typeOrConceptSymbol->GetSymbolType() == SymbolType::conceptSymbol)
    {
        ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(typeOrConceptSymbol);
        typeIdMap[conceptSymbol->TypeId()] = conceptSymbol;
    }
    else
    {
        Assert(false, "type or concept symbol expected");
    }
}

void SymbolTable::AddFunctionSymbolToFunctionIdMap(FunctionSymbol* functionSymbol)
{
    functionIdMap[functionSymbol->FunctionId()] = functionSymbol;
}

void SymbolTable::SetTypeIdFor(TypeSymbol* typeSymbol)
{
    typeSymbol->SetTypeId(boost::uuids::random_generator()()); 
}

void SymbolTable::SetTypeIdFor(ConceptSymbol* conceptSymbol)
{
    conceptSymbol->SetTypeId(boost::uuids::random_generator()());
}

void SymbolTable::SetFunctionIdFor(FunctionSymbol* functionSymbol)
{
    functionSymbol->SetFunctionId(boost::uuids::random_generator()());
}

FunctionSymbol* SymbolTable::GetFunctionById(const boost::uuids::uuid& functionId) const
{
    auto it = functionIdMap.find(functionId);
    if (it != functionIdMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void SymbolTable::EmplaceTypeRequest(SymbolReader& reader, Symbol* forSymbol, const boost::uuids::uuid& typeId, int index)
{
    EmplaceTypeOrConceptRequest(reader, forSymbol, typeId, index);
}

const int conceptRequestIndex = std::numeric_limits<int>::max();

void SymbolTable::EmplaceConceptRequest(SymbolReader& reader, Symbol* forSymbol, const boost::uuids::uuid& typeId)
{
    EmplaceTypeOrConceptRequest(reader, forSymbol, typeId, conceptRequestIndex);
}

void SymbolTable::EmplaceTypeOrConceptRequest(SymbolReader& reader, Symbol* forSymbol, const boost::uuids::uuid& typeId, int index)
{
    auto it = typeIdMap.find(typeId);
    if (it != typeIdMap.cend())
    {
        Symbol* typeOrConceptSymbol = it->second;
        if (typeOrConceptSymbol->IsTypeSymbol())
        {
            if (index == conceptRequestIndex)
            {
                throw Exception("internal error: invalid concept request (id denotes a type)", forSymbol->GetSpan(), forSymbol->SourceModuleId());
            }
            TypeSymbol* typeSymbol = static_cast<TypeSymbol*>(typeOrConceptSymbol);
            forSymbol->EmplaceType(typeSymbol, index);
        }
        else if (typeOrConceptSymbol->GetSymbolType() == SymbolType::conceptSymbol)
        {
            if (index != conceptRequestIndex)
            {
                throw Exception("internal error: invalid type request (id denotes a concept)", forSymbol->GetSpan(), forSymbol->SourceModuleId());
            }
            ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(typeOrConceptSymbol);
            forSymbol->EmplaceConcept(conceptSymbol);
        }
        else
        {
            Assert(false, "internal error: type or concept symbol expected");
        }
    }
    else
    {
        reader.AddTypeOrConceptRequest(TypeOrConceptRequest(forSymbol, typeId, index));
    }
}

void SymbolTable::EmplaceFunctionRequest(SymbolReader& reader, Symbol* forSymbol, const::boost::uuids::uuid& functionId, int index)
{
    auto it = functionIdMap.find(functionId);
    if (it != functionIdMap.cend())
    {
        FunctionSymbol* functionSymbol = it->second;
        forSymbol->EmplaceFunction(functionSymbol, index);
    }
    else
    {
        reader.AddFunctionRequest(FunctionRequest(forSymbol, functionId, index));
    }
}

void SymbolTable::ProcessTypeConceptAndFunctionRequests(const std::vector<TypeOrConceptRequest>& typeAndConceptRequests, const std::vector<FunctionRequest>& functionRequests)
{
    for (const TypeOrConceptRequest& typeOrConceptRequest : typeAndConceptRequests)
    {
        Symbol* symbol = typeOrConceptRequest.symbol;
        auto it = typeIdMap.find(typeOrConceptRequest.typeId);
        if (it != typeIdMap.cend())
        {
            Symbol* typeOrConceptSymbol = it->second;
            int index = typeOrConceptRequest.index;
            if (typeOrConceptSymbol->IsTypeSymbol())
            {
                if (index == conceptRequestIndex)
                {
                    throw Exception("internal error: invalid concept request (id denotes a type)", symbol->GetSpan(), symbol->SourceModuleId());
                }
                TypeSymbol* typeSymbol = static_cast<TypeSymbol*>(typeOrConceptSymbol);
                symbol->EmplaceType(typeSymbol, index);
            }
            else if (typeOrConceptSymbol->GetSymbolType() == SymbolType::conceptSymbol)
            {
                if (index != conceptRequestIndex)
                {
                    throw Exception("internal error: invalid type request (id denotes a concept)", symbol->GetSpan(), symbol->SourceModuleId());
                }
                ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(typeOrConceptSymbol);
                symbol->EmplaceConcept(conceptSymbol);
            }
            else
            {
                Assert(false, "internal error: type or concept symbol expected");
            }
        }
        else
        {
            throw std::runtime_error("internal error: cannot satisfy type or concept request for symbol '" + ToUtf8(symbol->Name()) + "': type or concept not found from symbol table");
        }
    }
    for (const FunctionRequest& functionRequest : functionRequests)
    {
        Symbol* symbol = functionRequest.symbol;
        auto it = functionIdMap.find(functionRequest.functionId);
        if (it != functionIdMap.cend())
        {
            FunctionSymbol* functionSymbol = it->second;
            int index = functionRequest.index;
            symbol->EmplaceFunction(functionSymbol, index);
        }
        else
        {
            throw std::runtime_error("internal error: cannot satisfy function request for symbol '" + ToUtf8(symbol->Name()) + "': function not found from symbol table");
        }
    }
}

TypeSymbol* SymbolTable::GetTypeByNameNoThrow(const std::u32string& typeName) const
{
    auto it = typeNameMap.find(typeName);
    if (it != typeNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

TypeSymbol* SymbolTable::GetTypeByName(const std::u32string& typeName) const
{
    TypeSymbol* typeSymbol = GetTypeByNameNoThrow(typeName);
    if (typeSymbol)
    {
        return typeSymbol;
    }
    else
    {
        throw std::runtime_error("type '" + ToUtf8(typeName) + "' not found");
    }
}

TypeSymbol* SymbolTable::MakeDerivedType(TypeSymbol* baseType, const TypeDerivationRec& derivationRec, const Span& span, const boost::uuids::uuid& moduleId)
{
    if (!baseType)
    {
        throw Exception("base type is null!", span, moduleId);
    }
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(module, baseType->GetModule(), span, baseType->GetSpan());
    }
#endif
    if (derivationRec.IsEmpty())
    {
        return baseType;
    }
    if (baseType->IsVoidType() && HasReferenceDerivation(derivationRec.derivations) && !HasPointerDerivation(derivationRec.derivations))
    {
        throw Exception("cannot have reference to void type", span, moduleId);
    }
    std::vector<DerivedTypeSymbol*>& mappedDerivedTypes = derivedTypeMap[baseType->TypeId()];
    int n = mappedDerivedTypes.size();
    for (int i = 0; i < n; ++i)
    {
        DerivedTypeSymbol* derivedType = mappedDerivedTypes[i];
        if (derivedType->DerivationRec() == derivationRec)
        {
            return derivedType;
        }
    }
    DerivedTypeSymbol* derivedType = new DerivedTypeSymbol(Span(), boost::uuids::nil_uuid(), MakeDerivedTypeName(baseType, derivationRec), baseType, derivationRec);
    derivedType->SetParent(&globalNs);
    derivedType->SetModule(module);
    derivedType->ComputeTypeId();
    mappedDerivedTypes.push_back(derivedType);
    derivedTypes.push_back(std::unique_ptr<DerivedTypeSymbol>(derivedType));
    if (derivedType->IsPointerType() && !derivedType->BaseType()->IsVoidType() && !derivedType->IsReferenceType())
    {
        TypedefSymbol* valueType = new TypedefSymbol(baseType->GetSpan(), baseType->SourceModuleId(), U"ValueType");
        valueType->SetModule(module);
        valueType->SetAccess(SymbolAccess::public_);
        valueType->SetType(derivedType->RemovePointer(span, moduleId));
        TypeSymbol* withoutConst = valueType->GetType()->RemoveConst(span, moduleId);
        if (withoutConst->IsBasicTypeSymbol())
        {
            valueType->SetType(withoutConst);
        }
        valueType->SetBound();
        derivedType->AddMember(valueType);
        TypedefSymbol* referenceType = new TypedefSymbol(baseType->GetSpan(), baseType->SourceModuleId(), U"ReferenceType");
        referenceType->SetModule(module);
        referenceType->SetAccess(SymbolAccess::public_);
        referenceType->SetType(valueType->GetType()->AddLvalueReference(span, moduleId));
        referenceType->SetBound();
        derivedType->AddMember(referenceType);
        TypedefSymbol* pointerType = new TypedefSymbol(baseType->GetSpan(), baseType->SourceModuleId(), U"PointerType");
        pointerType->SetModule(module);
        pointerType->SetAccess(SymbolAccess::public_);
        pointerType->SetType(derivedType);
        pointerType->SetBound();
        derivedType->AddMember(pointerType);
    }
    return derivedType;
}

ClassTemplateSpecializationSymbol* SymbolTable::MakeClassTemplateSpecialization(ClassTypeSymbol* classTemplate, const std::vector<TypeSymbol*>& templateArgumentTypes, 
    const Span& span, const boost::uuids::uuid& sourceModuleId)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, classTemplate->GetSpan(), span);
    }
#endif
    ClassTemplateSpecializationKey key(classTemplate, templateArgumentTypes);
    auto it = classTemplateSpecializationMap.find(key);
    if (it != classTemplateSpecializationMap.cend())
    {
        ClassTemplateSpecializationSymbol* classTemplateSpecialization = it->second;
        return classTemplateSpecialization;
    }
    std::u32string classTemplateSpecializationName = MakeClassTemplateSpecializationName(classTemplate, templateArgumentTypes);
    ClassTemplateSpecializationSymbol* classTemplateSpecialization = new ClassTemplateSpecializationSymbol(Span(), boost::uuids::nil_uuid(), classTemplateSpecializationName, classTemplate,
        templateArgumentTypes);
    SetTypeIdFor(classTemplateSpecialization);
    classTemplateSpecialization->SetGroupName(classTemplate->GroupName());
    classTemplateSpecializationMap[key] = classTemplateSpecialization;
    classTemplateSpecialization->SetParent(&globalNs);
    classTemplateSpecialization->SetModule(module);
    classTemplateSpecializations.push_back(std::unique_ptr<ClassTemplateSpecializationSymbol>(classTemplateSpecialization));
    ++numSpecializationsNew;
    return classTemplateSpecialization;
}

ClassTemplateSpecializationSymbol* SymbolTable::CopyClassTemplateSpecialization(ClassTemplateSpecializationSymbol* source)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, source->GetSpan(), Span());
    }
#endif
    ClassTypeSymbol* classTemplate = source->GetClassTemplate();
    if (classTemplate == nullptr)
    {
        throw std::runtime_error("class template is null!");
    }
    std::vector<TypeSymbol*>& templateArgumentTypes = source->TemplateArgumentTypes();
    ClassTemplateSpecializationKey key(classTemplate, templateArgumentTypes);
    std::u32string classTemplateSpecializationName = MakeClassTemplateSpecializationName(classTemplate, templateArgumentTypes);
    ClassTemplateSpecializationSymbol* copy = new ClassTemplateSpecializationSymbol(source->GetSpan(), source->SourceModuleId(), classTemplateSpecializationName, classTemplate, templateArgumentTypes);
    copy->SetTypeId(source->TypeId());
    copy->SetGroupName(classTemplate->GroupName());
    classTemplateSpecializationMap[key] = copy;
    copy->SetParent(&globalNs);
    copy->SetModule(module);
    classTemplateSpecializations.push_back(std::unique_ptr<ClassTemplateSpecializationSymbol>(copy));
    derivedTypeMap[copy->TypeId()].clear();
    specializationCopyMap[source] = copy;
    ++numSpecializationsCopied;
    return copy;
}

ClassTemplateSpecializationSymbol* SymbolTable::GetCurrentClassTemplateSpecialization(ClassTemplateSpecializationSymbol* source)
{
    auto it = specializationCopyMap.find(source);
    if (it != specializationCopyMap.cend())
    {
        return it->second;
    }
    return source;
}

ArrayTypeSymbol* SymbolTable::MakeArrayType(TypeSymbol* elementType, int64_t size, const Span& span, const boost::uuids::uuid& moduleId)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, elementType->GetSpan(), span);
    }
#endif
    ArrayKey key(elementType, size);
    auto it = arrayTypeMap.find(key);
    if (elementType->GetSymbolType() != SymbolType::classTemplateSpecializationSymbol && it != arrayTypeMap.cend())
    {
        ArrayTypeSymbol* arrayType = it->second;
        return arrayType;
    }
    ArrayTypeSymbol* arrayType = new ArrayTypeSymbol(span, moduleId, elementType->FullName() + U"[" + ToUtf32(std::to_string(size)) + U"]", elementType, size);
    SetTypeIdFor(arrayType);
    arrayTypeMap[key] = arrayType;
    arrayType->SetParent(&globalNs); 
    arrayType->SetModule(module);
    ArrayLengthFunction* arrayLengthFunction = new ArrayLengthFunction(arrayType);
    SetFunctionIdFor(arrayLengthFunction);
    arrayType->AddMember(arrayLengthFunction);
    ArrayBeginFunction* arrayBeginFunction = new ArrayBeginFunction(arrayType);
    SetFunctionIdFor(arrayBeginFunction);
    arrayType->AddMember(arrayBeginFunction);
    ArrayEndFunction* arrayEndFunction = new ArrayEndFunction(arrayType);
    SetFunctionIdFor(arrayEndFunction);
    arrayType->AddMember(arrayEndFunction);
    ArrayCBeginFunction* arrayCBeginFunction = new ArrayCBeginFunction(arrayType);
    SetFunctionIdFor(arrayCBeginFunction);
    arrayType->AddMember(arrayCBeginFunction);
    ArrayCEndFunction* arrayCEndFunction = new ArrayCEndFunction(arrayType);
    SetFunctionIdFor(arrayCEndFunction);
    arrayType->AddMember(arrayCEndFunction);
    TypedefSymbol* iterator = new TypedefSymbol(span, moduleId, U"Iterator");
    iterator->SetModule(module);
    iterator->SetAccess(SymbolAccess::public_);
    iterator->SetType(arrayType->ElementType()->AddPointer(span, moduleId));
    iterator->SetBound();
    arrayType->AddMember(iterator);
    TypedefSymbol* constIterator = new TypedefSymbol(span, moduleId, U"ConstIterator");
    constIterator->SetModule(module);
    constIterator->SetAccess(SymbolAccess::public_);
    constIterator->SetType(arrayType->ElementType()->AddConst(span, moduleId)->AddPointer(span, moduleId));
    constIterator->SetBound();
    arrayType->AddMember(constIterator);
    arrayTypes.push_back(std::unique_ptr<ArrayTypeSymbol>(arrayType));
    derivedTypeMap[arrayType->TypeId()].clear();
    return arrayType;
}

void SymbolTable::AddClassTemplateSpecializationsToClassTemplateSpecializationMap(const std::vector<ClassTemplateSpecializationSymbol*>& classTemplateSpecializations)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, Span(), Span());
    }
#endif
    for (ClassTemplateSpecializationSymbol* classTemplateSpecialization : classTemplateSpecializations)
    {
        ClassTemplateSpecializationKey key(classTemplateSpecialization->GetClassTemplate(), classTemplateSpecialization->TemplateArgumentTypes());
        auto it = classTemplateSpecializationMap.find(key);
        if (it == classTemplateSpecializationMap.cend())
        {
            classTemplateSpecializationMap[key] = classTemplateSpecialization;
        }
    }
}

void SymbolTable::AddConversion(FunctionSymbol* conversion, Module* module)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module && module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, conversion->GetSpan(), Span());
    }
#endif
    conversionTable.AddConversion(conversion);
}

void SymbolTable::AddConversion(FunctionSymbol* conversion)
{
    AddConversion(conversion, nullptr);
}

FunctionSymbol* SymbolTable::GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType, const Span& span, const boost::uuids::uuid& moduleId) const
{
    return conversionTable.GetConversion(sourceType, targetType, span, moduleId);
}

void SymbolTable::AddPolymorphicClass(ClassTypeSymbol* polymorphicClass)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, polymorphicClass->GetSpan(), Span());
    }
#endif
    if (!polymorphicClass->IsPolymorphic())
    {
        throw Exception("not a polymorphic class", polymorphicClass->GetSpan(), polymorphicClass->SourceModuleId());
    }
    polymorphicClasses.insert(polymorphicClass);
}

void SymbolTable::AddClassHavingStaticConstructor(ClassTypeSymbol* classHavingStaticConstructor)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, classHavingStaticConstructor->GetSpan(), Span());
    }
#endif
    if (!classHavingStaticConstructor->StaticConstructor())
    {
        throw Exception("not having static constructor", classHavingStaticConstructor->GetSpan(), classHavingStaticConstructor->SourceModuleId());
    }
    classesHavingStaticConstructor.insert(classHavingStaticConstructor);
}

void SymbolTable::AddJsonClass(const std::u32string& jsonClass)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (module->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), module, Span(), Span());
    }
#endif
    jsonClasses.insert(jsonClass);
}

std::vector<TypeSymbol*> SymbolTable::Types() const
{
    std::vector<TypeSymbol*> types;
    for (const auto& p : typeNameMap)
    {
        TypeSymbol* type = p.second;
        types.push_back(type);
    }
    for (const std::unique_ptr<DerivedTypeSymbol>& dt : derivedTypes)
    {
        types.push_back(dt.get());
    }
    for (const std::unique_ptr<ClassTemplateSpecializationSymbol>& ts : classTemplateSpecializations)
    {
        types.push_back(ts.get());
    }
    return types;
}

void SymbolTable::Copy(const SymbolTable& that)
{
    for (const auto& p : that.typeIdMap)
    {
        typeIdMap[p.first] = p.second;
    }
    for (const auto& p : that.functionIdMap)
    {
        functionIdMap[p.first] = p.second;
    }
}

void SymbolTable::MapProfiledFunction(const boost::uuids::uuid& functionId, const std::u32string& profiledFunctionName)
{
    profiledFunctionNameMap[functionId] = profiledFunctionName;
}

std::u32string SymbolTable::GetProfiledFunctionName(const boost::uuids::uuid& functionId) const
{
    auto it = profiledFunctionNameMap.find(functionId);
    if (it != profiledFunctionNameMap.cend())
    {
        return it->second;
    }
    return std::u32string();
}

std::unique_ptr<sngxml::dom::Document> SymbolTable::ToDomDocument() 
{
    TypeMap typeMap;
    std::unique_ptr<sngxml::dom::Document> doc(new sngxml::dom::Document());
    std::unique_ptr<sngxml::dom::Element> st(new sngxml::dom::Element(U"symbolTable"));
    std::unique_ptr<sngxml::dom::Element> globalNsElement = globalNs.ToDomElement(typeMap);
    std::unique_ptr<sngxml::dom::Element> symbols(new sngxml::dom::Element(U"symbols"));
    symbols->AppendChild(std::unique_ptr<sngxml::dom::Node>(globalNsElement.release()));
    st->AppendChild(std::unique_ptr<sngxml::dom::Node>(symbols.release()));
    std::unique_ptr<sngxml::dom::Element> types(new sngxml::dom::Element(U"types"));
    std::vector<std::unique_ptr<sngxml::dom::Element>> typeElements = typeMap.TypeElements();
    int n = typeElements.size();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(std::move(typeElements[i]));
        types->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    st->AppendChild(std::unique_ptr<sngxml::dom::Node>(types.release()));
    doc->AppendChild(std::unique_ptr<sngxml::dom::Node>(st.release()));
    return doc;
}

void SymbolTable::MapInvoke(IdentifierNode* invokeId, FunctionSymbol* functionSymbol)
{
    invokeMap[invokeId] = functionSymbol;
}

FunctionSymbol* SymbolTable::GetInvoke(IdentifierNode* invokeId) const
{
    auto it = invokeMap.find(invokeId);
    if (it != invokeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void SymbolTable::MapSymbol(Node* node, Symbol* symbol)
{
    mappedNodeSymbolMap[node] = symbol;
}

Symbol* SymbolTable::GetMappedSymbol(Node* node) const
{
    auto it = mappedNodeSymbolMap.find(node);
    if (it != mappedNodeSymbolMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void SymbolTable::MapIdentifierToSymbolDefinition(IdentifierNode* identifierNode, Module* module, Symbol* symbol)
{
    if (identifierSymbolDefinitionMap.find(identifierNode) != identifierSymbolDefinitionMap.cend()) return;
    identifierSymbolDefinitionMap[identifierNode] = symbol;
    SymbolLocation identifierLocation = MakeSymbolLocation(identifierNode->GetSpan(), module);
    SymbolLocation definitionLocation;
    if (symbol->GetLocation(definitionLocation))
    {
        symbolDefinitionMap[identifierLocation] = definitionLocation;
    }
}

SymbolLocation* SymbolTable::GetDefinitionLocation(const SymbolLocation& identifierLocation) 
{
    auto it = symbolDefinitionMap.find(identifierLocation);
    if (it != symbolDefinitionMap.cend())
    {
        return &(it->second);
    }
    else
    {
        return nullptr;
    }
}

void SymbolTable::InitUuids()
{
    derivationIds.clear();
    for (int i = 0; i < static_cast<int>(Derivation::max); ++i)
    {
        derivationIds.push_back(boost::uuids::random_generator()());
    }
    positionIds.clear();
    for (int i = 0; i < boost::uuids::uuid::static_size(); ++i)
    {
        positionIds.push_back(boost::uuids::random_generator()());
    }
}

const boost::uuids::uuid& SymbolTable::GetDerivationId(Derivation derivation) const
{
    if (!module->IsCore())
    {
        throw std::runtime_error("internal error: derivation id provided only from System.Core module");
    }
    int index = static_cast<int>(derivation);
    if (index < 0 || index >= derivationIds.size())
    {
        throw std::runtime_error("internal error: invalid derivation id index");
    }
    return derivationIds[index];
}

const boost::uuids::uuid& SymbolTable::GetPositionId(int index) const
{
    if (module->Name() != U"System.Core")
    {
        throw std::runtime_error("position id provided only from System.Core module");
    }
    if (index < 0 || index >= positionIds.size())
    {
        throw std::runtime_error("invalid position id index");
    }
    return positionIds[index];
}

void SymbolTable::AddFunctionSymbol(std::unique_ptr<FunctionSymbol>&& functionSymbol)
{
    std::lock_guard<std::recursive_mutex> lock(module->GetLock());
    functionSymbols.push_back(std::move(functionSymbol));
}

void SymbolTable::Check()
{
    globalNs.Check();
    if (!module)
    {
        throw SymbolCheckException("symbol table has no module", globalNs.GetSpan(), globalNs.SourceModuleId());
    }
    for (const auto& p : nsMap)
    {
        if (!p.first || !p.second)
        {
            throw SymbolCheckException("symbol table namespace map contains null namespace pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : nodeSymbolMap)
    {
        if (!p.first || !p.second)
        {
            throw SymbolCheckException("symbol table node symbol map contains null symbol or node pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : symbolNodeMap)
    {
        if (!p.first || !p.second)
        {
            throw SymbolCheckException("symbol table symbol node map contains null symbol or node pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : typeIdMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException("symbol table type id map contains null symbol pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : functionIdMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException("symbol table function id map contains null function pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : typeNameMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException("symbol table type name map contains null type pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : derivedTypeMap)
    {
        for (DerivedTypeSymbol* type : p.second)
        {
            if (!type)
            {
                throw SymbolCheckException("symbol table derived type name map contains null derived type pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
            }
        }
    }
    for (const auto& p : classTemplateSpecializationMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException("symbol table class template specialization map contains null specialization pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : specializationCopyMap)
    {
        if (!p.first || !p.second)
        {
            throw SymbolCheckException("symbol table specialization copy map contains null specialization pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (const auto& p : arrayTypeMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException("symbol table specialization array type map contains null array pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    conversionTable.Check();
    for (ClassTypeSymbol* p : polymorphicClasses)
    {
        if (!p)
        {
            throw SymbolCheckException("symbol table contains null polymorphic class pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
    for (ClassTypeSymbol* p : classesHavingStaticConstructor)
    {
        if (!p)
        {
            throw SymbolCheckException("symbol table contains null static class pointer", globalNs.GetSpan(), globalNs.SourceModuleId());
        }
    }
}

void SymbolTable::WriteSymbolDefinitionMap(SymbolWriter& writer)
{
    int64_t n = symbolDefinitionMap.size();
    writer.GetBinaryWriter().Write(n);
    for (const std::pair<SymbolLocation, SymbolLocation>& p : symbolDefinitionMap)
    {
        const SymbolLocation& left = p.first;
        writer.GetBinaryWriter().Write(left.moduleId);
        writer.GetBinaryWriter().Write(left.fileIndex);
        writer.GetBinaryWriter().Write(left.line);
        writer.GetBinaryWriter().Write(left.scol);
        writer.GetBinaryWriter().Write(left.ecol);
        const SymbolLocation& right = p.second;
        writer.GetBinaryWriter().Write(right.moduleId);
        writer.GetBinaryWriter().Write(right.fileIndex);
        writer.GetBinaryWriter().Write(right.line);
        writer.GetBinaryWriter().Write(right.scol);
        writer.GetBinaryWriter().Write(right.ecol);
    }
}

void SymbolTable::ReadSymbolDefinitionMap(SymbolReader& reader)
{
    int64_t n = reader.GetBinaryReader().ReadLong();
    for (int64_t i = 0; i < n; ++i)
    {
        SymbolLocation left;
        reader.GetBinaryReader().ReadUuid(left.moduleId);
        left.fileIndex = reader.GetBinaryReader().ReadInt();
        left.line = reader.GetBinaryReader().ReadInt();
        left.scol = reader.GetBinaryReader().ReadInt();
        left.ecol = reader.GetBinaryReader().ReadInt();
        SymbolLocation right;
        reader.GetBinaryReader().ReadUuid(right.moduleId);
        right.fileIndex = reader.GetBinaryReader().ReadInt();
        right.line = reader.GetBinaryReader().ReadInt();
        right.scol = reader.GetBinaryReader().ReadInt();
        right.ecol = reader.GetBinaryReader().ReadInt();
        symbolDefinitionMap[left] = right;
    }
}

class IntrinsicConcepts
{
public:
    static void Init();
    static void Done();
    static IntrinsicConcepts& Instance() { return *instance; }
    void AddIntrinsicConcept(ConceptNode* intrinsicConcept);
    const std::vector<std::unique_ptr<ConceptNode>>& GetIntrinsicConcepts() const { return intrinsicConcepts; }
    bool Initialized() const { return initialized; }
    void SetInitialized() { initialized = true; }
private:
    static std::unique_ptr<IntrinsicConcepts> instance;
    std::vector<std::unique_ptr<ConceptNode>> intrinsicConcepts;
    IntrinsicConcepts();
    bool initialized;
};

std::unique_ptr<IntrinsicConcepts> IntrinsicConcepts::instance;

void IntrinsicConcepts::Init()
{
    instance.reset(new IntrinsicConcepts());
}

void IntrinsicConcepts::Done()
{
    instance.reset();
}

IntrinsicConcepts::IntrinsicConcepts() : initialized(false)
{
}

void IntrinsicConcepts::AddIntrinsicConcept(ConceptNode* intrinsicConcept)
{
    intrinsicConcepts.push_back(std::unique_ptr<ConceptNode>(intrinsicConcept));
}

void InitCoreSymbolTable(SymbolTable& symbolTable)
{
    symbolTable.InitUuids();
    BoolTypeSymbol* boolType = new BoolTypeSymbol(Span(), boost::uuids::nil_uuid(), U"bool");
    SByteTypeSymbol* sbyteType = new SByteTypeSymbol(Span(), boost::uuids::nil_uuid(), U"sbyte");
    ByteTypeSymbol* byteType = new ByteTypeSymbol(Span(), boost::uuids::nil_uuid(), U"byte");
    ShortTypeSymbol* shortType = new ShortTypeSymbol(Span(), boost::uuids::nil_uuid(), U"short");
    UShortTypeSymbol* ushortType = new UShortTypeSymbol(Span(), boost::uuids::nil_uuid(), U"ushort");
    IntTypeSymbol* intType = new IntTypeSymbol(Span(), boost::uuids::nil_uuid(), U"int");
    UIntTypeSymbol* uintType = new UIntTypeSymbol(Span(), boost::uuids::nil_uuid(), U"uint");
    LongTypeSymbol* longType = new LongTypeSymbol(Span(), boost::uuids::nil_uuid(), U"long");
    ULongTypeSymbol* ulongType = new ULongTypeSymbol(Span(), boost::uuids::nil_uuid(), U"ulong");
    FloatTypeSymbol* floatType = new FloatTypeSymbol(Span(), boost::uuids::nil_uuid(), U"float");
    DoubleTypeSymbol* doubleType = new DoubleTypeSymbol(Span(), boost::uuids::nil_uuid(), U"double");
    CharTypeSymbol* charType = new CharTypeSymbol(Span(), boost::uuids::nil_uuid(), U"char");
    WCharTypeSymbol* wcharType = new WCharTypeSymbol(Span(), boost::uuids::nil_uuid(), U"wchar");
    UCharTypeSymbol* ucharType = new UCharTypeSymbol(Span(), boost::uuids::nil_uuid(), U"uchar");
    VoidTypeSymbol* voidType = new VoidTypeSymbol(Span(), boost::uuids::nil_uuid(), U"void");
    symbolTable.AddTypeSymbolToGlobalScope(boolType);
    symbolTable.AddTypeSymbolToGlobalScope(sbyteType);
    symbolTable.AddTypeSymbolToGlobalScope(byteType);
    symbolTable.AddTypeSymbolToGlobalScope(shortType);
    symbolTable.AddTypeSymbolToGlobalScope(ushortType);
    symbolTable.AddTypeSymbolToGlobalScope(intType);
    symbolTable.AddTypeSymbolToGlobalScope(uintType);
    symbolTable.AddTypeSymbolToGlobalScope(longType);
    symbolTable.AddTypeSymbolToGlobalScope(ulongType);
    symbolTable.AddTypeSymbolToGlobalScope(floatType);
    symbolTable.AddTypeSymbolToGlobalScope(doubleType);
    symbolTable.AddTypeSymbolToGlobalScope(charType);
    symbolTable.AddTypeSymbolToGlobalScope(wcharType);
    symbolTable.AddTypeSymbolToGlobalScope(ucharType);
    symbolTable.AddTypeSymbolToGlobalScope(voidType);
    symbolTable.AddTypeSymbolToGlobalScope(new NullPtrType(Span(), boost::uuids::nil_uuid(), U"@nullptr_type"));
    MakeBasicTypeOperations(symbolTable, boolType, sbyteType, byteType, shortType, ushortType, intType, uintType, longType, ulongType, floatType, doubleType, charType, wcharType, ucharType, voidType);
    if (!IntrinsicConcepts::Instance().Initialized())
    {
        IntrinsicConcepts::Instance().SetInitialized();
        IntrinsicConcepts::Instance().AddIntrinsicConcept(new SameConceptNode());
        IntrinsicConcepts::Instance().AddIntrinsicConcept(new DerivedConceptNode());
        IntrinsicConcepts::Instance().AddIntrinsicConcept(new ConvertibleConceptNode());
        IntrinsicConcepts::Instance().AddIntrinsicConcept(new ExplicitlyConvertibleConceptNode());
        IntrinsicConcepts::Instance().AddIntrinsicConcept(new CommonConceptNode());
        IntrinsicConcepts::Instance().AddIntrinsicConcept(new NonreferenceTypeConceptNode());
    }
    for (const std::unique_ptr<ConceptNode>& conceptNode : IntrinsicConcepts::Instance().GetIntrinsicConcepts())
    {
        symbolTable.BeginConcept(*conceptNode, false);
        ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(symbolTable.Container());
        conceptSymbol->SetAccess(SymbolAccess::public_);
        int n = conceptNode->TypeParameters().Count();
        for (int i = 0; i < n; ++i)
        {
            IdentifierNode* typeParamId = conceptNode->TypeParameters()[i];
            symbolTable.AddTemplateParameter(*typeParamId);
        }
        symbolTable.EndConcept();
        conceptSymbol->ComputeName();
    }
    symbolTable.AddTypeSymbolToGlobalScope(new StringFunctionContainerSymbol());
}

/*
void CreateClassFile(const std::string& executableFilePath, SymbolTable& symbolTable)
{
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(symbolTable.GetModule()->LogStreamId(), "Generating class file...");
    }
    boost::filesystem::path cfp = boost::filesystem::path(executableFilePath).replace_extension(".cls");
    std::string classFilePath = cfp.generic_string();
    const std::unordered_set<ClassTypeSymbol*>& polymorphicClasses = symbolTable.PolymorphicClasses();
    std::unordered_map<boost::uuids::uuid, ClassTypeSymbol*, boost::hash<boost::uuids::uuid>> classIdClassMap;
    for (ClassTypeSymbol* polymorphicClass : polymorphicClasses)
    {
        classIdClassMap[polymorphicClass->TypeId()] = polymorphicClass;
    }
    uint32_t n = classIdClassMap.size();
    BinaryWriter writer(classFilePath);
    writer.WriteULEB128UInt(n);
    for (const auto& p : classIdClassMap)
    {
        const boost::uuids::uuid& typeId = p.first;
        ClassTypeSymbol* cls = p.second;
        std::string vmtObjectName = cls->VmtObjectNameStr();
        boost::uuids::uuid baseClassTypeId = boost::uuids::nil_uuid();
        if (cls->BaseClass())
        {
            baseClassTypeId = cls->BaseClass()->TypeId();
        }
        writer.Write(typeId);
        writer.Write(vmtObjectName);
        writer.Write(baseClassTypeId);
    }
    const std::unordered_set<ClassTypeSymbol*>& classesHavingStaticConstructor = symbolTable.ClassesHavingStaticConstructor();
    uint32_t ns = classesHavingStaticConstructor.size();
    writer.WriteULEB128UInt(ns);
    for (ClassTypeSymbol* classHavingStaticConstructor : classesHavingStaticConstructor)
    {
        const boost::uuids::uuid& typeId = classHavingStaticConstructor->TypeId();
        writer.Write(typeId);
    }
    if (GetGlobalFlag(GlobalFlags::verbose) && !GetGlobalFlag(GlobalFlags::unitTest))
    {
        LogMessage(symbolTable.GetModule()->LogStreamId(), "==> " + classFilePath);
    }
}
*/

void InitSymbolTable()
{
    IntrinsicConcepts::Init();
}

void DoneSymbolTable()
{
    IntrinsicConcepts::Done();
}

} } // namespace cmajor::symbols
