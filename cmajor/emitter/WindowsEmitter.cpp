// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/emitter/WindowsEmitter.hpp>

namespace cmajor { namespace emitter {

WindowsEmitter::WindowsEmitter(EmittingContext& emittingContext_, const std::string& compileUnitModuleName_, cmajor::symbols::Module& symbolsModule_) : 
    BasicEmitter(emittingContext_, compileUnitModuleName_, symbolsModule_)
{
}

llvm::Function* WindowsEmitter::GetPersonalityFunction() const
{
    llvm::FunctionType* personalityFunctionType = llvm::FunctionType::get(builder.getInt32Ty(), true);
    llvm::Function* personalityFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("__CxxFrameHandler3", personalityFunctionType));
    return personalityFunction;
}

void WindowsEmitter::Visit(BoundReturnStatement& boundReturnStatement)
{
    SetCurrentDebugLocation(boundReturnStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundReturnStatement);
    Pad* prevCurrentPad = currentPad;
    while (currentPad != nullptr)
    {
        llvm::BasicBlock* returnTarget = llvm::BasicBlock::Create(context, "return", function);
        builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(currentPad->value), returnTarget);
        SetCurrentBasicBlock(returnTarget);
        currentPad = currentPad->parent;
    }
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
    currentPad = prevCurrentPad;
}

void WindowsEmitter::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    SetCurrentDebugLocation(boundGotoCaseStatement.GetSpan());
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
            llvm::BasicBlock* fromCatchTarget = llvm::BasicBlock::Create(context, "fromCatch", function);
            builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(currentPad->value), fromCatchTarget);
            SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
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
    currentPad = prevCurrentPad;
}

void WindowsEmitter::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    SetCurrentDebugLocation(boundGotoDefaultStatement.GetSpan());
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
            llvm::BasicBlock* fromCatchTarget = llvm::BasicBlock::Create(context, "fromCatch", function);
            builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(currentPad->value), fromCatchTarget);
            SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(breakTargetBlock);
    if (defaultDest)
    {
        builder.CreateBr(defaultDest);
    }
    else
    {
        throw Exception(&symbolsModule, "no default destination", boundGotoDefaultStatement.GetSpan());
    }
    currentPad = prevCurrentPad;
}

void WindowsEmitter::Visit(BoundBreakStatement& boundBreakStatement)
{
    SetCurrentDebugLocation(boundBreakStatement.GetSpan());
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
            llvm::BasicBlock* fromCatchTarget = llvm::BasicBlock::Create(context, "fromCatch", function);
            builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(currentPad->value), fromCatchTarget);
            SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(breakTargetBlock);
    builder.CreateBr(breakTarget);
    if (!currentCaseMap) // not in switch
    {
        llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
        SetCurrentBasicBlock(nextBlock);
        basicBlockOpen = true;
    }
    currentPad = prevCurrentPad;
}

void WindowsEmitter::Visit(BoundContinueStatement& boundContinueStatement)
{
    SetCurrentDebugLocation(boundContinueStatement.GetSpan());
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
            llvm::BasicBlock* fromCatchTarget = llvm::BasicBlock::Create(context, "fromCatch", function);
            builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(currentPad->value), fromCatchTarget);
            SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
    ExitBlocks(continueTargetBlock);
    builder.CreateBr(continueTarget);
    llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
    SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
    currentPad = prevCurrentPad;
}

void WindowsEmitter::Visit(BoundGotoStatement& boundGotoStatement)
{
    SetCurrentDebugLocation(boundGotoStatement.GetSpan());
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
            llvm::BasicBlock* fromCatchTarget = llvm::BasicBlock::Create(context, "fromCatch", function);
            builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(currentPad->value), fromCatchTarget);
            SetCurrentBasicBlock(fromCatchTarget);
            currentPad = currentPad->parent;
        }
        parent = parent->Parent();
    }
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
    currentPad = prevCurrentPad;
}

void WindowsEmitter::Visit(BoundTryStatement& boundTryStatement)
{
    ClearFlags();
    SetTarget(&boundTryStatement);
    llvm::BasicBlock* prevHandlerBlock = handlerBlock;
    llvm::BasicBlock* prevCleanupBlock = cleanupBlock;
    handlerBlock = llvm::BasicBlock::Create(context, "handlers", function);
    cleanupBlock = nullptr;
    boundTryStatement.TryBlock()->Accept(*this);
    llvm::BasicBlock* nextTarget = llvm::BasicBlock::Create(context, "next", function);
    builder.CreateBr(nextTarget);
    SetCurrentBasicBlock(handlerBlock);
    handlerBlock = prevHandlerBlock;
    Pad* parentPad = currentPad;
    llvm::CatchSwitchInst* catchSwitch = nullptr;
    if (parentPad == nullptr)
    {
        catchSwitch = builder.CreateCatchSwitch(llvm::ConstantTokenNone::get(context), prevHandlerBlock, 1);
    }
    else
    {
        catchSwitch = builder.CreateCatchSwitch(parentPad->value, prevHandlerBlock, 1);
    }
    Pad* pad = new Pad();
    pads.push_back(std::unique_ptr<Pad>(pad));
    pad->parent = parentPad;
    pad->value = catchSwitch;
    currentPad = pad;
    llvm::BasicBlock* catchPadTarget = llvm::BasicBlock::Create(context, "catchpad", function);
    catchSwitch->addHandler(catchPadTarget);
    SetCurrentBasicBlock(catchPadTarget);
    ArgVector catchPadArgs;
    catchPadArgs.push_back(llvm::Constant::getNullValue(builder.getInt8PtrTy()));
    catchPadArgs.push_back(builder.getInt32(64));
    catchPadArgs.push_back(llvm::Constant::getNullValue(builder.getInt8PtrTy()));
    llvm::CatchPadInst* catchPad = builder.CreateCatchPad(currentPad->value, catchPadArgs);
    currentPad->value = catchPad;
    llvm::BasicBlock* catchTarget = llvm::BasicBlock::Create(context, "catch", function);
    llvm::BasicBlock* resumeTarget = llvm::BasicBlock::Create(context, "resume", function);
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
        llvm::Value* handleThisEx = nullptr;
        if (currentPad == nullptr)
        {
            handleThisEx = builder.CreateCall(handleException, handleExceptionArgs);
        }
        else
        {
            std::vector<llvm::OperandBundleDef> bundles;
            std::vector<llvm::Value*> inputs;
            inputs.push_back(currentPad->value);
            bundles.push_back(llvm::OperandBundleDef("funclet", inputs));
            handleThisEx = llvm::CallInst::Create(handleException, handleExceptionArgs, bundles, "", CurrentBasicBlock());
            if (diBuilder)
            {
                llvm::cast<llvm::CallInst>(handleThisEx)->setDebugLoc(GetDebugLocation(boundCatchStatement->GetSpan()));
            }
        }
        llvm::BasicBlock* nextHandlerTarget = nullptr;
        if (i < n - 1)
        {
            catchTarget = llvm::BasicBlock::Create(context, "catch", function);
            nextHandlerTarget = catchTarget;
        }
        else
        {
            nextHandlerTarget = resumeTarget;
        }
        llvm::BasicBlock* thisHandlerTarget = llvm::BasicBlock::Create(context, "handler", function);
        builder.CreateCondBr(handleThisEx, thisHandlerTarget, nextHandlerTarget);
        SetCurrentBasicBlock(thisHandlerTarget);
        boundCatchStatement->CatchBlock()->Accept(*this);
        builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(currentPad->value), nextTarget);
    }
    SetCurrentBasicBlock(resumeTarget);

    std::vector<llvm::Type*> cxxThrowFunctionParamTypes;
    cxxThrowFunctionParamTypes.push_back(builder.getInt8PtrTy());
    cxxThrowFunctionParamTypes.push_back(builder.getInt8PtrTy());
    llvm::FunctionType* cxxThrowFunctionType = llvm::FunctionType::get(builder.getVoidTy(), cxxThrowFunctionParamTypes, false);
    llvm::Function* cxxThrowFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("_CxxThrowException", cxxThrowFunctionType));
    ArgVector rethrowArgs;
    rethrowArgs.push_back(llvm::Constant::getNullValue(builder.getInt8PtrTy()));
    rethrowArgs.push_back(llvm::Constant::getNullValue(builder.getInt8PtrTy()));
    std::vector<llvm::OperandBundleDef> bundles;
    std::vector<llvm::Value*> inputs;
    inputs.push_back(currentPad->value);
    bundles.push_back(llvm::OperandBundleDef("funclet", inputs));
    llvm::CallInst* callInst = llvm::CallInst::Create(cxxThrowFunction, rethrowArgs, bundles, "", resumeTarget);
    if (diBuilder)
    {
        callInst->setDebugLoc(GetCurrentDebugLocation());
    }
    builder.CreateBr(nextTarget);
    currentPad = parentPad;
    SetCurrentBasicBlock(nextTarget);
    basicBlockOpen = true;
    cleanupBlock = prevCleanupBlock;
}

void WindowsEmitter::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    SetCurrentDebugLocation(boundRethrowStatement.GetSpan());
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundRethrowStatement);
    boundRethrowStatement.ReleaseCall()->Accept(*this);
    if (diBuilder)
    {
        SetCurrentDebugLocation(boundRethrowStatement.GetSpan());
    }
    std::vector<llvm::Type*> cxxThrowFunctionParamTypes;
    cxxThrowFunctionParamTypes.push_back(builder.getInt8PtrTy());
    cxxThrowFunctionParamTypes.push_back(builder.getInt8PtrTy());
    llvm::FunctionType* cxxThrowFunctionType = llvm::FunctionType::get(builder.getVoidTy(), cxxThrowFunctionParamTypes, false);
    llvm::Function* cxxThrowFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("_CxxThrowException", cxxThrowFunctionType));
    ArgVector rethrowArgs;
    rethrowArgs.push_back(llvm::Constant::getNullValue(builder.getInt8PtrTy()));
    rethrowArgs.push_back(llvm::Constant::getNullValue(builder.getInt8PtrTy()));
    std::vector<llvm::OperandBundleDef> bundles;
    std::vector<llvm::Value*> inputs;
    inputs.push_back(currentPad->value);
    bundles.push_back(llvm::OperandBundleDef("funclet", inputs));
    llvm::CallInst* callInst = llvm::CallInst::Create(cxxThrowFunction, rethrowArgs, bundles, "", CurrentBasicBlock());
    if (diBuilder)
    {
        callInst->setDebugLoc(GetDebugLocation(boundRethrowStatement.GetSpan()));
    }
}

void WindowsEmitter::CreateCleanup()
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

void WindowsEmitter::GenerateCodeForCleanups()
{
    for (const std::unique_ptr<Cleanup>& cleanup : cleanups)
    {
        SetCurrentBasicBlock(cleanup->cleanupBlock);
        Pad* parentPad = cleanup->currentPad;
        llvm::CleanupPadInst* cleanupPad = nullptr;
        if (parentPad)
        {
            ArgVector args;
            cleanupPad = builder.CreateCleanupPad(parentPad->value, args);
        }
        else
        {
            ArgVector args;
            cleanupPad = builder.CreateCleanupPad(llvm::ConstantTokenNone::get(context), args);
        }
        Pad pad;
        pad.parent = parentPad;
        pad.value = cleanupPad;
        currentPad = &pad;
        for (const std::unique_ptr<BoundFunctionCall>& destructorCall : cleanup->destructors)
        {
            destructorCall->Accept(*this);
        }
        llvm::BasicBlock* unwindTarget = cleanup->handlerBlock;
        builder.CreateCleanupRet(llvm::cast<llvm::CleanupPadInst>(cleanupPad), unwindTarget);
    }
}

} } // namespace cmajor::emitter
