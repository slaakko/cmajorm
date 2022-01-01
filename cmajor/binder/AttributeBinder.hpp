// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_ATTRIBUTE_BINDER_INCLUDED
#define CMAJOR_BINDER_ATTRIBUTE_BINDER_INCLUDED
#include <cmajor/binder/BinderApi.hpp>
#include <sngcm/ast/Attribute.hpp>
#include <cmajor/symbols/Symbol.hpp>
#include <unordered_map>

namespace cmajor { namespace binder {

using namespace sngcm::ast;
using namespace cmajor::symbols;
class BoundCompileUnit;

class StatementBinder;

class BINDER_API AttributeProcessor
{
public:
    AttributeProcessor(const std::u32string& attributeName_);
    virtual ~AttributeProcessor();
    const std::u32string& AttributeName() const { return attributeName; }
    virtual void TypeCheck(AttributeNode* attribute, Symbol* symbol);
    virtual void GenerateSymbols(AttributeNode* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    virtual void GenerateImplementation(AttributeNode* attribute, Symbol* symbol, StatementBinder* statementBinder);
private:
    std::u32string attributeName;
};

class BINDER_API AttributeBinder
{
public:
    AttributeBinder(Module* module);
    AttributeBinder(const AttributeBinder&) = delete;
    AttributeBinder& operator=(const AttributeBinder&) = delete;
    void BindAttributes(AttributesNode* attrs, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateImplementation(AttributesNode* attrs, Symbol* symbol, StatementBinder* statementBinder);
private:
    std::vector<std::unique_ptr<AttributeProcessor>> attributeProcessors;
    std::unordered_map<std::u32string, AttributeProcessor*> attributeProcessorMap;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_ATTRIBUTE_BINDER_INCLUDED
