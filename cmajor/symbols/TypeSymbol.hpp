// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_TYPE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_TYPE_SYMBOL_INCLUDED
#include <cmajor/symbols/ContainerSymbol.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::ir;

struct TypeDerivationRec;
enum class ValueType : uint8_t;
class Value;
class TemplateParameterSymbol;
struct ConversionTableEntry;

class SYMBOLS_API TypeSymbol : public ContainerSymbol
{
public:
    TypeSymbol(SymbolType symbolType_, const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    bool IsTypeSymbol() const override { return true; }
    virtual bool IsInComplete() const { return false; }
    virtual bool IsIntegralType() const { return false; }
    virtual bool IsFloatingPointType() const { return false; }
    virtual bool IsUnsignedType() const { return false; }
    virtual bool IsVoidType() const { return false; }
    std::string TypeString() const override { return "type"; }
    virtual bool IsBasicTypeSymbol() const { return false; }
    virtual bool IsDelegateType() const { return false; }
    virtual bool IsClassDelegateType() const { return false; }
    virtual bool IsEnumeratedType() const { return false; }
    virtual bool IsCharacterType() const { return false; }
    virtual bool IsCharacterPointerType() const { return false; }
    virtual const TypeSymbol* BaseType() const { return this; }
    virtual TypeSymbol* BaseType() { return this; }
    virtual TypeSymbol* PlainType(const Span& span, const boost::uuids::uuid& moduleId) { return this; }
    virtual TypeSymbol* RemoveConst(const Span& span, const boost::uuids::uuid& moduleId) { return this; }
    virtual TypeSymbol* RemoveReference(const Span& span, const boost::uuids::uuid& moduleId) { return this; }
    virtual TypeSymbol* RemovePointer(const Span& span, const boost::uuids::uuid& moduleId) { return this; }
    virtual TypeSymbol* AddConst(const Span& span, const boost::uuids::uuid& moduleId);
    virtual TypeSymbol* AddLvalueReference(const Span& span, const boost::uuids::uuid& moduleId);
    virtual TypeSymbol* AddRvalueReference(const Span& span, const boost::uuids::uuid& moduleId);
    virtual TypeSymbol* AddPointer(const Span& span, const boost::uuids::uuid& moduleId);
    virtual void* IrType(Emitter& emitter) = 0;
    virtual void* CreateDefaultIrValue(Emitter& emitter) = 0;
    virtual void* CreateDIType(Emitter& emitter);
    virtual bool IsConstType() const { return false; }
    virtual bool IsReferenceType() const { return false; }
    virtual bool IsLvalueReferenceType() const { return false; }
    virtual bool IsRvalueReferenceType() const { return false; }
    virtual bool IsArrayType() const { return false; }
    virtual bool IsStringFunctionContainer() const { return false; }
    virtual bool IsPointerType() const { return false; }
    virtual bool IsNullPtrType() const { return false; }
    virtual bool IsVoidPtrType() const { return false; }
    virtual bool IsPolymorphicType() const { return false; }
    virtual bool IsSwitchConditionType() const { return false; }
    virtual int PointerCount() const { return 0; }
    virtual bool HasNontrivialDestructor() const { return false; }
    virtual bool ContainsTemplateParameter() const { return false; }
    virtual bool CompletelyBound() const { return IsBound(); }
    void SetTypeId(const boost::uuids::uuid& typeId_) { typeId = typeId_; }
    const boost::uuids::uuid& TypeId() const { Assert(!typeId.is_nil(), "type id not initialized");  return typeId; }
    bool TypeIdNotSet() const { return typeId.is_nil(); }
    virtual const TypeDerivationRec& DerivationRec() const;
    virtual TypeSymbol* RemoveDerivations(const TypeDerivationRec& sourceDerivationRec, const Span& span, const boost::uuids::uuid& moduleId);
    virtual TypeSymbol* Unify(TypeSymbol* that, const Span& span, const boost::uuids::uuid& moduleId) { return nullptr; }
    virtual TypeSymbol* UnifyTemplateArgumentType(SymbolTable& symbolTable, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMap, const Span& span, const boost::uuids::uuid& moduleId) { return nullptr; }
    virtual bool IsRecursive(TypeSymbol* type, std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>>& tested);
    virtual ValueType GetValueType() const;
    virtual Value* MakeValue() const { return nullptr; }
    std::u32string Id() const override;
    void* GetDIType(Emitter& emitter);
    uint64_t SizeInBits(Emitter& emitter);
    uint32_t AlignmentInBits(Emitter& emitter);
    const char* ClassName() const override { return "TypeSymbol"; }
    void Check() override;
private:
    boost::uuids::uuid typeId;
};

SYMBOLS_API bool CompareTypesForEquality(const TypeSymbol* left, const TypeSymbol* right);

inline bool TypesEqual(const TypeSymbol* left, const TypeSymbol* right)
{
    if (left->TypeId() == right->TypeId()) return true;
    return CompareTypesForEquality(left, right);
}

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_TYPE_SYMBOL_INCLUDED
