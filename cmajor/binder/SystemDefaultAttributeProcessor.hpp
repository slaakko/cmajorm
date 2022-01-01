// =================================
// Copyright (c) 2022 Seppo Laakko
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
    SystemDefaultAttributeProcessor();
    void TypeCheck(AttributeNode* attribute, Symbol* symbol) override;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_SYSTEM_DEFAULT_ATTRIBUTE_PROCESSOR_INCLUDED
