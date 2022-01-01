// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_INTERFACE_TYPE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_INTERFACE_TYPE_SYMBOL_INCLUDED
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>

namespace cmajor { namespace symbols {

class InterfaceTypeCopyConstructor;

class SYMBOLS_API InterfaceTypeSymbol : public TypeSymbol
{
public:
    InterfaceTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    std::string TypeString() const override { return "interface"; }
    void AddMember(Symbol* member) override;
    void Accept(SymbolCollector* collector) override;
    void SetSpecifiers(Specifiers specifiers);
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    const std::vector<MemberFunctionSymbol*>& MemberFunctions() const { return memberFunctions; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, MemberFunctionSymbol* interfaceMemberFunction, const Span& span, const boost::uuids::uuid& moduleId);
    void SetCopyConstructor(InterfaceTypeCopyConstructor* copyConstructor_) { copyConstructor = copyConstructor_; }
    InterfaceTypeCopyConstructor* CopyConstructor() { return copyConstructor; }
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "InterfaceTypeSymbol"; }
    void Check() override;
    std::string GetSymbolCategoryStr() const override { return "IN"; }
    std::string GetSymbolCategoryDescription() const override { return "interface"; }
private:
    std::vector<MemberFunctionSymbol*> memberFunctions;
    InterfaceTypeCopyConstructor* copyConstructor;
};

class SYMBOLS_API InterfaceTypeDefaultConstructor : public FunctionSymbol
{
public:
    InterfaceTypeDefaultConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    InterfaceTypeDefaultConstructor(InterfaceTypeSymbol* interfaceType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeDefaultConstructor"; }
private:
    InterfaceTypeSymbol* interfaceType;
};

class SYMBOLS_API InterfaceTypeCopyConstructor : public FunctionSymbol
{
public:
    InterfaceTypeCopyConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    InterfaceTypeCopyConstructor(InterfaceTypeSymbol* interfaceType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeCopyConstructor"; }
private:
    InterfaceTypeSymbol* interfaceType;
};

class SYMBOLS_API InterfaceTypeMoveConstructor : public FunctionSymbol
{
public:
    InterfaceTypeMoveConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    InterfaceTypeMoveConstructor(InterfaceTypeSymbol* interfaceType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeMoveConstructor"; }
private:
    InterfaceTypeSymbol* interfaceType;
};

class SYMBOLS_API InterfaceTypeCopyAssignment : public FunctionSymbol
{
public:
    InterfaceTypeCopyAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    InterfaceTypeCopyAssignment(InterfaceTypeSymbol* interfaceType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeCopyAssignment"; }
private:
    InterfaceTypeSymbol* interfaceType;
};

class SYMBOLS_API InterfaceTypeMoveAssignment : public FunctionSymbol
{
public:
    InterfaceTypeMoveAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    InterfaceTypeMoveAssignment(InterfaceTypeSymbol* interfaceType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "InterfaceTypeMoveAssignment"; }
private:
    InterfaceTypeSymbol* interfaceType;
};

class SYMBOLS_API ClassToInterfaceConversion : public FunctionSymbol
{
public:
    ClassToInterfaceConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassToInterfaceConversion(ClassTypeSymbol* sourceClassType_, InterfaceTypeSymbol* targetInterfaceType_, int32_t interfaceIndex_, const Span& span_, const boost::uuids::uuid& sourceModuleId_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 1; }
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsClassToInterfaceTypeConversion() const override {return true; }
    const char* ClassName() const override { return "ClassToInterfaceConversion"; }
    void Check() override;
private:
    ClassTypeSymbol* sourceClassType;
    InterfaceTypeSymbol* targetInterfaceType;
    int32_t interfaceIndex;
};

class SYMBOLS_API GetObjectPtrFromInterface : public FunctionSymbol
{
public:
    GetObjectPtrFromInterface(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_); 
    GetObjectPtrFromInterface(InterfaceTypeSymbol* interfaceTypeSymbol);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "GetObjectPtrFromInterface"; }
private:
    InterfaceTypeSymbol* interfaceType;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_INTERFACE_TYPE_SYMBOL_INCLUDED
