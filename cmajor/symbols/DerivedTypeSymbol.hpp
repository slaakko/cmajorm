// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_DERIVED_TYPE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_DERIVED_TYPE_SYMBOL_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>

namespace cmajor { namespace symbols {

enum class Derivation : uint8_t
{
    none = 0, constDerivation = 1, lvalueRefDerivation = 2, rvalueRefDerivation = 3, pointerDerivation = 4, max
};

SYMBOLS_API std::u32string DerivationStr(Derivation derivation);

struct SYMBOLS_API TypeDerivationRec
{
    std::vector<Derivation> derivations;
    bool IsEmpty() const { return derivations.empty(); }
};

inline bool operator==(const TypeDerivationRec& left, const TypeDerivationRec& right)
{
    return left.derivations == right.derivations;
}

inline bool operator!=(const TypeDerivationRec& left, const TypeDerivationRec& right)
{
    return !(left == right);
}

SYMBOLS_API std::u32string MakeDerivedTypeName(TypeSymbol* baseType, const TypeDerivationRec& derivationRec);

SYMBOLS_API bool HasFrontConstDerivation(const std::vector<Derivation>& derivations);
SYMBOLS_API bool HasReferenceDerivation(const std::vector<Derivation>& derivations);
SYMBOLS_API bool HasLvalueReferenceDerivation(const std::vector<Derivation>& derivations);
SYMBOLS_API bool HasRvalueReferenceDerivation(const std::vector<Derivation>& derivations);
SYMBOLS_API bool HasReferenceOrConstDerivation(const std::vector<Derivation>& derivations);
SYMBOLS_API bool HasPointerDerivation(const std::vector<Derivation>& derivations);
SYMBOLS_API int CountPointerDerivations(const std::vector<Derivation>& derivations);

SYMBOLS_API TypeDerivationRec MakePlainDerivationRec(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec RemoveReferenceDerivation(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec RemovePointerDerivation(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec RemoveConstDerivation(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec AddConstDerivation(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec AddLvalueReferenceDerivation(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec AddRvalueReferenceDerivation(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec AddPointerDerivation(const TypeDerivationRec& typeDerivationRec);
SYMBOLS_API TypeDerivationRec UnifyDerivations(const TypeDerivationRec& left, const TypeDerivationRec& right);

class SYMBOLS_API DerivedTypeSymbol : public TypeSymbol
{
public:
    DerivedTypeSymbol(const Span& span_, const std::u32string& name_);
    DerivedTypeSymbol(const Span& span_, const std::u32string& name_, TypeSymbol* baseType_, const TypeDerivationRec& derivationRec_);
    std::string TypeString() const override { return "derived_type"; }
    std::u32string SimpleName() const override;
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void ComputeTypeId();
    const TypeSymbol* BaseType() const override { return baseType; }
    TypeSymbol* BaseType() override { return baseType; }
    TypeSymbol* PlainType(const Span& span) override;
    TypeSymbol* PlainType(const Span& span, Module* module) override;
    TypeSymbol* RemoveReference(const Span& span) override;
    TypeSymbol* RemovePointer(const Span& span) override;
    TypeSymbol* RemoveConst(const Span& span) override;
    TypeSymbol* AddConst(const Span& span) override;
    TypeSymbol* AddLvalueReference(const Span& span) override;
    TypeSymbol* AddRvalueReference(const Span& span) override;
    TypeSymbol* AddPointer(const Span& span) override;
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    void* CreateDIType(Emitter& emitter) override;
    bool IsConstType() const override;
    bool IsReferenceType() const override;
    bool IsLvalueReferenceType() const override;
    bool IsRvalueReferenceType() const override;
    bool IsPointerType() const override;
    bool IsVoidPtrType() const override;
    bool IsCharacterPointerType() const override;
    int PointerCount() const override;
    bool ContainsTemplateParameter() const override { return baseType->ContainsTemplateParameter(); }
    const TypeDerivationRec& DerivationRec() const override { return derivationRec; }
    TypeSymbol* RemoveDerivations(const TypeDerivationRec& sourceDerivationRec, const Span& span) override;
    TypeSymbol* Unify(TypeSymbol* sourceType, const Span& span) override;
    bool IsRecursive(TypeSymbol* type, std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>>& tested) override;
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "DerivedTypeSymbol"; }
    void Check() override;
private:
    TypeSymbol* baseType;
    TypeDerivationRec derivationRec;
};

class SYMBOLS_API NullPtrType : public TypeSymbol
{
public:
    NullPtrType(const Span& span_, const std::u32string& name_);
    std::string TypeString() const override { return "nullptr_type"; }
    bool IsPointerType() const override { return true; }
    bool IsNullPtrType() const override { return true; }
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    ValueType GetValueType() const override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "NullPtrTypeSymbol"; }
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_DERIVED_TYPE_SYMBOL_INCLUDED
