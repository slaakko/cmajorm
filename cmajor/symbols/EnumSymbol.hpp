// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_ENUM_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_ENUM_SYMBOL_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Value.hpp>

namespace cmajor { namespace symbols {

class EnumConstantSymbol;

class EnumTypeSymbol : public TypeSymbol
{
public:
    EnumTypeSymbol(const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    std::string TypeString() const override { return "enumerated_type"; }
    std::string Syntax() const override;
    bool IsEnumeratedType() const override { return true; }
    bool IsParentSymbol() const override { return true; }
    void Accept(SymbolCollector* collector) override;
    void CollectMembers(SymbolCollector* collector);
    void Dump(CodeFormatter& formatter) override;
    void SetSpecifiers(Specifiers specifiers);
    const TypeSymbol* UnderlyingType() const { return underlyingType; }
    TypeSymbol* UnderlyingType() { return underlyingType; }
    void SetUnderlyingType(TypeSymbol* underlyingType_) { underlyingType = underlyingType_; }
    std::vector<EnumConstantSymbol*> GetEnumConstants();
    llvm::Type* IrType(Emitter& emitter) override { return underlyingType->IrType(emitter); }
    llvm::Constant* CreateDefaultIrValue(Emitter& emitter) override { return underlyingType->CreateDefaultIrValue(emitter); }
    llvm::DIType* CreateDIType(Emitter& emitter) override;
    bool IsSwitchConditionType() const override { return true; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    std::u32string Id() const override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "EnumTypeSymbol"; }
    void Check() override;
private:
    TypeSymbol* underlyingType;
};

class EnumConstantSymbol : public Symbol
{
public:
    EnumConstantSymbol(const Span& span_, const std::u32string& name_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    std::string TypeString() const override { return "enumeration_constant"; }
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    bool Evaluating() const { return evaluating; }
    void SetEvaluating() { evaluating = true; }
    void ResetEvaluating() { evaluating = false; }
    const TypeSymbol* GetType() const { return static_cast<const EnumTypeSymbol*>(Parent()); }
    TypeSymbol* GetType() { return static_cast<EnumTypeSymbol*>(Parent()); }
    void SetValue(Value* value_);
    const Value* GetValue() const { return value.get(); }
    Value* GetValue() { return value.get(); }
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "EnumConstantSymbol"; }
    void SetStrValue(const std::u32string& strValue_) { strValue = strValue_; }
    const std::u32string& StrValue() const { return strValue; }
private:
    std::unique_ptr<Value> value;
    bool evaluating;
    std::u32string strValue;
};

class EnumTypeDefaultConstructor : public FunctionSymbol
{
public:
    EnumTypeDefaultConstructor(const Span& span_, const std::u32string& name_);
    EnumTypeDefaultConstructor(EnumTypeSymbol* enumType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "EnumTypeDefaultConstructor"; }
    void Check() override;
private:
    FunctionSymbol* underlyingTypeDefaultConstructor;
};

class EnumTypeCopyConstructor : public FunctionSymbol
{
public:
    EnumTypeCopyConstructor(const Span& span_, const std::u32string& name_);
    EnumTypeCopyConstructor(EnumTypeSymbol* enumType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "EnumTypeCopyConstructor"; }
    void Check() override;
private:
    FunctionSymbol* underlyingTypeCopyConstructor;
};

class EnumTypeMoveConstructor : public FunctionSymbol
{
public:
    EnumTypeMoveConstructor(const Span& span_, const std::u32string& name_);
    EnumTypeMoveConstructor(EnumTypeSymbol* enumType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "EnumTypeMoveConstructor"; }
    void Check() override;
private:
    FunctionSymbol* underlyingTypeMoveConstructor;
};

class EnumTypeCopyAssignment : public FunctionSymbol
{
public:
    EnumTypeCopyAssignment(const Span& span_, const std::u32string& name_);
    EnumTypeCopyAssignment(EnumTypeSymbol* enumType_, TypeSymbol* voidType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "EnumTypeCopyAssignment"; }
    void Check() override;
private:
    FunctionSymbol* underlyingTypeCopyAssignment;
};

class EnumTypeMoveAssignment : public FunctionSymbol
{
public:
    EnumTypeMoveAssignment(const Span& span_, const std::u32string& name_);
    EnumTypeMoveAssignment(EnumTypeSymbol* enumType_, TypeSymbol* voidType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "EnumTypeMoveAssignment"; }
    void Check() override;
private:
    FunctionSymbol* underlyingTypeMoveAssignment;
};

class EnumTypeReturn : public FunctionSymbol
{
public:
    EnumTypeReturn(const Span& span_, const std::u32string& name_);
    EnumTypeReturn(EnumTypeSymbol* enumType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "EnumTypeReturn"; }
    void Check() override;
private:
    FunctionSymbol* underlyingTypeReturn;
};

class EnumTypeEqualityOp : public FunctionSymbol
{
public:
    EnumTypeEqualityOp(const Span& span_, const std::u32string& name_);
    EnumTypeEqualityOp(EnumTypeSymbol* enumType_, TypeSymbol* boolType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "EnumTypeEqualityOp"; }
    void Check() override;
private:
    FunctionSymbol* underlyingTypeEquality;
};

class EnumTypeToUnderlyingTypeConversion : public FunctionSymbol
{
public:
    EnumTypeToUnderlyingTypeConversion(const Span& span_, const std::u32string& name_);
    EnumTypeToUnderlyingTypeConversion(const Span& span_, const std::u32string& name_, TypeSymbol* sourceType_, TypeSymbol* targetType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 1; }
    TypeSymbol* ConversionSourceType() const override { return sourceType; }
    TypeSymbol* ConversionTargetType() const override { return targetType; }
    const char* ClassName() const override { return "EnumTypeToUnderlyingTypeConversion"; }
    void Check() override;
private:
    TypeSymbol* sourceType;
    TypeSymbol* targetType;
};

class UnderlyingTypeToEnumTypeConversion : public FunctionSymbol
{
public:
    UnderlyingTypeToEnumTypeConversion(const Span& span_, const std::u32string& name_);
    UnderlyingTypeToEnumTypeConversion(const Span& span_, const std::u32string& name_, TypeSymbol* sourceType_, TypeSymbol* targetType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    ConversionType GetConversionType() const override { return ConversionType::explicit_; }
    uint8_t ConversionDistance() const override { return 255; }
    TypeSymbol* ConversionSourceType() const override { return sourceType; }
    TypeSymbol* ConversionTargetType() const override { return targetType; }
    const char* ClassName() const override { return "UnderlyingTypeToEnumTypeConversion"; }
    void Check() override;
private:
    TypeSymbol* sourceType;
    TypeSymbol* targetType;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_ENUM_SYMBOL_INCLUDED
