// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegenllvm/LlvmCodeGenerator.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundNamespace.hpp>
#include <cmajor/binder/BoundEnum.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Util.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace codegenllvm {

using namespace cmajor::symbols;
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

LlvmCodeGenerator::LlvmCodeGenerator(cmajor::ir::EmittingContext& emittingContext_) :
    cmajor::codegenbase::CodeGenerator(emittingContext_), symbolTable(nullptr), symbolsModule(nullptr), emitter(GetEmitter()), emittingContext(&emittingContext_), debugInfo(false),
    compileUnit(nullptr), currentClass(nullptr), currentFunction(nullptr), trueBlock(nullptr), falseBlock(nullptr), breakTarget(nullptr), continueTarget(nullptr), handlerBlock(nullptr),
    cleanupBlock(nullptr), entryBasicBlock(nullptr), newCleanupNeeded(false), currentPad(nullptr), prevLineNumber(0), destructorCallGenerated(false), lastInstructionWasRet(false),
    basicBlockOpen(false), lastAlloca(nullptr), compoundLevel(0), function(nullptr), sequenceSecond(nullptr), currentBlock(nullptr), genJumpingBoolCode(false), breakTargetBlock(nullptr),
    continueTargetBlock(nullptr), defaultDest(nullptr), currentCaseMap(nullptr)
{
    emitter->SetEmittingDelegate(this);
}

void LlvmCodeGenerator::GenerateCode(void* boundCompileUnit)
{
    BoundCompileUnit* compileUnit = static_cast<BoundCompileUnit*>(boundCompileUnit);
    compileUnit->Accept(*this);
}

void LlvmCodeGenerator::Visit(BoundCompileUnit& boundCompileUnit)
{
    symbolTable = &boundCompileUnit.GetSymbolTable();
    symbolsModule = &boundCompileUnit.GetModule();
    NativeModule nativeModule(emitter, boundCompileUnit.GetCompileUnitNode()->FilePath());
    module = nativeModule.module;
    emitter->SetTargetTriple(emittingContext->TargetTriple());
    emitter->SetDataLayout(emittingContext->DataLayout());
    emitter->SetSourceFileName(boundCompileUnit.GetCompileUnitNode()->FilePath());
    emitter->ResetCurrentDebugLocation();
    debugInfo = false;
    if (GetGlobalFlag(GlobalFlags::generateDebugInfo) && boundCompileUnit.GetCompileUnitNode() && !boundCompileUnit.GetCompileUnitNode()->IsSynthesizedUnit())
    {
        emitter->StartDebugInfo(boundCompileUnit.GetCompileUnitNode()->FilePath(), GetCompilerVersion(), GetGlobalFlag(GlobalFlags::release), boundCompileUnit.GetCompileUnitNode());
        debugInfo = true;
    }
    compileUnit = &boundCompileUnit;
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
        BoundNode* boundNode = boundCompileUnit.BoundNodes()[i].get();
        boundNode->Accept(*this);
    }
    if (debugInfo)
    {
        emitter->FinalizeDebugInfo();
    }
    if (GetGlobalFlag(GlobalFlags::emitLlvm))
    {
        emitter->EmitIrText(boundCompileUnit.LLFilePath());
    }
    emitter->VerifyModule();
    if (GetGlobalFlag(GlobalFlags::release))
    {
        emitter->EmitIrFile(boundCompileUnit.BCFilePath());
        Optimize(GetOptimizationLevel(), boundCompileUnit.BCFilePath(), boundCompileUnit.OptBCFilePath());
        if (GetGlobalFlag(GlobalFlags::emitOptLlvm))
        {
            emitter->Disassemble(boundCompileUnit.OptBCFilePath(), boundCompileUnit.OptLLFilePath());
        }
        emitter->Compile(boundCompileUnit.OptBCFilePath(), boundCompileUnit.ObjectFilePath(), GetOptimizationLevel());
    }
    else
    {
        emitter->EmitObjectCodeFile(boundCompileUnit.ObjectFilePath());
    }
    if (debugInfo)
    {
        emitter->EndDebugInfo();
    }
}

void LlvmCodeGenerator::Visit(BoundNamespace& boundNamespace)
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
                void* ns = emitter->CreateDebugInfoForNamespace(emitter->CurrentScope(), ToUtf8(components[i]));
                emitter->PushScope(ns);
            }
        }
        else
        {
            emitter->PushScope(emitter->GetDebugInfoForFile(boundNamespace.GetSpan().fileIndex));
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
                emitter->PopScope();
            }
        }
        else
        {
            emitter->PopScope();
        }
    }
}

void LlvmCodeGenerator::Visit(BoundClass& boundClass)
{
    classStack.push(currentClass);
    currentClass = &boundClass;
    bool prevDebugInfo = debugInfo;
    void* prevDIBuilder = emitter->DIBuilder();
    if (!boundClass.ContainsSourceFunctions())
    {
        emitter->ResetCurrentDebugLocation();
        debugInfo = false;
        emitter->SetCurrentDIBuilder(nullptr);
    }
    if (debugInfo)
    {
        emitter->MapClassPtr(currentClass->GetClassTypeSymbol()->TypeId(), currentClass->GetClassTypeSymbol());
        void* diType = emitter->GetDITypeByTypeId(currentClass->GetClassTypeSymbol()->TypeId());
        if (diType)
        {
            emitter->PushScope(diType);
        }
        else
        {
            void* baseClassDIType = nullptr;
            if (currentClass->GetClassTypeSymbol()->BaseClass())
            {
                baseClassDIType = currentClass->GetClassTypeSymbol()->BaseClass()->GetDIType(*emitter);
            }
            void* vtableHolderClassDIType = nullptr;
            if (currentClass->GetClassTypeSymbol()->IsPolymorphic() && currentClass->GetClassTypeSymbol()->VmtPtrHolderClass())
            {
                vtableHolderClassDIType = currentClass->GetClassTypeSymbol()->VmtPtrHolderClass()->CreateDIForwardDeclaration(*emitter);
                emitter->MapFwdDeclaration(vtableHolderClassDIType, currentClass->GetClassTypeSymbol()->VmtPtrHolderClass()->TypeId());
            }
            void* classIrType = currentClass->GetClassTypeSymbol()->IrType(*emitter);
            Span classSpan = currentClass->GetClassTypeSymbol()->GetSpan();
            if (currentClass->GetClassTypeSymbol()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(currentClass->GetClassTypeSymbol());
                classSpan = specialization->GetClassTemplate()->GetSpan();
            }
            void* forwardDeclaration = emitter->CreateIrDIForwardDeclaration(classIrType, ToUtf8(currentClass->GetClassTypeSymbol()->Name()), ToUtf8(currentClass->GetClassTypeSymbol()->MangledName()),
                classSpan);
            emitter->SetDITypeByTypeId(currentClass->GetClassTypeSymbol()->TypeId(), forwardDeclaration);
            std::vector<void*> memberVariableElements;
            for (MemberVariableSymbol* memberVariable : currentClass->GetClassTypeSymbol()->MemberVariables())
            {
                int memberVariableLayoutIndex = memberVariable->LayoutIndex();
                uint64_t offsetInBits = emitter->GetOffsetInBits(classIrType, memberVariableLayoutIndex);
                memberVariableElements.push_back(memberVariable->GetDIMemberType(*emitter, offsetInBits));
            }
            void* clsDIType = emitter->CreateDITypeForClassType(classIrType, memberVariableElements, classSpan, ToUtf8(currentClass->GetClassTypeSymbol()->Name()), vtableHolderClassDIType,
                ToUtf8(currentClass->GetClassTypeSymbol()->MangledName()), baseClassDIType);
            emitter->MapFwdDeclaration(forwardDeclaration, currentClass->GetClassTypeSymbol()->TypeId());
            emitter->SetDITypeByTypeId(currentClass->GetClassTypeSymbol()->TypeId(), clsDIType);
            emitter->PushScope(clsDIType);
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
        emitter->PopScope();
    }
    currentClass = classStack.top();
    classStack.pop();
    debugInfo = prevDebugInfo;
    emitter->SetCurrentDIBuilder(prevDIBuilder);
}

void LlvmCodeGenerator::Visit(BoundEnumTypeDefinition& boundEnumTypeDefinition)
{
    if (debugInfo)
    {
        EnumTypeSymbol* enumTypeSymbol = boundEnumTypeDefinition.GetEnumTypeSymbol();
        uint64_t sizeInBits = enumTypeSymbol->SizeInBits(*emitter);
        uint32_t alignInBits = enumTypeSymbol->AlignmentInBits(*emitter);
        std::vector<void*> elements;
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
            elements.push_back(emitter->CreateDITypeForEnumConstant(ToUtf8(enumConstant->Name()), value));
        }
        void* enumTypeDI = emitter->CreateDITypeForEnumType(ToUtf8(enumTypeSymbol->Name()), ToUtf8(enumTypeSymbol->MangledName()), enumTypeSymbol->GetSpan(), elements,
            sizeInBits, alignInBits, enumTypeSymbol->UnderlyingType()->GetDIType(*emitter));
        emitter->SetDITypeByTypeId(enumTypeSymbol->TypeId(), enumTypeDI);
    }
}

void LlvmCodeGenerator::Visit(BoundFunction& boundFunction)
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
    void* functionType = functionSymbol->IrType(*emitter);
    function = emitter->GetOrInsertFunction(ToUtf8(functionSymbol->MangledName()), functionType);
    bool setInline = false;
    if (GetGlobalFlag(GlobalFlags::release) && functionSymbol->IsInline())
    {
        emitter->AddInlineFunctionAttribute(function);
        functionSymbol->SetLinkOnceOdrLinkage();
    }
    if (functionSymbol->HasLinkOnceOdrLinkage())
    {
        setInline = true;
        void* comdat = emitter->GetOrInsertAnyFunctionComdat(ToUtf8(functionSymbol->MangledName()), function);
    }
    if (functionSymbol->GetFlag(FunctionSymbolFlags::winapi))
    {
        emitter->SetFunctionCallConventionToStdCall(function);
    }
    emitter->SetFunctionLinkage(function, setInline);
    emitter->SetFunction(function);
    bool hasSource = functionSymbol->HasSource();
    bool prevDebugInfo = debugInfo;
    void* prevDIBuilder = emitter->DIBuilder();
    if (!hasSource)
    {
        emitter->ResetCurrentDebugLocation();
        debugInfo = false;
        emitter->SetCurrentDIBuilder(nullptr);
    }
    if (debugInfo)
    {
        emitter->SetInPrologue(true);
        emitter->SetCurrentDebugLocation(Span());
        std::vector<void*> elementTypes;
        if (functionSymbol->ReturnType())
        {
            elementTypes.push_back(functionSymbol->ReturnType()->GetDIType(*emitter)); // 0'th entry is return type
        }
        else
        {
            elementTypes.push_back(symbolTable->GetTypeByName(U"void")->GetDIType(*emitter));
        }
        for (ParameterSymbol* parameter : functionSymbol->Parameters())
        {
            TypeSymbol* parameterType = parameter->GetType();
            elementTypes.push_back(parameterType->GetDIType(*emitter));
        }
        void* subroutineType = emitter->CreateSubroutineType(elementTypes);
        void* subprogram = nullptr;
        if (functionSymbol->IsConstructorDestructorOrNonstaticMemberFunction())
        {
            unsigned virtuality = 0;
            if (functionSymbol->IsVirtualAbstractOrOverride())
            {
                if (functionSymbol->IsAbstract())
                {
                    virtuality = emitter->GetPureVirtualVirtuality();
                }
                else
                {
                    virtuality = emitter->GetVirtualVirtuality();
                }
            }
            void* vtableHolder = nullptr;
            Symbol* parent = functionSymbol->Parent();
            if (parent && parent->IsClassTypeSymbol())
            {
                ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(parent);
                if (cls->IsPolymorphic() && cls->VmtPtrHolderClass())
                {
                    vtableHolder = cls->VmtPtrHolderClass()->GetDIType(*emitter);
                }
            }
            unsigned flags = emitter->GetFunctionFlags(functionSymbol->IsStatic(), AccessFlag(*emitter, functionSymbol->Access()), functionSymbol->IsExplicit());
            unsigned vtableIndex = 0;
            if (functionSymbol->VmtIndex() != -1)
            {
                vtableIndex = functionSymbol->VmtIndex();
            }
            subprogram = emitter->CreateDIMethod(ToUtf8(functionSymbol->Name()), ToUtf8(functionSymbol->MangledName()), functionSymbol->GetSpan(), subroutineType, virtuality, vtableIndex,
                vtableHolder, flags);
        }
        else
        {
            unsigned flags = AccessFlag(*emitter, functionSymbol->Access());
            subprogram = emitter->CreateDIFunction(ToUtf8(functionSymbol->Name()), ToUtf8(functionSymbol->MangledName()), functionSymbol->GetSpan(), subroutineType, flags);
        }
        emitter->SetDISubprogram(function, subprogram);
        emitter->PushScope(subprogram);
    }
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
        if (debugInfo)
        {
            void* paramVar = emitter->CreateDIParameterVariable(ToUtf8(parameter->Name()), i + 1, parameter->GetSpan(), parameter->GetType()->GetDIType(*emitter), allocaInst);
        }
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
        if (debugInfo && localVariable->GetSpan().Valid())
        {
            void* localVar = emitter->CreateDIAutoVariable(ToUtf8(localVariable->Name()), localVariable->GetSpan(), localVariable->GetType()->GetDIType(*emitter), allocaInst);
        }
    }
    if (!functionSymbol->DontThrow())
    {
        int funId = compileUnit->Install(ToUtf8(functionSymbol->FullName()));
        int sfpId = compileUnit->Install(compileUnit->SourceFilePath());
        void* funValue = GetGlobalStringPtr(funId);
        void* sfpValue = GetGlobalStringPtr(sfpId);
        std::vector<void*> enterFunctionParamTypes;
        enterFunctionParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
        enterFunctionParamTypes.push_back(emitter->GetIrTypeForVoidPtrType());
        void* enterFunctionType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), enterFunctionParamTypes);
        void* enterFunction = emitter->GetOrInsertFunction("RtEnterFunction", enterFunctionType);
        std::vector<void*> enterFunctionArgs;
        enterFunctionArgs.push_back(funValue);
        enterFunctionArgs.push_back(sfpValue);
        emitter->CreateCall(enterFunction, enterFunctionArgs);
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
            if (debugInfo)
            {
                emitter->SetInPrologue(false);
                emitter->SetCurrentDebugLocation(boundFunction.Body()->GetSpan());
                emitter->CreateCall(callee, args);
                emitter->SetInPrologue(true);
                emitter->SetCurrentDebugLocation(Span());
            }
            else
            {
                emitter->CreateCall(callee, args);
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
            NativeValue paramValue(parameter->IrObject(*emitter));
            copyCtorArgs.push_back(&paramValue);
            NativeValue argumentValue(arg);
            copyCtorArgs.push_back(&argumentValue);
            if (debugInfo)
            {
                emitter->SetInPrologue(false);
                emitter->SetCurrentDebugLocation(boundFunction.Body()->GetSpan());
                copyConstructor->GenerateCall(*emitter, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
                emitter->SetInPrologue(true);
                emitter->SetCurrentDebugLocation(Span());
            }
            else
            {
                copyConstructor->GenerateCall(*emitter, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
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
            NativeValue paramValue(parameter->IrObject(*emitter));
            paramValue.SetType(interfaceType->AddPointer(Span()));
            copyCtorArgs.push_back(&paramValue);
            NativeValue argumentValue(arg);
            argumentValue.SetType(interfaceType->AddPointer(Span()));
            copyCtorArgs.push_back(&argumentValue);
            if (debugInfo)
            {
                emitter->SetInPrologue(false);
                emitter->SetCurrentDebugLocation(boundFunction.Body()->GetSpan());
                copyConstructor->GenerateCall(*emitter, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
                emitter->SetInPrologue(true);
                emitter->SetCurrentDebugLocation(Span());
            }
            else
            {
                copyConstructor->GenerateCall(*emitter, copyCtorArgs, OperationFlags::none, boundFunction.Body()->GetSpan());
            }
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
    if (debugInfo)
    {
        emitter->SetInPrologue(false);
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
    if (functionSymbol->HasTry() || !cleanups.empty())
    {
        void* personalityFunction = GetPersonalityFunction();
        emitter->SetPersonalityFunction(function, personalityFunction);
    }
    if (functionSymbol->DontThrow() && !functionSymbol->HasTry() && cleanups.empty())
    {
        emitter->AddNoUnwindAttribute(function);
    }
    else
    {
        emitter->AddUWTableAttribute(function);
    }
    GenerateCodeForCleanups();
    if (debugInfo)
    {
        emitter->PopScope();
    }
    debugInfo = prevDebugInfo;
    emitter->SetCurrentDIBuilder(prevDIBuilder);
}

void LlvmCodeGenerator::Visit(BoundSequenceStatement& boundSequenceStatement)
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

void LlvmCodeGenerator::Visit(BoundCompoundStatement& boundCompoundStatement)
{
    if (debugInfo)
    {
        emitter->SetCurrentDebugLocation(boundCompoundStatement.GetSpan());
    }
    if (debugInfo && compoundLevel > 0)
    {
        void* block = emitter->CreateLexicalBlock(boundCompoundStatement.GetSpan());
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
    SetLineNumber(boundCompoundStatement.GetSpan().line);
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
        emitter->PopScope();
    }
}

void LlvmCodeGenerator::Visit(BoundIfStatement& boundIfStatement)
{
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

void LlvmCodeGenerator::Visit(BoundWhileStatement& boundWhileStatement)
{
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

void LlvmCodeGenerator::Visit(BoundDoStatement& boundDoStatement)
{
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

void LlvmCodeGenerator::Visit(BoundForStatement& boundForStatement)
{
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

void LlvmCodeGenerator::Visit(BoundSwitchStatement& boundSwitchStatement)
{
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

void LlvmCodeGenerator::Visit(BoundCaseStatement& boundCaseStatement)
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
            void* caseDest = it->second;
            emitter->SetCurrentBasicBlock(caseDest);
            if (boundCaseStatement.CompoundStatement())
            {
                boundCaseStatement.CompoundStatement()->Accept(*this);
            }
        }
        else
        {
            throw Exception(symbolsModule, "case not found", boundCaseStatement.GetSpan());
        }
    }
    else
    {
        throw Exception(symbolsModule, "no cases", boundCaseStatement.GetSpan());
    }
}

void LlvmCodeGenerator::Visit(BoundDefaultStatement& boundDefaultStatement)
{
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
        throw Exception(symbolsModule, "no default destination", boundDefaultStatement.GetSpan());
    }
}

void LlvmCodeGenerator::Visit(BoundConstructionStatement& boundConstructionStatement)
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
                        std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(symbolsModule, currentBlock->EndSpan(), classType->Destructor()));
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

void LlvmCodeGenerator::Visit(BoundAssignmentStatement& boundAssignmentStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundAssignmentStatement);
    boundAssignmentStatement.AssignmentCall()->Accept(*this);
}

void LlvmCodeGenerator::Visit(BoundExpressionStatement& boundExpressionStatement)
{
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

void LlvmCodeGenerator::Visit(BoundEmptyStatement& boundEmptyStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundEmptyStatement);
    void* retType = emitter->GetIrTypeForVoid();
    std::vector<void*> paramTypes;
    void* doNothingFunType = emitter->GetIrTypeForFunction(retType, paramTypes);
    void* doNothingFun = emitter->GetOrInsertFunction("llvm.donothing", doNothingFunType);
    std::vector<void*> args;
    std::vector<void*> bundles;
    if (currentPad != nullptr)
    {
        bundles.push_back(currentPad->value);
    }
    if (currentPad == nullptr)
    {
        emitter->CreateCall(doNothingFun, args);
    }
    else
    {
        void* callInst = emitter->CreateCallInst(doNothingFun, args, bundles, boundEmptyStatement.GetSpan());
    }
}

void LlvmCodeGenerator::Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement)
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

void LlvmCodeGenerator::Visit(BoundThrowStatement& boundThrowStatement)
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
    SetTarget(&boundThrowStatement);
    boundThrowStatement.ThrowCallExpr()->Accept(*this);
}

void LlvmCodeGenerator::Visit(BoundParameter& boundParameter)
{
    boundParameter.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundLocalVariable& boundLocalVariable)
{
    boundLocalVariable.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundMemberVariable& boundMemberVariable)
{
    boundMemberVariable.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundConstant& boundConstant)
{
    boundConstant.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundEnumConstant& boundEnumConstant)
{
    boundEnumConstant.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundLiteral& boundLiteral)
{
    boundLiteral.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundTemporary& boundTemporary)
{
    boundTemporary.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundSizeOfExpression& boundSizeOfExpression)
{
    boundSizeOfExpression.Load(*emitter, OperationFlags::none);
}

void LlvmCodeGenerator::Visit(BoundAddressOfExpression& boundAddressOfExpression)
{
    boundAddressOfExpression.Load(*emitter, OperationFlags::none);
}

void LlvmCodeGenerator::Visit(BoundDereferenceExpression& boundDereferenceExpression)
{
    boundDereferenceExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression)
{
    boundReferenceToPointerExpression.Load(*emitter, OperationFlags::none);
}

void LlvmCodeGenerator::Visit(BoundFunctionCall& boundFunctionCall)
{
    boundFunctionCall.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundDelegateCall& boundDelegateCall)
{
    boundDelegateCall.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundClassDelegateCall& boundClassDelegateCall)
{
    boundClassDelegateCall.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundConversion& boundConversion)
{
    boundConversion.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundConstructExpression& boundConstructExpression)
{
    boundConstructExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression)
{
    boundConstructAndReturnTemporaryExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult)
{
    boundClassOrClassDelegateConversionResult.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundIsExpression& boundIsExpression)
{
    boundIsExpression.Load(*emitter, OperationFlags::none);
    GenJumpingBoolCode();
}

void LlvmCodeGenerator::Visit(BoundAsExpression& boundAsExpression)
{
    boundAsExpression.Load(*emitter, OperationFlags::none);
}

void LlvmCodeGenerator::Visit(BoundTypeNameExpression& boundTypeNameExpression)
{
    boundTypeNameExpression.Load(*emitter, OperationFlags::none);
}

void LlvmCodeGenerator::Visit(BoundBitCast& boundBitCast)
{
    boundBitCast.Load(*emitter, OperationFlags::none);
}

void LlvmCodeGenerator::Visit(BoundFunctionPtr& boundFunctionPtr)
{
    boundFunctionPtr.Load(*emitter, OperationFlags::none);
}

void LlvmCodeGenerator::Visit(BoundDisjunction& boundDisjunction)
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

void LlvmCodeGenerator::Visit(BoundConjunction& boundConjunction)
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

void LlvmCodeGenerator::Visit(BoundGlobalVariable& boundGlobalVariable)
{
    GlobalVariableSymbol* globalVariableSymbol = boundGlobalVariable.GetGlobalVariableSymbol();
    globalVariableSymbol->CreateIrObject(*emitter);
}

void* LlvmCodeGenerator::GetGlobalStringPtr(int stringId)
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

void* LlvmCodeGenerator::GetGlobalWStringConstant(int stringId)
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
        std::vector<void*> wcharConstants;
        for (char16_t c : str)
        {
            wcharConstants.push_back(emitter->CreateIrValueForUShort(static_cast<uint16_t>(c)));
        }
        wcharConstants.push_back(emitter->CreateIrValueForUShort(static_cast<uint16_t>(0)));
        void* arrayType = emitter->GetIrTypeForArrayType(emitter->GetIrTypeForUShort(), length + 1);
        void* stringObject = emitter->GetOrInsertGlobal("wstring" + std::to_string(stringId), arrayType);
        void* stringGlobal = stringObject;
        emitter->SetPrivateLinkage(stringGlobal);
        void* constant = emitter->CreateIrValueForConstantArray(arrayType, wcharConstants, std::string());
        emitter->SetInitializer(stringGlobal, constant);
        void* stringValue = stringGlobal;
        utf16stringMap[stringId] = stringValue;
        return stringValue;
    }
}

void* LlvmCodeGenerator::GetGlobalUStringConstant(int stringId)
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
        std::vector<void*> ucharConstants;
        for (char32_t c : str)
        {
            ucharConstants.push_back(emitter->CreateIrValueForUInt(static_cast<uint32_t>(c)));
        }
        ucharConstants.push_back(emitter->CreateIrValueForUInt(static_cast<uint32_t>(0)));
        void* arrayType = emitter->GetIrTypeForArrayType(emitter->GetIrTypeForUInt(), length + 1);
        void* stringObject = emitter->GetOrInsertGlobal("ustring" + std::to_string(stringId), arrayType);
        void* stringGlobal = stringObject;
        emitter->SetPrivateLinkage(stringGlobal);
        void* constant = emitter->CreateIrValueForConstantArray(arrayType, ucharConstants, std::string());
        emitter->SetInitializer(stringGlobal, constant);
        void* stringValue = stringGlobal;
        utf32stringMap[stringId] = stringValue;
        return stringValue;
    }
}

void* LlvmCodeGenerator::GetGlobalUuidConstant(int uuidId)
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
        void* uuidObject = emitter->GetOrInsertGlobal("uuid" + std::to_string(uuidId), arrayType);
        void* uuidGlobal = uuidObject;
        emitter->SetPrivateLinkage(uuidGlobal);
        void* constant = emitter->CreateIrValueForConstantArray(arrayType, byteConstants, std::string());
        emitter->SetInitializer(uuidGlobal, constant);
        void* uuidValue = uuidGlobal;
        uuidMap[uuidId] = uuidValue;
        return uuidValue;
    }
}

void LlvmCodeGenerator::SetLineNumber(int32_t lineNumber)
{
    if (currentFunction->GetFunctionSymbol()->DontThrow()) return;
    if (prevLineNumber == lineNumber) return;
    if (debugInfo)
    {
        emitter->SetCurrentDebugLocation(Span());
    }
    prevLineNumber = lineNumber;
    std::vector<void*> setLineNumberFunctionParamTypes;
    setLineNumberFunctionParamTypes.push_back(emitter->GetIrTypeForInt());
    void* setLineNumberFunctionType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), setLineNumberFunctionParamTypes);
    void* setLineNumberFunction = emitter->GetOrInsertFunction("RtSetLineNumber", setLineNumberFunctionType);
    std::vector<void*> setLineNumberFunctionArgs;
    setLineNumberFunctionArgs.push_back(emitter->CreateIrValueForInt(lineNumber));
    if (currentPad == nullptr)
    {
        emitter->CreateCall(setLineNumberFunction, setLineNumberFunctionArgs);
    }
    else
    {
        std::vector<void*> bundles;
        bundles.push_back(currentPad->value);
        void* callInst = emitter->CreateCallInst(setLineNumberFunction, setLineNumberFunctionArgs, bundles, Span());
    }
}

void* LlvmCodeGenerator::HandlerBlock()
{
    return handlerBlock;
}

void* LlvmCodeGenerator::CleanupBlock()
{
    return cleanupBlock;
}

bool LlvmCodeGenerator::NewCleanupNeeded()
{
    return newCleanupNeeded;
}

std::string LlvmCodeGenerator::GetSourceFilePath(int32_t fileIndex)
{
    return symbolsModule->GetFilePath(fileIndex);
}

cmajor::ir::Pad* LlvmCodeGenerator::CurrentPad()
{
    return currentPad;
}

void* LlvmCodeGenerator::CreateClassDIType(void* classPtr)
{
    ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(classPtr);
    return cls->CreateDIType(*emitter);
}

void LlvmCodeGenerator::CreateExitFunctionCall()
{
    if (currentFunction->GetFunctionSymbol()->DontThrow()) return;
    std::vector<void*> exitFunctionParamTypes;
    void* exitFunctionType = emitter->GetIrTypeForFunction(emitter->GetIrTypeForVoid(), exitFunctionParamTypes);
    void* exitFunction = emitter->GetOrInsertFunction("RtExitFunction", exitFunctionType);
    std::vector<void*> exitFunctionArgs;
    if (currentPad == nullptr)
    {
        emitter->CreateCall(exitFunction, exitFunctionArgs);
    }
    else
    {
        std::vector<void*> bundles;
        bundles.push_back(currentPad->value);
        void* callInst = emitter->CreateCallInst(exitFunction, exitFunctionArgs, bundles, Span());
        if (debugInfo)
        {
            emitter->SetDebugLoc(callInst);
        }
    }
}

void LlvmCodeGenerator::SetTarget(BoundStatement* labeledStatement)
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
        throw Exception(symbolsModule, "target for labeled statement not found", labeledStatement->GetSpan());
    }
}

void LlvmCodeGenerator::ExitBlocks(BoundCompoundStatement* targetBlock)
{
    if (debugInfo)
    {
        if (currentBlock->EndSpan().Valid())
        {
            emitter->SetCurrentDebugLocation(currentBlock->EndSpan());
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

void LlvmCodeGenerator::GenJumpingBoolCode()
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

void LlvmCodeGenerator::ClearFlags()
{
    destructorCallGenerated = false;
    lastInstructionWasRet = false;
    basicBlockOpen = false;
}

int LlvmCodeGenerator::Install(const std::string& str)
{
    return compileUnit->Install(str);
}

int LlvmCodeGenerator::Install(const std::u16string& str)
{
    return compileUnit->Install(str);
}

int LlvmCodeGenerator::Install(const std::u32string& str)
{
    return compileUnit->Install(str);
}

void LlvmCodeGenerator::InsertAllocaIntoEntryBlock(void* allocaInst)
{
    emitter->InsertAllocaIntoBasicBlock(allocaInst, lastAlloca, entryBasicBlock);
    lastAlloca = allocaInst;
}

void LlvmCodeGenerator::Optimize(int optimizationLevel, const std::string& bcFilePath, const std::string& optBCFilePath)
{
    emitter->Optimize(bcFilePath, optBCFilePath, "-O" + std::to_string(optimizationLevel));
}

} } // namespace cmajor::codegen
