// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/SystemDefaultAttributeProcessor.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

SystemDefaultAttributeProcessor::SystemDefaultAttributeProcessor(Module* module_) : AttributeProcessor(U"system_default"), module(module_)
{
}

void SystemDefaultAttributeProcessor::TypeCheck(Attribute* attribute, Symbol* symbol)
{
    if (symbol->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
        if (attribute->Value().empty() || attribute->Value() == U"true")
        {
            functionSymbol->SetSystemDefault();
        }
        else if (attribute->Value() != U"false")
        {
            throw Exception(module, "unknown attribute value '" + ToUtf8(attribute->Value()) + "' for attribute '" + ToUtf8(attribute->Name()) + "'", attribute->GetSpan());
        }
    }
    else
    {
        AttributeProcessor::TypeCheck(attribute, symbol);
    }
}

} } // namespace cmajor::binder
