// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmllvm/EmittingContext.hpp>

namespace cmllvm {

EmittingContext::EmittingContext(int optimizationLevel) : targetTriple(), optimizationLevel(0), targetMachine(), dataLayout()
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    llvm::PassRegistry* passRegistry = llvm::PassRegistry::getPassRegistry();
    llvm::initializeCore(*passRegistry);
    llvm::initializeScalarOpts(*passRegistry);
    llvm::initializeVectorization(*passRegistry);
    llvm::initializeIPO(*passRegistry);
    llvm::initializeAnalysis(*passRegistry);
    llvm::initializeTransformUtils(*passRegistry);
    llvm::initializeInstCombine(*passRegistry);
    llvm::initializeInstrumentation(*passRegistry);
    llvm::initializeTarget(*passRegistry);
    llvm::initializeCodeGen(*passRegistry);
#ifdef _WIN32
    targetTriple = "x86_64-pc-windows-msvc";
#else
    targetTriple = llvm::sys::getDefaultTargetTriple();
#endif
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target)
    {
        throw std::runtime_error("EmittingContext: TargetRegistry::lookupTarget failed: " + error);
    }
    llvm::TargetOptions targetOptions = {};
#ifdef _WIN32
    targetOptions.ExceptionModel = llvm::ExceptionHandling::WinEH;
#endif
    llvm::Optional<llvm::Reloc::Model> relocModel;
    llvm::CodeModel::Model codeModel = llvm::CodeModel::Large;
    llvm::CodeGenOpt::Level codeGenLevel = llvm::CodeGenOpt::None;
    switch (optimizationLevel)
    {
        case 0: codeGenLevel = llvm::CodeGenOpt::None; break;
        case 1: codeGenLevel = llvm::CodeGenOpt::Less; break;
        case 2: codeGenLevel = llvm::CodeGenOpt::Default; break;
        case 3: codeGenLevel = llvm::CodeGenOpt::Aggressive; break;
    }
    targetMachine.reset(target->createTargetMachine(targetTriple, "generic", "", targetOptions, relocModel, codeModel, codeGenLevel));
    dataLayout.reset(new llvm::DataLayout(targetMachine->createDataLayout()));
}

} // namespace cmllvm
