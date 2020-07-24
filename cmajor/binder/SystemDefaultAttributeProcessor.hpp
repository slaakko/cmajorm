// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_SYSTEM_DEFAULT_ATTRIBUTE_PROCESSOR_INCLUDED
#define CMAJOR_BINDER_SYSTEM_DEFAULT_ATTRIBUTE_PROCESSOR_INCLUDED
#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class SystemDefaultAttributeProcessor : public AttributeProcessor
{
public:
    SystemDefaultAttributeProcessor(Module* module_);
    void TypeCheck(Attribute* attribute, Symbol* symbol) override;
private:
    Module* module;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_SYSTEM_DEFAULT_ATTRIBUTE_PROCESSOR_INCLUDED
