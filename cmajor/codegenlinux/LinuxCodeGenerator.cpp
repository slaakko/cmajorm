// =================================
// Copyright (c) 2019 Seppo Laakko
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
        CreateExitFunctionCall();
        emitter->CreateRet(returnValue);
        lastInstructionWasRet = true;
    }
    else
    {
        ExitBlocks(nullptr);
        CreateExitFunctionCall();
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
    /*
    std::vector<llvm::Type*> lpElemTypes;
    lpElemTypes.push_back(builder.getInt8PtrTy());
    lpElemTypes.push_back(builder.getInt32Ty());
    llvm::StructType* lpType = llvm::StructType::get(context, lpElemTypes);
    llvm::LandingPadInst* lp = builder.CreateLandingPad(lpType, 1);
    */

    std::vector<void*> exTypeInfoElemTypes;
    exTypeInfoElemTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    exTypeInfoElemTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    void* exTypeInfoType = emitter->GetIrTypeForClassType(exTypeInfoElemTypes);
    void* exceptionTypeId = emitter->GetOrInsertGlobal("_ZTIN6cmajor2eh9ExceptionE", exTypeInfoType);
    emitter->AddClauseToLangdingPad(lp, exceptionTypeId);

    /*
    std::vector<llvm::Type*> exTypeInfoElemTypes;
    exTypeInfoElemTypes.push_back(builder.getInt8PtrTy());
    exTypeInfoElemTypes.push_back(builder.getInt8PtrTy());
    llvm::StructType* exTypeInfoType = llvm::StructType::get(context, exTypeInfoElemTypes);
    llvm::Constant* exceptionTypeId = compileUnitModule->getOrInsertGlobal("_ZTIN6cmajor2eh9ExceptionE", exTypeInfoType);
    lp->addClause(llvm::cast<llvm::Constant>(builder.CreateBitCast(exceptionTypeId, builder.getInt8PtrTy())));
    */

    std::vector<unsigned int> exPtrIndex;
    exPtrIndex.push_back(0);
    void* exPtrAlloca = emitter->CreateAlloca(emitter->GetIrTypeForVoidPtrType());
    InsertAllocaIntoEntryBlock(exPtrAlloca);
    void* exPtr = emitter->CreateExtractValue(lp, exPtrIndex);
    emitter->CreateStore(exPtr, exPtrAlloca);

/*
    std::vector<unsigned int> exPtrIndex;
    exPtrIndex.push_back(0);
    llvm::AllocaInst* exPtrAlloca = new llvm::AllocaInst(builder.getInt8PtrTy(), 0);
    InsertAllocaIntoEntryBlock(exPtrAlloca);
    llvm::Value* exPtr = builder.CreateExtractValue(lp, exPtrIndex);
    builder.CreateStore(exPtr, exPtrAlloca);
*/

    std::vector<unsigned int> exIdIndex;
    exIdIndex.push_back(1);
    void* exId = emitter->CreateExtractValue(lp, exIdIndex);
    void* exIdAlloca = emitter->CreateAlloca(emitter->GetIrTypeForVoidPtrType());
    emitter->CreateStore(exId, exIdAlloca);
    InsertAllocaIntoEntryBlock(exIdAlloca);

/*
    std::vector<unsigned int> exIdIndex;
    exIdIndex.push_back(1);
    llvm::Value* exId = builder.CreateExtractValue(lp, exIdIndex);
    llvm::AllocaInst* exIdAlloca = new llvm::AllocaInst(builder.getInt32Ty(), 0);
    builder.CreateStore(exId, exIdAlloca);
    InsertAllocaIntoEntryBlock(exIdAlloca);
*/

    void* testBlock = emitter->CreateBasicBlock("test");
    emitter->CreateBr(testBlock);
    emitter->SetCurrentBasicBlock(testBlock);
    void* loadedExId = emitter->CreateLoad(exIdAlloca);
    std::vector<void*> llvmEhParamTypes;
    llvmEhParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
    void* llvmEHTypeIdForType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForInt(), llvmEhParamTypes);
    void* llvmEHTypeIdFor = emitter->GetOrInsertFunction("llvm.eh.typeid.for", llvmEHTypeIdForType);
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
    void* cxaBeginCatch = emitter->GetOrInsertFunction("__cxa_begin_catch", cxaBeginCatchType);
    std::vector<void*> cxaBeginCatchArgs;
    void* loadedExPtr = emitter->CreateLoad(exPtrAlloca);
    cxaBeginCatchArgs.push_back(loadedExPtr);
    void* cxaBeginCatchValue = emitter->CreateCall(cxaBeginCatch, cxaBeginCatchArgs);
    void* catchTarget = emitter->CreateBasicBlock("catch");
    emitter->CreateBr(catchTarget);
/*
    llvm::BasicBlock* testBlock = llvm::BasicBlock::Create(context, "test", function);
    builder.CreateBr(testBlock);
    SetCurrentBasicBlock(testBlock);
    llvm::Value* loadedExId = builder.CreateLoad(exIdAlloca);
    std::vector<llvm::Type*> llvmEhParamTypes;
    llvmEhParamTypes.push_back(builder.getInt8PtrTy());
    llvm::FunctionType* llvmEHTypeIdForType = llvm::FunctionType::get(builder.getInt32Ty(), llvmEhParamTypes, false);
    llvm::Function* llvmEHTypeIdFor = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("llvm.eh.typeid.for", llvmEHTypeIdForType));
    ArgVector llvmEHTypeIdForArgs;
    llvmEHTypeIdForArgs.push_back(llvm::cast<llvm::Constant>(builder.CreateBitCast(exceptionTypeId, builder.getInt8PtrTy())));
    llvm::Value* ehSelector = builder.CreateCall(llvmEHTypeIdFor, llvmEHTypeIdForArgs);
    llvm::Value* match = builder.CreateICmpEQ(loadedExId, ehSelector);
    llvm::BasicBlock* beginCatchBlock = llvm::BasicBlock::Create(context, "handlers", function);
    llvm::BasicBlock* resumeBlock = llvm::BasicBlock::Create(context, "resume", function);
    builder.CreateCondBr(match, beginCatchBlock, resumeBlock);
    SetCurrentBasicBlock(beginCatchBlock);
    std::vector<llvm::Type*> cxaBeginCatchParamTypes;
    cxaBeginCatchParamTypes.push_back(builder.getInt8PtrTy());
    llvm::FunctionType* cxaBeginCatchType = llvm::FunctionType::get(builder.getInt8PtrTy(), cxaBeginCatchParamTypes, false);
    llvm::Function* cxaBeginCatch = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__cxa_begin_catch", cxaBeginCatchType));
    ArgVector cxaBeginCatchArgs;
    llvm::Value* loadedExPtr = builder.CreateLoad(exPtrAlloca);
    cxaBeginCatchArgs.push_back(loadedExPtr);
    llvm::Value* cxaBeginCatchValue = builder.CreateCall(cxaBeginCatch, cxaBeginCatchArgs);
    llvm::BasicBlock* catchTarget = llvm::BasicBlock::Create(context, "catch", function);
    builder.CreateBr(catchTarget);
*/
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
        void* handleException = emitter->GetOrInsertFunction("RtHandleException", handleExceptionFunctionType);
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

        /*
            int n = boundTryStatement.Catches().size();
            for (int i = 0; i < n; ++i)
            {
                const std::unique_ptr<BoundCatchStatement>& boundCatchStatement = boundTryStatement.Catches()[i];
                SetCurrentBasicBlock(catchTarget);
                std::vector<llvm::Type*> handleExceptionParamTypes;
                handleExceptionParamTypes.push_back(builder.getInt8PtrTy());
                llvm::FunctionType* handleExceptionFunctionType = llvm::FunctionType::get(builder.getInt1Ty(), handleExceptionParamTypes, false);
                ArgVector handleExceptionArgs;
                UuidValue uuidValue(boundCatchStatement->GetSpan(), boundCatchStatement->CatchedTypeUuidId());
                llvm::Value* catchTypeIdValue = uuidValue.IrValue(*this);
                handleExceptionArgs.push_back(catchTypeIdValue);
                llvm::Function* handleException = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("RtHandleException", handleExceptionFunctionType));
                llvm::Value* handleThisEx = builder.CreateCall(handleException, handleExceptionArgs);
                llvm::BasicBlock* nextHandlerTarget = nullptr;
                if (i < n - 1)
                {
                    catchTarget = llvm::BasicBlock::Create(context, "catch", function);
                    nextHandlerTarget = catchTarget;
                }
                else
                {
                    nextHandlerTarget = resumeBlock;
                }
                llvm::BasicBlock* thisHandlerTarget = llvm::BasicBlock::Create(context, "handler", function);
                builder.CreateCondBr(handleThisEx, thisHandlerTarget, nextHandlerTarget);
        */

        emitter->SetCurrentBasicBlock(thisHandlerTarget);
        boundCatchStatement->CatchBlock()->Accept(*this);
        std::vector<void*> cxaEndCatchParamTypes;
        void* cxaEndCatchType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), cxaEndCatchParamTypes);
        void* cxaEndCatch = emitter->GetOrInsertFunction("__cxa_end_catch", cxaEndCatchType);
        std::vector<void*> cxaEndCatchArgs;
        void* cxaEndCatchValue = emitter->CreateCall(cxaEndCatch, cxaEndCatchArgs);
        emitter->CreateBr(nextTarget);
/*
        SetCurrentBasicBlock(thisHandlerTarget);
        boundCatchStatement->CatchBlock()->Accept(*this);
        std::vector<llvm::Type*> cxaEndCatchParamTypes;
        llvm::FunctionType* cxaEndCatchType = llvm::FunctionType::get(builder.getVoidTy(), cxaEndCatchParamTypes, false);
        llvm::Function* cxaEndCatch = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__cxa_end_catch", cxaEndCatchType));
        ArgVector cxaEndCatchArgs;
        llvm::Value* cxaEndCatchValue = builder.CreateCall(cxaEndCatch, cxaEndCatchArgs);
        builder.CreateBr(nextTarget);
    }
*/
    }
    emitter->SetCurrentBasicBlock(resumeBlock);
    void* loadedExPtrForResume = emitter->CreateLoad(exPtrAlloca);
    void* loadedExIdForResume = emitter->CreateLoad(exIdAlloca);
    void* resume1 = emitter->CreateInsertValue(emitter->CreateUndefValue(lpType), loadedExPtrForResume, exPtrIndex);
    void* resume2 = emitter->CreateInsertValue(resume1, loadedExIdForResume, exIdIndex);
    emitter->CreateResume(resume2);
    emitter->SetCurrentBasicBlock(nextTarget);
    basicBlockOpen = true;

/*
    SetCurrentBasicBlock(resumeBlock);
    llvm::Value* loadedExPtrForResume = builder.CreateLoad(exPtrAlloca);
    llvm::Value* loadedExIdForResume = builder.CreateLoad(exIdAlloca);
    llvm::Value* resume1 = builder.CreateInsertValue(llvm::UndefValue::get(lpType), loadedExPtrForResume, exPtrIndex);
    llvm::Value* resume2 = builder.CreateInsertValue(resume1, loadedExIdForResume, exIdIndex);
    builder.CreateResume(resume2);
    SetCurrentBasicBlock(nextTarget);
    basicBlockOpen = true;
*/
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
    void* cxaRethrowFunction = emitter->GetOrInsertFunction("__cxa_rethrow", rethrowFunctionType);
    std::vector<void*> rethrowArgs;
    emitter->CreateCall(cxaRethrowFunction, rethrowArgs);
/*
    llvm::FunctionType* rethrowFunctionType = llvm::FunctionType::get(builder.getVoidTy(), false);
    llvm::Function* cxaRethrowFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__cxa_rethrow", rethrowFunctionType, nullptr));
    ArgVector rethrowArgs;
    builder.CreateCall(cxaRethrowFunction, rethrowArgs);
*/
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
    /*
    cleanupBlock = llvm::BasicBlock::Create(context, "cleanup", function);
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
    Cleanup* cleanup = new Cleanup(cleanupBlock, handlerBlock, currentPad);
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
    cleanups.push_back(std::unique_ptr<Cleanup>(cleanup));
    newCleanupNeeded = false;
*/
}

void* LinuxCodeGenerator::GetPersonalityFunction() const
{
    void* personalityFunctionType = emitter->GetIrTypeForVariableParamFunction(emitter->GetIrTypeForInt());
    void* personalityFunction = emitter->GetOrInsertFunction("__gxx_personality_v0", personalityFunctionType);
    return personalityFunction;
/*
    llvm::FunctionType* personalityFunctionType = llvm::FunctionType::get(builder.getInt32Ty(), true);
    llvm::Function* personalityFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__gxx_personality_v0", personalityFunctionType));
    return personalityFunction;
*/
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
/*
    for (const std::unique_ptr<Cleanup>& cleanup : cleanups)
    {
        SetCurrentBasicBlock(cleanup->cleanupBlock);
        std::vector<llvm::Type*> lpElemTypes;
        lpElemTypes.push_back(builder.getInt8PtrTy());
        lpElemTypes.push_back(builder.getInt32Ty());
        llvm::StructType* lpType = llvm::StructType::get(context, lpElemTypes);
        llvm::LandingPadInst* lp = builder.CreateLandingPad(lpType, 0);
        lp->setCleanup(true);
        for (const std::unique_ptr<BoundFunctionCall>& destructorCall : cleanup->destructors)
        {
            destructorCall->Accept(*this);
        }
        builder.CreateResume(lp);
    }
*/
}

} } // namespace cmajor::codegenlinux
