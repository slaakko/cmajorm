// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CONSTANT_STRUCTURE_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_CONSTANT_STRUCTURE_REPOSITORY_INCLUDED
#include <cmajor/symbols/ConstantSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class ConstantStructureRepository
{
public:
    ConstantStructureRepository();
    void AddConstantStructure(ConstantSymbol* constantStructureSymbol);
    const std::vector<ConstantSymbol*>& ConstantStructures() const { return constantStructures; }
private:
    std::vector<ConstantSymbol*> constantStructures;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CONSTANT_STRUCTURE_REPOSITORY_INCLUDED
