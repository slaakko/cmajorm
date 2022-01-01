// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_FORMATTER_INCLUDED
#define CMSX_MACHINE_FORMATTER_INCLUDED
#include <system-x/machine/Api.hpp>
#include <stdint.h>

namespace cmsx::machine {

class Trap;
class Swym;
class Ldb;
class Ldbi;
class Ldw;
class Ldwi;
class Ldt;
class Ldti;
class Ldo;
class Ldoi;
class Ldbu;
class Ldbui;
class Ldwu;
class Ldwui;
class Ldtu;
class Ldtui;
class Ldou;
class Ldoui;
class Ldht;
class Ldhti;
class Ldsf;
class Ldsfi;
class Stb;
class Stbi;
class Stw;
class Stwi;
class Stt;
class Stti;
class Sto;
class Stoi;
class Stbu;
class Stbui;
class Stwu;
class Stwui;
class Sttu;
class Sttui;
class Stou;
class Stoui;
class Stht;
class Sthti;
class Stsf;
class Stsfi;
class Stco;
class Stcoi;
class Add;
class Addi;
class Sub;
class Subi;
class Mul;
class Muli;
class Div;
class Divi;
class Addu;
class Addui;
class Subu;
class Subui;
class Mulu;
class Mului;
class Divu;
class Divui;
class I2Addu;
class I2Addui;
class I4Addu;
class I4Addui;
class I8Addu;
class I8Addui;
class I16Addu;
class I16Addui;
class Neg;
class Negi;
class Negu;
class Negui;
class Sl;
class Sli;
class Slu;
class Sluí;
class Sr;
class Sri;
class Sru;
class Srui;
class Cmp;
class Cmpi;
class Cmpu;
class Cmpui;
class Csn;
class Csni;
class Csz;
class Cszi;
class Csp;
class Cspi;
class Csod;
class Csodi;
class Csnn;
class Csnni;
class Csnz;
class Csnzi;
class Csnp;
class Csnpi;
class Csev;
class Csevi;
class Zsn;
class Zsni;
class Zsz;
class Zszi;
class Zsp;
class Zspi;
class Zsod;
class Zsodi;
class Zsnn;
class Zsnni;
class Zsnz;
class Zsnzi;
class Zsnp;
class Zsnpi;
class Zsev;
class Zsevi;
class And;
class Andi;
class Or;
class Ori;
class Xor;
class Xori;
class Andn;
class Andni;
class Orn;
class Orni;
class Nand;
class Nandi;
class Nor;
class Nori;
class Nxor;
class Nxori;
class Mux;
class Muxi;
class Sadd;
class Saddi;
class Bdif;
class Bdifi;
class Wdif;
class Wdifi;
class Tdif;
class Tdifi;
class Odif;
class Odifi;
class Fadd;
class Fsub;
class Fmul;
class Fdiv;
class Frem;
class Fsqrt;
class Fint;
class Fcmp;
class Feql;
class Fix;
class Fixu;
class Flot;
class Floti;
class Flotu;
class Flotui;
class Sflot;
class Sfloti;
class Sflotu;
class Sflotui;
class Seth;
class Setmh;
class Setml;
class Setl;
class Inch;
class Incmh;
class Incml;
class Incl;
class Orh;
class Ormh;
class Orml;
class Orl;
class Andnh;
class Andnmh;
class Andnml;
class Andnl;
class Jmp;
class Jmpb;
class Go;
class Goi;
class Call;
class Calli;
class Ret;
class Bn;
class Bnb;
class Bz;
class Bzb;
class Bp;
class Bpb;
class Bod;
class Bodb;
class Bnn;
class Bnnb;
class Bnz;
class Bnzb;
class Bnp;
class Bnpb;
class Bev;
class Bevb;
class Pbn;
class Pbnb;
class Pbz;
class Pbzb;
class Pbp;
class Pbpb;
class Pbod;
class Pbodb;
class Pbnn;
class Pbnnb;
class Pbnz;
class Pbnzb;
class Pbnp;
class Pbnpb;
class Pbev;
class Pbevb;
class Get;
class Put;
class Geta;
class Getab;

class CMSX_MACHINE_API Formatter
{
public:
    virtual ~Formatter();
    virtual void Format(Trap& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Swym& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldbi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldw& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldwi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldt& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldti& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldo& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldoi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldbu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldbui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldwu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldwui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldtu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldtui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldou& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldoui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldht& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldhti& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldsf& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ldsfi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stbi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stw& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stwi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stt& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stti& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sto& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stoi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stbu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stbui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stwu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stwui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sttu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sttui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stou& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stoui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stht& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sthti& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stsf& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stsfi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stco& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Stcoi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Add& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Addi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sub& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Subi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Mul& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Muli& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Div& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Divi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Addu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Addui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Subu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Subui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Mulu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Mului& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Divu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Divui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I2Addu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I2Addui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I4Addu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I4Addui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I8Addu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I8Addui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I16Addu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(I16Addui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Neg& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Negi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Negu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Negui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sl& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sli& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Slu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sluí& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sr& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sri& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sru& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Srui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Cmp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Cmpi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Cmpu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Cmpui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csni& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Cszi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Cspi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csod& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csodi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csnn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csnni& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csnz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csnzi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csnp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csnpi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csev& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Csevi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsni& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zszi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zspi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsod& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsodi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsnn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsnni& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsnz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsnzi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsnp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsnpi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsev& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Zsevi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(And& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Andi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Or& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ori& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Xor& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Xori& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Andn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Andni& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Orn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Orni& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Nand& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Nandi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Nor& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Nori& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Nxor& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Nxori& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Mux& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Muxi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sadd& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Saddi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bdif& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bdifi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Wdif& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Wdifi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Tdif& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Tdifi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Odif& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Odifi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fadd& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fsub& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fmul& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fdiv& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Frem& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fsqrt& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fint& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fcmp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Feql& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fix& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Fixu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Flot& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Floti& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Flotu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Flotui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sflot& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sfloti& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sflotu& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Sflotui& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Seth& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Setmh& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Setml& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Setl& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Inch& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Incmh& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Incml& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Incl& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Orh& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ormh& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Orml& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Orl& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Andnh& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Andnmh& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Andnml& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Andnl& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Jmp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Jmpb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Go& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Goi& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Call& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Calli& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Ret& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bnb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bzb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bpb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bod& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bodb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bnn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bnnb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bnz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bnzb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bnp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bnpb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bev& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Bevb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbnb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbzb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbpb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbod& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbodb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbnn& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbnnb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbnz& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbnzb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbnp& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbnpb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbev& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Pbevb& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Get& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Put& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Geta& inst, uint8_t x, uint8_t y, uint8_t z) {}
    virtual void Format(Getab& inst, uint8_t x, uint8_t y, uint8_t z) {}
};

} // cmsx::machine

#endif // CMSX_MACHINE_FORMATTER_INCLUDED
