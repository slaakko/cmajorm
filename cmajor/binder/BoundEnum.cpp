// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundEnum.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace binder {

BoundEnumTypeDefinition::BoundEnumTypeDefinition(Module* module_, EnumTypeSymbol* enumTypeSymbol_) : BoundNode(module_, enumTypeSymbol_->GetSpan(), BoundNodeType::boundEnumTypeDefinition), enumTypeSymbol(enumTypeSymbol_)
{
}

void BoundEnumTypeDefinition::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundEnumTypeDefinition::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot load from enum type", GetSpan());
}

void BoundEnumTypeDefinition::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to enum type", GetSpan());
}

} } // namespace cmajor::binder
