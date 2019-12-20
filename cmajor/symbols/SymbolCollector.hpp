// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SYMBOL_COLLECTOR_INCLUDED
#define CMAJOR_SYMBOLS_SYMBOL_COLLECTOR_INCLUDED
#include <cmajor/symbols/NamespaceSymbol.hpp>
#include <cmajor/symbols/BasicTypeSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/ConstantSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/symbols/DelegateSymbol.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>

namespace cmajor { namespace symbols {

struct ByFullName
{
    bool operator()(Symbol* left, Symbol* right) const;
};

struct ByDocName
{
    bool operator()(Symbol* left, Symbol* right) const;
};

class SymbolCollector
{
public:
    SymbolCollector();
    void AddBasicType(BasicTypeSymbol* basicType);
    void AddClass(ClassTypeSymbol* class_);
    void AddConcept(ConceptSymbol* conceptSymbol);
    void AddConstant(ConstantSymbol* constant);
    void AddDelegate(DelegateTypeSymbol* delegate);
    void AddClassDelegate(ClassDelegateTypeSymbol* classDelegate);
    void AddEnumeratedType(EnumTypeSymbol* enumType);
    void AddEnumerationConstant(EnumConstantSymbol* enumConstant);
    void AddFunction(FunctionSymbol* function);
    void AddInterface(InterfaceTypeSymbol* interface_);
    void AddTypedef(TypedefSymbol* typedef_);
    void AddMemberVariable(MemberVariableSymbol* memberVariable);
    void AddGlobalVariable(GlobalVariableSymbol* globalVariable);
    void SortByFullName();
    void SortByDocName();
    const std::vector<BasicTypeSymbol*>& BasicTypes() const { return basicTypes; }
    const std::vector<ClassTypeSymbol*>& Classes() const { return classes; }
    const std::vector<InterfaceTypeSymbol*>& Interfaces() const { return interfaces; }
    const std::vector<FunctionSymbol*>& Functions() const { return functions; }
    const std::vector<TypedefSymbol*>& Typedefs() const { return typedefs; }
    const std::vector<ConceptSymbol*>& Concepts() const { return concepts; }
    const std::vector<DelegateTypeSymbol*>& Delegates() const { return delegates; }
    const std::vector<ClassDelegateTypeSymbol*>& ClassDelegates() const { return classDelegates; }
    const std::vector<ConstantSymbol*>& Constants() const { return constants; }
    const std::vector<EnumTypeSymbol*>& EnumeratedTypes() const { return enumeratedTypes; }
    const std::vector<EnumConstantSymbol*>& EnumerationConstants() const { return enumerationConstants; }
    const std::vector<MemberVariableSymbol*>& MemberVariables() const { return memberVariables; }
    bool IsEmpty() const;
private:
    std::vector<BasicTypeSymbol*> basicTypes;
    std::vector<ClassTypeSymbol*> classes;
    std::vector<InterfaceTypeSymbol*> interfaces;
    std::vector<FunctionSymbol*> functions;
    std::vector<TypedefSymbol*> typedefs;
    std::vector<ConceptSymbol*> concepts;
    std::vector<ConstantSymbol*> constants;
    std::vector<DelegateTypeSymbol*> delegates;
    std::vector<ClassDelegateTypeSymbol*> classDelegates;
    std::vector<EnumTypeSymbol*> enumeratedTypes;
    std::vector<EnumConstantSymbol*> enumerationConstants;
    std::vector<MemberVariableSymbol*> memberVariables;
    std::vector<GlobalVariableSymbol*> globalVariables;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_SYMBOL_COLLECTOR_INCLUDED
