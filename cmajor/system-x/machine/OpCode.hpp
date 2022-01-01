// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_OPCODE_INCLUDED
#define CMSX_MACHINE_OPCODE_INCLUDED
#include <system-x/machine/Api.hpp>
#include <string>
#include <cstdint>

namespace cmsx::machine {

const std::uint8_t TRAP = 0x00u;
const std::uint8_t FCMP = 0x01u;
const std::uint8_t FUN = 0x02u;
const std::uint8_t FEQL = 0x03u;
const std::uint8_t FADD = 0x04u;
const std::uint8_t FIX = 0x05u;
const std::uint8_t FSUB = 0x06u;
const std::uint8_t FIXU = 0x07u;

const std::uint8_t FLOT = 0x08u;
const std::uint8_t FLOTI = 0x09u;
const std::uint8_t FLOTU = 0x0Au;
const std::uint8_t FLOTUI = 0x0Bu;
const std::uint8_t SFLOT = 0x0Cu;
const std::uint8_t SFLOTI = 0x0Du;
const std::uint8_t SFLOTU = 0x0Eu;
const std::uint8_t SFLOTUI = 0x0Fu;

const std::uint8_t FMUL = 0x10u;
const std::uint8_t FCMPE = 0x11u;
const std::uint8_t FUNE = 0x12u;
const std::uint8_t FEQLE = 0x13u;
const std::uint8_t FDIV = 0x14u;
const std::uint8_t FSQRT = 0x15u;
const std::uint8_t FREM = 0x16u;
const std::uint8_t FINT = 0x17u;

const std::uint8_t MUL = 0x18u;
const std::uint8_t MULI = 0x19u;
const std::uint8_t MULU = 0x1Au;
const std::uint8_t MULUI = 0x1Bu;
const std::uint8_t DIV = 0x1Cu;
const std::uint8_t DIVI = 0x1Du;
const std::uint8_t DIVU = 0x1Eu;
const std::uint8_t DIVUI = 0x1Fu;

const std::uint8_t ADD = 0x20u;
const std::uint8_t ADDI = 0x21u;
const std::uint8_t ADDU = 0x22u;
const std::uint8_t ADDUI = 0x23u;
const std::uint8_t SUB = 0x24u;
const std::uint8_t SUBI = 0x25u;
const std::uint8_t SUBU = 0x26u;
const std::uint8_t SUBUI = 0x27u;

const std::uint8_t I2ADDU = 0x28u;
const std::uint8_t I2ADDUI = 0x29u;
const std::uint8_t I4ADDU = 0x2Au;
const std::uint8_t I4ADDUI = 0x2Bu;
const std::uint8_t I8ADDU = 0x2Cu;
const std::uint8_t I8ADDUI = 0x2Du;
const std::uint8_t I16ADDU = 0x2Eu;
const std::uint8_t I16ADDUI = 0x2Fu;

const std::uint8_t CMP = 0x30u;
const std::uint8_t CMPI = 0x31u;
const std::uint8_t CMPU = 0x32u;
const std::uint8_t CMPUI = 0x33u;
const std::uint8_t NEG = 0x34u;
const std::uint8_t NEGI = 0x35u;
const std::uint8_t NEGU = 0x36u;
const std::uint8_t NEGUI = 0x37u;

const std::uint8_t SL = 0x38u;
const std::uint8_t SLI = 0x39u;
const std::uint8_t SLU = 0x3Au;
const std::uint8_t SLUI = 0x3Bu;
const std::uint8_t SR = 0x03Cu;
const std::uint8_t SRI = 0x3Du;
const std::uint8_t SRU = 0x3Eu;
const std::uint8_t SRUI = 0x3Fu;

const std::uint8_t BN = 0x40u;
const std::uint8_t BNB = 0x41u;
const std::uint8_t BZ = 0x42u;
const std::uint8_t BZB = 0x43u;
const std::uint8_t BP = 0x44u;
const std::uint8_t BPB = 0x45u;
const std::uint8_t BOD = 0x46u;
const std::uint8_t BODB = 0x47u;

const std::uint8_t BNN = 0x48u;
const std::uint8_t BNNB = 0x49u;
const std::uint8_t BNZ = 0x4Au;
const std::uint8_t BNZB = 0x4Bu;
const std::uint8_t BNP = 0x4Cu;
const std::uint8_t BNPB = 0x4Du;
const std::uint8_t BEV = 0x4Eu;
const std::uint8_t BEVB = 0x4Fu;

const std::uint8_t PBN = 0x50u;
const std::uint8_t PBNB = 0x51u;
const std::uint8_t PBZ = 0x52u;
const std::uint8_t PBZB = 0x53u;
const std::uint8_t PBP = 0x54u;
const std::uint8_t PBPB = 0x55u;
const std::uint8_t PBOD = 0x56u;
const std::uint8_t PBODB = 0x57u;

const std::uint8_t PBNN = 0x58u;
const std::uint8_t PBNNB = 0x59u;
const std::uint8_t PBNZ = 0x5Au;
const std::uint8_t PBNZB = 0x5Bu;
const std::uint8_t PBNP = 0x5Cu;
const std::uint8_t PBNPB = 0x5Du;
const std::uint8_t PBEV = 0x5Eu;
const std::uint8_t PBEVB = 0x5Fu;

const std::uint8_t CSN = 0x60u;
const std::uint8_t CSNI = 0x61u;
const std::uint8_t CSZ = 0x62u;
const std::uint8_t CSZI = 0x63u;
const std::uint8_t CSP = 0x64u;
const std::uint8_t CSPI = 0x65u;
const std::uint8_t CSOD = 0x66u;
const std::uint8_t CSODI = 0x67u;

const std::uint8_t CSNN = 0x68u;
const std::uint8_t CSNNI = 0x69u;
const std::uint8_t CSNZ = 0x6Au;
const std::uint8_t CSNZI = 0x6Bu;
const std::uint8_t CSNP = 0x6Cu;
const std::uint8_t CSNPI = 0x6Du;
const std::uint8_t CSEV = 0x6Eu;
const std::uint8_t CSEVI = 0x6Fu;

const std::uint8_t ZSN = 0x70u;
const std::uint8_t ZSNI = 0x71u;
const std::uint8_t ZSZ = 0x72u;
const std::uint8_t ZSZI = 0x73u;
const std::uint8_t ZSP = 0x74u;
const std::uint8_t ZSPI = 0x75u;
const std::uint8_t ZSOD = 0x76u;
const std::uint8_t ZSODI = 0x77u;

const std::uint8_t ZSNN = 0x78u;
const std::uint8_t ZSNNI = 0x79u;
const std::uint8_t ZSNZ = 0x7Au;
const std::uint8_t ZSNZI = 0x7Bu;
const std::uint8_t ZSNP = 0x7Cu;
const std::uint8_t ZSNPI = 0x7Du;
const std::uint8_t ZSEV = 0x7Eu;
const std::uint8_t ZSEVI = 0x7Fu;

const std::uint8_t LDB = 0x80u;
const std::uint8_t LDBI = 0x81u;
const std::uint8_t LDBU = 0x82u;
const std::uint8_t LDBUI = 0x83u;
const std::uint8_t LDW = 0x84u;
const std::uint8_t LDWI = 0x85u;
const std::uint8_t LDWU = 0x86u;
const std::uint8_t LDWUI = 0x87u;

const std::uint8_t LDT = 0x88u;
const std::uint8_t LDTI = 0x89u;
const std::uint8_t LDTU = 0x8Au;
const std::uint8_t LDTUI = 0x8Bu;
const std::uint8_t LDO = 0x8Cu;
const std::uint8_t LDOI = 0x8Du;
const std::uint8_t LDOU = 0x8Eu;
const std::uint8_t LDOUI = 0x8Fu;

const std::uint8_t LDSF = 0x90u;
const std::uint8_t LDSFI = 0x91u;
const std::uint8_t LDHT = 0x92u;
const std::uint8_t LDHTI = 0x93u;
const std::uint8_t CSWAP = 0x94u;
const std::uint8_t CSWAPI = 0x95u;
const std::uint8_t LDUNC = 0x96u;
const std::uint8_t LDUNCI = 0x97u;

const std::uint8_t LDVTS = 0x98u;
const std::uint8_t LDVTSI = 0x99u;
const std::uint8_t PRELD = 0x9Au;
const std::uint8_t PRELDI = 0x9Bu;
const std::uint8_t PREGO = 0x9Cu;
const std::uint8_t PREGOI = 0x9Du;
const std::uint8_t GO = 0x9Eu;
const std::uint8_t GOI = 0x9Fu;

const std::uint8_t STB = 0xA0u;
const std::uint8_t STBI = 0xA1u;
const std::uint8_t STBU = 0xA2u;
const std::uint8_t STBUI = 0xA3u;
const std::uint8_t STW = 0xA4u;
const std::uint8_t STWI = 0xA5u;
const std::uint8_t STWU = 0xA6u;
const std::uint8_t STWUI = 0xA7u;

const std::uint8_t STT = 0xA8u;
const std::uint8_t STTI = 0xA9u;
const std::uint8_t STTU = 0xAAu;
const std::uint8_t STTUI = 0xABu;
const std::uint8_t STO = 0xACu;
const std::uint8_t STOI = 0xADu;
const std::uint8_t STOU = 0xAEu;
const std::uint8_t STOUI = 0xAFu;

const std::uint8_t STSF = 0xB0u;
const std::uint8_t STSFI = 0xB1u;
const std::uint8_t STHT = 0xB2u;
const std::uint8_t STHTI = 0xB3u;
const std::uint8_t STCO = 0xB4u;
const std::uint8_t STCOI = 0xB5u;
const std::uint8_t STUNC = 0xB6u;
const std::uint8_t STUNCI = 0xB7u;

const std::uint8_t SYNCD = 0xB8u;
const std::uint8_t SYNCDI = 0xB9u;
const std::uint8_t PREST = 0xBAu;
const std::uint8_t PRESTI = 0xBBu;
const std::uint8_t SYNCID = 0xBCu;
const std::uint8_t SYNCIDI = 0xBDu;
const std::uint8_t CALL = 0xBEu;
const std::uint8_t CALLI = 0xBFu;

const std::uint8_t OR = 0xC0u;
const std::uint8_t ORI = 0xC1u;
const std::uint8_t ORN = 0xC2u;
const std::uint8_t ORNI = 0xC3u;
const std::uint8_t NOR = 0xC4u;
const std::uint8_t NORI = 0xC5u;
const std::uint8_t XOR = 0xC6u;
const std::uint8_t XORI = 0xC7u;

const std::uint8_t AND = 0xC8u;
const std::uint8_t ANDI = 0xC9u;
const std::uint8_t ANDN = 0xCAu;
const std::uint8_t ANDNI = 0xCBu;
const std::uint8_t NAND = 0xCCu;
const std::uint8_t NANDI = 0xCDu;
const std::uint8_t NXOR = 0xCEu;
const std::uint8_t NXORI = 0xCFu;

const std::uint8_t BDIF = 0xD0;
const std::uint8_t BDIFI = 0xD1u;
const std::uint8_t WDIF = 0xD2u;
const std::uint8_t WDIFI = 0xD3u;
const std::uint8_t TDIF = 0xD4u;
const std::uint8_t TDIFI = 0xD5u;
const std::uint8_t ODIF = 0xD6u;
const std::uint8_t ODIFI = 0xD7u;

const std::uint8_t MUX = 0xD8u;
const std::uint8_t MUXI = 0xD9u;
const std::uint8_t SADD = 0xDAu;
const std::uint8_t SADDI = 0xDBu;
const std::uint8_t MOR = 0xDCu;
const std::uint8_t MORI = 0xDDu;
const std::uint8_t MXOR = 0xDEu;
const std::uint8_t MXORI = 0xDFu;

const std::uint8_t SETH = 0xE0u;
const std::uint8_t SETMH = 0xE1u;
const std::uint8_t SETML = 0xE2u;
const std::uint8_t SETL = 0xE3u;
const std::uint8_t INCH = 0xE4u;
const std::uint8_t INCMH = 0xE5u;
const std::uint8_t INCML = 0xE6u;
const std::uint8_t INCL = 0xE7u;

const std::uint8_t ORH = 0xE8u;
const std::uint8_t ORMH = 0xE9u;
const std::uint8_t ORML = 0xEAu;
const std::uint8_t ORL = 0xEBu;
const std::uint8_t ANDNH = 0xECu;
const std::uint8_t ANDNMH = 0xEDu;
const std::uint8_t ANDNML = 0xEEu;
const std::uint8_t ANDNL = 0xEFu;

const std::uint8_t JMP = 0xF0u;
const std::uint8_t JMPB = 0xF1u;
const std::uint8_t PUSHJ = 0xF2u;
const std::uint8_t PUSHJB = 0xF3u;
const std::uint8_t GETA = 0xF4u;
const std::uint8_t GETAB = 0xF5u;
const std::uint8_t PUT = 0xF6u;
const std::uint8_t PUTI = 0xF7u;

const std::uint8_t RET = 0xF8u;
const std::uint8_t RESUME = 0xF9u;
const std::uint8_t SAVE = 0xFAu;
const std::uint8_t UNSAVE = 0xFBu;
const std::uint8_t SYNC = 0xFCu;
const std::uint8_t SWYM = 0xFDu;
const std::uint8_t GET = 0xFEu;
const std::uint8_t TRIP = 0xFFu;

CMSX_MACHINE_API int GetOpCode(const std::string& opCodeName);
CMSX_MACHINE_API const std::string& GetOpCodeName(uint8_t opCode);
CMSX_MACHINE_API int MaxOpCodeNameLength();
CMSX_MACHINE_API void InitOpCode();
CMSX_MACHINE_API void DoneOpCode();
CMSX_MACHINE_API bool IsJumpInst(std::uint8_t opCode);

} // cmsx::machine

#endif // CMSX_MACHINE_OPCODE_INCLUDED
