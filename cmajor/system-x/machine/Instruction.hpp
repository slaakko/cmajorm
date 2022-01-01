// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_INSTRUCTION_INCLUDED
#define CMSX_MACHINE_INSTRUCTION_INCLUDED
#include <system-x/machine/OpCode.hpp>
#include <stdint.h>

namespace cmsx::machine {

class Machine;
class Formatter;

class CMSX_MACHINE_API Instruction
{
public:
    Instruction(uint8_t opCode_);
    virtual ~Instruction();
    uint8_t OpCode() const { return opCode; }
    virtual void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z);
    virtual void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z);
    bool IsJumpInstruction() const { return IsJumpInst(opCode); }
private:
    uint8_t opCode;
};

class CMSX_MACHINE_API Trap : public Instruction
{
public:
    Trap();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};


class CMSX_MACHINE_API Swym : public Instruction
{
public:
    Swym();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldb : public Instruction
{
public:
    Ldb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldbi : public Instruction
{
public:
    Ldbi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldw : public Instruction
{
public:
    Ldw();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldwi : public Instruction
{
public:
    Ldwi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldt : public Instruction
{
public:
    Ldt();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldti : public Instruction
{
public:
    Ldti();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldo : public Instruction
{
public:
    Ldo();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldoi : public Instruction
{
public:
    Ldoi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldbu : public Instruction
{
public:
    Ldbu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldbui : public Instruction
{
public:
    Ldbui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldwu : public Instruction
{
public:
    Ldwu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldwui : public Instruction
{
public:
    Ldwui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldtu : public Instruction
{
public:
    Ldtu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldtui : public Instruction
{
public:
    Ldtui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldou : public Instruction
{
public:
    Ldou();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldoui : public Instruction
{
public:
    Ldoui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldht : public Instruction
{
public:
    Ldht();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldhti : public Instruction
{
public:
    Ldhti();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldsf : public Instruction
{
public:
    Ldsf();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ldsfi : public Instruction
{
public:
    Ldsfi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stb : public Instruction
{
public:
    Stb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stbi : public Instruction
{
public:
    Stbi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stw : public Instruction
{
public:
    Stw();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stwi : public Instruction
{
public:
    Stwi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stt : public Instruction
{
public:
    Stt();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stti : public Instruction
{
public:
    Stti();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sto : public Instruction
{
public:
    Sto();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stoi : public Instruction
{
public:
    Stoi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stbu : public Instruction
{
public:
    Stbu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stbui : public Instruction
{
public:
    Stbui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stwu : public Instruction
{
public:
    Stwu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stwui : public Instruction
{
public:
    Stwui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sttu : public Instruction
{
public:
    Sttu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sttui : public Instruction
{
public:
    Sttui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stou : public Instruction
{
public:
    Stou();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stoui : public Instruction
{
public:
    Stoui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stht : public Instruction
{
public:
    Stht();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sthti : public Instruction
{
public:
    Sthti();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stsf : public Instruction
{
public:
    Stsf();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stsfi : public Instruction
{
public:
    Stsfi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stco : public Instruction
{
public:
    Stco();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Stcoi : public Instruction
{
public:
    Stcoi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Add : public Instruction
{
public:
    Add();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Addi : public Instruction
{
public:
    Addi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sub : public Instruction
{
public:
    Sub();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Subi : public Instruction
{
public:
    Subi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Mul : public Instruction
{
public:
    Mul();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Muli : public Instruction
{
public:
    Muli();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Div : public Instruction
{
public:
    Div();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Divi : public Instruction
{
public:
    Divi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Addu : public Instruction
{
public:
    Addu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Addui : public Instruction
{
public:
    Addui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Subu : public Instruction
{
public:
    Subu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Subui : public Instruction
{
public:
    Subui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Mulu : public Instruction
{
public:
    Mulu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Mului : public Instruction
{
public:
    Mului();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Divu : public Instruction
{
public:
    Divu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Divui : public Instruction
{
public:
    Divui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I2Addu : public Instruction
{
public:
    I2Addu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I2Addui : public Instruction
{
public:
    I2Addui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I4Addu : public Instruction
{
public:
    I4Addu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I4Addui : public Instruction
{
public:
    I4Addui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I8Addu : public Instruction
{
public:
    I8Addu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I8Addui : public Instruction
{
public:
    I8Addui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I16Addu : public Instruction
{
public:
    I16Addu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API I16Addui : public Instruction
{
public:
    I16Addui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Neg : public Instruction
{
public:
    Neg();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Negi : public Instruction
{
public:
    Negi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Negu : public Instruction
{
public:
    Negu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Negui : public Instruction
{
public:
    Negui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sl : public Instruction
{
public:
    Sl();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sli : public Instruction
{
public:
    Sli();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Slu : public Instruction
{
public:
    Slu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sluí : public Instruction
{
public:
    Sluí();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sr : public Instruction
{
public:
    Sr();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sri : public Instruction
{
public:
    Sri();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sru : public Instruction
{
public:
    Sru();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Srui : public Instruction
{
public:
    Srui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Cmp : public Instruction
{
public:
    Cmp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Cmpi : public Instruction
{
public:
    Cmpi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Cmpu : public Instruction
{
public:
    Cmpu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Cmpui : public Instruction
{
public:
    Cmpui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csn : public Instruction
{
public:
    Csn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csni : public Instruction
{
public:
    Csni();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csz : public Instruction
{
public:
    Csz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Cszi : public Instruction
{
public:
    Cszi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csp : public Instruction
{
public:
    Csp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Cspi : public Instruction
{
public:
    Cspi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csod : public Instruction
{
public:
    Csod();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csodi : public Instruction
{
public:
    Csodi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csnn : public Instruction
{
public:
    Csnn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csnni : public Instruction
{
public:
    Csnni();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csnz : public Instruction
{
public:
    Csnz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csnzi : public Instruction
{
public:
    Csnzi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csnp : public Instruction
{
public:
    Csnp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csnpi : public Instruction
{
public:
    Csnpi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csev : public Instruction
{
public:
    Csev();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Csevi : public Instruction
{
public:
    Csevi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsn : public Instruction
{
public:
    Zsn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsni : public Instruction
{
public:
    Zsni();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsz : public Instruction
{
public:
    Zsz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zszi : public Instruction
{
public:
    Zszi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsp : public Instruction
{
public:
    Zsp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zspi : public Instruction
{
public:
    Zspi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsod : public Instruction
{
public:
    Zsod();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsodi : public Instruction
{
public:
    Zsodi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsnn : public Instruction
{
public:
    Zsnn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsnni : public Instruction
{
public:
    Zsnni();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsnz : public Instruction
{
public:
    Zsnz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsnzi : public Instruction
{
public:
    Zsnzi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsnp : public Instruction
{
public:
    Zsnp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsnpi : public Instruction
{
public:
    Zsnpi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsev : public Instruction
{
public:
    Zsev();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Zsevi : public Instruction
{
public:
    Zsevi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API And : public Instruction
{
public:
    And();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Andi : public Instruction
{
public:
    Andi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Or : public Instruction
{
public:
    Or();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ori : public Instruction
{
public:
    Ori();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Xor : public Instruction
{
public:
    Xor();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Xori : public Instruction
{
public:
    Xori();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Andn : public Instruction
{
public:
    Andn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Andni : public Instruction
{
public:
    Andni();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Orn : public Instruction
{
public:
    Orn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Orni : public Instruction
{
public:
    Orni();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Nand : public Instruction
{
public:
    Nand();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Nandi : public Instruction
{
public:
    Nandi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Nor : public Instruction
{
public:
    Nor();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Nori : public Instruction
{
public:
    Nori();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Nxor : public Instruction
{
public:
    Nxor();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Nxori : public Instruction
{
public:
    Nxori();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Mux : public Instruction
{
public:
    Mux();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Muxi : public Instruction
{
public:
    Muxi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sadd : public Instruction
{
public:
    Sadd();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Saddi : public Instruction
{
public:
    Saddi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bdif : public Instruction
{
public:
    Bdif();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bdifi : public Instruction
{
public:
    Bdifi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Wdif : public Instruction
{
public:
    Wdif();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Wdifi : public Instruction
{
public:
    Wdifi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Tdif : public Instruction
{
public:
    Tdif();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Tdifi : public Instruction
{
public:
    Tdifi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Odif : public Instruction
{
public:
    Odif();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Odifi : public Instruction
{
public:
    Odifi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fadd : public Instruction
{
public:
    Fadd();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fsub : public Instruction
{
public:
    Fsub();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fmul : public Instruction
{
public:
    Fmul();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fdiv : public Instruction
{
public:
    Fdiv();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Frem : public Instruction
{
public:
    Frem();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fsqrt : public Instruction
{
public:
    Fsqrt();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fint : public Instruction
{
public:
    Fint();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fcmp : public Instruction
{
public:
    Fcmp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Feql : public Instruction
{
public:
    Feql();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fix : public Instruction
{
public:
    Fix();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Fixu : public Instruction
{
public:
    Fixu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Flot : public Instruction
{
public:
    Flot();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Floti : public Instruction
{
public:
    Floti();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Flotu : public Instruction
{
public:
    Flotu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Flotui : public Instruction
{
public:
    Flotui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sflot : public Instruction
{
public:
    Sflot();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sfloti : public Instruction
{
public:
    Sfloti();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sflotu : public Instruction
{
public:
    Sflotu();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Sflotui : public Instruction
{
public:
    Sflotui();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Seth : public Instruction
{
public:
    Seth();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Setmh : public Instruction
{
public:
    Setmh();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Setml : public Instruction
{
public:
    Setml();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Setl : public Instruction
{
public:
    Setl();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Inch : public Instruction
{
public:
    Inch();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Incmh : public Instruction
{
public:
    Incmh();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Incml : public Instruction
{
public:
    Incml();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Incl : public Instruction
{
public:
    Incl();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Orh : public Instruction
{
public:
    Orh();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ormh : public Instruction
{
public:
    Ormh();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Orml : public Instruction
{
public:
    Orml();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Orl : public Instruction
{
public:
    Orl();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Andnh : public Instruction
{
public:
    Andnh();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Andnmh : public Instruction
{
public:
    Andnmh();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Andnml : public Instruction
{
public:
    Andnml();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Andnl : public Instruction
{
public:
    Andnl();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Jmp : public Instruction
{
public:
    Jmp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Jmpb : public Instruction
{
public:
    Jmpb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Go : public Instruction
{
public:
    Go();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Goi : public Instruction
{
public:
    Goi();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Call : public Instruction
{
public:
    Call();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Calli : public Instruction
{
public:
    Calli();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Ret : public Instruction
{
public:
    Ret();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bn : public Instruction
{
public:
    Bn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bnb : public Instruction
{
public:
    Bnb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bz : public Instruction
{
public:
    Bz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bzb : public Instruction
{
public:
    Bzb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bp : public Instruction
{
public:
    Bp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bpb : public Instruction
{
public:
    Bpb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bod : public Instruction
{
public:
    Bod();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bodb : public Instruction
{
public:
    Bodb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bnn : public Instruction
{
public:
    Bnn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bnnb : public Instruction
{
public:
    Bnnb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bnz : public Instruction
{
public:
    Bnz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bnzb : public Instruction
{
public:
    Bnzb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bnp : public Instruction
{
public:
    Bnp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bnpb : public Instruction
{
public:
    Bnpb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bev : public Instruction
{
public:
    Bev();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Bevb : public Instruction
{
public:
    Bevb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbn : public Instruction
{
public:
    Pbn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbnb : public Instruction
{
public:
    Pbnb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbz : public Instruction
{
public:
    Pbz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbzb : public Instruction
{
public:
    Pbzb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbp : public Instruction
{
public:
    Pbp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbpb : public Instruction
{
public:
    Pbpb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbod : public Instruction
{
public:
    Pbod();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbodb : public Instruction
{
public:
    Pbodb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbnn : public Instruction
{
public:
    Pbnn();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbnnb : public Instruction
{
public:
    Pbnnb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbnz : public Instruction
{
public:
    Pbnz();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbnzb : public Instruction
{
public:
    Pbnzb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbnp : public Instruction
{
public:
    Pbnp();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbnpb : public Instruction
{
public:
    Pbnpb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbev : public Instruction
{
public:
    Pbev();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Pbevb : public Instruction
{
public:
    Pbevb();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Get : public Instruction
{
public:
    Get();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Put : public Instruction
{
public:
    Put();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Geta : public Instruction
{
public:
    Geta();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

class CMSX_MACHINE_API Getab : public Instruction
{
public:
    Getab();
    void Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z) override;
    void Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z) override;
};

} // cmsx::machine

#endif // CMSX_MACHINE_INSTRUCTION_INCLUDED
