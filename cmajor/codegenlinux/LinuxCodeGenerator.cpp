// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegenlinux/LinuxCodeGenerator.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace codegenlinux {

LinuxCodeGenerator::LinuxCodeGenerator(cmajor::ir::EmittingContext& emittingContext_) : LlvmCodeGenerator(emittingContext_)
{
}

void LinuxCodeGenerator::Visit(BoundReturnStatement& boundReturnStatement)
{
    emitter->SetCurrentDebugLocation(boundReturnStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundReturnStatement);
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
}

void LinuxCodeGenerator::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    emitter->SetCurrentDebugLocation(boundGotoCaseStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoCaseStatement);
    Assert(breakTargetBlock, "break target not set");
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
}

void LinuxCodeGenerator::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    emitter->SetCurrentDebugLocation(boundGotoDefaultStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoDefaultStatement);
    Assert(breakTargetBlock, "break target not set");
    ExitBlocks(breakTargetBlock);
    if (defaultDest)
    {
        emitter->CreateBr(defaultDest);
    }
    else
    {
        throw Exception(symbolsModule, "no default destination", boundGotoDefaultStatement.GetSpan());
    }
}

void LinuxCodeGenerator::Visit(BoundBreakStatement& boundBreakStatement)
{
    emitter->SetCurrentDebugLocation(boundBreakStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundBreakStatement);
    Assert(breakTarget && breakTargetBlock, "break target not set");
    ExitBlocks(breakTargetBlock);
    emitter->CreateBr(breakTarget);
    void* nextBlock = emitter->CreateBasicBlock("next");
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void LinuxCodeGenerator::Visit(BoundContinueStatement& boundContinueStatement)
{
    emitter->SetCurrentDebugLocation(boundContinueStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundContinueStatement);
    Assert(continueTarget && continueTargetBlock, "continue target not set");
    ExitBlocks(continueTargetBlock);
    emitter->CreateBr(continueTarget);
    void* nextBlock = emitter->CreateBasicBlock("next");
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void LinuxCodeGenerator::Visit(BoundGotoStatement& boundGotoStatement)
{
    emitter->SetCurrentDebugLocation(boundGotoStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoStatement);
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
}

void LinuxCodeGenerator::Visit(BoundTryStatement& boundTryStatement)
{
    ClearFlags();
    SetTarget(&boundTryStatement);
    void* prevHandlerBlock = handlerBlock;
    void* prevCleanupBlock = cleanupBlock;
    handlerBlock = emitter->CreateBasicBlock("lpad");
    cleanupBlock = nullptr;
    boundTryStatement.TryBlock()->Accept(*this);
    void* nextTarget = emitter->CreateBasicBlock("next");
    emitter->CreateBr(nextTarget);
    emitter->SetCurrentBasicBlock(handlerBlock);
    handlerBlock = prevHandlerBlock;
    std::vector<void*> lpElemTypes;
    lpElemTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    lpElemTypes.push_back(emitter->GetIrTypeForInt());
    void* lpType = emitter->GetIrTypeForClassType(lpElemTypes);
    void* lp = emitter->CreateLandingPad(lpType);
    std::vector<void*> exTypeInfoElemTypes;
    exTypeInfoElemTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    exTypeInfoElemTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    void* exTypeInfoType = emitter->GetIrTypeForClassType(exTypeInfoElemTypes);
    void* exceptionTypeId = emitter->GetOrInsertGlobal("_ZTIN6cmajor2eh9ExceptionE", exTypeInfoType);
    emitter->AddClauseToLangdingPad(lp, exceptionTypeId);
    std::vector<unsigned int> exPtrIndex;
    exPtrIndex.push_back(0);
    void* exPtrAlloca = emitter->NewAllocaInst(emitter->GetIrTypeForVoidPtrType());
    InsertAllocaIntoEntryBlock(exPtrAlloca);
    void* exPtr = emitter->CreateExtractValue(lp, exPtrIndex);
    emitter->CreateStore(exPtr, exPtrAlloca);
    std::vector<unsigned int> exIdIndex;
    exIdIndex.push_back(1);
    void* exId = emitter->CreateExtractValue(lp, exIdIndex);
    void* exIdAlloca = emitter->NewAllocaInst(emitter->GetIrTypeForInt());
    emitter->CreateStore(exId, exIdAlloca);
    InsertAllocaIntoEntryBlock(exIdAlloca);
    void* testBlock = emitter->CreateBasicBlock("test");
    emitter->CreateBr(testBlock);
    emitter->SetCurrentBasicBlock(testBlock);
    void* loadedExId = emitter->CreateLoad(exIdAlloca);
    std::vector<void*> llvmEhParamTypes;
    llvmEhParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    void* llvmEHTypeIdForType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForInt(), llvmEhParamTypes);
    void* llvmEHTypeIdFor = emitter->GetOrInsertFunction("llvm.eh.typeid.for", llvmEHTypeIdForType, true);
    std::vector<void*> llvmEHTypeIdForArgs;
    llvmEHTypeIdForArgs.push_back(emitter->CreateBitCast(exceptionTypeId, emitter->GetIrTypeForVoidPtrType()));
    void* ehSelector = emitter->CreateCall(llvmEHTypeIdFor, llvmEHTypeIdForArgs);
    void* match = emitter->CreateICmpEQ(loadedExId, ehSelector);
    void* beginCatchBlock = emitter->CreateBasicBlock("handlers");
    void* resumeBlock = emitter->CreateBasicBlock("resume");
    emitter->CreateCondBr(match, beginCatchBlock, resumeBlock);
    emitter->SetCurrentBasicBlock(beginCatchBlock);
    std::vector<void*> cxaBeginCatchParamTypes;
    cxaBeginCatchParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    void* cxaBeginCatchType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoidPtrType(), cxaBeginCatchParamTypes);
    void* cxaBeginCatch = emitter->GetOrInsertFunction("__cxa_begin_catch", cxaBeginCatchType, true);
    std::vector<void*> cxaBeginCatchArgs;
    void* loadedExPtr = emitter->CreateLoad(exPtrAlloca);
    cxaBeginCatchArgs.push_back(loadedExPtr);
    void* cxaBeginCatchValue = emitter->CreateCall(cxaBeginCatch, cxaBeginCatchArgs);
    void* catchTarget = emitter->CreateBasicBlock("catch");
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
        void* handleThisEx = emitter->CreateCall(handleException, handleExceptionArgs);
        void* nextHandlerTarget = nullptr;
        if (i < n - 1)
        {
            catchTarget = emitter->CreateBasicBlock("catch");
            nextHandlerTarget = catchTarget;
        }
        else
        {
            nextHandlerTarget = resumeBlock;
        }
        void* thisHandlerTarget = emitter->CreateBasicBlock("handler");
        emitter->CreateCondBr(handleThisEx, thisHandlerTarget, nextHandlerTarget);
        emitter->SetCurrentBasicBlock(thisHandlerTarget);
        boundCatchStatement->CatchBlock()->Accept(*this);
        std::vector<void*> cxaEndCatchParamTypes;
        void* cxaEndCatchType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), cxaEndCatchParamTypes);
        void* cxaEndCatch = emitter->GetOrInsertFunction("__cxa_end_catch", cxaEndCatchType, true);
        std::vector<void*> cxaEndCatchArgs;
        void* cxaEndCatchValue = emitter->CreateCall(cxaEndCatch, cxaEndCatchArgs);
        emitter->CreateBr(nextTarget);
    }
    emitter->SetCurrentBasicBlock(resumeBlock);
    void* loadedExPtrForResume = emitter->CreateLoad(exPtrAlloca);
    void* loadedExIdForResume = emitter->CreateLoad(exIdAlloca);
    void* resume1 = emitter->CreateInsertValue(emitter->CreateUndefValue(lpType), loadedExPtrForResume, exPtrIndex);
    void* resume2 = emitter->CreateInsertValue(resume1, loadedExIdForResume, exIdIndex);
    emitter->CreateResume(resume2);
    emitter->SetCurrentBasicBlock(nextTarget);
    basicBlockOpen = true;
}

void LinuxCodeGenerator::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    emitter->SetCurrentDebugLocation(boundRethrowStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundRethrowStatement);
    boundRethrowStatement.ReleaseCall()->Accept(*this);
    void* rethrowFunctionType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), std::vector<void*>());
    void* cxaRethrowFunction = emitter->GetOrInsertFunction("__cxa_rethrow", rethrowFunctionType, false);
    std::vector<void*> rethrowArgs;
    emitter->CreateCall(cxaRethrowFunction, rethrowArgs);
}

void LinuxCodeGenerator::CreateCleanup()
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

void* LinuxCodeGenerator::GetPersonalityFunction() const
{
    void* personalityFunctionType = emitter->GetIrTypeForVariableParamFunction(emitter->GetIrTypeForInt());
    void* personalityFunction = emitter->GetOrInsertFunction("__gxx_personality_v0", personalityFunctionType, true);
    return personalityFunction;
}

void LinuxCodeGenerator::GenerateCodeForCleanups()
{
    for (const std::unique_ptr<cmajor::codegenllvm::Cleanup>& cleanup : cleanups)
    {
        emitter->SetCurrentBasicBlock(cleanup->cleanupBlock);
        std::vector<void*> lpElemTypes;
        lpElemTypes.push_back(emitter->GetIrTypeForVoidPtrType());
        lpElemTypes.push_back(emitter->GetIrTypeForInt());
        void* lpType = emitter->GetIrTypeForClassType(lpElemTypes);
        void* lp = emitter->CreateLandingPad(lpType);
        emitter->SetLandindPadAsCleanup(lp);
        for (const std::unique_ptr<BoundFunctionCall>& destructorCall : cleanup->destructors)
        {
            destructorCall->Accept(*this);
        }
        emitter->CreateResume(lp);
    }
}

} } // namespace cmajor::codegenlinux
