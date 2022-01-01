// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/SystemDefaultAttributeProcessor.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

SystemDefaultAttributeProcessor::SystemDefaultAttributeProcessor() : AttributeProcessor(U"system_default")
{
}

void SystemDefaultAttributeProcessor::TypeCheck(AttributeNode* attribute, Symbol* symbol)
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
            throw Exception("unknown attribute value '" + ToUtf8(attribute->Value()) + "' for attribute '" + ToUtf8(attribute->Name()) + "'", attribute->GetSpan(), attribute->ModuleId());
        }
    }
    else
    {
        AttributeProcessor::TypeCheck(attribute, symbol);
    }
}

} } // namespace cmajor::binder
