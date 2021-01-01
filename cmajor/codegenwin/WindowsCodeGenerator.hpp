// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegenllvm/LlvmCodeGenerator.hpp>
#include <cmajor/codegenwin/CodeGenWinApi.hpp>
#include <cmajor/binder/BoundStatement.hpp>

namespace cmajor { namespace codegenwin {

using namespace cmajor::binder;

class CODEGENWIN_API WindowsCodeGenerator : public cmajor::codegenllvm::LlvmCodeGenerator
{
public:
    WindowsCodeGenerator(cmajor::ir::EmittingContext& emittingContext_);
    void Visit(BoundReturnStatement& boundReturnStatement) override;
    void Visit(BoundGotoCaseStatement& boundGotoCaseStatement) override;
    void Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement) override;
    void Visit(BoundBreakStatement& boundBreakStatement) override;
    void Visit(BoundContinueStatement& boundContinueStatement) override;
    void Visit(BoundGotoStatement& boundGotoStatement) override;
    void Visit(BoundTryStatement& boundTryStatement) override;
    void Visit(BoundRethrowStatement& boundRethrowStatement) override;
    void CreateCleanup() override;
    void* GetPersonalityFunction() const override;
    void GenerateCodeForCleanups() override;
};

} } // namespace cmajor::codegenwin
