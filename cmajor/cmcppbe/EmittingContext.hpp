// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPBE_EMITTING_CONTEXT_INCLUDED
#define CMAJOR_CMCPPBE_EMITTING_CONTEXT_INCLUDED
#include <cmajor/ir/EmittingContext.hpp>
#include <cmajor/cmcppi/Context.hpp>

namespace cmcppbe {

class EmittingContext : public cmajor::ir::EmittingContext
{
public:
    EmittingContext(int optimizationLevel_);
    int OptimizationLevel() const override { return optimizationLevel; }
    const std::string& TargetTriple() const override { return targetTriple; }
    void* DataLayout() override { return nullptr; }
    void* TargetMachine() override { return nullptr; }
private:
    int optimizationLevel;
    std::string targetTriple;
};

} // namespace cmcppbe

#endif // CMAJOR_CMCPPBE_EMITTING_CONTEXT_INCLUDED
