// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_EMITTER_LINUX_EMITTER_INCLUDED
#define CMAJOR_EMITTER_LINUX_EMITTER_INCLUDED
#include <cmajor/emitter/BasicEmitter.hpp>

namespace cmajor { namespace emitter {

class LinuxEmitter : public BasicEmitter
{
public:
    LinuxEmitter(EmittingContext& emittingContext_, const std::string& compileUnitModuleName_, cmajor::symbols::Module& symbolsModule_);
    llvm::Function* GetPersonalityFunction() const override;
    void Visit(BoundReturnStatement& boundReturnStatement) override;
    void Visit(BoundGotoCaseStatement& boundGotoCaseStatement) override;
    void Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement) override;
    void Visit(BoundBreakStatement& boundBreakStatement) override;
    void Visit(BoundContinueStatement& boundContinueStatement) override;
    void Visit(BoundGotoStatement& boundGotoStatement) override;
    void Visit(BoundTryStatement& boundTryStatement) override;
    void Visit(BoundRethrowStatement& boundRethrowStatement) override;
    void CreateCleanup() override;
    void GenerateCodeForCleanups() override;
};

} } // namespace cmajor::emitter

#endif // CMAJOR_EMITTER_LINUX_EMITTER_INCLUDED
