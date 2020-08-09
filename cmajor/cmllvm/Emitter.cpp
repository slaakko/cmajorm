// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmllvm/Emitter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/System.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Process.hpp>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/SourceMgr.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace cmllvm {

using namespace soulng::util;

const unsigned cmajorLanguageTag = llvm::dwarf::DW_LANG_C_plus_plus_11; // closest approximation

Emitter::Emitter(llvm::LLVMContext& context_, cmajor::ir::EmittingContext& emittingContext_) :
    cmajor::ir::Emitter(&stack), emittingContext(emittingContext_), emittingDelegate(nullptr),
    context(context_), builder(context), module(nullptr), dataLayout(nullptr), diCompileUnit(nullptr), diFile(nullptr), diBuilder(nullptr), columnSpanProvider(nullptr), compileUnitIndex(-1),
    stack(), objectPointer(nullptr), function(nullptr), currentBasicBlock(nullptr), inPrologue(false), currentDIBuilder(nullptr), substituteLineNumber(false), currentLineNumber(-1)
{
}

void* Emitter::GetIrTypeForBool()
{
    return llvm::Type::getInt1Ty(context); 
}

void* Emitter::GetIrTypeForSByte()
{
    return llvm::Type::getInt8Ty(context);
}

void* Emitter::GetIrTypeForByte()
{
    return llvm::Type::getInt8Ty(context);
}

void* Emitter::GetIrTypeForShort()
{
    return llvm::Type::getInt16Ty(context);
}

void* Emitter::GetIrTypeForUShort()
{
    return llvm::Type::getInt16Ty(context);
}

void* Emitter::GetIrTypeForInt()
{
    return llvm::Type::getInt32Ty(context);
}

void* Emitter::GetIrTypeForUInt()
{
    return llvm::Type::getInt32Ty(context);
}

void* Emitter::GetIrTypeForLong()
{
    return llvm::Type::getInt64Ty(context);
}

void* Emitter::GetIrTypeForULong()
{
    return llvm::Type::getInt64Ty(context);
}

void* Emitter::GetIrTypeForFloat()
{
    return llvm::Type::getFloatTy(context);
}

void* Emitter::GetIrTypeForDouble()
{
    return llvm::Type::getDoubleTy(context);
}

void* Emitter::GetIrTypeForChar()
{
    return llvm::Type::getInt8Ty(context);
}

void* Emitter::GetIrTypeForWChar()
{
    return llvm::Type::getInt16Ty(context);
}

void* Emitter::GetIrTypeForUChar()
{
    return llvm::Type::getInt32Ty(context);
}

void* Emitter::GetIrTypeForVoid()
{
    return llvm::Type::getVoidTy(context);
}

void* Emitter::GetIrTypeForFunction(void* retType, const std::vector<void*>& paramTypes)
{
    std::vector<llvm::Type*> parameterTypes;
    for (void* paramType : paramTypes)
    {
        parameterTypes.push_back(static_cast<llvm::Type*>(paramType));
    }
    return llvm::FunctionType::get(static_cast<llvm::Type*>(retType), parameterTypes, false);
}

void* Emitter::GetIrTypeForVariableParamFunction(void* retType)
{
    return llvm::FunctionType::get(static_cast<llvm::Type*>(retType), true);
}

void* Emitter::GetIrTypeForArrayType(void* elementType, int64_t size)
{
    llvm::Type* elementIrType = static_cast<llvm::Type*>(elementType);
    return llvm::ArrayType::get(elementIrType, size);
}

void* Emitter::GetIrTypeForClassType(const std::vector<void*>& elementTypes)
{
    std::vector<llvm::Type*> elementIrTypes;
    for (void* elementType : elementTypes)
    {
        elementIrTypes.push_back(static_cast<llvm::Type*>(elementType));
    }
    return llvm::StructType::get(context, elementIrTypes);
}

void* Emitter::CreateFwdIrTypeForClassType()
{
    return llvm::StructType::create(context);
}

void Emitter::SetFwdIrTypeBody(void* forwardDeclaredType, const std::vector<void*>& elementTypes)
{
    llvm::StructType* structType = static_cast<llvm::StructType*>(forwardDeclaredType);
    std::vector<llvm::Type*> elementIrTypes;
    for (void* elementType : elementTypes)
    {
        elementIrTypes.push_back(static_cast<llvm::Type*>(elementType));
    }
    structType->setBody(elementIrTypes);
}

void* Emitter::GetIrTypeForDelegateType(void* retType, const std::vector<void*>& paramTypes)
{
    llvm::Type* returnType = static_cast<llvm::Type*>(retType);
    std::vector<llvm::Type*> parameterTypes;
    for (void* paramType : paramTypes)
    {
        parameterTypes.push_back(static_cast<llvm::Type*>(paramType));
    }
    return llvm::PointerType::get(llvm::FunctionType::get(returnType, parameterTypes, false), 0);
}

void* Emitter::GetIrTypeForVoidPtrType()
{
    return llvm::Type::getInt8PtrTy(context);
}

void* Emitter::GetIrTypeForStructType(const std::vector<void*>& elementTypes)
{
    std::vector<llvm::Type*> elementIrTypes;
    for (void* elementType : elementTypes)
    {
        elementIrTypes.push_back(static_cast<llvm::Type*>(elementType));
    }
    return llvm::StructType::get(context, elementIrTypes);
}

void* Emitter::CreateDITypeForEnumConstant(const std::string& name, int64_t value)
{
    return diBuilder->createEnumerator(name, value);
}

void* Emitter::CreateDITypeForEnumType(const std::string& name, const std::string& mangledName, const Span& span, const std::vector<void*>& enumConstantElements, uint64_t sizeInBits,
    uint32_t alignInBits, void* underlyingDIType)
{
    std::vector<llvm::Metadata*> elements;
    for (void* element : enumConstantElements)
    {
        elements.push_back(static_cast<llvm::Metadata*>(element));
    }
    return diBuilder->createEnumerationType(nullptr, name, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)), span.line, sizeInBits, alignInBits,
        diBuilder->getOrCreateArray(elements), static_cast<llvm::DIType*>(underlyingDIType), mangledName);
}

void Emitter::MapFwdDeclaration(void* fwdDeclaration, const boost::uuids::uuid& typeId)
{
    fwdDeclarationMap[static_cast<llvm::DIType*>(fwdDeclaration)] = typeId;
}

void Emitter::SaveObjectPointer(void* objectPointer_)
{
    if (objectPointer == nullptr)
    {
        objectPointer = static_cast<llvm::Value*>(objectPointer_);
    }
}

void Emitter::SetFunctionName(const std::string& functionName)
{
}

void Emitter::BeginScope()
{
}

void Emitter::EndScope()
{
}

int16_t Emitter::GetCurrentScopeId() const
{
    return 0;
}

void Emitter::SetCurrentScopeId(int16_t scopeId)
{
}

int32_t Emitter::AddControlFlowGraphNode()
{
    return -1;
}

void Emitter::SetCurrentControlFlowGraphNodeId(int32_t controlFlowGraphNodeId)
{
}

void Emitter::AddControlFlowGraphEdge(int32_t startNodeId, int32_t endNodeId)
{
}

void Emitter::AddLocalVariable(const std::string& localVariableName, const boost::uuids::uuid& typeId, void* irObject)
{
}

void Emitter::BeginInstructionFlag(int16_t flag)
{
}

void Emitter::EndInstructionFlag(int16_t flag)
{
}

void Emitter::PushScope(void* scope)
{
    scopes.push_back(static_cast<llvm::DIScope*>(scope));
}

void Emitter::PopScope()
{
    scopes.pop_back();
}

void* Emitter::CurrentScope()
{
    llvm::DIScope* currentScope = diCompileUnit;
    if (!scopes.empty())
    {
        currentScope = scopes.back();
    }
    return currentScope;
}

int Emitter::GetColumn(const Span& span) const
{
    int column = 1;
    if (columnSpanProvider)
    {
        column = columnSpanProvider->GetColumn(span);
    }
    return column;
}

void Emitter::StartDebugInfo(const std::string& sourceFilePath, const std::string& compilerVersion, bool optimized, cmajor::common::ColumnSpanProvider* columnSpanProvider_)
{
    module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
#ifdef _WIN32
    module->addModuleFlag(llvm::Module::Warning, "CodeView", 1);
#endif
    diBuilder.reset(new llvm::DIBuilder(*module));
    currentDIBuilder = diBuilder.get();
    llvm::DIFile* sourceFile = diBuilder->createFile(Path::GetFileName(sourceFilePath), Path::GetDirectoryName(sourceFilePath));
    SetDIFile(sourceFile);
    llvm::DICompileUnit* diCompileUnit = diBuilder->createCompileUnit(cmajorLanguageTag, sourceFile, "Cmajor compiler version " + compilerVersion, optimized, "", 0);
    SetDICompileUnit(diCompileUnit);
    SetColumnSpanProvider(columnSpanProvider_);
    PushScope(sourceFile);
}

void Emitter::FinalizeDebugInfo()
{
    ReplaceForwardDeclarations();
    diBuilder->finalize();
}

void Emitter::EndDebugInfo()
{
    PopScope();
    diBuilder.reset();
}

void* Emitter::CreateDebugInfoForNamespace(void* scope, const std::string& name)
{
    llvm::DINamespace* ns = diBuilder->createNameSpace(static_cast<llvm::DIScope*>(CurrentScope()), name, false);
    return ns;
}

void Emitter::EmitIrText(const std::string& filePath)
{
    std::ofstream llFile(filePath);
    llvm::raw_os_ostream llOs(llFile);
    module->print(llOs, nullptr);
    llOs.flush();
}

void Emitter::EmitIrFile(const std::string& filePath)
{
    std::ofstream bcFile(filePath, std::ios::binary);
    llvm::raw_os_ostream bcOs(bcFile);
    llvm::WriteBitcodeToFile(*module, bcOs);
    bcOs.flush();
}

void Emitter::Optimize(const std::string& bcFilePath, const std::string& optBCFilePath, const std::string& optimizationFlags)
{
    std::string optErrors;
    std::string optCommandLine;
    optCommandLine.append("opt ").append(optimizationFlags).append(" -o=").append(QuotedPath(optBCFilePath)).append(" ").append(QuotedPath(bcFilePath));
    try
    {
        Process process(optCommandLine, Process::Redirections::processStdErr);
        optErrors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + optCommandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("optimization of '" + bcFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + optErrors);
    }
}

void Emitter::Disassemble(const std::string& bcFilePath, const std::string& llFilePath)
{
    std::string disCommandLine;
    std::string errors;
    disCommandLine.append("llvm-dis -o=").append(QuotedPath(llFilePath)).append(" ").append(QuotedPath(bcFilePath));
    try
    {
        Process process(disCommandLine, Process::Redirections::processStdErr);
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + disCommandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("disassembly of '" + bcFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
}

void Emitter::Compile(const std::string& bcFilePath, const std::string& objectFilePath, int optimizationLevel)
{
/*
    llvm::SMDiagnostic error;
    std::unique_ptr<llvm::Module> mod = llvm::parseIRFile(bcFilePath, error, context);
    auto it = mod->begin();
    bool modified = false;
    while (it != mod->end())
    {
        if (it->getUnnamedAddr() == llvm::GlobalValue::UnnamedAddr::Local)
        {
            it->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::None);
            modified = true;
        }
        ++it;
    }
    if (modified)
    {
        std::ofstream bcFile(bcFilePath, std::ios::binary);
        llvm::raw_os_ostream bcOs(bcFile);
        llvm::WriteBitcodeToFile(*mod, bcOs);
        bcOs.flush();
    }
*/
    std::string llcCommandLine;
    std::string errors;
    llcCommandLine.append("llc -O=").append(std::to_string(optimizationLevel)).append(" --filetype=obj").append(" -o=").append(QuotedPath(objectFilePath).append(" ").append(QuotedPath(bcFilePath)));
    try
    {
        Process process(llcCommandLine, Process::Redirections::processStdErr);
        errors = process.ReadToEnd(Process::StdHandle::stdErr);
        process.WaitForExit();
        int exitCode = process.ExitCode();
        if (exitCode != 0)
        {
            throw std::runtime_error("executing '" + llcCommandLine + "' failed with exit code: " + std::to_string(exitCode));
        }
    }
    catch (const std::exception & ex)
    {
        throw std::runtime_error("compilation of '" + bcFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
}

void Emitter::VerifyModule()
{
    std::string errorMessageStore;
    llvm::raw_string_ostream errorMessage(errorMessageStore);
    if (llvm::verifyModule(*module, &errorMessage)) 
    {
        throw std::runtime_error("Emitter: verification of module '" + module->getSourceFileName() + "' failed. " + errorMessage.str());
    }
}

void Emitter::EmitObjectCodeFile(const std::string& objectFilePath)
{
    llvm::legacy::PassManager passManager;
    std::error_code errorCode;
    llvm::raw_fd_ostream objectFile(objectFilePath, errorCode, llvm::sys::fs::F_None);
#if (LLVM_VERSION_MAJOR >= 10)
    if (static_cast<llvm::TargetMachine*>(emittingContext.TargetMachine())->addPassesToEmitFile(passManager, objectFile, nullptr, llvm::CodeGenFileType::CGFT_ObjectFile))
    {
        throw std::runtime_error("Emitter: cannot emit object code file '" + objectFilePath + "': addPassesToEmitFile failed");
    }
#else
    if (static_cast<llvm::TargetMachine*>(emittingContext.TargetMachine())->addPassesToEmitFile(passManager, objectFile, nullptr, llvm::TargetMachine::CodeGenFileType::CGFT_ObjectFile))
    {
        throw std::runtime_error("Emitter: cannot emit object code file '" + objectFilePath + "': addPassesToEmitFile failed");
    }
#endif
    passManager.run(*module);
    objectFile.flush();
    if (objectFile.has_error())
    {
        throw std::runtime_error("Emitter: could not emit object code file '" + objectFilePath + "': " + errorCode.message());
    }
}

llvm::DebugLoc Emitter::GetDebugLocation(const Span& span)
{
    if (!diCompileUnit || !span.Valid() || !currentDIBuilder) return llvm::DebugLoc();
    int column = GetColumn(span);
    return llvm::DebugLoc::get(span.line, column, static_cast<llvm::DIScope*>(CurrentScope()));
}

void Emitter::ResetCurrentDebugLocation()
{
    currentDebugLocation = llvm::DebugLoc();
    builder.SetCurrentDebugLocation(currentDebugLocation);
}

void Emitter::SetCurrentDebugLocation(const Span& span)
{
    if (!diCompileUnit || !currentDIBuilder) return;
    if (inPrologue || !span.Valid())
    {
        currentDebugLocation = llvm::DebugLoc();
        builder.SetCurrentDebugLocation(currentDebugLocation);
    }
    else
    {
        currentDebugLocation = GetDebugLocation(span);
        builder.SetCurrentDebugLocation(currentDebugLocation);
    }
}

void* Emitter::GetArrayBeginAddress(void* arrayPtr)
{
    ArgVector elementIndeces;
    elementIndeces.push_back(builder.getInt64(0));
    elementIndeces.push_back(builder.getInt64(0));
    llvm::Value* beginPtr = builder.CreateGEP(static_cast<llvm::Value*>(arrayPtr), elementIndeces);
    return beginPtr;
}

void* Emitter::GetArrayEndAddress(void* arrayPtr, uint64_t size)
{
    ArgVector elementIndeces;
    elementIndeces.push_back(builder.getInt64(0));
    elementIndeces.push_back(builder.getInt64(size));
    llvm::Value* endPtr = builder.CreateGEP(static_cast<llvm::Value*>(arrayPtr), elementIndeces);
    return endPtr;

}

void* Emitter::CreateBasicBlock(const std::string& name)
{
    return llvm::BasicBlock::Create(context, name, Function());
}

void* Emitter::CreateIncludeBasicBlockInstruction(void* basicBlock)
{
    return nullptr;
}

void Emitter::PushParentBlock()
{
}

void Emitter::PopParentBlock()
{
}

void Emitter::SetHandlerBlock(void* tryBlock, void* catchBlock)
{
}

void Emitter::SetCleanupBlock(void* cleanupBlock)
{
}

int Emitter::GetBasicBlockId(void* basicBlock)
{
    return -1;
}

void Emitter::CreateBr(void* targetBasicBlock)
{
    builder.CreateBr(static_cast<llvm::BasicBlock*>(targetBasicBlock));
}

void Emitter::CreateCondBr(void* cond, void* trueBasicBlock, void* falseBasicBlock)
{
    builder.CreateCondBr(static_cast<llvm::Value*>(cond), static_cast<llvm::BasicBlock*>(trueBasicBlock), static_cast<llvm::BasicBlock*>(falseBasicBlock));
}

void* Emitter::CreateArrayIndexAddress(void* arrayPtr, void* index)
{
    ArgVector elementIndeces;
    elementIndeces.push_back(builder.getInt64(0));
    elementIndeces.push_back(static_cast<llvm::Value*>(index));
    return builder.CreateGEP(static_cast<llvm::Value*>(arrayPtr), elementIndeces);
}

void Emitter::CreateStore(void* value, void* ptr)
{
    builder.CreateStore(static_cast<llvm::Value*>(value), static_cast<llvm::Value*>(ptr));
}

void* Emitter::CreateLoad(void* ptr)
{
    return builder.CreateLoad(static_cast<llvm::Value*>(ptr));
}

void* Emitter::CreateAdd(void* left, void* right)
{
    return builder.CreateAdd(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateFAdd(void* left, void* right)
{
    return builder.CreateFAdd(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateSub(void* left, void* right)
{
    return builder.CreateSub(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateFSub(void* left, void* right)
{
    return builder.CreateFSub(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateMul(void* left, void* right)
{
    return builder.CreateMul(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateFMul(void* left, void* right)
{
    return builder.CreateFMul(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateUDiv(void* left, void* right)
{
    return builder.CreateUDiv(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateSDiv(void* left, void* right)
{
    return builder.CreateSDiv(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateFDiv(void* left, void* right)
{
    return builder.CreateFDiv(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateURem(void* left, void* right)
{
    return builder.CreateURem(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateSRem(void* left, void* right)
{
    return builder.CreateSRem(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateAnd(void* left, void* right)
{
    return builder.CreateAnd(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateOr(void* left, void* right)
{
    return builder.CreateOr(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateXor(void* left, void* right)
{
    return builder.CreateXor(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateShl(void* left, void* right)
{
    return builder.CreateShl(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateAShr(void* left, void* right)
{
    return builder.CreateAShr(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateLShr(void* left, void* right)
{
    return builder.CreateLShr(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateICmpEQ(void* left, void* right)
{
    return builder.CreateICmpEQ(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateFCmpOEQ(void* left, void* right)
{
    return builder.CreateFCmpOEQ(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

void* Emitter::CreateICmpULT(void* leftValue, void* rightValue)
{
    return builder.CreateICmpULT(static_cast<llvm::Value*>(leftValue), static_cast<llvm::Value*>(rightValue));
}

void* Emitter::CreateICmpSLT(void* leftValue, void* rightValue)
{
    return builder.CreateICmpSLT(static_cast<llvm::Value*>(leftValue), static_cast<llvm::Value*>(rightValue));
}

void* Emitter::CreateFCmpOLT(void* leftValue, void* rightValue)
{
    return builder.CreateFCmpOLT(static_cast<llvm::Value*>(leftValue), static_cast<llvm::Value*>(rightValue));
}

void* Emitter::CreateSExt(void* operand, void* destinationType)
{
    return builder.CreateSExt(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateZExt(void* operand, void* destinationType)
{
    return builder.CreateZExt(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateFPExt(void* operand, void* destinationType)
{
    return builder.CreateFPExt(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateTrunc(void* operand, void* destinationType)
{
    return builder.CreateTrunc(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateFPTrunc(void* operand, void* destinationType)
{
    return builder.CreateFPTrunc(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateBitCast(void* operand, void* destinationType)
{
    return builder.CreateBitCast(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateUIToFP(void* operand, void* destinationType)
{
    return builder.CreateUIToFP(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateSIToFP(void* operand, void* destinationType)
{
    return builder.CreateSIToFP(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateFPToUI(void* operand, void* destinationType)
{
    return builder.CreateFPToUI(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateFPToSI(void* operand, void* destinationType)
{
    return builder.CreateFPToSI(static_cast<llvm::Value*>(operand), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateIntToPtr(void* intValue, void* destinationType)
{
    return builder.CreateIntToPtr(static_cast<llvm::Value*>(intValue), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreatePtrToInt(void* ptrValue, void* destinationType)
{
    return builder.CreatePtrToInt(static_cast<llvm::Value*>(ptrValue), static_cast<llvm::Type*>(destinationType));
}

void* Emitter::CreateNot(void* value)
{
    return builder.CreateNot(static_cast<llvm::Value*>(value));
}

void* Emitter::CreateNeg(void* value)
{
    return builder.CreateNeg(static_cast<llvm::Value*>(value));
}

void* Emitter::CreateFNeg(void* value)
{
    return builder.CreateFNeg(static_cast<llvm::Value*>(value));
}

void* Emitter::CreateNop()
{
    return nullptr;
}

void* Emitter::CreateSave()
{
    return nullptr;
}

void* Emitter::CreateBeginTry()
{
    return nullptr;
}

void* Emitter::CreateEndTry(void* nextDest)
{
    return nullptr;
}

void* Emitter::CreateBeginCatch()
{
    return nullptr;
}

void* Emitter::CreateEndCatch(void* nextDest)
{
    return nullptr;
}

void* Emitter::GetOrInsertGlobal(const std::string& name, void* type)
{
    return module->getOrInsertGlobal(name, static_cast<llvm::Type*>(type));
}

void* Emitter::GetOrInsertAnyComdat(const std::string& name, void* global)
{
    llvm::GlobalVariable* globalVar = llvm::cast<llvm::GlobalVariable>(static_cast<llvm::Constant*>(global));
    llvm::Comdat* comdat = module->getOrInsertComdat(name);
    comdat->setSelectionKind(llvm::Comdat::SelectionKind::Any);
    globalVar->setComdat(comdat);
    return comdat;
}

void* Emitter::GetOrInsertAnyFunctionComdat(const std::string& name, void* function)
{
    llvm::Comdat* comdat = module->getOrInsertComdat(name);
    comdat->setSelectionKind(llvm::Comdat::SelectionKind::Any);
    static_cast<llvm::Function*>(function)->setComdat(comdat);
    return comdat;
}

void Emitter::SetInitializer(void* global, void* initializer)
{
    llvm::GlobalVariable* globalVar = llvm::cast<llvm::GlobalVariable>(static_cast<llvm::Constant*>(global));
    globalVar->setInitializer(static_cast<llvm::Constant*>(initializer));
}

void Emitter::SetPrivateLinkage(void* global)
{
    llvm::GlobalVariable* globalVar = llvm::cast<llvm::GlobalVariable>(static_cast<llvm::Constant*>(global));
    globalVar->setLinkage(llvm::GlobalValue::PrivateLinkage);
}

void* Emitter::GetOrInsertFunction(const std::string& name, void* type, bool nothrow)
{
#if (LLVM_VERSION_MAJOR >= 9)
    return module->getOrInsertFunction(name, static_cast<llvm::FunctionType*>(type)).getCallee();
#else
    return module->getOrInsertFunction(name, static_cast<llvm::FunctionType*>(type));
#endif
}

void* Emitter::CreateGlobalStringPtr(const std::string& name)
{
    llvm::Constant* strConstant = llvm::ConstantDataArray::getString(context, name);
    llvm::GlobalVariable* gv = new llvm::GlobalVariable(*module, strConstant->getType(), true, llvm::GlobalValue::PrivateLinkage, strConstant, "", nullptr, llvm::GlobalVariable::NotThreadLocal, 0);
    gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
    gv->setAlignment(1);
    llvm::Constant* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    llvm::Constant* indices[] = { zero, zero };
    return llvm::ConstantExpr::getInBoundsGetElementPtr(gv->getValueType(), gv, indices);
}

void* Emitter::CreateGlobalWStringPtr(const std::u16string& name)
{
    return nullptr;
}

void* Emitter::CreateGlobalUStringPtr(const std::u32string& name)
{
    return nullptr;
}

void* Emitter::CreateCall(void* callee, const std::vector<void*>& args)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    return builder.CreateCall(static_cast<llvm::Value*>(callee), arguments);
}

void* Emitter::CreateCallInst(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    std::vector<llvm::OperandBundleDef> bundleDefs;
    for (void* bundle : bundles)
    {
        std::vector<llvm::Value*> inputs;
        inputs.push_back(static_cast<llvm::Value*>(bundle));
        bundleDefs.push_back(llvm::OperandBundleDef("funclet", inputs));
    }
    llvm::CallInst* callInst = llvm::CallInst::Create(static_cast<llvm::Value*>(callee), arguments, bundleDefs, "", static_cast<llvm::BasicBlock*>(CurrentBasicBlock()));
    if (DIBuilder())
    {
        callInst->setDebugLoc(GetDebugLocation(span));
    }
    return callInst;
}

void* Emitter::CreateCallInstToBasicBlock(void* callee, const std::vector<void*>& args, const std::vector<void*>& bundles, void* basicBlock, const Span& span)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    std::vector<llvm::OperandBundleDef> bundleDefs;
    for (void* bundle : bundles)
    {
        std::vector<llvm::Value*> inputs;
        inputs.push_back(static_cast<llvm::Value*>(bundle));
        bundleDefs.push_back(llvm::OperandBundleDef("funclet", inputs));
    }
    llvm::CallInst* callInst = llvm::CallInst::Create(static_cast<llvm::Value*>(callee), arguments, bundleDefs, "", static_cast<llvm::BasicBlock*>(basicBlock));
    if (DIBuilder())
    {
        callInst->setDebugLoc(GetDebugLocation(span));
    }
    return callInst;
}

void* Emitter::CreateInvoke(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    return builder.CreateInvoke(static_cast<llvm::Value*>(callee), static_cast<llvm::BasicBlock*>(normalBlock), static_cast<llvm::BasicBlock*>(unwindBlock), arguments);
}

void* Emitter::CreateInvokeInst(void* callee, void* normalBlock, void* unwindBlock, const std::vector<void*>& args, const std::vector<void*>& bundles, const Span& span)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    std::vector<llvm::OperandBundleDef> bundleDefs;
    for (void* bundle : bundles)
    {
        std::vector<llvm::Value*> inputs;
        inputs.push_back(static_cast<llvm::Value*>(bundle));
        bundleDefs.push_back(llvm::OperandBundleDef("funclet", inputs));
    }
    llvm::InvokeInst* invokeInst = llvm::InvokeInst::Create(static_cast<llvm::Value*>(callee), static_cast<llvm::BasicBlock*>(normalBlock), static_cast<llvm::BasicBlock*>(unwindBlock),
        arguments, bundleDefs, "", static_cast<llvm::BasicBlock*>(CurrentBasicBlock()));
    if (DIBuilder())
    {
        invokeInst->setDebugLoc(GetDebugLocation(span));
    }
    return invokeInst;
}

void* Emitter::GetObjectFromClassDelegate(void* classDelegatePtr)
{
    ArgVector objectIndeces;
    objectIndeces.push_back(builder.getInt32(0));
    objectIndeces.push_back(builder.getInt32(0));
    llvm::Value* objectPtr = builder.CreateGEP(static_cast<llvm::Value*>(classDelegatePtr), objectIndeces);
    return objectPtr;
}

void* Emitter::GetDelegateFromClassDelegate(void* classDelegatePtr)
{
    ArgVector delegateIndeces;
    delegateIndeces.push_back(builder.getInt32(0));
    delegateIndeces.push_back(builder.getInt32(1));
    llvm::Value* delegatePtr = builder.CreateGEP(static_cast<llvm::Value*>(classDelegatePtr), delegateIndeces);
    return delegatePtr;
}

void* Emitter::GetObjectFromInterface(void* interfaceTypePtr)
{
    ArgVector objectIndeces;
    objectIndeces.push_back(builder.getInt32(0));
    objectIndeces.push_back(builder.getInt32(0));
    llvm::Value* objectPtrPtr = builder.CreateGEP(static_cast<llvm::Value*>(interfaceTypePtr), objectIndeces);
    llvm::Value* objectPtr = builder.CreateLoad(objectPtrPtr);
    return objectPtr;
}

void* Emitter::GetObjectPtrFromInterface(void* interfaceTypePtr)
{
    ArgVector objectIndeces;
    objectIndeces.push_back(builder.getInt32(0));
    objectIndeces.push_back(builder.getInt32(0));
    llvm::Value* objectPtrPtr = builder.CreateGEP(static_cast<llvm::Value*>(interfaceTypePtr), objectIndeces);
    return objectPtrPtr;
}

void* Emitter::GetImtPtrFromInterface(void* interfaceTypePtr)
{
    ArgVector interfaceIndeces;
    interfaceIndeces.push_back(builder.getInt32(0));
    interfaceIndeces.push_back(builder.getInt32(1));
    llvm::Value* interfacePtrPtr = builder.CreateGEP(static_cast<llvm::Value*>(interfaceTypePtr), interfaceIndeces);
    llvm::Value* interfacePtr = builder.CreateLoad(interfacePtrPtr);
    llvm::Value* imtPtr = builder.CreateBitCast(interfacePtr, llvm::PointerType::get(builder.getInt8PtrTy(), 0));
    return imtPtr;
}

void* Emitter::GetInterfaceMethod(void* imtPtr, int32_t methodIndex, void* interfaceMethodType)
{
    ArgVector methodIndeces;
    methodIndeces.push_back(builder.getInt32(methodIndex));
    llvm::Value* methodPtrPtr = builder.CreateGEP(static_cast<llvm::Value*>(imtPtr), methodIndeces);
    llvm::Value* methodPtr = builder.CreateLoad(methodPtrPtr);
    llvm::Value* callee = builder.CreateBitCast(methodPtr, llvm::PointerType::get(static_cast<llvm::Type*>(interfaceMethodType), 0));
    return callee;
}

void* Emitter::GetVmtPtr(void* thisPtr, int32_t vmtPtrIndex, void* vmtPtrType)
{
    ArgVector vmtPtrIndeces;
    vmtPtrIndeces.push_back(builder.getInt32(0));
    vmtPtrIndeces.push_back(builder.getInt32(vmtPtrIndex));
    llvm::Value* vmtPtrPtr = builder.CreateGEP(static_cast<llvm::Value*>(thisPtr), vmtPtrIndeces);
    void* vmtPtr = builder.CreateBitCast(builder.CreateLoad(vmtPtrPtr), static_cast<llvm::Type*>(vmtPtrType));
    return vmtPtr;
}

void* Emitter::GetMethodPtr(void* vmtPtr, int32_t vmtIndex)
{
    ArgVector funPtrIndeces;
    funPtrIndeces.push_back(builder.getInt32(0));
    funPtrIndeces.push_back(builder.getInt32(vmtIndex));
    llvm::Value* funPtrPtr = builder.CreateGEP(static_cast<llvm::Value*>(vmtPtr), funPtrIndeces);
    void* funAsVoidPtr = builder.CreateLoad(funPtrPtr);
    return funAsVoidPtr;
}

void* Emitter::GetImtArray(void* vmtObjectPtr, int32_t imtsVmtIndexOffset)
{
    ArgVector imtsArrayIndeces;
    imtsArrayIndeces.push_back(builder.getInt32(0));
    imtsArrayIndeces.push_back(builder.getInt32(imtsVmtIndexOffset)); 
    llvm::Value* imtsArrayPtrPtr = builder.CreateGEP(static_cast<llvm::Value*>(vmtObjectPtr), imtsArrayIndeces);
    llvm::Value* imtsArrayPtr = builder.CreateBitCast(imtsArrayPtrPtr, llvm::PointerType::get(llvm::PointerType::get(builder.getInt8PtrTy(), 0), 0));
    llvm::Value* imtArray = builder.CreateLoad(imtsArrayPtr);
    return imtArray;
}

void* Emitter::GetImt(void* imtArray, int32_t interfaceIndex)
{
    ArgVector imtArrayIndeces;
    imtArrayIndeces.push_back(builder.getInt32(interfaceIndex));
    llvm::Value* imtArrayPtr = builder.CreateGEP(static_cast<llvm::Value*>(imtArray), imtArrayIndeces);
    llvm::Value* imt = builder.CreateLoad(imtArrayPtr);
    return imt;
}

void* Emitter::GetMemberVariablePtr(void* classPtr, int32_t memberVariableLayoutIndex)
{
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(memberVariableLayoutIndex));
    llvm::Value* memberVariablePtr = builder.CreateGEP(static_cast<llvm::Value*>(classPtr), indeces);
    return memberVariablePtr;
}

void* Emitter::SizeOf(void* ptrType)
{
    void* nullPtr = CreateDefaultIrValueForPtrType(static_cast<llvm::Type*>(ptrType));
    llvm::Value* gep = builder.CreateGEP(static_cast<llvm::Value*>(nullPtr), builder.getInt64(1));
    llvm::Value* size = builder.CreatePtrToInt(gep, builder.getInt64Ty());
    return size;
}

/*
void* Emitter::GetClassIdPtr(void* vmtPtr)
{
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(0));
    llvm::Value* classIdPtr = builder.CreateGEP(static_cast<llvm::Value*>(vmtPtr), indeces);
    return classIdPtr;
}
*/

void* Emitter::GetClassIdPtr(void* vmtPtr, int32_t classIdVmtIndexOffset)
{
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(classIdVmtIndexOffset));
    llvm::Value* classIdPtr = builder.CreateGEP(static_cast<llvm::Value*>(vmtPtr), indeces);
    return classIdPtr;
}

void* Emitter::GetClassName(void* vmtPtr, int32_t classNameVmtIndexOffset)
{
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(classNameVmtIndexOffset)); 
    llvm::Value* classNamePtr = builder.CreateGEP(static_cast<llvm::Value*>(vmtPtr), indeces);
    llvm::Value* className = builder.CreateLoad(classNamePtr);
    return className;
}

void* Emitter::ComputeAddress(void* ptr, void* index)
{
    return builder.CreateGEP(static_cast<llvm::Value*>(ptr), static_cast<llvm::Value*>(index));
}

void* Emitter::CreatePtrDiff(void* left, void* right)
{
    return builder.CreatePtrDiff(static_cast<llvm::Value*>(left), static_cast<llvm::Value*>(right));
}

uint32_t Emitter::GetPrivateFlag()
{
    return static_cast<uint32_t>(llvm::DINode::DIFlags::FlagPrivate);
}

uint32_t Emitter::GetProtectedFlag()
{
    return static_cast<uint32_t>(llvm::DINode::DIFlags::FlagProtected);
}

uint32_t Emitter::GetPublicFlag()
{
    return static_cast<uint32_t>(llvm::DINode::DIFlags::FlagPublic);
}

uint32_t Emitter::GetNoFlags()
{
    return static_cast<uint32_t>(llvm::DINode::DIFlags::FlagZero);
}

void* Emitter::CreateModule(const std::string& moduleName)
{
    return new llvm::Module(moduleName, context);
}

void Emitter::DestroyModule(void* module)
{
    delete static_cast<llvm::Module*>(module);
}

void Emitter::SetDIFile(void* diFile_)
{
    diFile = static_cast<llvm::DIFile*>(diFile_);
}

void* Emitter::GetDebugInfoForFile(int32_t fileIndex)
{
    if (fileIndex == -1)
    {
        return diFile;
    }
    auto it = fileMap.find(fileIndex);
    if (it != fileMap.cend())
    {
        return it->second;
    }
    std::string sourceFilePath = GetSourceFilePath(fileIndex);
    if (sourceFilePath.empty())
    {
        return diFile;
    }
    llvm::DIFile* file = diBuilder->createFile(Path::GetFileName(sourceFilePath), Path::GetDirectoryName(sourceFilePath));
    fileMap[fileIndex] = file;
    return file;
}

void* Emitter::GetDITypeByTypeId(const boost::uuids::uuid& typeId) const
{
    auto it = diTypeTypeIdMap.find(typeId);
    if (it != diTypeTypeIdMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetDITypeByTypeId(const boost::uuids::uuid& typeId, void* diType, const std::string& typeName)
{
    diTypeTypeIdMap[typeId] = static_cast<llvm::DIType*>(diType);
    diTypeNameMap[static_cast<llvm::DIType*>(diType)] = typeName;
}

void* Emitter::GetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId) 
{
    auto it = diMemberTypeMap.find(memberVariableId);
    if (it != diMemberTypeMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void Emitter::SetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId, void* diType)
{
    diMemberTypeMap[memberVariableId] = static_cast<llvm::DIDerivedType*>(diType);
}

void* Emitter::CreateDIMemberType(void* scope, const std::string& name, const Span& span, uint64_t sizeInBits, uint64_t alignInBits, uint64_t offsetInBits, void* diType)
{
    llvm::DINode::DIFlags flags = llvm::DINode::DIFlags::FlagZero;
    return diBuilder->createMemberType(static_cast<llvm::DIType*>(scope), name, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)), span.line, sizeInBits, alignInBits, offsetInBits, flags,
        static_cast<llvm::DIType*>(diType));
}

void* Emitter::CreateConstDIType(void* diType)
{
    return diBuilder->createQualifiedType(llvm::dwarf::DW_TAG_const_type, static_cast<llvm::DIType*>(diType));
}

void* Emitter::CreateLValueRefDIType(void* diType)
{
    return diBuilder->createReferenceType(llvm::dwarf::DW_TAG_reference_type, static_cast<llvm::DIType*>(diType));
}

void* Emitter::CreateRValueRefDIType(void* diType)
{
    return diBuilder->createReferenceType(llvm::dwarf::DW_TAG_rvalue_reference_type, static_cast<llvm::DIType*>(diType));
}

void* Emitter::CreatePointerDIType(void* diType)
{
    return diBuilder->createPointerType(static_cast<llvm::DIType*>(diType), 64);
}

void* Emitter::CreateUnspecifiedDIType(const std::string& name)
{
    return diBuilder->createUnspecifiedType(name);
}

void Emitter::MapClassPtr(const boost::uuids::uuid& typeId, void* classPtr, const std::string& className)
{
    if (classPtrMap.find(typeId) == classPtrMap.cend())
    {
        classPtrMap[typeId] = classPtr;
        classNameMap[classPtr] = className;
    }
}

uint64_t Emitter::GetSizeInBits(void* irType)
{
    return dataLayout->getTypeSizeInBits(static_cast<llvm::Type*>(irType));
}

uint64_t Emitter::GetAlignmentInBits(void* irType)
{
    return 8 * dataLayout->getABITypeAlignment(static_cast<llvm::Type*>(irType));
}

void Emitter::ReplaceForwardDeclarations()
{
    std::unordered_map<llvm::DIType*, boost::uuids::uuid> currentFwdDeclarationMap;
    std::swap(currentFwdDeclarationMap, fwdDeclarationMap);
    while (!currentFwdDeclarationMap.empty())
    {
        for (const auto& p : currentFwdDeclarationMap)
        {
            llvm::DIType* fwdDeclaration = p.first;
            const boost::uuids::uuid& typeId = p.second;
            void* diType = GetDITypeByTypeId(typeId);
            if (!diType)
            {
                auto it = classPtrMap.find(typeId);
                if (it != classPtrMap.cend())
                {
                    void* classPtr = it->second;
                    //std::string className = classNameMap[classPtr];
                    //std::cout << "> create di type for class" << className << std::endl;
                    diType = CreateClassDIType(classPtr);
                    //std::cout << "< create di type for class" << className << std::endl;
                }
                else
                {
                    throw std::runtime_error("Emitter::ReplaceForwardDeclarations(): class ptr not mapped");
                }
            }
            //std::string typeName = diTypeNameMap[static_cast<llvm::DIType*>(diType)];
            //std::cout << "> replacing " << typeName << std::endl;
            fwdDeclaration->replaceAllUsesWith(static_cast<llvm::DIType*>(diType));
            //std::cout << "< replacing " << typeName << std::endl;
        }
        currentFwdDeclarationMap.clear();
        std::swap(currentFwdDeclarationMap, fwdDeclarationMap);
    }
}

void* Emitter::GetIrObject(void* symbol) const
{
    auto it = irObjectMap.find(symbol);
    if (it != irObjectMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("emitter: IR object not found");
    }
}

void Emitter::SetIrObject(void* symbol, void* irObject)
{
    irObjectMap[symbol] = static_cast<llvm::Value*>(irObject);
}

void* Emitter::GetIrTypeByTypeId(const boost::uuids::uuid& typeId)
{
    auto it = irTypeTypeIdMap.find(typeId);
    if (it != irTypeTypeIdMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetIrTypeByTypeId(const boost::uuids::uuid& typeId, void* irType)
{
    irTypeTypeIdMap[typeId] = static_cast<llvm::Type*>(irType);
}

void* Emitter::GetIrTypeForPtrType(void* baseIrType)
{
    return llvm::PointerType::get(static_cast<llvm::Type*>(baseIrType), 0);
}

std::string Emitter::GetIrTypeName(void* irType)
{
    return std::string();
}

std::string Emitter::MakeVmtVariableName(const std::string& vmtObjectName)
{
    return std::string();
}

void* Emitter::CreateDefaultIrValueForArrayType(void* arrayIrType, const std::vector<void*>& arrayOfDefaults)
{
    std::vector<llvm::Constant*> arrayOfConstants;
    for (void* constant : arrayOfDefaults)
    {
        arrayOfConstants.push_back(static_cast<llvm::Constant*>(constant));
    }
    llvm::Type* arrayType = static_cast<llvm::Type*>(arrayIrType);
    return llvm::ConstantArray::get(llvm::cast<llvm::ArrayType>(arrayType), arrayOfConstants);
}

void* Emitter::CreateDefaultIrValueForBool()
{
    return builder.getInt1(false);
}

void* Emitter::CreateDefaultIrValueForSByte()
{
    return builder.getInt8(0);
}

void* Emitter::CreateDefaultIrValueForByte()
{
    return builder.getInt8(0);
}

void* Emitter::CreateDefaultIrValueForShort()
{
    return builder.getInt16(0);
}

void* Emitter::CreateDefaultIrValueForUShort()
{
    return builder.getInt16(0);
}

void* Emitter::CreateDefaultIrValueForInt()
{
    return builder.getInt32(0);
}

void* Emitter::CreateDefaultIrValueForUInt()
{
    return builder.getInt32(0);
}

void* Emitter::CreateDefaultIrValueForLong()
{
    return builder.getInt64(0);
}

void* Emitter::CreateDefaultIrValueForULong()
{
    return builder.getInt64(0);
}

void* Emitter::CreateDefaultIrValueForFloat()
{
    return llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 0.0);
}

void* Emitter::CreateDefaultIrValueForDouble()
{
    return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0);
}

void* Emitter::CreateDefaultIrValueForChar()
{
    return builder.getInt8(0);
}

void* Emitter::CreateDefaultIrValueForWChar()
{
    return builder.getInt16(0);
}

void* Emitter::CreateDefaultIrValueForUChar()
{
    return builder.getInt32(0);
}

void* Emitter::CreateDefaultIrValueForStruct(void* irType, const std::vector<void*>& defaultMembers)
{
    std::vector<llvm::Constant*> arrayOfDefaults;
    for (void* constant : defaultMembers)
    {
        arrayOfDefaults.push_back(static_cast<llvm::Constant*>(constant));
    }
    return llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(irType)), arrayOfDefaults);
}

void* Emitter::CreateDefaultIrValueForDelegateType(void* irType)
{
    return llvm::Constant::getNullValue(static_cast<llvm::Type*>(irType));
}

void* Emitter::CreateDefaultIrValueForVoidPtrType()
{
    return llvm::Constant::getNullValue(builder.getInt8PtrTy());
}

void* Emitter::CreateDefaultIrValueForDerivedType(void* irType)
{
    return llvm::Constant::getNullValue(static_cast<llvm::Type*>(irType));
}

void* Emitter::CreateDefaultIrValueForPtrType(void* irType)
{
    return llvm::Constant::getNullValue(static_cast<llvm::Type*>(irType));
}

void* Emitter::CreateIrValueForBool(bool value)
{
    return builder.getInt1(value);
}

void* Emitter::CreateIrValueForSByte(int8_t value)
{
    return builder.getInt8(static_cast<uint8_t>(value));
}

void* Emitter::CreateIrValueForByte(uint8_t value)
{
    return builder.getInt8(value);
}

void* Emitter::CreateIrValueForShort(int16_t value)
{
    return builder.getInt16(static_cast<uint16_t>(value));
}

void* Emitter::CreateIrValueForUShort(uint16_t value)
{
    return builder.getInt16(value);
}

void* Emitter::CreateIrValueForInt(int32_t value)
{
    if (substituteLineNumber)
    {
        return builder.getInt32(static_cast<uint32_t>(currentLineNumber));
    }
    else
    {
        return builder.getInt32(static_cast<uint32_t>(value));
    }
}

void* Emitter::CreateIrValueForUInt(uint32_t value)
{
    return builder.getInt32(value);
}

void* Emitter::CreateIrValueForLong(int64_t value)
{
    return builder.getInt64(static_cast<uint64_t>(value));
}

void* Emitter::CreateIrValueForULong(uint64_t value)
{
    return builder.getInt64(value);
}

void* Emitter::CreateIrValueForFloat(float value)
{
    return llvm::ConstantFP::get(builder.getFloatTy(), value);
}

void* Emitter::CreateIrValueForDouble(double value)
{
    return llvm::ConstantFP::get(builder.getDoubleTy(), value);
}

void* Emitter::CreateIrValueForChar(uint8_t value)
{
    return builder.getInt8(value);
}

void* Emitter::CreateIrValueForWChar(uint16_t value)
{
    return builder.getInt16(value);
}

void* Emitter::CreateIrValueForUChar(uint32_t value)
{
    return builder.getInt32(value);
}

void* Emitter::CreateIrValueForWString(void* wstringConstant)
{
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(0));
    return builder.CreateGEP(static_cast<llvm::Value*>(wstringConstant), indeces);
}

void* Emitter::CreateIrValueForUString(void* ustringConstant)
{
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(0));
    return builder.CreateGEP(static_cast<llvm::Value*>(ustringConstant), indeces);
}

void* Emitter::CreateIrValueForConstantArray(void* arrayIrType, const std::vector<void*>& elements, const std::string& prefix)
{
    std::vector<llvm::Constant*> elementConstants;
    for (void* elementConstant : elements)
    {
        elementConstants.push_back(static_cast<llvm::Constant*>(elementConstant));
    }
    return llvm::ConstantArray::get(llvm::cast<llvm::ArrayType>(static_cast<llvm::Type*>(arrayIrType)), elementConstants);
}

void* Emitter::CreateIrValueForConstantStruct(void* structIrType, const std::vector<void*>& elementConstants)
{
    std::vector<llvm::Constant*> memberConstants;
    for (void* elementConstant : elementConstants)
    {
        memberConstants.push_back(static_cast<llvm::Constant*>(elementConstant));
    }
    return llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(structIrType)), memberConstants);
}

void* Emitter::CreateIrValueForUuid(void* uuidConstant)
{
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(0));
    return builder.CreateBitCast(builder.CreateGEP(static_cast<llvm::Value*>(uuidConstant), indeces), builder.getInt8PtrTy());
}

void* Emitter::GetConversionValue(void* type, void* from)
{
    return nullptr;
}

void* Emitter::CreateDITypeForBool()
{
    return diBuilder->createBasicType("bool", 8, llvm::dwarf::DW_ATE_boolean);
}

void* Emitter::CreateDITypeForSByte()
{
    return diBuilder->createBasicType("sbyte", 8, llvm::dwarf::DW_ATE_signed);
}

void* Emitter::CreateDITypeForByte()
{
    return diBuilder->createBasicType("byte", 8, llvm::dwarf::DW_ATE_unsigned);
}

void* Emitter::CreateDITypeForShort()
{
    return diBuilder->createBasicType("short", 16, llvm::dwarf::DW_ATE_signed);
}

void* Emitter::CreateDITypeForUShort()
{
    return diBuilder->createBasicType("ushort", 16, llvm::dwarf::DW_ATE_unsigned);
}

void* Emitter::CreateDITypeForInt()
{
    return diBuilder->createBasicType("int", 32, llvm::dwarf::DW_ATE_signed);
}

void* Emitter::CreateDITypeForUInt()
{
    return diBuilder->createBasicType("uint", 32, llvm::dwarf::DW_ATE_unsigned);
}

void* Emitter::CreateDITypeForLong()
{
    return diBuilder->createBasicType("long", 64, llvm::dwarf::DW_ATE_signed);
}

void* Emitter::CreateDITypeForULong()
{
    return diBuilder->createBasicType("ulong", 64, llvm::dwarf::DW_ATE_unsigned);
}

void* Emitter::CreateDITypeForFloat()
{
    return diBuilder->createBasicType("float", 32, llvm::dwarf::DW_ATE_float);
}

void* Emitter::CreateDITypeForDouble()
{
    return diBuilder->createBasicType("double", 64, llvm::dwarf::DW_ATE_float);
}

void* Emitter::CreateDITypeForChar()
{
    return diBuilder->createBasicType("char", 8, llvm::dwarf::DW_ATE_unsigned_char);
}

void* Emitter::CreateDITypeForWChar()
{
    return diBuilder->createBasicType("wchar", 16, llvm::dwarf::DW_ATE_unsigned_char);
}

void* Emitter::CreateDITypeForUChar()
{
    return diBuilder->createBasicType("uchar", 32, llvm::dwarf::DW_ATE_unsigned_char);
}

void* Emitter::CreateDITypeForVoid()
{
    return diBuilder->createUnspecifiedType("void");
}

void* Emitter::CreateDITypeForArray(void* elementDIType, const std::vector<void*>& elements)
{
    // todo...
    std::vector<llvm::Metadata*> metadataElements;
    return diBuilder->createArrayType(elements.size(), 8, static_cast<llvm::DIType*>(elementDIType), diBuilder->getOrCreateArray(metadataElements));

}

#if (LLVM_VERSION_MAJOR >= 10)

void* Emitter::CreateIrDIForwardDeclaration(void* irType, const std::string& name, const std::string& mangledName, const Span& span)
{
    uint64_t sizeInBits = dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(irType)))->getSizeInBits();
    uint64_t alignInBits = 8 * dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(irType)))->getAlignment().value();
    uint64_t offsetInBits = 0; // todo?
    return diBuilder->createReplaceableCompositeType(llvm::dwarf::DW_TAG_class_type, name, nullptr, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)), span.line,
        0, sizeInBits, alignInBits, llvm::DINode::DIFlags::FlagZero, mangledName);
}

#else

void* Emitter::CreateIrDIForwardDeclaration(void* irType, const std::string& name, const std::string& mangledName, const Span& span)
{
    uint64_t sizeInBits = dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(irType)))->getSizeInBits();
    uint32_t alignInBits = 8 * dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(irType)))->getAlignment();
    uint64_t offsetInBits = 0; // todo?
    return diBuilder->createReplaceableCompositeType(llvm::dwarf::DW_TAG_class_type, name, nullptr, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)), span.line,
        0, sizeInBits, alignInBits, llvm::DINode::DIFlags::FlagZero, mangledName);
}

#endif

uint64_t Emitter::GetOffsetInBits(void* classIrType, int layoutIndex)
{
    const llvm::StructLayout* structLayout = dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(classIrType)));
    uint64_t offsetInBits = structLayout->getElementOffsetInBits(layoutIndex);
    return offsetInBits;
}

#if (LLVM_VERSION_MAJOR >= 10)

void* Emitter::CreateDITypeForClassType(void* irType, const std::vector<void*>& memberVariableElements, const Span& classSpan, const std::string& name, void* vtableHolderClass,
    const std::string& mangledName, void* baseClassDIType)
{
    std::vector<llvm::Metadata*> elements;
    const llvm::StructLayout* structLayout = dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(irType)));
    for (void* element : memberVariableElements)
    {
        elements.push_back(static_cast<llvm::Metadata*>(element));
    }
    llvm::MDNode* templateParams = nullptr;
    uint64_t sizeInBits = structLayout->getSizeInBits();
    uint64_t alignInBits = 8 * structLayout->getAlignment().value();
    uint64_t offsetInBits = 0; // todo?
    llvm::DINode::DIFlags flags = llvm::DINode::DIFlags::FlagZero;
    return diBuilder->createClassType(static_cast<llvm::DIScope*>(CurrentScope()), name, static_cast<llvm::DIFile*>(GetDebugInfoForFile(classSpan.fileIndex)), classSpan.line, sizeInBits, alignInBits, offsetInBits,
        flags, static_cast<llvm::DIType*>(baseClassDIType), diBuilder->getOrCreateArray(elements), static_cast<llvm::DIType*>(vtableHolderClass), templateParams, mangledName);
}


#else

void* Emitter::CreateDITypeForClassType(void* irType, const std::vector<void*>& memberVariableElements, const Span& classSpan, const std::string& name, void* vtableHolderClass,
    const std::string& mangledName, void* baseClassDIType)
{
    std::vector<llvm::Metadata*> elements;
    const llvm::StructLayout* structLayout = dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::Type*>(irType)));
    for (void* element : memberVariableElements)
    {
        elements.push_back(static_cast<llvm::Metadata*>(element));
    }
    llvm::MDNode* templateParams = nullptr;
    uint64_t sizeInBits = structLayout->getSizeInBits();
    uint32_t alignInBits = 8 * structLayout->getAlignment();
    uint64_t offsetInBits = 0; // todo?
    llvm::DINode::DIFlags flags = llvm::DINode::DIFlags::FlagZero;
    return diBuilder->createClassType(static_cast<llvm::DIScope*>(CurrentScope()), name, static_cast<llvm::DIFile*>(GetDebugInfoForFile(classSpan.fileIndex)), classSpan.line, sizeInBits, alignInBits, offsetInBits,
        flags, static_cast<llvm::DIType*>(baseClassDIType), diBuilder->getOrCreateArray(elements), static_cast<llvm::DIType*>(vtableHolderClass), templateParams, mangledName);
}

#endif

void* Emitter::GetFunctionIrType(void* symbol) const
{
    auto it = functionIrTypeMap.find(symbol);
    if (it != functionIrTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetFunctionIrType(void* symbol, void* irType)
{
    functionIrTypeMap[symbol] = static_cast<llvm::FunctionType*>(irType);
}

bool Emitter::IsVmtObjectCreated(void* symbol) const
{
    return vmtObjectCreatedSet.find(symbol) != vmtObjectCreatedSet.cend();
}

void Emitter::SetVmtObjectCreated(void* symbol)
{
    vmtObjectCreatedSet.insert(symbol);
}

bool Emitter::IsStaticObjectCreated(void* symbol) const
{
    return staticObjectCreatedSet.find(symbol) != staticObjectCreatedSet.cend();
}

void Emitter::SetStaticObjectCreated(void* symbol)
{
    staticObjectCreatedSet.insert(symbol);
}

void* Emitter::GetStaticObjectType(void* symbol) const
{
    auto it = staticTypeMap.find(symbol);
    if (it != staticTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetStaticObjectType(void* symbol, void* type)
{
    staticTypeMap[symbol] = static_cast<llvm::StructType*>(type);
}

void* Emitter::GetVmtObjectType(void* symbol) const
{
    auto it = vmtObjectTypeMap.find(symbol);
    if (it != vmtObjectTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetVmtObjectType(void* symbol, void* vmtObjectType)
{
    vmtObjectTypeMap[symbol] = static_cast<llvm::ArrayType*>(vmtObjectType);
}

std::string Emitter::GetStaticObjectName(void* symbol) const
{
    auto it = staticObjectNameMap.find(symbol);
    if (it != staticObjectNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Emitter::SetStaticObjectName(void* symbol, const std::string& staticObjectName)
{
    staticObjectNameMap[symbol] = staticObjectName;
}

std::string Emitter::GetVmtObjectName(void* symbol) const
{
    auto it = vmtObjectNameMap.find(symbol);
    if (it != vmtObjectNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Emitter::SetVmtObjectName(void* symbol, const std::string& vmtObjectName)
{
    vmtObjectNameMap[symbol] = vmtObjectName;
}

std::string Emitter::GetImtArrayObjectName(void* symbol) const
{
    auto it = imtArrayObjectNameMap.find(symbol);
    if (it != imtArrayObjectNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Emitter::SetImtArrayObjectName(void* symbol, const std::string& imtArrayObjectName)
{
    imtArrayObjectNameMap[symbol] = imtArrayObjectName;
}

uint64_t Emitter::GetClassTypeSizeInBits(void* classIrType)
{
    uint64_t sizeInBits = dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::StructType*>(classIrType)))->getSizeInBits();
    return sizeInBits;
}

#if (LLVM_VERSION_MAJOR >= 10)

uint64_t Emitter::GetClassTypeAlignmentInBits(void* classIrType)
{
    uint64_t alignInBits = 8 * dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::StructType*>(classIrType)))->getAlignment().value();
    return alignInBits;
}

#else

uint64_t Emitter::GetClassTypeAlignmentInBits(void* classIrType)
{
    uint32_t alignInBits = 8 * dataLayout->getStructLayout(llvm::cast<llvm::StructType>(static_cast<llvm::StructType*>(classIrType)))->getAlignment();
    return alignInBits;
}

#endif

void Emitter::AddInlineFunctionAttribute(void* function)
{
    llvm::Function* fun = static_cast<llvm::Function*>(function);
    fun->addFnAttr(llvm::Attribute::InlineHint);
}

void Emitter::SetFunctionLinkage(void* function, bool setInline)
{
    llvm::Function* fun = static_cast<llvm::Function*>(function);
    if (setInline)
    {
        fun->setLinkage(llvm::GlobalValue::LinkageTypes::WeakODRLinkage);
    }
}

void Emitter::SetFunctionLinkageToLinkOnceODRLinkage(void* function)
{
    llvm::Function* fun = static_cast<llvm::Function*>(function);
    fun->setLinkage(llvm::GlobalValue::LinkageTypes::LinkOnceODRLinkage);
}

void Emitter::SetFunctionCallConventionToStdCall(void* function)
{
    llvm::Function* fun = static_cast<llvm::Function*>(function);
    fun->setCallingConv(llvm::CallingConv::X86_StdCall);
    //fun->setCallingConv(llvm::CallingConv::X86_ThisCall);
}

void* Emitter::CreateSubroutineType(const std::vector<void*>& elementTypes)
{
    std::vector<llvm::Metadata*> elements;
    for (void* elementType : elementTypes)
    {
        elements.push_back(static_cast<llvm::Metadata*>(elementType));
    }
    llvm::DISubroutineType* subroutineType = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray(elements));
    return subroutineType;
}

unsigned Emitter::GetPureVirtualVirtuality()
{
    return static_cast<unsigned>(llvm::dwarf::DW_VIRTUALITY_pure_virtual);
}

unsigned Emitter::GetVirtualVirtuality()
{
    return static_cast<unsigned>(llvm::dwarf::DW_VIRTUALITY_virtual);
}

unsigned Emitter::GetFunctionFlags(bool isStatic, unsigned accessFlags, bool isExplicit)
{
    unsigned flags = llvm::DINode::DIFlags::FlagZero;
    if (isStatic)
    {
        flags |= llvm::DINode::FlagStaticMember;
    }
    flags |= accessFlags;
    if (isExplicit)
    {
        flags |= llvm::DINode::FlagExplicit;
    }
    return flags;
}

/*
void* Emitter::CreateDIMethod(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned virtuality, unsigned vtableIndex, void* vtableHolder,
    unsigned flags)
{
    void* subprogram = diBuilder->createMethod(static_cast<llvm::DIScope*>(CurrentScope()), name, mangledName, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)),
        span.line, static_cast<llvm::DISubroutineType*>(subroutineType), false, true, virtuality, vtableIndex, 0, static_cast<llvm::DIType*>(vtableHolder),
        static_cast<llvm::DINode::DIFlags>(flags));
    return subprogram;
}
*/

void* Emitter::CreateDIMethod(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned virtuality, unsigned vtableIndex, void* vtableHolder,
    unsigned flags)
{
    void* subprogram = diBuilder->createMethod(static_cast<llvm::DIScope*>(CurrentScope()), name, mangledName, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)),
        span.line, static_cast<llvm::DISubroutineType*>(subroutineType), vtableIndex, 0, static_cast<llvm::DIType*>(vtableHolder),
        static_cast<llvm::DINode::DIFlags>(flags), llvm::DISubprogram::SPFlagDefinition);
    return subprogram;
}

/*
void* Emitter::CreateDIFunction(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned flags)
{
    void* subprogram = diBuilder->createFunction(static_cast<llvm::DIScope*>(CurrentScope()), name, mangledName, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)),
        span.line, static_cast<llvm::DISubroutineType*>(subroutineType), false, true, span.line, static_cast<llvm::DINode::DIFlags>(flags));
    return subprogram;
}
*/

void* Emitter::CreateDIFunction(const std::string& name, const std::string& mangledName, const Span& span, void* subroutineType, unsigned flags)
{
    void* subprogram = diBuilder->createFunction(static_cast<llvm::DIScope*>(CurrentScope()), name, mangledName, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)),
        span.line, static_cast<llvm::DISubroutineType*>(subroutineType), span.line, static_cast<llvm::DINode::DIFlags>(flags),
        llvm::DISubprogram::SPFlagDefinition);
    return subprogram;
}

void Emitter::SetDISubprogram(void* function, void* subprogram)
{
    static_cast<llvm::Function*>(function)->setSubprogram(static_cast<llvm::DISubprogram*>(subprogram));
}

void* Emitter::CreateAlloca(void* irType)
{
    return builder.CreateAlloca(static_cast<llvm::Type*>(irType));
}

void* Emitter::NewAllocaInst(void* irType)
{
    return new llvm::AllocaInst(static_cast<llvm::Type*>(irType), 0);
}

void* Emitter::CreateDIParameterVariable(const std::string& name, int index, const Span& span, void* irType, void* allocaInst)
{
    llvm::DILocalVariable* paramVar = diBuilder->createParameterVariable(static_cast<llvm::DIScope*>(CurrentScope()), name, index, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)),
        span.line, static_cast<llvm::DIType*>(irType));
    llvm::SmallVector<int64_t, 13> expr; // todo
    diBuilder->insertDeclare(static_cast<llvm::Value*>(allocaInst), paramVar, diBuilder->createExpression(expr), GetDebugLocation(span), builder.GetInsertBlock());
    return paramVar;
}

void* Emitter::CreateDIAutoVariable(const std::string& name, const Span& span, void* irType, void* allocaInst)
{
    llvm::DILocalVariable* localVar = diBuilder->createAutoVariable(static_cast<llvm::DIScope*>(CurrentScope()), name, static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)),
        span.line, static_cast<llvm::DIType*>(irType));
    llvm::SmallVector<int64_t, 13> expr; // todo
    diBuilder->insertDeclare(static_cast<llvm::Value*>(allocaInst), localVar, diBuilder->createExpression(expr), GetDebugLocation(span), builder.GetInsertBlock());
    return localVar;
}

void* Emitter::GetFunctionArgument(void* function, int argumentIndex)
{
    auto it = static_cast<llvm::Function*>(function)->args().begin() + argumentIndex;
    return &*it;
}

void Emitter::SetDebugLoc(void* callInst)
{
    static_cast<llvm::CallInst*>(callInst)->setDebugLoc(GetCurrentDebugLocation());
}

void* Emitter::CreateRet(void* value)
{
    return builder.CreateRet(static_cast<llvm::Value*>(value));
}

void* Emitter::CreateRetVoid()
{
    return builder.CreateRetVoid();
}

void Emitter::SetPersonalityFunction(void* function, void* personalityFunction)
{
    static_cast<llvm::Function*>(function)->setPersonalityFn(llvm::ConstantExpr::getBitCast(static_cast<llvm::Function*>(personalityFunction), builder.getInt8PtrTy()));
}

void Emitter::AddNoUnwindAttribute(void* function)
{
    static_cast<llvm::Function*>(function)->addFnAttr(llvm::Attribute::NoUnwind);
}

void Emitter::AddUWTableAttribute(void* function)
{
    static_cast<llvm::Function*>(function)->addFnAttr(llvm::Attribute::UWTable);
}

void* Emitter::CreateLexicalBlock(const Span& span)
{
    llvm::DILexicalBlock* block = diBuilder->createLexicalBlock(static_cast<llvm::DIScope*>(CurrentScope()), static_cast<llvm::DIFile*>(GetDebugInfoForFile(span.fileIndex)),
        span.line, GetColumn(span));
    PushScope(block);
    return block;
}

void* Emitter::CreateSwitch(void* condition, void* defaultDest, unsigned numCases)
{
    void* switchInst = builder.CreateSwitch(static_cast<llvm::Value*>(condition), static_cast<llvm::BasicBlock*>(defaultDest), numCases);
    return switchInst;
}

void Emitter::AddCase(void* switchInst, void* caseValue, void* caseDest)
{
    static_cast<llvm::SwitchInst*>(switchInst)->addCase(llvm::cast<llvm::ConstantInt>(static_cast<llvm::Value*>(caseValue)), static_cast<llvm::BasicBlock*>(caseDest));
}

void* Emitter::CreateCleanupPadWithParent(void* parentPad, const std::vector<void*>& args)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    return builder.CreateCleanupPad(static_cast<llvm::Value*>(parentPad), arguments);
}

void* Emitter::CreateCleanupPad(const std::vector<void*>& args)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    return builder.CreateCleanupPad(llvm::ConstantTokenNone::get(context), arguments);
}

void* Emitter::CreateCleanupRet(void* cleanupPad, void* unwindTarget)
{
    return builder.CreateCleanupRet(llvm::cast<llvm::CleanupPadInst>(static_cast<llvm::Value*>(cleanupPad)), static_cast<llvm::BasicBlock*>(unwindTarget));
}

void* Emitter::CreateCatchRet(void* catchPad, void* returnTarget)
{
    return builder.CreateCatchRet(llvm::cast<llvm::CatchPadInst>(static_cast<llvm::Value*>(catchPad)), static_cast<llvm::BasicBlock*>(returnTarget));
}

void* Emitter::CreateCatchSwitch(void* unwindBlock)
{
    return builder.CreateCatchSwitch(llvm::ConstantTokenNone::get(context), static_cast<llvm::BasicBlock*>(unwindBlock), 1);
}

void* Emitter::CreateCatchSwitchWithParent(void* parentPad, void* unwindBlock)
{
    return builder.CreateCatchSwitch(static_cast<llvm::Value*>(parentPad), static_cast<llvm::BasicBlock*>(unwindBlock), 1);
}

void Emitter::AddHandlerToCatchSwitch(void* catchSwitch, void* target)
{
    static_cast<llvm::CatchSwitchInst*>(catchSwitch)->addHandler(static_cast<llvm::BasicBlock*>(target));
}

void* Emitter::CreateCatchPad(void* parentPad, const std::vector<void*>& args)
{
    std::vector<llvm::Value*> arguments;
    for (void* arg : args)
    {
        arguments.push_back(static_cast<llvm::Value*>(arg));
    }
    return builder.CreateCatchPad(static_cast<llvm::Value*>(parentPad), arguments);
}

void* Emitter::GenerateTrap(const std::vector<void*>& args)
{
    return nullptr;
}

void Emitter::SetCompileUnitId(const std::string& compileUnitId_)
{
    compileUnitId = compileUnitId_;
}

void* Emitter::GetClsIdValue(const std::string& typeId)
{
    return nullptr;
}

void* Emitter::CreateMDBool(bool value)
{
    return nullptr;
}

void* Emitter::CreateMDLong(int64_t value)
{
    return nullptr;
}

void* Emitter::CreateMDString(const std::string& value)
{
    return nullptr;
}

void* Emitter::CreateMDStructRef(int id)
{
    return nullptr;
}

void* Emitter::CreateMDStruct()
{
    return nullptr;
}

void* Emitter::CreateMDBasicBlockRef(void* bb)
{
    return nullptr;
}

int Emitter::GetMDStructId(void* mdStruct)
{
    return 0;
}

void Emitter::AddMDItem(void* mdStruct, const std::string& fieldName, void* mdItem)
{
}

void Emitter::SetFunctionMdId(void* function, int mdId)
{
}

void Emitter::SetCurrentSourceSpan(int32_t line, int16_t scol, int16_t ecol)
{
}

void* Emitter::GetMDStructRefForSourceFile(const std::string& sourceFileName)
{
    return nullptr;
}

void Emitter::SetMetadataRef(void* inst, void* mdStructRef)
{
}

void Emitter::FinalizeFunction(void* function, bool hasCleanup)
{
}

int Emitter::Install(const std::string& str)
{
    return emittingDelegate->Install(str);
}

int Emitter::Install(const std::u16string& str)
{
    return emittingDelegate->Install(str);
}

int Emitter::Install(const std::u32string& str)
{
    return emittingDelegate->Install(str);
}

void* Emitter::CreateLandingPad(void* lpType)
{
    llvm::LandingPadInst* lp = builder.CreateLandingPad(static_cast<llvm::Type*>(lpType), 1);
    return lp;
}

void Emitter::SetLandindPadAsCleanup(void* landingPad)
{
    llvm::LandingPadInst* lp = static_cast<llvm::LandingPadInst*>(landingPad);
    lp->setCleanup(true);
}

void Emitter::InsertAllocaIntoBasicBlock(void* allocaInst, void* lastAlloca, void* basicBlock)
{
    if (lastAlloca)
    {
        static_cast<llvm::AllocaInst*>(allocaInst)->insertAfter(static_cast<llvm::AllocaInst*>(lastAlloca));
    }
    else
    {
        llvm::BasicBlock* block = static_cast<llvm::BasicBlock*>(basicBlock);
        if (block->empty())
        {
            block->getInstList().push_back(static_cast<llvm::AllocaInst*>(allocaInst));
        }
        else
        {
            block->getInstList().insert(block->getInstList().begin(), static_cast<llvm::AllocaInst*>(allocaInst));
        }
    }
}

void Emitter::AddClauseToLangdingPad(void* landingPad, void* exceptionTypeId)
{
    llvm::LandingPadInst* lp = static_cast<llvm::LandingPadInst*>(landingPad);
    llvm::Constant* typeId = static_cast<llvm::Constant*>(exceptionTypeId);
    lp->addClause(llvm::cast<llvm::Constant>(builder.CreateBitCast(typeId, builder.getInt8PtrTy())));
}

void* Emitter::CreateExtractValue(void* aggregate, const std::vector<unsigned int>& indeces)
{
    return builder.CreateExtractValue(static_cast<llvm::Value*>(aggregate), indeces);
}

void* Emitter::CreateInsertValue(void* aggregate, void* value, const std::vector<unsigned int>& indeces)
{
    return builder.CreateInsertValue(static_cast<llvm::Value*>(aggregate), static_cast<llvm::Value*>(value), indeces);
}

void* Emitter::CreateUndefValue(void* type)
{
    return llvm::UndefValue::get(static_cast<llvm::Type*>(type));
}

void Emitter::CreateResume(void* exception)
{
    builder.CreateResume(static_cast<llvm::Value*>(exception));
}

void Emitter::DebugPrintDebugInfo(const std::string& filePath)
{
    std::vector<std::string> info;
    for (auto& p : diTypeNameMap)
    {
        std::stringstream s;
        s << std::setw(16) << std::setfill('0');
        s << std::hex << p.first;
        s << std::setw(0);
        s << " = " << p.second;
        info.push_back(s.str());
    }
    std::sort(info.begin(), info.end());
    std::ofstream file(filePath);
    for (const std::string& line : info)
    {
        file << line << std::endl;
    }
}

void Emitter::BeginSubstituteLineNumber(int32_t lineNumber)
{
    substituteLineNumber = true;
    currentLineNumber = lineNumber;
}

void Emitter::EndSubstituteLineNumber()
{
    substituteLineNumber = false;
}

} // namespace cmllvm
