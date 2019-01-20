// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/emitter/BasicEmitter.hpp>
#include <cmajor/emitter/EmittingContextImpl.hpp>
#include <cmajor/binder/BoundNamespace.hpp>
#include <cmajor/binder/BoundEnum.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/System.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Util.hpp>
#include <mutex>
#include <fstream>

namespace cmajor { namespace emitter {

using namespace cmajor::unicode;

BasicEmitter::BasicEmitter(EmittingContext& emittingContext_, const std::string& compileUnitModuleName_, cmajor::symbols::Module& symbolsModule_) :
    cmajor::ir::Emitter(emittingContext_.GetEmittingContextImpl()->Context()), emittingContext(emittingContext_), symbolTable(nullptr),
    compileUnitModule(new llvm::Module(compileUnitModuleName_, emittingContext.GetEmittingContextImpl()->Context())), symbolsModule(symbolsModule_), builder(Builder()), stack(Stack()),
    context(emittingContext.GetEmittingContextImpl()->Context()), compileUnit(nullptr), function(nullptr), trueBlock(nullptr), falseBlock(nullptr), breakTarget(nullptr), continueTarget(nullptr),
    handlerBlock(nullptr), cleanupBlock(nullptr), entryBasicBlock(nullptr), newCleanupNeeded(false), currentPad(nullptr), genJumpingBoolCode(false), currentClass(nullptr), currentFunction(nullptr),
    currentBlock(nullptr), breakTargetBlock(nullptr), continueTargetBlock(nullptr), sequenceSecond(nullptr), currentCaseMap(nullptr), defaultDest(nullptr), prevLineNumber(0), destructorCallGenerated(false),
    lastInstructionWasRet(false), basicBlockOpen(false), lastAlloca(nullptr), compoundLevel(0), debugInfo(false)
{
    compileUnitModule->setTargetTriple(emittingContext.GetEmittingContextImpl()->TargetTriple());
    compileUnitModule->setDataLayout(emittingContext.GetEmittingContextImpl()->DataLayout());
    compileUnitModule->setSourceFileName(compileUnitModuleName_);
    SetModule(compileUnitModule.get());
    SetDataLayout(&emittingContext.GetEmittingContextImpl()->DataLayout());
}

void BasicEmitter::GenJumpingBoolCode()
{
    if (!genJumpingBoolCode) return;
    Assert(trueBlock, "true block not set");
    Assert(falseBlock, "false block not set");
    llvm::Value* cond = stack.Pop();
    if (sequenceSecond)
    {
        genJumpingBoolCode = false;
        sequenceSecond->SetGenerated();
        sequenceSecond->Accept(*this);
        genJumpingBoolCode = true;
    }
    builder.CreateCondBr(cond, trueBlock, falseBlock);
}

void BasicEmitter::SetTarget(BoundStatement* labeledStatement)
{
    if (labeledStatement->Label().empty()) return;
    auto it = labeledStatementMap.find(labeledStatement);
    if (it != labeledStatementMap.cend())
    {
        llvm::BasicBlock* target = it->second;
        builder.CreateBr(target);
        SetCurrentBasicBlock(target);
    }
    else
    {
        throw Exception(&symbolsModule, "target for labeled statement not found", labeledStatement->GetSpan());
    }
}

void BasicEmitter::ClearFlags()
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
}

void BasicEmitter::InsertAllocaIntoEntryBlock(llvm::AllocaInst* allocaInst)
{
    if (lastAlloca)
    {
        allocaInst->insertAfter(lastAlloca);
    }
    else
    {
        if (entryBasicBlock->empty())
        {
            entryBasicBlock->getInstList().push_back(allocaInst);
        }
        else
        {
            entryBasicBlock->getInstList().insert(entryBasicBlock->getInstList().begin(), allocaInst);
        }
    }
    lastAlloca = allocaInst;
}

void BasicEmitter::Visit(BoundCompileUnit& boundCompileUnit)
{
    ResetCurrentDebugLocation();
    debugInfo = false;
    SetDIBuilder(nullptr);
    if (GetGlobalFlag(GlobalFlags::generateDebugInfo) && boundCompileUnit.GetCompileUnitNode() && !boundCompileUnit.GetCompileUnitNode()->IsSynthesizedUnit())
    {
        compileUnitModule->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
#ifdef _WIN32
        compileUnitModule->addModuleFlag(llvm::Module::Warning, "CodeView", 1);
#endif
        diBuilder.reset(new llvm::DIBuilder(*compileUnitModule));
        SetDIBuilder(diBuilder.get());
        debugInfo = true;
        std::string sourceFilePath = boundCompileUnit.GetCompileUnitNode()->FilePath();
        llvm::DIFile* sourceFile = diBuilder->createFile(Path::GetFileName(sourceFilePath), Path::GetDirectoryName(sourceFilePath));
        SetDIFile(sourceFile);
        llvm::DICompileUnit* diCompileUnit = diBuilder->createCompileUnit(cmajorLanguageTag, sourceFile, "Cmajor compiler version " + GetCompilerVersion(), GetGlobalFlag(GlobalFlags::release), "", 0);
        SetDICompileUnit(diCompileUnit);
        SetCurrentCompileUnitNode(boundCompileUnit.GetCompileUnitNode());
        PushScope(sourceFile);
    }
    compileUnit = &boundCompileUnit;
    symbolTable = &boundCompileUnit.GetSymbolTable();
    ConstantArrayRepository& constantArrayRepository = boundCompileUnit.GetConstantArrayRepository();
    for (ConstantSymbol* constantSymbol : constantArrayRepository.ConstantArrays())
    {
        constantSymbol->ArrayIrObject(*this, true);
    }
    ConstantStructureRepository& constantStructureRepository = boundCompileUnit.GetConstantStructureRepository();
    for (ConstantSymbol* constantSymbol : constantStructureRepository.ConstantStructures())
    {
        constantSymbol->StructureIrObject(*this, true);
    }
    int n = boundCompileUnit.BoundNodes().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* boundNode = boundCompileUnit.BoundNodes()[i].get();
        boundNode->Accept(*this);
    }
    if (debugInfo)
    {
        ReplaceForwardDeclarations();
        diBuilder->finalize();
    }
    if (GetGlobalFlag(GlobalFlags::emitLlvm))
    {
        std::ofstream llFile(boundCompileUnit.LLFilePath());
        llvm::raw_os_ostream llOs(llFile);
        compileUnitModule->print(llOs, nullptr);
    }
    std::string errorMessageStore;
    llvm::raw_string_ostream errorMessage(errorMessageStore);
    if (verifyModule(*compileUnitModule, &errorMessage))
    {
        throw std::runtime_error("Emitter: verification of module '" + compileUnitModule->getSourceFileName() + "' failed. " + errorMessage.str());
    }
    llvm::legacy::PassManager passManager;
    std::error_code errorCode;
    llvm::raw_fd_ostream objectFile(boundCompileUnit.ObjectFilePath(), errorCode, llvm::sys::fs::F_None);
    if (emittingContext.GetEmittingContextImpl()->TargetMachine().addPassesToEmitFile(passManager, objectFile, nullptr, llvm::TargetMachine::CodeGenFileType::CGFT_ObjectFile))
    {
        throw std::runtime_error("Emitter: cannot emit object code file '" + boundCompileUnit.ObjectFilePath() + "': addPassesToEmitFile failed");
    }
    passManager.run(*compileUnitModule);
    objectFile.flush();
    if (objectFile.has_error())
    {
        throw std::runtime_error("Emitter: could not emit object code file '" + boundCompileUnit.ObjectFilePath() + "': " + errorCode.message());
    }
    if (GetGlobalFlag(GlobalFlags::emitOptLlvm))
    {
        std::string optCommandLine;
        optCommandLine.append("opt -O").append(std::to_string(GetOptimizationLevel())).append(" ").append(QuotedPath(boundCompileUnit.LLFilePath())).append(" -S -o ");
        optCommandLine.append(QuotedPath(boundCompileUnit.OptLLFilePath()));
        System(optCommandLine);
    }
    if (debugInfo)
    {
        PopScope();
        diBuilder.reset();
    }
}

void BasicEmitter::Visit(BoundNamespace& boundNamespace)
{
    int numComponents = 0;
    if (debugInfo)
    {
        if (!boundNamespace.GetNamespaceNode().Id()->Str().empty())
        {
            std::vector<std::u32string> components = Split(boundNamespace.GetNamespaceNode().Id()->Str(), '.');
            numComponents = components.size();
            for (int i = 0; i < numComponents; ++i)
            {
                llvm::DINamespace* ns = diBuilder->createNameSpace(CurrentScope(), ToUtf8(components[i]), false);
                PushScope(ns);
            }
        }
        else
        {
            PushScope(GetFile(boundNamespace.GetSpan().FileIndex()));
        }
    }
    int n = boundNamespace.Members().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* member = boundNamespace.Members()[i].get();
        member->Accept(*this);
    }
    if (debugInfo)
    {
        if (!boundNamespace.GetNamespaceNode().Id()->Str().empty())
        {
            for (int i = 0; i < numComponents; ++i)
            {
                PopScope();
            }
        }
        else
        {
            PopScope();
        }
    }
}

void BasicEmitter::Visit(BoundClass& boundClass)
{
    classStack.push(currentClass);
    currentClass = &boundClass;
    bool prevDebugInfo = debugInfo;
    llvm::DIBuilder* prevDIBuilder = DIBuilder();
    if (!boundClass.ContainsSourceFunctions())
    {
        ResetCurrentDebugLocation();
        debugInfo = false;
        SetDIBuilder(nullptr);
    }
    if (debugInfo)
    {
        MapClassPtr(currentClass->GetClassTypeSymbol()->TypeId(), currentClass->GetClassTypeSymbol());
        llvm::DIType* diType = GetDITypeByTypeId(currentClass->GetClassTypeSymbol()->TypeId());
        if (diType)
        {
            PushScope(diType);
        }
        else
        {
            llvm::DIType* baseClass = nullptr;
            if (currentClass->GetClassTypeSymbol()->BaseClass())
            {
                baseClass = currentClass->GetClassTypeSymbol()->BaseClass()->GetDIType(*this);
            }
            llvm::DIType* vtableHolderClass = nullptr;
            if (currentClass->GetClassTypeSymbol()->IsPolymorphic() && currentClass->GetClassTypeSymbol()->VmtPtrHolderClass())
            {
                vtableHolderClass = currentClass->GetClassTypeSymbol()->VmtPtrHolderClass()->CreateDIForwardDeclaration(*this);
                MapFwdDeclaration(vtableHolderClass, currentClass->GetClassTypeSymbol()->VmtPtrHolderClass()->TypeId());
            }
            llvm::MDNode* templateParams = nullptr;
            uint64_t sizeInBits = DataLayout()->getStructLayout(llvm::cast<llvm::StructType>(currentClass->GetClassTypeSymbol()->IrType(*this)))->getSizeInBits();
            uint32_t alignInBits = 8 * DataLayout()->getStructLayout(llvm::cast<llvm::StructType>(currentClass->GetClassTypeSymbol()->IrType(*this)))->getAlignment();
            uint64_t offsetInBits = 0; // todo?
            llvm::DINode::DIFlags flags = llvm::DINode::DIFlags::FlagZero;
            Span classSpan = currentClass->GetClassTypeSymbol()->GetSpan();
            if (currentClass->GetClassTypeSymbol()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(currentClass->GetClassTypeSymbol());
                classSpan = specialization->GetClassTemplate()->GetSpan();
            }
            llvm::DIType* forwardDeclaration = diBuilder->createReplaceableCompositeType(llvm::dwarf::DW_TAG_class_type, ToUtf8(currentClass->GetClassTypeSymbol()->Name()), 
                CurrentScope(), GetFile(classSpan.FileIndex()), classSpan.LineNumber(),
                0, sizeInBits, alignInBits, llvm::DINode::DIFlags::FlagZero, ToUtf8(currentClass->GetClassTypeSymbol()->MangledName()));
            SetDITypeByTypeId(currentClass->GetClassTypeSymbol()->TypeId(), forwardDeclaration);
            std::vector<llvm::Metadata*> elements;
            for (MemberVariableSymbol* memberVariable : currentClass->GetClassTypeSymbol()->MemberVariables())
            {
                int memberVariableLayoutIndex = memberVariable->LayoutIndex();
                uint64_t offsetInBits = DataLayout()->getStructLayout(llvm::cast<llvm::StructType>(currentClass->GetClassTypeSymbol()->IrType(*this)))->getElementOffsetInBits(memberVariableLayoutIndex);
                elements.push_back(memberVariable->GetDIMemberType(*this, offsetInBits));
            }
            llvm::DICompositeType* cls = diBuilder->createClassType(CurrentScope(), ToUtf8(currentClass->GetClassTypeSymbol()->Name()),
                GetFile(classSpan.FileIndex()),
                classSpan.LineNumber(), sizeInBits, alignInBits, offsetInBits, flags, baseClass, diBuilder->getOrCreateArray(elements), vtableHolderClass, templateParams, 
                ToUtf8(currentClass->GetClassTypeSymbol()->MangledName()));
            MapFwdDeclaration(forwardDeclaration, currentClass->GetClassTypeSymbol()->TypeId());
            SetDITypeByTypeId(currentClass->GetClassTypeSymbol()->TypeId(), cls);
            PushScope(cls);
        }
    }
    int n = boundClass.Members().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* boundNode = boundClass.Members()[i].get();
        boundNode->Accept(*this);
    }
    if (debugInfo)
    {
        PopScope();
    }
    currentClass = classStack.top();
    classStack.pop();
    debugInfo = prevDebugInfo;
    SetDIBuilder(prevDIBuilder);
}

void BasicEmitter::Visit(BoundEnumTypeDefinition& boundEnumTypeDefinition)
{
    if (debugInfo)
    {
        EnumTypeSymbol* enumTypeSymbol = boundEnumTypeDefinition.GetEnumTypeSymbol();
        uint64_t sizeInBits = enumTypeSymbol->SizeInBits(*this);
        uint32_t alignInBits = enumTypeSymbol->AlignmentInBits(*this);
        std::vector<llvm::Metadata*> elements;
        std::vector<EnumConstantSymbol*> enumConstants = enumTypeSymbol->GetEnumConstants();
        for (EnumConstantSymbol* enumConstant : enumConstants)
        {
            int64_t value = 0;
            if (enumTypeSymbol->UnderlyingType()->IsUnsignedType())
            {
                Value* val = enumConstant->GetValue()->As(symbolTable->GetTypeByName(U"ulong"), false, enumTypeSymbol->GetSpan(), true);
                if (val)
                {
                    ULongValue* ulongValue = static_cast<ULongValue*>(val);
                    value = static_cast<int64_t>(ulongValue->GetValue());
                }
            }
            else
            {
                Value* val = enumConstant->GetValue()->As(symbolTable->GetTypeByName(U"long"), false, enumTypeSymbol->GetSpan(), true);
                if (val)
                {
                    LongValue* longValue = static_cast<LongValue*>(val);
                    value = longValue->GetValue();
                }
            }
            elements.push_back(diBuilder->createEnumerator(ToUtf8(enumConstant->Name()), value));
        }
        llvm::DIType* enumType = diBuilder->createEnumerationType(CurrentScope(), ToUtf8(enumTypeSymbol->Name()), GetFile(enumTypeSymbol->GetSpan().FileIndex()),
            enumTypeSymbol->GetSpan().LineNumber(), sizeInBits, alignInBits, diBuilder->getOrCreateArray(elements), enumTypeSymbol->UnderlyingType()->GetDIType(*this), ToUtf8(enumTypeSymbol->MangledName()));
        SetDITypeByTypeId(enumTypeSymbol->TypeId(), enumType);
    }
}

void BasicEmitter::Visit(BoundFunction& boundFunction)
{
    if (!boundFunction.Body()) return;
    currentFunction = &boundFunction;
    handlerBlock = nullptr;
    cleanupBlock = nullptr;
    newCleanupNeeded = false;
    currentPad = nullptr;
    prevLineNumber = 0;
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    lastAlloca = nullptr;
    compoundLevel = 0;
    cleanups.clear();
    pads.clear();
    labeledStatementMap.clear();
    FunctionSymbol* functionSymbol = boundFunction.GetFunctionSymbol();
    llvm::FunctionType* functionType = functionSymbol->IrType(*this);
    function = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction(ToUtf8(functionSymbol->MangledName()), functionType));
    if (GetGlobalFlag(GlobalFlags::release) && functionSymbol->IsInline())
    {
        function->addFnAttr(llvm::Attribute::InlineHint);
        functionSymbol->SetLinkOnceOdrLinkage();
    }
    if (functionSymbol->HasLinkOnceOdrLinkage())
    {
        llvm::Comdat* comdat = compileUnitModule->getOrInsertComdat(ToUtf8(functionSymbol->MangledName()));
        comdat->setSelectionKind(llvm::Comdat::SelectionKind::Any);
        function->setLinkage(llvm::GlobalValue::LinkageTypes::LinkOnceODRLinkage);
        function->setComdat(comdat);
    }
    SetFunction(function);
    bool hasSource = functionSymbol->HasSource();
    bool prevDebugInfo = debugInfo;
    llvm::DIBuilder* prevDIBuilder = DIBuilder();
    if (!hasSource)
    {
        ResetCurrentDebugLocation();
        debugInfo = false;
        SetDIBuilder(nullptr);
    }
    if (debugInfo)
    {
        SetInPrologue(true);
        SetCurrentDebugLocation(Span());
        std::vector<llvm::Metadata*> elementTypes;
        if (functionSymbol->ReturnType())
        {
            elementTypes.push_back(functionSymbol->ReturnType()->GetDIType(*this)); // 0'th entry is return type
        }
        else
        {
            elementTypes.push_back(symbolTable->GetTypeByName(U"void")->GetDIType(*this));
        }
        for (ParameterSymbol* parameter : functionSymbol->Parameters())
        {
            TypeSymbol* parameterType = parameter->GetType();
            elementTypes.push_back(parameterType->GetDIType(*this));
        }
        llvm::DISubroutineType* subroutineType = diBuilder->createSubroutineType(diBuilder->getOrCreateTypeArray(elementTypes));
        llvm::DISubprogram* subprogram = nullptr;
        if (functionSymbol->IsConstructorDestructorOrNonstaticMemberFunction())
        {
            unsigned int virtuality = 0;
            if (functionSymbol->IsVirtualAbstractOrOverride())
            {
                if (functionSymbol->IsAbstract())
                {
                    virtuality = llvm::dwarf::DW_VIRTUALITY_pure_virtual;
                }
                else
                {
                    virtuality = llvm::dwarf::DW_VIRTUALITY_virtual;
                }
            }
            llvm::DIType* vtableHolder = nullptr;
            Symbol* parent = functionSymbol->Parent();
            if (parent && parent->IsClassTypeSymbol())
            {
                ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(parent);
                if (cls->IsPolymorphic() && cls->VmtPtrHolderClass())
                {
                    vtableHolder = cls->VmtPtrHolderClass()->GetDIType(*this);
                }
            }
            llvm::DINode::DIFlags flags = llvm::DINode::DIFlags::FlagZero;
            if (functionSymbol->IsStatic())
            {
                flags |= llvm::DINode::FlagStaticMember;
            }
            flags |= AccessFlag(functionSymbol->Access());
            if (functionSymbol->IsExplicit())
            {
                flags |= llvm::DINode::FlagExplicit;
            }
            unsigned vtableIndex = 0;
            if (functionSymbol->VmtIndex() != -1)
            {
                vtableIndex = functionSymbol->VmtIndex();
            }
            subprogram = diBuilder->createMethod(CurrentScope(), ToUtf8(functionSymbol->Name()), ToUtf8(functionSymbol->MangledName()), GetFile(functionSymbol->GetSpan().FileIndex()), functionSymbol->GetSpan().LineNumber(),
                subroutineType, false, true, virtuality, vtableIndex, 0, vtableHolder, flags); 
        }
        else
        {
            llvm::DINode::DIFlags flags = AccessFlag(functionSymbol->Access());
            subprogram = diBuilder->createFunction(CurrentScope(), ToUtf8(functionSymbol->Name()), ToUtf8(functionSymbol->MangledName()), GetFile(functionSymbol->GetSpan().FileIndex()), 
                functionSymbol->GetSpan().LineNumber(),
                subroutineType, false, true, functionSymbol->GetSpan().LineNumber(), flags); 
        }
        function->setSubprogram(subprogram);
        PushScope(subprogram);
    }
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", function);
    entryBasicBlock = entryBlock;
    SetCurrentBasicBlock(entryBlock);
    if (currentClass && !currentClass->IsInlineFunctionContainer())
    {
        ClassTypeSymbol* classTypeSymbol = currentClass->GetClassTypeSymbol();
        if (!IsVmtObjectCreated(classTypeSymbol))
        {
            classTypeSymbol->VmtObject(*this, true);
        }
        if (!IsStaticObjectCreated(classTypeSymbol))
        {
            classTypeSymbol->StaticObject(*this, true);
        }
    }
    int np = functionSymbol->Parameters().size();
    for (int i = 0; i < np; ++i)
    {
        ParameterSymbol* parameter = functionSymbol->Parameters()[i];
        llvm::AllocaInst* allocaInst = builder.CreateAlloca(parameter->GetType()->IrType(*this));
        SetIrObject(parameter, allocaInst);
        lastAlloca = allocaInst;
        if (debugInfo)
        {
            llvm::SmallVector<int64_t, 13> expr; // todo
            llvm::DILocalVariable* paramVar = diBuilder->createParameterVariable(CurrentScope(), ToUtf8(parameter->Name()), i + 1, GetFile(parameter->GetSpan().FileIndex()), 
                parameter->GetSpan().LineNumber(), parameter->GetType()->GetDIType(*this));
            diBuilder->insertDeclare(allocaInst, paramVar, diBuilder->createExpression(expr), GetDebugLocation(parameter->GetSpan()), builder.GetInsertBlock()); 
        }
    }
    if (functionSymbol->ReturnParam())
    {
        ParameterSymbol* parameter = functionSymbol->ReturnParam();
        llvm::AllocaInst* allocaInst = builder.CreateAlloca(parameter->GetType()->IrType(*this));
        SetIrObject(parameter, allocaInst);
        lastAlloca = allocaInst;
    }
    int nlv = functionSymbol->LocalVariables().size();
    for (int i = 0; i < nlv; ++i)
    {
        LocalVariableSymbol* localVariable = functionSymbol->LocalVariables()[i];
        llvm::AllocaInst* allocaInst = builder.CreateAlloca(localVariable->GetType()->IrType(*this));
        SetIrObject(localVariable, allocaInst);
        lastAlloca = allocaInst;
        if (debugInfo && localVariable->GetSpan().Valid())
        {
            llvm::SmallVector<int64_t, 13> expr; // todo
            llvm::DILocalVariable* localVar = diBuilder->createAutoVariable(CurrentScope(), ToUtf8(localVariable->Name()), GetFile(localVariable->GetSpan().FileIndex()),
                localVariable->GetSpan().LineNumber(), localVariable->GetType()->GetDIType(*this));
            diBuilder->insertDeclare(allocaInst, localVar, diBuilder->createExpression(expr), GetDebugLocation(localVariable->GetSpan()), builder.GetInsertBlock());
        }
    }
    if (!functionSymbol->DontThrow())
    {
        int funId = compileUnit->Install(ToUtf8(functionSymbol->FullName()));
        int sfpId = compileUnit->Install(compileUnit->SourceFilePath());
        llvm::Value* funValue = GetGlobalStringPtr(funId);
        llvm::Value* sfpValue = GetGlobalStringPtr(sfpId);
        std::vector<llvm::Type*> enterFunctionParamTypes;
        enterFunctionParamTypes.push_back(builder.getInt8PtrTy());
        enterFunctionParamTypes.push_back(builder.getInt8PtrTy());
        llvm::FunctionType* enterFunctionType = llvm::FunctionType::get(builder.getVoidTy(), enterFunctionParamTypes, false);
        llvm::Function* enterFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("RtEnterFunction", enterFunctionType));
        ArgVector enterFunctionArgs;
        enterFunctionArgs.push_back(funValue);
        enterFunctionArgs.push_back(sfpValue);
        builder.CreateCall(enterFunction, enterFunctionArgs);
    }
    auto it = function->args().begin();
    for (int i = 0; i < np; ++i)
    {
        ParameterSymbol* parameter = functionSymbol->Parameters()[i];
        if (parameter->GetType()->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parameter->GetType());
            FunctionSymbol* copyConstructor = classType->CopyConstructor();
            if (!copyConstructor)
            {
                copyConstructor = compileUnit->GetCopyConstructorFor(classType->TypeId());
            }
            llvm::FunctionType* copyCtorType = copyConstructor->IrType(*this);
            llvm::Function* callee = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction(ToUtf8(copyConstructor->MangledName()), copyCtorType));
            ArgVector args;
            args.push_back(parameter->IrObject(*this));
            args.push_back(&*it);
            if (debugInfo)
            {
                SetInPrologue(false);
                SetCurrentDebugLocation(boundFunction.Body()->GetSpan());
                builder.CreateCall(callee, args);
                SetInPrologue(true);
                SetCurrentDebugLocation(Span());
            }
            else
            {
                builder.CreateCall(callee, args);
            }
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
            LlvmValue paramValue(parameter->IrObject(*this));
            copyCtorArgs.push_back(&paramValue);
            LlvmValue argumentValue(&*it);
            copyCtorArgs.push_back(&argumentValue);
            if (debugInfo)
            {
                SetInPrologue(false);
                SetCurrentDebugLocation(boundFunction.Body()->GetSpan());
                copyConstructor->GenerateCall(*this, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
                SetInPrologue(true);
                SetCurrentDebugLocation(Span());
            }
            else 
            {
                copyConstructor->GenerateCall(*this, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
            }
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
            LlvmValue paramValue(parameter->IrObject(*this));
            paramValue.SetType(interfaceType->AddPointer(Span()));
            copyCtorArgs.push_back(&paramValue);
            LlvmValue argumentValue(&*it);
            argumentValue.SetType(interfaceType->AddPointer(Span()));
            copyCtorArgs.push_back(&argumentValue);
            if (debugInfo)
            {
                SetInPrologue(false);
                SetCurrentDebugLocation(boundFunction.Body()->GetSpan());
                copyConstructor->GenerateCall(*this, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
                SetInPrologue(true);
                SetCurrentDebugLocation(Span());
            }
            else
            {
                copyConstructor->GenerateCall(*this, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
            }
        }
        else
        {
            builder.CreateStore(&*it, parameter->IrObject(*this));
        }
        ++it;
    }
    if (functionSymbol->ReturnParam())
    {
        builder.CreateStore(&*it, functionSymbol->ReturnParam()->IrObject(*this));
    }
    for (BoundStatement* labeledStatement : boundFunction.LabeledStatements())
    {
        llvm::BasicBlock* target = llvm::BasicBlock::Create(context, ToUtf8(labeledStatement->Label()), function);
        labeledStatementMap[labeledStatement] = target;
    }
    BoundCompoundStatement* body = boundFunction.Body();
    if (debugInfo)
    {
        SetInPrologue(false);
    }
    body->Accept(*this);
    BoundStatement* lastStatement = nullptr;
    if (!body->Statements().empty())
    {
        lastStatement = body->Statements().back().get();
    }
    if (!lastStatement || lastStatement->GetBoundNodeType() != BoundNodeType::boundReturnStatement || lastStatement->GetBoundNodeType() == BoundNodeType::boundReturnStatement && destructorCallGenerated)
    {
        CreateExitFunctionCall();
        if (functionSymbol->ReturnType() && functionSymbol->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol && !functionSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
        {
            llvm::Value* defaultValue = functionSymbol->ReturnType()->CreateDefaultIrValue(*this);
            builder.CreateRet(defaultValue);
            lastInstructionWasRet = true;
        }
        else
        {
            builder.CreateRetVoid();
            lastInstructionWasRet = true;
        }
    }
    if (functionSymbol->HasTry() || !cleanups.empty())
    {
        llvm::Function* personalityFunction = GetPersonalityFunction();
        function->setPersonalityFn(llvm::ConstantExpr::getBitCast(personalityFunction, builder.getInt8PtrTy()));
    }
    if (functionSymbol->DontThrow() && !functionSymbol->HasTry() && cleanups.empty())
    {
        function->addFnAttr(llvm::Attribute::NoUnwind);
    }
    else
    {
        function->addFnAttr(llvm::Attribute::UWTable);
    }
    GenerateCodeForCleanups();
    if (debugInfo)
    {
        PopScope();
    }
    debugInfo = prevDebugInfo;
    SetDIBuilder(prevDIBuilder);
}

void BasicEmitter::Visit(BoundSequenceStatement& boundSequenceStatement)
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

void BasicEmitter::ExitBlocks(BoundCompoundStatement* targetBlock)
{
    if (debugInfo)
    {
        if (currentBlock->EndSpan().Valid())
        {
            SetCurrentDebugLocation(currentBlock->EndSpan());
        }
    }
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
                        llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
                        if (!lastInstructionWasRet)
                        {
                            builder.CreateBr(nextBlock);
                        }
                        SetCurrentBasicBlock(nextBlock);
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

void BasicEmitter::Visit(BoundCompoundStatement& boundCompoundStatement)
{
    if (debugInfo)
    {
        SetCurrentDebugLocation(boundCompoundStatement.GetSpan());
    }
    if (debugInfo && compoundLevel > 0)
    {
        llvm::DILexicalBlock* block = diBuilder->createLexicalBlock(CurrentScope(), GetFile(boundCompoundStatement.GetSpan().FileIndex()), boundCompoundStatement.GetSpan().LineNumber(), 
            GetColumn(boundCompoundStatement.GetSpan()));
        PushScope(block);
    }
    ++compoundLevel;
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundCompoundStatement);
    BoundCompoundStatement* prevBlock = currentBlock;
    currentBlock = &boundCompoundStatement;
    blockDestructionMap[currentBlock] = std::vector<std::unique_ptr<BoundFunctionCall>>();
    blocks.push_back(currentBlock);
    SetLineNumber(boundCompoundStatement.GetSpan().LineNumber());
    int n = boundCompoundStatement.Statements().size();
    for (int i = 0; i < n; ++i)
    {
        BoundStatement* boundStatement = boundCompoundStatement.Statements()[i].get();
        boundStatement->Accept(*this);
    }
    ExitBlocks(prevBlock);
    blocks.pop_back();
    currentBlock = prevBlock;
    --compoundLevel;
    if (debugInfo && compoundLevel > 0)
    {
        PopScope();
    }
}

void BasicEmitter::Visit(BoundIfStatement& boundIfStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundIfStatement);
    llvm::BasicBlock* prevTrueBlock = trueBlock;
    llvm::BasicBlock* prevFalseBlock = falseBlock;
    trueBlock = llvm::BasicBlock::Create(context, "true", function);
    llvm::BasicBlock* nextBlock = llvm::BasicBlock::Create(context, "next", function);
    if (boundIfStatement.ElseS())
    {
        falseBlock = llvm::BasicBlock::Create(context, "false", function);
    }
    else
    {
        falseBlock = nextBlock;
    }
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundIfStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    SetCurrentBasicBlock(trueBlock);
    boundIfStatement.ThenS()->Accept(*this);
    builder.CreateBr(nextBlock);
    if (boundIfStatement.ElseS())
    {
        SetCurrentBasicBlock(falseBlock);
        boundIfStatement.ElseS()->Accept(*this);
        builder.CreateBr(nextBlock);
    }
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
    SetCurrentBasicBlock(nextBlock);
    basicBlockOpen = true;
}

void BasicEmitter::Visit(BoundWhileStatement& boundWhileStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundWhileStatement);
    llvm::BasicBlock* prevTrueBlock = trueBlock;
    llvm::BasicBlock* prevFalseBlock = falseBlock;
    llvm::BasicBlock* prevBreakTarget = breakTarget;
    llvm::BasicBlock* prevContinueTarget = continueTarget;
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    BoundCompoundStatement* prevContinueTargetBlock = continueTargetBlock;
    breakTargetBlock = currentBlock;
    continueTargetBlock = currentBlock;
    trueBlock = llvm::BasicBlock::Create(context, "true", function);
    falseBlock = llvm::BasicBlock::Create(context, "next", function);
    breakTarget = falseBlock;
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context, "cond", function);
    builder.CreateBr(condBlock);
    SetCurrentBasicBlock(condBlock);
    continueTarget = condBlock;
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundWhileStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    SetCurrentBasicBlock(trueBlock);
    boundWhileStatement.Statement()->Accept(*this);
    builder.CreateBr(condBlock);
    SetCurrentBasicBlock(falseBlock);
    breakTargetBlock = prevBreakTargetBlock;
    continueTargetBlock = prevContinueTargetBlock;
    breakTarget = prevBreakTarget;
    continueTarget = prevContinueTarget;
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
}

void BasicEmitter::Visit(BoundDoStatement& boundDoStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundDoStatement);
    llvm::BasicBlock* prevTrueBlock = trueBlock;
    llvm::BasicBlock* prevFalseBlock = falseBlock;
    llvm::BasicBlock* prevBreakTarget = breakTarget;
    llvm::BasicBlock* prevContinueTarget = continueTarget;
    llvm::BasicBlock* doBlock = llvm::BasicBlock::Create(context, "do", function);
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context, "cond", function);
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    BoundCompoundStatement* prevContinueTargetBlock = continueTargetBlock;
    breakTargetBlock = currentBlock;
    continueTargetBlock = currentBlock;
    trueBlock = doBlock;
    falseBlock = llvm::BasicBlock::Create(context, "next", function);
    breakTarget = falseBlock;
    continueTarget = condBlock;
    builder.CreateBr(doBlock);
    SetCurrentBasicBlock(doBlock);
    boundDoStatement.Statement()->Accept(*this);
    builder.CreateBr(condBlock);
    SetCurrentBasicBlock(condBlock);
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundDoStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    SetCurrentBasicBlock(falseBlock);
    basicBlockOpen = true;
    breakTargetBlock = prevBreakTargetBlock;
    continueTargetBlock = prevContinueTargetBlock;
    breakTarget = prevBreakTarget;
    continueTarget = prevContinueTarget;
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
}

void BasicEmitter::Visit(BoundForStatement& boundForStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundForStatement);
    llvm::BasicBlock* prevTrueBlock = trueBlock;
    llvm::BasicBlock* prevFalseBlock = falseBlock;
    llvm::BasicBlock* prevBreakTarget = breakTarget;
    llvm::BasicBlock* prevContinueTarget = continueTarget;
    boundForStatement.InitS()->Accept(*this);
    llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context, "cond", function);
    llvm::BasicBlock* actionBlock = llvm::BasicBlock::Create(context, "action", function);
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(context, "loop", function);
    trueBlock = actionBlock;
    falseBlock = llvm::BasicBlock::Create(context, "next", function);
    breakTarget = falseBlock;
    continueTarget = loopBlock;
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    BoundCompoundStatement* prevContinueTargetBlock = continueTargetBlock;
    breakTargetBlock = currentBlock;
    continueTargetBlock = currentBlock;
    builder.CreateBr(condBlock);
    SetCurrentBasicBlock(condBlock);
    bool prevGenJumpingBoolCode = genJumpingBoolCode;
    genJumpingBoolCode = true;
    boundForStatement.Condition()->Accept(*this);
    genJumpingBoolCode = prevGenJumpingBoolCode;
    SetCurrentBasicBlock(actionBlock);
    boundForStatement.ActionS()->Accept(*this);
    builder.CreateBr(loopBlock);
    SetCurrentBasicBlock(loopBlock);
    boundForStatement.LoopS()->Accept(*this);
    builder.CreateBr(condBlock);
    SetCurrentBasicBlock(falseBlock);
    basicBlockOpen = true;
    breakTargetBlock = prevBreakTargetBlock;
    continueTargetBlock = prevContinueTargetBlock;
    breakTarget = prevBreakTarget;
    continueTarget = prevContinueTarget;
    trueBlock = prevTrueBlock;
    falseBlock = prevFalseBlock;
}

void BasicEmitter::Visit(BoundSwitchStatement& boundSwitchStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundSwitchStatement);
    llvm::BasicBlock* prevBreakTarget = breakTarget;
    BoundCompoundStatement* prevBreakTargetBlock = breakTargetBlock;
    breakTargetBlock = currentBlock;
    boundSwitchStatement.Condition()->Accept(*this);
    llvm::Value* condition = stack.Pop();
    llvm::BasicBlock* prevDefaultDest = defaultDest;
    llvm::BasicBlock* next = nullptr;
    if (boundSwitchStatement.DefaultStatement())
    {
        defaultDest = llvm::BasicBlock::Create(context, "default", function);
        next = llvm::BasicBlock::Create(context, "next", function);
    }
    else
    {
        defaultDest = llvm::BasicBlock::Create(context, "next", function);
        next = defaultDest;
    }
    breakTarget = next;
    int n = boundSwitchStatement.CaseStatements().size();
    llvm::SwitchInst* switchInst = builder.CreateSwitch(condition, defaultDest, n);
    std::unordered_map<IntegralValue, llvm::BasicBlock*, IntegralValueHash>* prevCaseMap = currentCaseMap;
    std::unordered_map<IntegralValue, llvm::BasicBlock*, IntegralValueHash> caseMap;
    currentCaseMap = &caseMap;
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<BoundCaseStatement>& caseS = boundSwitchStatement.CaseStatements()[i];
        llvm::BasicBlock* caseDest = llvm::BasicBlock::Create(context, "case" + std::to_string(i), function);
        for (const std::unique_ptr<Value>& caseValue : caseS->CaseValues())
        {
            IntegralValue integralCaseValue(caseValue.get());
            caseMap[integralCaseValue] = caseDest;
            switchInst->addCase(llvm::cast<llvm::ConstantInt>(caseValue->IrValue(*this)), caseDest);
        }
    }
    for (int i = 0; i < n; ++i)
    {
        const std::unique_ptr<BoundCaseStatement>& caseS = boundSwitchStatement.CaseStatements()[i];
        caseS->Accept(*this);
        if (basicBlockOpen)
        {
            builder.CreateBr(next);
            basicBlockOpen = false;
        }
    }
    if (boundSwitchStatement.DefaultStatement())
    {
        boundSwitchStatement.DefaultStatement()->Accept(*this);
        if (basicBlockOpen)
        {
            builder.CreateBr(next);
            basicBlockOpen = false;
        }
    }
    SetCurrentBasicBlock(next);
    basicBlockOpen = true;
    currentCaseMap = prevCaseMap;
    defaultDest = prevDefaultDest;
    breakTargetBlock = prevBreakTargetBlock;
    breakTarget = prevBreakTarget;
}

void BasicEmitter::Visit(BoundCaseStatement& boundCaseStatement)
{
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
            llvm::BasicBlock* caseDest = it->second;
            SetCurrentBasicBlock(caseDest);
            boundCaseStatement.CompoundStatement()->Accept(*this);
        }
        else
        {
            throw Exception(&symbolsModule, "case not found", boundCaseStatement.GetSpan());
        }
    }
    else
    {
        throw Exception(&symbolsModule, "no cases", boundCaseStatement.GetSpan());
    }
}

void BasicEmitter::Visit(BoundDefaultStatement& boundDefaultStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundDefaultStatement);
    if (defaultDest)
    {
        SetCurrentBasicBlock(defaultDest);
        boundDefaultStatement.CompoundStatement()->Accept(*this);
    }
    else
    {
        throw Exception(&symbolsModule, "no default destination", boundDefaultStatement.GetSpan());
    }
}

void BasicEmitter::Visit(BoundConstructionStatement& boundConstructionStatement)
{
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
                        std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(&symbolsModule, currentBlock->EndSpan(), classType->Destructor()));
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

void BasicEmitter::Visit(BoundAssignmentStatement& boundAssignmentStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundAssignmentStatement);
    boundAssignmentStatement.AssignmentCall()->Accept(*this);
}

void BasicEmitter::Visit(BoundExpressionStatement& boundExpressionStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundExpressionStatement);
    boundExpressionStatement.Expression()->Accept(*this);
    if (boundExpressionStatement.Expression()->HasValue())
    {
        stack.Pop();
    }
}

void BasicEmitter::Visit(BoundEmptyStatement& boundEmptyStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundEmptyStatement);
    llvm::Type* retType = llvm::Type::getVoidTy(context);
    std::vector<llvm::Type*> paramTypes;
    llvm::FunctionType* doNothingFunType = llvm::FunctionType::get(retType, paramTypes, false);
    llvm::Function* doNothingFun = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("llvm.donothing", doNothingFunType));
    ArgVector args;
    std::vector<llvm::OperandBundleDef> bundles;
    if (currentPad != nullptr)
    {
        std::vector<llvm::Value*> inputs;
        inputs.push_back(currentPad->value);
        bundles.push_back(llvm::OperandBundleDef("funclet", inputs));
    }
    if (currentPad == nullptr)
    {
        builder.CreateCall(doNothingFunType, doNothingFun, args);
    }
    else
    {
        llvm::CallInst* callInst = llvm::CallInst::Create(doNothingFun, args, bundles, "", CurrentBasicBlock());
        if (debugInfo)
        {
            callInst->setDebugLoc(GetDebugLocation(boundEmptyStatement.GetSpan()));
        }
    }
}

void BasicEmitter::Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement)
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
    llvm::Value* classPtrValue = stack.Pop();
    ArgVector indeces;
    indeces.push_back(builder.getInt32(0));
    indeces.push_back(builder.getInt32(vmtPtrIndex));
    llvm::Value* ptr = builder.CreateGEP(classPtrValue, indeces);
    llvm::Value* vmtPtr = builder.CreateBitCast(boundSetVmtPtrStatement.ClassType()->VmtObject(*this, false), builder.getInt8PtrTy());
    builder.CreateStore(vmtPtr, ptr);
}

void BasicEmitter::Visit(BoundThrowStatement& boundThrowStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundThrowStatement);
    boundThrowStatement.ThrowCallExpr()->Accept(*this);
}

void BasicEmitter::Visit(BoundParameter& boundParameter)
{
    boundParameter.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundLocalVariable& boundLocalVariable)
{
    boundLocalVariable.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundMemberVariable& boundMemberVariable)
{
    boundMemberVariable.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundConstant& boundConstant)
{
    boundConstant.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundEnumConstant& boundEnumConstant)
{
    boundEnumConstant.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundLiteral& boundLiteral)
{
    boundLiteral.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundTemporary& boundTemporary)
{
    boundTemporary.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundSizeOfExpression& boundSizeOfExpression)
{
    boundSizeOfExpression.Load(*this, OperationFlags::none);
}

void BasicEmitter::Visit(BoundAddressOfExpression& boundAddressOfExpression)
{
    boundAddressOfExpression.Load(*this, OperationFlags::none);
}

void BasicEmitter::Visit(BoundDereferenceExpression& boundDereferenceExpression)
{
    boundDereferenceExpression.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression)
{
    boundReferenceToPointerExpression.Load(*this, OperationFlags::none);
}

void BasicEmitter::Visit(BoundFunctionCall& boundFunctionCall)
{
    boundFunctionCall.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundDelegateCall& boundDelegateCall)
{
    boundDelegateCall.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundClassDelegateCall& boundClassDelegateCall)
{
    boundClassDelegateCall.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundConversion& boundConversion)
{
    boundConversion.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundConstructExpression& boundConstructExpression)
{
    boundConstructExpression.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression)
{
    boundConstructAndReturnTemporaryExpression.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult)
{
    boundClassOrClassDelegateConversionResult.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundIsExpression& boundIsExpression)
{
    boundIsExpression.Load(*this, OperationFlags::none);
    GenJumpingBoolCode();
}

void BasicEmitter::Visit(BoundAsExpression& boundAsExpression)
{
    boundAsExpression.Load(*this, OperationFlags::none);
}

void BasicEmitter::Visit(BoundTypeNameExpression& boundTypeNameExpression)
{
    boundTypeNameExpression.Load(*this, OperationFlags::none);
}

void BasicEmitter::Visit(BoundBitCast& boundBitCast)
{
    boundBitCast.Load(*this, OperationFlags::none);
}

void BasicEmitter::Visit(BoundFunctionPtr& boundFunctionPtr)
{
    boundFunctionPtr.Load(*this, OperationFlags::none);
}

void BasicEmitter::Visit(BoundDisjunction& boundDisjunction)
{
    if (genJumpingBoolCode)
    {
        Assert(trueBlock, "true block not set");
        Assert(falseBlock, "false block not set");
        llvm::BasicBlock* rightBlock = llvm::BasicBlock::Create(context, "right", function);
        llvm::BasicBlock* prevFalseBlock = falseBlock;
        falseBlock = rightBlock;
        boundDisjunction.Left()->Accept(*this);
        SetCurrentBasicBlock(rightBlock);
        falseBlock = prevFalseBlock;
        boundDisjunction.Right()->Accept(*this);
        boundDisjunction.DestroyTemporaries(*this);
    }
}

void BasicEmitter::Visit(BoundConjunction& boundConjunction)
{
    if (genJumpingBoolCode)
    {
        Assert(trueBlock, "true block not set");
        Assert(falseBlock, "false block not set");
        llvm::BasicBlock* rightBlock = llvm::BasicBlock::Create(context, "right", function);
        llvm::BasicBlock* prevTrueBlock = trueBlock;
        trueBlock = rightBlock;
        boundConjunction.Left()->Accept(*this);
        trueBlock = prevTrueBlock;
        SetCurrentBasicBlock(rightBlock);
        boundConjunction.Right()->Accept(*this);
        boundConjunction.DestroyTemporaries(*this);
    }
}

std::string BasicEmitter::GetSourceFilePath(int32_t fileIndex)
{
    return symbolsModule.GetFilePath(fileIndex);
}

llvm::Value* BasicEmitter::GetGlobalStringPtr(int stringId)
{
    auto it = utf8stringMap.find(stringId);
    if (it != utf8stringMap.cend())
    {
        return it->second;
    }
    else
    {
        llvm::Value* stringValue = builder.CreateGlobalStringPtr(compileUnit->GetUtf8String(stringId));
        utf8stringMap[stringId] = stringValue;
        return stringValue;
    }
}

llvm::Value* BasicEmitter::GetGlobalWStringConstant(int stringId)
{
    auto it = utf16stringMap.find(stringId);
    if (it != utf16stringMap.cend())
    {
        return it->second;
    }
    else
    {
        const std::u16string& str = compileUnit->GetUtf16String(stringId);
        uint64_t length = str.length();
        std::vector<llvm::Constant*> wcharConstants;
        for (char16_t c : str)
        {
            wcharConstants.push_back(builder.getInt16(static_cast<uint16_t>(c)));
        }
        wcharConstants.push_back(builder.getInt16(static_cast<uint32_t>(0)));
        llvm::Constant* stringObject = compileUnitModule->getOrInsertGlobal("wstring" + std::to_string(stringId), llvm::ArrayType::get(builder.getInt16Ty(), length + 1));
        llvm::GlobalVariable* stringGlobal = llvm::cast<llvm::GlobalVariable>(stringObject);
        stringGlobal->setLinkage(llvm::GlobalValue::PrivateLinkage);
        llvm::Constant* constant = llvm::ConstantArray::get(llvm::ArrayType::get(builder.getInt16Ty(), length + 1), wcharConstants);
        stringGlobal->setInitializer(constant);
        llvm::Value* stringValue = stringGlobal;
        utf16stringMap[stringId] = stringValue;
        return stringValue;
    }
}

llvm::Value* BasicEmitter::GetGlobalUStringConstant(int stringId)
{
    auto it = utf32stringMap.find(stringId);
    if (it != utf32stringMap.cend())
    {
        return it->second;
    }
    else
    {
        const std::u32string& str = compileUnit->GetUtf32String(stringId);
        uint64_t length = str.length();
        std::vector<llvm::Constant*> ucharConstants;
        for (char32_t c : str)
        {
            ucharConstants.push_back(builder.getInt32(static_cast<uint32_t>(c)));
        }
        ucharConstants.push_back(builder.getInt32(static_cast<uint32_t>(0)));
        llvm::Constant* stringObject = compileUnitModule->getOrInsertGlobal("ustring" + std::to_string(stringId), llvm::ArrayType::get(builder.getInt32Ty(), length + 1));
        llvm::GlobalVariable* stringGlobal = llvm::cast<llvm::GlobalVariable>(stringObject);
        stringGlobal->setLinkage(llvm::GlobalValue::PrivateLinkage);
        llvm::Constant* constant = llvm::ConstantArray::get(llvm::ArrayType::get(builder.getInt32Ty(), length + 1), ucharConstants);
        stringGlobal->setInitializer(constant);
        llvm::Value* stringValue = stringGlobal;
        utf32stringMap[stringId] = stringValue;
        return stringValue;
    }
}

llvm::Value* BasicEmitter::GetGlobalUuidConstant(int uuidId)
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
        std::vector<llvm::Constant*> byteConstants;
        for (boost::uuids::uuid::value_type x : uuid)
        {
            byteConstants.push_back(builder.getInt8(static_cast<int8_t>(x)));
        }
        llvm::Constant* uuidObject = compileUnitModule->getOrInsertGlobal("uuid" + std::to_string(uuidId), llvm::ArrayType::get(builder.getInt8Ty(), length));
        llvm::GlobalVariable* uuidGlobal = llvm::cast<llvm::GlobalVariable>(uuidObject);
        uuidGlobal->setLinkage(llvm::GlobalValue::PrivateLinkage);
        llvm::Constant* constant = llvm::ConstantArray::get(llvm::ArrayType::get(builder.getInt8Ty(), length), byteConstants);
        uuidGlobal->setInitializer(constant);
        llvm::Value* uuidValue = uuidGlobal;
        uuidMap[uuidId] = uuidValue;
        return uuidValue;
    }
}

void BasicEmitter::CreateExitFunctionCall()
{
    if (currentFunction->GetFunctionSymbol()->DontThrow()) return;
    std::vector<llvm::Type*> exitFunctionParamTypes;
    llvm::FunctionType* exitFunctionType = llvm::FunctionType::get(builder.getVoidTy(), exitFunctionParamTypes, false);
    llvm::Function* exitFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("RtExitFunction", exitFunctionType));
    ArgVector exitFunctionArgs;
    if (currentPad == nullptr)
    {
        builder.CreateCall(exitFunction, exitFunctionArgs);
    }
    else
    {
        std::vector<llvm::OperandBundleDef> bundles;
        std::vector<llvm::Value*> inputs;
        inputs.push_back(currentPad->value);
        bundles.push_back(llvm::OperandBundleDef("funclet", inputs));
        llvm::CallInst* callInst = llvm::CallInst::Create(exitFunction, exitFunctionArgs, bundles, "", CurrentBasicBlock());
        if (debugInfo)
        {
            callInst->setDebugLoc(GetCurrentDebugLocation());
        }
    }
}

void BasicEmitter::SetLineNumber(int32_t lineNumber)
{
    if (currentFunction->GetFunctionSymbol()->DontThrow()) return;
    if (prevLineNumber == lineNumber) return;
    if (debugInfo)
    {
        SetCurrentDebugLocation(Span());
    }
    prevLineNumber = lineNumber;
    std::vector<llvm::Type*> setLineNumberFunctionParamTypes;
    setLineNumberFunctionParamTypes.push_back(builder.getInt32Ty());
    llvm::FunctionType* setLineNumberFunctionType = llvm::FunctionType::get(builder.getVoidTy(), setLineNumberFunctionParamTypes, false);
    llvm::Function* setLineNumberFunction = llvm::cast<llvm::Function>(compileUnitModule->getOrInsertFunction("RtSetLineNumber", setLineNumberFunctionType));
    ArgVector setLineNumberFunctionArgs;
    setLineNumberFunctionArgs.push_back(builder.getInt32(lineNumber));
    if (currentPad == nullptr)
    {
        builder.CreateCall(setLineNumberFunction, setLineNumberFunctionArgs);
    }
    else
    {
        std::vector<llvm::OperandBundleDef> bundles;
        std::vector<llvm::Value*> inputs;
        inputs.push_back(currentPad->value);
        bundles.push_back(llvm::OperandBundleDef("funclet", inputs));
        llvm::CallInst* callInst = llvm::CallInst::Create(setLineNumberFunction, setLineNumberFunctionArgs, bundles, "", CurrentBasicBlock());
        if (debugInfo)
        {
            callInst->setDebugLoc(GetCurrentDebugLocation());
        }
    }
}

llvm::DIType* BasicEmitter::CreateClassDIType(void* classPtr)
{
    ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(classPtr);
    return cls->CreateDIType(*this);
}

} } // namespace cmajor::emitter
