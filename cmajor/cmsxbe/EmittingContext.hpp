// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXBE_EMITTING_CONTEXT_INCLUDED
#define CMAJOR_CMSXBE_EMITTING_CONTEXT_INCLUDED
#include <cmajor/ir/EmittingContext.hpp>
#include <cmajor/cmsxi/Context.hpp>

namespace cmsxbe {

class EmittingContext : public cmajor::ir::EmittingContext
{
public:
    EmittingContext(int optimizationLevel_);
    int OptimizationLevel() const override { return optimizationLevel; }
    const std::string& TargetTriple() const override { return targetTriple; }
    void* DataLayout() override { return nullptr; }
    void* TargetMachine() override { return nullptr; }
    cmsxi::Context* GetContext() { return &context; }
private:
    int optimizationLevel;
    std::string targetTriple;
    cmsxi::Context context;
};

} // namespace cmsxbe

#endif // CMAJOR_CMSXBE_EMITTING_CONTEXT_INCLUDED
