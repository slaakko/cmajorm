// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegencpp/CmCppCodeGenerator.hpp>
#include <cmajor/cmcppbe/EmittingContext.hpp>
#include <cmajor/cmcppi/CompileUnit.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundNamespace.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>

namespace cmajor { namespace codegencpp {

using namespace soulng::util;
using namespace soulng::unicode;

struct NativeModule
{
    NativeModule(cmajor::ir::Emitter* emitter_, const std::string& moduleFilePath_) : emitter(emitter_)
    {
        module = emitter->CreateModule(moduleFilePath_);
        emitter->SetModule(module);
    }
    ~NativeModule()
    {
        emitter->DestroyModule(module);
    }
    cmajor::ir::Emitter* emitter;
    void* module;
};

CmCppCodeGenerator::CmCppCodeGenerator(cmajor::ir::EmittingContext& emittingContext_) :
    cmajor::codegenbase::CodeGenerator(emittingContext_), emitter(GetEmitter()), emittingContext(&emittingContext_), symbolTable(nullptr), module(nullptr), compileUnit(nullptr),
    nativeCompileUnit(nullptr), function(nullptr), entryBasicBlock(nullptr), lastInstructionWasRet(false), destructorCallGenerated(false), genJumpingBoolCode(false),
    trueBlock(nullptr), falseBlock(nullptr), breakTarget(nullptr), continueTarget(nullptr), sequenceSecond(nullptr), currentFunction(nullptr), currentBlock(nullptr),
    breakTargetBlock(nullptr), continueTargetBlock(nullptr), lastAlloca(nullptr), currentClass(nullptr), basicBlockOpen(false), defaultDest(nullptr), currentCaseMap(nullptr),
    generateLineNumbers(false), currentTryBlockId(-1), nextTryBlockId(0), currentTryNextBlock(nullptr), handlerBlock(nullptr), cleanupBlock(nullptr), newCleanupNeeded(false),
    inTryBlock(false), prevWasTerminator(false)
{
    emitter->SetEmittingDelegate(this);
}

void CmCppCodeGenerator::GenerateCode(void* boundCompileUnit)
{
    BoundCompileUnit* compileUnit = static_cast<BoundCompileUnit*>(boundCompileUnit);
    compileUnit->Accept(*this);
}

void CmCppCodeGenerator::Compile(const std::string& intermediateCodeFile)
{
    const Tool& compilerTool = GetCompilerTool();
    std::string outputDirectory = GetFullPath(Path::Combine(Path::GetDirectoryName(intermediateCodeFile), compilerTool.outputDirectory));
    boost::filesystem::create_directories(outputDirectory);
    std::string intermediateCompileCommand;
    std::string intermediateCompileErrorFilePath = intermediateCodeFile + ".error";
    intermediateCompileCommand.append("cmfileredirector -2 ").append(intermediateCompileErrorFilePath).append("  ").append(compilerTool.commandName);
    for (const std::string& arg : compilerTool.args)
    {
        if (arg.find('$') != std::string::npos)
        {
            std::string modifiedArg = arg;
            if (arg.find("$SOURCE_FILE$") != std::string::npos)
            {
                modifiedArg = soulng::util::Replace(arg, "$SOURCE_FILE$", QuotedPath(intermediateCodeFile));
            }
            else if (arg.find("$GENERATE_ASSEMBLY_FILE_OPTION$") != std::string::npos)
            {
                if (GetGlobalFlag(GlobalFlags::emitLlvm))
                {
                    modifiedArg = soulng::util::Replace(modifiedArg, "$GENERATE_ASSEMBLY_FILE_OPTION$", "");
                }
                else
                {
                    continue;
                }
            }
            else if (arg.find("$DEBUG_INFORMATION_FILE$") != std::string::npos)
            {
                modifiedArg = soulng::util::Replace(modifiedArg, "$DEBUG_INFORMATION_FILE$",
                    QuotedPath(GetFullPath(Path::ChangeExtension(intermediateCodeFile, compilerTool.debugInformationFileExtension))));
            }
            else if (arg.find("$ASSEMBLY_FILE$") != std::string::npos)
            {
                modifiedArg = soulng::util::Replace(modifiedArg, "$ASSEMBLY_FILE$", QuotedPath(GetFullPath(Path::ChangeExtension(intermediateCodeFile, compilerTool.assemblyFileExtension))));
            }
            else if (arg.find("$OBJECT_FILE$") != std::string::npos)
            {
                modifiedArg = soulng::util::Replace(modifiedArg, "$OBJECT_FILE$", QuotedPath(compileUnit->ObjectFilePath()));
            }
            intermediateCompileCommand.append(" ").append(modifiedArg);
        }
        else
        {
            intermediateCompileCommand.append(" ").append(arg);
        }
    }
    try
    {
        System(intermediateCompileCommand); 
        boost::filesystem::remove(boost::filesystem::path(intermediateCompileErrorFilePath)); 
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(intermediateCompileErrorFilePath);
        throw std::runtime_error("compiling intermediate code '" + intermediateCodeFile + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
}

void CmCppCodeGenerator::Visit(BoundCompileUnit& boundCompileUnit)
{
    boundCompileUnit.ResetCodeGenerated();
    std::string intermediateFilePath = Path::ChangeExtension(boundCompileUnit.LLFilePath(), ".cpp");
    NativeModule nativeModule(emitter, intermediateFilePath);
    compileUnitId = boundCompileUnit.Id();
    emitter->SetCompileUnitId(compileUnitId);
    emitter->SetCurrentLineNumber(0);
    generateLineNumbers = false;
    symbolTable = &boundCompileUnit.GetSymbolTable();
    module = &boundCompileUnit.GetModule();
    compileUnit = &boundCompileUnit;
    nativeCompileUnit = static_cast<cmcppi::CompileUnit*>(nativeModule.module);
    nativeCompileUnit->SetId(compileUnitId);
    nativeCompileUnit->SetSourceFilePath(boundCompileUnit.SourceFilePath());
    ConstantArrayRepository& constantArrayRepository = boundCompileUnit.GetConstantArrayRepository();
    for (ConstantSymbol* constantSymbol : constantArrayRepository.ConstantArrays())
    {
        constantSymbol->ArrayIrObject(*emitter, true);
    }
    ConstantStructureRepository& constantStructureRepository = boundCompileUnit.GetConstantStructureRepository();
    for (ConstantSymbol* constantSymbol : constantStructureRepository.ConstantStructures())
    {
        constantSymbol->StructureIrObject(*emitter, true);
    }
    int n = boundCompileUnit.BoundNodes().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* node = boundCompileUnit.BoundNodes()[i].get();
        node->Accept(*this);
    }
    nativeCompileUnit->Write();
    if (!GetGlobalFlag(GlobalFlags::disableCodeGen))
    {
        Compile(intermediateFilePath);
    }
}

void CmCppCodeGenerator::Visit(BoundNamespace& boundNamespace)
{
    int n = boundNamespace.Members().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* node = boundNamespace.Members()[i].get();
        node->Accept(*this);
    }
}

void CmCppCodeGenerator::Visit(BoundClass& boundClass)
{
    classStack.push(currentClass);
    currentClass = &boundClass;
    int n = boundClass.Members().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* boundNode = boundClass.Members()[i].get();
        boundNode->Accept(*this);
    }
    currentClass = classStack.top();
    classStack.pop();
}

void CmCppCodeGenerator::Visit(BoundFunction& boundFunction)
{
    if (!boundFunction.Body()) return;
    currentFunction = &boundFunction;
    FunctionSymbol* functionSymbol = boundFunction.GetFunctionSymbol();
    if (functionSymbol->Parent()->GetSymbolType() != SymbolType::classTemplateSpecializationSymbol && functionSymbol->IsTemplateSpecialization())
    {
        functionSymbol->SetFlag(FunctionSymbolFlags::dontReuse);
    }
    if (functionSymbol->CodeGenerated()) return;
    functionSymbol->SetCodeGenerated();
    void* functionType = functionSymbol->IrType(*emitter);
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    lastAlloca = nullptr;
    handlerBlock = nullptr;
    cleanupBlock = nullptr;
    newCleanupNeeded = false;
    labeledStatementMap.clear();
    cleanups.clear();
    if (functionSymbol->HasSource())
    {
        generateLineNumbers = true;
        emitter->SetCurrentLineNumber(boundFunction.Body()->GetSpan().line);
    }
    else
    {
        generateLineNumbers = false;
        emitter->SetCurrentLineNumber(0);
    }
    function = emitter->GetOrInsertFunction(ToUtf8(functionSymbol->MangledName()), functionType);
    if (functionSymbol->HasSource())
    {
        // todo metadata
    }
    if (GetGlobalFlag(GlobalFlags::release) && functionSymbol->IsInline())
    {
        emitter->AddInlineFunctionAttribute(function);
        functionSymbol->SetLinkOnceOdrLinkage();
    }
    if (functionSymbol->HasLinkOnceOdrLinkage())
    {
        void* comdat = emitter->GetOrInsertAnyFunctionComdat(ToUtf8(functionSymbol->MangledName()), function);
        emitter->SetFunctionLinkageToLinkOnceODRLinkage(function);
    }
    emitter->SetFunction(function);
    void* entryBlock = emitter->CreateBasicBlock("entry");
    entryBasicBlock = entryBlock;
    emitter->SetCurrentBasicBlock(entryBlock);
    if (currentClass && !currentClass->IsInlineFunctionContainer())
    {
        ClassTypeSymbol* classTypeSymbol = currentClass->GetClassTypeSymbol();
        if (!emitter->IsVmtObjectCreated(classTypeSymbol))
        {
            classTypeSymbol->VmtObject(*emitter, true);
        }
        if (!emitter->IsStaticObjectCreated(classTypeSymbol))
        {
            classTypeSymbol->StaticObject(*emitter, true);
        }
    }
    int np = functionSymbol->Parameters().size();
    for (int i = 0; i < np; ++i)
    {
        ParameterSymbol* parameter = functionSymbol->Parameters()[i];
        void* allocaInst = emitter->CreateAlloca(parameter->GetType()->IrType(*emitter));
        emitter->SetIrObject(parameter, allocaInst);
        lastAlloca = allocaInst;
    }
    if (functionSymbol->ReturnParam())
    {
        ParameterSymbol* parameter = functionSymbol->ReturnParam();
        void* allocaInst = emitter->CreateAlloca(parameter->GetType()->IrType(*emitter));
        emitter->SetIrObject(parameter, allocaInst);
        lastAlloca = allocaInst;
    }
    int nlv = functionSymbol->LocalVariables().size();
    for (int i = 0; i < nlv; ++i)
    {
        LocalVariableSymbol* localVariable = functionSymbol->LocalVariables()[i];
        void* allocaInst = emitter->CreateAlloca(localVariable->GetType()->IrType(*emitter));
        emitter->SetIrObject(localVariable, allocaInst);
        lastAlloca = allocaInst;
    }
    bool byValueComplexParams = false;
    for (int i = 0; i < np; ++i)
    {
        ParameterSymbol* parameter = functionSymbol->Parameters()[i];
        if (parameter->GetType()->IsClassTypeSymbol() ||
            (parameter->GetType()->GetSymbolType() == SymbolType::classDelegateTypeSymbol) ||
            (parameter->GetType()->GetSymbolType() == SymbolType::interfaceTypeSymbol))
        {
            byValueComplexParams = true;
            break;
        }
    }
    if (byValueComplexParams)
    {
        emitter->CreateSave();
    }
    for (int i = 0; i < np; ++i)
    {
        void* arg = emitter->GetFunctionArgument(function, i);
        ParameterSymbol* parameter = functionSymbol->Parameters()[i];
        if (parameter->GetType()->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parameter->GetType());
            FunctionSymbol* copyConstructor = classType->CopyConstructor();
            if (!copyConstructor)
            {
                copyConstructor = compileUnit->GetCopyConstructorFor(classType->TypeId());
            }
            void* copyCtorType = copyConstructor->IrType(*emitter);
            void* callee = emitter->GetOrInsertFunction(ToUtf8(copyConstructor->MangledName()), copyCtorType);
            std::vector<void*> args;
            args.push_back(parameter->IrObject(*emitter));
            args.push_back(arg);
            emitter->CreateCall(callee, args);
        }
        else if (parameter->GetType()->GetSymbolType() == SymbolType::classDelegateTypeSymbol)
        {
            ClassDelegateTypeSymbol* classDelegateType = static_cast<ClassDelegateTypeSymbol*>(parameter->GetType());
            FunctionSymbol* copyConstructor = classDelegateType->CopyConstructor();
            if (!copyConstructor)
            {
                throw std::runtime_error("internal error: class delegate type has no copy constructor");
            }
            std::vector<GenObject*> copyCtorArgs;
            NativeValue paramValue(parameter->IrObject(*emitter));
            copyCtorArgs.push_back(&paramValue);
            NativeValue argumentValue(arg);
            copyCtorArgs.push_back(&argumentValue);
            copyConstructor->GenerateCall(*emitter, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
        }
        else if (parameter->GetType()->GetSymbolType() == SymbolType::interfaceTypeSymbol)
        {
            InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(parameter->GetType());
            FunctionSymbol* copyConstructor = interfaceType->CopyConstructor();
            if (!copyConstructor)
            {
                copyConstructor = compileUnit->GetCopyConstructorFor(interfaceType->TypeId());
            }
            std::vector<GenObject*> copyCtorArgs;
            NativeValue paramValue(parameter->IrObject(*emitter));
            paramValue.SetType(interfaceType->AddPointer(Span()));
            copyCtorArgs.push_back(&paramValue);
            NativeValue argumentValue(arg);
            argumentValue.SetType(interfaceType->AddPointer(Span()));
            copyCtorArgs.push_back(&argumentValue);
            copyConstructor->GenerateCall(*emitter, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
        }
        else
        {
            emitter->CreateStore(arg, parameter->IrObject(*emitter));
        }
    }
    if (functionSymbol->ReturnParam())
    {
        void* arg = emitter->GetFunctionArgument(function, np);
        emitter->CreateStore(arg, functionSymbol->ReturnParam()->IrObject(*emitter));
    }
    for (BoundStatement* labeledStatement : boundFunction.LabeledStatements())
    {
        void* target = emitter->CreateBasicBlock(ToUtf8(labeledStatement->Label()));
        labeledStatementMap[labeledStatement] = target;
    }
    BoundCompoundStatement* body = boundFunction.Body();
    body->Accept(*this);
    BoundStatement* lastStatement = nullptr;
    if (!body->Statements().empty())
    {
        lastStatement = body->Statements().back().get();
    }
    if (!lastStatement || lastStatement->GetBoundNodeType() != BoundNodeType::boundReturnStatement ||
        lastStatement->GetBoundNodeType() == BoundNodeType::boundReturnStatement && destructorCallGenerated)
    {
        if (functionSymbol->ReturnType() && functionSymbol->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol && !functionSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
        {
            void* defaultValue = functionSymbol->ReturnType()->CreateDefaultIrValue(*emitter);
            emitter->CreateRet(defaultValue);
            lastInstructionWasRet = true;
        }
        else
        {
            emitter->CreateRetVoid();
            lastInstructionWasRet = true;
        }
    }
    GenerateCodeForCleanups(functionSymbol);
    emitter->FinalizeFunction(function);
}

void CmCppCodeGenerator::Visit(BoundCompoundStatement& boundCompoundStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundCompoundStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundCompoundStatement);
    BoundCompoundStatement* prevBlock = currentBlock;
    currentBlock = &boundCompoundStatement;
    blockDestructionMap[currentBlock] = std::vector<std::unique_ptr<BoundFunctionCall>>();
    blocks.push_back(currentBlock);
    int n = boundCompoundStatement.Statements().size();
    for (int i = 0; i < n; ++i)
    {
        BoundStatement* statement = boundCompoundStatement.Statements()[i].get();
        statement->Accept(*this);
    }
    ExitBlocks(prevBlock);
    blocks.pop_back();
    currentBlock = prevBlock;
}

void CmCppCodeGenerator::Visit(BoundSequenceStatement& boundSequenceStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundSequenceStatement);
    BoundStatement* prevSequence = sequenceSecond;
    sequenceSecond = boundSequenceStatement.Second();
    boundSequenceStatement.First()->Accept(*this);
    sequenceSecond = prevSequence;
    if (!boundSequenceStatement.Second()->Generated())
    {
        boundSequenceStatement.Second()->Accept(*this);
    }
}

void CmCppCodeGenerator::Visit(BoundReturnStatement& boundReturnStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundReturnStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundReturnStatement);
    BoundFunctionCall* returnFunctionCall = boundReturnStatement.ReturnFunctionCall();
    if (returnFunctionCall)
    {
        ExitBlocks(nullptr);
        boundReturnStatement.ReturnFunctionCall()->Accept(*this);
        void* returnValue = emitter->Stack().Pop();
        if (sequenceSecond)
        {
            sequenceSecond->SetGenerated();
            sequenceSecond->Accept(*this);
        }
        emitter->CreateRet(returnValue);
        lastInstructionWasRet = true;
    }
    else
    {
        ExitBlocks(nullptr);
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

void CmCppCodeGenerator::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundGotoCaseStatement.GetSpan().line);
    }
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
        throw Exception(module, "case not found", boundGotoCaseStatement.GetSpan());
    }
}

void CmCppCodeGenerator::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundGotoDefaultStatement.GetSpan().line);
    }
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
        throw Exception(module, "no default destination", boundGotoDefaultStatement.GetSpan());
    }
}

void CmCppCodeGenerator::Visit(BoundBreakStatement& boundBreakStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundBreakStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundBreakStatement);
    Assert(breakTarget && breakTargetBlock, "break target not set");
    ExitBlocks(breakTargetBlock);
    emitter->CreateBr(breakTarget);
    if (!currentCaseMap) // not in switch
    {
        void* nextBlock = emitter->CreateBasicBlock("next");
        emitter->SetCurrentBasicBlock(nextBlock);
        basicBlockOpen = true;
    }
}

void CmCppCodeGenerator::Visit(BoundContinueStatement& boundContinueStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundContinueStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    prevWasTerminator = false;
    SetTarget(&boundContinueStatement);
    Assert(continueTarget && continueTargetBlock, "continue target not set");
    ExitBlocks(continueTargetBlock);
    emitter->CreateBr(continueTarget);
    void* nextBlock = emitter->CreateBasicBlock("next");
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void CmCppCodeGenerator::Visit(BoundGotoStatement& boundGotoStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundGotoStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    prevWasTerminator = false;
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
        throw Exception(module, "goto target not found", boundGotoStatement.GetSpan());
    }
    void* nextBlock = emitter->CreateBasicBlock("next");
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void CmCppCodeGenerator::Visit(BoundIfStatement& boundIfStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundIfStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundIfStatement);
    void* prevTrueBlock = trueBlock;
    void* prevFalseBlock = falseBlock;
    trueBlock = emitter->CreateBasicBlock("true");
    void* nextBlock = emitter->CreateBasicBlock("next");
    if (boundIfStatement.ElseS())
    {
        falseBlock = emitter->CreateBasicBlock("false");
    }
    else
    {
        falseBlock = nextBlock;
    }
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundIfStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    emitter->SetCurrentBasicBlock(trueBlock);
    boundIfStatement.ThenS()->Accept(*this);
    emitter->CreateBr(nextBlock);
    if (boundIfStatement.ElseS())
    {
        emitter->SetCurrentBasicBlock(falseBlock);
        boundIfStatement.ElseS()->Accept(*this);
        emitter->CreateBr(nextBlock);
    }
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
    emitter->SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void CmCppCodeGenerator::Visit(BoundWhileStatement& boundWhileStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundWhileStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundWhileStatement);
    void* prevTrueBlock = trueBlock;
    void* prevFalseBlock = falseBlock;
    void* prevBreakTarget = breakTarget;
    void* prevContinueTarget = continueTarget;
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    BoundCompoundStatement* prevContinueTargetBlock = continueTargetBlock;
    breakTargetBlock = currentBlock;
    continueTargetBlock = currentBlock;
    trueBlock = emitter->CreateBasicBlock("true");
    falseBlock = emitter->CreateBasicBlock("next");
    breakTarget = falseBlock;
    void* condBlock = emitter->CreateBasicBlock("cond");
    emitter->CreateBr(condBlock);
    emitter->SetCurrentBasicBlock(condBlock);
    continueTarget = condBlock;
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundWhileStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    emitter->SetCurrentBasicBlock(trueBlock);
    boundWhileStatement.Statement()->Accept(*this);
    emitter->CreateBr(condBlock);
    emitter->SetCurrentBasicBlock(falseBlock);
    breakTargetBlock = prevBreakTargetBlock;
    continueTargetBlock = prevContinueTargetBlock;
    breakTarget = prevBreakTarget;
    continueTarget = prevContinueTarget;
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
}

void CmCppCodeGenerator::Visit(BoundDoStatement& boundDoStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundDoStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundDoStatement);
    void* prevTrueBlock = trueBlock;
    void* prevFalseBlock = falseBlock;
    void* prevBreakTarget = breakTarget;
    void* prevContinueTarget = continueTarget;
    void* doBlock = emitter->CreateBasicBlock("do");
    void* condBlock = emitter->CreateBasicBlock("cond");
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    BoundCompoundStatement* prevContinueTargetBlock = continueTargetBlock;
    breakTargetBlock = currentBlock;
    continueTargetBlock = currentBlock;
    trueBlock = doBlock;
    falseBlock = emitter->CreateBasicBlock("next");
    breakTarget = falseBlock;
    continueTarget = condBlock;
    emitter->CreateBr(doBlock);
    emitter->SetCurrentBasicBlock(doBlock);
    boundDoStatement.Statement()->Accept(*this);
    emitter->CreateBr(condBlock);
    emitter->SetCurrentBasicBlock(condBlock);
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundDoStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    emitter->SetCurrentBasicBlock(falseBlock);
    basicBlockOpen = true;
    breakTargetBlock = prevBreakTargetBlock;
    continueTargetBlock = prevContinueTargetBlock;
    breakTarget = prevBreakTarget;
    continueTarget = prevContinueTarget;
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
}

void CmCppCodeGenerator::Visit(BoundForStatement& boundForStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundForStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundForStatement);
    void* prevTrueBlock = trueBlock;
    void* prevFalseBlock = falseBlock;
    void* prevBreakTarget = breakTarget;
    void* prevContinueTarget = continueTarget;
    boundForStatement.InitS()->Accept(*this);
    void* condBlock = emitter->CreateBasicBlock("cond");
    void* actionBlock = emitter->CreateBasicBlock("action");
    void* loopBlock = emitter->CreateBasicBlock("loop");
    trueBlock = actionBlock;
    falseBlock = emitter->CreateBasicBlock("next");
    breakTarget = falseBlock;
    continueTarget = loopBlock;
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    BoundCompoundStatement* prevContinueTargetBlock = continueTargetBlock;
    breakTargetBlock = currentBlock;
    continueTargetBlock = currentBlock;
    emitter->CreateBr(condBlock);
    emitter->SetCurrentBasicBlock(condBlock);
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundForStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    emitter->SetCurrentBasicBlock(actionBlock);
    boundForStatement.ActionS()->Accept(*this);
    emitter->CreateBr(loopBlock);
    emitter->SetCurrentBasicBlock(loopBlock);
    boundForStatement.LoopS()->Accept(*this);
    emitter->CreateBr(condBlock);
    emitter->SetCurrentBasicBlock(falseBlock);
    basicBlockOpen = true;
    breakTargetBlock = prevBreakTargetBlock;
    continueTargetBlock = prevContinueTargetBlock;
    breakTarget = prevBreakTarget;
    continueTarget = prevContinueTarget;
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
}

void CmCppCodeGenerator::Visit(BoundSwitchStatement& boundSwitchStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundSwitchStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundSwitchStatement);
    void* prevBreakTarget = breakTarget;
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    breakTargetBlock = currentBlock;
    boundSwitchStatement.Condition()->Accept(*this);
    void* condition = emitter->Stack().Pop();
    void* prevDefaultDest = defaultDest;
    void* next = nullptr;
    if (boundSwitchStatement.DefaultStatement())
    {
        defaultDest = emitter->CreateBasicBlock("default");
        next = emitter->CreateBasicBlock("next");
    }
    else
    {
        defaultDest = emitter->CreateBasicBlock("next");
        next = defaultDest;
    }
    breakTarget = next;
    unsigned n = boundSwitchStatement.CaseStatements().size();
    void* switchInst = emitter->CreateSwitch(condition, defaultDest, n);
    std::unordered_map<IntegralValue, void*, IntegralValueHash>* prevCaseMap = currentCaseMap;
    std::unordered_map<IntegralValue, void*, IntegralValueHash> caseMap;
    currentCaseMap = &caseMap;
    for (unsigned i = 0; i < n; ++i)
    {
        const std::unique_ptr<BoundCaseStatement>& caseS = boundSwitchStatement.CaseStatements()[i];
        void* caseDest = emitter->CreateBasicBlock("case" + std::to_string(i));
        for (const std::unique_ptr<Value>& caseValue : caseS->CaseValues())
        {
            IntegralValue integralCaseValue(caseValue.get());
            caseMap[integralCaseValue] = caseDest;
            emitter->AddCase(switchInst, caseValue->IrValue(*emitter), caseDest);
        }
    }
    for (unsigned i = 0; i < n; ++i)
    {
        const std::unique_ptr<BoundCaseStatement>& caseS = boundSwitchStatement.CaseStatements()[i];
        caseS->Accept(*this);
        if (basicBlockOpen)
        {
            emitter->CreateBr(next);
            basicBlockOpen = false;
        }
    }
    if (boundSwitchStatement.DefaultStatement())
    {
        boundSwitchStatement.DefaultStatement()->Accept(*this);
        if (basicBlockOpen)
        {
            emitter->CreateBr(next);
            basicBlockOpen = false;
        }
    }
    emitter->SetCurrentBasicBlock(next);
    basicBlockOpen = true;
    currentCaseMap = prevCaseMap;
    defaultDest = prevDefaultDest;
    breakTargetBlock = prevBreakTargetBlock;
    breakTarget = prevBreakTarget;
}

void CmCppCodeGenerator::Visit(BoundCaseStatement& boundCaseStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundCaseStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundCaseStatement);
    if (!boundCaseStatement.CaseValues().empty())
    {
        IntegralValue integralCaseValue(boundCaseStatement.CaseValues().front().get());
        auto it = currentCaseMap->find(integralCaseValue);
        if (it != currentCaseMap->cend())
        {
            void* caseDest = it->second;
            emitter->SetCurrentBasicBlock(caseDest);
            if (boundCaseStatement.CompoundStatement())
            {
                boundCaseStatement.CompoundStatement()->Accept(*this);
            }
        }
        else
        {
            throw Exception(module, "case not found", boundCaseStatement.GetSpan());
        }
    }
    else
    {
        throw Exception(module, "no cases", boundCaseStatement.GetSpan());
    }

}

void CmCppCodeGenerator::Visit(BoundDefaultStatement& boundDefaultStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundDefaultStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundDefaultStatement);
    if (defaultDest)
    {
        emitter->SetCurrentBasicBlock(defaultDest);
        if (boundDefaultStatement.CompoundStatement())
        {
            boundDefaultStatement.CompoundStatement()->Accept(*this);
        }
    }
    else
    {
        throw Exception(module, "no default destination", boundDefaultStatement.GetSpan());
    }
}

void CmCppCodeGenerator::Visit(BoundConstructionStatement& boundConstructionStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundConstructionStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundConstructionStatement);
    boundConstructionStatement.ConstructorCall()->Accept(*this);
    if (!boundConstructionStatement.ConstructorCall()->GetFunctionSymbol()->IsBasicTypeOperation())
    {
        int n = boundConstructionStatement.ConstructorCall()->Arguments().size();
        if (n > 0)
        {
            const std::unique_ptr<BoundExpression>& firstArgument = boundConstructionStatement.ConstructorCall()->Arguments()[0];
            TypeSymbol* firstArgumentBaseType = firstArgument->GetType()->BaseType();
            if (firstArgumentBaseType->IsClassTypeSymbol())
            {
                if (firstArgument->GetType()->IsPointerType() && firstArgument->GetType()->RemovePointer(boundConstructionStatement.GetSpan())->IsClassTypeSymbol())
                {
                    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(firstArgumentBaseType);
                    if (classType->Destructor())
                    {
                        newCleanupNeeded = true;
                        std::unique_ptr<BoundExpression> classPtrArgument(firstArgument->Clone());
                        std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(module, currentBlock->EndSpan(), classType->Destructor()));
                        destructorCall->AddArgument(std::move(classPtrArgument));
                        Assert(currentBlock, "current block not set");
                        auto it = blockDestructionMap.find(currentBlock);
                        if (it != blockDestructionMap.cend())
                        {
                            std::vector<std::unique_ptr<BoundFunctionCall>>& destructorCallVec = it->second;
                            destructorCallVec.push_back(std::move(destructorCall));
                        }
                        else
                        {
                            Assert(false, "block destruction not found");
                        }
                    }
                }
            }
        }
    }
}

void CmCppCodeGenerator::Visit(BoundAssignmentStatement& boundAssignmentStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundAssignmentStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundAssignmentStatement);
    boundAssignmentStatement.AssignmentCall()->Accept(*this);
}

void CmCppCodeGenerator::Visit(BoundExpressionStatement& boundExpressionStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundExpressionStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundExpressionStatement);
    boundExpressionStatement.Expression()->Accept(*this);
    if (boundExpressionStatement.Expression()->HasValue())
    {
        emitter->Stack().Pop();
    }
}

void CmCppCodeGenerator::Visit(BoundEmptyStatement& boundEmptyStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundEmptyStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundEmptyStatement);
    // todo
}

void CmCppCodeGenerator::Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundSetVmtPtrStatement);
    BoundExpression* classPtr = boundSetVmtPtrStatement.ClassPtr();
    TypeSymbol* type = classPtr->GetType()->BaseType();
    Assert(type->IsClassTypeSymbol(), "class type expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
    int32_t vmtPtrIndex = classType->VmtPtrIndex();
    Assert(vmtPtrIndex != -1, "invalid vmt ptr index");
    classPtr->Accept(*this);
    void* classPtrValue = emitter->Stack().Pop();
    void* ptr = emitter->GetMemberVariablePtr(classPtrValue, vmtPtrIndex);
    void* vmtPtr = emitter->CreateBitCast(boundSetVmtPtrStatement.ClassType()->VmtObject(*emitter, false), emitter->GetIrTypeForVoidPtrType());
    emitter->CreateStore(vmtPtr, ptr);
}

void CmCppCodeGenerator::Visit(BoundThrowStatement& boundThrowStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundThrowStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundThrowStatement);
    boundThrowStatement.ThrowCallExpr()->Accept(*this);
}

void CmCppCodeGenerator::Visit(BoundTryStatement& boundTryStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundTryStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundTryStatement);
    void* prevHandlerBlock = handlerBlock;
    void* prevCleanupBlock = cleanupBlock;
    handlerBlock = emitter->CreateBasicBlock("handlers");
    void* resumeBlock = emitter->CreateBasicBlock("resume");
    cleanupBlock = nullptr;

/*
    int64_t parentTryBlockId = currentTryBlockId;
    currentTryBlockId = nextTryBlockId++;
    void* nop1 = emitter->CreateNop();
    void* beginTry = emitter->CreateMDStruct();
    int beginTryId = emitter->GetMDStructId(beginTry);
    void* beginTryMdRef = emitter->CreateMDStructRef(beginTryId);
    emitter->SetMetadataRef(nop1, beginTryMdRef);
*/
    bool prevInTryBlock = inTryBlock;
    inTryBlock = true;
    void* beginTryBlock = emitter->CreateBasicBlock("beginTry");
    emitter->CreateBr(beginTryBlock);
    emitter->SetCurrentBasicBlock(beginTryBlock);
    emitter->CreateBeginTry();
    boundTryStatement.TryBlock()->Accept(*this);
    inTryBlock = prevInTryBlock;
    void* endTryBlock = emitter->CreateBasicBlock("endTry");
    emitter->CreateBr(endTryBlock);
    emitter->SetCurrentBasicBlock(endTryBlock);
/*
    void* nop2 = emitter->CreateNop();
    void* endTry = emitter->CreateMDStruct();
    int endTryId = emitter->GetMDStructId(endTry);
    void* endTryMdRef = emitter->CreateMDStructRef(endTryId);
    emitter->SetMetadataRef(nop2, endTryMdRef);
*/
    void* tryNextBlock = emitter->CreateBasicBlock("tryNext");
    emitter->CreateEndTry(tryNextBlock, handlerBlock);
    void* prevTryNextBlock = currentTryNextBlock;
    currentTryNextBlock = tryNextBlock;
    emitter->SetCurrentBasicBlock(handlerBlock);
    handlerBlock = prevHandlerBlock;
    int n = boundTryStatement.Catches().size();
    void* catchTarget = emitter->CreateBasicBlock("catch");
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<BoundCatchStatement>& boundCatchStatement = boundTryStatement.Catches()[i];
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
        emitter->SetCurrentBasicBlock(thisHandlerTarget);
        boundCatchStatement->CatchBlock()->Accept(*this);
        emitter->CreateBr(tryNextBlock);
    }
    emitter->SetCurrentBasicBlock(resumeBlock);
    emitter->CreateResume(nullptr);
    emitter->SetCurrentBasicBlock(tryNextBlock);
/*
    currentTryBlockId = parentTryBlockId;
    currentTryNextBlock = prevTryNextBlock;
*/
    cleanupBlock = prevCleanupBlock;
    basicBlockOpen = true;
}

void CmCppCodeGenerator::Visit(BoundParameter& boundParameter)
{
    boundParameter.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundLocalVariable& boundLocalVariable)
{
    boundLocalVariable.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundMemberVariable& boundMemberVariable)
{
    boundMemberVariable.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundConstant& boundConstant)
{
    boundConstant.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundEnumConstant& boundEnumConstant)
{
    boundEnumConstant.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundLiteral& boundLiteral)
{
    boundLiteral.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundTemporary& boundTemporary)
{
    boundTemporary.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundSizeOfExpression& boundSizeOfExpression)
{
    boundSizeOfExpression.Load(*emitter, OperationFlags::none);
}

void CmCppCodeGenerator::Visit(BoundAddressOfExpression& boundAddressOfExpression)
{
    boundAddressOfExpression.Load(*emitter, OperationFlags::none);
}

void CmCppCodeGenerator::Visit(BoundDereferenceExpression& boundDereferenceExpression)
{
    boundDereferenceExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression)
{
    boundReferenceToPointerExpression.Load(*emitter, OperationFlags::none);
}

void CmCppCodeGenerator::Visit(BoundFunctionCall& boundFunctionCall)
{
    boundFunctionCall.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundDelegateCall& boundDelegateCall)
{
    boundDelegateCall.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundClassDelegateCall& boundClassDelegateCall)
{
    boundClassDelegateCall.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundConversion& boundConversion)
{
    boundConversion.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundConstructExpression& boundConstructExpression)
{
    boundConstructExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression)
{
    boundConstructAndReturnTemporaryExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult)
{
    boundClassOrClassDelegateConversionResult.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundIsExpression& boundIsExpression)
{
    boundIsExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void CmCppCodeGenerator::Visit(BoundAsExpression& boundAsExpression)
{
    boundAsExpression.Load(*emitter, OperationFlags::none);
}

void CmCppCodeGenerator::Visit(BoundTypeNameExpression& boundTypeNameExpression)
{
    boundTypeNameExpression.Load(*emitter, OperationFlags::none);
}

void CmCppCodeGenerator::Visit(BoundBitCast& boundBitCast)
{
    boundBitCast.Load(*emitter, OperationFlags::none);
}

void CmCppCodeGenerator::Visit(BoundFunctionPtr& boundFunctionPtr)
{
    boundFunctionPtr.Load(*emitter, OperationFlags::none);
}

void CmCppCodeGenerator::Visit(BoundDisjunction& boundDisjunction)
{
    if (genJumpingBoolCode)
    {
        Assert(trueBlock, "true block not set");
        Assert(falseBlock, "false block not set");
        void* rightBlock = emitter->CreateBasicBlock("right");
        void* prevFalseBlock = falseBlock;
        falseBlock = rightBlock;
        boundDisjunction.Left()->Accept(*this);
        emitter->SetCurrentBasicBlock(rightBlock);
        falseBlock = prevFalseBlock;
        boundDisjunction.Right()->Accept(*this);
        boundDisjunction.DestroyTemporaries(*emitter);
    }
}

void CmCppCodeGenerator::Visit(BoundConjunction& boundConjunction)
{
    if (genJumpingBoolCode)
    {
        Assert(trueBlock, "true block not set");
        Assert(falseBlock, "false block not set");
        void* rightBlock = emitter->CreateBasicBlock("right");
        void* prevTrueBlock = trueBlock;
        trueBlock = rightBlock;
        boundConjunction.Left()->Accept(*this);
        trueBlock = prevTrueBlock;
        emitter->SetCurrentBasicBlock(rightBlock);
        boundConjunction.Right()->Accept(*this);
        boundConjunction.DestroyTemporaries(*emitter);
    }
}

void CmCppCodeGenerator::Visit(BoundGlobalVariable& boundGlobalVariable)
{
    GlobalVariableSymbol* globalVariableSymbol = boundGlobalVariable.GetGlobalVariableSymbol();
    globalVariableSymbol->CreateIrObject(*emitter);
}

void CmCppCodeGenerator::GenJumpingBoolCode()
{
    if (!genJumpingBoolCode) return;
    Assert(trueBlock, "true block not set");
    Assert(falseBlock, "false block not set");
    void* cond = emitter->Stack().Pop();
    if (sequenceSecond)
    {
        genJumpingBoolCode = false;
        sequenceSecond->SetGenerated();
        sequenceSecond->Accept(*this);
        genJumpingBoolCode = true;
    }
    emitter->CreateCondBr(cond, trueBlock, falseBlock);
}

void CmCppCodeGenerator::SetTarget(BoundStatement* labeledStatement)
{
    if (labeledStatement->Label().empty()) return;
    auto it = labeledStatementMap.find(labeledStatement);
    if (it != labeledStatementMap.cend())
    {
        void* target = it->second;
        emitter->CreateBr(target);
        emitter->SetCurrentBasicBlock(target);
    }
    else
    {
        throw Exception(module, "target for labeled statement not found", labeledStatement->GetSpan());
    }
}

void CmCppCodeGenerator::ExitBlocks(BoundCompoundStatement* targetBlock)
{
    bool createBasicBlock = false;
    BoundStatement* lastStatement = nullptr;
    if (!currentFunction->Body()->Statements().empty())
    {
        lastStatement = currentFunction->Body()->Statements().back().get();
    }
    BoundStatement* currentBlockLastStatement = nullptr;
    if (currentBlock && !currentBlock->Statements().empty())
    {
        currentBlockLastStatement = currentBlock->Statements().back().get();
    }
    if (lastStatement && currentBlockLastStatement && lastStatement == currentBlockLastStatement && currentBlockLastStatement->GetBoundNodeType() == BoundNodeType::boundReturnStatement)
    {
        createBasicBlock = true;
    }
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
                    if (createBasicBlock)
                    {
                        void* nextBlock = emitter->CreateBasicBlock("next");
                        if (!lastInstructionWasRet)
                        {
                            emitter->CreateBr(nextBlock);
                        }
                        emitter->SetCurrentBasicBlock(nextBlock);
                        createBasicBlock = false;
                    }
                    destructorCall->Accept(*this);
                    destructorCallGenerated = true;
                    newCleanupNeeded = true;
                }
            }
        }
    }
}

void* CmCppCodeGenerator::GetGlobalStringPtr(int stringId)
{
    auto it = utf8stringMap.find(stringId);
    if (it != utf8stringMap.cend())
    {
        return it->second;
    }
    else
    {
        void* stringValue = emitter->CreateGlobalStringPtr(compileUnit->GetUtf8String(stringId));
        utf8stringMap[stringId] = stringValue;
        return stringValue;
    }
}

void* CmCppCodeGenerator::GetGlobalWStringConstant(int stringId)
{
    auto it = utf16stringMap.find(stringId);
    if (it != utf16stringMap.cend())
    {
        return it->second;
    }
    else
    {
        const std::u16string& str = compileUnit->GetUtf16String(stringId);
        void* stringValue = emitter->CreateGlobalWStringPtr(str);
        utf16stringMap[stringId] = stringValue;
        return stringValue;
    }
}

void* CmCppCodeGenerator::GetGlobalUStringConstant(int stringId)
{
    auto it = utf32stringMap.find(stringId);
    if (it != utf32stringMap.cend())
    {
        return it->second;
    }
    else
    {
        const std::u32string& str = compileUnit->GetUtf32String(stringId);
        void* stringValue = emitter->CreateGlobalUStringPtr(str);
        utf32stringMap[stringId] = stringValue;
        return stringValue;
    }
}

void* CmCppCodeGenerator::GetGlobalUuidConstant(int uuidId)
{
    auto it = uuidMap.find(uuidId);
    if (it != uuidMap.cend())
    {
        return it->second;
    }
    else
    {
        const boost::uuids::uuid& uuid = compileUnit->GetUuid(uuidId);
        uint64_t length = uuid.static_size();
        std::vector<void*> byteConstants;
        for (boost::uuids::uuid::value_type x : uuid)
        {
            byteConstants.push_back(emitter->CreateIrValueForByte(static_cast<int8_t>(x)));
        }
        void* arrayType = emitter->GetIrTypeForArrayType(emitter->GetIrTypeForByte(), length);
        void* uuidObject = emitter->GetOrInsertGlobal("uuid" + std::to_string(uuidId) + "_" + compileUnitId, arrayType);
        void* uuidGlobal = uuidObject;
        emitter->SetPrivateLinkage(uuidGlobal);
        void* constant = emitter->CreateIrValueForConstantArray(arrayType, byteConstants, "");
        emitter->SetInitializer(uuidGlobal, constant);
        void* uuidValue = uuidGlobal;
        uuidMap[uuidId] = uuidValue;
        return uuidValue;
    }
}

void* CmCppCodeGenerator::HandlerBlock()
{
    return handlerBlock;
}

void* CmCppCodeGenerator::CleanupBlock()
{
    return cleanupBlock;
}

bool CmCppCodeGenerator::NewCleanupNeeded()
{
    return newCleanupNeeded;
}

bool CmCppCodeGenerator::InTryBlock() const
{
    return inTryBlock;
}

int CmCppCodeGenerator::CurrentTryBlockId() const
{
    return currentTryBlockId;
}

int CmCppCodeGenerator::Install(const std::string& str)
{
    return compileUnit->Install(str);
}

int CmCppCodeGenerator::Install(const std::u16string& str)
{
    return compileUnit->Install(str);
}

int CmCppCodeGenerator::Install(const std::u32string& str)
{
    return compileUnit->Install(str);
}

void CmCppCodeGenerator::CreateCleanup()
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
    Cleanup* cleanup = new Cleanup(cleanupBlock);
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

void CmCppCodeGenerator::GenerateCodeForCleanups(FunctionSymbol* function)
{
    for (const std::unique_ptr<Cleanup>& cleanup : cleanups)
    {
        emitter->SetCurrentBasicBlock(cleanup->cleanupBlock);
        for (const std::unique_ptr<BoundFunctionCall>& destructorCall : cleanup->destructors)
        {
            destructorCall->Accept(*this);
        }
        emitter->CreateResume(nullptr);
        if (function->ReturnType() && function->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol && !function->ReturnsClassInterfaceOrClassDelegateByValue())
        {
            void* defaultValue = function->ReturnType()->CreateDefaultIrValue(*emitter);
            emitter->CreateRet(defaultValue);
        }
        else
        {
            emitter->CreateRetVoid();
        }
    }
}

} } // namespace cmajor::codegencpp
