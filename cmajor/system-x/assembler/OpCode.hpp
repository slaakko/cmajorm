// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_OPCODE_INCLUDED
#define CMSX_ASSEMBLER_OPCODE_INCLUDED
#include <system-x/assembler/Node.hpp>

namespace cmsx::assembler {

const int IS = 256;
const int EXTERN = 257;
const int LINKONCE = 258;
const int FUNC = 259;
const int ENDF = 260;
const int STRUCT = 261;
const int ENDS = 262;
const int BYTE = 263;
const int WYDE = 264;
const int TETRA = 265;
const int OCTA = 266;
const int LINK = 267;
const int CODE = 268;
const int DATA = 269;
const int DEBUG = 270;
const int LDA = 271;
const int SET = 272;
const int BSPEC = 273;
const int ESPEC = 274;
const int numInsts = ESPEC + 1;

const uint64_t FILEINFO = 0;
const uint64_t FUNCINFO = 1;
const uint64_t LINEINFO = 2;
const uint64_t BEGINTRY = 3;
const uint64_t ENDTRY = 4;
const uint64_t CATCH = 5;
const uint64_t BEGINCLEANUP = 6;
const uint64_t ENDCLEANUP = 7;

class CMSX_ASSEMBLER_API OpCode : public Node
{
public:
    OpCode(const SourcePos& sourcePos_, const std::string& name_);
    OpCode(int value_);
    void MakeImmediate();
    const std::string& Name() const { return name; }
    int Value() const { return value; }
    void SetValue(int value_) { value = value_; }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;;
private:
    std::string name;
    int value;
};

CMSX_ASSEMBLER_API std::string GetOpCodeName(int opCode);
CMSX_ASSEMBLER_API int GetOpCode(const std::string& opCodeName);
CMSX_ASSEMBLER_API int MaxOpCodeWidth();

CMSX_ASSEMBLER_API void InitOpCode();
CMSX_ASSEMBLER_API void DoneOpCode();

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_OPCODE_INCLUDED
