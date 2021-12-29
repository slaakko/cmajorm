// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/BinaryFileFormatter.hpp>
#include <system-x/object/BinaryFile.hpp>

namespace cmsx::object {

BinaryFileFormatter::BinaryFileFormatter(BinaryFile* file_) : file(file_), currentSection(nullptr), address(0), absoluteAddress(0), setAddress(0)
{
}

void BinaryFileFormatter::FormatCurrentAddress()
{
}

void BinaryFileFormatter::FormatAssembledBytes(uint8_t opc, uint8_t x, uint8_t y, uint8_t z)
{
}

void BinaryFileFormatter::FormatLabel()
{
}

void BinaryFileFormatter::FormatOpCode(const std::string& opCodeName)
{
}

void BinaryFileFormatter::FormatByteOperand(uint8_t operand)
{
}

void BinaryFileFormatter::FormatRegOperand(uint8_t reg)
{
}

void BinaryFileFormatter::FormatComma()
{
}

void BinaryFileFormatter::FormatColon()
{
}

void BinaryFileFormatter::FormatEol()
{
}

void BinaryFileFormatter::FormatUndefined()
{
}

void BinaryFileFormatter::FormatSpecialReg(uint8_t reg)
{
}

void BinaryFileFormatter::FormatWydeOperand(uint16_t wyde)
{
}

void BinaryFileFormatter::FormatAddress(uint64_t address)
{
}

void BinaryFileFormatter::FormatSetAddress(uint64_t saddr)
{
}

void BinaryFileFormatter::Format()
{
    FormatSection(file->GetHeaderSection());
    FormatSection(file->GetCodeSection());
    FormatSection(file->GetDataSection());
    FormatSection(file->GetSymbolSection());
    FormatSection(file->GetLinkSection());
    FormatSection(file->GetDebugSection());
    FormatSymbolTable(file->GetSymbolTable());
}

void BinaryFileFormatter::FormatSection(HeaderSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSection(CodeSection* section)
{
    currentSection = section;
    int64_t baseAddress = currentSection->BaseAddress();
    int64_t n = currentSection->Length();
    address = 0;
    while (address < n)
    {
        absoluteAddress = baseAddress + address;
        FormatCurrentAddress();
        int8_t opc = section->GetByte(address);
        ++address;
        int8_t x = section->GetByte(address);
        ++address;
        int8_t y = section->GetByte(address);
        ++address;
        int8_t z = section->GetByte(address);
        ++address;
        FormatAssembledBytes(opc, x, y, z);
        FormatLabel();
        Instruction* inst = machine.GetInstruction(opc);
        inst->Format(*this, x, y, z);
        FormatEol();
    }
}

void BinaryFileFormatter::FormatSection(DataSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSection(SymbolSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSection(LinkSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSection(DebugSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSymbolTable(SymbolTable& symbolTable)
{
    // todo
}

void BinaryFileFormatter::Format(Trap& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Swym& inst, uint8_t x, uint8_t y, uint8_t z) 
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
}

void BinaryFileFormatter::Format(Ldb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldbi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldw& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldwi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldt& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldo& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldoi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldbu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldbui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldwu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldwui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldtu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldtui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldou& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldoui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldht& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldhti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Ldsf& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ldsfi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stbi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stw& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stwi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stt& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sto& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stoi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stbu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stbui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stwu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stwui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sttu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Sttui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stou& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stoui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stht& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Sthti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stsf& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stsfi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Stco& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Stcoi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Add& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Addi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sub& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Subi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Mul& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Muli& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Div& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Divi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Subu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Subui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Mulu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Mului& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Divu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Divui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(I2Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(I2Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(I4Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(I4Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(I8Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(I8Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(I16Addu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(I16Addui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y); 
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Neg& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y); 
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Negi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Negu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Negui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Sli& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Slu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Sluí& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sr& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Sri& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sru& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Srui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Cmp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Cmpi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Cmpu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Cmpui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Csni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Cszi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Cspi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csod& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Csodi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csnn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Csnni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csnz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Csnzi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x); 
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csnp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Csnpi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Csev& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Csevi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatRegOperand(y);
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zsni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zszi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zspi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsod& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zsodi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsnn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zsnni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsnz& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zsnzi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsnp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zsnpi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Zsev& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Zsevi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(And& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Andi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Or& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Ori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Xor& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Xori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Andn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Andni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Orn& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Orni& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Nand& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Nandi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Nor& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Nori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Nxor& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Nxori& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Mux& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Muxi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sadd& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Saddi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Bdif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Bdifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Wdif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Wdifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Tdif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Tdifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Odif& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Odifi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Fadd& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fsub& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fmul& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x); 
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fdiv& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Frem& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fsqrt& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fint& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fcmp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Feql& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fix& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Fixu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Flot& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Floti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Flotu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Flotui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sflot& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Sfloti& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Sflotu& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Sflotui& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatByteOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Seth& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 48;
    setAddress = value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Setmh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 32;
    setAddress = value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Setml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    setAddress = value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Setl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    setAddress = value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Inch& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 48;
    setAddress = setAddress + value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Incmh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 32;
    setAddress = setAddress + value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Incml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    setAddress = setAddress + value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Incl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz);
    setAddress = setAddress + value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Orh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 48;
    setAddress = setAddress | value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Ormh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 32;
    setAddress = setAddress | value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Orml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    setAddress = setAddress | value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Orl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz);
    setAddress = setAddress | value;
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Andnh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Andnmh& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Andnml& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Andnl& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    FormatWydeOperand(yz);
}

void BinaryFileFormatter::Format(Jmp& inst, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t ra = ((static_cast<uint64_t>(x) << 16) | (static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    if (x == 0xFF && y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Jmpb& inst, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t ra = ((static_cast<uint64_t>(x) << 16) | (static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    if (x == 0xFF && y == 0xFF && z == 0xFF)
    {
        FormatUndefined();
    }
    else
    {
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Go& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Goi& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
}

void BinaryFileFormatter::Format(Call& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatRegOperand(z);
    FormatColon();
    FormatSetAddress(setAddress);
}

void BinaryFileFormatter::Format(Calli& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatByteOperand(x);
    FormatComma();
    FormatRegOperand(y);
    FormatComma();
    FormatByteOperand(z);
    FormatColon();
    FormatSetAddress(setAddress);
}

void BinaryFileFormatter::Format(Ret& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
}

void BinaryFileFormatter::Format(Bn& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bnb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Bz& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bzb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Bp& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bpb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Bod& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bodb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Bnn& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bnnb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Bnz& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bnzb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Bnp& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bnpb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Bev& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Bevb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbn& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbnb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbz& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbzb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbp& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbpb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbod& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbodb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbnn& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbnnb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbnz& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbnzb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbnp& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbnpb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Pbev& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Pbevb& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

void BinaryFileFormatter::Format(Get& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatRegOperand(x);
    FormatComma();
    FormatSpecialReg(z);
}

void BinaryFileFormatter::Format(Put& inst, uint8_t x, uint8_t y, uint8_t z)
{
    FormatOpCode(GetOpCodeName(inst.OpCode()));
    FormatSpecialReg(x);
    FormatComma();
    FormatRegOperand(z);
}

void BinaryFileFormatter::Format(Geta& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() + ra);
    }
}

void BinaryFileFormatter::Format(Getab& inst, uint8_t x, uint8_t y, uint8_t z)
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
        FormatAddress(AbsoluteAddress() - ra);
    }
}

} // namespace cmsx::object
