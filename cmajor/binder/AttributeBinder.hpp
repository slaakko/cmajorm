// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_ATTRIBUTE_BINDER_INCLUDED
#define CMAJOR_BINDER_ATTRIBUTE_BINDER_INCLUDED
#include <cmajor/ast/Attribute.hpp>
#include <cmajor/symbols/Symbol.hpp>
#include <unordered_map>

namespace cmajor { namespace binder {

using namespace cmajor::ast;
using namespace cmajor::symbols;
class BoundCompileUnit;

class StatementBinder;

class AttributeProcessor
{
public:
    AttributeProcessor(const std::u32string& attributeName_);
    virtual ~AttributeProcessor();
    const std::u32string& AttributeName() const { return attributeName; }
    virtual void TypeCheck(Attribute* attribute, Symbol* symbol);
    virtual void GenerateSymbols(Attribute* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    virtual void GenerateImplementation(Attribute* attribute, Symbol* symbol, StatementBinder* statementBinder);
private:
    std::u32string attributeName;
};

class AttributeBinder
{
public:
    AttributeBinder(Module* module);
    void BindAttributes(Attributes* attrs, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope);
    void GenerateImplementation(Attributes* attrs, Symbol* symbol, StatementBinder* statementBinder);
private:
    std::vector<std::unique_ptr<AttributeProcessor>> attributeProcessors;
    std::unordered_map<std::u32string, AttributeProcessor*> attributeProcessorMap;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_ATTRIBUTE_BINDER_INCLUDED
