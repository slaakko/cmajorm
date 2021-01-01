// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/ConstantStructureRepository.hpp>

namespace cmajor {namespace binder {

ConstantStructureRepository::ConstantStructureRepository()
{
}

void ConstantStructureRepository::AddConstantStructure(ConstantSymbol* constantStructureSymbol)
{
    constantStructures.push_back(constantStructureSymbol);
}

} } // namespace cmajor::binder
