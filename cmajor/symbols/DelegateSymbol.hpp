// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_DELEGATE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_DELEGATE_SYMBOL_INCLUDED
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace symbols {

class ParameterSymbol;

class SYMBOLS_API DelegateTypeSymbol : public TypeSymbol
{
public:
    DelegateTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void AddMember(Symbol* member) override;
    std::string TypeString() const override { return "delegate"; }
    std::string Syntax() const override;
    std::u32string Id() const override;
    bool IsDelegateType() const override { return true; }
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    //llvm::DIType* CreateDIType(Emitter& emitter) override; todo
    void SetSpecifiers(Specifiers specifiers);
    int Arity() const { return parameters.size(); }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    const TypeSymbol* ReturnType() const { return returnType; }
    TypeSymbol* ReturnType() { return returnType; }
    void SetReturnType(TypeSymbol* returnType_) { returnType = returnType_; }
    bool ReturnsClassInterfaceOrClassDelegateByValue() const;
    void SetReturnParam(ParameterSymbol* returnParam_);
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId);
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "DelegateTypeSymbol"; }
    void Check() override;
private:
    TypeSymbol* returnType;
    std::vector<ParameterSymbol*> parameters;
    std::unique_ptr<ParameterSymbol> returnParam;
};

class SYMBOLS_API DelegateTypeDefaultConstructor : public FunctionSymbol
{
public:
    DelegateTypeDefaultConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    DelegateTypeDefaultConstructor(DelegateTypeSymbol* delegateType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "DelegateTypeDefaultConstructor"; }
    void Check() override;
private:
    DelegateTypeSymbol* delegateType;
};

class SYMBOLS_API DelegateTypeCopyConstructor : public FunctionSymbol
{
public:
    DelegateTypeCopyConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    DelegateTypeCopyConstructor(DelegateTypeSymbol* delegateType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "DelegateTypeCopyConstructor"; }
};

class SYMBOLS_API DelegateTypeMoveConstructor : public FunctionSymbol
{
public:
    DelegateTypeMoveConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    DelegateTypeMoveConstructor(DelegateTypeSymbol* delegateType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "DelegateTypeMoveConstructor"; }
};

class SYMBOLS_API DelegateTypeCopyAssignment : public FunctionSymbol
{
public:
    DelegateTypeCopyAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    DelegateTypeCopyAssignment(DelegateTypeSymbol* delegateType, TypeSymbol* voidType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "DelegateTypeCopyAssignment"; }
};

class SYMBOLS_API DelegateTypeMoveAssignment : public FunctionSymbol
{
public:
    DelegateTypeMoveAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    DelegateTypeMoveAssignment(DelegateTypeSymbol* delegateType, TypeSymbol* voidType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "DelegateTypeMoveAssignment"; }
};

class SYMBOLS_API DelegateTypeReturn : public FunctionSymbol
{
public:
    DelegateTypeReturn(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    DelegateTypeReturn(DelegateTypeSymbol* delegateType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "DelegateTypeReturn"; }
};

class SYMBOLS_API DelegateTypeEquality : public FunctionSymbol
{
public:
    DelegateTypeEquality(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    DelegateTypeEquality(DelegateTypeSymbol* delegateType, TypeSymbol* boolType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "DelegateTypeEquality"; }
};

class SYMBOLS_API FunctionToDelegateConversion : public FunctionSymbol
{
public:
    FunctionToDelegateConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    FunctionToDelegateConversion(TypeSymbol* sourceType_, TypeSymbol* targetType_, FunctionSymbol* function_);
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 1; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "FunctionToDelegateConversion"; }
    void Check() override;
private:
    TypeSymbol* sourceType;
    TypeSymbol* targetType;
    FunctionSymbol* function;
};

class SYMBOLS_API DelegateToVoidPtrConversion : public FunctionSymbol
{
public:
    DelegateToVoidPtrConversion(TypeSymbol* delegateType_, TypeSymbol* voidPtrType_);
    ConversionType GetConversionType() const override { return ConversionType::explicit_; }
    uint8_t ConversionDistance() const override { return 255; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    const char* ClassName() const override { return "DelegateToVoidPtrConversion"; }
private:
    TypeSymbol* delegateType;
    TypeSymbol* voidPtrType;
};

class SYMBOLS_API VoidPtrToDelegateConversion : public FunctionSymbol
{
public:
    VoidPtrToDelegateConversion(TypeSymbol* voidPtrType_, TypeSymbol* delegateType_, TypeSymbol* ulongType_);
    ConversionType GetConversionType() const override { return ConversionType::explicit_; }
    uint8_t ConversionDistance() const override { return 255; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    const char* ClassName() const override { return "VoidPtrToDelegateConversion"; }
private:
    TypeSymbol* voidPtrType;
    TypeSymbol* delegateType;
    TypeSymbol* ulongType;
};

class SYMBOLS_API ClassDelegateTypeSymbol : public TypeSymbol
{
public:
    ClassDelegateTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void AddMember(Symbol* member) override;
    std::string TypeString() const override { return "class_delegate"; }
    std::string Syntax() const override;
    std::u32string Id() const override;
    bool IsClassDelegateType() const override { return true; }
    void Accept(SymbolCollector* collector) override;
    void Dump(CodeFormatter& formatter) override;
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    //llvm::DIType* CreateDIType(Emitter& emitter) override; todo
    void SetSpecifiers(Specifiers specifiers);
    int Arity() const { return parameters.size(); }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    const TypeSymbol* ReturnType() const { return returnType; }
    TypeSymbol* ReturnType() { return returnType; }
    void SetReturnType(TypeSymbol* returnType_) { returnType = returnType_; }
    bool ReturnsClassInterfaceOrClassDelegateByValue() const;
    void SetReturnParam(ParameterSymbol* returnParam_);
    DelegateTypeSymbol* DelegateType() { return delegateType; }
    ClassTypeSymbol* ObjectDelegatePairType() { Assert(objectDelegatePairType, "object delegate pair type not set");  return objectDelegatePairType; }
    FunctionSymbol* CopyConstructor() { Assert(copyConstructor, "class delegate copy constructor not set");  return copyConstructor; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId);
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "ClassDelegateTypeSymbol"; }
    void Check() override;
private:
    TypeSymbol* returnType;
    std::vector<ParameterSymbol*> parameters;
    std::unique_ptr<ParameterSymbol> returnParam;
    DelegateTypeSymbol* delegateType;
    ClassTypeSymbol* objectDelegatePairType;
    FunctionSymbol* copyConstructor;
};

class SYMBOLS_API ClassDelegateTypeDefaultConstructor : public FunctionSymbol
{
public:
    ClassDelegateTypeDefaultConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassDelegateTypeDefaultConstructor(ClassDelegateTypeSymbol* classDelegateType_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    const char* ClassName() const override { return "ClassDelegateTypeDefaultConstructor"; }
    void Check() override;
private:
    ClassDelegateTypeSymbol* classDelegateType;
};

class SYMBOLS_API ClassDelegateTypeCopyConstructor : public FunctionSymbol
{
public:
    ClassDelegateTypeCopyConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassDelegateTypeCopyConstructor(ClassDelegateTypeSymbol* classDelegateType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    bool IsClassDelegateCopyConstructor() const override { return true; }
    const char* ClassName() const override { return "ClassDelegateTypeCopyConstructor"; }
};

class SYMBOLS_API ClassDelegateTypeMoveConstructor : public FunctionSymbol
{
public:
    ClassDelegateTypeMoveConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassDelegateTypeMoveConstructor(ClassDelegateTypeSymbol* classDelegateType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    const char* ClassName() const override { return "ClassDelegateTypeMoveConstructor"; }
};

class SYMBOLS_API ClassDelegateTypeCopyAssignment : public FunctionSymbol
{
public:
    ClassDelegateTypeCopyAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassDelegateTypeCopyAssignment(ClassDelegateTypeSymbol* classDelegateType, TypeSymbol* voidType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    const char* ClassName() const override { return "ClassDelegateTypeCopyAssignment"; }
};

class SYMBOLS_API ClassDelegateTypeMoveAssignment : public FunctionSymbol
{
public:
    ClassDelegateTypeMoveAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassDelegateTypeMoveAssignment(ClassDelegateTypeSymbol* classDelegateType, TypeSymbol* voidType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    const char* ClassName() const override { return "ClassDelegateTypeMoveAssignment"; }
};

class SYMBOLS_API ClassDelegateTypeEquality : public FunctionSymbol
{
public:
    ClassDelegateTypeEquality(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassDelegateTypeEquality(ClassDelegateTypeSymbol* classDelegateType, TypeSymbol* boolType);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    const char* ClassName() const override { return "ClassDelegateTypeEquality"; }
};

class SYMBOLS_API MemberFunctionToClassDelegateConversion : public FunctionSymbol
{
public:
    MemberFunctionToClassDelegateConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    MemberFunctionToClassDelegateConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, TypeSymbol* sourceType_, ClassDelegateTypeSymbol* targetType_, FunctionSymbol* function_);
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 1; }
    bool IsMemberFunctionToClassDelegateConversion() const override { return true; }
    std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction) override;
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "MemberFunctionToClassDelegateConversion"; }
    void Check() override;
private:
    TypeSymbol* sourceType;
    ClassDelegateTypeSymbol* targetType;
    FunctionSymbol* function;
    //LocalVariableSymbol* objectDelegatePairVariable;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_DELEGATE_SYMBOL_INCLUDED
