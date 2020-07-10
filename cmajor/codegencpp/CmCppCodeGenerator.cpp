// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegencpp/CmCppCodeGenerator.hpp>
#include <cmajor/cmcppbe/EmittingContext.hpp>
#include <cmajor/cmcppi/CompileUnit.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
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
#include <soulng/util/Log.hpp>
#include <soulng/util/Process.hpp>
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
    generateLineNumbers(false), currentTryBlockId(-1), nextTryBlockId(0), currentTryNextBlock(nullptr), handlerBlock(nullptr), cleanupBlock(nullptr), inTryBlock(false),
    prevWasTerminator(false), numTriesInCurrentBlock(0), tryIndex(0), prevLineNumber(0)
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
    if (GetGlobalFlag(GlobalFlags::disableCodeGen)) return;
    const Tool& compilerTool = GetCompilerTool(GetPlatform(), GetToolChain());
    const Configuration& configuration = GetToolConfiguration(compilerTool, GetConfig());
    std::string outputDirectory = GetFullPath(Path::Combine(Path::GetDirectoryName(intermediateCodeFile), configuration.outputDirectory));
    boost::filesystem::create_directories(outputDirectory);
    std::string intermediateCompileCommand;
    std::string errors;
    intermediateCompileCommand.append(compilerTool.commandName);
    for (const std::string& arg : configuration.args)
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
        Process::Redirections redirections = Process::Redirections::processStdErr;
        Process process(intermediateCompileCommand, redirections);
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + intermediateCompileCommand + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("compiling intermediate code '" + intermediateCodeFile + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
}

void CmCppCodeGenerator::Visit(BoundCompileUnit& boundCompileUnit)
{
    std::string intermediateFilePath = Path::ChangeExtension(boundCompileUnit.LLFilePath(), ".cpp");
    NativeModule nativeModule(emitter, intermediateFilePath);
    compileUnitId = boundCompileUnit.Id();
    symbolTable = &boundCompileUnit.GetSymbolTable();
    module = &boundCompileUnit.GetModule();
    compileUnit = &boundCompileUnit;
    if (!module->IsCore())
    {
        module->AddCompileUnitId(compileUnitId);
    }
    TypeSymbol* longType = module->GetSymbolTable().GetTypeByName(U"long");
    module->GetTypeIndex().AddType(longType->TypeId(), longType, *emitter);
    emitter->SetCompileUnitId(compileUnitId);
    emitter->SetCurrentLineNumber(0);
    generateLineNumbers = false;
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
    GenerateInitUnwindInfoFunction(boundCompileUnit);
    GenerateInitCompileUnitFunction(boundCompileUnit);
    if (boundCompileUnit.GetGlobalInitializationFunctionSymbol() != nullptr)
    {
        GenerateGlobalInitFuncion(boundCompileUnit);
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
    if (compileUnit->CodeGenerated(functionSymbol)) return;
    compileUnit->SetCodeGenerated(functionSymbol);
    void* functionType = functionSymbol->IrType(*emitter);
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    lastAlloca = nullptr;
    handlerBlock = nullptr;
    cleanupBlock = nullptr;
    labeledStatementMap.clear();
    cleanups.clear();
    cleanupMap.clear();
    tryIndexCleanupTryBlockMap.clear();
    tryIndexMap.clear();
    numTriesInCurrentBlock = 0;
    tryIndex = 0;
    prevLineNumber = 0;
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
    function = emitter->GetOrInsertFunction(ToUtf8(functionSymbol->MangledName()), functionType, functionSymbol->DontThrow());
    if (GetGlobalFlag(GlobalFlags::release) && functionSymbol->IsInline())
    {
        emitter->AddInlineFunctionAttribute(function);
        functionSymbol->SetLinkOnceOdrLinkage();
    }
    else if (functionSymbol->IsGeneratedFunction())
    {
        emitter->AddInlineFunctionAttribute(function);
        functionSymbol->SetLinkOnceOdrLinkage();
    }
    if (functionSymbol->HasLinkOnceOdrLinkage())
    {
        void* comdat = emitter->GetOrInsertAnyFunctionComdat(ToUtf8(functionSymbol->MangledName()), function);
        emitter->SetFunctionLinkageToLinkOnceODRLinkage(function);
    }
    int32_t fileIndex = -1;
    boost::uuids::uuid functionId;
    if (functionSymbol->HasSource())
    {
        fileIndex = functionSymbol->GetSpan().fileIndex;
        functionId = functionSymbol->FunctionId();
        module->GetFileIndex().AddFile(fileIndex, module);
        module->GetFunctionIndex().AddFunction(functionId, functionSymbol);
        if (functionSymbol == module->GetSymbolTable().MainFunctionSymbol())
        {
            module->GetFunctionIndex().SetMainFunctionId(functionId);
        }
    }
    emitter->SetFunction(function, fileIndex, functionId);
    emitter->SetFunctionName(ToUtf8(functionSymbol->FullName()));
    void* entryBlock = emitter->CreateBasicBlock("entry");
    if (functionSymbol->HasSource())
    {
        emitter->BeginScope();
    }
    entryBasicBlock = entryBlock;
    emitter->SetCurrentBasicBlock(entryBlock);
    emitter->PushParentBlock();
    if (functionSymbol->HasSource())
    {
        emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::beginBrace));
        emitter->CreateNop();
        emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::beginBrace));
    }
    emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::entryCode));
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
        if (functionSymbol->HasSource())
        {
            const boost::uuids::uuid& typeId = parameter->GetType()->TypeId();
            module->GetTypeIndex().AddType(typeId, parameter->GetType(), *emitter);
            emitter->AddLocalVariable(ToUtf8(parameter->Name()), typeId, parameter->IrObject(*emitter));
        }
        lastAlloca = allocaInst;
    }
    if (functionSymbol->ReturnParam())
    {
        ParameterSymbol* parameter = functionSymbol->ReturnParam();
        void* allocaInst = emitter->CreateAlloca(parameter->GetType()->IrType(*emitter));
        emitter->SetIrObject(parameter, allocaInst);
        if (functionSymbol->HasSource())
        {
            const boost::uuids::uuid& typeId = parameter->GetType()->TypeId();
            module->GetTypeIndex().AddType(typeId, parameter->GetType(), *emitter);
            emitter->AddLocalVariable(ToUtf8(parameter->Name()), typeId, parameter->IrObject(*emitter));
        }
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
    if (!functionSymbol->DontThrow())
    {
        GenerateEnterFunctionCode(boundFunction);
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
            void* callee = emitter->GetOrInsertFunction(ToUtf8(copyConstructor->MangledName()), copyCtorType, copyConstructor->DontThrow());
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
    emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::entryCode));
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
        GenerateExitFunctionCode(boundFunction);
        if (functionSymbol->ReturnType() && functionSymbol->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol && !functionSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
        {
            void* defaultValue = functionSymbol->ReturnType()->CreateDefaultIrValue(*emitter);
            if (generateLineNumbers)
            {
                emitter->SetCurrentLineNumber(body->EndSpan().line);
            }
            emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
            emitter->CreateRet(defaultValue);
            emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
            lastInstructionWasRet = true;
        }
        else
        {
            if (generateLineNumbers)
            {
                emitter->SetCurrentLineNumber(body->EndSpan().line);
            }
            emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
            emitter->CreateRetVoid();
            emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
            lastInstructionWasRet = true;
        }
    }
    emitter->PopParentBlock();
    emitter->FinalizeFunction(function, functionSymbol->HasCleanup());
    if (functionSymbol->HasSource())
    {
        emitter->EndScope();
    }
}

void CmCppCodeGenerator::Visit(BoundCompoundStatement& boundCompoundStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundCompoundStatement.GetSpan().line);
        emitter->BeginScope();
        if (&boundCompoundStatement != currentFunction->Body())
        {
            emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::beginBrace));
            emitter->SetCurrentLineNumber(boundCompoundStatement.GetSpan().line);
            emitter->CreateNop();
            emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::beginBrace));
        }
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    int prevNumTriesInCurrentBlock = numTriesInCurrentBlock;
    numTriesInCurrentBlock = 0;
    SetTarget(&boundCompoundStatement);
    BoundCompoundStatement* prevBlock = currentBlock;
    currentBlock = &boundCompoundStatement;
    blockDestructionMap[currentBlock] = std::vector<std::unique_ptr<BoundFunctionCall>>();
    blocks.push_back(currentBlock);
    SetLineNumber(boundCompoundStatement.GetSpan().line);
    int n = boundCompoundStatement.Statements().size();
    for (int i = 0; i < n; ++i)
    {
        BoundStatement* statement = boundCompoundStatement.Statements()[i].get();
        statement->Accept(*this);
    }
    ExitBlocks(prevBlock);
    GenerateCodeForCleanups();
    for (int i = 0; i < numTriesInCurrentBlock; ++i)
    {
        emitter->PopParentBlock();
    }
    if (generateLineNumbers)
    {
        if (&boundCompoundStatement != currentFunction->Body())
        {
            emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
            emitter->SetCurrentLineNumber(boundCompoundStatement.EndSpan().line);
            emitter->CreateNop();
            emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
        }
        emitter->EndScope();
    }
    blocks.pop_back();
    currentBlock = prevBlock;
    numTriesInCurrentBlock = prevNumTriesInCurrentBlock;
    if (boundCompoundStatement.Parent() && boundCompoundStatement.Parent()->GetBoundNodeType() == BoundNodeType::boundTryStatement)
    {
        emitter->CreateBr(currentTryNextBlock);
    }
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
        boundReturnStatement.ReturnFunctionCall()->Accept(*this);
        void* returnValue = emitter->Stack().Pop();
        if (sequenceSecond)
        {
            sequenceSecond->SetGenerated();
            sequenceSecond->Accept(*this);
        }
        ExitBlocks(nullptr);
        GenerateExitFunctionCode(*currentFunction);
        if (generateLineNumbers)
        {
            emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
            emitter->SetCurrentLineNumber(currentBlock->EndSpan().line);
        }
        emitter->CreateRet(returnValue);
        if (generateLineNumbers)
        {
            emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
        }
        lastInstructionWasRet = true;
    }
    else
    {
        ExitBlocks(nullptr);
        GenerateExitFunctionCode(*currentFunction);
        if (generateLineNumbers)
        {
            emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
            emitter->SetCurrentLineNumber(currentBlock->EndSpan().line);
        }
        emitter->CreateRetVoid();
        if (generateLineNumbers)
        {
            emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::endBrace));
        }
        lastInstructionWasRet = true;
    }
    BoundCompoundStatement* body = currentFunction->Body();
    BoundStatement* lastStatement = nullptr;
    if (!body->Statements().empty())
    {
        lastStatement = body->Statements().back().get();
    }
    if (lastStatement && !lastStatement->IsOrContainsBoundReturnStatement())
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
        LocalVariableSymbol* localVariable = boundConstructionStatement.GetLocalVariable();
        if (localVariable)
        {
            const boost::uuids::uuid& typeId = localVariable->GetType()->TypeId();
            module->GetTypeIndex().AddType(typeId, localVariable->GetType(), *emitter);
            emitter->AddLocalVariable(ToUtf8(localVariable->Name()), typeId, localVariable->IrObject(*emitter));
        }
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
                        void* cleanupTry = emitter->CreateBasicBlock("cleanupTry");
                        emitter->CreateBr(cleanupTry);
                        emitter->SetCurrentBasicBlock(cleanupTry);
                        emitter->PushParentBlock();
                        emitter->CreateBeginTry();
                        tryIndexMap[numTriesInCurrentBlock] = tryIndex;
                        tryIndexCleanupTryBlockMap[tryIndex] = cleanupTry;
                        std::unique_ptr<BoundExpression> classPtrArgument(firstArgument->Clone());
                        std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(module, currentBlock->EndSpan(), classType->Destructor()));
                        destructorCall->AddArgument(std::move(classPtrArgument));
                        GenerateCleanup(tryIndex, destructorCall.get());
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
                        ++numTriesInCurrentBlock;
                        ++tryIndex;
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
    emitter->CreateNop();
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
    void* tryNextBlock = emitter->CreateBasicBlock("tryNext");
    void* prevTryNextBlock = currentTryNextBlock;
    currentTryNextBlock = tryNextBlock;
    cleanupBlock = nullptr;
    bool prevInTryBlock = inTryBlock;
    inTryBlock = true;
    void* tryBlock = emitter->CreateBasicBlock("try");
    emitter->CreateBr(tryBlock);
    emitter->SetCurrentBasicBlock(tryBlock);
    emitter->PushParentBlock();
    emitter->CreateBeginTry();
    boundTryStatement.TryBlock()->Accept(*this);
    inTryBlock = prevInTryBlock;
    emitter->SetCurrentBasicBlock(tryBlock);
    emitter->CreateEndTry(tryNextBlock);
    emitter->CreateBeginCatch();
    handlerBlock = emitter->CreateBasicBlock("handlers");
    emitter->CreateBr(handlerBlock);
    emitter->CreateIncludeBasicBlockInstruction(handlerBlock);
    emitter->SetHandlerBlock(tryBlock, handlerBlock);
    emitter->SetCurrentBasicBlock(handlerBlock);
    emitter->PushParentBlock();
    void* resumeBlock = nullptr;
    int n = boundTryStatement.Catches().size();
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
        void* handleException = emitter->GetOrInsertFunction("RtHandleException", handleExceptionFunctionType, true);
        void* handleThisEx = emitter->CreateCall(handleException, handleExceptionArgs);
        void* nextHandlerTarget = nullptr;
        if (i < n - 1)
        {
            nextHandlerTarget = emitter->CreateBasicBlock("catch");
        }
        else
        {
            resumeBlock = emitter->CreateBasicBlock("resume");
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
    currentFunction->GetFunctionSymbol()->SetHasCleanup();
    emitter->CreateBr(tryNextBlock);
    emitter->PopParentBlock();
    emitter->SetCurrentBasicBlock(tryBlock);
    emitter->CreateEndCatch(nullptr);
    emitter->CreateBr(tryNextBlock);
    basicBlockOpen = false;
    emitter->PopParentBlock();
    emitter->SetCurrentBasicBlock(tryNextBlock);
    currentTryNextBlock = prevTryNextBlock;
    cleanupBlock = prevCleanupBlock;
    basicBlockOpen = true;
}

void CmCppCodeGenerator::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    if (generateLineNumbers)
    {
        emitter->SetCurrentLineNumber(boundRethrowStatement.GetSpan().line);
    }
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundRethrowStatement);
    boundRethrowStatement.ReleaseCall()->Accept(*this);
    emitter->CreateResume(nullptr);
    currentFunction->GetFunctionSymbol()->SetHasCleanup();
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

void CmCppCodeGenerator::GenerateCleanup(int tryIndex, BoundFunctionCall* destructorCall)
{
    cleanupBlock = emitter->CreateBasicBlock("cleanup");
    emitter->SetCleanupBlock(cleanupBlock);
    std::unique_ptr<Cleanup> cleanup(new Cleanup(cleanupBlock));
    cleanup->destructors.push_back(std::unique_ptr<BoundFunctionCall>(static_cast<BoundFunctionCall*>(destructorCall->Clone())));
    cleanupMap[tryIndex] = cleanup.get();
    cleanups.push_back(std::move(cleanup));
}

void CmCppCodeGenerator::GenerateCodeForCleanups()
{
    for (int i = 0; i < numTriesInCurrentBlock; ++i)
    {
        --tryIndex;
        auto cleanupIt = cleanupMap.find(tryIndex);
        if (cleanupIt != cleanupMap.cend())
        {
            Cleanup* cleanup = cleanupIt->second;
            auto it = tryIndexCleanupTryBlockMap.find(tryIndex);
            if (it != tryIndexCleanupTryBlockMap.cend())
            {
                void* cleanupTryBlock = it->second;
                void* prevBasicBlock = emitter->CurrentBasicBlock();
                emitter->SetCurrentBasicBlock(cleanupTryBlock);
                emitter->CreateEndTry(nullptr);
                emitter->SetCurrentBasicBlock(cleanupTryBlock);
                emitter->CreateBeginCatch();
                for (const std::unique_ptr<BoundFunctionCall>& destructorCall : cleanup->destructors)
                {
                    destructorCall->Accept(*this);
                }
                emitter->SetCurrentBasicBlock(cleanupTryBlock);
                emitter->CreateResume(nullptr);
                emitter->CreateEndCatch(nullptr);
                currentFunction->GetFunctionSymbol()->SetHasCleanup();
                emitter->SetCurrentBasicBlock(prevBasicBlock);
            }
            else
            {
                throw std::runtime_error("internal error: try index " + std::to_string(tryIndex) + " not found");
            }
        }
        else
        {
            throw std::runtime_error("internal error: cleanup for try index " + std::to_string(tryIndex) + " not found");
        }
    }
}

void CmCppCodeGenerator::SetLineNumber(int32_t lineNumber)
{
    if (currentFunction->GetFunctionSymbol()->DontThrow()) return;
    if (prevLineNumber == lineNumber) return;
    prevLineNumber = lineNumber;
    BoundStatement* setLineNumberStatement = currentFunction->GetLineCode();
    if (setLineNumberStatement)
    {
        bool prevGenJumpingBoolCode = genJumpingBoolCode;
        genJumpingBoolCode = false;
        emitter->BeginSubstituteLineNumber(lineNumber);
        setLineNumberStatement->Accept(*this);
        emitter->EndSubstituteLineNumber();
        genJumpingBoolCode = prevGenJumpingBoolCode;
    }
}

std::string CmCppCodeGenerator::GetSourceFilePath(int32_t fileIndex)
{
    return module->GetFilePath(fileIndex);
}

void CmCppCodeGenerator::GenerateEnterFunctionCode(BoundFunction& boundFunction)
{
    const std::vector<std::unique_ptr<BoundStatement>>& enterCode = boundFunction.EnterCode();
    if (enterCode.empty()) return;
    compileUnitFunctions.insert(boundFunction.GetFunctionSymbol());
    LocalVariableSymbol* prevUnwindInfoVar = boundFunction.GetFunctionSymbol()->PrevUnwindInfoVar();
    void* prevUnwindInfoAlloca = emitter->CreateAlloca(prevUnwindInfoVar->GetType()->IrType(*emitter));
    emitter->SetIrObject(prevUnwindInfoVar, prevUnwindInfoAlloca);
    LocalVariableSymbol* unwindInfoVar = boundFunction.GetFunctionSymbol()->UnwindInfoVar();
    void* unwindInfoAlloca = emitter->CreateAlloca(unwindInfoVar->GetType()->IrType(*emitter));
    emitter->SetIrObject(unwindInfoVar, unwindInfoAlloca);
    lastAlloca = unwindInfoAlloca;
    for (const auto& statement : enterCode)
    {
        statement->Accept(*this);
    }
}

void CmCppCodeGenerator::GenerateExitFunctionCode(BoundFunction& boundFunction)
{
    const std::vector<std::unique_ptr<BoundStatement>>& exitCode = boundFunction.ExitCode();
    if (exitCode.empty()) return;
    emitter->BeginInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::exitCode));
    for (const auto& statement : exitCode)
    {
        statement->Accept(*this);
    }
    emitter->EndInstructionFlag(static_cast<int16_t>(cmajor::debug::InstructionFlags::exitCode));
}

void CmCppCodeGenerator::GenerateInitUnwindInfoFunction(BoundCompileUnit& boundCompileUnit)
{
    FunctionSymbol* initUnwindInfoFunctionSymbol = boundCompileUnit.GetInitUnwindInfoFunctionSymbol();
    if (!initUnwindInfoFunctionSymbol) return;
    if (compileUnitFunctions.empty()) return;
    FunctionSymbol* addCompileUnitFunctionSymbol = boundCompileUnit.GetSystemRuntimeAddCompileUnitFunctionSymbol();
    if (!addCompileUnitFunctionSymbol) return;
    void* functionType = initUnwindInfoFunctionSymbol->IrType(*emitter);
    void* function = emitter->GetOrInsertFunction(ToUtf8(initUnwindInfoFunctionSymbol->MangledName()), functionType, true);
    emitter->SetFunction(function, -1, boost::uuids::nil_uuid());
    emitter->SetFunctionName(ToUtf8(initUnwindInfoFunctionSymbol->FullName()));
    void* entryBlock = emitter->CreateBasicBlock("entry");
    emitter->SetCurrentBasicBlock(entryBlock);
    for (FunctionSymbol* compileUnitFunction : compileUnitFunctions)
    {
        std::unique_ptr<BoundFunctionCall> boundFunctionCall(new BoundFunctionCall(module, compileUnitFunction->GetSpan(), addCompileUnitFunctionSymbol));
        BoundBitCast* functionPtrAsVoidPtr = new BoundBitCast(module, std::unique_ptr<BoundExpression>(
                new BoundFunctionPtr(module, compileUnitFunction->GetSpan(), compileUnitFunction, symbolTable->GetTypeByName(U"void")->AddPointer(compileUnitFunction->GetSpan()))),
                symbolTable->GetTypeByName(U"void")->AddPointer(compileUnitFunction->GetSpan()));
        boundFunctionCall->AddArgument(std::unique_ptr<BoundExpression>(functionPtrAsVoidPtr));
        std::string functionName = ToUtf8(compileUnitFunction->FullName());
        int functionNameStringId = Install(functionName);
        BoundLiteral* boundFunctionNameLiteral = new BoundLiteral(module, std::unique_ptr<Value>(new StringValue(compileUnitFunction->GetSpan(), functionNameStringId, functionName)),
            symbolTable->GetTypeByName(U"char")->AddConst(compileUnitFunction->GetSpan())->AddPointer(compileUnitFunction->GetSpan()));
        boundFunctionCall->AddArgument(std::unique_ptr<BoundExpression>(boundFunctionNameLiteral));
        std::string sourceFilePath = GetSourceFilePath(compileUnitFunction->GetSpan().fileIndex);
        int sourceFilePathStringId = Install(sourceFilePath);
        BoundLiteral* boundSourceFilePathLiteral = new BoundLiteral(module, std::unique_ptr<Value>(new StringValue(compileUnitFunction->GetSpan(), sourceFilePathStringId, sourceFilePath)),
            symbolTable->GetTypeByName(U"char")->AddConst(compileUnitFunction->GetSpan())->AddPointer(compileUnitFunction->GetSpan()));
        boundFunctionCall->AddArgument(std::unique_ptr<BoundExpression>(boundSourceFilePathLiteral));
        boundFunctionCall->Accept(*this);
    }
    emitter->CreateRetVoid();
}

void CmCppCodeGenerator::GenerateInitCompileUnitFunction(BoundCompileUnit& boundCompileUnit)
{
    generateLineNumbers = false;
    emitter->SetCurrentLineNumber(0);
    FunctionSymbol* initCompileUnitFunctionSymbol = boundCompileUnit.GetInitCompileUnitFunctionSymbol();
    if (!initCompileUnitFunctionSymbol) return;
    Span span = initCompileUnitFunctionSymbol->GetSpan();
    void* functionType = initCompileUnitFunctionSymbol->IrType(*emitter);
    void* function = emitter->GetOrInsertFunction(ToUtf8(initCompileUnitFunctionSymbol->MangledName()), functionType, true);
    emitter->SetFunction(function, -1, boost::uuids::nil_uuid());
    emitter->SetFunctionName(ToUtf8(initCompileUnitFunctionSymbol->FullName()));
    void* entryBlock = emitter->CreateBasicBlock("entry");
    emitter->SetCurrentBasicBlock(entryBlock);
    FunctionSymbol* initUnwindInfoFunctionSymbol = boundCompileUnit.GetInitUnwindInfoFunctionSymbol();
    if (!initUnwindInfoFunctionSymbol)
    {
        emitter->CreateRetVoid();
        return;
    }
    FunctionSymbol* pushCompileUnitUnwindInfoInitFunctionSymbol = boundCompileUnit.GetPushCompileUnitUnwindInfoInitFunctionSymbol();
    TypeSymbol* initUnwindInfoDelegateType = boundCompileUnit.GetInitUnwindInfoDelegateType();
    GlobalVariableSymbol* compileUnitUnwindInfoVarSymbol = boundCompileUnit.GetCompileUnitUnwindInfoVarSymbol();
    BoundGlobalVariable* boundCompileUnitUnwindInfoVar = new BoundGlobalVariable(module, span, compileUnitUnwindInfoVarSymbol);
    BoundAddressOfExpression* unwindInfoVarAddress = new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundCompileUnitUnwindInfoVar),
        boundCompileUnitUnwindInfoVar->GetType()->AddPointer(span));
    BoundFunctionPtr* boundInitUnwindInfoFunction = new BoundFunctionPtr(module, span, initUnwindInfoFunctionSymbol, initUnwindInfoDelegateType);
    std::unique_ptr<BoundFunctionCall> boundFunctionCall(new BoundFunctionCall(module, span, pushCompileUnitUnwindInfoInitFunctionSymbol));
    boundFunctionCall->AddArgument(std::unique_ptr<BoundExpression>(boundInitUnwindInfoFunction));
    boundFunctionCall->AddArgument(std::unique_ptr<BoundExpression>(unwindInfoVarAddress));
    boundFunctionCall->Accept(*this);
    emitter->CreateRetVoid();
}

void CmCppCodeGenerator::GenerateGlobalInitFuncion(BoundCompileUnit& boundCompileUnit)
{
    generateLineNumbers = false;
    emitter->SetCurrentLineNumber(0);
    FunctionSymbol* globalInitFunctionSymbol = boundCompileUnit.GetGlobalInitializationFunctionSymbol();
    if (!globalInitFunctionSymbol) return;
    Span span = globalInitFunctionSymbol->GetSpan();
    void* functionType = globalInitFunctionSymbol->IrType(*emitter);
    void* function = emitter->GetOrInsertFunction(ToUtf8(globalInitFunctionSymbol->MangledName()), functionType, true);
    emitter->SetFunction(function, -1, boost::uuids::nil_uuid());
    emitter->SetFunctionName(ToUtf8(globalInitFunctionSymbol->FullName()));
    void* entryBlock = emitter->CreateBasicBlock("entry");
    emitter->SetCurrentBasicBlock(entryBlock);
    const std::vector<std::unique_ptr<FunctionSymbol>>& allCompileUnitInitFunctionSymbols = boundCompileUnit.AllCompileUnitInitFunctionSymbols();
    for (const std::unique_ptr<FunctionSymbol>& initCompileUnitFunctionSymbol : allCompileUnitInitFunctionSymbols)
    {
        std::unique_ptr<BoundFunctionCall> boundFunctionCall(new BoundFunctionCall(module, span, initCompileUnitFunctionSymbol.get()));
        boundFunctionCall->Accept(*this);
    }
    emitter->CreateRetVoid();
}

} } // namespace cmajor::codegencpp
