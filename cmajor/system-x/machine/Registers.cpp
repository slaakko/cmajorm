// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Registers.hpp>
#include <stdexcept>
#include <memory>

namespace cmsx::machine {

Registers::Registers() : pc(0)
{
    for (int i = 0; i < 256; ++i)
    {
        localRegs[i] = 0;
        globalRegs[i] = 0;
    }
}

void Registers::SetInterrupt(uint64_t interruptBit)
{
    uint64_t rq = GetSpecial(rQ);
    rq = rq | interruptBit;
    SetSpecial(rQ, rq);
}

void Registers::ResetInterrupt(uint64_t interruptBit)
{
    uint64_t rq = GetSpecial(rQ);
    rq = rq & ~interruptBit;
    SetSpecial(rQ, rq);
}

uint64_t Registers::GetInterruptBits() 
{
    uint64_t rq = GetSpecial(rQ);
    uint64_t rk = GetSpecial(rK);
    SetSpecial(rQ, rq & ~rk);
    return rq & rk;
}

class RegMap
{
public:
    static void Init();
    static void Done();
    static RegMap& Instance() { return *instance; }
    const std::string& GetGlobalRegName(uint8_t reg) const;
    uint8_t GetGlobalRegNumber(const std::string& globalRegName) const;
private:
    RegMap();
    static std::unique_ptr<RegMap> instance;
    std::map<uint8_t, std::string> globalRegNameMap;
    std::map<std::string, uint8_t> globalRegNumberMap;
};

std::unique_ptr<RegMap> RegMap::instance;

void RegMap::Init()
{
    instance.reset(new RegMap());
}

void RegMap::Done()
{
    instance.reset();
}

RegMap::RegMap()
{
    globalRegNameMap[regAX] = "ax";
    globalRegNameMap[regBX] = "bx";
    globalRegNameMap[regCX] = "cx";
    globalRegNameMap[regDX] = "dx";
    globalRegNameMap[regEX] = "ex";
    globalRegNameMap[regSP] = "sp";
    globalRegNameMap[regFP] = "fp";
    globalRegNameMap[regIX] = "ix";
    globalRegNameMap[rA] = "rA";
    globalRegNameMap[rB] = "rB";
    globalRegNameMap[rC] = "rC";
    globalRegNameMap[rD] = "rD";
    globalRegNameMap[rE] = "rE";
    globalRegNameMap[rF] = "rF";
    globalRegNameMap[rG] = "rG";
    globalRegNameMap[rH] = "rH";
    globalRegNameMap[rI] = "rI";
    globalRegNameMap[rJ] = "rJ";
    globalRegNameMap[rK] = "rK";
    globalRegNameMap[rL] = "rL";
    globalRegNameMap[rM] = "rM";
    globalRegNameMap[rN] = "rN";
    globalRegNameMap[rO] = "rO";
    globalRegNameMap[rP] = "rP";
    globalRegNameMap[rQ] = "rQ";
    globalRegNameMap[rR] = "rR";
    globalRegNameMap[rS] = "rS";
    globalRegNameMap[rT] = "rT";
    globalRegNameMap[rU] = "rU";
    globalRegNameMap[rV] = "rV";
    globalRegNameMap[rW] = "rW";
    globalRegNameMap[rX] = "rX";
    globalRegNameMap[rY] = "rY";
    globalRegNameMap[rZ] = "rZ";
    globalRegNameMap[rBB] = "rBB";
    globalRegNameMap[rTT] = "rTT";
    globalRegNameMap[rWW] = "rWW";
    globalRegNameMap[rXX] = "rXX";
    globalRegNameMap[rYY] = "rYY";
    globalRegNameMap[rZZ] = "rZZ";
    globalRegNumberMap["ax"] = regAX;
    globalRegNumberMap["bx"] = regBX;
    globalRegNumberMap["cx"] = regCX;
    globalRegNumberMap["dx"] = regDX;
    globalRegNumberMap["ex"] = regEX;
    globalRegNumberMap["sp"] = regSP;
    globalRegNumberMap["fp"] = regFP;
    globalRegNumberMap["ix"] = regIX;
    globalRegNumberMap["rA"] = rA;
    globalRegNumberMap["rB"] = rB;
    globalRegNumberMap["rC"] = rC;
    globalRegNumberMap["rD"] = rD;
    globalRegNumberMap["rE"] = rE;
    globalRegNumberMap["rF"] = rF;
    globalRegNumberMap["rG"] = rG;
    globalRegNumberMap["rH"] = rH;
    globalRegNumberMap["rI"] = rI;
    globalRegNumberMap["rJ"] = rJ;
    globalRegNumberMap["rK"] = rK;
    globalRegNumberMap["rL"] = rL;
    globalRegNumberMap["rM"] = rM;
    globalRegNumberMap["rN"] = rN;
    globalRegNumberMap["rO"] = rO;
    globalRegNumberMap["rP"] = rP;
    globalRegNumberMap["rQ"] = rQ;
    globalRegNumberMap["rR"] = rR;
    globalRegNumberMap["rS"] = rS;
    globalRegNumberMap["rT"] = rT;
    globalRegNumberMap["rU"] = rU;
    globalRegNumberMap["rV"] = rV;
    globalRegNumberMap["rW"] = rW;
    globalRegNumberMap["rX"] = rX;
    globalRegNumberMap["rY"] = rY;
    globalRegNumberMap["rZ"] = rZ;
    globalRegNumberMap["rBB"] = rBB;
    globalRegNumberMap["rTT"] = rTT;
    globalRegNumberMap["rWW"] = rWW;
    globalRegNumberMap["rXX"] = rXX;
    globalRegNumberMap["rYY"] = rYY;
    globalRegNumberMap["rZZ"] = rZZ;
}

const std::string& RegMap::GetGlobalRegName(uint8_t reg) const
{
    auto it = globalRegNameMap.find(reg);
    if (it != globalRegNameMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("name for global register " + std::to_string(reg) + " not found");
    }
}

uint8_t RegMap::GetGlobalRegNumber(const std::string& globalRegName) const
{
    auto it = globalRegNumberMap.find(globalRegName);
    if (it != globalRegNumberMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("number for global register '" + globalRegName + "' not found");
    }
}

const std::string& GetGlobalRegName(uint8_t globalReg)
{
    return RegMap::Instance().GetGlobalRegName(globalReg);
}

uint8_t GetGlobalRegNumber(const std::string& globalRegName)
{
    return RegMap::Instance().GetGlobalRegNumber(globalRegName);
}

void InitReg()
{
    RegMap::Init();
}

void DoneReg()
{
    RegMap::Done();
}

} // cmsx::machine
