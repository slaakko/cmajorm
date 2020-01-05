// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/ConstantArrayRepository.hpp>

namespace cmajor { namespace binder {

ConstantArrayRepository::ConstantArrayRepository()
{
}

void ConstantArrayRepository::AddConstantArray(ConstantSymbol* constantArraySymbol)
{
    constantArrays.push_back(constantArraySymbol);
}

} } // namespace cmajor::binder
