// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/InstructionFormatter.hpp>

namespace cmsx::object {

InstructionFormatter::InstructionFormatter(Machine& machine_) : 
    Formatter(), machine(machine_), absoluteAddress(-1), setAddressMap(nullptr)
{
}

void InstructionFormatter::FormatInstruction(uint64_t address, std::map<int64_t, int64_t>* setAddressMap_, uint8_t opc, uint8_t x, uint8_t y, uint8_t z)
{
    absoluteAddress = address;
    setAddressMap = setAddressMap_;
    FormatCurrentAddress(absoluteAddress);
    FormatAssembledBytes(opc, x, y, z);
    FormatLabel(absoluteAddress);
    Instruction* inst = machine.GetInstruction(opc);
    inst->Format(*this, x, y, z);
    FormatEol();
}

void InstructionFormatter::FormatCurrentAddress(uint64_t currentAddress)
{
}

void InstructionFormatter::FormatAssembledBytes(uint8_t opc, uint8_t x, uint8_t y, uint8_t z)
{
}

void InstructionFormatter::FormatLabel(uint64_t currentAddress)
{
}

void InstructionFormatter::FormatOpCode(const std::string& opCodeName)
{
}

void InstructionFormatter::FormatByteOperand(uint8_t operand)
{
}

void InstructionFormatter::FormatRegOperand(uint8_t reg)
{
}

void InstructionFormatter::FormatComma()
{
}

void InstructionFormatter::FormatColon()
{
}

void InstructionFormatter::FormatEol()
{
}

void InstructionFormatter::FormatUndefined()
{
}

void InstructionFormatter::FormatSpecialReg(uint8_t reg)
{
}

void InstructionFormatter::FormatWydeOperand(uint16_t wyde)
{
}

void InstructionFormatter::FormatAddress(uint64_t address)
{
}

void InstructionFormatter::FormatSetAddress(uint64_t saddr)
{
}

void InstructionFormatter::Format(Trap& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Swym& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
}

void InstructionFormatter::Format(Ldb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldbi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldw& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldwi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldt& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldo& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldoi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldbu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldbui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldwu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldwui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldtu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldtui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldou& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldoui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldht& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldhti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Ldsf& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ldsfi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stbi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stw& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stwi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stt& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sto& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stoi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stbu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stbui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stwu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stwui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sttu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Sttui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stou& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stoui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stht& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Sthti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stsf& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stsfi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Stco& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Stcoi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Add& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Addi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sub& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Subi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Mul& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Muli& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Div& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Divi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Subu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Subui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Mulu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Mului& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Divu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Divui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(I2Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(I2Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(I4Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(I4Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(I8Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(I8Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(I16Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(I16Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Neg& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Negi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Negu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Negui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Sli& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Slu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Sluí& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sr& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Sri& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sru& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Srui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Cmp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Cmpi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Cmpu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Cmpui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Csni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Cszi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Cspi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csod& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Csodi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csnn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Csnni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csnz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Csnzi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csnp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Csnpi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Csev& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Csevi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatRegOperand(y);
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zsni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zszi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zspi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsod& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zsodi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsnn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zsnni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsnz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zsnzi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsnp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zsnpi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Zsev& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Zsevi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(And& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Andi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Or& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Ori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Xor& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Xori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Andn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Andni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Orn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Orni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Nand& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Nandi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Nor& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Nori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Nxor& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Nxori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Mux& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Muxi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sadd& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Saddi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Bdif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Bdifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Wdif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Wdifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Tdif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Tdifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Odif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Odifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Fadd& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fsub& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fmul& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fdiv& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Frem& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fsqrt& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fint& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fcmp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Feql& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fix& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Fixu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Flot& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Floti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Flotu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Flotui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sflot& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Sfloti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Sflotu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Sflotui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Seth& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 48;
    SetSetAddress(absoluteAddress, value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Setmh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 32;
    SetSetAddress(absoluteAddress, value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Setml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    SetSetAddress(absoluteAddress, value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Setl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    SetSetAddress(absoluteAddress, value); 
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Inch& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 48;
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) + value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Incmh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 32;
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) + value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Incml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) + value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Incl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz);
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) + value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Orh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 48;
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) | value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Ormh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 32;
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) | value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Orml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) | value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Orl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz);
    SetSetAddress(absoluteAddress, GetSetAddress(absoluteAddress - 4) | value);
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Andnh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Andnmh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Andnml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Andnl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void InstructionFormatter::Format(Jmp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t ra = ((static_cast<uint64_t>(x) << 16) | (static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    if (x == 0xFF && y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Jmpb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t ra = ((static_cast<uint64_t>(x) << 16) | (static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    if (x == 0xFF && y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Go& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Goi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void InstructionFormatter::Format(Call& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
    FormatColon();
    FormatSetAddress(GetSetAddress(absoluteAddress - 4));
}

void InstructionFormatter::Format(Calli& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
    FormatColon();
    FormatSetAddress(GetSetAddress(absoluteAddress - 4));
}

void InstructionFormatter::Format(Ret& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
}

void InstructionFormatter::Format(Bn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bnb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Bz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bzb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Bp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bpb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Bod& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bodb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Bnn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bnnb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Bnz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bnzb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Bnp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bnpb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Bev& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Bevb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbnb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbzb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbpb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbod& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbodb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbnn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbnnb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbnz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbnzb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbnp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbnpb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Pbev& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Pbevb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

void InstructionFormatter::Format(Get& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatSpecialReg(z);
}

void InstructionFormatter::Format(Put& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatSpecialReg(x);
    FormatComma();
    FormatRegOperand(z);
}

void InstructionFormatter::Format(Geta& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress + ra);
    }
}

void InstructionFormatter::Format(Getab& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    if (y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        FormatAddress(absoluteAddress - ra);
    }
}

int64_t InstructionFormatter::GetSetAddress(int64_t absoluteAddress) const
{
    if (setAddressMap)
    {
        auto it = setAddressMap->find(absoluteAddress);
        if (it != setAddressMap->cend())
        {
            return it->second;
        }
    }
    return -1;
}

void InstructionFormatter::SetSetAddress(int64_t absoluteAddress, int64_t setAddress)
{
    if (setAddressMap)
    {
        (*setAddressMap)[absoluteAddress] = setAddress;
    }
}

} // cmsx::object
