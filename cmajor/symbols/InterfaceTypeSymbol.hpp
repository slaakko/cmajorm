// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_INTERFACE_TYPE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_INTERFACE_TYPE_SYMBOL_INCLUDED
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>

namespace cmajor { namespace symbols {

class MemberFunctionSymbol;
class InterfaceTypeCopyConstructor;

class SYMBOLS_API InterfaceTypeSymbol : public TypeSymbol
{
public:
    InterfaceTypeSymbol(const Span& span_, const std::u32string& name_);
    std::string TypeString() const override { return "interface"; }
    void AddMember(Symbol* member) override;
    void Accept(SymbolCollector* collector) override;
    void SetSpecifiers(Specifiers specifiers);
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    const std::vector<MemberFunctionSymbol*>& MemberFunctions() const { return memberFunctions; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, MemberFunctionSymbol* interfaceMemberFunction, const Span& span);
    void SetCopyConstructor(InterfaceTypeCopyConstructor* copyConstructor_) { copyConstructor = copyConstructor_; }
    InterfaceTypeCopyConstructor* CopyConstructor() { return copyConstructor; }
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "InterfaceTypeSymbol"; }
    void Check() override;
private:
    std::vector<MemberFunctionSymbol*> memberFunctions;
    InterfaceTypeCopyConstructor* copyConstructor;
};

class SYMBOLS_API InterfaceTypeDefaultConstructor : public FunctionSymbol
{
public:
    InterfaceTypeDefaultConstructor(InterfaceTypeSymbol* interfaceType_, const Span& span_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeDefaultConstructor"; }
};

class SYMBOLS_API InterfaceTypeCopyConstructor : public FunctionSymbol
{
public:
    InterfaceTypeCopyConstructor(InterfaceTypeSymbol* interfaceType_, const Span& span_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeCopyConstructor"; }
};

class SYMBOLS_API InterfaceTypeMoveConstructor : public FunctionSymbol
{
public:
    InterfaceTypeMoveConstructor(InterfaceTypeSymbol* interfaceType_, const Span& span_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeMoveConstructor"; }
};

class SYMBOLS_API InterfaceTypeCopyAssignment : public FunctionSymbol
{
public:
    InterfaceTypeCopyAssignment(InterfaceTypeSymbol* interfaceType_, const Span& span_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeCopyAssignment"; }
};

class SYMBOLS_API InterfaceTypeMoveAssignment : public FunctionSymbol
{
public:
    InterfaceTypeMoveAssignment(InterfaceTypeSymbol* interfaceType_, const Span& span_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeMoveAssignment"; }
};

class SYMBOLS_API ClassToInterfaceConversion : public FunctionSymbol
{
public:
    ClassToInterfaceConversion(ClassTypeSymbol* sourceClassType_, InterfaceTypeSymbol* targetInterfaceType_, int32_t interfaceIndex_, const Span& span_);
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 1; }
    TypeSymbol* ConversionSourceType() const override { return sourceClassType->AddLvalueReference(Span()); }
    TypeSymbol* ConversionTargetType() const override { return targetInterfaceType; }
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsClassToInterfaceTypeConversion() const override {return true; }
    const char* ClassName() const override { return "ClassToInterfaceConversion"; }
    void Check() override;
private:
    ClassTypeSymbol* sourceClassType;
    InterfaceTypeSymbol* targetInterfaceType;
    //LocalVariableSymbol* temporaryInterfaceObjectVar;
    int32_t interfaceIndex;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_INTERFACE_TYPE_SYMBOL_INCLUDED
