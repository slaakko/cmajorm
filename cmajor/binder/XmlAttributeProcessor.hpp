// =================================
// Copyright (c) 2020 Seppo Laakko
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
    void TypeCheck(Attribute* attribute, Symbol* symbol) override;
    void GenerateSymbols(Attribute* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope) override;
    void GenerateImplementation(Attribute* attribute, Symbol* symbol, StatementBinder* statementBinder) override;
private:
    void GenerateToXmlSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateSystemDomElementConstructorSymbol(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateToXmlMemberFunctionImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toJsonJsonObjectMemberFunctionSymbol, StatementBinder* statementBinder);
    void GenerateSystemDomElementConstructorImplementation(Attribute* attribute, ClassTypeSymbol* classTypeSymbol, ConstructorSymbol* constructorSymbol, StatementBinder* statementBinder);
    std::unordered_map<Symbol*, MemberFunctionSymbol*> toXmlMemberFunctionSymbolMap;
    std::unordered_map<Symbol*, ConstructorSymbol*> constructorSymbolMap;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_XML_ATTRIBUTE_PROCESSOR_INCLUDED
