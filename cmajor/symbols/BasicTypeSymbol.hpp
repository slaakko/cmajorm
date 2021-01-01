// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_BASIC_TYPE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_BASIC_TYPE_SYMBOL_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>

namespace cmajor { namespace symbols {

class SYMBOLS_API BasicTypeSymbol : public TypeSymbol
{
public:
    BasicTypeSymbol(SymbolType symbolType_, const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "basic_type"; }
    bool IsBasicTypeSymbol() const override { return true; }
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    FunctionSymbol* DefaultConstructor() { return defaultConstructor; }
    void SetDefaultConstructor(FunctionSymbol* defaultConstructor_) { defaultConstructor = defaultConstructor_; }
    FunctionSymbol* CopyConstructor() { return copyConstructor; }
    void SetCopyConstructor(FunctionSymbol* copyConstructor_) { copyConstructor = copyConstructor_; }
    FunctionSymbol* MoveConstructor() { return moveConstructor; }
    void SetMoveConstructor(FunctionSymbol* moveConstructor_) { moveConstructor = moveConstructor_; }
    FunctionSymbol* CopyAssignment() { return copyAssignment; }
    void SetCopyAssignment(FunctionSymbol* copyAssignment_) { copyAssignment = copyAssignment_; }
    FunctionSymbol* MoveAssignment() { return moveAssignment; }
    void SetMoveAssignment(FunctionSymbol* moveAssignment_) { moveAssignment = moveAssignment_; }
    FunctionSymbol* ReturnFun() { return returnFun; }
    void SetReturnFun(FunctionSymbol* returnFun_) { returnFun = returnFun_; }
    FunctionSymbol* EqualityOp() { return equalityOp; }
    void SetEqualityOp(FunctionSymbol* equalityOp_) { equalityOp = equalityOp_; }
    const char* ClassName() const override { return "BasicTypeSymbol"; }
    void Check() override;
private:
    FunctionSymbol* defaultConstructor;
    FunctionSymbol* copyConstructor;
    FunctionSymbol* moveConstructor;
    FunctionSymbol* copyAssignment;
    FunctionSymbol* moveAssignment;
    FunctionSymbol* returnFun;
    FunctionSymbol* equalityOp;
};

class SYMBOLS_API BoolTypeSymbol : public BasicTypeSymbol
{
public:
    BoolTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "bool"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForBool(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForBool(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "BoolTypeSymbol"; }
};

class SYMBOLS_API SByteTypeSymbol : public BasicTypeSymbol
{
public:
    SByteTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "sbyte"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForSByte(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForSByte(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "SByteTypeSymbol"; }
};

class SYMBOLS_API ByteTypeSymbol : public BasicTypeSymbol
{
public:
    ByteTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "byte"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForByte(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForByte(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsUnsignedType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "ByteTypeSymbol"; }
};

class SYMBOLS_API ShortTypeSymbol : public BasicTypeSymbol
{
public:
    ShortTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "short"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForShort(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForShort(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "ShortTypeSymbol"; }
};

class SYMBOLS_API UShortTypeSymbol : public BasicTypeSymbol
{
public:
    UShortTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "ushort"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForUShort(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForUShort(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsUnsignedType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "UShortTypeSymbol"; }
};

class SYMBOLS_API IntTypeSymbol : public BasicTypeSymbol
{
public:
    IntTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "int"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForInt(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForInt(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "IntTypeSymbol"; }
};

class SYMBOLS_API UIntTypeSymbol : public BasicTypeSymbol
{
public:
    UIntTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "uint"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForUInt(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForUInt(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsUnsignedType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "UIntTypeSymbol"; }
};

class SYMBOLS_API LongTypeSymbol : public BasicTypeSymbol
{
public:
    LongTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "long"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForLong(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForLong(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "LongTypeSymbol"; }
};

class SYMBOLS_API ULongTypeSymbol : public BasicTypeSymbol
{
public:
    ULongTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "ulong"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForULong(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForULong(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsIntegralType() const override { return true; }
    bool IsUnsignedType() const override { return true; }
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "ULongTypeSymbol"; }
};

class SYMBOLS_API FloatTypeSymbol : public BasicTypeSymbol
{
public:
    FloatTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "float"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForFloat(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForFloat(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsFloatingPointType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "FloatTypeSymbol"; }
};

class SYMBOLS_API DoubleTypeSymbol : public BasicTypeSymbol
{
public:
    DoubleTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "double"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForDouble(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForDouble(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsFloatingPointType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "DoubleTypeSymbol"; }
};

class SYMBOLS_API CharTypeSymbol : public BasicTypeSymbol
{
public:
    CharTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "char"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForChar(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForChar(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsSwitchConditionType() const override { return true; }
    bool IsUnsignedType() const override { return true; }
    bool IsCharacterType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "CharTypeSymbol"; }
};

class SYMBOLS_API WCharTypeSymbol : public BasicTypeSymbol
{
public:
    WCharTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "wchar"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForWChar(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForWChar(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsSwitchConditionType() const override { return true; }
    bool IsUnsignedType() const override { return true; }
    bool IsCharacterType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "WCharTypeSymbol"; }
};

class SYMBOLS_API UCharTypeSymbol : public BasicTypeSymbol
{
public:
    UCharTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "uchar"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForUChar(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { return emitter.CreateDefaultIrValueForUChar(); }
    void* CreateDIType(Emitter& emitter) override;
    bool IsSwitchConditionType() const override { return true; }
    bool IsUnsignedType() const override { return true; }
    bool IsCharacterType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "UCharTypeSymbol"; }
};

class SYMBOLS_API VoidTypeSymbol : public BasicTypeSymbol
{
public:
    VoidTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "void"; }
    void* IrType(Emitter& emitter) override { return emitter.GetIrTypeForVoid(); }
    void* CreateDefaultIrValue(Emitter& emitter) override { Assert(false, "tried to create default value for void type"); return nullptr; }
    void* CreateDIType(Emitter& emitter) override;
    bool IsVoidType() const override { return true; }
    const char* ClassName() const override { return "VoidTypeSymbol"; }
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_BASIC_TYPE_SYMBOL_INCLUDED
