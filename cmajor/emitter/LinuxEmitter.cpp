// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/emitter/LinuxEmitter.hpp>

namespace cmajor { namespace emitter {

LinuxEmitter::LinuxEmitter(EmittingContext& emittingContext_, const std::string& compileUnitModuleName_, cmajor::symbols::Module& symbolsModule_) : 
    BasicEmitter(emittingContext_, compileUnitModuleName_, symbolsModule_)
{
}

llvm::Function* LinuxEmitter::GetPersonalityFunction() const
{
    llvm::FunctionType* personalityFunctionType = llvm::FunctionType::get(builder.getInt32Ty(), true);
    llvm::Function* personalityFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__gxx_personality_v0", personalityFunctionType));
    return personalityFunction;
}

void LinuxEmitter::Visit(BoundReturnStatement& boundReturnStatement)
{
    SetCurrentDebugLocation(boundReturnStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundReturnStatement);
    BoundFunctionCall* returnFunctionCall = boundReturnStatement.ReturnFunctionCall();
    if (returnFunctionCall)
    {
        returnFunctionCall->Accept(*this);
        llvm::Value* returnValue = stack.Pop();
        if (sequenceSecond)
        {
            sequenceSecond->SetGenerated();
            sequenceSecond->Accept(*this);
        }
        ExitBlocks(nullptr);
        CreateExitFunctionCall();
        builder.CreateRet(returnValue);
        lastInstructionWasRet = true;
    }
    else
    {
        ExitBlocks(nullptr);
        CreateExitFunctionCall();
        builder.CreateRetVoid();
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
        llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
        SetCurrentBasicBlock(nextBlock);
        basicBlockOpen = true;
        lastInstructionWasRet = false;
    }
}

void LinuxEmitter::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    SetCurrentDebugLocation(boundGotoCaseStatement.GetSpan());
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
        llvm::BasicBlock* caseDest = it->second;
        builder.CreateBr(caseDest);
    }
    else
    {
        throw Exception(&symbolsModule, "case not found", boundGotoCaseStatement.GetSpan());
    }
}

void LinuxEmitter::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    SetCurrentDebugLocation(boundGotoDefaultStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoDefaultStatement);
    Assert(breakTargetBlock, "break target not set");
    ExitBlocks(breakTargetBlock);
    if (defaultDest)
    {
        builder.CreateBr(defaultDest);
    }
    else
    {
        throw Exception(&symbolsModule, "no default destination", boundGotoDefaultStatement.GetSpan());
    }
}

void LinuxEmitter::Visit(BoundBreakStatement& boundBreakStatement)
{
    SetCurrentDebugLocation(boundBreakStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundBreakStatement);
    Assert(breakTarget && breakTargetBlock, "break target not set");
    ExitBlocks(breakTargetBlock);
    builder.CreateBr(breakTarget);
    if (!currentCaseMap) // not in switch
    {
        llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
        SetCurrentBasicBlock(nextBlock);
        basicBlockOpen = true;
    }
}

void LinuxEmitter::Visit(BoundContinueStatement& boundContinueStatement)
{
    SetCurrentDebugLocation(boundContinueStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundContinueStatement);
    Assert(continueTarget && continueTargetBlock, "continue target not set");
    ExitBlocks(continueTargetBlock);
    builder.CreateBr(continueTarget);
    llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
    SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void LinuxEmitter::Visit(BoundGotoStatement& boundGotoStatement)
{
    SetCurrentDebugLocation(boundGotoStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundGotoStatement);
    ExitBlocks(boundGotoStatement.TargetBlock());
    auto it = labeledStatementMap.find(boundGotoStatement.TargetStatement());
    if (it != labeledStatementMap.cend())
    {
        llvm::BasicBlock* target = it->second;
        builder.CreateBr(target);
    }
    else
    {
        throw Exception(&symbolsModule, "goto target not found", boundGotoStatement.GetSpan());
    }
    llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
    SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void LinuxEmitter::Visit(BoundTryStatement& boundTryStatement)
{
    ClearFlags();
    SetTarget(&boundTryStatement);
    llvm::BasicBlock* prevHandlerBlock = handlerBlock;
    llvm::BasicBlock* prevCleanupBlock = cleanupBlock;
    handlerBlock = llvm::BasicBlock::Create(context, "lpad", function);
    cleanupBlock = nullptr;
    boundTryStatement.TryBlock()->Accept(*this);
    llvm::BasicBlock* nextTarget = llvm::BasicBlock::Create(context, "next", function);
    builder.CreateBr(nextTarget);
    SetCurrentBasicBlock(handlerBlock);
    handlerBlock = prevHandlerBlock;
    std::vector<llvm::Type*> lpElemTypes;
    lpElemTypes.push_back(builder.getInt8PtrTy());
    lpElemTypes.push_back(builder.getInt32Ty());
    llvm::StructType* lpType = llvm::StructType::get(context, lpElemTypes);
    llvm::LandingPadInst* lp = builder.CreateLandingPad(lpType, 1);
    std::vector<llvm::Type*> exTypeInfoElemTypes;
    exTypeInfoElemTypes.push_back(builder.getInt8PtrTy());
    exTypeInfoElemTypes.push_back(builder.getInt8PtrTy());
    llvm::StructType* exTypeInfoType = llvm::StructType::get(context, exTypeInfoElemTypes);
    llvm::Constant* exceptionTypeId = compileUnitModule->getOrInsertGlobal("_ZTIN6cmajor2eh9ExceptionE", exTypeInfoType);
    lp->addClause(llvm::cast<llvm::Constant>(builder.CreateBitCast(exceptionTypeId, builder.getInt8PtrTy())));
    std::vector<unsigned int> exPtrIndex;
    exPtrIndex.push_back(0);
    llvm::Value* exPtr = builder.CreateExtractValue(lp, exPtrIndex);
    llvm::AllocaInst* exPtrAlloca = new llvm::AllocaInst(builder.getInt8PtrTy(), 0);
    InsertAllocaIntoEntryBlock(exPtrAlloca);
    builder.CreateStore(exPtr, exPtrAlloca);
    std::vector<unsigned int> exIdIndex;
    exIdIndex.push_back(1);
    llvm::Value* exId = builder.CreateExtractValue(lp, exIdIndex);
    llvm::AllocaInst* exIdAlloca = new llvm::AllocaInst(builder.getInt32Ty(), 0);
    builder.CreateStore(exId, exIdAlloca);
    InsertAllocaIntoEntryBlock(exIdAlloca);
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
        SetCurrentBasicBlock(thisHandlerTarget);
        boundCatchStatement->CatchBlock()->Accept(*this);
        std::vector<llvm::Type*> cxaEndCatchParamTypes;
        llvm::FunctionType* cxaEndCatchType = llvm::FunctionType::get(builder.getVoidTy(), cxaEndCatchParamTypes, false);
        llvm::Function* cxaEndCatch = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__cxa_end_catch", cxaEndCatchType));
        ArgVector cxaEndCatchArgs;
        llvm::Value* cxaEndCatchValue = builder.CreateCall(cxaEndCatch, cxaEndCatchArgs);
        builder.CreateBr(nextTarget);
    }
    SetCurrentBasicBlock(resumeBlock);
    llvm::Value* loadedExPtrForResume = builder.CreateLoad(exPtrAlloca);
    llvm::Value* loadedExIdForResume = builder.CreateLoad(exIdAlloca);
    llvm::Value* resume1 = builder.CreateInsertValue(llvm::UndefValue::get(lpType), loadedExPtrForResume, exPtrIndex);
    llvm::Value* resume2 = builder.CreateInsertValue(resume1, loadedExIdForResume, exIdIndex);
    builder.CreateResume(resume2);
    SetCurrentBasicBlock(nextTarget);
    basicBlockOpen = true;
}

void LinuxEmitter::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    SetCurrentDebugLocation(boundRethrowStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundRethrowStatement);
    boundRethrowStatement.ReleaseCall()->Accept(*this);
    llvm::FunctionType* rethrowFunctionType = llvm::FunctionType::get(builder.getVoidTy(), false);
    llvm::Function* cxaRethrowFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__cxa_rethrow", rethrowFunctionType,  nullptr));
    ArgVector rethrowArgs;
    builder.CreateCall(cxaRethrowFunction, rethrowArgs);
}

void LinuxEmitter::CreateCleanup()
{
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
}

void LinuxEmitter::GenerateCodeForCleanups()
{
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
}

} } // namespace cmajor::emitter
