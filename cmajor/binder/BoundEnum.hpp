// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_ENUMT_INCLUDED
#define CMAJOR_BINDER_BOUND_ENUMT_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BoundEnumTypeDefinition : public BoundNode
{
public:
    BoundEnumTypeDefinition(Module* module_, EnumTypeSymbol* enumTypeSymbol_);
    void Accept(BoundNodeVisitor& visitor) override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    EnumTypeSymbol* GetEnumTypeSymbol() const { return enumTypeSymbol; }
private:
    EnumTypeSymbol* enumTypeSymbol;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_ENUMT_INCLUDED
