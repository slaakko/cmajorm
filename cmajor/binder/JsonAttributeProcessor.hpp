// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_JSON_ATTRIBUTE_PROCESSOR_INCLUDED
#define CMAJOR_BINDER_JSON_ATTRIBUTE_PROCESSOR_INCLUDED
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Module.hpp>
#include <map>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class JsonAttributeProcessor : public AttributeProcessor
{
public:
    JsonAttributeProcessor(Module* module_);
    void TypeCheck(Attribute* attribute, Symbol* symbol) override;
    void GenerateSymbols(Attribute* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope) override;
    void GenerateImplementation(Attribute* attribute, Symbol* symbol, StatementBinder* statementBinder) override;
private:
    Module* module;
    std::unordered_map<Symbol*, MemberFunctionSymbol*> jsonCreatorMap;
    std::unordered_map<Symbol*, ConstructorSymbol*> jsonConstructorMap;
    std::unordered_map<Symbol*, MemberFunctionSymbol*> toJsonJsonObjectMemberFunctionSymbolMap;
    std::unordered_map<Symbol*, MemberFunctionSymbol*> toJsonObjectMemberFunctionSymbolMap;
    void GenerateMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol);
    void CheckMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol, std::map<std::u32string, Attribute*>& memberVariableFieldNames);
    void GenerateJsonCreatorFunctionSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol);
    void GenerateJsonConstructorSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol);
    void GenerateToJsonJsonObjectSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol);
    void GenerateToJsonSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateJsonConstructorImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, ConstructorSymbol* jsonConstructorSymbol, StatementBinder* statementBinder);
    void GenerateJsonCreatorImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* jsonCreatorFunctionSymbol, StatementBinder* statementBinder);
    void GenerateToJsonJsonObjectImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol, StatementBinder* statementBinder);
    void GenerateToJsonImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonMemberFunctionSymbol, StatementBinder* statementBinder);
};

class JsonFieldNameAttributeProcessor : public AttributeProcessor
{
public:
    JsonFieldNameAttributeProcessor(Module* module_);
    void TypeCheck(Attribute* attribute, Symbol* symbol) override;
private:
    Module* module;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_JSON_ATTRIBUTE_PROCESSOR_INCLUDED
