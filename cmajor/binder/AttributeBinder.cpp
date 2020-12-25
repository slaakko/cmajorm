// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/AttributeBinder.hpp>
#include <cmajor/binder/JsonAttributeProcessor.hpp>
#include <cmajor/binder/SystemDefaultAttributeProcessor.hpp>
#include <cmajor/binder/XmlAttributeProcessor.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor {namespace binder {

using namespace soulng::unicode;
using namespace cmajor::symbols;

AttributeProcessor::AttributeProcessor(const std::u32string& attributeName_) : attributeName(attributeName_)
{
}

AttributeProcessor::~AttributeProcessor()
{
}

void AttributeProcessor::TypeCheck(Attribute* attribute, Symbol* symbol)
{
    throw Exception("attribute '" + ToUtf8(attribute->Name()) + "' for symbol type '" + symbol->TypeString() + "' not supported", attribute->GetSpan(), attribute->ModuleId(), 
        symbol->GetSpan(), symbol->SourceModuleId());
}

void AttributeProcessor::GenerateSymbols(Attribute* attribute, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
}

void AttributeProcessor::GenerateImplementation(Attribute* attribute, Symbol* symbol, StatementBinder* statementBinder)
{
}

AttributeBinder::AttributeBinder(Module* module)
{
    AttributeProcessor* jsonAttributeProcessor = new JsonAttributeProcessor(module);
    attributeProcessors.push_back(std::unique_ptr<AttributeProcessor>(jsonAttributeProcessor));
    JsonFieldNameAttributeProcessor* jsonFieldNameAttributeProcessor = new JsonFieldNameAttributeProcessor();
    attributeProcessors.push_back(std::unique_ptr<AttributeProcessor>(jsonFieldNameAttributeProcessor));
    AttributeProcessor* systemDefaultAttributeProcessor = new SystemDefaultAttributeProcessor();
    attributeProcessors.push_back(std::unique_ptr<AttributeProcessor>(systemDefaultAttributeProcessor));
    AttributeProcessor* xmlAttributeProcessor = new XmlAttributeProcessor();
    attributeProcessors.push_back(std::unique_ptr<AttributeProcessor>(xmlAttributeProcessor));
    for (const std::unique_ptr<AttributeProcessor>& attributeProcessor : attributeProcessors)
    {
        attributeProcessorMap[attributeProcessor->AttributeName()] = attributeProcessor.get();
    }
}

void AttributeBinder::BindAttributes(Attributes* attrs, Symbol* symbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    if (!attrs) return;
    const std::vector<std::unique_ptr<Attribute>>& attributes = attrs->GetAttributes();
    for (const std::unique_ptr<Attribute>& attribute : attributes)
    {
        const std::u32string& attrName = attribute->Name();
        auto it = attributeProcessorMap.find(attrName);
        if (it != attributeProcessorMap.cend())
        {
            AttributeProcessor* processor = it->second;
            processor->TypeCheck(attribute.get(), symbol);
            processor->GenerateSymbols(attribute.get(), symbol, boundCompileUnit, containerScope);
        }
        else
        {
            throw Exception("unknown attribute '" + ToUtf8(attrName) + "'", attribute->GetSpan(), attribute->ModuleId());
        }
    }
    symbol->SetAttributes(std::unique_ptr<Attributes>(attrs->Clone()));
}

void AttributeBinder::GenerateImplementation(Attributes* attrs, Symbol* symbol, StatementBinder* statementBinder)
{
    if (!attrs) return;
    const std::vector<std::unique_ptr<Attribute>>& attributes = attrs->GetAttributes();
    for (const std::unique_ptr<Attribute>& attribute : attributes)
    {
        const std::u32string& attrName = attribute->Name();
        auto it = attributeProcessorMap.find(attrName);
        if (it != attributeProcessorMap.cend())
        {
            AttributeProcessor* processor = it->second;
            processor->GenerateImplementation(attribute.get(), symbol, statementBinder);
        }
        else
        {
            throw Exception("unknown attribute '" + ToUtf8(attrName) + "'", attribute->GetSpan(), attribute->ModuleId());
        }
    }
}

} } // namespace cmajor::binder
