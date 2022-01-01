// =================================
// Copyright (c) 2022 Seppo Laakko
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
    void TypeCheck(AttributeNode* attribute, Symbol* symbol) override;
    void GenerateSymbols(AttributeNode* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope) override;
    void GenerateImplementation(AttributeNode* attribute, Symbol* symbol, StatementBinder* statementBinder) override;
private:
    Module* module;
    std::unordered_map<Symbol*, MemberFunctionSymbol*> jsonCreatorMap;
    std::unordered_map<Symbol*, ConstructorSymbol*> jsonConstructorMap;
    std::unordered_map<Symbol*, MemberFunctionSymbol*> toJsonJsonObjectMemberFunctionSymbolMap;
    std::unordered_map<Symbol*, MemberFunctionSymbol*> toJsonObjectMemberFunctionSymbolMap;
    void GenerateMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol);
    void CheckMemberVariableJsonFieldNames(ClassTypeSymbol* classTypeSymbol, std::map<std::u32string, AttributeNode*>& memberVariableFieldNames);
    void GenerateJsonCreatorFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol);
    void GenerateJsonConstructorSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol);
    void GenerateToJsonJsonObjectSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol);
    void GenerateToJsonSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateJsonConstructorImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, ConstructorSymbol* jsonConstructorSymbol, StatementBinder* statementBinder);
    void GenerateJsonCreatorImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* jsonCreatorFunctionSymbol, StatementBinder* statementBinder);
    void GenerateToJsonJsonObjectImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol, StatementBinder* statementBinder);
    void GenerateToJsonImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonMemberFunctionSymbol, StatementBinder* statementBinder);
};

class JsonFieldNameAttributeProcessor : public AttributeProcessor
{
public:
    JsonFieldNameAttributeProcessor();
    void TypeCheck(AttributeNode* attribute, Symbol* symbol) override;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_JSON_ATTRIBUTE_PROCESSOR_INCLUDED
