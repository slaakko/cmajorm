// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_SYMBOL_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <cmajor/symbols/TypeMap.hpp>
#include <sngcm/ast/Attribute.hpp>
#include <sngcm/ast/Specifier.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <unordered_set>
#include <stdint.h>

namespace cmajor { namespace symbols {

using soulng::lexer::Span;
using namespace sngcm::ast;
using namespace soulng::util;
using namespace cmajor::ir;

class SymbolWriter;
class SymbolReader;
class ContainerScope;
class ContainerSymbol;
class TypeSymbol;
class ConceptSymbol;
class ClassTypeSymbol;
class InterfaceTypeSymbol;
class NamespaceSymbol;
class FunctionSymbol;
class SymbolTable;
class Module;
class SymbolCollector;

enum class SymbolType : uint8_t
{
    boolTypeSymbol, sbyteTypeSymbol, byteTypeSymbol, shortTypeSymbol, ushortTypeSymbol, intTypeSymbol, uintTypeSymbol, longTypeSymbol, ulongTypeSymbol, floatTypeSymbol, doubleTypeSymbol, 
    charTypeSymbol, wcharTypeSymbol, ucharTypeSymbol, voidTypeSymbol, nullPtrTypeSymbol,
    arrayTypeSymbol, derivedTypeSymbol,
    namespaceSymbol, functionSymbol, staticConstructorSymbol, constructorSymbol, destructorSymbol, memberFunctionSymbol, conversionFunctionSymbol, functionGroupSymbol,
    classGroupTypeSymbol, classTypeSymbol, classTemplateSpecializationSymbol, interfaceTypeSymbol, conceptGroupSymbol, conceptSymbol,
    delegateTypeSymbol, classDelegateTypeSymbol, declarationBlock, typedefSymbol, constantSymbol, enumTypeSymbol, enumConstantSymbol,
    templateParameterSymbol, boundTemplateParameterSymbol, parameterSymbol, localVariableSymbol, memberVariableSymbol,
    basicTypeUnaryPlus, basicTypeIntUnaryMinus, basicTypeFloatUnaryMinus, basicTypeComplement, basicTypeAdd, basicTypeFAdd, basicTypeSub, basicTypeFSub, basicTypeMul, basicTypeFMul, 
    basicTypeSDiv, basicTypeUDiv, basicTypeFDiv, basicTypeSRem, basicTypeURem, basicTypeAnd, basicTypeOr, basicTypeXor, basicTypeShl, basicTypeAShr, basicTypeLShr,
    basicTypeNot, basicTypeIntegerEquality, basicTypeUnsignedIntegerLessThan, basicTypeSignedIntegerLessThan, basicTypeFloatingEquality, basicTypeFloatingLessThan,
    defaultInt1, defaultSInt8, defaultUInt8, defaultSInt16, defaultUInt16, defaultSInt32, defaultUInt32, defaultSInt64, defaultUInt64, 
    defaultChar, defaultWChar, defaultUChar, defaultFloat, defaultDouble, basicTypeCopyCtor, basicTypeMoveCtor, basicTypeCopyAssignment, basicTypeMoveAssignment,
    basicTypeReturn,
    basicTypeImplicitSignExtension, basicTypeImplicitZeroExtension, basicTypeExplicitSignExtension, basicTypeExplicitZeroExtension, basicTypeTruncation, basicTypeBitCast,
    basicTypeImplicitUnsignedIntToFloating, basicTypeImplicitSignedIntToFloating, basicTypeExplicitUnsignedIntToFloating, basicTypeExplicitSignedIntToFloating, 
    basicTypeFloatingToUnsignedInt, basicTypeFloatingToSignedInt, basicTypeFloatingExtension, basicTypeFloatingTruncation, trap,
    enumTypeDefaultConstructor, enumTypeCopyConstructor, enumTypeMoveConstructor, enumTypeCopyAssignment, enumTypeMoveAssignment, enumTypeReturn, enumTypeEquality, 
    enumTypeToUnderlyingType, underlyingToEnumType, 
    delegateTypeDefaultConstructor, delegateTypeCopyConstructor, delegateTypeMoveConstructor, delegateTypeCopyAssignment, delegateTypeMoveAssignment, delegateTypeReturn, delegateTypeEquality,
    functionToDelegateSymbol,
    classDelegateTypeDefaultConstructor, classDelegateTypeCopyConstructor, classDelegateTypeMoveConstructor, classDelegateTypeCopyAssignment, classDelegateTypeMoveAssignment, 
    classDelegateTypeEquality, memberFunctionToClassDelegateSymbol, 
    arrayLengthFunctionSymbol, arrayBeginFunctionSymbol, arrayEndFunctionSymbol, arrayCBeginFunctionSymbol, arrayCEndFunctionSymbol,
    namespaceTypeSymbol, functionGroupTypeSymbol, memberExpressionTypeSymbol, variableValueSymbol, globalVariableSymbol, globalVariableGroupSymbol,
    stringFunctionContainerSymbol, stringLengthFunctionSymbol,
    maxSymbol
};

std::string SymbolTypeStr(SymbolType symbolType);

enum class SymbolAccess : uint8_t
{
    private_ = 0, protected_ = 1, internal_ = 2, public_ = 3
};

SYMBOLS_API uint32_t AccessFlag(Emitter& emitter, SymbolAccess access);

enum class SymbolFlags : uint8_t
{
    none = 0, 
    access = 1 << 0 | 1 << 1,
    static_ = 1 << 2,
    external = 1 << 3,
    nothrow_ = 1 << 4,
    project = 1 << 5,
    bound = 1 << 6,
    inConversionTable = 1 << 7
};

inline SymbolFlags operator&(SymbolFlags left, SymbolFlags right)
{
    return SymbolFlags(uint8_t(left) & uint8_t(right));
}

inline SymbolFlags operator|(SymbolFlags left, SymbolFlags right)
{
    return SymbolFlags(uint8_t(left) | uint8_t(right));
}

inline SymbolFlags operator~(SymbolFlags flags)
{
    return SymbolFlags(~uint8_t(flags));
}

std::string SymbolFlagStr(SymbolFlags symbolFlags);
std::string SymbolFlagStr(SymbolFlags symbolFlags, bool noAccess);

class SYMBOLS_API Symbol
{
public:
    Symbol(SymbolType symbolType_, const Span& span_, const std::u32string& name_);
    virtual ~Symbol();
    virtual void Write(SymbolWriter& writer);
    virtual void Read(SymbolReader& reader);
    virtual void EmplaceType(TypeSymbol* typeSymbol, int index) {}
    virtual void EmplaceConcept(ConceptSymbol* conceptSymbol) {}
    virtual void EmplaceFunction(FunctionSymbol* functionSymbol, int index) {}
    virtual bool IsExportSymbol() const { return IsProject(); }
    virtual bool IsContainerSymbol() const { return false; }
    virtual bool IsFunctionSymbol() const { return false; }
    virtual bool IsTypeSymbol() const { return false; }
    virtual bool IsClassTypeSymbol() const { return false; }
    virtual bool IsParentSymbol() const { return false; }
    virtual bool IsVariableSymbol() const { return false; }
    virtual bool IsGlobalVariableSymbol() const { return false; }
    virtual void Accept(SymbolCollector* collector) {}
    virtual const ContainerScope* GetContainerScope() const;
    virtual ContainerScope* GetContainerScope();
    virtual std::u32string FullName() const;
    virtual std::u32string FullNameWithSpecifiers() const;
    virtual std::u32string SimpleName() const { return Name(); }
    virtual std::u32string DocName() const { return Name(); }
    virtual std::u32string CodeName() const { return Name(); }
    virtual std::u32string Id() const { return mangledName; }
    virtual SymbolAccess DeclaredAccess() const { return Access(); }
    virtual std::string TypeString() const { return "symbol";  }
    virtual void* IrObject(Emitter& emitter);
    virtual void ComputeMangledName();
    virtual void Dump(CodeFormatter& formatter) {}
    virtual std::string GetSpecifierStr() const;
    virtual std::string Syntax() const;
    virtual void Check();
    void SetMangledName(const std::u32string& mangledName_);
    SymbolAccess Access() const { return SymbolAccess(flags & SymbolFlags::access);  }
    void SetAccess(SymbolAccess access_) { flags = flags | SymbolFlags(access_); }
    void SetAccess(Specifiers accessSpecifiers);
    bool IsSameParentOrAncestorOf(const Symbol* that) const;
    SymbolType GetSymbolType() const { return symbolType; }
    const Span& GetSpan() const { return span; }
    void SetSpan(const Span& span_) { span = span_; }
    const std::u32string& Name() const { return name; }
    void SetName(const std::u32string& name_) { name = name_; }
    SymbolFlags GetSymbolFlags() const { return flags; }
    bool IsStatic() const { return GetFlag(SymbolFlags::static_); }
    void SetStatic() { SetFlag(SymbolFlags::static_); }
    bool IsExternal() const { return GetFlag(SymbolFlags::external); }
    void SetExternal() { SetFlag(SymbolFlags::external); }
    bool IsNothrow() const { return GetFlag(SymbolFlags::nothrow_); }
    void SetNothrow() { SetFlag(SymbolFlags::nothrow_); }
    bool IsProject() const { return GetFlag(SymbolFlags::project); }
    void SetProject() { SetFlag(SymbolFlags::project); }
    bool IsBound() const { return GetFlag(SymbolFlags::bound); }
    void SetBound() { SetFlag(SymbolFlags::bound); }
    bool GetFlag(SymbolFlags flag) const { return (flags & flag) != SymbolFlags::none; }
    void SetFlag(SymbolFlags flag) { flags = flags | flag; }
    void ResetFlag(SymbolFlags flag) { flags = flags & ~flag; }
    const Symbol* Parent() const { return parent; }
    Symbol* Parent() { return parent; }
    void SetParent(Symbol* parent_) { parent = parent_; }
    const NamespaceSymbol* Ns() const;
    NamespaceSymbol* Ns();
    const ClassTypeSymbol* ClassNoThrow() const;
    ClassTypeSymbol* ClassNoThrow();
    const ContainerSymbol* ClassOrNsNoThrow() const;
    ContainerSymbol* ClassOrNsNoThrow() ;
    const ContainerSymbol* ClassInterfaceOrNsNoThrow() const;
    ContainerSymbol* ClassInterfaceOrNsNoThrow();
    const ContainerSymbol* ClassInterfaceEnumDelegateOrNsNoThrow() const;
    ContainerSymbol* ClassInterfaceEnumDelegateOrNsNoThrow();
    const ClassTypeSymbol* Class() const;
    ClassTypeSymbol* Class();
    const ClassTypeSymbol* ContainingClassNoThrow() const;
    ClassTypeSymbol* ContainingClassNoThrow();
    const InterfaceTypeSymbol* InterfaceNoThrow() const;
    InterfaceTypeSymbol* InterfaceNoThrow();
    const InterfaceTypeSymbol* ContainingInterfaceNoThrow() const;
    InterfaceTypeSymbol* ContainingInterfaceNoThrow() ;
    const FunctionSymbol* FunctionNoThrow() const;
    FunctionSymbol* FunctionNoThrow();
    const FunctionSymbol* Function() const;
    FunctionSymbol* Function();
    const FunctionSymbol* ContainingFunctionNoThrow() const;
    FunctionSymbol* ContainingFunctionNoThrow() ;
    const ContainerScope* ClassOrNsScope() const;
    ContainerScope* ClassOrNsScope();
    const ContainerScope* ClassInterfaceOrNsScope() const;
    ContainerScope* ClassInterfaceOrNsScope() ;
    const ContainerScope* ClassInterfaceEnumDelegateOrNsScope() const;
    ContainerScope* ClassInterfaceEnumDelegateOrNsScope();
    Module* GetModule() const { return module; }
    Module* GetModule() { return module; }
    void SetModule(Module* module_) { module = module_; }
    const CompileUnitNode* GetCompileUnit() const { return compileUnit; }
    void SetCompileUnit(CompileUnitNode* compileUnit_) { compileUnit = compileUnit_; }
    const std::u32string& MangledName() const { return mangledName; }
    void SetAttributes(std::unique_ptr<Attributes>&& attributes_);
    Attributes* GetAttributes() const { return attributes.get(); }
    std::unique_ptr<sngxml::dom::Element> ToDomElement(TypeMap& typeMap);
    virtual std::unique_ptr<sngxml::dom::Element> CreateDomElement(TypeMap& typeMap);
    virtual std::u32string Info() const { return std::u32string(); }
    virtual void AppendChildElements(sngxml::dom::Element* element, TypeMap& typeMap) const {}
    virtual bool HasProjectMembers() const { return false; }
    virtual const char* ClassName() const { return "Symbol"; }
private:
    SymbolType symbolType;
    Span span;
    std::u32string name;
    SymbolFlags flags;
    std::u32string mangledName;
    Symbol* parent;
    Module* module;
    CompileUnitNode* compileUnit;
    std::unique_ptr<Attributes> attributes;
};

class SymbolCreator
{
public:
    virtual ~SymbolCreator();
    virtual Symbol* CreateSymbol(const Span& span, const std::u32string& name) = 0;
};

class SymbolFactory
{
public:
    static void Init();
    static void Done();
    static SymbolFactory& Instance() { Assert(instance, "symbol factory not initialized"); return *instance; }
    Symbol* CreateSymbol(SymbolType symbolType, const Span& span, const std::u32string& name);
    void Register(SymbolType symbolType, SymbolCreator* creator);
private:
    static std::unique_ptr<SymbolFactory> instance;
    std::vector<std::unique_ptr<SymbolCreator>> symbolCreators;
    SymbolFactory();
};

void InitSymbol();
void DoneSymbol();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_SYMBOL_INCLUDED
