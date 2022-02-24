// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/OpCode.hpp>
#include <system-x/assembler/Visitor.hpp>
#include <system-x/machine/OpCode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <map>

using namespace soulng::util;

namespace cmsx::assembler {

class OpCodeMap
{
public:
    static void Init();
    static void Done();
    static OpCodeMap& Instance() { return *instance; }
    const std::string& GetOpCodeName(int opCode) const;
    int GetOpCode(const std::string& opCodeName) const;
    int MaxOpCodeNameLength() const { return maxOpCodeNameLength; }
private:
    OpCodeMap();
    int maxOpCodeNameLength;
    static std::unique_ptr<OpCodeMap> instance;
    std::map<std::string, int> codeValueMap;
    std::map<int, std::string> codeNameMap;
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

OpCodeMap::OpCodeMap() : maxOpCodeNameLength(0)
{
    codeValueMap["IS"] = IS;
    codeNameMap[IS] = "IS";
    codeValueMap["EXTERN"] = EXTERN;
    codeNameMap[EXTERN] = "EXTERN";
    codeValueMap["LINKONCE"] = LINKONCE;
    codeNameMap[LINKONCE] = "LINKONCE";
    codeValueMap["FUNC"] = FUNC;
    codeNameMap[FUNC] = "FUNC";
    codeValueMap["ENDF"] = ENDF;
    codeNameMap[ENDF] = "ENDF";
    codeValueMap["STRUCT"] = STRUCT;
    codeNameMap[STRUCT] = "STRUCT";
    codeValueMap["ENDS"] = ENDS;
    codeNameMap[ENDS] = "ENDS";
    codeValueMap["BYTE"] = BYTE;
    codeNameMap[BYTE] = "BYTE";
    codeValueMap["WYDE"] = WYDE;
    codeNameMap[WYDE] = "WYDE";
    codeValueMap["TETRA"] = TETRA;
    codeNameMap[TETRA] = "TETRA";
    codeValueMap["OCTA"] = OCTA;
    codeNameMap[OCTA] = "OCTA";
    codeValueMap[".LINK"] = LINK;
    codeNameMap[LINK] = ".LINK";
    codeValueMap[".CODE"] = CODE;
    codeNameMap[CODE] = ".CODE";
    codeValueMap[".DATA"] = DATA;
    codeNameMap[DATA] = ".DATA";
    codeValueMap[".DEBUG"] = DEBUG;
    codeNameMap[DEBUG] = ".DEBUG";
    codeValueMap["LDA"] = LDA;
    codeNameMap[LDA] = "LDA";
    codeValueMap["SET"] = SET;
    codeNameMap[SET] = "SET";
    codeValueMap["BSPEC"] = BSPEC;
    codeNameMap[BSPEC] = "BSPEC";
    codeValueMap["ESPEC"] = ESPEC;
    codeNameMap[ESPEC] = "ESPEC";
    for (const auto& valueName : codeNameMap)
    {
        int n = valueName.second.length();
        if (n > maxOpCodeNameLength)
        {
            maxOpCodeNameLength = n;
        }
    }
}

const std::string& OpCodeMap::GetOpCodeName(int opCode) const
{
    auto it = codeNameMap.find(opCode);
    if (it != codeNameMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("invalid opCode " + std::to_string(opCode));
    }
}

int OpCodeMap::GetOpCode(const std::string& opCodeName) const
{
    int opCode = cmsx::machine::GetOpCode(opCodeName);
    if (opCode == -1)
    {
        auto it = codeValueMap.find(opCodeName);
        if (it != codeValueMap.cend())
        {
            return it->second;
        }
    }
    return opCode;
}

OpCode::OpCode(const SourcePos& sourcePos_, const std::string& name_) : Node(NodeKind::opCodeNode, sourcePos_), name(name_), value(-1)
{
    if (name == "8ADDU" || name == "8ADDUI")
    {
        int x = 0;
    }
}

OpCode::OpCode(int value_) : Node(NodeKind::opCodeNode, SourcePos()), value(value_)
{
}

void OpCode::MakeImmediate()
{
    ++value;
}

void OpCode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void OpCode::Write(CodeFormatter& formatter)
{
    formatter.Write(Format(GetOpCodeName(value), MaxOpCodeWidth(), FormatWidth::min));
}

std::string GetOpCodeName(int opCode)
{
    if (opCode >= 256)
    {
        return OpCodeMap::Instance().GetOpCodeName(opCode);
    }
    else if (opCode >= 0)
    {
        return cmsx::machine::GetOpCodeName(static_cast<uint8_t>(opCode));
    }
    else
    {
        throw std::runtime_error("invalid opCode " + std::to_string(opCode));
    }
}

int GetOpCode(const std::string& opCodeName)
{
    return OpCodeMap::Instance().GetOpCode(opCodeName);
}

int MaxOpCodeWidth()
{
    return std::max(OpCodeMap::Instance().MaxOpCodeNameLength(), cmsx::machine::MaxOpCodeNameLength());
}

void InitOpCode()
{
    OpCodeMap::Init();
}

void DoneOpCode()
{
    OpCodeMap::Done();
}

} // namespace cmsx::assembler
