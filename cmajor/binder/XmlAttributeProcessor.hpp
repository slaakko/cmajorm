// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_XML_ATTRIBUTE_PROCESSOR_INCLUDED
#define CMAJOR_BINDER_XML_ATTRIBUTE_PROCESSOR_INCLUDED
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace binder {

class XmlAttributeProcessor : public AttributeProcessor
{
public:
    XmlAttributeProcessor();
    void TypeCheck(AttributeNode* attribute, Symbol* symbol) override;
    void GenerateSymbols(AttributeNode* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope) override;
    void GenerateImplementation(AttributeNode* attribute, Symbol* symbol, StatementBinder* statementBinder) override;
private:
    void GenerateToXmlSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateSystemDomElementConstructorSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateToXmlMemberFunctionImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol, StatementBinder* statementBinder);
    void GenerateSystemDomElementConstructorImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, ConstructorSymbol* constructorSymbol, StatementBinder* statementBinder);
    std::unordered_map<Symbol*, MemberFunctionSymbol*> toXmlMemberFunctionSymbolMap;
    std::unordered_map<Symbol*, ConstructorSymbol*> constructorSymbolMap;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_XML_ATTRIBUTE_PROCESSOR_INCLUDED
