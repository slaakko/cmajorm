// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/DerivedTypeSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/util/Unicode.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::unicode;

std::u32string DerivationStr(Derivation derivation)
{
    switch (derivation)
    {
        case Derivation::constDerivation: return U"C";
        case Derivation::lvalueRefDerivation: return U"R";
        case Derivation::rvalueRefDerivation: return U"RR";
        case Derivation::pointerDerivation: return U"P";
        default: return std::u32string();
    }
}

bool HasFrontConstDerivation(const std::vector<Derivation>& derivations)
{
    if (!derivations.empty())
    {
        if (derivations[0] == Derivation::constDerivation)
        {
            return true;
        }
    }
    return false;
}

bool HasReferenceDerivation(const std::vector<Derivation>& derivations)
{
    for (Derivation derivation : derivations)
    {
        if (derivation == Derivation::lvalueRefDerivation || derivation == Derivation::rvalueRefDerivation)
        {
            return true;
        }
    }
    return false;
}

bool HasLvalueReferenceDerivation(const std::vector<Derivation>& derivations) 
{
    for (Derivation derivation : derivations)
    {
        if (derivation == Derivation::lvalueRefDerivation)
        {
            return true;
        }
    }
    return false;
}

bool HasRvalueReferenceDerivation(const std::vector<Derivation>& derivations)
{
    for (Derivation derivation : derivations)
    {
        if (derivation == Derivation::rvalueRefDerivation)
        {
            return true;
        }
    }
    return false;
}

bool HasReferenceOrConstDerivation(const std::vector<Derivation>& derivations)
{
    for (Derivation derivation : derivations)
    {
        if (derivation == Derivation::lvalueRefDerivation || derivation == Derivation::rvalueRefDerivation || derivation == Derivation::constDerivation)
        {
            return true;
        }
    }
    return false;
}

bool HasPointerDerivation(const std::vector<Derivation>& derivations)
{
    for (Derivation derivation : derivations)
    {
        if (derivation == Derivation::pointerDerivation) return true;
    }
    return false;
}

int CountPointerDerivations(const std::vector<Derivation>& derivations)
{
    int numPointers = 0;
    for (Derivation derivation : derivations)
    {
        if (derivation == Derivation::pointerDerivation)
        {
            ++numPointers;
        }
    }
    return numPointers;
}

TypeDerivationRec MakePlainDerivationRec(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec plainDerivationRec;
    for (Derivation derivation : typeDerivationRec.derivations)
    {
        if (derivation == Derivation::pointerDerivation)
        {
            plainDerivationRec.derivations.push_back(derivation);
        }
    }
    return plainDerivationRec;
}

TypeDerivationRec RemoveReferenceDerivation(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec referenceRemovedDerivationRec;
    for (Derivation derivation : typeDerivationRec.derivations)
    {
        if (derivation == Derivation::constDerivation || derivation == Derivation::pointerDerivation)
        {
            referenceRemovedDerivationRec.derivations.push_back(derivation);
        }
    }
    return referenceRemovedDerivationRec;
}

TypeDerivationRec RemovePointerDerivation(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec pointerRemovedDerivationRec;
    int numPointers = CountPointerDerivations(typeDerivationRec.derivations);
    if (numPointers > 0)
    {
        --numPointers;
    }
    int ip = 0;
    int n = typeDerivationRec.derivations.size();
    for (int i = 0; i < n; ++i)
    {
        Derivation derivation = typeDerivationRec.derivations[i];
        if (derivation == Derivation::pointerDerivation)
        {
            if (ip < numPointers)
            {
                pointerRemovedDerivationRec.derivations.push_back(derivation);
                ++ip;
            }
        }
        else
        {
            pointerRemovedDerivationRec.derivations.push_back(derivation);
        }
    }
    return pointerRemovedDerivationRec;
}

TypeDerivationRec RemoveConstDerivation(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec constRemovedDerivationRec;
    for (Derivation derivation : typeDerivationRec.derivations)
    {
        if (derivation != Derivation::constDerivation)
        {
            constRemovedDerivationRec.derivations.push_back(derivation);
        }
    }
    return constRemovedDerivationRec;
}

TypeDerivationRec AddConstDerivation(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec constAddedDerivationRec;
    constAddedDerivationRec.derivations.push_back(Derivation::constDerivation);
    for (Derivation derivation : typeDerivationRec.derivations)
    {
        if (derivation != Derivation::constDerivation)
        {
            constAddedDerivationRec.derivations.push_back(derivation);
        }
    }
    return constAddedDerivationRec;
}

TypeDerivationRec AddLvalueReferenceDerivation(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec lvalueReferenceAddedDerivationRec;
    for (Derivation derivation : typeDerivationRec.derivations)
    {
        if (derivation != Derivation::lvalueRefDerivation && derivation != Derivation::rvalueRefDerivation)
        {
            lvalueReferenceAddedDerivationRec.derivations.push_back(derivation);
        }
    }
    lvalueReferenceAddedDerivationRec.derivations.push_back(Derivation::lvalueRefDerivation);
    return lvalueReferenceAddedDerivationRec;
}

TypeDerivationRec AddRvalueReferenceDerivation(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec rvalueReferenceAddedDerivationRec;
    for (Derivation derivation : typeDerivationRec.derivations)
    {
        if (derivation != Derivation::lvalueRefDerivation && derivation != Derivation::rvalueRefDerivation)
        {
            rvalueReferenceAddedDerivationRec.derivations.push_back(derivation);
        }
    }
    rvalueReferenceAddedDerivationRec.derivations.push_back(Derivation::rvalueRefDerivation);
    return rvalueReferenceAddedDerivationRec;
}

TypeDerivationRec AddPointerDerivation(const TypeDerivationRec& typeDerivationRec)
{
    TypeDerivationRec pointerAddedDerivationRec;
    int numPointers = CountPointerDerivations(typeDerivationRec.derivations);
    int np = 0;
    for (Derivation derivation : typeDerivationRec.derivations)
    {
        pointerAddedDerivationRec.derivations.push_back(derivation);
        if (derivation == Derivation::pointerDerivation)
        {
            ++np;
        }
        if (np == numPointers)
        {
            pointerAddedDerivationRec.derivations.push_back(Derivation::pointerDerivation);
            ++np;
        }
    }
    return pointerAddedDerivationRec;
}

TypeDerivationRec UnifyDerivations(const TypeDerivationRec& left, const TypeDerivationRec& right)
{
    TypeDerivationRec result;
    if (HasFrontConstDerivation(left.derivations) || HasFrontConstDerivation(right.derivations))
    {
        result.derivations.push_back(Derivation::constDerivation);
    }
    int pointerCount = CountPointerDerivations(left.derivations) + CountPointerDerivations(right.derivations);
    for (int i = 0; i < pointerCount; ++i)
    {
        result.derivations.push_back(Derivation::pointerDerivation);
    }
    if (HasLvalueReferenceDerivation(left.derivations))
    {
        result.derivations.push_back(Derivation::lvalueRefDerivation);
    }
    else if (HasRvalueReferenceDerivation(left.derivations))
    {
        result.derivations.push_back(Derivation::rvalueRefDerivation);
    }
    else if (HasLvalueReferenceDerivation(right.derivations))
    {
        result.derivations.push_back(Derivation::lvalueRefDerivation);
    }
    else if (HasRvalueReferenceDerivation(right.derivations))
    {
        result.derivations.push_back(Derivation::rvalueRefDerivation);
    }
    return result;
}

std::u32string MakeDerivedTypeName(TypeSymbol* baseType, const TypeDerivationRec& derivationRec)
{
    std::u32string derivedTypeName;
    bool constAdded = false;
    if (HasFrontConstDerivation(derivationRec.derivations))
    {
        derivedTypeName.append(U"const");
        constAdded = true;
    }
    if (!derivedTypeName.empty())
    {
        derivedTypeName.append(1, U' ');
    }
    derivedTypeName.append(baseType->Name());
    for (Derivation derivation : derivationRec.derivations)
    {
        switch (derivation)
        {
            case Derivation::constDerivation:
            {
                if (constAdded)
                {
                    constAdded = false;
                }
                else
                {
                    if (!derivedTypeName.empty())
                    {
                        derivedTypeName.append(1, U' ');
                    }
                    derivedTypeName.append(U"const ");
                }
                break;
            }
            case Derivation::lvalueRefDerivation:
            {
                derivedTypeName.append(U"&");
                break;
            }
            case Derivation::rvalueRefDerivation:
            {
                derivedTypeName.append(U"&&");
                break;
            }
            case Derivation::pointerDerivation:
            {
                derivedTypeName.append(U"*");
                break;
            }
        }
    }
    return derivedTypeName;
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::u32string& name_) : TypeSymbol(SymbolType::derivedTypeSymbol, span_, name_), baseType(), derivationRec()
{
}

DerivedTypeSymbol::DerivedTypeSymbol(const Span& span_, const std::u32string& name_, TypeSymbol* baseType_, const TypeDerivationRec& derivationRec_) : 
    TypeSymbol(SymbolType::derivedTypeSymbol, span_, name_), baseType(baseType_), derivationRec(derivationRec_)
{
}

std::u32string DerivedTypeSymbol::SimpleName() const
{
    std::u32string simpleName = baseType->SimpleName();
    for (Derivation derivation : derivationRec.derivations)
    {
        simpleName.append(U"_").append(DerivationStr(derivation));
    }
    return simpleName;
}

void DerivedTypeSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(baseType->TypeId());
    writer.GetBinaryWriter().Write(static_cast<uint8_t>(derivationRec.derivations.size()));
    for (Derivation derivation : derivationRec.derivations)
    {
        writer.GetBinaryWriter().Write(static_cast<uint8_t>(derivation));
    }
}

void DerivedTypeSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 0);
    uint8_t nd = reader.GetBinaryReader().ReadByte();
    for (uint8_t i = 0; i < nd; ++i)
    {
        Derivation derivation = static_cast<Derivation>(reader.GetBinaryReader().ReadByte());
        derivationRec.derivations.push_back(derivation);
    }
}

void DerivedTypeSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    Assert(index == 0, "invalid emplace type index");
    baseType = typeSymbol;
    if (!baseType)
    {
        throw std::runtime_error("internal error: derived type emplace type: base type is null");
    }
}

void DerivedTypeSymbol::ComputeTypeId()
{
    Module* systemCoreModule = GetModule()->GetSystemCoreModule();
    if (!systemCoreModule)
    {
        throw std::runtime_error("internal error: system core module not found");
    }
    SymbolTable& systemCoreSymbolTable = systemCoreModule->GetSymbolTable();
    boost::uuids::uuid typeId = baseType->TypeId();
    int n = derivationRec.derivations.size();
    for (int i = 0; i < n; ++i)
    {
        const boost::uuids::uuid& derivationId = systemCoreSymbolTable.GetDerivationId(derivationRec.derivations[i]);
        const boost::uuids::uuid& positionId = systemCoreSymbolTable.GetPositionId(i);
        for (int k = 0; k < boost::uuids::uuid::static_size(); ++k)
        {
            typeId.data[k] = typeId.data[k] ^ derivationId.data[k] ^ positionId.data[k];
        }
    }
    SetTypeId(typeId);
}

bool DerivedTypeSymbol::IsConstType() const
{
    return HasFrontConstDerivation(derivationRec.derivations);
}

bool DerivedTypeSymbol::IsReferenceType() const 
{
    return HasReferenceDerivation(derivationRec.derivations);
}

bool DerivedTypeSymbol::IsLvalueReferenceType() const
{
    return HasLvalueReferenceDerivation(derivationRec.derivations);
}

bool DerivedTypeSymbol::IsRvalueReferenceType() const
{
    return HasRvalueReferenceDerivation(derivationRec.derivations);
}

bool DerivedTypeSymbol::IsPointerType() const 
{
    return HasPointerDerivation(derivationRec.derivations);
}

bool DerivedTypeSymbol::IsVoidPtrType() const 
{
    return baseType->IsVoidType() && derivationRec.derivations.size() == 1 && derivationRec.derivations.front() == Derivation::pointerDerivation;
}

int DerivedTypeSymbol::PointerCount() const
{
    return CountPointerDerivations(derivationRec.derivations);
}

TypeSymbol* DerivedTypeSymbol::PlainType(const Span& span) 
{
    return PlainType(span, GetRootModuleForCurrentThread());
}

TypeSymbol* DerivedTypeSymbol::PlainType(const Span& span, Module* module)
{
    if (!module)
    {
        module = GetRootModuleForCurrentThread();
    }
    return module->GetSymbolTable().MakeDerivedType(baseType, MakePlainDerivationRec(derivationRec), span);
}

TypeSymbol* DerivedTypeSymbol::RemoveReference(const Span& span)
{
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, RemoveReferenceDerivation(derivationRec), span);
}

TypeSymbol* DerivedTypeSymbol::RemovePointer(const Span& span)
{
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, RemovePointerDerivation(derivationRec), span);
}

TypeSymbol* DerivedTypeSymbol::RemoveConst(const Span& span) 
{
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, RemoveConstDerivation(derivationRec), span);
}

TypeSymbol* DerivedTypeSymbol::AddConst(const Span& span)
{
    if (IsConstType())
    {
        return this;
    }
    else
    {
        return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, AddConstDerivation(derivationRec), span);
    }
}

TypeSymbol* DerivedTypeSymbol::AddLvalueReference(const Span& span)
{
    if (IsLvalueReferenceType())
    {
        return this;
    }
    else
    {
        return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, AddLvalueReferenceDerivation(derivationRec), span);
    }
}

TypeSymbol* DerivedTypeSymbol::AddRvalueReference(const Span& span)
{
    if (IsRvalueReferenceType())
    {
        return this;
    }
    else
    {
        return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, AddRvalueReferenceDerivation(derivationRec), span);
    }
}

TypeSymbol* DerivedTypeSymbol::AddPointer(const Span& span)
{
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, AddPointerDerivation(derivationRec), span);
}

TypeSymbol* DerivedTypeSymbol::RemoveDerivations(const TypeDerivationRec& sourceDerivationRec, const Span& span)
{
    TypeDerivationRec result;
    const std::vector<Derivation>& sourceDerivations = sourceDerivationRec.derivations;
    if (!HasFrontConstDerivation(sourceDerivations) && HasFrontConstDerivation(derivationRec.derivations))
    {
        result.derivations.push_back(Derivation::constDerivation);
    }
    int pointerDiff = CountPointerDerivations(derivationRec.derivations) - CountPointerDerivations(sourceDerivations);
    if (pointerDiff != 0)
    {
        for (int i = 0; i < pointerDiff; ++i)
        {
            result.derivations.push_back(Derivation::pointerDerivation);
        }
    }
    if (!HasLvalueReferenceDerivation(sourceDerivations) && HasLvalueReferenceDerivation(derivationRec.derivations))
    {
        result.derivations.push_back(Derivation::lvalueRefDerivation);
    }
    else if (!HasRvalueReferenceDerivation(sourceDerivations) && HasRvalueReferenceDerivation(derivationRec.derivations))
    {
        result.derivations.push_back(Derivation::rvalueRefDerivation);
    }
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(baseType, result, span);
}

TypeSymbol* DerivedTypeSymbol::Unify(TypeSymbol* sourceType, const Span& span) 
{
    TypeSymbol* newBaseType = baseType->Unify(sourceType->BaseType(), span);
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(newBaseType, UnifyDerivations(derivationRec, sourceType->DerivationRec()), span);
}

bool DerivedTypeSymbol::IsRecursive(TypeSymbol* type, std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>>& tested) 
{
    if (tested.find(TypeId()) != tested.cend()) return TypesEqual(type, this);
    tested.insert(TypeId());
    return TypeSymbol::IsRecursive(type, tested) || baseType->IsRecursive(type, tested);
}

void* DerivedTypeSymbol::IrType(Emitter& emitter) 
{
    void* localIrType = emitter.GetIrTypeByTypeId(TypeId());
    if (!localIrType)
    {
        if (baseType->IsVoidType())
        {
            localIrType = emitter.GetIrTypeForByte();
        }
        else
        {
            localIrType = baseType->IrType(emitter);
        }
        for (Derivation derivation : derivationRec.derivations)
        {
            switch (derivation)
            {
                case Derivation::lvalueRefDerivation:
                case Derivation::rvalueRefDerivation:
                case Derivation::pointerDerivation:
                {
                    localIrType = emitter.GetIrTypeForPtrType(localIrType);
                    break;
                }
                default:
                {
                    // todo
                    break;
                }
            }
        }
        emitter.SetIrTypeByTypeId(TypeId(), localIrType);
    }
    return localIrType;
}

void* DerivedTypeSymbol::CreateDefaultIrValue(Emitter& emitter)
{
    if (HasFrontConstDerivation(derivationRec.derivations) && !HasPointerDerivation(derivationRec.derivations) && !HasReferenceDerivation(derivationRec.derivations))
    {
        return baseType->CreateDefaultIrValue(emitter);
    }
    else
    {
        return emitter.CreateDefaultIrValueForDerivedType(IrType(emitter));
    }
}

void* DerivedTypeSymbol::CreateDIType(Emitter& emitter)
{
    void* diType = baseType->GetDIType(emitter);
    for (Derivation derivation : derivationRec.derivations)
    {
        switch (derivation)
        {
            case Derivation::constDerivation:
            {
                return emitter.CreateConstDIType(diType);
            }
            case Derivation::lvalueRefDerivation:
            {
                return emitter.CreateLValueRefDIType(diType);
            }
            case Derivation::rvalueRefDerivation:
            {
                return emitter.CreateRValueRefDIType(diType);
            }
            case Derivation::pointerDerivation:
            {
                return emitter.CreatePointerDIType(diType);
            }
        }
    }
    return diType;
}

ValueType DerivedTypeSymbol::GetValueType() const 
{
    if (HasPointerDerivation(derivationRec.derivations))
    {
        return ValueType::pointerValue;
    }
    return ValueType::none;
}

void DerivedTypeSymbol::Check()
{
    TypeSymbol::Check();
    if (!baseType)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "derived type symbol has no base type", GetSpan());
    }
}

NullPtrType::NullPtrType(const Span& span_, const std::u32string& name_) : TypeSymbol(SymbolType::nullPtrTypeSymbol, span_, name_)
{
}

void* NullPtrType::IrType(Emitter& emitter)
{
    return emitter.GetIrTypeForVoidPtrType();
}

void* NullPtrType::CreateDefaultIrValue(Emitter& emitter)
{
    return emitter.CreateDefaultIrValueForPtrType(IrType(emitter));
}

ValueType NullPtrType::GetValueType() const
{
    return ValueType::nullValue;
}

} } // namespace cmajor::symbols
