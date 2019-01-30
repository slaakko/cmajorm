// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMLLVM_EMITTING_CONTEXT_INCLUDED
#define CMAJOR_CMLLVM_EMITTING_CONTEXT_INCLUDED
#include <cmajor/ir/EmittingContext.hpp>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Object/ObjectFile.h>
#include <llvm/CodeGen/TargetPassConfig.h>

namespace cmllvm {

class EmittingContext : public cmajor::ir::EmittingContext
{
public:
    EmittingContext(int optimizationLevel);
    llvm::LLVMContext& Context() { return context; }
    const std::string& TargetTriple() const override { return targetTriple; }
    void* DataLayout() override { return dataLayout.get(); }
    void* TargetMachine() override { return targetMachine.get(); }
    int OptimizationLevel() const override { return optimizationLevel; }
private:
    llvm::LLVMContext context;
    std::string targetTriple;
    int optimizationLevel;
    std::unique_ptr<llvm::TargetMachine> targetMachine;
    std::unique_ptr<llvm::DataLayout> dataLayout;
};

} // namespace cmllvm

#endif // CMAJOR_CMLLVM_EMITTING_CONTEXT_INCLUDED
