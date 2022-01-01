// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Instruction.hpp>
#include <system-x/machine/Interrupt.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/OpCode.hpp>
#include <system-x/machine/Formatter.hpp>
#include <system-x/machine/Util.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <stdexcept>
#include <tgmath.h>

namespace cmsx::machine {

Instruction::Instruction(uint8_t opCode_) : opCode(opCode_)
{
}

Instruction::~Instruction()
{
}

void Instruction::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    throw std::runtime_error("instruction '" + GetOpCodeName(opCode) + "' (" + std::to_string(opCode) + ") not implemented");
}

void Instruction::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
}

Trap::Trap() : Instruction(TRAP)
{
}

void Trap::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t trap = (static_cast<uint64_t>(x) << 16) | (static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z);
    machine.Regs().SetSpecial(rX, trap);
    machine.Regs().SetInterrupt(SOFTWARE_INTERRUPT_BIT);
}

void Trap::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Swym::Swym() : Instruction(SWYM)
{
}

void Swym::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
}

void Swym::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldb::Ldb() : Instruction(LDB)
{
}

void Ldb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint8_t value = machine.Mem().ReadByte(a, Protection::read);
    uint64_t xx = SignExtend(value);
    machine.Regs().Set(x, xx);
}

void Ldb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldbi::Ldbi() : Instruction(LDBI)
{
}

void Ldbi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint8_t value = machine.Mem().ReadByte(a, Protection::read);
    uint64_t xx = SignExtend(value);
    machine.Regs().Set(x, xx);
}

void Ldbi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldw::Ldw() : Instruction(LDW)
{
}

void Ldw::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint16_t value = machine.Mem().ReadWyde(a, Protection::read);
    uint64_t xx = SignExtend(value);
    machine.Regs().Set(x, xx);
}

void Ldw::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldwi::Ldwi() : Instruction(LDWI)
{
}

void Ldwi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint16_t value = machine.Mem().ReadWyde(a, Protection::read);
    uint64_t xx = SignExtend(value);
    machine.Regs().Set(x, xx);
}

void Ldwi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldt::Ldt() : Instruction(LDT)
{
}

void Ldt::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint32_t value = machine.Mem().ReadTetra(a, Protection::read);
    uint64_t xx = SignExtend(value);
    machine.Regs().Set(x, xx);
}

void Ldt::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldti::Ldti() : Instruction(LDTI)
{
}

void Ldti::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint32_t value = machine.Mem().ReadTetra(a, Protection::read);
    uint64_t xx = SignExtend(value);
    machine.Regs().Set(x, xx);
}

void Ldti::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldo::Ldo() : Instruction(LDO)
{
}

void Ldo::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t value = machine.Mem().ReadOcta(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldo::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldoi::Ldoi() : Instruction(LDOI)
{
}

void Ldoi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t value = machine.Mem().ReadOcta(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldoi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldbu::Ldbu() : Instruction(LDBU)
{
}

void Ldbu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint8_t value = machine.Mem().ReadByte(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldbu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldbui::Ldbui() : Instruction(LDBUI)
{
}

void Ldbui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint8_t value = machine.Mem().ReadByte(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldbui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldwu::Ldwu() : Instruction(LDWU)
{
}

void Ldwu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint16_t value = machine.Mem().ReadWyde(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldwu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldwui::Ldwui() : Instruction(LDWUI)
{
}


void Ldwui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint16_t value = machine.Mem().ReadWyde(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldwui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldtu::Ldtu() : Instruction(LDTU)
{
}

void Ldtu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint32_t value = machine.Mem().ReadTetra(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldtu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldtui::Ldtui() : Instruction(LDTUI)
{
}

void Ldtui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint32_t value = machine.Mem().ReadTetra(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldtui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldou::Ldou() : Instruction(LDOU)
{
}

void Ldou::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t value = machine.Mem().ReadOcta(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldou::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldoui::Ldoui() : Instruction(LDOUI)
{
}

void Ldoui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t value = machine.Mem().ReadOcta(a, Protection::read);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Ldoui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldht::Ldht() : Instruction(LDHT)
{
}

void Ldht::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint32_t value = machine.Mem().ReadTetra(a, Protection::read);
    uint64_t xx = static_cast<uint64_t>(value) << 32;
    machine.Regs().Set(x, xx);
}

void Ldht::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldhti::Ldhti() : Instruction(LDHTI)
{
}

void Ldhti::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint32_t value = machine.Mem().ReadTetra(a, Protection::read);
    uint64_t xx = static_cast<uint64_t>(value) << 32;
    machine.Regs().Set(x, xx);
}

void Ldhti::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldsf::Ldsf() : Instruction(LDSF)
{
}

void Ldsf::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint32_t u = machine.Mem().ReadTetra(a, Protection::read);
    float f = *static_cast<float*>(static_cast<void*>(&u));
    double d = f;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&d));
    machine.Regs().Set(x, xx);
}

void Ldsf::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ldsfi::Ldsfi() : Instruction(LDSFI)
{
}

void Ldsfi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint32_t u = machine.Mem().ReadTetra(a, Protection::read);
    float f = *static_cast<float*>(static_cast<void*>(&u));
    double d = f;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&d));
    machine.Regs().Set(x, xx);
}

void Ldsfi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stb::Stb() : Instruction(STB)
{
}

void Stb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint8_t value = static_cast<uint8_t>(xx & 0xFF);
    machine.Mem().WriteByte(a, value, Protection::write);
}

void Stb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stbi::Stbi() : Instruction(STBI)
{
}

void Stbi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint8_t value = static_cast<uint8_t>(xx & 0xFF);
    machine.Mem().WriteByte(a, value, Protection::write);
}

void Stbi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stw::Stw() : Instruction(STW)
{
}

void Stw::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint16_t value = static_cast<uint16_t>(xx & 0xFFFF);
    machine.Mem().WriteWyde(a, value, Protection::write);
}

void Stw::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stwi::Stwi() : Instruction(STWI)
{
}

void Stwi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint16_t value = static_cast<uint16_t>(xx & 0xFFFF);
    machine.Mem().WriteWyde(a, value, Protection::write);
}

void Stwi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stt::Stt() : Instruction(STT)
{
}

void Stt::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint32_t value = static_cast<uint32_t>(xx & 0xFFFFFFFF);
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Stt::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stti::Stti() : Instruction(STTI)
{
}

void Stti::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint32_t value = static_cast<uint32_t>(xx & 0xFFFFFFFF);
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Stti::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sto::Sto() : Instruction(STO)
{
}

void Sto::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx;
    machine.Mem().WriteOcta(a, value, Protection::write);
}

void Sto::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stoi::Stoi() : Instruction(STOI)
{
}

void Stoi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx;
    machine.Mem().WriteOcta(a, value, Protection::write);
}

void Stoi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stbu::Stbu() : Instruction(STBU)
{
}

void Stbu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint8_t value = static_cast<uint8_t>(xx & 0xFF);
    machine.Mem().WriteByte(a, value, Protection::write);
}

void Stbu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stbui::Stbui() : Instruction(STBUI)
{
}

void Stbui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint8_t value = static_cast<uint8_t>(xx & 0xFF);
    machine.Mem().WriteByte(a, value, Protection::write);
}

void Stbui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stwu::Stwu() : Instruction(STWU)
{
}

void Stwu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint16_t value = static_cast<uint16_t>(xx & 0xFFFF);
    machine.Mem().WriteWyde(a, value, Protection::write);
}

void Stwu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stwui::Stwui() : Instruction(STWUI)
{
}

void Stwui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint16_t value = static_cast<uint16_t>(xx & 0xFFFF);
    machine.Mem().WriteWyde(a, value, Protection::write);
}

void Stwui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sttu::Sttu() : Instruction(STTU)
{
}

void Sttu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint32_t value = static_cast<uint32_t>(xx & 0xFFFFFFFF);
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Sttu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sttui::Sttui() : Instruction(STTUI)
{
}

void Sttui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint32_t value = static_cast<uint32_t>(xx & 0xFFFFFFFF);
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Sttui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stou::Stou() : Instruction(STOU)
{
}

void Stou::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx;
    machine.Mem().WriteOcta(a, value, Protection::write);
}

void Stou::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stoui::Stoui() : Instruction(STOUI)
{
}

void Stoui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx;
    machine.Mem().WriteOcta(a, value, Protection::write);
}

void Stoui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stht::Stht() : Instruction(STHT)
{
}

void Stht::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint32_t value = xx >> 32;
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Stht::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sthti::Sthti() : Instruction(STHTI)
{
}

void Sthti::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    uint32_t value = xx >> 32;
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Sthti::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stsf::Stsf() : Instruction(STSF)
{
}

void Stsf::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    double d = *static_cast<double*>(static_cast<void*>(&xx));
    float f = static_cast<float>(d);
    uint32_t value = *static_cast<uint32_t*>(static_cast<void*>(&f));
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Stsf::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stsfi::Stsfi() : Instruction(STSFI)
{
}

void Stsfi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = machine.Regs().Get(x);
    double d = *static_cast<double*>(static_cast<void*>(&xx));
    float f = static_cast<float>(d);
    uint32_t value = *static_cast<uint32_t*>(static_cast<void*>(&f));
    machine.Mem().WriteTetra(a, value, Protection::write);
}

void Stsfi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stco::Stco() : Instruction(STCO)
{
}

void Stco::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t a = yy + zz;
    uint64_t xx = x;
    uint64_t value = xx;
    machine.Mem().WriteOcta(a, value, Protection::write);
}

void Stco::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Stcoi::Stcoi() : Instruction(STCOI)
{
}

void Stcoi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t a = yy + zz;
    uint64_t xx = x;
    uint64_t value = xx;
    machine.Mem().WriteOcta(a, value, Protection::write);
}

void Stcoi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Add::Add() : Instruction(ADD)
{
}

void Add::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = static_cast<int64_t>(machine.Regs().Get(z));
    int64_t value = yy + zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Add::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Addi::Addi() : Instruction(ADDI)
{
}

void Addi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = z;
    int64_t value = yy + zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Addi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sub::Sub() : Instruction(SUB)
{
}

void Sub::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = static_cast<int64_t>(machine.Regs().Get(z));
    int64_t value = yy - zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Sub::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Subi::Subi() : Instruction(SUBI)
{
}

void Subi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = z;
    int64_t value = yy - zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Subi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Mul::Mul() : Instruction(MUL)
{
}

void Mul::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = static_cast<int64_t>(machine.Regs().Get(z));
    int64_t value = yy * zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Mul::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Muli::Muli() : Instruction(MULI)
{
}

void Muli::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = z;
    int64_t value = yy * zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Muli::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Div::Div() : Instruction(DIV)
{
}

void Div::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = static_cast<int64_t>(machine.Regs().Get(z));
    int64_t value = 0;
    int64_t remainder = yy;
    if (zz != 0)
    {
        value = yy / zz;
        remainder = yy % zz;
    }
    uint64_t rr = static_cast<uint64_t>(remainder);
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
    machine.Regs().SetSpecial(rR, rr);
}

void Div::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Divi::Divi() : Instruction(DIVI)
{
}

void Divi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = z;
    int64_t value = 0;
    int64_t remainder = yy;
    if (zz != 0)
    {
        value = yy / zz;
        remainder = yy % zz;
    }
    uint64_t rr = static_cast<uint64_t>(remainder);
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
    machine.Regs().SetSpecial(rR, rr);
}

void Divi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Addu::Addu() : Instruction(ADDU)
{
}

void Addu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = yy + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Addu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Addui::Addui() : Instruction(ADDUI)
{
}

void Addui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = yy + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Addui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Subu::Subu() : Instruction(SUBU)
{
}

void Subu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = yy - zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Subu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Subui::Subui() : Instruction(SUBUI)
{
}

void Subui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = yy - zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Subui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Mulu::Mulu() : Instruction(MULU)
{
}

void Mulu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    boost::multiprecision::uint128_t yv = yy;
    uint64_t zz = machine.Regs().Get(z);
    boost::multiprecision::uint128_t zv = zz;
    boost::multiprecision::uint128_t value = yv * zv;
    uint64_t xx = static_cast<uint64_t>(value);
    uint64_t hh = static_cast<uint64_t>(value >> 64);
    machine.Regs().Set(x, xx);
    machine.Regs().SetSpecial(rH, hh);
}

void Mulu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Mului::Mului() : Instruction(MULUI)
{
}

void Mului::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    boost::multiprecision::uint128_t yv = yy;
    uint64_t zz = z;
    boost::multiprecision::uint128_t zv = zz;
    boost::multiprecision::uint128_t value = yv * zv;
    uint64_t xx = static_cast<uint64_t>(value);
    uint64_t hh = static_cast<uint64_t>(value >> 64);
    machine.Regs().Set(x, xx);
    machine.Regs().SetSpecial(rH, hh);
}

void Mului::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Divu::Divu() : Instruction(DIVU)
{
}

void Divu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t rd = machine.Regs().GetSpecial(rD);
    uint64_t xx = 0;
    uint64_t rr = 0;
    if (rd >= zz)
    {
        xx = rd;
        rr = yy;
    }
    else
    {
        boost::multiprecision::uint128_t yv = yy;
        boost::multiprecision::uint128_t dv = rd;
        boost::multiprecision::uint128_t dd = (dv << 64) | yv;
        boost::multiprecision::uint128_t value = dd / zz;
        xx = static_cast<uint64_t>(value);
        rr = static_cast<uint64_t>(dd % zz);
    }
    machine.Regs().Set(x, xx);
    machine.Regs().SetSpecial(rR, rr);
}

void Divu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Divui::Divui() : Instruction(DIVUI)
{
}

void Divui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t rd = machine.Regs().GetSpecial(rD);
    uint64_t xx = 0;
    uint64_t rr = 0;
    if (rd >= zz)
    {
        xx = rd;
        rr = yy;
    }
    else
    {
        boost::multiprecision::uint128_t yv = yy;
        boost::multiprecision::uint128_t dv = rd;
        boost::multiprecision::uint128_t dd = (dv << 64) | yv;
        boost::multiprecision::uint128_t value = dd / zz;
        xx = static_cast<uint64_t>(value);
        rr = static_cast<uint64_t>(dd % zz);
    }
    machine.Regs().Set(x, xx);
    machine.Regs().SetSpecial(rR, rr);

}

void Divui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I2Addu::I2Addu() : Instruction(I2ADDU)
{
}

void I2Addu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = (yy << 1) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I2Addu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I2Addui::I2Addui() : Instruction(I2ADDUI)
{
}

void I2Addui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = (yy << 1) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I2Addui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I4Addu::I4Addu() : Instruction(I4ADDU)
{
}

void I4Addu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = (yy << 2) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I4Addu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I4Addui::I4Addui() : Instruction(I4ADDUI)
{
}

void I4Addui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = (yy << 2) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I4Addui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I8Addu::I8Addu() : Instruction(I8ADDU)
{
}

void I8Addu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = (yy << 3) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I8Addu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I8Addui::I8Addui() : Instruction(I8ADDUI)
{
}

void I8Addui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = (yy << 3) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I8Addui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I16Addu::I16Addu() : Instruction(I16ADDU)
{
}

void I16Addu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = (yy << 4) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I16Addu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

I16Addui::I16Addui() : Instruction(I16ADDUI)
{
}

void I16Addui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = (yy << 4) + zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void I16Addui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Neg::Neg() : Instruction(NEG)
{
}

void Neg::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = y;
    int64_t zz = static_cast<int64_t>(machine.Regs().Get(z));
    int64_t value = yy - zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Neg::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Negi::Negi() : Instruction(NEGI)
{
}

void Negi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = y;
    int64_t zz = z;
    int64_t value = yy - zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Negi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Negu::Negu() : Instruction(NEGU)
{
}

void Negu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = y;
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = yy - zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Negu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Negui::Negui() : Instruction(NEGUI)
{
}

void Negui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = y;
    uint64_t zz = z;
    uint64_t value = yy - zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Negui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sl::Sl() : Instruction(SL)
{
}

void Sl::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = std::abs(static_cast<int64_t>(machine.Regs().Get(z)));
    int64_t value = yy << zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Sl::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sli::Sli() : Instruction(SLI)
{
}

void Sli::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = z;
    int64_t value = yy << zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Sli::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Slu::Slu() : Instruction(SLU)
{
}

void Slu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = yy << zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Slu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sluí::Sluí() : Instruction(SLUI)
{
}

void Sluí::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = yy << zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Sluí::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sr::Sr() : Instruction(SR)
{
}

void Sr::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = std::abs(static_cast<int64_t>(machine.Regs().Get(z)));
    int64_t value = yy >> zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Sr::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sri::Sri() : Instruction(SRI)
{
}

void Sri::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = z;
    int64_t value = yy >> zz;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Sri::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sru::Sru() : Instruction(SRU)
{
}

void Sru::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t value = yy >> zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Sru::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Srui::Srui() : Instruction(SRUI)
{
}

void Srui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t value = yy >> zz;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Srui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Cmp::Cmp() : Instruction(CMP)
{
}

void Cmp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = static_cast<int64_t>(machine.Regs().Get(z));
    int64_t gr = static_cast<int64_t>(yy > zz);
    int64_t ls = static_cast<int64_t>(yy < zz);
    int64_t value = gr - ls;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Cmp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Cmpi::Cmpi() : Instruction(CMPI)
{
}

void Cmpi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    int64_t zz = z;
    int64_t gr = static_cast<int64_t>(yy > zz);
    int64_t ls = static_cast<int64_t>(yy < zz);
    int64_t value = gr - ls;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Cmpi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Cmpu::Cmpu() : Instruction(CMPU)
{
}

void Cmpu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    int64_t gr = static_cast<int64_t>(yy > zz);
    int64_t ls = static_cast<int64_t>(yy < zz);
    int64_t value = gr - ls;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Cmpu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Cmpui::Cmpui() : Instruction(CMPUI)
{
}

void Cmpui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    int64_t gr = static_cast<int64_t>(yy > zz);
    int64_t ls = static_cast<int64_t>(yy < zz);
    int64_t value = gr - ls;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Cmpui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csn::Csn() : Instruction(CSN)
{
}

void Csn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy < 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csni::Csni() : Instruction(CSNI)
{
}

void Csni::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy < 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csni::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csz::Csz() : Instruction(CSZ)
{
}

void Csz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if (yy == 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Cszi::Cszi() : Instruction(CSZI)
{
}

void Cszi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if (yy == 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Cszi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csp::Csp() : Instruction(CSP)
{
}

void Csp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy > 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Cspi::Cspi() : Instruction(CSPI)
{
}

void Cspi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy > 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Cspi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csod::Csod() : Instruction(CSOD)
{
}

void Csod::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if (yy & 1)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csod::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csodi::Csodi() : Instruction(CSODI)
{
}

void Csodi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if (yy & 1)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csodi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csnn::Csnn() : Instruction(CSNN)
{
}

void Csnn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy >= 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csnn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csnni::Csnni() : Instruction(CSNNI)
{
}

void Csnni::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy >= 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csnni::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csnz::Csnz() : Instruction(CSNZ)
{
}

void Csnz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if (yy != 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csnz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csnzi::Csnzi() : Instruction(CSNZI)
{
}

void Csnzi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if (yy != 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csnzi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csnp::Csnp() : Instruction(CSNP)
{
}

void Csnp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy <= 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csnp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csnpi::Csnpi() : Instruction(CSNPI)
{
}

void Csnpi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    if (yy <= 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csnpi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csev::Csev() : Instruction(CSEV)
{
}

void Csev::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if ((yy & 1) == 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csev::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Csevi::Csevi() : Instruction(CSEVI)
{
}

void Csevi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    if ((yy & 1) == 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
        machine.Regs().Set(x, xx);
    }
}

void Csevi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsn::Zsn() : Instruction(ZSN)
{
}

void Zsn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy < 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsni::Zsni() : Instruction(ZSNI)
{
}

void Zsni::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy < 0)
    {
        uint64_t zz = z;
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsni::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsz::Zsz() : Instruction(ZSZ)
{
}

void Zsz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if (yy == 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zszi::Zszi() : Instruction(ZSZI)
{
}

void Zszi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if (yy == 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zszi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsp::Zsp() : Instruction(ZSP)
{
}

void Zsp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy > 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zspi::Zspi() : Instruction(ZSPI)
{
}

void Zspi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy > 0)
    {
        uint64_t zz = z;
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zspi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsod::Zsod() : Instruction(ZSOD)
{
}

void Zsod::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if (yy & 1)
    {
        uint64_t zz = machine.Regs().Get(z);
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsod::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsodi::Zsodi() : Instruction(ZSODI)
{
}

void Zsodi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if (yy & 1)
    {
        uint64_t zz = z;
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsodi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsnn::Zsnn() : Instruction(ZSNN)
{
}

void Zsnn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy >= 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsnn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsnni::Zsnni() : Instruction(ZSNNI)
{
}

void Zsnni::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy >= 0)
    {
        uint64_t zz = z;
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsnni::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsnz::Zsnz() : Instruction(ZSNZ)
{
}

void Zsnz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if (yy != 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        uint64_t xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsnz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsnzi::Zsnzi() : Instruction(ZSNZI)
{
}

void Zsnzi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if (yy != 0)
    {
        uint64_t zz = z;
        uint64_t xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsnzi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsnp::Zsnp() : Instruction(ZSNP)
{
}

void Zsnp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy <= 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsnp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsnpi::Zsnpi() : Instruction(ZSNPI)
{
}

void Zsnpi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t yy = static_cast<int64_t>(machine.Regs().Get(y));
    uint64_t xx = 0;
    if (yy <= 0)
    {
        uint64_t zz = z;
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsnpi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsev::Zsev() : Instruction(ZSEV)
{
}

void Zsev::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if ((yy & 1) == 0)
    {
        uint64_t zz = machine.Regs().Get(z);
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsev::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Zsevi::Zsevi() : Instruction(ZSEVI)
{
}

void Zsevi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t xx = 0;
    if ((yy & 1) == 0)
    {
        uint64_t zz = z;
        xx = zz;
    }
    machine.Regs().Set(x, xx);
}

void Zsevi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

And::And() : Instruction(AND)
{
}

void And::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = yy & zz;
    machine.Regs().Set(x, xx);
}

void And::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Andi::Andi() : Instruction(ANDI)
{
}

void Andi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = yy & zz;
    machine.Regs().Set(x, xx);
}

void Andi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Or::Or() : Instruction(OR)
{
}

void Or::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = yy | zz;
    machine.Regs().Set(x, xx);
}

void Or::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ori::Ori() : Instruction(ORI)
{
}

void Ori::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = yy | zz;
    machine.Regs().Set(x, xx);
}

void Ori::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Xor::Xor() : Instruction(XOR)
{
}

void Xor::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = yy ^ zz;
    machine.Regs().Set(x, xx);
}

void Xor::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Xori::Xori() : Instruction(XORI)
{
}

void Xori::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = yy ^ zz;
    machine.Regs().Set(x, xx);
}

void Xori::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Andn::Andn() : Instruction(ANDN)
{
}

void Andn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = yy & ~zz;
    machine.Regs().Set(x, xx);
}

void Andn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Andni::Andni() : Instruction(ANDNI)
{
}

void Andni::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = yy & ~zz;
    machine.Regs().Set(x, xx);
}

void Andni::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Orn::Orn() : Instruction(ORN)
{
}

void Orn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = yy | ~zz;
    machine.Regs().Set(x, xx);
}

void Orn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Orni::Orni() : Instruction(ORNI)
{
}

void Orni::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = yy | ~zz;
    machine.Regs().Set(x, xx);
}

void Orni::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Nand::Nand() : Instruction(NAND)
{
}

void Nand::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = ~(yy & zz);
    machine.Regs().Set(x, xx);
}

void Nand::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Nandi::Nandi() : Instruction(NANDI)
{
}

void Nandi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = ~(yy & zz);
    machine.Regs().Set(x, xx);
}

void Nandi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Nor::Nor() : Instruction(NOR)
{
}

void Nor::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = ~(yy | zz);
    machine.Regs().Set(x, xx);
}

void Nor::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Nori::Nori() : Instruction(NORI)
{
}

void Nori::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = ~(yy | zz);
    machine.Regs().Set(x, xx);
}

void Nori::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Nxor::Nxor() : Instruction(NXOR)
{
}

void Nxor::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = ~(yy ^ zz);
    machine.Regs().Set(x, xx);
}

void Nxor::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Nxori::Nxori() : Instruction(NXORI)
{
}

void Nxori::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = ~(yy ^ zz);
    machine.Regs().Set(x, xx);
}

void Nxori::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Mux::Mux() : Instruction(MUX)
{
}

void Mux::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t mm = machine.Regs().GetSpecial(rM);
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = (yy & mm) | (zz & ~mm);
    machine.Regs().Set(x, xx);
}

void Mux::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Muxi::Muxi() : Instruction(MUXI)
{
}

void Muxi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t mm = machine.Regs().GetSpecial(rM);
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = (yy & mm) | (zz & ~mm);
    machine.Regs().Set(x, xx);
}

void Muxi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sadd::Sadd() : Instruction(SADD)
{
}

void Sadd::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = BitCount(yy & ~zz);
    machine.Regs().Set(x, xx);
}

void Sadd::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Saddi::Saddi() : Instruction(SADDI)
{
}

void Saddi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = BitCount(yy & ~zz);
    machine.Regs().Set(x, xx);
}

void Saddi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bdif::Bdif() : Instruction(BDIF)
{
}

void Bdif::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = 0;
    for (int i = 0; i < 8; ++i)
    {
        uint8_t yv = static_cast<uint8_t>(yy);
        uint8_t zv = static_cast<uint8_t>(zz);
        uint8_t xv = SaturatingSubtraction(yv, zv);
        xx = xx | (static_cast<uint64_t>(xv) | static_cast<uint64_t>(i << 3));
        yy = yy >> 8;
        zz = zz >> 8;
    }
    machine.Regs().Set(x, xx);
}

void Bdif::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bdifi::Bdifi() : Instruction(BDIFI)
{
}

void Bdifi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = 0;
    for (int i = 0; i < 8; ++i)
    {
        uint8_t yv = static_cast<uint8_t>(yy);
        uint8_t zv = static_cast<uint8_t>(zz);
        uint8_t xv = SaturatingSubtraction(yv, zv);
        xx = xx | (static_cast<uint64_t>(xv) | static_cast<uint64_t>(i << 3));
        yy = yy >> 8;
        zz = zz >> 8;
    }
    machine.Regs().Set(x, xx);
}

void Bdifi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Wdif::Wdif() : Instruction(WDIF)
{
}

void Wdif::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = 0;
    for (int i = 0; i < 4; ++i)
    {
        uint16_t yv = static_cast<uint16_t>(yy);
        uint16_t zv = static_cast<uint16_t>(zz);
        uint16_t xv = SaturatingSubtraction(yv, zv);
        xx = xx | (static_cast<uint64_t>(xv) | static_cast<uint64_t>(i << 4));
        yy = yy >> 16;
        zz = zz >> 16;
    }
    machine.Regs().Set(x, xx);
}

void Wdif::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Wdifi::Wdifi() : Instruction(WDIFI)
{
}

void Wdifi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = 0;
    for (int i = 0; i < 4; ++i)
    {
        uint16_t yv = static_cast<uint16_t>(yy);
        uint16_t zv = static_cast<uint16_t>(zz);
        uint16_t xv = SaturatingSubtraction(yv, zv);
        xx = xx | (static_cast<uint64_t>(xv) | static_cast<uint64_t>(i << 4));
        yy = yy >> 16;
        zz = zz >> 16;
    }
    machine.Regs().Set(x, xx);
}

void Wdifi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Tdif::Tdif() : Instruction(TDIF)
{
}

void Tdif::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = 0;
    for (int i = 0; i < 2; ++i)
    {
        uint32_t yv = static_cast<uint32_t>(yy);
        uint32_t zv = static_cast<uint32_t>(zz);
        uint32_t xv = SaturatingSubtraction(yv, zv);
        xx = xx | (static_cast<uint64_t>(xv) | static_cast<uint64_t>(i << 5));
        yy = yy >> 32;
        zz = zz >> 32;
    }
    machine.Regs().Set(x, xx);
}

void Tdif::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Tdifi::Tdifi() : Instruction(TDIFI)
{
}

void Tdifi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = 0;
    for (int i = 0; i < 2; ++i)
    {
        uint32_t yv = static_cast<uint32_t>(yy);
        uint32_t zv = static_cast<uint32_t>(zz);
        uint32_t xv = SaturatingSubtraction(yv, zv);
        xx = xx | (static_cast<uint64_t>(xv) | static_cast<uint64_t>(i << 5));
        yy = yy >> 32;
        zz = zz >> 32;
    }
    machine.Regs().Set(x, xx);
}

void Tdifi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Odif::Odif() : Instruction(ODIF)
{
}

void Odif::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t xx = SaturatingSubtraction(yy, zz);
    machine.Regs().Set(x, xx);
}

void Odif::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Odifi::Odifi() : Instruction(ODIFI)
{
}

void Odifi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t xx = SaturatingSubtraction(yy, zz);
    machine.Regs().Set(x, xx);
}

void Odifi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fadd::Fadd() : Instruction(FADD)
{
}

void Fadd::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    double yv = *static_cast<double*>(static_cast<void*>(&yy));
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    double value = yv + zv;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Fadd::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fsub::Fsub() : Instruction(FSUB)
{
}

void Fsub::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    double yv = *static_cast<double*>(static_cast<void*>(&yy));
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    double value = yv - zv;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Fsub::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fmul::Fmul() : Instruction(FMUL)
{
}

void Fmul::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    double yv = *static_cast<double*>(static_cast<void*>(&yy));
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    double value = yv * zv;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Fmul::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fdiv::Fdiv() : Instruction(FDIV)
{
}

void Fdiv::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    double yv = *static_cast<double*>(static_cast<void*>(&yy));
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    double value = yv / zv;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Fdiv::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Frem::Frem() : Instruction(FREM)
{
}

void Frem::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    double yv = *static_cast<double*>(static_cast<void*>(&yy));
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    double value = fmod(yv, zv);
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Frem::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fsqrt::Fsqrt() : Instruction(FSQRT)
{
}

void Fsqrt::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    double value = std::sqrt(zz);
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Fsqrt::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fint::Fint() : Instruction(FINT)
{
}

void Fint::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    double value = static_cast<uint64_t>(zv);
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Fint::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fcmp::Fcmp() : Instruction(FCMP)
{
}

void Fcmp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    double yv = *static_cast<double*>(static_cast<void*>(&yy));
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    int64_t gr = yv > zv;
    int64_t ls = yv < zv;
    int64_t value = gr - ls;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Fcmp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Feql::Feql() : Instruction(FEQL)
{
}

void Feql::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    double yv = *static_cast<double*>(static_cast<void*>(&yy));
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    bool value = yv == zv;
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Feql::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fix::Fix() : Instruction(FIX)
{
}

void Fix::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    int64_t value = static_cast<int64_t>(zv);
    uint64_t xx = static_cast<uint64_t>(value);
    machine.Regs().Set(x, xx);
}

void Fix::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Fixu::Fixu() : Instruction(FIXU)
{
}

void Fixu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = machine.Regs().Get(z);
    double zv = *static_cast<double*>(static_cast<void*>(&zz));
    uint64_t value = static_cast<uint64_t>(zv);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Fixu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Flot::Flot() : Instruction(FLOT)
{
}

void Flot::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t zz = static_cast<uint64_t>(machine.Regs().Get(z));
    double value = zz;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Flot::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Floti::Floti() : Instruction(FLOTI)
{
}

void Floti::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t zz = z;
    double value = zz;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Floti::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Flotu::Flotu() : Instruction(FLOTU)
{
}

void Flotu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = machine.Regs().Get(z);
    double value = zz;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Flotu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Flotui::Flotui() : Instruction(FLOTUI)
{
}

void Flotui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = z;
    double value = zz;
    uint64_t xx = *static_cast<uint64_t*>(static_cast<void*>(&value));
    machine.Regs().Set(x, xx);
}

void Flotui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sflot::Sflot() : Instruction(SFLOT)
{
}

void Sflot::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t zz = static_cast<uint64_t>(machine.Regs().Get(z));
    float value = static_cast<float>(zz);
    uint32_t vv = *static_cast<uint32_t*>(static_cast<void*>(&value));
    uint64_t xx = vv;
    machine.Regs().Set(x, xx);
}

void Sflot::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sfloti::Sfloti() : Instruction(SFLOTI)
{
}

void Sfloti::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t zz = z;
    float value = static_cast<float>(zz);
    uint32_t vv = *static_cast<uint32_t*>(static_cast<void*>(&value));
    uint64_t xx = vv;
    machine.Regs().Set(x, xx);
}

void Sfloti::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sflotu::Sflotu() : Instruction(SFLOTU)
{
}

void Sflotu::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = machine.Regs().Get(z);
    float value = static_cast<float>(zz);
    uint32_t vv = *static_cast<uint32_t*>(static_cast<void*>(&value));
    uint64_t xx = vv;
    machine.Regs().Set(x, xx);
}

void Sflotu::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Sflotui::Sflotui() : Instruction(SFLOTUI)
{
}

void Sflotui::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = z;
    float value = static_cast<float>(zz);
    uint32_t vv = *static_cast<uint32_t*>(static_cast<void*>(&value));
    uint64_t xx = vv;
    machine.Regs().Set(x, xx);
}

void Sflotui::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Seth::Seth() : Instruction(SETH)
{
}

void Seth::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 48;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Seth::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Setmh::Setmh() : Instruction(SETMH)
{
}

void Setmh::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 32;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Setmh::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Setml::Setml() : Instruction(SETML)
{
}

void Setml::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz) << 16;
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Setml::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Setl::Setl() : Instruction(SETL)
{
}

void Setl::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t value = static_cast<uint64_t>(yz);
    uint64_t xx = value;
    machine.Regs().Set(x, xx);
}

void Setl::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Inch::Inch() : Instruction(INCH)
{
}

void Inch::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx + (static_cast<uint64_t>(yz) << 48);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Inch::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Incmh::Incmh() : Instruction(INCMH)
{
}

void Incmh::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx + (static_cast<uint64_t>(yz) << 32);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Incmh::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Incml::Incml() : Instruction(INCML)
{
}

void Incml::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx + (static_cast<uint64_t>(yz) << 16);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Incml::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Incl::Incl() : Instruction(INCL)
{
}

void Incl::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx + static_cast<uint64_t>(yz);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Incl::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Orh::Orh() : Instruction(ORH)
{
}

void Orh::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx | (static_cast<uint64_t>(yz) << 48);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Orh::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ormh::Ormh() : Instruction(ORMH)
{
}

void Ormh::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx | (static_cast<uint64_t>(yz) << 32);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Ormh::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Orml::Orml() : Instruction(ORML)
{
}

void Orml::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx | (static_cast<uint64_t>(yz) << 16);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Orml::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Orl::Orl() : Instruction(ORL)
{
}

void Orl::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx | static_cast<uint64_t>(yz);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Orl::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Andnh::Andnh() : Instruction(ANDNH)
{
}

void Andnh::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx & ~(static_cast<uint64_t>(yz) << 48);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Andnh::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Andnmh::Andnmh() : Instruction(ANDNMH)
{
}

void Andnmh::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx & ~(static_cast<uint64_t>(yz) << 32);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Andnmh::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Andnml::Andnml() : Instruction(ANDNML)
{
}

void Andnml::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx & ~(static_cast<uint64_t>(yz) << 16);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Andnml::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Andnl::Andnl() : Instruction(ANDNL)
{
}

void Andnl::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint16_t yz = (static_cast<uint16_t>(y) << 8) | (static_cast<uint16_t>(z));
    uint64_t xx = machine.Regs().Get(x);
    uint64_t value = xx & ~static_cast<uint64_t>(yz);
    xx = value;
    machine.Regs().Set(x, xx);
}

void Andnl::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Jmp::Jmp() : Instruction(JMP)
{
}

void Jmp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t ra = ((static_cast<uint64_t>(x) << 16) | (static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + ra;
    machine.Regs().SetPC(to);
}

void Jmp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Jmpb::Jmpb() : Instruction(JMPB)
{
}

void Jmpb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t ra = ((static_cast<uint64_t>(x) << 16) | (static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc - ra;
    machine.Regs().SetPC(to);
}

void Jmpb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Go::Go() : Instruction(GO)
{
}

void Go::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t to = yy + zz;
    uint64_t pc = machine.Regs().GetPC();
    uint64_t xx = pc + 4;
    machine.Regs().SetPC(to);
    machine.Regs().Set(x, xx);
}

void Go::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Goi::Goi() : Instruction(GOI)
{
}

void Goi::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t to = yy + zz;
    uint64_t pc = machine.Regs().GetPC();
    uint64_t xx = pc + 4;
    machine.Regs().SetPC(to);
    machine.Regs().Set(x, xx);
}

void Goi::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Call::Call() : Instruction(CALL)
{
}

void Call::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = machine.Regs().Get(z);
    uint64_t to = yy + zz;
    uint64_t pc = machine.Regs().GetPC();
    uint64_t ret = pc + 4;
    uint64_t sp = machine.Regs().Get(regSP);
    Memory& mem = machine.Mem();
    for (int i = 0; i < x; ++i)
    {
        mem.WriteOcta(sp, machine.Regs().Get(i), Protection::write);
        sp = sp + 8;
    }
    uint64_t xx = x;
    mem.WriteOcta(sp, xx, Protection::write);
    sp = sp + 8;
    mem.WriteOcta(sp, machine.Regs().GetSpecial(rL), Protection::write);
    sp = sp + 8;
    mem.WriteOcta(sp, ret, Protection::write);
    sp = sp + 8;
    machine.Regs().Set(regSP, sp);
    machine.Regs().SetPC(to);
    machine.Regs().SetSpecial(rL, 0);
}

void Call::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Calli::Calli() : Instruction(CALLI)
{
}

void Calli::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t yy = machine.Regs().Get(y);
    uint64_t zz = z;
    uint64_t to = yy + zz;
    uint64_t pc = machine.Regs().GetPC();
    uint64_t ret = pc + 4;
    uint64_t sp = machine.Regs().Get(regSP);
    Memory& mem = machine.Mem();
    for (int i = 0; i < x; ++i)
    {
        mem.WriteOcta(sp, machine.Regs().Get(i), Protection::write);
        sp = sp + 8;
    }
    uint64_t xx = x;
    mem.WriteOcta(sp, xx, Protection::write);
    sp = sp + 8;
    mem.WriteOcta(sp, machine.Regs().GetSpecial(rL), Protection::write);
    sp = sp + 8;
    mem.WriteOcta(sp, ret, Protection::write);
    sp = sp + 8;
    machine.Regs().Set(regSP, sp);
    machine.Regs().SetPC(to);
    machine.Regs().SetSpecial(rL, 0);
}

void Calli::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Ret::Ret() : Instruction(RET)
{
}

void Ret::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    Memory& mem = machine.Mem();
    uint64_t sp = machine.Regs().Get(regSP);
    sp = sp - 8;
    uint64_t ret = mem.ReadOcta(sp, Protection::read);
    sp = sp - 8;
    uint64_t rl = mem.ReadOcta(sp, Protection::read);
    machine.Regs().SetSpecial(rL, rl);
    sp = sp - 8;
    int n = static_cast<int>(mem.ReadOcta(sp, Protection::read));
    for (int i = n - 1; i >= 0; --i)
    {
        sp = sp - 8;
        uint64_t rr = mem.ReadOcta(sp, Protection::read);
        machine.Regs().Set(static_cast<uint8_t>(i), rr);
    }
    machine.Regs().Set(regSP, sp);
    machine.Regs().SetPC(ret);
}

void Ret::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bn::Bn() : Instruction(BN)
{
}

void Bn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx < 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bnb::Bnb() : Instruction(BNB)
{
}

void Bnb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx < 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bnb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bz::Bz() : Instruction(BZ)
{
}

void Bz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bzb::Bzb() : Instruction(BZB)
{
}

void Bzb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bzb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bp::Bp() : Instruction(BP)
{
}

void Bp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx > 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bpb::Bpb() : Instruction(BPB)
{
}

void Bpb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx > 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bpb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bod::Bod() : Instruction(BOD)
{
}

void Bod::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx & 1)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bod::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bodb::Bodb() : Instruction(BODB)
{
}

void Bodb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx & 1)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bodb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bnn::Bnn() : Instruction(BNN)
{
}

void Bnn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx >= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bnn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bnnb::Bnnb() : Instruction(BNNB)
{
}

void Bnnb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx >= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bnnb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bnz::Bnz() : Instruction(BNZ)
{
}

void Bnz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx != 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bnz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bnzb::Bnzb() : Instruction(BNZB)
{
}

void Bnzb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx != 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bnzb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bnp::Bnp() : Instruction(BNP)
{
}

void Bnp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx <= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bnp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bnpb::Bnpb() : Instruction(BNPB)
{
}

void Bnpb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx <= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bnpb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bev::Bev() : Instruction(BEV)
{
}

void Bev::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if ((xx & 1) == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Bev::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Bevb::Bevb() : Instruction(BEVB)
{
}

void Bevb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if ((xx & 1) == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Bevb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbn::Pbn() : Instruction(PBN)
{
}

void Pbn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx < 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbnb::Pbnb() : Instruction(PBNB)
{
}

void Pbnb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx < 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbnb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbz::Pbz() : Instruction(PBZ)
{
}

void Pbz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbzb::Pbzb() : Instruction(PBZB)
{
}

void Pbzb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbzb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbp::Pbp() : Instruction(PBP)
{
}

void Pbp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx > 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbpb::Pbpb() : Instruction(PBPB)
{
}

void Pbpb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx > 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbpb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbod::Pbod() : Instruction(PBOD)
{
}

void Pbod::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx & 1)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbod::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbodb::Pbodb() : Instruction(PBODB)
{
}

void Pbodb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx & 1)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbodb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbnn::Pbnn() : Instruction(PBNN)
{
}

void Pbnn::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx >= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbnn::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbnnb::Pbnnb() : Instruction(PBNNB)
{
}

void Pbnnb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx >= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbnnb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbnz::Pbnz() : Instruction(PBNZ)
{
}

void Pbnz::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx != 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbnz::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbnzb::Pbnzb() : Instruction(PBNZB)
{
}

void Pbnzb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx != 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbnzb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbnp::Pbnp() : Instruction(PBNP)
{
}

void Pbnp::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx <= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbnp::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbnpb::Pbnpb() : Instruction(PBNPB)
{
}

void Pbnpb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    int64_t xx = static_cast<int64_t>(machine.Regs().Get(x));
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if (xx <= 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbnpb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbev::Pbev() : Instruction(PBEV)
{
}

void Pbev::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if ((xx & 1) == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc + ra;
    }
    machine.Regs().SetPC(to);
}

void Pbev::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Pbevb::Pbevb() : Instruction(PBEVB)
{
}

void Pbevb::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().Get(x);
    uint64_t pc = machine.Regs().GetPC();
    uint64_t to = pc + 4;
    if ((xx & 1) == 0)
    {
        uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
        to = pc - ra;
    }
    machine.Regs().SetPC(to);
}

void Pbevb::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Get::Get() : Instruction(GET)
{
}

void Get::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t xx = machine.Regs().GetSpecial(z);
    machine.Regs().Set(x, xx);
}

void Get::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Put::Put() : Instruction(PUT)
{
}

void Put::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t zz = machine.Regs().Get(z);
    machine.Regs().SetSpecial(x, zz);
}

void Put::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Geta::Geta() : Instruction(GETA)
{
}

void Geta::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t pc = machine.Regs().GetPC();
    uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    uint64_t to = to = pc + ra;
    uint64_t xx = to;
    machine.Regs().Set(x, xx);
}

void Geta::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

Getab::Getab() : Instruction(GETAB)
{
}

void Getab::Execute(Machine& machine, uint8_t x, uint8_t y, uint8_t z)
{
    uint64_t pc = machine.Regs().GetPC();
    uint64_t ra = ((static_cast<uint64_t>(y) << 8) | static_cast<uint64_t>(z)) << static_cast<uint64_t>(2);
    uint64_t to = to = pc - ra;
    uint64_t xx = to;
    machine.Regs().Set(x, xx);
}

void Getab::Format(Formatter& formatter, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Format(*this, x, y, z);
}

} // cmsx::machine
