// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/SimpleAssemblyCodeGenerator.hpp>
#include <system-x/intermediate/LinearScanRegisterAllocator.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/assembler/Constant.hpp>

namespace cmsx::intermediate {

SimpleAssemblyCodeGenerator::SimpleAssemblyCodeGenerator(Context* context_, cmsx::assembler::AssemblyFile* assemblyFile_) : 
    Visitor(context_), assemblyFile(assemblyFile_), emitSection(cmsx::assembler::AssemblySectionKind::code), 
    assemblyFunction(nullptr), assemblyStructure(nullptr), assemblyInst(nullptr), currentInst(nullptr), registerAllocator(nullptr), leader(false)
{
}

void SimpleAssemblyCodeGenerator::WriteOutputFile()
{
    assemblyFile->Write();
}

void SimpleAssemblyCodeGenerator::Visit(GlobalVariable& globalVariable)
{
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
    if (globalVariable.Initializer())
    {
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
    if (function.GetFlag(FunctionFlags::once))
    {
        assemblyFile->GetLinkSection()->GetOrCreateLinkOnceObject()->AddLinkOnceSymbol(cmsx::assembler::MakeGlobalSymbol(function.Name()));
    }
    else
    {
        assemblyFile->GetLinkSection()->GetOrCreateExternObject()->AddExternSymbol(cmsx::assembler::MakeGlobalSymbol(function.Name()));
    }
    emitSection = cmsx::assembler::AssemblySectionKind::code;
    RegisterPool::Init();
    std::unique_ptr<RegisterAllocator> linearScanRregisterAllocator = LinearScanRegisterAllocation(function);
    registerAllocator = linearScanRregisterAllocator.get();
    assemblyFunction = assemblyFile->CreateFunction(function.Name());
    function.VisitBasicBlocks(*this);
    assemblyFunction->SetActiveFunctionPart(cmsx::assembler::FunctionPart::prologue);
    EmitPrologue(*this);
    assemblyFunction->SetActiveFunctionPart(cmsx::assembler::FunctionPart::epilogue);
    EmitEpilogue(*this);
}

void SimpleAssemblyCodeGenerator::Visit(BasicBlock& basicBlock)
{
    Instruction* inst = basicBlock.FirstInstruction();
    leader = true;
    while (inst)
    {
        currentInst = inst;
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
    EmitSwitch(inst, *this);
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
    assemblyInst = nullptr;
    for (ConstantValue* elementValue : value.Elements())
    {
        elementValue->Accept(*this);
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

}

void SimpleAssemblyCodeGenerator::Visit(ConversionValue& value)
{

}

void SimpleAssemblyCodeGenerator::Visit(ClsIdValue& value)
{

}

void SimpleAssemblyCodeGenerator::Visit(SymbolValue& value)
{

}

} // cmsx::intermediate
