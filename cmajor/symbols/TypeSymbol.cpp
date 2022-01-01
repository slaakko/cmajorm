// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

TypeSymbol::TypeSymbol(SymbolType symbolType_, const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    ContainerSymbol(symbolType_, span_, sourceModuleId_, name_), typeId(boost::uuids::nil_uuid())
{
}

void TypeSymbol::Write(SymbolWriter& writer)
{
    ContainerSymbol::Write(writer);
    Assert(!typeId.is_nil(), "type id not set");
    writer.GetBinaryWriter().Write(typeId);
}

void TypeSymbol::Read(SymbolReader& reader)
{
    ContainerSymbol::Read(reader);
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->AddTypeOrConceptSymbolToTypeIdMap(this);
}

TypeSymbol* TypeSymbol::AddConst(const Span& span, const boost::uuids::uuid& moduleId)
{
    TypeDerivationRec typeDerivationRec;
    typeDerivationRec.derivations.push_back(Derivation::constDerivation);
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(this, typeDerivationRec, span, moduleId);
}

TypeSymbol* TypeSymbol::AddLvalueReference(const Span& span, const boost::uuids::uuid& moduleId)
{
    TypeDerivationRec typeDerivationRec;
    typeDerivationRec.derivations.push_back(Derivation::lvalueRefDerivation);
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(this, typeDerivationRec, span, moduleId);
}

TypeSymbol* TypeSymbol::AddRvalueReference(const Span& span, const boost::uuids::uuid& moduleId)
{
    TypeDerivationRec typeDerivationRec;
    typeDerivationRec.derivations.push_back(Derivation::rvalueRefDerivation);
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(this, typeDerivationRec, span, moduleId);
}

TypeSymbol* TypeSymbol::AddPointer(const Span& span, const boost::uuids::uuid& moduleId)
{
    TypeDerivationRec typeDerivationRec;
    typeDerivationRec.derivations.push_back(Derivation::pointerDerivation);
    return GetRootModuleForCurrentThread()->GetSymbolTable().MakeDerivedType(this, typeDerivationRec, span, moduleId);
}

void* TypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateUnspecifiedDIType(ToUtf8(Name()));
}

const TypeDerivationRec& TypeSymbol::DerivationRec() const
{
    static TypeDerivationRec emptyDerivationRec;
    return emptyDerivationRec;
}

TypeSymbol* TypeSymbol::RemoveDerivations(const TypeDerivationRec& sourceDerivationRec, const Span& span, const boost::uuids::uuid& moduleId)
{
    if (HasPointerDerivation(sourceDerivationRec.derivations)) return nullptr;
    return this;
}

bool TypeSymbol::IsRecursive(TypeSymbol* type, std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>>& tested) 
{ 
    if (tested.find(TypeId()) != tested.cend()) return TypesEqual(type, this);
    tested.insert(TypeId());
    return TypesEqual(type, this); 
}

ValueType TypeSymbol::GetValueType() const
{
    return ValueType::none;
}

std::u32string TypeSymbol::Id() const 
{ 
    return ToUtf32(boost::uuids::to_string(TypeId())); 
}

void* TypeSymbol::GetDIType(Emitter& emitter)
{
    void* localDiType = emitter.GetDITypeByTypeId(TypeId());
    if (!localDiType)
    {
        if (IsClassTypeSymbol())
        {
            ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(this);
            emitter.MapClassPtr(classTypeSymbol->TypeId(), classTypeSymbol, ToUtf8(classTypeSymbol->FullName()));
            localDiType = classTypeSymbol->CreateDIForwardDeclaration(emitter);
            emitter.MapFwdDeclaration(localDiType, classTypeSymbol->TypeId());
            emitter.SetDITypeByTypeId(classTypeSymbol->TypeId(), localDiType, ToUtf8(classTypeSymbol->FullName()));
        }
        localDiType = CreateDIType(emitter);
        emitter.SetDITypeByTypeId(TypeId(), localDiType, ToUtf8(FullName()));
    }
    return localDiType;
}

uint64_t TypeSymbol::SizeInBits(Emitter& emitter) 
{
    return emitter.GetSizeInBits(IrType(emitter));
}

uint32_t TypeSymbol::AlignmentInBits(Emitter& emitter)
{
    return emitter.GetAlignmentInBits(IrType(emitter));
}

void TypeSymbol::Check()
{
    ContainerSymbol::Check();
    if (typeId.is_nil())
    {
        throw SymbolCheckException("type symbol contains empty type id", GetSpan(), SourceModuleId());
    }
}

bool CompareTypesForEquality(const TypeSymbol* left, const TypeSymbol* right)
{
    if (left->GetSymbolType() == SymbolType::derivedTypeSymbol && right->GetSymbolType() == SymbolType::derivedTypeSymbol)
    {
        const DerivedTypeSymbol* derivedLeft = static_cast<const DerivedTypeSymbol*>(left);
        const DerivedTypeSymbol* derivedRight = static_cast<const DerivedTypeSymbol*>(right);
        if (TypesEqual(derivedLeft->BaseType(), derivedRight->BaseType()) && derivedLeft->DerivationRec() == derivedRight->DerivationRec())
        {
            return true;
        }
    }
    return false;
}

} } // namespace cmajor::symbols
