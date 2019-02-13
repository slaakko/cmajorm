// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/OpCode.hpp>
#include <cmajor/util/Error.hpp>
#include <vector>

namespace cmsystemx {

using namespace cmajor::util;

class OpCodeMap
{
public:
    static void Init();
    static void Done();
    static OpCodeMap& Instance() { return *instance; }
    const std::string& GetOpCodeName(int opCode);
    int GetMaxOpCodeNameLength() const { return maxOpCodeNameLength; }
private:
    OpCodeMap();
    static std::unique_ptr<OpCodeMap> instance;
    std::vector<std::string> opCodeNames;
    int maxOpCodeNameLength;
};

std::unique_ptr<OpCodeMap> OpCodeMap::instance;

void OpCodeMap::Init()
{
    instance.reset(new OpCodeMap());
}

void OpCodeMap::Done()
{
    instance.reset();
}

OpCodeMap::OpCodeMap() : maxOpCodeNameLength(-1)
{
    opCodeNames.resize(numOpCodes);
    opCodeNames[TRAP] = "TRAP";
    opCodeNames[FCMP] = "FCMP";
    opCodeNames[FUN] = "FUN";
    opCodeNames[FEQL] = "FEQL";
    opCodeNames[FADD] = "FADD";
    opCodeNames[FIX] = "FIX";
    opCodeNames[FSUB] = "FSUB";
    opCodeNames[FIXU] = "FIXU";

    opCodeNames[FLOT] = "FLOT";
    opCodeNames[FLOTI] = "FLOTI";
    opCodeNames[FLOTU] = "FLOTU";
    opCodeNames[FLOTUI] = "FLOTUI";
    opCodeNames[SFLOT] = "SFLOT";
    opCodeNames[SFLOTI] = "SFLOTI";
    opCodeNames[SFLOTU] = "SFLOTU";
    opCodeNames[SFLOTUI] = "SFLOTUI";

    opCodeNames[FMUL] = "FMUL";
    opCodeNames[FCMPE] = "FCMPE";
    opCodeNames[FUNE] = "FUNE";
    opCodeNames[FEQLE] = "FEQLE";
    opCodeNames[FDIV] = "FDIV";
    opCodeNames[FSQRT] = "FSQRT";
    opCodeNames[FREM] = "FREM";
    opCodeNames[FINT] = "FINT";

    opCodeNames[MUL] = "MUL";
    opCodeNames[MULI] = "MULI";
    opCodeNames[MULU] = "MULU";
    opCodeNames[MULUI] = "MULUI";
    opCodeNames[DIV] = "DIV";
    opCodeNames[DIVI] = "DIVI";
    opCodeNames[DIVU] = "DIVU";
    opCodeNames[DIVUI] = "DIVUI";

    opCodeNames[ADD] = "ADD";
    opCodeNames[ADDI] = "ADDI";
    opCodeNames[ADDU] = "ADDU";
    opCodeNames[ADDUI] = "ADDUI";
    opCodeNames[SUB] = "SUB";
    opCodeNames[SUBI] = "SUBI";
    opCodeNames[SUBU] = "SUBU";
    opCodeNames[SUBUI] = "SUBUI";

    opCodeNames[I2ADDU] = "2ADDU";
    opCodeNames[I2ADDUI] = "2ADDUI";
    opCodeNames[I4ADDU] = "4ADDU";
    opCodeNames[I4ADDUI] = "4ADDUI";
    opCodeNames[I8ADDU] = "8ADDU";
    opCodeNames[I8ADDUI] = "8ADDUI";
    opCodeNames[I16ADDU] = "16ADDU";
    opCodeNames[I16ADDUI] = "16ADDUI";

    opCodeNames[CMP] = "CMP";
    opCodeNames[CMPI] = "CMPI";
    opCodeNames[CMPU] = "CMPU";
    opCodeNames[CMPUI] = "CMPUI";
    opCodeNames[NEG] = "NEG";
    opCodeNames[NEGI] = "NEGI";
    opCodeNames[NEGU] = "NEGU";
    opCodeNames[NEGUI] = "NEGUI";

    opCodeNames[SL] = "SL";
    opCodeNames[SLI] = "SLI";
    opCodeNames[SLU] = "SLU";
    opCodeNames[SLUI] = "SLUI";
    opCodeNames[SR] = "SR";
    opCodeNames[SRI] = "SRI";
    opCodeNames[SRU] = "SRU";
    opCodeNames[SRUI] = "SRUI";

    opCodeNames[BN] = "BN";
    opCodeNames[BNB] = "BNB";
    opCodeNames[BZ] = "BZ";
    opCodeNames[BZB] = "BZB";
    opCodeNames[BP] = "BP";
    opCodeNames[BPB] = "BPB";
    opCodeNames[BOD] = "BOD";
    opCodeNames[BODB] = "BODB";

    opCodeNames[BNN] = "BNN";
    opCodeNames[BNNB] = "BNNB";
    opCodeNames[BNZ] = "BNZ";
    opCodeNames[BNZB] = "BNZB";
    opCodeNames[BNP] = "BNP";
    opCodeNames[BNPB] = "BNPB";
    opCodeNames[BEV] = "BEV";
    opCodeNames[BEVB] = "BEVB";

    opCodeNames[PBN] = "PBN";
    opCodeNames[PBNB] = "PBNB";
    opCodeNames[PBZ] = "PBZ";
    opCodeNames[PBZB] = "PBZB";
    opCodeNames[PBP] = "PBP";
    opCodeNames[PBPB] = "PBPB";
    opCodeNames[PBOD] = "PBOD";
    opCodeNames[PBODB] = "PBODB";

    opCodeNames[PBNN] = "PBNN";
    opCodeNames[PBNNB] = "PBNNB";
    opCodeNames[PBNZ] = "PBNZ";
    opCodeNames[PBNZB] = "PBNZB";
    opCodeNames[PBNP] = "PBNP";
    opCodeNames[PBNPB] = "PBNPB";
    opCodeNames[PBEV] = "PBEV";
    opCodeNames[PBEVB] = "PBEVB";

    opCodeNames[CSN] = "CSN";
    opCodeNames[CSNI] = "CSNI";
    opCodeNames[CSZ] = "CSZ";
    opCodeNames[CSZI] = "CSZI";
    opCodeNames[CSP] = "CSP";
    opCodeNames[CSPI] = "CSPI";
    opCodeNames[CSOD] = "CSOD";
    opCodeNames[CSODI] = "CSODI";

    opCodeNames[CSNN] = "CSNN";
    opCodeNames[CSNNI] = "CSNNI";
    opCodeNames[CSNZ] = "CSNZ";
    opCodeNames[CSNZI] = "CSNZI";
    opCodeNames[CSNP] = "CSNP";
    opCodeNames[CSNPI] = "CSNPI";
    opCodeNames[CSEV] = "CSEV";
    opCodeNames[CSEVI] = "CSEVI";

    opCodeNames[ZSN] = "ZSN";
    opCodeNames[ZSNI] = "ZSNI";
    opCodeNames[ZSZ] = "ZSZ";
    opCodeNames[ZSZI] = "ZSZI";
    opCodeNames[ZSP] = "ZSP";
    opCodeNames[ZSPI] = "ZSPI";
    opCodeNames[ZSOD] = "ZSOD";
    opCodeNames[ZSODI] = "ZSODI";

    opCodeNames[ZSNN] = "ZSNN";
    opCodeNames[ZSNNI] = "ZSNNI";
    opCodeNames[ZSNZ] = "ZSNZ";
    opCodeNames[ZSNZI] = "ZSNZI";
    opCodeNames[ZSNP] = "ZSNP";
    opCodeNames[ZSNPI] = "ZSNPI";
    opCodeNames[ZSEV] = "ZSEV";
    opCodeNames[ZSEVI] = "ZSEVI";

    opCodeNames[LDB] = "LDB";
    opCodeNames[LDBI] = "LDBI";
    opCodeNames[LDBU] = "LDBU";
    opCodeNames[LDBUI] = "LDBUI";
    opCodeNames[LDW] = "LDW";
    opCodeNames[LDWI] = "LDWI";
    opCodeNames[LDWU] = "LDWU";
    opCodeNames[LDWUI] = "LDWUI";

    opCodeNames[LDT] = "LDT";
    opCodeNames[LDTI] = "LDTI";
    opCodeNames[LDTU] = "LDTU";
    opCodeNames[LDTUI] = "LDTUI";
    opCodeNames[LDO] = "LDO";
    opCodeNames[LDOI] = "LDOI";
    opCodeNames[LDOU] = "LDOU";
    opCodeNames[LDOUI] = "LDOUI";

    opCodeNames[LDSF] = "LDSF";
    opCodeNames[LDSFI] = "LDSFI";
    opCodeNames[LDHT] = "LDHT";
    opCodeNames[LDHTI] = "LDHTI";
    opCodeNames[CSWAP] = "CSWAP";
    opCodeNames[CSWAPI] = "CSWAPI";
    opCodeNames[LDUNC] = "LDUNC";
    opCodeNames[LDUNCI] = "LDUNCI";

    opCodeNames[LDVTS] = "LDVTS";
    opCodeNames[LDVTSI] = "LDVTSI";
    opCodeNames[PRELD] = "PRELD";
    opCodeNames[PRELDI] = "PRELDI";
    opCodeNames[PREGO] = "PREGO";
    opCodeNames[PREGOI] = "PREGOI";
    opCodeNames[GO] = "GO";
    opCodeNames[GOI] = "GOI";

    opCodeNames[STB] = "STB";
    opCodeNames[STBI] = "STBI";
    opCodeNames[STBU] = "STBU";
    opCodeNames[STBUI] = "STBUI";
    opCodeNames[STW] = "STW";
    opCodeNames[STWI] = "STWI";
    opCodeNames[STWU] = "STWU";
    opCodeNames[STWUI] = "STWUI";

    opCodeNames[STT] = "STT";
    opCodeNames[STTI] = "STTI";
    opCodeNames[STTU] = "STTU";
    opCodeNames[STTUI] = "STTUI";
    opCodeNames[STO] = "STO";
    opCodeNames[STOI] = "STOI";
    opCodeNames[STOU] = "STOU";
    opCodeNames[STOUI] = "STOUI";

    opCodeNames[STSF] = "STSF";
    opCodeNames[STSFI] = "STSFI";
    opCodeNames[STHT] = "STHT";
    opCodeNames[STHTI] = "STHTI";
    opCodeNames[STCO] = "STCO";
    opCodeNames[STCOI] = "STCOI";
    opCodeNames[STUNC] = "STUNC";
    opCodeNames[STUNCI] = "STUNCI";

    opCodeNames[SYNCD] = "SYNCD";
    opCodeNames[SYNCDI] = "SYNCDI";
    opCodeNames[PREST] = "PREST";
    opCodeNames[PRESTI] = "PRESTI";
    opCodeNames[SYNCID] = "SYNCID";
    opCodeNames[SYNCIDI] = "SYNCIDI";
    opCodeNames[CALL] = "CALL";
    opCodeNames[CALLI] = "CALLI";

    opCodeNames[OR] = "OR";
    opCodeNames[ORI] = "ORI";
    opCodeNames[ORN] = "ORN";
    opCodeNames[ORNI] = "ORNI";
    opCodeNames[NOR] = "NOR";
    opCodeNames[NORI] = "NORI";
    opCodeNames[XOR] = "XOR";
    opCodeNames[XORI] = "XORI";

    opCodeNames[AND] = "AND";
    opCodeNames[ANDI] = "ANDI";
    opCodeNames[ANDN] = "ANDN";
    opCodeNames[ANDNI] = "ANDNI";
    opCodeNames[NAND] = "NAND";
    opCodeNames[NANDI] = "NANDI";
    opCodeNames[NXOR] = "NXOR";
    opCodeNames[NXORI] = "NXORI";

    opCodeNames[BDIF] = "BDIF";
    opCodeNames[BDIFI] = "BDIFI";
    opCodeNames[WDIF] = "WDIF";
    opCodeNames[WDIFI] = "WDIFI";
    opCodeNames[TDIF] = "TDIF";
    opCodeNames[TDIFI] = "TDIFI";
    opCodeNames[ODIF] = "ODIF";
    opCodeNames[ODIFI] = "ODIFI";

    opCodeNames[MUX] = "MUX";
    opCodeNames[MUXI] = "MUXI";
    opCodeNames[SADD] = "SADD";
    opCodeNames[SADDI] = "SADDI";
    opCodeNames[MOR] = "MOR";
    opCodeNames[MORI] = "MORI";
    opCodeNames[MXOR] = "MXOR";
    opCodeNames[MXORI] = "MXORI";

    opCodeNames[SETH] = "SETH";
    opCodeNames[SETMH] = "SETMH";
    opCodeNames[SETML] = "SETML";
    opCodeNames[SETL] = "SETL";
    opCodeNames[INCH] = "INCH";
    opCodeNames[INCMH] = "INCMH";
    opCodeNames[INCML] = "INCML";
    opCodeNames[INCL] = "INCL";

    opCodeNames[ORH] = "ORH";
    opCodeNames[ORMH] = "ORMH";
    opCodeNames[ORML] = "ORML";
    opCodeNames[ORL] = "ORL";
    opCodeNames[ANDNH] = "ANDNH";
    opCodeNames[ANDNMH] = "ANDNMH";
    opCodeNames[ANDNML] = "ANDNML";
    opCodeNames[ANDNL] = "ANDNL";

    opCodeNames[JMP] = "JMP";
    opCodeNames[JMPB] = "JMPB";
    opCodeNames[PUSHJ] = "PUSHJ";
    opCodeNames[PUSHJB] = "PUSHJB";
    opCodeNames[GETA] = "GETA";
    opCodeNames[GETAB] = "GETAB";
    opCodeNames[PUT] = "PUT";
    opCodeNames[PUTI] = "PUTI";

    opCodeNames[RET] = "RET";
    opCodeNames[RESUME] = "RESUME";
    opCodeNames[SAVE] = "SAVE";
    opCodeNames[UNSAVE] = "UNSAVE";
    opCodeNames[SYNC] = "SYNC";
    opCodeNames[SWYM] = "SWYM";
    opCodeNames[GET] = "GET";
    opCodeNames[TRIP] = "TRIP";

    opCodeNames[IS] = "IS";
    opCodeNames[EXTERN] = "EXTERN";
    opCodeNames[LINKONCE] = "LINKONCE";
    opCodeNames[FUNC] = "FUNC";
    opCodeNames[ENDF] = "ENDF";
    opCodeNames[STRUCT] = "STRUCT";
    opCodeNames[ENDS] = "ENDS";
    opCodeNames[BYTE] = "BYTE";
    opCodeNames[WYDE] = "WYDE";
    opCodeNames[TETRA] = "TETRA";
    opCodeNames[OCTA] = "OCTA";
    opCodeNames[CODE] = ".CODE";
    opCodeNames[DATA] = ".DATA";
    opCodeNames[LDA] = "LDA";
    opCodeNames[SET] = "SET";

    int n = opCodeNames.size();
    for (int i = 0; i < n; ++i)
    {
        int nameLength = opCodeNames[i].length();
        if (nameLength > maxOpCodeNameLength)
        {
            maxOpCodeNameLength = nameLength;
        }
    }
}

const std::string& OpCodeMap::GetOpCodeName(int opCode)
{
    Assert(opCode >= 0 && opCode < opCodeNames.size(), "invalid opcode");
    return opCodeNames[opCode];
}

int GetMaxOpCodeNameLength()
{
    return OpCodeMap::Instance().GetMaxOpCodeNameLength();
}

const std::string& GetOpCodeName(int opCode)
{
    return OpCodeMap::Instance().GetOpCodeName(opCode);
}

void OpCodeInit()
{
    OpCodeMap::Init();
}

void OpCodeDone()
{
    OpCodeMap::Done();
}

} // namespace cmsystemx 
