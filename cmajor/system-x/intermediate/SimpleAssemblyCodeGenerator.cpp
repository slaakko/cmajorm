// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/SimpleAssemblyCodeGenerator.hpp>
#include <system-x/intermediate/OptCodeGen.hpp>
#include <system-x/intermediate/LinearScanRegisterAllocator.hpp>
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/assembler/Constant.hpp>
#include <symbols/GlobalFlags.hpp>
#include <soulng/util/Uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;

SimpleAssemblyCodeGenerator::SimpleAssemblyCodeGenerator(Context* context_, cmsx::assembler::AssemblyFile* assemblyFile_) : 
    Visitor(context_), assemblyFile(assemblyFile_), emitSection(cmsx::assembler::AssemblySectionKind::code), 
    assemblyFunction(nullptr), assemblyStructure(nullptr), assemblyInst(nullptr), currentInst(nullptr), currentFunction(nullptr),
    registerAllocator(nullptr), leader(false), debugInfo(nullptr), lineNumber(0)
{
}

void SimpleAssemblyCodeGenerator::AddSourceFileInfo(CompileUnit& compileUnit)
{
    if (compileUnit.GetMetadataRef())
    {
        sourceFileNameMap[compileUnit.FilePath()] = compileUnit.GetMetadataRef()->NodeId();
    }
}

void SimpleAssemblyCodeGenerator::GenerateDebugInfo()
{
    for (Function* debugInfoFunc : debugInfoFunctions)
    {
        MetadataRef* metadataRef = debugInfoFunc->GetMetadataRef();
        if (metadataRef)
        {
            MetadataStruct* metadataStruct = metadataRef->GetMetadataStruct();
            if (metadataStruct)
            {
                MetadataItem* sourceFileItem = metadataStruct->GetItem("sourceFile");
                if (sourceFileItem && sourceFileItem->Kind() == MetadataItemKind::metadataRef)
                {
                    MetadataRef* mdRef = static_cast<MetadataRef*>(sourceFileItem);
                    MetadataStruct* mdStruct = mdRef->GetMetadataStruct();
                    if (mdStruct)
                    {
                        MetadataItem* sourceFileNameItem = mdStruct->GetItem("sourceFileName");
                        if (sourceFileNameItem && sourceFileNameItem->Kind() == MetadataItemKind::metadataString)
                        {
                            MetadataString* mdString = static_cast<MetadataString*>(sourceFileNameItem);
                            sourceFileNameMap[mdString->Value()] = mdStruct->Id();
                        }
                    }
                }
            }
        }
    }
    for (const std::pair<std::string, int64_t >& r : sourceFileNameMap)
    {
        EmitSourceFileNameDebugInfo(r.first, r.second, *this);
    }
    for (Function* debugInfoFunc : debugInfoFunctions)
    {
        int64_t frameSize = 0;
        auto it = frameSizeMap.find(debugInfoFunc);
        if (it != frameSizeMap.cend())
        {
            frameSize = it->second;
        }
        EmitFunctionDebugInfo(debugInfoFunc, frameSize, *this);
    }
}

void SimpleAssemblyCodeGenerator::WriteOutputFile()
{
    assemblyFile->Write();
}

void SimpleAssemblyCodeGenerator::Visit(GlobalVariable& globalVariable)
{
    if (globalVariable.Initializer())
    {
        if (globalVariable.Once())
        {
            assemblyFile->GetLinkSection()->GetOrCreateLinkOnceObject()->AddLinkOnceSymbol(cmsx::assembler::MakeGlobalSymbol(globalVariable.Name()));
        }
        else
        {
            assemblyFile->GetLinkSection()->GetOrCreateExternObject()->AddExternSymbol(cmsx::assembler::MakeGlobalSymbol(globalVariable.Name()));
        }
        emitSection = cmsx::assembler::AssemblySectionKind::data;
        cmsx::assembler::AssemblySection* dataSection = assemblyFile->GetDataSection();
        assemblyStructure = dataSection->CreateStructure(globalVariable.Name());
        cmsx::assembler::Instruction* octaInst = new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
        symbolName = globalVariable.Name() + "_data";
        octaInst->AddOperand(cmsx::assembler::MakeGlobalSymbol(symbolName));
        leader = false;
        Emit(octaInst);
        leader = true;
        assemblyInst = nullptr;
        globalVariable.Initializer()->Accept(*this);
    }
}

void SimpleAssemblyCodeGenerator::Emit(cmsx::assembler::Instruction* assemblyInstruction)
{
    if (emitSection == cmsx::assembler::AssemblySectionKind::code)
    {
        if (leader)
        {
            leader = false;
            assemblyInstruction->SetLabel(cmsx::assembler::MakeLocalSymbol(currentInst->Parent()->Id()));
        }
        assemblyFunction->AddInstruction(assemblyInstruction);
    }
    else if (emitSection == cmsx::assembler::AssemblySectionKind::data)
    {
        if (leader)
        {
            leader = false;
            assemblyInstruction->SetLabel(cmsx::assembler::MakeGlobalSymbol(symbolName));
        }
        assemblyStructure->AddInstruction(assemblyInstruction);
    }
    assemblyInst = assemblyInstruction;
}

void SimpleAssemblyCodeGenerator::EmitOcta(uint64_t value) 
{
    if (!assemblyInst)
    {
        cmsx::assembler::Instruction* octaInst = new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
        Emit(octaInst);
    }
    assemblyInst->AddOperand(cmsx::assembler::MakeConstantExpr(value));
}

void SimpleAssemblyCodeGenerator::EmitTetra(uint32_t value)
{
    if (!assemblyInst)
    {
        cmsx::assembler::Instruction* tetraInst = new cmsx::assembler::Instruction(cmsx::assembler::TETRA);
        Emit(tetraInst);
    }
    assemblyInst->AddOperand(cmsx::assembler::MakeConstantExpr(value));
}

void SimpleAssemblyCodeGenerator::EmitWyde(uint16_t value)
{
    if (!assemblyInst)
    {
        cmsx::assembler::Instruction* wydeInst = new cmsx::assembler::Instruction(cmsx::assembler::WYDE);
        Emit(wydeInst);
    }
    assemblyInst->AddOperand(cmsx::assembler::MakeConstantExpr(value));
}

void SimpleAssemblyCodeGenerator::EmitByte(uint8_t value)
{
    if (!assemblyInst)
    {
        cmsx::assembler::Instruction* byteInst = new cmsx::assembler::Instruction(cmsx::assembler::BYTE);
        Emit(byteInst);
    }
    assemblyInst->AddOperand(cmsx::assembler::MakeConstantExpr(value));
}

void SimpleAssemblyCodeGenerator::EmitSymbol(const std::string& name)
{
    if (!assemblyInst)
    {
        cmsx::assembler::Instruction* octaInst = new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
        Emit(octaInst);
    }
    assemblyInst->AddOperand(cmsx::assembler::MakeGlobalSymbol(name));
}

int SimpleAssemblyCodeGenerator::ExitLabelId() const
{
    return currentInst->Parent()->Parent()->LastBasicBlock()->Id() + 1;
}

void SimpleAssemblyCodeGenerator::EmitClsId(const std::string& typeId)
{
    if (!assemblyInst)
    {
        cmsx::assembler::Instruction* octaInst = new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
        Emit(octaInst);
    }
    assemblyInst->AddOperand(new cmsx::assembler::ClsIdConstant(SourcePos(), typeId));
}

void SimpleAssemblyCodeGenerator::EmitDebugInfoInst(cmsx::assembler::Instruction* assemblyInstruction)
{
    if (!debugInfo)
    {
        debugInfo = assemblyFile->GetDebugSection()->CreateDebugInfo();
    }
    debugInfo->AddInstruction(assemblyInstruction);
}

void SimpleAssemblyCodeGenerator::SetCurrentLineNumber(uint32_t lineNumber_)
{
    if (lineNumber == 0)
    {
        lineNumber = lineNumber_;
    }
    else if (lineNumber != lineNumber_)
    {
        EmitLineNumberInfo(lineNumber, *this);
        lineNumber = lineNumber_;
    }
}

void SimpleAssemblyCodeGenerator::BeginTry(uint32_t tryBlockId, uint32_t parentTryBlockId)
{
    EmitBeginTry(tryBlockId, parentTryBlockId, *this);
}

void SimpleAssemblyCodeGenerator::EndTry(uint32_t tryBlockId)
{
    EmitEndTry(tryBlockId, *this);
}

void SimpleAssemblyCodeGenerator::Catch(uint32_t catchBlockId, uint32_t tryBlockId, const std::string& caughtTypeIdStr)
{
    boost::uuids::uuid caughtTypeId = boost::lexical_cast<boost::uuids::uuid>(caughtTypeIdStr);
    uint64_t caughtTypeId1;
    uint64_t caughtTypeId2;
    UuidToInts(caughtTypeId, caughtTypeId1, caughtTypeId2);
    EmitCatch(catchBlockId, tryBlockId, caughtTypeId1, caughtTypeId2, *this);
}

void SimpleAssemblyCodeGenerator::BeginCleanup(uint32_t cleanupBlockId, uint32_t tryBlockId)
{
    EmitBeginCleanup(cleanupBlockId, tryBlockId, *this);
}

void SimpleAssemblyCodeGenerator::EndCleanup(uint32_t cleanupBlockId)
{
    EmitEndCleanup(cleanupBlockId, *this);
}

void SimpleAssemblyCodeGenerator::Error(const std::string& message)
{
    cmsx::intermediate::Error(message, currentInst->GetSourcePos(), GetContext());
}

const SourcePos& SimpleAssemblyCodeGenerator::GetSourcePos() const
{ 
    return currentInst->GetSourcePos(); 
}

void SimpleAssemblyCodeGenerator::Visit(Function& function)
{
    if (!function.IsDefined()) return;
    currentFunction = &function;
    lineNumber = 0;
    if (function.GetFlag(FunctionFlags::once))
    {
        assemblyFile->GetLinkSection()->GetOrCreateLinkOnceObject()->AddLinkOnceSymbol(cmsx::assembler::MakeGlobalSymbol(function.Name()));
    }
    else
    {
        assemblyFile->GetLinkSection()->GetOrCreateExternObject()->AddExternSymbol(cmsx::assembler::MakeGlobalSymbol(function.Name()));
    }
    emitSection = cmsx::assembler::AssemblySectionKind::code;
    Ctx()->ResetRegisterPool();
    std::unique_ptr<RegisterAllocator> linearScanRregisterAllocator = LinearScanRegisterAllocation(function, Ctx());
    registerAllocator = linearScanRregisterAllocator.get();
    assemblyFunction = assemblyFile->CreateFunction(function.Name());
    function.VisitBasicBlocks(*this);
    if (lineNumber != 0)
    {
        EmitLineNumberInfo(lineNumber, *this);
    }
    assemblyFunction->SetActiveFunctionPart(cmsx::assembler::FunctionPart::prologue);
    EmitPrologue(*this);
    assemblyFunction->SetActiveFunctionPart(cmsx::assembler::FunctionPart::epilogue);
    EmitEpilogue(*this);
    if (function.GetMetadataRef())
    {
        debugInfoFunctions.push_back(&function);
        frameSizeMap[&function] = registerAllocator->GetFrame().Size();
    }
}

void SimpleAssemblyCodeGenerator::Visit(BasicBlock& basicBlock)
{
    Instruction* inst = basicBlock.FirstInstruction();
    leader = true;
    while (inst)
    {
        currentInst = inst;
        ProcessInstructionMetadata(inst, *this);
        RegisterAllocationAction action = registerAllocator->Run(inst);
        if (action == RegisterAllocationAction::spill)
        {
            for (const SpillData& spillData : registerAllocator->GetSpillData())
            {
                EmitStore(spillData.spillToFrameLocation, spillData.registerToSpill, *this);
            }
        }
        inst->Accept(*this);
        inst = inst->Next();
    }
}

void SimpleAssemblyCodeGenerator::Visit(StoreInstruction& inst)
{
    EmitStore(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ArgInstruction& inst)
{
    EmitArg(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(JmpInstruction& inst)
{
    EmitJmp(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(BranchInstruction& inst)
{
    EmitBranch(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ProcedureCallInstruction& inst)
{
    EmitProcedureCall(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(RetInstruction& inst)
{
    EmitRet(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(SwitchInstruction& inst)
{
    if (cmajor::symbols::GetOptimizationLevel() > 0)
    {
        EmitOptSwitch(inst, *this);
    }
    else
    {
        EmitSwitch(inst, *this);
    }
}

void SimpleAssemblyCodeGenerator::Visit(NotInstruction& inst)
{
    EmitNot(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(NegInstruction& inst)
{
    EmitNeg(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(SignExtendInstruction& inst)
{
    EmitSignExtension(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ZeroExtendInstruction& inst)
{
    EmitZeroExtension(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(TruncateInstruction& inst)
{
    EmitTruncate(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(BitcastInstruction& inst)
{
    EmitBitcast(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(IntToFloatInstruction& inst)
{
    EmitIntToFloat(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(FloatToIntInstruction& inst)
{
    EmitFloatToInt(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(IntToPtrInstruction& inst)
{
    EmitIntToPtr(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(PtrToIntInstruction& inst)
{
    EmitPtrToInt(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(AddInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(SubInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(MulInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(DivInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ModInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(AndInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(OrInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(XorInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ShlInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ShrInstruction& inst)
{
    EmitBinOpInst(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(EqualInstruction& inst)
{
    EmitEqual(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(LessInstruction& inst)
{
    EmitLess(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ParamInstruction& inst)
{
    EmitParam(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(LoadInstruction& inst)
{
    EmitLoad(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ElemAddrInstruction& inst)
{
    EmitElemAddr(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(PtrOffsetInstruction& inst)
{
    EmitPtrOffset(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(PtrDiffInstruction& inst)
{
    EmitPtrDiff(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(FunctionCallInstruction& inst)
{
    EmitFunctionCall(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(TrapInstruction& inst)
{
    EmitTrap(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(PhiInstruction& inst) 
{
    Error("simple assembly code generator does not support phi instructions");
}

void SimpleAssemblyCodeGenerator::Visit(NoOperationInstruction& inst)
{
    EmitNop(inst, *this);
}

void SimpleAssemblyCodeGenerator::Visit(BoolValue& value)
{
    EmitBool(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(SByteValue& value)
{
    EmitSByte(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ByteValue& value)
{
    cmsx::intermediate::EmitByte(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ShortValue& value)
{
    EmitShort(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(UShortValue& value)
{
    EmitUShort(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(IntValue& value)
{
    EmitInt(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(UIntValue& value)
{
    EmitUInt(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(LongValue& value)
{
    EmitLong(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ULongValue& value)
{
    EmitULong(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(FloatValue& value)
{
    EmitFloat(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(DoubleValue& value)
{
    EmitDouble(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(NullValue& value)
{
    EmitNull(*this);
}

void SimpleAssemblyCodeGenerator::Visit(AddressValue& value)
{
    EmitAddress(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(ArrayValue& value)
{
    if (value.GetType()->IsArrayType())
    {
        if (!assemblyInst)
        {
            ArrayType* arrayType = static_cast<ArrayType*>(value.GetType());
            Type* elementType = arrayType->ElementType();
            assemblyInst = elementType->MakeAssemblyInst(GetContext());
            Emit(assemblyInst);
        }
        for (ConstantValue* elementValue : value.Elements())
        {
            elementValue->Accept(*this);
        }
    }
    else
    {
        Error("error generating array value: array type expected");
    }
}

void SimpleAssemblyCodeGenerator::Visit(StructureValue& value)
{
    for (ConstantValue* fieldValue : value.FieldValues())
    {
        assemblyInst = nullptr;
        fieldValue->Accept(*this);
    }
}

void SimpleAssemblyCodeGenerator::Visit(StringValue& value)
{
    EmitString(value, *this);
}

void SimpleAssemblyCodeGenerator::Visit(StringArrayValue& value)
{
    for (ConstantValue* elementValue : value.Strings())
    {
        assemblyInst = nullptr;
        elementValue->Accept(*this);
    }
}

void SimpleAssemblyCodeGenerator::Visit(ConversionValue& value)
{
    if (!assemblyInst)
    {
        assemblyInst = value.GetType()->MakeAssemblyInst(GetContext());
        Emit(assemblyInst);
    }
    value.From()->Accept(*this);
}

void SimpleAssemblyCodeGenerator::Visit(ClsIdValue& value)
{
    cmsx::intermediate::EmitClsId(value.TypeId(), *this);
}

void SimpleAssemblyCodeGenerator::Visit(SymbolValue& value)
{
    cmsx::intermediate::EmitSymbol(value, *this);
}

} // cmsx::intermediate
