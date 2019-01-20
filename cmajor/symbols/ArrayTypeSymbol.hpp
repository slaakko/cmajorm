// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_ARRAY_TYPE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_ARRAY_TYPE_SYMBOL_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace symbols {

class ArrayTypeSymbol : public TypeSymbol
{
public:
    ArrayTypeSymbol(const Span& span_, const std::u32string& name_);
    ArrayTypeSymbol(const Span& span_, const std::u32string& name_, TypeSymbol* elementType_, int64_t size_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    llvm::Type* IrType(Emitter& emitter) override;
    llvm::Constant* CreateDefaultIrValue(Emitter& emitter) override;
    llvm::DIType* CreateDIType(Emitter& emitter) override;
    bool IsArrayType() const override { return true; }
    TypeSymbol* ElementType() const { return elementType; }
    int64_t Size() const { return size; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    const char* ClassName() const override { return "ArrayTypeSymbol"; }
    void Check() override;
private:
    TypeSymbol* elementType;
    int64_t size;
};

class ArrayLengthFunction : public FunctionSymbol
{
public:
    ArrayLengthFunction(const Span& span_, const std::u32string& name_);
    ArrayLengthFunction(ArrayTypeSymbol* arrayType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    std::unique_ptr<Value> ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span) const override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsCompileTimePrimitiveFunction() const override { return true; }
    const char* ClassName() const override { return "ArrayLengthFunction"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
};

class ArrayBeginFunction : public FunctionSymbol
{
public:
    ArrayBeginFunction(const Span& span_, const std::u32string& name_);
    ArrayBeginFunction(ArrayTypeSymbol* arrayType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayBeginFunction"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
};

class ArrayEndFunction : public FunctionSymbol
{
public:
    ArrayEndFunction(const Span& span_, const std::u32string& name_);
    ArrayEndFunction(ArrayTypeSymbol* arrayType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayEndFunction"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
};

class ArrayCBeginFunction : public FunctionSymbol
{
public:
    ArrayCBeginFunction(const Span& span_, const std::u32string& name_);
    ArrayCBeginFunction(ArrayTypeSymbol* arrayType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayCBeginFunction"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
};

class ArrayCEndFunction : public FunctionSymbol
{
public:
    ArrayCEndFunction(const Span& span_, const std::u32string& name_);
    ArrayCEndFunction(ArrayTypeSymbol* arrayType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayCEndFunction"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
};

class ArrayTypeDefaultConstructor : public FunctionSymbol
{
public:
    ArrayTypeDefaultConstructor(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeDefaultConstructor_, const Span& span_);
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flag, const Span& spans) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayTypeDefaultConstructor"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
    FunctionSymbol* elementTypeDefaultConstructor;
};

class ArrayTypeCopyConstructor : public FunctionSymbol
{
public:
    ArrayTypeCopyConstructor(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeCopyConstructor_, const Span& span_);
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayTypeCopyConstructor"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
    FunctionSymbol* elementTypeCopyConstructor;
};

class ArrayTypeMoveConstructor : public FunctionSymbol
{
public:
    ArrayTypeMoveConstructor(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeMoveConstructor_, const Span& span_);
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayTypeMoveConstructor"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
    FunctionSymbol* elementTypeMoveConstructor;
};

class ArrayTypeCopyAssignment : public FunctionSymbol
{
public:
    ArrayTypeCopyAssignment(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeCopyAssignment_, const Span& span_);
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayTypeCopyAssignment"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
    FunctionSymbol* elementTypeCopyAssignment;
};

class ArrayTypeMoveAssignment : public FunctionSymbol
{
public:
    ArrayTypeMoveAssignment(ArrayTypeSymbol* arrayType_, FunctionSymbol* elementTypeMoveAssignment_, const Span& span_);
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "ArrayTypeMoveAssignment"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
    FunctionSymbol* elementTypeMoveAssignment;
};

class ArrayTypeElementAccess : public FunctionSymbol
{
public:
    ArrayTypeElementAccess(ArrayTypeSymbol* arrayType_, const Span& span_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsArrayElementAccess() const override { return true; }
    const char* ClassName() const override { return "ArrayTypeElementAccess"; }
    void Check() override;
private:
    ArrayTypeSymbol* arrayType;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_ARRAY_TYPE_SYMBOL_INCLUDED
