// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/CodeGen.hpp>
#include <system-x/intermediate/Code.hpp>
#include <system-x/intermediate/Data.hpp>
#include <system-x/intermediate/RegisterAllocator.hpp>
#include <system-x/machine/OpCode.hpp>
#include <system-x/machine/Registers.hpp>
#include <system-x/assembler/Constant.hpp>
#include <stdexcept>

namespace cmsx::intermediate {

CodeGenerator::~CodeGenerator()
{
}

cmsx::assembler::Node* MakeRegOperand(const Register& reg)
{
    switch (reg.kind)
    {
        case RegisterKind::local:
        {
            return cmsx::assembler::MakeLocalRegOperand(reg.number);
        }
        case RegisterKind::global:
        {
            return cmsx::assembler::MakeGlobalRegOperand(reg.number);
        }
        default:
        {
            throw std::runtime_error("invalid register");
        }
    }
}

int64_t GetIndex(Value* index, CodeGenerator& codeGen)
{
    if (index->IsIntegerValue())
    {
        return index->GetIntegerValue();
    }
    else
    {
        codeGen.Error("error getting index: invalid index value kind");
    }
    return 0;
}

int64_t GetOffset(Type* type, int64_t index, CodeGenerator& codeGen)
{
    StructureType* structureType = type->GetStructurePointeeType(codeGen.GetSourcePos(), codeGen.Ctx());
    return structureType->GetFieldOffset(index);
}

int64_t GetElementSize(Type* type, CodeGenerator& codeGen)
{
    ArrayType* arrayType = type->GetArrayPointeeType(codeGen.GetSourcePos(), codeGen.Ctx());
    return arrayType->ElementType()->Size();
}

int64_t GetPointeeSize(Type* type, CodeGenerator& codeGen)
{
    if (type->IsPointerType())
    {
        PointerType* pointerType = static_cast<PointerType*>(type);
        Type* pointeeType = pointerType->RemovePointer(codeGen.GetSourcePos(), codeGen.Ctx());
        return pointeeType->Size();
    }
    else
    {
        codeGen.Error("error getting pointee size: pointer type expected");
    }
    return -1;
}

FrameLocation GetFrameLocation(Value* value, CodeGenerator& codeGen)
{
    Instruction* inst = nullptr;
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        inst = regValue->Inst();
        if (!inst)
        {
            codeGen.Error("error getting frame location: instruction for reg value not set");
        }
    }
    else if (value->IsInstruction())
    {
        inst = static_cast<Instruction*>(value);
    }
    if (inst)
    {
        Locations locs = codeGen.RegAllocator()->GetLocations(inst);
        if ((locs & Locations::frame) != Locations::none)
        {
            FrameLocation frameLocation = codeGen.RegAllocator()->GetFrameLocation(inst);
            if (frameLocation.Valid())
            {
                return frameLocation;
            }
            else
            {
                codeGen.Error("error getting frame location: frame location not valid");
            }
        }
        else
        {
            codeGen.Error("error getting frame location: value not in frame");
        }
    }
    else
    {
        codeGen.Error("error getting frame location: invalid value kind");
    }
    return FrameLocation();
}

void EmitFrameLocationOperand(const FrameLocation& frameLocation, cmsx::assembler::Instruction* instruction, CodeGenerator& codeGen)
{
    instruction->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regFP)));
    if (frameLocation.IsWithinImmediateRange())
    {
        instruction->AddOperand(cmsx::assembler::MakeConstantExpr(8 * frameLocation.index));
    }
    else
    {
        cmsx::assembler::Instruction* setIxInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setIxInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regIX));
        setIxInst->AddOperand(cmsx::assembler::MakeConstantExpr(8 * frameLocation.index));
        codeGen.Emit(setIxInst);
        instruction->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regIX));
    }
}

void EmitArgLocationOperand(cmsx::assembler::Instruction* instruction, CodeGenerator& codeGen)
{
    instruction->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regFP)));
    cmsx::assembler::Instruction* setIxInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setIxInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regIX));
    cmsx::assembler::DecimalConstant* node = new cmsx::assembler::DecimalConstant(SourcePos(), 0);
    codeGen.RegAllocator()->GetFrame().CurrentCallFrame()->NextArgLocation(node);
    setIxInst->AddOperand(node);
    codeGen.Emit(setIxInst);
    instruction->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regIX));
}

void EmitPtrOperand(Value* value, cmsx::assembler::Instruction* instruction, CodeGenerator& codeGen)
{
    Instruction* inst = nullptr;
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        inst = regValue->Inst();
        if (!inst)
        {
            codeGen.Error("error emitting ptr operand: instruction for reg value not set");
        }
    }
    else if (value->IsInstruction())
    {
        inst = static_cast<Instruction*>(value);
    }
    else if (value->IsAddressValue())
    {
        AddressValue* addressValue = static_cast<AddressValue*>(value);
        GlobalVariable* globalVar = addressValue->Value();
        cmsx::assembler::Instruction* ldouInst = new cmsx::assembler::Instruction(cmsx::machine::LDOU);
        ldouInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
        ldouInst->AddOperand(cmsx::assembler::MakeGlobalSymbol(globalVar->Name()));
        codeGen.Emit(ldouInst);
        instruction->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
        return;
    }
    if (inst)
    {
        if (inst->IsLocalInstruction())
        {
            FrameLocation frameLocation = GetFrameLocation(value, codeGen);
            EmitFrameLocationOperand(frameLocation, instruction, codeGen);
        }
        else
        {
            cmsx::assembler::Node* operand = MakeRegOperand(value, GetGlobalRegister(cmsx::machine::regAX), codeGen);
            instruction->AddOperand(operand);
        }
    }
    else
    {
        codeGen.Error("error emitting ptr operand: invalid value kind");
    }
}

cmsx::assembler::Node* MakeRegOperand(Value* value, const Register& r, CodeGenerator& codeGen)
{
    if (value->Kind() == ValueKind::regValue)
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        Instruction* inst = regValue->Inst();
        if (inst)
        {
            Locations locs = codeGen.RegAllocator()->GetLocations(inst);
            if ((locs & Locations::reg) != Locations::none)
            {
                Register reg = codeGen.RegAllocator()->GetRegister(inst);
                if (reg.Valid())
                {
                    return MakeRegOperand(reg);
                }
                else
                {
                    codeGen.Error("error making reg operand: reg not valid");
                }
            }
            else if ((locs & Locations::frame) != Locations::none)
            {
                FrameLocation frameLocation = codeGen.RegAllocator()->GetFrameLocation(inst);
                if (frameLocation.Valid())
                {
                    EmitLoad(frameLocation, r, codeGen);
                }
                else
                {
                    codeGen.Error("error making reg operand: frame location not valid");
                }
            }
            else
            {
                codeGen.Error("error making reg operand: no locations for inst");
            }
        }
        else
        {
            codeGen.Error("error making reg operand: instruction for reg value not set");
        }
    }
    else
    {
        cmsx::assembler::Instruction* inst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        inst->AddOperand(MakeRegOperand(r));
        codeGen.Emit(inst);
        switch (value->Kind())
        {
            case ValueKind::boolValue:
            {
                BoolValue* v = static_cast<BoolValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::sbyteValue:
            {
                SByteValue* v = static_cast<SByteValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::byteValue:
            {
                ByteValue* v = static_cast<ByteValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::shortValue:
            {
                ShortValue* v = static_cast<ShortValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::ushortValue:
            {
                UShortValue* v = static_cast<UShortValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::intValue:
            {
                IntValue* v = static_cast<IntValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::uintValue:
            {
                UIntValue* v = static_cast<UIntValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::longValue:
            {
                LongValue* v = static_cast<LongValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::ulongValue:
            {
                ULongValue* v = static_cast<ULongValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::floatValue:
            {
                FloatValue* v = static_cast<FloatValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::doubleValue:
            {
                DoubleValue* v = static_cast<DoubleValue*>(value);
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(v->GetValue()));
                break;
            }
            case ValueKind::nullValue:
            {
                inst->AddOperand(cmsx::assembler::MakeConstantExpr(0));
                break;
            }
            default:
            {
                codeGen.Error("error making reg operand: not implemented for value kind " + value->KindStr());
            }
        }
    }
    return MakeRegOperand(r);
}

cmsx::assembler::Node* MakeCalleeOperand(Value* value, const Register& reg, CodeGenerator& codeGen)
{
    if (value->IsSymbolValue())
    {
        SymbolValue* symbolValue = static_cast<SymbolValue*>(value);
        return cmsx::assembler::MakeGlobalSymbol(symbolValue->Symbol());
    }
    else 
    {
        return MakeRegOperand(value, reg, codeGen);
    }
}

cmsx::assembler::Node* MakeTrapOperand(Value* value, CodeGenerator& codeGen)
{
    if (value->IsSymbolValue())
    {
        SymbolValue* symbolValue = static_cast<SymbolValue*>(value);
        return cmsx::assembler::MakeGlobalSymbol(symbolValue->Symbol());
    }
    else
    {
        switch (value->Kind())
        {
            case ValueKind::sbyteValue:
            {
                SByteValue* v = static_cast<SByteValue*>(value);
                return cmsx::assembler::MakeConstantExpr(v->GetValue());
            }
            case ValueKind::byteValue:
            {
                ByteValue* v = static_cast<ByteValue*>(value);
                return cmsx::assembler::MakeConstantExpr(v->GetValue());
            }
            default:
            {
                codeGen.Error("error making trap operand: not implemented for value kind " + value->KindStr());
            }
        }
    }
    return nullptr;
}

void EmitParam(ParamInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting param: reg not valid");
    }
    Locations locs = codeGen.RegAllocator()->GetLocations(&inst);
    if ((locs & Locations::frame) != Locations::none)
    {
        FrameLocation frameLocation = codeGen.RegAllocator()->GetFrameLocation(&inst);
        EmitLoad(frameLocation, reg, codeGen);
    }
    else
    {
        codeGen.Error("error emitting param: no frame location");
    }
}

void EmitLoad(const FrameLocation& frameLocation, const Register& reg, CodeGenerator& codeGen)
{
    if (!frameLocation.Valid())
    {
        codeGen.Error("error emitting load: frame location not valid");
    }
    cmsx::assembler::Instruction* instruction = new cmsx::assembler::Instruction(cmsx::machine::LDOU);
    instruction->AddOperand(MakeRegOperand(reg));
    instruction->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regFP)));
    if (frameLocation.IsWithinImmediateRange())
    {
        instruction->AddOperand(cmsx::assembler::MakeConstantExpr(8 * frameLocation.index));
    }
    else
    {
        cmsx::assembler::Instruction* setIxInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setIxInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regIX));
        setIxInst->AddOperand(cmsx::assembler::MakeConstantExpr(8 * frameLocation.index));
        codeGen.Emit(setIxInst);
        instruction->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regIX));
    }
    codeGen.Emit(instruction);
}

void EmitStore(const FrameLocation& frameLocation, const Register& reg, CodeGenerator& codeGen)
{
    if (!frameLocation.Valid())
    {
        codeGen.Error("error emitting store: frame location not valid");
    }
    cmsx::assembler::Instruction* instruction = new cmsx::assembler::Instruction(cmsx::machine::STOU);
    instruction->AddOperand(MakeRegOperand(reg));
    EmitFrameLocationOperand(frameLocation, instruction, codeGen);
    codeGen.Emit(instruction);
}

void EmitLoad(LoadInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting load: reg not valid");
    }
    Type* type = inst.Result()->GetType();
    int machineInst = -1;
    if (type->IsFundamentalType())
    {
        switch (type->Id())
        {
            case boolTypeId:
            case sbyteTypeId:
            {
                machineInst = cmsx::machine::LDB;
                break;
            }
            case byteTypeId:
            {
                machineInst = cmsx::machine::LDBU;
                break;
            }
            case shortTypeId:
            {
                machineInst = cmsx::machine::LDW;
                break;
            }
            case ushortTypeId:
            {
                machineInst = cmsx::machine::LDWU;
                break;
            }
            case intTypeId:
            {
                machineInst = cmsx::machine::LDT;
                break;
            }
            case uintTypeId:
            {
                machineInst = cmsx::machine::LDTU;
                break;
            }
            case longTypeId:
            {
                machineInst = cmsx::machine::LDO;
                break;
            }
            case ulongTypeId:
            {
                machineInst = cmsx::machine::LDOU;
                break;
            }
            case floatTypeId:
            {
                machineInst = cmsx::machine::LDSF;
                break;
            }
            case doubleTypeId:
            {
                machineInst = cmsx::machine::LDOU;
                break;
            }
        }
    }
    else if (type->IsPointerType())
    {
        machineInst = cmsx::machine::LDOU;
    }
    else
    {
        codeGen.Error("error emitting load: invalid type kind");
    }
    if (machineInst == -1)
    {
        codeGen.Error("error emitting load: invalid machine instruction");
    }
    cmsx::assembler::Instruction* instruction = new cmsx::assembler::Instruction(machineInst);
    instruction->AddOperand(MakeRegOperand(reg));
    EmitPtrOperand(inst.Ptr(), instruction, codeGen);
    codeGen.Emit(instruction);
}

void EmitStore(StoreInstruction& inst, CodeGenerator& codeGen)
{
    int machineInst = -1;
    Type* type = inst.GetValue()->GetType();
    if (type->IsFundamentalType())
    {
        switch (type->Id())
        {
            case boolTypeId:
            case sbyteTypeId: 
            {
                machineInst = cmsx::machine::STB;
                break;
            }
            case byteTypeId:
            {
                machineInst = cmsx::machine::STBU;
                break;
            }
            case shortTypeId:
            {
                machineInst = cmsx::machine::STW;
                break;
            }
            case ushortTypeId:
            {
                machineInst = cmsx::machine::STWU;
                break;
            }
            case intTypeId:
            {
                machineInst = cmsx::machine::STT;
                break;
            }
            case uintTypeId:
            {
                machineInst = cmsx::machine::STTU;
                break;
            }
            case longTypeId:
            {
                machineInst = cmsx::machine::STO;
                break;
            }
            case ulongTypeId:
            {
                machineInst = cmsx::machine::STOU;
                break;
            }
            case floatTypeId:
            {
                machineInst = cmsx::machine::STSF;
                break;
            }
            case doubleTypeId:
            {
                machineInst = cmsx::machine::STOU;
                break;
            }
        }
    }
    else if (type->IsPointerType())
    {
        machineInst = cmsx::machine::STOU;
    }
    else
    {
        codeGen.Error("error emitting store: invalid type kind");
    }
    if (machineInst == -1)
    {
        codeGen.Error("error emitting store: invalid machine instruction");
    }
    cmsx::assembler::Instruction* instruction = new cmsx::assembler::Instruction(machineInst);
    instruction->AddOperand(MakeRegOperand(inst.GetValue(), GetGlobalRegister(cmsx::machine::regAX), codeGen));
    EmitPtrOperand(inst.GetPtr(), instruction, codeGen);
    codeGen.Emit(instruction);
}

void EmitSignExtension(SignExtendInstruction& inst, CodeGenerator& codeGen)
{
    cmsx::assembler::Instruction* instruction = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting sign extension: reg not valid");
    }
    instruction->AddOperand(MakeRegOperand(reg));
    instruction->AddOperand(MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen));
    codeGen.Emit(instruction);
}

void EmitNot(NotInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting not: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    cmsx::assembler::Instruction* zszInst = new cmsx::assembler::Instruction(cmsx::machine::ZSZ);
    zszInst->AddOperand(MakeRegOperand(reg));
    zszInst->AddOperand(operandReg);
    zszInst->AddOperand(cmsx::assembler::MakeConstantExpr(1));
    codeGen.Emit(zszInst);
}

void EmitNeg(NegInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting neg: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    int machineInst = -1;
    switch (inst.Result()->GetType()->Id())
    {
        case sbyteTypeId: machineInst = cmsx::machine::NEG; break;
        case byteTypeId: machineInst = cmsx::machine::NEGU; break;
        case shortTypeId: machineInst = cmsx::machine::NEG; break;
        case ushortTypeId: machineInst = cmsx::machine::NEGU; break;
        case intTypeId: machineInst = cmsx::machine::NEG; break;
        case uintTypeId: machineInst = cmsx::machine::NEGU; break;
        case longTypeId: machineInst = cmsx::machine::NEG; break;
        case ulongTypeId: machineInst = cmsx::machine::NEGU; break;
        case doubleTypeId: machineInst = cmsx::machine::FSUB; break;
        default:
        {
            codeGen.Error("error emitting neg: invalid result type");
        }
    }
    cmsx::assembler::Instruction* negInst = new cmsx::assembler::Instruction(machineInst);
    negInst->AddOperand(MakeRegOperand(reg));
    if (machineInst == cmsx::machine::FSUB)
    {
        negInst->AddOperand(cmsx::assembler::MakeConstantExpr(0.0));
    }
    else
    {
        negInst->AddOperand(cmsx::assembler::MakeConstantExpr(0));
    }
    negInst->AddOperand(operandReg);
    codeGen.Emit(negInst);
}


void EmitZeroExtension(ZeroExtendInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting zero extension: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regBX)));
    cmsx::assembler::Node* maskOperand = nullptr;
    switch (inst.Result()->GetType()->Id())
    {
        case boolTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0x01), true);
            break;
        }
        case byteTypeId:
        case sbyteTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFF), true);
            break;
        }
        case ushortTypeId:
        case shortTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFFFF), true);
            break;
        }
        case uintTypeId:
        case intTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFFFFFFFF), true);
            break;
        }
        case ulongTypeId:
        case longTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFFFFFFFFFFFFFFFF), true);
            break;
        }
    }
    if (!maskOperand)
    {
        codeGen.Error("error emitting truncate: invalid result type");
    }
    setInst->AddOperand(maskOperand);
    codeGen.Emit(setInst);
    cmsx::assembler::Instruction* andInst = new cmsx::assembler::Instruction(cmsx::machine::AND);
    andInst->AddOperand(MakeRegOperand(reg));
    andInst->AddOperand(operandReg);
    andInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regBX)));
    codeGen.Emit(andInst);
}

void EmitSwitch(SwitchInstruction& inst, CodeGenerator& codeGen)
{
    for (const auto& caseTarget : inst.CaseTargets())
    {
        cmsx::assembler::Node* caseReg = MakeRegOperand(caseTarget.caseValue, GetGlobalRegister(cmsx::machine::regCX), codeGen);
        cmsx::assembler::Instruction* cmpInst = new cmsx::assembler::Instruction(cmsx::machine::CMP);
        cmpInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
        cmpInst->AddOperand(MakeRegOperand(inst.Cond(), GetGlobalRegister(cmsx::machine::regBX), codeGen));
        cmpInst->AddOperand(caseReg);
        codeGen.Emit(cmpInst);
        cmsx::assembler::Instruction* branchInst = new cmsx::assembler::Instruction(cmsx::machine::BZ);
        branchInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
        branchInst->AddOperand(cmsx::assembler::MakeLocalSymbol(caseTarget.targetBlock->Id()));
        codeGen.Emit(branchInst);
    }
    cmsx::assembler::Instruction* jmpInst = new cmsx::assembler::Instruction(cmsx::machine::JMP);
    jmpInst->AddOperand(cmsx::assembler::MakeLocalSymbol(inst.DefaultTargetBlock()->Id()));
    codeGen.Emit(jmpInst);
}

void EmitBinOpInst(BinaryInstruction& inst, CodeGenerator& codeGen)
{
    int signedMachineInst = -1;
    int unsignedMachineInst = -1;
    int floatingMachineInst = -1;
    int machineInst = -1;
    bool mod = false;
    switch (inst.GetOpCode())
    {
        case OpCode::add:
        {
            signedMachineInst = cmsx::machine::ADD;
            unsignedMachineInst = cmsx::machine::ADDU;
            floatingMachineInst = cmsx::machine::FADD;
            break;
        }
        case OpCode::sub:
        {
            signedMachineInst = cmsx::machine::SUB;
            unsignedMachineInst = cmsx::machine::SUBU;
            floatingMachineInst = cmsx::machine::FSUB;
            break;
        }
        case OpCode::mul:
        {
            signedMachineInst = cmsx::machine::MUL;
            unsignedMachineInst = cmsx::machine::MULU;
            floatingMachineInst = cmsx::machine::FMUL;
            break;
        }
        case OpCode::div_:
        {
            signedMachineInst = cmsx::machine::DIV;
            unsignedMachineInst = cmsx::machine::DIVU;
            floatingMachineInst = cmsx::machine::FDIV;
            break;
        }
        case OpCode::mod:
        {
            signedMachineInst = cmsx::machine::DIV;
            unsignedMachineInst = cmsx::machine::DIVU;
            floatingMachineInst = cmsx::machine::FREM;
            mod = true;
            break;
        }
        case OpCode::and_:
        {
            signedMachineInst = cmsx::machine::AND;
            unsignedMachineInst = cmsx::machine::AND;
            break;
        }
        case OpCode::or_:
        {
            signedMachineInst = cmsx::machine::OR;
            unsignedMachineInst = cmsx::machine::OR;
            break;
        }
        case OpCode::xor_:
        {
            signedMachineInst = cmsx::machine::XOR;
            unsignedMachineInst = cmsx::machine::XOR;
            break;
        }
        case OpCode::shl:
        {
            signedMachineInst = cmsx::machine::SL;
            unsignedMachineInst = cmsx::machine::SLU;
            break;
        }
        case OpCode::shr:
        {
            signedMachineInst = cmsx::machine::SR;
            unsignedMachineInst = cmsx::machine::SRU;
            break;
        }
    }
    Type* type = inst.Result()->GetType();
    if (type->IsFundamentalType())
    {
        switch (type->Id())
        {
            case sbyteTypeId:
            case shortTypeId:
            case intTypeId:
            case longTypeId:
            {
                machineInst = signedMachineInst;
                break;
            }
            case byteTypeId:
            case ushortTypeId:
            case uintTypeId:
            case ulongTypeId:
            {
                machineInst = unsignedMachineInst;
                break;
            }
            case doubleTypeId:
            {
                machineInst = floatingMachineInst;
                break;
            }
        }
    }
    if (machineInst == -1)
    {
        codeGen.Error("error emitting binary operator: invalid machine instruction");
    }
    cmsx::assembler::Instruction* instruction = new cmsx::assembler::Instruction(machineInst);
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting binary operator: reg not valid");
    }
    instruction->AddOperand(MakeRegOperand(reg));
    instruction->AddOperand(MakeRegOperand(inst.Left(), GetGlobalRegister(cmsx::machine::regAX), codeGen));
    instruction->AddOperand(MakeRegOperand(inst.Right(), GetGlobalRegister(cmsx::machine::regBX), codeGen));
    codeGen.Emit(instruction);
    if (mod)
    {
        cmsx::assembler::Instruction* remInst = new cmsx::assembler::Instruction(cmsx::machine::GET);
        remInst->AddOperand(MakeRegOperand(reg));
        remInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::rR)));
        codeGen.Emit(remInst);
    }
}

void EmitEqual(EqualInstruction& inst, CodeGenerator& codeGen)
{
    int machineInst = -1;
    Type* type = inst.Left()->GetType();
    if (type->IsFundamentalType())
    {
        switch (type->Id())
        {
            case sbyteTypeId:
            case shortTypeId:
            case intTypeId:
            case longTypeId:
            {
                machineInst = cmsx::machine::CMP;
                break;
            }
            case byteTypeId:
            case ushortTypeId:
            case uintTypeId:
            case ulongTypeId:
            {
                machineInst = cmsx::machine::CMPU;
                break;
            }
            case doubleTypeId:
            {
                machineInst = cmsx::machine::FCMP;
                break;
            }
        }
    }
    else if (type->IsPointerType())
    {
        machineInst = cmsx::machine::CMPU;
    }
    else
    {
        codeGen.Error("error emitting equal: invalid type kind");
    }
    if (machineInst == -1)
    {
        codeGen.Error("error emitting equal: invalid machine instruction");
    }
    cmsx::assembler::Instruction* cmpInst = new cmsx::assembler::Instruction(machineInst);
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting equal: reg not valid");
    }
    cmpInst->AddOperand(MakeRegOperand(reg));
    cmpInst->AddOperand(MakeRegOperand(inst.Left(), GetGlobalRegister(cmsx::machine::regAX), codeGen));
    cmpInst->AddOperand(MakeRegOperand(inst.Right(), GetGlobalRegister(cmsx::machine::regBX), codeGen));
    codeGen.Emit(cmpInst);
    cmsx::assembler::Instruction* zszInst = new cmsx::assembler::Instruction(cmsx::machine::ZSZ);
    zszInst->AddOperand(MakeRegOperand(reg));
    zszInst->AddOperand(MakeRegOperand(reg));
    zszInst->AddOperand(cmsx::assembler::MakeConstantExpr(1));
    codeGen.Emit(zszInst);
}

void EmitLess(LessInstruction& inst, CodeGenerator& codeGen)
{
    int machineInst = -1;
    Type* type = inst.Left()->GetType();
    if (type->IsFundamentalType())
    {
        switch (type->Id())
        {
            case sbyteTypeId:
            case shortTypeId:
            case intTypeId:
            case longTypeId:
            {
                machineInst = cmsx::machine::CMP;
                break;
            }
            case byteTypeId:
            case ushortTypeId:
            case uintTypeId:
            case ulongTypeId:
            {
                machineInst = cmsx::machine::CMPU;
                break;
            }
            case doubleTypeId:
            {
                machineInst = cmsx::machine::FCMP;
                break;
            }
        }
    }
    else if (type->IsPointerType())
    {
        machineInst = cmsx::machine::CMPU;
    }
    else
    {
        codeGen.Error("error emitting less: invalid type kind");
    }
    if (machineInst == -1)
    {
        codeGen.Error("error emitting less: invalid machine instruction");
    }
    cmsx::assembler::Instruction* cmpInst = new cmsx::assembler::Instruction(machineInst);
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting less: reg not valid");
    }
    cmpInst->AddOperand(MakeRegOperand(reg));
    cmpInst->AddOperand(MakeRegOperand(inst.Left(), GetGlobalRegister(cmsx::machine::regAX), codeGen));
    cmpInst->AddOperand(MakeRegOperand(inst.Right(), GetGlobalRegister(cmsx::machine::regBX), codeGen));
    codeGen.Emit(cmpInst);
    cmsx::assembler::Instruction* zsnInst = new cmsx::assembler::Instruction(cmsx::machine::ZSN);
    zsnInst->AddOperand(MakeRegOperand(reg));
    zsnInst->AddOperand(MakeRegOperand(reg));
    zsnInst->AddOperand(cmsx::assembler::MakeConstantExpr(1));
    codeGen.Emit(zsnInst);
}

void EmitJmp(JmpInstruction& inst, CodeGenerator& codeGen)
{
    cmsx::assembler::Instruction* jmpInst = new cmsx::assembler::Instruction(cmsx::machine::JMP);
    jmpInst->AddOperand(cmsx::assembler::MakeLocalSymbol(inst.TargetLabelId()));
    codeGen.Emit(jmpInst);
}

void EmitBranch(BranchInstruction& inst, CodeGenerator& codeGen)
{
    cmsx::assembler::Instruction* branchInst = new cmsx::assembler::Instruction(cmsx::machine::BNZ);
    branchInst->AddOperand(MakeRegOperand(inst.Cond(), GetGlobalRegister(cmsx::machine::regAX), codeGen));
    branchInst->AddOperand(cmsx::assembler::MakeLocalSymbol(inst.TrueTargetLabelId()));
    codeGen.Emit(branchInst);
    cmsx::assembler::Instruction* jmpInst = new cmsx::assembler::Instruction(cmsx::machine::JMP);
    jmpInst->AddOperand(cmsx::assembler::MakeLocalSymbol(inst.FalseTargetLabelId()));
    codeGen.Emit(jmpInst);
}

void EmitNop(NoOperationInstruction& inst, CodeGenerator& codeGen)
{
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
    codeGen.Emit(setInst);
}

void EmitElemAddr(ElemAddrInstruction& inst, CodeGenerator& codeGen)
{
    ElemAddrKind elemAddrKind = inst.GetElemAddrKind(codeGen.Ctx());
    if (elemAddrKind == ElemAddrKind::array)
    {
        cmsx::assembler::Node* indexReg = MakeRegOperand(inst.Index(), GetGlobalRegister(cmsx::machine::regBX), codeGen);
        int64_t indexFactor = GetElementSize(inst.Ptr()->GetType(), codeGen);
        bool indexTypeIsUnsignedType = inst.Index()->GetType()->IsUnsignedType();
        cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
        if (indexTypeIsUnsignedType)
        {
            setInst->AddOperand(cmsx::assembler::MakeConstantExpr(static_cast<uint64_t>(indexFactor)));
        }
        else
        {
            setInst->AddOperand(cmsx::assembler::MakeConstantExpr(indexFactor));
        }
        codeGen.Emit(setInst);
        int machineMulInst = cmsx::machine::MUL;
        if (indexTypeIsUnsignedType)
        {
            machineMulInst = cmsx::machine::MULU;
        }
        cmsx::assembler::Instruction* mulInst = new cmsx::assembler::Instruction(machineMulInst);
        mulInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regIX)));
        mulInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
        mulInst->AddOperand(indexReg);
        codeGen.Emit(mulInst);
    }
    else if (elemAddrKind == ElemAddrKind::structure)
    {
        int64_t index = GetIndex(inst.Index(), codeGen);
        int64_t offset = GetOffset(inst.Ptr()->GetType(), index, codeGen);
        cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regIX)));
        setInst->AddOperand(cmsx::assembler::MakeConstantExpr(offset));
        codeGen.Emit(setInst);
    }
    else
    {
        codeGen.Error("error emitting elemaddr: invalid elemaddr kind");
    }
    cmsx::assembler::Instruction* ldaInst = new cmsx::assembler::Instruction(cmsx::assembler::LDA);
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting elemaddr: reg not valid");
    }
    ldaInst->AddOperand(MakeRegOperand(reg));
    if (elemAddrKind == ElemAddrKind::array)
    {
        EmitPtrOperand(inst.Ptr(), ldaInst, codeGen);
    }
    else if (elemAddrKind == ElemAddrKind::structure)
    {
        cmsx::assembler::Node* operand = MakeRegOperand(inst.Ptr(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
        ldaInst->AddOperand(operand);
    }
    ldaInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regIX)));
    codeGen.Emit(ldaInst);
}

void EmitPtrOffset(PtrOffsetInstruction& inst, CodeGenerator& codeGen)
{
    cmsx::assembler::Node* offsetReg = MakeRegOperand(inst.Offset(), GetGlobalRegister(cmsx::machine::regBX), codeGen);
    int64_t offsetFactor = GetPointeeSize(inst.Ptr()->GetType(), codeGen);
    bool offsetTypeIsUnsignedType = inst.Ptr()->GetType()->RemovePointer(inst.GetSourcePos(), codeGen.Ctx())->IsUnsignedType();
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
    if (offsetTypeIsUnsignedType)
    {
        setInst->AddOperand(cmsx::assembler::MakeConstantExpr(static_cast<uint64_t>(offsetFactor)));
    }
    else
    {
        setInst->AddOperand(cmsx::assembler::MakeConstantExpr(offsetFactor));
    }
    codeGen.Emit(setInst);
    int machineMulInst = cmsx::machine::MUL;
    if (offsetTypeIsUnsignedType)
    {
        machineMulInst = cmsx::machine::MULU;
    }
    cmsx::assembler::Instruction* mulInst = new cmsx::assembler::Instruction(machineMulInst);
    mulInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regIX)));
    mulInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
    mulInst->AddOperand(offsetReg);
    codeGen.Emit(mulInst);
    cmsx::assembler::Instruction* ldaInst = new cmsx::assembler::Instruction(cmsx::assembler::LDA);
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting ptroffset: reg not valid");
    }
    ldaInst->AddOperand(MakeRegOperand(reg));
    cmsx::assembler::Node* operand = MakeRegOperand(inst.Ptr(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    ldaInst->AddOperand(operand);
    ldaInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regIX)));
    codeGen.Emit(ldaInst);
}

void EmitPtrDiff(PtrDiffInstruction& inst, CodeGenerator& codeGen)
{
    int64_t scaleFactor = GetPointeeSize(inst.LeftPtr()->GetType(), codeGen);
    cmsx::assembler::Instruction* diff = new cmsx::assembler::Instruction(cmsx::machine::SUB);
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting ptrdiff: reg not valid");
    }
    diff->AddOperand(MakeRegOperand(reg));
    cmsx::assembler::Node* leftOperand = MakeRegOperand(inst.LeftPtr(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    diff->AddOperand(leftOperand);
    cmsx::assembler::Node* rightOperand = MakeRegOperand(inst.RightPtr(), GetGlobalRegister(cmsx::machine::regBX), codeGen);
    diff->AddOperand(rightOperand);
    codeGen.Emit(diff);
    cmsx::assembler::Instruction* scaledDiff = new cmsx::assembler::Instruction(cmsx::machine::DIV);
    scaledDiff->AddOperand(MakeRegOperand(reg));
    scaledDiff->AddOperand(MakeRegOperand(reg));
    scaledDiff->AddOperand(cmsx::assembler::MakeConstantExpr(scaleFactor));
    codeGen.Emit(scaledDiff);
}

void EmitBitcast(BitcastInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting bitcast: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(reg));
    setInst->AddOperand(operandReg);
    codeGen.Emit(setInst);
}

void EmitPtrToInt(PtrToIntInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting ptrtoint: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(reg));
    setInst->AddOperand(operandReg);
    codeGen.Emit(setInst);
}

void EmitIntToPtr(IntToPtrInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting inttoptr: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(reg));
    setInst->AddOperand(operandReg);
    codeGen.Emit(setInst);
}

void EmitFloatToInt(FloatToIntInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting floattoint: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    int machineInst = -1;
    if (inst.Result()->GetType()->IsUnsignedType())
    {
        machineInst = cmsx::machine::FIXU;
    }
    else
    {
        machineInst = cmsx::machine::FIX;
    }
    cmsx::assembler::Instruction* fixInst = new cmsx::assembler::Instruction(machineInst);
    fixInst->AddOperand(MakeRegOperand(reg));
    fixInst->AddOperand(operandReg);
    codeGen.Emit(fixInst);
}

void EmitIntToFloat(IntToFloatInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting inttofloat: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    int machineInst = -1;
    if (inst.Operand()->GetType()->IsUnsignedType())
    {
        if (inst.Result()->GetType()->IsFloatType())
        {
            machineInst = cmsx::machine::SFLOTU;
        }
        else if (inst.Result()->GetType()->IsDoubleType())
        {
            machineInst = cmsx::machine::FLOTU;
        }
        else
        {
            codeGen.Error("error emitting inttofloat: invalid result type");
        }
    }
    else
    {
        if (inst.Result()->GetType()->IsFloatType())
        {
            machineInst = cmsx::machine::SFLOT;
        }
        else if (inst.Result()->GetType()->IsDoubleType())
        {
            machineInst = cmsx::machine::FLOT;
        }
        else
        {
            codeGen.Error("error emitting inttofloat: invalid result type");
        }
    }
    cmsx::assembler::Instruction* flotInst = new cmsx::assembler::Instruction(machineInst);
    flotInst->AddOperand(MakeRegOperand(reg));
    flotInst->AddOperand(operandReg);
    codeGen.Emit(flotInst);
}

void EmitTruncate(TruncateInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting truncate: reg not valid");
    }
    cmsx::assembler::Node* operandReg = MakeRegOperand(inst.Operand(), GetGlobalRegister(cmsx::machine::regAX), codeGen);
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regBX)));
    cmsx::assembler::Node* maskOperand = nullptr;
    switch (inst.Result()->GetType()->Id())
    {
        case boolTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0x01), true);
            break;
        }
        case byteTypeId:
        case sbyteTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFF), true);
            break;
        }
        case ushortTypeId:
        case shortTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFFFF), true);
            break;
        }
        case uintTypeId:
        case intTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFFFFFFFF), true);
            break;
        }
        case ulongTypeId:
        case longTypeId:
        {
            maskOperand = cmsx::assembler::MakeConstantExpr(uint64_t(0xFFFFFFFFFFFFFFFF), true);
            break;
        }
    }
    if (!maskOperand)
    {
        codeGen.Error("error emitting truncate: invalid result type");
    }
    setInst->AddOperand(maskOperand);
    codeGen.Emit(setInst);
    cmsx::assembler::Instruction* andInst = new cmsx::assembler::Instruction(cmsx::machine::AND);
    andInst->AddOperand(MakeRegOperand(reg));
    andInst->AddOperand(operandReg);
    andInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regBX)));
    codeGen.Emit(andInst);
}

void EmitArg(ArgInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting arg: reg not valid");
    }
    bool addrEmitted = false;
    if (inst.Arg()->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(inst.Arg());
        Instruction* argInst = regValue->Inst();
        if (argInst->IsLocalInstruction())
        {
            cmsx::assembler::Instruction* ldaInst = new cmsx::assembler::Instruction(cmsx::assembler::LDA);
            ldaInst->AddOperand(MakeRegOperand(reg));
            FrameLocation frameLocation = GetFrameLocation(inst.Arg(), codeGen);
            EmitFrameLocationOperand(frameLocation, ldaInst, codeGen);
            codeGen.Emit(ldaInst);
            addrEmitted = true;
        }
    }
    cmsx::assembler::Instruction* stouInst = new cmsx::assembler::Instruction(cmsx::machine::STOU);
    if (addrEmitted)
    {
        stouInst->AddOperand(MakeRegOperand(reg));
    }
    else
    {
        stouInst->AddOperand(MakeRegOperand(inst.Arg(), reg, codeGen));
    }
    EmitArgLocationOperand(stouInst, codeGen);
    codeGen.Emit(stouInst);
}

void EmitProcedureCall(ProcedureCallInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting procedure call: reg not valid");
    }
    int saveNumLocals = 0;
    int lastActiveLocalReg = codeGen.RegAllocator()->LastActiveLocalReg();
    if (lastActiveLocalReg != -1)
    {
        saveNumLocals = lastActiveLocalReg + 1;
    }
    cmsx::assembler::Instruction* callInst = new cmsx::assembler::Instruction(cmsx::machine::CALL);
    Frame& frame = codeGen.RegAllocator()->GetFrame();
    frame.CurrentCallFrame()->SetSaveNumLocals(saveNumLocals);
    callInst->AddOperand(cmsx::assembler::MakeConstantExpr(saveNumLocals));
    callInst->AddOperand(MakeCalleeOperand(inst.Callee(), reg, codeGen));
    codeGen.Emit(callInst);
    frame.AddCallFrame();
}

void EmitFunctionCall(FunctionCallInstruction& inst, CodeGenerator& codeGen)
{
    Register reg = codeGen.RegAllocator()->GetRegister(&inst);
    if (!reg.Valid())
    {
        codeGen.Error("error emitting function call: reg not valid");
    }
    int saveNumLocals = 0;
    int lastActiveLocalReg = codeGen.RegAllocator()->LastActiveLocalReg();
    if (lastActiveLocalReg != -1)
    {
        saveNumLocals = lastActiveLocalReg + 1;
    }
    cmsx::assembler::Instruction* callInst = new cmsx::assembler::Instruction(cmsx::machine::CALL);
    Frame& frame = codeGen.RegAllocator()->GetFrame();
    frame.CurrentCallFrame()->SetSaveNumLocals(saveNumLocals);
    callInst->AddOperand(cmsx::assembler::MakeConstantExpr(saveNumLocals));
    callInst->AddOperand(MakeCalleeOperand(inst.Callee(), reg, codeGen));
    codeGen.Emit(callInst);
    cmsx::assembler::Instruction* setRetValInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setRetValInst->AddOperand(MakeRegOperand(reg));
    setRetValInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
    codeGen.Emit(setRetValInst);
    frame.AddCallFrame();
}

void EmitRet(RetInstruction& inst, CodeGenerator& codeGen)
{
    if (inst.ReturnValue())
    {
        cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regAX)));
        setInst->AddOperand(MakeRegOperand(inst.ReturnValue(), GetGlobalRegister(cmsx::machine::regAX), codeGen));
        codeGen.Emit(setInst);
    }
    int targetLabelId = codeGen.ExitLabelId();
    cmsx::assembler::Instruction* jmpInst = new cmsx::assembler::Instruction(cmsx::machine::JMP);
    jmpInst->AddOperand(cmsx::assembler::MakeLocalSymbol(targetLabelId));
    codeGen.Emit(jmpInst);
}

void EmitTrap(TrapInstruction& inst, CodeGenerator& codeGen)
{
    int n = inst.Args().size();
    if (n > 4)
    {
        codeGen.Error("error emitting trap: too many arguments");
    }
    for (int i = 0; i < n; ++i)
    {
        Value* arg = inst.Args()[i];
        cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        uint8_t reg = cmsx::machine::regAX + i;
        setInst->AddOperand(MakeRegOperand(GetGlobalRegister(reg)));
        setInst->AddOperand(MakeRegOperand(arg, GetGlobalRegister(reg), codeGen));
        codeGen.Emit(setInst);
    }
    cmsx::assembler::Instruction* trapInst = new cmsx::assembler::Instruction(cmsx::machine::TRAP);
    trapInst->AddOperand(MakeTrapOperand(inst.Op1(), codeGen));
    trapInst->AddOperand(MakeTrapOperand(inst.Op2(), codeGen));
    trapInst->AddOperand(MakeTrapOperand(inst.Op3(), codeGen));
    codeGen.Emit(trapInst);
}

void EmitPrologue(CodeGenerator& codeGen)
{
    cmsx::assembler::Instruction* stoInst = new cmsx::assembler::Instruction(cmsx::machine::STO);
    stoInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regFP)));
    stoInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regSP)));
    codeGen.Emit(stoInst);
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regFP)));
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regSP)));
    codeGen.Emit(setInst);
    Frame& frame = codeGen.RegAllocator()->GetFrame();
    if (frame.IsWithinWydeRange())
    {
        cmsx::assembler::Instruction* inclInst = new cmsx::assembler::Instruction(cmsx::machine::INCL);
        inclInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regSP)));
        inclInst->AddOperand(cmsx::assembler::MakeConstantExpr(frame.Size()));
        codeGen.Emit(inclInst);
    }
    else
    {
        cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regIX)));
        setInst->AddOperand(cmsx::assembler::MakeConstantExpr(frame.Size()));
        codeGen.Emit(setInst);
        cmsx::assembler::Instruction* addInst = new cmsx::assembler::Instruction(cmsx::machine::ADD);
        addInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regSP)));
        addInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regSP)));
        addInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regIX)));
        codeGen.Emit(addInst);
    }
    frame.ResolveCallFrames();
}

void EmitEpilogue(CodeGenerator& codeGen)
{
    cmsx::assembler::Instruction* setInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
    setInst->SetLabel(cmsx::assembler::MakeLocalSymbol(codeGen.ExitLabelId()));
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regSP)));
    setInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regFP)));
    codeGen.Emit(setInst);
    cmsx::assembler::Instruction* ldoInst = new cmsx::assembler::Instruction(cmsx::machine::LDO);
    ldoInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regFP)));
    ldoInst->AddOperand(MakeRegOperand(GetGlobalRegister(cmsx::machine::regSP)));
    codeGen.Emit(ldoInst);
    cmsx::assembler::Instruction* retInst = new cmsx::assembler::Instruction(cmsx::machine::RET);
    codeGen.Emit(retInst);
}

void EmitBool(BoolValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitByte(static_cast<uint8_t>(value.GetValue()));
}

void EmitSByte(SByteValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitByte(static_cast<uint8_t>(value.GetValue()));
}

void EmitByte(ByteValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitByte(value.GetValue());
}

void EmitShort(ShortValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitWyde(static_cast<uint16_t>(value.GetValue()));
}

void EmitUShort(UShortValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitWyde(value.GetValue());
}

void EmitInt(IntValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitTetra(static_cast<uint32_t>(value.GetValue()));
}

void EmitUInt(UIntValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitTetra(value.GetValue());
}

void EmitLong(LongValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitOcta(static_cast<uint64_t>(value.GetValue()));
}

void EmitULong(ULongValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitOcta(value.GetValue());
}

void EmitFloat(FloatValue& value, CodeGenerator& codeGen)
{
    uint32_t v = *static_cast<uint32_t*>(static_cast<void*>(&value));
    codeGen.EmitTetra(v);
}

void EmitDouble(DoubleValue& value, CodeGenerator& codeGen)
{
    uint64_t v = *static_cast<uint64_t*>(static_cast<void*>(&value));
    codeGen.EmitOcta(v);
}

void EmitNull(CodeGenerator& codeGen)
{
    codeGen.EmitOcta(0);
}

void EmitAddress(AddressValue& value, CodeGenerator& codeGen)
{
    codeGen.EmitSymbol(value.Value()->Name());
}

void EmitString(StringValue& value, CodeGenerator& codeGen)
{
    for (char c : value.Value())
    {
        codeGen.EmitByte(static_cast<uint8_t>(c));
    }
    codeGen.EmitByte(0);
}

} // cmsx::intermediate
