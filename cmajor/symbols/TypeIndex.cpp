// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/TypeIndex.hpp>
#include <cmajor/symbols/BasicTypeSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/DerivedTypeSymbol.hpp>
#include <cmajor/symbols/DelegateSymbol.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/ArrayTypeSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/Module.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

cmajor::debug::DIPrimitiveType::Kind ToPrimitiveTypeKind(SymbolType symbolType)
{
    switch (symbolType)
    {
        case SymbolType::boolTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::boolType;
        case SymbolType::sbyteTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::sbyteType;
        case SymbolType::byteTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::byteType;
        case SymbolType::shortTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::shortType;
        case SymbolType::ushortTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::ushortType;
        case SymbolType::intTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::intType;
        case SymbolType::uintTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::uintType;
        case SymbolType::longTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::longType;
        case SymbolType::ulongTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::ulongType;
        case SymbolType::floatTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::floatType;
        case SymbolType::doubleTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::doubleType;
        case SymbolType::charTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::charType;
        case SymbolType::wcharTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::wcharType;
        case SymbolType::ucharTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::ucharType;
        case SymbolType::voidTypeSymbol: return cmajor::debug::DIPrimitiveType::Kind::voidType;
        default: return cmajor::debug::DIPrimitiveType::Kind::none;
    }
}

void TypeIndex::AddType(const boost::uuids::uuid& typeId, TypeSymbol* typeSymbol, cmajor::ir::Emitter& emitter)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    auto it = typeMap.find(typeId);
    if (it == typeMap.cend())
    {
        if (typeSymbol->IsBasicTypeSymbol())
        {
            cmajor::debug::DIPrimitiveType* type = new cmajor::debug::DIPrimitiveType();
            type->SetPrimitiveTypeKind(ToPrimitiveTypeKind(typeSymbol->GetSymbolType()));
            type->SetId(typeSymbol->TypeId());
            type->SetName(ToUtf8(typeSymbol->Name()));
            type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
            typeMap[type->Id()] = type;
            diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
        }
        else
        {
            switch (typeSymbol->GetSymbolType())
            {
                case SymbolType::enumTypeSymbol:
                {
                    EnumTypeSymbol* enumType = static_cast<EnumTypeSymbol*>(typeSymbol);
                    cmajor::debug::DIEnumType* type = new cmajor::debug::DIEnumType();
                    type->SetId(typeSymbol->TypeId());
                    type->SetName(ToUtf8(typeSymbol->FullName()));
                    type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                    typeMap[type->Id()] = type;
                    type->SetUnderlyingTypeId(enumType->UnderlyingType()->TypeId());
                    diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                    AddType(enumType->UnderlyingType()->TypeId(), enumType->UnderlyingType(), emitter);
                    std::vector<EnumConstantSymbol*> enumConstants = enumType->GetEnumConstants();
                    for (EnumConstantSymbol* enumConstant : enumConstants)
                    {
                        cmajor::debug::DIEnumConstant ec;
                        ec.SetName(ToUtf8(enumConstant->Name()));
                        ec.SetValue(enumConstant->GetValue()->ToString());
                        ec.SetStrValue(ToUtf8(enumConstant->StrValue()));
                        type->AddEnumConstant(std::move(ec));
                    }
                    break;
                }
                case SymbolType::classTypeSymbol:
                {
                    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(typeSymbol);
                    cmajor::debug::DIClassType* type = new cmajor::debug::DIClassType();
                    type->SetId(typeSymbol->TypeId());
                    type->SetName(ToUtf8(typeSymbol->FullName()));
                    type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                    typeMap[type->Id()] = type;
                    diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                    if (classType->IsPolymorphic())
                    {
                        type->SetPolymorphic();
                        type->SetVmtPtrIndex(classType->VmtPtrIndex());
                        type->SetVmtVariableName(emitter.MakeVmtVariableName(classType->VmtObjectName(emitter)));
                    }
                    if (classType->BaseClass())
                    {
                        type->SetBaseClassId(classType->BaseClass()->TypeId());
                        AddType(classType->BaseClass()->TypeId(), classType->BaseClass(), emitter);
                    }
                    for (TemplateParameterSymbol* templateParameter : classType->TemplateParameters())
                    {
                        cmajor::debug::DITemplateParameter* tp = new cmajor::debug::DITemplateParameter();
                        tp->SetId(templateParameter->TypeId());
                        tp->SetName(ToUtf8(templateParameter->Name())); 
                        typeMap[tp->Id()] = type;
                        type->AddTemplateParameter(tp);
                    }
                    for (MemberVariableSymbol* memberVariable : classType->MemberVariables())
                    {
                        cmajor::debug::DIVariable* variable = new cmajor::debug::DIVariable(cmajor::debug::DIVariable::Kind::memberVariable);
                        variable->SetName(ToUtf8(memberVariable->Name()));
                        variable->SetIrName("m" + std::to_string(memberVariable->LayoutIndex()));
                        variable->SetTypeId(memberVariable->GetType()->TypeId());
                        AddType(memberVariable->GetType()->TypeId(), memberVariable->GetType(), emitter);
                        type->AddMemberVariable(variable);
                    }
                    break;
                }
                case SymbolType::classTemplateSpecializationSymbol:
                {
                    ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(typeSymbol);
                    cmajor::debug::DIClassTemplateSpecializationType* type = new cmajor::debug::DIClassTemplateSpecializationType();
                    ClassTypeSymbol* classTemplateType = specialization->GetClassTemplate();
                    AddType(classTemplateType->TypeId(), classTemplateType, emitter);
                    type->SetPrimaryTypeId(classTemplateType->TypeId());
                    for (TypeSymbol* templateArgumentType : specialization->TemplateArgumentTypes())
                    {
                        type->AddTemplateArgumentTypeId(templateArgumentType->TypeId());
                        AddType(templateArgumentType->TypeId(), templateArgumentType, emitter);
                    }
                    cmajor::debug::ContainerClassTemplateKind containerKind = GetContainerClassTemplateKind(classTemplateType->FullName());
                    type->SetContainerClassTemplateKind(containerKind);
                    if (containerKind != cmajor::debug::ContainerClassTemplateKind::notContainerClassTemplate)
                    {
                        Symbol* valueTypeSymbol = specialization->GetContainerScope()->Lookup(U"ValueType", ScopeLookup::this_and_base_and_parent);
                        if (valueTypeSymbol && valueTypeSymbol->GetSymbolType() == SymbolType::typedefSymbol)
                        {
                            TypedefSymbol* valueTypeTypedef = static_cast<TypedefSymbol*>(valueTypeSymbol);
                            TypeSymbol* valueType = valueTypeTypedef->GetType();
                            type->SetValueTypeId(valueType->TypeId());
                            AddType(valueType->TypeId(), valueType, emitter);
                        }
                    }
                    type->SetId(typeSymbol->TypeId());
                    type->SetName(ToUtf8(typeSymbol->FullName()));
                    type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                    typeMap[type->Id()] = type;
                    diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                    if (specialization->IsPolymorphic())
                    {
                        type->SetPolymorphic();
                        type->SetVmtPtrIndex(specialization->VmtPtrIndex());
                        type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                        type->SetVmtVariableName(emitter.MakeVmtVariableName(specialization->VmtObjectName(emitter)));
                    }
                    if (specialization->BaseClass())
                    {
                        type->SetBaseClassId(specialization->BaseClass()->TypeId());
                        AddType(specialization->BaseClass()->TypeId(), specialization->BaseClass(), emitter);
                    }
                    for (MemberVariableSymbol* memberVariable : specialization->MemberVariables())
                    {
                        cmajor::debug::DIVariable* variable = new cmajor::debug::DIVariable(cmajor::debug::DIVariable::Kind::memberVariable);
                        variable->SetName(ToUtf8(memberVariable->Name()));
                        variable->SetIrName("m" + std::to_string(memberVariable->LayoutIndex()));
                        variable->SetTypeId(memberVariable->GetType()->TypeId());
                        AddType(memberVariable->GetType()->TypeId(), memberVariable->GetType(), emitter);
                        type->AddMemberVariable(variable);
                    }
                    break;
                }
                case SymbolType::delegateTypeSymbol:
                {
                    DelegateTypeSymbol* delegateTypeSymbol = static_cast<DelegateTypeSymbol*>(typeSymbol);
                    cmajor::debug::DIDelegateType* type = new cmajor::debug::DIDelegateType();
                    type->SetId(typeSymbol->TypeId());
                    type->SetName(ToUtf8(typeSymbol->FullName()));
                    type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                    typeMap[type->Id()] = type;
                    diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                    break;
                }
                case SymbolType::classDelegateTypeSymbol:
                {
                    ClassDelegateTypeSymbol* classDelegateTypeSymbol = static_cast<ClassDelegateTypeSymbol*>(typeSymbol);
                    ClassTypeSymbol* classType = classDelegateTypeSymbol->ObjectDelegatePairType();
                    cmajor::debug::DIClassDelegateType* type = new cmajor::debug::DIClassDelegateType();
                    type->SetId(typeSymbol->TypeId());
                    type->SetName(ToUtf8(typeSymbol->FullName()));
                    type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                    typeMap[type->Id()] = type;
                    diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                    AddType(classType->TypeId(), classType, emitter);
                    type->SetClassTypeId(classType->TypeId());
                    break;
                }
                case SymbolType::interfaceTypeSymbol:
                {
                    InterfaceTypeSymbol* interfaceTypeSymbol = static_cast<InterfaceTypeSymbol*>(typeSymbol);
                    cmajor::debug::DIInterfaceType* type = new cmajor::debug::DIInterfaceType();
                    type->SetId(typeSymbol->TypeId());
                    type->SetName(ToUtf8(typeSymbol->FullName()));
                    type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                    typeMap[type->Id()] = type;
                    diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                    break;
                }
                case SymbolType::derivedTypeSymbol:
                {
                    DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(typeSymbol);
                    if (derivedType->IsConstType())
                    {
                        TypeSymbol* nonConstType = derivedType->RemoveConst(derivedType->GetSpan());
                        cmajor::debug::DIConstType* type = new cmajor::debug::DIConstType();
                        type->SetId(typeSymbol->TypeId());
                        type->SetName(ToUtf8(typeSymbol->FullName()));
                        type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                        type->SetBaseTypeId(nonConstType->TypeId());
                        typeMap[type->Id()] = type;
                        diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                        AddType(nonConstType->TypeId(), nonConstType, emitter);
                    }
                    else if (derivedType->IsReferenceType())
                    {
                        TypeSymbol* nonReferenceType = derivedType->RemoveReference(derivedType->GetSpan());
                        cmajor::debug::DIReferenceType* type = new cmajor::debug::DIReferenceType();
                        type->SetId(typeSymbol->TypeId());
                        type->SetName(ToUtf8(typeSymbol->FullName()));
                        type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                        type->SetBaseTypeId(nonReferenceType->TypeId());
                        typeMap[type->Id()] = type;
                        diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                        AddType(nonReferenceType->TypeId(), nonReferenceType, emitter);
                    }
                    else if (derivedType->IsPointerType())
                    {
                        TypeSymbol* pointedToType = derivedType->RemovePointer(derivedType->GetSpan());
                        cmajor::debug::DIPointerType* pointerType = new cmajor::debug::DIPointerType();
                        pointerType->SetPointedTypeId(pointedToType->TypeId());
                        pointerType->SetId(typeSymbol->TypeId());
                        pointerType->SetName(ToUtf8(pointedToType->FullName() + U"*"));
                        pointerType->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                        typeMap[pointerType->Id()] = pointerType;
                        diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(pointerType));
                        AddType(pointedToType->TypeId(), pointedToType, emitter);
                    }
                    break;
                }
                case SymbolType::arrayTypeSymbol:
                {
                    ArrayTypeSymbol* arrayTypeSymbol = static_cast<ArrayTypeSymbol*>(typeSymbol);
                    cmajor::debug::DIArrayType* type = new cmajor::debug::DIArrayType();
                    type->SetId(typeSymbol->TypeId());
                    type->SetName(ToUtf8(typeSymbol->FullName()));
                    type->SetIrName(emitter.GetIrTypeName(typeSymbol->IrType(emitter)));
                    typeMap[type->Id()] = type;
                    TypeSymbol* elementType = arrayTypeSymbol->ElementType();
                    type->SetElementTypeId(elementType->TypeId());
                    type->SetSize(arrayTypeSymbol->Size());
                    diTypes.push_back(std::unique_ptr<cmajor::debug::DIType>(type));
                    AddType(elementType->TypeId(), elementType, emitter);
                    break;
                }
            }
        }
    }
}

void TypeIndex::Write(soulng::util::BinaryWriter& writer)
{
    int32_t n = diTypes.size();
    writer.Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        cmajor::debug::DIType* type = diTypes[i].get();
        cmajor::debug::WriteType(writer, type);
    }
}

} } // namespace cmajor::symbols
