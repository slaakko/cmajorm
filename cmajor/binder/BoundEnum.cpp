// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundEnum.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace binder {

BoundEnumTypeDefinition::BoundEnumTypeDefinition(EnumTypeSymbol* enumTypeSymbol_) : BoundNode(enumTypeSymbol_->GetSpan(), enumTypeSymbol_->SourceModuleId(), 
    BoundNodeType::boundEnumTypeDefinition), enumTypeSymbol(enumTypeSymbol_)
{
}

void BoundEnumTypeDefinition::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundEnumTypeDefinition::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot load from enum type", GetSpan(), ModuleId());
}

void BoundEnumTypeDefinition::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to enum type", GetSpan(), ModuleId());
}

} } // namespace cmajor::binder
