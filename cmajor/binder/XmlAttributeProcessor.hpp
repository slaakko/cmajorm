// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_XML_ATTRIBUTE_PROCESSOR_INCLUDED
#define CMAJOR_BINDER_XML_ATTRIBUTE_PROCESSOR_INCLUDED
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <sngcm/ast/Class.hpp>

namespace cmajor { namespace binder {

class BoundClass;

using sngcm::ast::MemberFunctionNode;

class XmlAttributeProcessor : public AttributeProcessor
{
public:
    XmlAttributeProcessor();
    void TypeCheck(AttributeNode* attribute, Symbol* symbol) override;
    void GenerateSymbols(AttributeNode* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope) override;
    void GenerateImplementation(AttributeNode* attribute, Symbol* symbol, StatementBinder* statementBinder) override;
private:
    void TypeCheckClass(ClassTypeSymbol* classType);
    void GenerateMemberSymbols(AttributeNode* attribute, ClassTypeSymbol* classType, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void CheckXmlSerializableInterface(ClassTypeSymbol* classType, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void CheckVirtualDestructor(ClassTypeSymbol* classTypeSymbol);
    void GenerateDestructorImplementation(DestructorSymbol* destructorSymbol, StatementBinder* statementBinder);
    void GenerateMemberVariableSymbols(AttributeNode* attribute, ClassTypeSymbol* classType, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateStaticClassNameSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateStaticClassNameImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* staticClassNameSymbol, StatementBinder* statementBinder);
    void GenerateCreateFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateCreateImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* createFunctionSymbol, StatementBinder* statementBinder);
    void GenerateRegisterFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateRegisterImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* registerSymbol, StatementBinder* statementBinder);
    void GenerateDestroyObjectFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateDestroyObjectImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* destroyObjectSymbol, StatementBinder* statementBinder);
    void GenerateObjectIdFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateObjectIdImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* objectIdSymbol, StatementBinder* statementBinder);
    void GenerateSetObjectIdFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateSetObjectIdImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setObjectIdSymbol, StatementBinder* statementBinder);
    void GenerateContainerFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateContainerImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* containerSymbol, StatementBinder* statementBinder);
    void GenerateSetContainerFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateSetContainerImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setContainerSymbol, StatementBinder* statementBinder);
    void GenerateClassIdFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateClassIdImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* classIdSymbol, StatementBinder* statementBinder);
    void GenerateClassNameFunctionSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateClassNameImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* classNameSymbol, StatementBinder* statementBinder);
    void GenerateSetObjectXmlAttributesSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateSetObjectXmlAttributesImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setObjectXmlAttributesSymbol, StatementBinder* statementBinder);
    void GenerateToXmlPlainSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateToXmlPlainImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toXmlSymbol, StatementBinder* statementBinder);
    void GenerateToXmlSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateToXmlImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* toXmlSymbol, StatementBinder* statementBinder);
    void GenerateFromXmlSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateFromXmlImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* fromXmlSymbol, StatementBinder* statementBinder);
    void GenerateGetPtrsSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateGetPtrsImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* getPtrsSymbol, StatementBinder* statementBinder);
    void GenerateIsOwnedSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateIsOwnedImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* isOwnedMemFunSymbol, StatementBinder* statementBinder);
    void GenerateSetOwnedSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateSetOwnedImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setOwnedMemFunSymbol, StatementBinder* statementBinder);
    void GenerateResetOwnedSymbol(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateResetOwnedImplementation(AttributeNode* attribute, ClassTypeSymbol* classTypeSymbol, MemberFunctionSymbol* setOwnedMemFunSymbol, StatementBinder* statementBinder);
    bool HasXmlBaseClass(ClassTypeSymbol* classType) const;
    void CompileMemberFunction(MemberFunctionSymbol* memberFunctionSymbol, CompoundStatementNode& compoundStatementNode, MemberFunctionNode& memberFunctionNode, 
        std::unique_ptr<BoundFunction>&& boundFunction, StatementBinder* statementBinder);
    std::set<std::u32string> reservedMemberFunctionNames;
    std::set<std::u32string> reservedMemberVariableNames;
    std::map<Symbol*, std::vector<std::pair<FunctionSymbol*, int>>> functionSymbolMap;
};


} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_XML_ATTRIBUTE_PROCESSOR_INCLUDED
