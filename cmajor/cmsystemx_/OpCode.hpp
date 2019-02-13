// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_OPCODE_INCLUDED
#define CMAJOR_CMSX_OPCODE_INCLUDED
#include <cmajor/cmsystemx/CmSystemxApi.hpp>
#include <string>

namespace cmsystemx {

const int TRAP = 0x00;
const int FCMP = 0x01;
const int FUN = 0x02;
const int FEQL = 0x03;
const int FADD = 0x04;
const int FIX = 0x05;
const int FSUB = 0x06;
const int FIXU = 0x07;

const int FLOT = 0x08;
const int FLOTI = 0x09;
const int FLOTU = 0x0A;
const int FLOTUI = 0x0B;
const int SFLOT = 0x0C;
const int SFLOTI = 0x0D;
const int SFLOTU = 0x0E;
const int SFLOTUI = 0x0F;

const int FMUL = 0x10;
const int FCMPE = 0x11;
const int FUNE = 0x12;
const int FEQLE = 0x13;
const int FDIV = 0x14;
const int FSQRT = 0x15;
const int FREM = 0x16;
const int FINT = 0x17;

const int MUL = 0x18;
const int MULI = 0x19;
const int MULU = 0x1A;
const int MULUI = 0x1B;
const int DIV = 0x1C;
const int DIVI = 0x1D;
const int DIVU = 0x1E;
const int DIVUI = 0x1F;

const int ADD = 0x20;
const int ADDI = 0x21;
const int ADDU = 0x22;
const int ADDUI = 0x23;
const int SUB = 0x24;
const int SUBI = 0x25;
const int SUBU = 0x26;
const int SUBUI = 0x27;

const int I2ADDU = 0x28;
const int I2ADDUI = 0x29;
const int I4ADDU = 0x2A;
const int I4ADDUI = 0x2B;
const int I8ADDU = 0x2C;
const int I8ADDUI = 0x2D;
const int I16ADDU = 0x2E;
const int I16ADDUI = 0x2F;

const int CMP = 0x30;
const int CMPI = 0x31;
const int CMPU = 0x32;
const int CMPUI = 0x33;
const int NEG = 0x34;
const int NEGI = 0x35;
const int NEGU = 0x36;
const int NEGUI = 0x37;

const int SL = 0x38;
const int SLI = 0x39;
const int SLU = 0x3A;
const int SLUI = 0x3B;
const int SR = 0x03C;
const int SRI = 0x3D;
const int SRU = 0x3E;
const int SRUI = 0x3F;

const int BN = 0x40;
const int BNB = 0x41;
const int BZ = 0x42;
const int BZB = 0x43;
const int BP = 0x44;
const int BPB = 0x45;
const int BOD = 0x46;
const int BODB = 0x47;

const int BNN = 0x48;
const int BNNB = 0x49;
const int BNZ = 0x4A;
const int BNZB = 0x4B;
const int BNP = 0x4C;
const int BNPB = 0x4D;
const int BEV = 0x4E;
const int BEVB = 0x4F;

const int PBN = 0x50;
const int PBNB = 0x51;
const int PBZ = 0x52;
const int PBZB = 0x53;
const int PBP = 0x54;
const int PBPB = 0x55;
const int PBOD = 0x56;
const int PBODB = 0x57;

const int PBNN = 0x58;
const int PBNNB = 0x59;
const int PBNZ = 0x5A;
const int PBNZB = 0x5B;
const int PBNP = 0x5C;
const int PBNPB = 0x5D;
const int PBEV = 0x5E;
const int PBEVB = 0x5F;

const int CSN = 0x60;
const int CSNI = 0x61;
const int CSZ = 0x62;
const int CSZI = 0x63;
const int CSP = 0x64;
const int CSPI = 0x65;
const int CSOD = 0x66;
const int CSODI = 0x67;

const int CSNN = 0x68;
const int CSNNI = 0x69;
const int CSNZ = 0x6A;
const int CSNZI = 0x6B;
const int CSNP = 0x6C;
const int CSNPI = 0x6D;
const int CSEV = 0x6E;
const int CSEVI = 0x6F;

const int ZSN = 0x70;
const int ZSNI = 0x71;
const int ZSZ = 0x72;
const int ZSZI = 0x73;
const int ZSP = 0x74;
const int ZSPI = 0x75;
const int ZSOD = 0x76;
const int ZSODI = 0x77;

const int ZSNN = 0x78;
const int ZSNNI = 0x79;
const int ZSNZ = 0x7A;
const int ZSNZI = 0x7B;
const int ZSNP = 0x7C;
const int ZSNPI = 0x7D;
const int ZSEV = 0x7E;
const int ZSEVI = 0x7F;

const int LDB = 0x80;
const int LDBI = 0x81;
const int LDBU = 0x82;
const int LDBUI = 0x83;
const int LDW = 0x84;
const int LDWI = 0x85;
const int LDWU = 0x86;
const int LDWUI = 0x87;

const int LDT = 0x88;
const int LDTI = 0x89;
const int LDTU = 0x8A;
const int LDTUI = 0x8B;
const int LDO = 0x8C;
const int LDOI = 0x8D;
const int LDOU = 0x8E;
const int LDOUI = 0x8F;

const int LDSF = 0x90;
const int LDSFI = 0x91;
const int LDHT = 0x92;
const int LDHTI = 0x93;
const int CSWAP = 0x94;
const int CSWAPI = 0x95;
const int LDUNC = 0x96;
const int LDUNCI = 0x97;

const int LDVTS = 0x98;
const int LDVTSI = 0x99;
const int PRELD = 0x9A;
const int PRELDI = 0x9B;
const int PREGO = 0x9C;
const int PREGOI = 0x9D;
const int GO = 0x9E;
const int GOI = 0x9F;

const int STB = 0xA0;
const int STBI = 0xA1;
const int STBU = 0xA2;
const int STBUI = 0xA3;
const int STW = 0xA4;
const int STWI = 0xA5;
const int STWU = 0xA6;
const int STWUI = 0xA7;

const int STT = 0xA8;
const int STTI = 0xA9;
const int STTU = 0xAA;
const int STTUI = 0xAB;
const int STO = 0xAC;
const int STOI = 0xAD;
const int STOU = 0xAE;
const int STOUI = 0xAF;

const int STSF = 0xB0;
const int STSFI = 0xB1;
const int STHT = 0xB2;
const int STHTI = 0xB3;
const int STCO = 0xB4;
const int STCOI = 0xB5;
const int STUNC = 0xB6;
const int STUNCI = 0xB7;

const int SYNCD = 0xB8;
const int SYNCDI = 0xB9;
const int PREST = 0xBA;
const int PRESTI = 0xBB;
const int SYNCID = 0xBC;
const int SYNCIDI = 0xBD;
const int CALL = 0xBE;
const int CALLI = 0xBF;

const int OR = 0xC0;
const int ORI = 0xC1;
const int ORN = 0xC2;
const int ORNI = 0xC3;
const int NOR = 0xC4;
const int NORI = 0xC5;
const int XOR = 0xC6;
const int XORI = 0xC7;

const int AND = 0xC8;
const int ANDI = 0xC9;
const int ANDN = 0xCA;
const int ANDNI = 0xCB;
const int NAND = 0xCC;
const int NANDI = 0xCD;
const int NXOR = 0xCE;
const int NXORI = 0xCF;

const int BDIF = 0xD0;
const int BDIFI = 0xD1;
const int WDIF = 0xD2;
const int WDIFI = 0xD3;
const int TDIF = 0xD4;
const int TDIFI = 0xD5;
const int ODIF = 0xD6;
const int ODIFI = 0xD7;

const int MUX = 0xD8;
const int MUXI = 0xD9;
const int SADD = 0xDA;
const int SADDI = 0xDB;
const int MOR = 0xDC;
const int MORI = 0xDD;
const int MXOR = 0xDE;
const int MXORI = 0xDF;

const int SETH = 0xE0;
const int SETMH = 0xE1;
const int SETML = 0xE2;
const int SETL = 0xE3;
const int INCH = 0xE4;
const int INCMH = 0xE5;
const int INCML = 0xE6;
const int INCL = 0xE7;

const int ORH = 0xE8;
const int ORMH = 0xE9;
const int ORML = 0xEA;
const int ORL = 0xEB;
const int ANDNH = 0xEC;
const int ANDNMH = 0xED;
const int ANDNML = 0xEE;
const int ANDNL = 0xEF;

const int JMP = 0xF0;
const int JMPB = 0xF1;
const int PUSHJ = 0xF2;
const int PUSHJB = 0xF3;
const int GETA = 0xF4;
const int GETAB = 0xF5;
const int PUT = 0xF6;
const int PUTI = 0xF7;

const int RET = 0xF8;
const int RESUME = 0xF9;
const int SAVE = 0xFA;
const int UNSAVE = 0xFB;
const int SYNC = 0xFC;
const int SWYM = 0xFD;
const int GET = 0xFE;
const int TRIP = 0xFF;

const int IS = 0x100;
const int EXTERN = 0x101;
const int LINKONCE = 0x102;
const int FUNC = 0x103;
const int ENDF = 0x104;
const int STRUCT = 0x105;
const int ENDS = 0x106;
const int BYTE = 0x107;
const int WYDE = 0x108;
const int TETRA = 0x109;
const int OCTA = 0x10A;
const int CODE = 0x10B;
const int DATA = 0x10C;
const int LDA = 0x10D;
const int SET = 0x10E;

const int numOpCodes = SET + 1;

CMSYSTEMX_API int GetMaxOpCodeNameLength();
CMSYSTEMX_API const std::string& GetOpCodeName(int opCode);

CMSYSTEMX_API void OpCodeInit();
CMSYSTEMX_API void OpCodeDone();

} // namespace cmsystemx

#endif // CMAJOR_CMSX_OPCODE_INCLUDED
