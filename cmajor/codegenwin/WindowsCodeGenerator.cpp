// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegenwin/WindowsCodeGenerator.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace codegenwin {

using namespace cmajor::binder;

WindowsCodeGenerator::WindowsCodeGenerator(cmajor::ir::EmittingContext& emittingContext_) : LlvmCodeGenerator(emittingContext_)
{
}

void WindowsCodeGenerator::Visit(BoundReturnStatement& boundReturnStatement)
{
    emitter->SetCurrentDebugLocation(boundReturnStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundReturnStatement);
    Pad* prevCurrentPad = currentPad;
    while (currentPad != nullptr)
    {
        void* returnTarget = emitter->CreateBasicBlock("return");
        emitter->CreateCatchRet(currentPad->value, returnTarget);
        emitter->SetCurrentBasicBlock(returnTarget);
        currentPad = currentPad->parent;
    }
    BoundFunctionCall* returnFunctionCall = boundReturnStatement.ReturnFunctionCall();
    if (returnFunctionCall)
    {
        returnFunctionCall->Accept(*this);
        void* returnValue = emitter->Stack().Pop();
        if (sequenceSecond)
        {
            sequenceSecond->SetGenerated();
            sequenceSecond->Accept(*this);
        }
        ExitBlocks(nullptr);
        //CreateExitFunctionCall();
        GenerateExitFunctionCode(*currentFunction);
        emitter->CreateRet(returnValue);
        lastInstructionWasRet = true;
    }
    else
    {
        ExitBlocks(nullptr);
        //CreateExitFunctionCall();
        GenerateExitFunctionCode(*currentFunction);
        emitter->CreateRetVoid();
        lastInstructionWasRet = true;
    }
    BoundCompoundStatement* body = currentFunction->Body();
    BoundStatement* lastStatement = nullptr;
    if (!body->Statements().empty())
    {
        lastStatement = body->Statements().back().get();
    }
    if (lastStatement && lastStatement != &boundReturnStatement)
    {
        void* nextBlock = emitter->CreateBasicBlock("next");
        emitter->SetCurrentBasicBlock(nextBlock);
        basicBlockOpen = true;
        lastInstructionWasRet = false;
    }
    currentPad = prevCurrentPad;
}

void WindowsCodeGenerator::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    emitter->SetCurrentDebugLocation(boundGotoCaseStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoCaseStatement);
    Assert(breakTargetBlock, "break target not set");
    Pad* prevCurrentPad = currentPad;
    BoundStatement* parent = currentBlock;
    while (currentPad != nullptr && parent && parent != breakTargetBlock)
    {
        if (parent->GetBoundNodeType() == BoundNodeType::boundTryStatement)
        {
            void* fromCatchTarget = emitter->CreateBasicBlock("fromCatch");
            emitter->CreateCatchRet(currentPad->value, fromCatchTarget);
            emitter->SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(breakTargetBlock);
    IntegralValue integralCaseValue(boundGotoCaseStatement.CaseValue());
    auto it = currentCaseMap->find(integralCaseValue);
    if (it != currentCaseMap->cend())
    {
        void* caseDest = it->second;
        emitter->CreateBr(caseDest);
    }
    else
    {
        throw Exception(symbolsModule, "case not found", boundGotoCaseStatement.GetSpan());
    }
    currentPad = prevCurrentPad;
}

void WindowsCodeGenerator::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    emitter->SetCurrentDebugLocation(boundGotoDefaultStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoDefaultStatement);
    Assert(breakTargetBlock, "break target not set");
    Pad* prevCurrentPad = currentPad;
    BoundStatement* parent = currentBlock;
    while (currentPad != nullptr && parent && parent != breakTargetBlock)
    {
        if (parent->GetBoundNodeType() == BoundNodeType::boundTryStatement)
        {
            void* fromCatchTarget = emitter->CreateBasicBlock("fromCatch");
            emitter->CreateCatchRet(currentPad->value, fromCatchTarget);
            emitter->SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(breakTargetBlock);
    if (defaultDest)
    {
        emitter->CreateBr(defaultDest);
    }
    else
    {
        throw Exception(symbolsModule, "no default destination", boundGotoDefaultStatement.GetSpan());
    }
    currentPad = prevCurrentPad;
}

void WindowsCodeGenerator::Visit(BoundBreakStatement& boundBreakStatement)
{
    emitter->SetCurrentDebugLocation(boundBreakStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundBreakStatement);
    Assert(breakTarget && breakTargetBlock, "break target not set");
    Pad* prevCurrentPad = currentPad;
    BoundStatement* parent = currentBlock;
    while (currentPad != nullptr && parent && parent != breakTargetBlock)
    {
        if (parent->GetBoundNodeType() == BoundNodeType::boundTryStatement)
        {
            void* fromCatchTarget = emitter->CreateBasicBlock("fromCatch");
            emitter->CreateCatchRet(currentPad->value, fromCatchTarget);
            emitter->SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(breakTargetBlock);
    emitter->CreateBr(breakTarget);
    void* nextBlock = emitter->CreateBasicBlock("next");
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
    currentPad = prevCurrentPad;
}

void WindowsCodeGenerator::Visit(BoundContinueStatement& boundContinueStatement)
{
    emitter->SetCurrentDebugLocation(boundContinueStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundContinueStatement);
    Assert(continueTarget && continueTargetBlock, "continue target not set");
    Pad* prevCurrentPad = currentPad;
    BoundStatement* parent = currentBlock;
    while (currentPad != nullptr && parent && parent != continueTargetBlock)
    {
        if (parent->GetBoundNodeType() == BoundNodeType::boundTryStatement)
        {
            void* fromCatchTarget = emitter->CreateBasicBlock("fromCatch");
            emitter->CreateCatchRet(currentPad->value, fromCatchTarget);
            emitter->SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(continueTargetBlock);
    emitter->CreateBr(continueTarget);
    void* nextBlock = emitter->CreateBasicBlock("next");
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
    currentPad = prevCurrentPad;
}

void WindowsCodeGenerator::Visit(BoundGotoStatement& boundGotoStatement)
{
    emitter->SetCurrentDebugLocation(boundGotoStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoStatement);
    Pad* prevCurrentPad = currentPad;
    BoundStatement* parent = currentBlock;
    while (currentPad != nullptr && parent && parent != boundGotoStatement.TargetBlock())
    {
        if (parent->GetBoundNodeType() == BoundNodeType::boundTryStatement)
        {
            void* fromCatchTarget = emitter->CreateBasicBlock("fromCatch");
            emitter->CreateCatchRet(currentPad->value, fromCatchTarget);
            emitter->SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(boundGotoStatement.TargetBlock());
    auto it = labeledStatementMap.find(boundGotoStatement.TargetStatement());
    if (it != labeledStatementMap.cend())
    {
        void* target = it->second;
        emitter->CreateBr(target);
    }
    else
    {
        throw Exception(symbolsModule, "goto target not found", boundGotoStatement.GetSpan());
    }
    void* nextBlock = emitter->CreateBasicBlock("next");
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
    currentPad = prevCurrentPad;
}

void WindowsCodeGenerator::Visit(BoundTryStatement& boundTryStatement)
{
    ClearFlags();
    SetTarget(&boundTryStatement);
    void* prevHandlerBlock = handlerBlock;
    void* prevCleanupBlock = cleanupBlock;
    handlerBlock = emitter->CreateBasicBlock("handlers");
    cleanupBlock = nullptr;
    boundTryStatement.TryBlock()->Accept(*this);
    void* nextTarget = emitter->CreateBasicBlock("next");
    emitter->CreateBr(nextTarget);
    emitter->SetCurrentBasicBlock(handlerBlock);
    handlerBlock = prevHandlerBlock;
    Pad* parentPad = currentPad;
    void* catchSwitch = nullptr;
    if (parentPad == nullptr)
    {
        catchSwitch = emitter->CreateCatchSwitch(prevHandlerBlock);
    }
    else
    {
        catchSwitch = emitter->CreateCatchSwitchWithParent(parentPad->value, prevHandlerBlock);
    }
    Pad* pad = new Pad();
    pads.push_back(std::unique_ptr<Pad>(pad));
    pad->parent = parentPad;
    pad->value = catchSwitch;
    currentPad = pad;
    void* catchPadTarget = emitter->CreateBasicBlock("catchpad");
    emitter->AddHandlerToCatchSwitch(catchSwitch, catchPadTarget);
    emitter->SetCurrentBasicBlock(catchPadTarget);
    std::vector<void*> catchPadArgs;
    catchPadArgs.push_back(emitter->CreateDefaultIrValueForVoidPtrType());
    catchPadArgs.push_back(emitter->CreateIrValueForInt(64));
    catchPadArgs.push_back(emitter->CreateDefaultIrValueForVoidPtrType());
    void* catchPad = emitter->CreateCatchPad(currentPad->value, catchPadArgs);
    currentPad->value = catchPad;
    void* catchTarget = emitter->CreateBasicBlock("catch");
    void* resumeTarget = emitter->CreateBasicBlock("resume");
    emitter->CreateBr(catchTarget);
    int n = boundTryStatement.Catches().size();
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<BoundCatchStatement>& boundCatchStatement = boundTryStatement.Catches()[i];
        emitter->SetCurrentBasicBlock(catchTarget);
        std::vector<void*> handleExceptionParamTypes;
        handleExceptionParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
        void* handleExceptionFunctionType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForBool(), handleExceptionParamTypes);
        std::vector<void*> handleExceptionArgs;
        UuidValue uuidValue(boundCatchStatement->GetSpan(), boundCatchStatement->CatchedTypeUuidId());
        void* catchTypeIdValue = uuidValue.IrValue(*emitter);
        handleExceptionArgs.push_back(catchTypeIdValue);
        void* handleException = emitter->GetOrInsertFunction("RtHandleException", handleExceptionFunctionType, true);
        void* handleThisEx = nullptr;
        if (currentPad == nullptr)
        {
            handleThisEx = emitter->CreateCall(handleException, handleExceptionArgs);
        }
        else
        {
            std::vector<void*> bundles;
            bundles.push_back(currentPad->value);
            handleThisEx = emitter->CreateCallInst(handleException, handleExceptionArgs, bundles, boundCatchStatement->GetSpan());
        }
        void* nextHandlerTarget = nullptr;
        if (i < n - 1)
        {
            catchTarget = emitter->CreateBasicBlock("catch");
            nextHandlerTarget = catchTarget;
        }
        else
        {
            nextHandlerTarget = resumeTarget;
        }
        void* thisHandlerTarget = emitter->CreateBasicBlock("handler");
        emitter->CreateCondBr(handleThisEx, thisHandlerTarget, nextHandlerTarget);
        emitter->SetCurrentBasicBlock(thisHandlerTarget);
        boundCatchStatement->CatchBlock()->Accept(*this);
        emitter->CreateCatchRet(currentPad->value, nextTarget);
    }
    emitter->SetCurrentBasicBlock(resumeTarget);
    std::vector<void*> cxxThrowFunctionParamTypes;
    cxxThrowFunctionParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    cxxThrowFunctionParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    void* cxxThrowFunctionType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), cxxThrowFunctionParamTypes);
    void* cxxThrowFunction = emitter->GetOrInsertFunction("_CxxThrowException", cxxThrowFunctionType, false);
    std::vector<void*> rethrowArgs;
    rethrowArgs.push_back(emitter->CreateDefaultIrValueForVoidPtrType());
    rethrowArgs.push_back(emitter->CreateDefaultIrValueForVoidPtrType());
    std::vector<void*> bundles;
    bundles.push_back(currentPad->value);
    emitter->CreateCallInstToBasicBlock(cxxThrowFunction, rethrowArgs, bundles, resumeTarget, Span());
    emitter->CreateBr(nextTarget);
    currentPad = parentPad;
    emitter->SetCurrentBasicBlock(nextTarget);
    basicBlockOpen = true;
    cleanupBlock = prevCleanupBlock;
}

void WindowsCodeGenerator::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    emitter->SetCurrentDebugLocation(boundRethrowStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundRethrowStatement);
    boundRethrowStatement.ReleaseCall()->Accept(*this);
    if (emitter->DIBuilder())
    {
        emitter->SetCurrentDebugLocation(boundRethrowStatement.GetSpan());
    }
    std::vector<void*> cxxThrowFunctionParamTypes;
    cxxThrowFunctionParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    cxxThrowFunctionParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    void* cxxThrowFunctionType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), cxxThrowFunctionParamTypes);
    void* cxxThrowFunction = emitter->GetOrInsertFunction("_CxxThrowException", cxxThrowFunctionType, false);
    std::vector<void*> rethrowArgs;
    rethrowArgs.push_back(emitter->CreateDefaultIrValueForVoidPtrType());
    rethrowArgs.push_back(emitter->CreateDefaultIrValueForVoidPtrType());
    std::vector<void*> bundles;
    bundles.push_back(currentPad->value);
    void* callInst = emitter->CreateCallInst(cxxThrowFunction, rethrowArgs, bundles, boundRethrowStatement.GetSpan());
}

void WindowsCodeGenerator::CreateCleanup()
{
    cleanupBlock = emitter->CreateBasicBlock("cleanup");
    BoundCompoundStatement* targetBlock = nullptr;
    BoundStatement* parent = currentBlock->Parent();
    while (parent && parent->GetBoundNodeType() != BoundNodeType::boundTryStatement)
    {
        parent = parent->Parent();
    }
    if (parent)
    {
        targetBlock = parent->Block();
    }
    cmajor::codegenllvm::Cleanup* cleanup = new cmajor::codegenllvm::Cleanup(cleanupBlock, handlerBlock, currentPad);
    int n = blocks.size();
    for (int i = n - 1; i >= 0; --i)
    {
        BoundCompoundStatement* block = blocks[i];
        if (block == targetBlock)
        {
            break;
        }
        auto it = blockDestructionMap.find(block);
        if (it != blockDestructionMap.cend())
        {
            std::vector<std::unique_ptr<BoundFunctionCall>>& destructorCallVec = it->second;
            int nd = destructorCallVec.size();
            for (int i = nd - 1; i >= 0; --i)
            {
                std::unique_ptr<BoundFunctionCall>& destructorCall = destructorCallVec[i];
                if (destructorCall)
                {
                    cleanup->destructors.push_back(std::unique_ptr<BoundFunctionCall>(static_cast<BoundFunctionCall*>(destructorCall->Clone())));
                }
            }
        }
    }
    cleanups.push_back(std::unique_ptr<cmajor::codegenllvm::Cleanup>(cleanup));
    newCleanupNeeded = false;
}

void* WindowsCodeGenerator::GetPersonalityFunction() const
{
    void* personalityFunctionType = emitter->GetIrTypeForVariableParamFunction(emitter->GetIrTypeForInt());
    void* personalityFunction = emitter->GetOrInsertFunction("__CxxFrameHandler3", personalityFunctionType, false);
    return personalityFunction;
}

void WindowsCodeGenerator::GenerateCodeForCleanups()
{
    for (const std::unique_ptr<cmajor::codegenllvm::Cleanup>& cleanup : cleanups)
    {
        emitter->SetCurrentBasicBlock(cleanup->cleanupBlock);
        Pad* parentPad = cleanup->currentPad;
        void* cleanupPad = nullptr;
        if (parentPad)
        {
            std::vector<void*> args;
            cleanupPad = emitter->CreateCleanupPadWithParent(parentPad->value, args);
        }
        else
        {
            std::vector<void*> args;
            cleanupPad = emitter->CreateCleanupPad(args);
        }
        Pad pad;
        pad.parent = parentPad;
        pad.value = cleanupPad;
        currentPad = &pad;
        for (const std::unique_ptr<BoundFunctionCall>& destructorCall : cleanup->destructors)
        {
            destructorCall->Accept(*this);
        }
        void* unwindTarget = cleanup->handlerBlock;
        emitter->CreateCleanupRet(cleanupPad, unwindTarget);
    }
}

} } // namespace cmajor::codegenwin

