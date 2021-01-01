// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Scope.hpp>

namespace cmcppi {

Scope::Scope(int16_t id_, int16_t parentScopeId_) : id(id_), parentScopeId(parentScopeId_)
{
}

void Scope::AddLocalVariable(cmajor::debug::DIVariable* localVariable)
{
    localVariables.push_back(std::unique_ptr<cmajor::debug::DIVariable>(localVariable));
}

cmajor::debug::DIVariable* Scope::GetLocalVariable(int32_t index) const
{
    return localVariables[index].get();
}

} // namespace cmcppi
