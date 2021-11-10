// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/OpCode.hpp>
#include <map>
#include <memory>
#include <vector>

namespace cmsx::machine {

class OpCodes
{
public:
    static void Init();
    static void Done();
    static OpCodes& Instance() { return *instance; }
    const std::string& GetOpCodeName(std::uint8_t opCode) const { return names[opCode]; }
    int GetOpCode(const std::string& opCodeName) const;
    int MaxOpCodeNameLength() const { return maxOpCodeNameLength; }
private:
    OpCodes();
    void MakeNames();
    void MakeCodes();
    static std::unique_ptr<OpCodes> instance;
    std::vector<std::string> names;
    std::map<std::string, std::uint8_t> codes;
    int maxOpCodeNameLength;
};

std::unique_ptr<OpCodes> OpCodes::instance;

void OpCodes::Init()
{
    instance.reset(new OpCodes());
}

void OpCodes::Done()
{
    instance.reset();
}

OpCodes::OpCodes() : maxOpCodeNameLength(0)
{
    MakeNames();
    MakeCodes();
}

int OpCodes::GetOpCode(const std::string& opCodeName) const
{
    auto it = codes.find(opCodeName);
    if (it != codes.cend())
    {
        return it->second;
    }
    else
    {
        return -1;
    }
}

void OpCodes::MakeNames()
{
    names.resize(256);

    names[TRAP] = "TRAP";
    names[FCMP] = "FCMP";
    names[FUN] = "FUN";
    names[FEQL] = "FEQL";
    names[FADD] = "FADD";
    names[FIX] = "FIX";
    names[FSUB] = "FSUB";
    names[FIXU] = "FIXU";

    names[FLOT] = "FLOT";
    names[FLOTI] = "FLOTI";
    names[FLOTU] = "FLOTU";
    names[FLOTUI] = "FLOTUI";
    names[SFLOT] = "SFLOT";
    names[SFLOTI] = "SFLOTI";
    names[SFLOTU] = "SFLOTU";
    names[SFLOTUI] = "SFLOTUI";

    names[FMUL] = "FMUL";
    names[FCMPE] = "FCMPE";
    names[FUNE] = "FUNE";
    names[FEQLE] = "FEQLE";
    names[FDIV] = "FDIV";
    names[FSQRT] = "FSQRT";
    names[FREM] = "FREM";
    names[FINT] = "FINT";

    names[MUL] = "MUL";
    names[MULI] = "MULI";
    names[MULU] = "MULU";
    names[MULUI] = "MULUI";
    names[DIV] = "DIV";
    names[DIVI] = "DIVI";
    names[DIVU] = "DIVU";
    names[DIVUI] = "DIVUI";

    names[ADD] = "ADD";
    names[ADDI] = "ADDI";
    names[ADDU] = "ADDU";
    names[ADDUI] = "ADDUI";
    names[SUB] = "SUB";
    names[SUBI] = "SUBI";
    names[SUBU] = "SUBU";
    names[SUBUI] = "SUBUI";

    names[I2ADDU] = "2ADDU";
    names[I2ADDUI] = "2ADDUI";
    names[I4ADDU] = "4ADDU";
    names[I4ADDUI] = "4ADDUI";
    names[I8ADDU] = "8ADDU";
    names[I8ADDUI] = "8ADDUI";
    names[I16ADDU] = "16ADDU";
    names[I16ADDUI] = "16ADDUI";

    names[CMP] = "CMP";
    names[CMPI] = "CMPI";
    names[CMPU] = "CMPU";
    names[CMPUI] = "CMPUI";
    names[NEG] = "NEG";
    names[NEGI] = "NEGI";
    names[NEGU] = "NEGU";
    names[NEGUI] = "NEGUI";

    names[SL] = "SL";
    names[SLI] = "SLI";
    names[SLU] = "SLU";
    names[SLUI] = "SLUI";
    names[SR] = "SR";
    names[SRI] = "SRI";
    names[SRU] = "SRU";
    names[SRUI] = "SRUI";

    names[BN] = "BN";
    names[BNB] = "BNB";
    names[BZ] = "BZ";
    names[BZB] = "BZB";
    names[BP] = "BP";
    names[BPB] = "BPB";
    names[BOD] = "BOD";
    names[BODB] = "BODB";

    names[BNN] = "BNN";
    names[BNNB] = "BNNB";
    names[BNZ] = "BNZ";
    names[BNZB] = "BNZB";
    names[BNP] = "BNP";
    names[BNPB] = "BNPB";
    names[BEV] = "BEV";
    names[BEVB] = "BEVB";

    names[PBN] = "PBN";
    names[PBNB] = "PBNB";
    names[PBZ] = "PBZ";
    names[PBZB] = "PBZB";
    names[PBP] = "PBP";
    names[PBPB] = "PBPB";
    names[PBOD] = "PBOD";
    names[PBODB] = "PBODB";

    names[PBNN] = "PBNN";
    names[PBNNB] = "PBNNB";
    names[PBNZ] = "PBNZ";
    names[PBNZB] = "PBNZB";
    names[PBNP] = "PBNP";
    names[PBNPB] = "PBNPB";
    names[PBEV] = "PBEV";
    names[PBEVB] = "PBEVB";

    names[CSN] = "CSN";
    names[CSNI] = "CSNI";
    names[CSZ] = "CSZ";
    names[CSZI] = "CSZI";
    names[CSP] = "CSP";
    names[CSPI] = "CSPI";
    names[CSOD] = "CSOD";
    names[CSODI] = "CSODI";

    names[CSNN] = "CSNN";
    names[CSNNI] = "CSNNI";
    names[CSNZ] = "CSNZ";
    names[CSNZI] = "CSNZI";
    names[CSNP] = "CSNP";
    names[CSNPI] = "CSNPI";
    names[CSEV] = "CSEV";
    names[CSEVI] = "CSEVI";

    names[ZSN] = "ZSN";
    names[ZSNI] = "ZSNI";
    names[ZSZ] = "ZSZ";
    names[ZSZI] = "ZSZI";
    names[ZSP] = "ZSP";
    names[ZSPI] = "ZSPI";
    names[ZSOD] = "ZSOD";
    names[ZSODI] = "ZSODI";

    names[ZSNN] = "ZSNN";
    names[ZSNNI] = "ZSNNI";
    names[ZSNZ] = "ZSNZ";
    names[ZSNZI] = "ZSNZI";
    names[ZSNP] = "ZSNP";
    names[ZSNPI] = "ZSNPI";
    names[ZSEV] = "ZSEV";
    names[ZSEVI] = "ZSEVI";

    names[LDB] = "LDB";
    names[LDBI] = "LDBI";
    names[LDBU] = "LDBU";
    names[LDBUI] = "LDBUI";
    names[LDW] = "LDW";
    names[LDWI] = "LDWI";
    names[LDWU] = "LDWU";
    names[LDWUI] = "LDWUI";

    names[LDT] = "LDT";
    names[LDTI] = "LDTI";
    names[LDTU] = "LDTU";
    names[LDTUI] = "LDTUI";
    names[LDO] = "LDO";
    names[LDOI] = "LDOI";
    names[LDOU] = "LDOU";
    names[LDOUI] = "LDOUI";

    names[LDSF] = "LDSF";
    names[LDSFI] = "LDSFI";
    names[LDHT] = "LDHT";
    names[LDHTI] = "LDHTI";
    names[CSWAP] = "CSWAP";
    names[CSWAPI] = "CSWAPI";
    names[LDUNC] = "LDUNC";
    names[LDUNCI] = "LDUNCI";

    names[LDVTS] = "LDVTS";
    names[LDVTSI] = "LDVTSI";
    names[PRELD] = "PRELD";
    names[PRELDI] = "PRELDI";
    names[PREGO] = "PREGO";
    names[PREGOI] = "PREGOI";
    names[GO] = "GO";
    names[GOI] = "GOI";

    names[STB] = "STB";
    names[STBI] = "STBI";
    names[STBU] = "STBU";
    names[STBUI] = "STBUI";
    names[STW] = "STW";
    names[STWI] = "STWI";
    names[STWU] = "STWU";
    names[STWUI] = "STWUI";

    names[STT] = "STT";
    names[STTI] = "STTI";
    names[STTU] = "STTU";
    names[STTUI] = "STTUI";
    names[STO] = "STO";
    names[STOI] = "STOI";
    names[STOU] = "STOU";
    names[STOUI] = "STOUI";

    names[STSF] = "STSF";
    names[STSFI] = "STSFI";
    names[STHT] = "STHT";
    names[STHTI] = "STHTI";
    names[STCO] = "STCO";
    names[STCOI] = "STCOI";
    names[STUNC] = "STUNC";
    names[STUNCI] = "STUNCI";

    names[SYNCD] = "SYNCD";
    names[SYNCDI] = "SYNCDI";
    names[PREST] = "PREST";
    names[PRESTI] = "PRESTI";
    names[SYNCID] = "SYNCID";
    names[SYNCIDI] = "SYNCIDI";
    names[CALL] = "CALL";
    names[CALLI] = "CALLI";

    names[OR] = "OR";
    names[ORI] = "ORI";
    names[ORN] = "ORN";
    names[ORNI] = "ORNI";
    names[NOR] = "NOR";
    names[NORI] = "NORI";
    names[XOR] = "XOR";
    names[XORI] = "XORI";

    names[AND] = "AND";
    names[ANDI] = "ANDI";
    names[ANDN] = "ANDN";
    names[ANDNI] = "ANDNI";
    names[NAND] = "NAND";
    names[NANDI] = "NANDI";
    names[NXOR] = "NXOR";
    names[NXORI] = "NXORI";

    names[BDIF] = "BDIF";
    names[BDIFI] = "BDIFI";
    names[WDIF] = "WDIF";
    names[WDIFI] = "WDIFI";
    names[TDIF] = "TDIF";
    names[TDIFI] = "TDIFI";
    names[ODIF] = "ODIF";
    names[ODIFI] = "ODIFI";

    names[MUX] = "MUX";
    names[MUXI] = "MUXI";
    names[SADD] = "SADD";
    names[SADDI] = "SADDI";
    names[MOR] = "MOR";
    names[MORI] = "MORI";
    names[MXOR] = "MXOR";
    names[MXORI] = "MXORI";

    names[SETH] = "SETH";
    names[SETMH] = "SETMH";
    names[SETML] = "SETML";
    names[SETL] = "SETL";
    names[INCH] = "INCH";
    names[INCMH] = "INCMH";
    names[INCML] = "INCML";
    names[INCL] = "INCL";

    names[ORH] = "ORH";
    names[ORMH] = "ORMH";
    names[ORML] = "ORML";
    names[ORL] = "ORL";
    names[ANDNH] = "ANDNH";
    names[ANDNMH] = "ANDNMH";
    names[ANDNML] = "ANDNML";
    names[ANDNL] = "ANDNL";

    names[JMP] = "JMP";
    names[JMPB] = "JMPB";
    names[PUSHJ] = "PUSHJ";
    names[PUSHJB] = "PUSHJB";
    names[GETA] = "GETA";
    names[GETAB] = "GETAB";
    names[PUT] = "PUT";
    names[PUTI] = "PUTI";

    names[RET] = "RET";
    names[RESUME] = "RESUME";
    names[SAVE] = "SAVE";
    names[UNSAVE] = "UNSAVE";
    names[SYNC] = "SYNC";
    names[SWYM] = "SWYM";
    names[GET] = "GET";
    names[TRIP] = "TRIP";

    int n = names.size();
    for (int i = 0; i < n; ++i)
    {
        int nameLength = names[i].length();
        if (nameLength > maxOpCodeNameLength)
        {
            maxOpCodeNameLength = nameLength;
        }
    }
}

void OpCodes::MakeCodes()
{
    codes["TRAP"] = TRAP;
    codes["FCMP"] = FCMP;
    codes["FUN"] = FUN;
    codes["FEQL"] = FEQL;
    codes["FADD"] = FADD;
    codes["FIX"] = FIX;
    codes["FSUB"] = FSUB;
    codes["FIXU"] = FIXU;

    codes["FLOT"] = FLOT;
    codes["FLOTI"] = FLOTI;
    codes["FLOTU"] = FLOTU;
    codes["FLOTUI"] = FLOTUI;
    codes["SFLOT"] = SFLOT;
    codes["SFLOTI"] = SFLOTI;
    codes["SFLOTU"] = SFLOTU;
    codes["SFLOTUI"] = SFLOTUI;

    codes["FMUL"] = FMUL;
    codes["FCMPE"] = FCMPE;
    codes["FUNE"] = FUNE;
    codes["FEQLE"] = FEQLE;
    codes["FDIV"] = FDIV;
    codes["FSQRT"] = FSQRT;
    codes["FREM"] = FREM;
    codes["FINT"] = FINT;

    codes["MUL"] = MUL;
    codes["MULI"] = MULI;
    codes["MULU"] = MULU;
    codes["MULUI"] = MULUI;
    codes["DIV"] = DIV;
    codes["DIVI"] = DIVI;
    codes["DIVU"] = DIVU;
    codes["DIVUI"] = DIVUI;

    codes["ADD"] = ADD;
    codes["ADDI"] = ADDI;
    codes["ADDU"] = ADDU;
    codes["ADDUI"] = ADDUI;
    codes["SUB"] = SUB;
    codes["SUBI"] = SUBI;
    codes["SUBU"] = SUBU;
    codes["SUBUI"] = SUBUI;

    codes["2ADDU"] = I2ADDU;
    codes["2ADDUI"] = I2ADDUI;
    codes["4ADDU"] = I4ADDU;
    codes["4ADDUI"] = I4ADDUI;
    codes["8ADDU"] = I8ADDU;
    codes["8ADDUI"] = I8ADDUI;
    codes["16ADDU"] = I16ADDU;
    codes["16ADDUI"] = I16ADDUI;

    codes["CMP"] = CMP;
    codes["CMPI"] = CMPI;
    codes["CMPU"] = CMPU;
    codes["CMPUI"] = CMPUI;
    codes["NEG"] = NEG;
    codes["NEGI"] = NEGI;
    codes["NEGU"] = NEGU;
    codes["NEGUI"] = NEGUI;

    codes["SL"] = SL;
    codes["SLI"] = SLI;
    codes["SLU"] = SLU;
    codes["SLUI"] = SLUI;
    codes["SR"] = SR;
    codes["SRI"] = SRI;
    codes["SRU"] = SRU;
    codes["SRUI"] = SRUI;

    codes["BN"] = BN;
    codes["BNB"] = BNB;
    codes["BZ"] = BZ;
    codes["BZB"] = BZB;
    codes["BP"] = BP;
    codes["BPB"] = BPB;
    codes["BOD"] = BOD;
    codes["BODB"] = BODB;

    codes["BNN"] = BNN;
    codes["BNNB"] = BNNB;
    codes["BNZ"] = BNZ;
    codes["BNZB"] = BNZB;
    codes["BNP"] = BNP;
    codes["BNPB"] = BNPB;
    codes["BEV"] = BEV;
    codes["BEVB"] = BEVB;

    codes["PBN"] = PBN;
    codes["PBNB"] = PBNB;
    codes["PBZ"] = PBZ;
    codes["PBZB"] = PBZB;
    codes["PBP"] = PBP;
    codes["PBPB"] = PBPB;
    codes["PBOD"] = PBOD;
    codes["PBODB"] = PBODB;

    codes["PBNN"] = PBNN;
    codes["PBNNB"] = PBNNB;
    codes["PBNZ"] = PBNZ;
    codes["PBNZB"] = PBNZB;
    codes["PBNP"] = PBNP;
    codes["PBNPB"] = PBNPB;
    codes["PBEV"] = PBEV;
    codes["PBEVB"] = PBEVB;

    codes["CSN"] = CSN;
    codes["CSNI"] = CSNI;
    codes["CSZ"] = CSZ;
    codes["CSZI"] = CSZI;
    codes["CSP"] = CSP;
    codes["CSPI"] = CSPI;
    codes["CSOD"] = CSOD;
    codes["CSODI"] = CSODI;

    codes["CSNN"] = CSNN;
    codes["CSNNI"] = CSNNI;
    codes["CSNZ"] = CSNZ;
    codes["CSNZI"] = CSNZI;
    codes["CSNP"] = CSNP;
    codes["CSNPI"] = CSNPI;
    codes["CSEV"] = CSEV;
    codes["CSEVI"] = CSEVI;

    codes["ZSN"] = ZSN;
    codes["ZSNI"] = ZSNI;
    codes["ZSZ"] = ZSZ;
    codes["ZSZI"] = ZSZI;
    codes["ZSP"] = ZSP;
    codes["ZSPI"] = ZSPI;
    codes["ZSOD"] = ZSOD;
    codes["ZSODI"] = ZSODI;

    codes["ZSNN"] = ZSNN;
    codes["ZSNNI"] = ZSNNI;
    codes["ZSNZ"] = ZSNZ;
    codes["ZSNZI"] = ZSNZI;
    codes["ZSNP"] = ZSNP;
    codes["ZSNPI"] = ZSNPI;
    codes["ZSEV"] = ZSEV;
    codes["ZSEVI"] = ZSEVI;

    codes["LDB"] = LDB;
    codes["LDBI"] = LDBI;
    codes["LDBU"] = LDBU;
    codes["LDBUI"] = LDBUI;
    codes["LDW"] = LDW;
    codes["LDWI"] = LDWI;
    codes["LDWU"] = LDWU;
    codes["LDWUI"] = LDWUI;

    codes["LDT"] = LDT;
    codes["LDTI"] = LDTI;
    codes["LDTU"] = LDTU;
    codes["LDTUI"] = LDTUI;
    codes["LDO"] = LDO;
    codes["LDOI"] = LDOI;
    codes["LDOU"] = LDOU;
    codes["LDOUI"] = LDOUI;

    codes["LDSF"] = LDSF;
    codes["LDSFI"] = LDSFI;
    codes["LDHT"] = LDHT;
    codes["LDHTI"] = LDHTI;
    codes["CSWAP"] = CSWAP;
    codes["CSWAPI"] = CSWAPI;
    codes["LDUNC"] = LDUNC;
    codes["LDUNCI"] = LDUNCI;

    codes["LDVTS"] = LDVTS;
    codes["LDVTSI"] = LDVTSI;
    codes["PRELD"] = PRELD;
    codes["PRELDI"] = PRELDI;
    codes["PREGO"] = PREGO;
    codes["PREGOI"] = PREGOI;
    codes["GO"] = GO;
    codes["GOI"] = GOI;

    codes["STB"] = STB;
    codes["STBI"] = STBI;
    codes["STBU"] = STBU;
    codes["STBUI"] = STBUI;
    codes["STW"] = STW;
    codes["STWI"] = STWI;
    codes["STWU"] = STWU;
    codes["STWUI"] = STWUI;

    codes["STT"] = STT;
    codes["STTI"] = STTI;
    codes["STTU"] = STTU;
    codes["STTUI"] = STTUI;
    codes["STO"] = STO;
    codes["STOI"] = STOI;
    codes["STOU"] = STOU;
    codes["STOUI"] = STOUI;

    codes["STSF"] = STSF;
    codes["STSFI"] = STSFI;
    codes["STHT"] = STHT;
    codes["STHTI"] = STHTI;
    codes["STCO"] = STCO;
    codes["STCOI"] = STCOI;
    codes["STUNC"] = STUNC;
    codes["STUNCI"] = STUNCI;

    codes["SYNCD"] = SYNCD;
    codes["SYNCDI"] = SYNCDI;
    codes["PREST"] = PREST;
    codes["PRESTI"] = PRESTI;
    codes["SYNCID"] = SYNCID;
    codes["SYNCIDI"] = SYNCIDI;
    codes["CALL"] = CALL;
    codes["CALLI"] = CALLI;

    codes["OR"] = OR;
    codes["ORI"] = ORI;
    codes["ORN"] = ORN;
    codes["ORNI"] = ORNI;
    codes["NOR"] = NOR;
    codes["NORI"] = NORI;
    codes["XOR"] = XOR;
    codes["XORI"] = XORI;

    codes["AND"] = AND;
    codes["ANDI"] = ANDI;
    codes["ANDN"] = ANDN;
    codes["ANDNI"] = ANDNI;
    codes["NAND"] = NAND;
    codes["NANDI"] = NANDI;
    codes["NXOR"] = NXOR;
    codes["NXORI"] = NXORI;

    codes["BDIF"] = BDIF;
    codes["BDIFI"] = BDIFI;
    codes["WDIF"] = WDIF;
    codes["WDIFI"] = WDIFI;
    codes["TDIF"] = TDIF;
    codes["TDIFI"] = TDIFI;
    codes["ODIF"] = ODIF;
    codes["ODIFI"] = ODIFI;

    codes["MUX"] = MUX;
    codes["MUXI"] = MUXI;
    codes["SADD"] = SADD;
    codes["SADDI"] = SADDI;
    codes["MOR"] = MOR;
    codes["MORI"] = MORI;
    codes["MXOR"] = MXOR;
    codes["MXORI"] = MXORI;

    codes["SETH"] = SETH;
    codes["SETMH"] = SETMH;
    codes["SETML"] = SETML;
    codes["SETL"] = SETL;
    codes["INCH"] = INCH;
    codes["INCMH"] = INCMH;
    codes["INCML"] = INCML;
    codes["INCL"] = INCL;

    codes["ORH"] = ORH;
    codes["ORMH"] = ORMH;
    codes["ORML"] = ORML;
    codes["ORL"] = ORL;
    codes["ANDNH"] = ANDNH;
    codes["ANDNMH"] = ANDNMH;
    codes["ANDNML"] = ANDNML;
    codes["ANDNL"] = ANDNL;

    codes["JMP"] = JMP;
    codes["JMPB"] = JMPB;
    codes["PUSHJ"] = PUSHJ;
    codes["PUSHJB"] = PUSHJB;
    codes["GETA"] = GETA;
    codes["GETAB"] = GETAB;
    codes["PUT"] = PUT;
    codes["PUTI"] = PUTI;

    codes["RET"] = RET;
    codes["RESUME"] = RESUME;
    codes["SAVE"] = SAVE;
    codes["UNSAVE"] = UNSAVE;
    codes["SYNC"] = SYNC;
    codes["SWYM"] = SWYM;
    codes["GET"] = GET;
    codes["TRIP"] = TRIP;
}

int MaxOpCodeNameLength()
{
    return OpCodes::Instance().MaxOpCodeNameLength();
}

int GetOpCode(const std::string& opCodeName)
{
    return OpCodes::Instance().GetOpCode(opCodeName);
}

const std::string& GetOpCodeName(uint8_t opCode)
{
    return OpCodes::Instance().GetOpCodeName(opCode);
}

void OpCodeInit()
{
    OpCodes::Init();
}

void OpCodeDone()
{
    OpCodes::Done();
}

} // cmsx::machine
