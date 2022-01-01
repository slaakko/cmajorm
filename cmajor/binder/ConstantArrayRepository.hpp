// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CONSTANT_ARRAY_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_CONSTANT_ARRAY_REPOSITORY_INCLUDED
#include <cmajor/symbols/ConstantSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class ConstantArrayRepository
{
public:
    ConstantArrayRepository();
    void AddConstantArray(ConstantSymbol* constantArraySymbol);
    const std::vector<ConstantSymbol*>& ConstantArrays() const { return constantArrays; }
private:
    std::vector<ConstantSymbol*> constantArrays;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CONSTANT_ARRAY_REPOSITORY_INCLUDED
