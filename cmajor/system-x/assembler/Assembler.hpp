// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_ASSEMBLER_INCLUDED
#define CMSX_ASSEMBLER_ASSEMBLER_INCLUDED
#include <system-x/assembler/Instruction.hpp>
#include <system-x/assembler/Visitor.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <system-x/object/Symbol.hpp>
#include <soulng/lexer/Lexer.hpp>
#include <soulng/lexer/SourcePos.hpp>
#include <string>

namespace cmsx::assembler {

using soulng::lexer::SourcePos;

enum class Stage
{
    none, resolve, generateCode
};

class CMSX_ASSEMBLER_API Assembler : public Visitor
{
public:
    Assembler(soulng::lexer::Lexer* lexer_, const std::string& assemblyFilePath_, const std::string& objectFilePath_);
    void Assemble();
    void Visit(DecimalConstant& node) override;
    void Visit(HexadecimalConstant& node) override;
    void Visit(ByteConstant& node) override;
    void Visit(WydeConstant& node) override;
    void Visit(TetraConstant& node) override;
    void Visit(CharacterConstant& node) override;
    void Visit(StringConstant& node) override;
    void Visit(ClsIdConstant& node) override;
    void Visit(UnaryExpression& node) override;
    void Visit(BinaryExpression& node) override;
    void Visit(ParenthesizedExpression& node) override;
    void Visit(OpCode& node) override;
    void Visit(LocalSymbol& node) override;
    void Visit(GlobalSymbol& node) override;
    void Visit(At& node) override;
    void Visit(OperandList& node) override;
    void Visit(Instruction& node) override;
    void AddInstruction(Instruction* instruction);
    void AddModeInstruction(const std::string& mode, const SourcePos& sourcePos);
    Instruction* CurrentInstruction() const { return currentInstruction; }
    cmsx::object::ObjectFile* GetObjectFile() const { return objectFile.get(); }
    cmsx::object::Segment CurrentSegment() const { return currentSegment; }
    cmsx::object::Section* CurrentSection() const { return currentSection; }
    cmsx::object::Symbol* CurrentSymbol() const { return currentSymbol; }
    cmsx::object::Symbol* CurrentFunctionSymbol() const { return currentFunctionSymbol; }
    void SetCurrentFunctionSymbol(cmsx::object::Symbol* currentFunctionSymbol_) { currentFunctionSymbol = currentFunctionSymbol_; }
    cmsx::object::Symbol* CurrentStructureSymbol() const { return currentStructureSymbol; }
    void SetCurrentStructureSymbol(cmsx::object::Symbol* currentStructureSymbol_) { currentStructureSymbol = currentStructureSymbol_; }
    int ParentIndex() const { return parentIndex; }
    void SetParentIndex(int parentIndex_) { parentIndex = parentIndex_; }
    void Error(const std::string& message, const SourcePos& sourcePos);
    Node* MakeDecimalConstant(const SourcePos& sourcePos, const std::string& s);
    Node* MakeHexConstant(const SourcePos& sourcePos, const std::string& s);
    Node* MakeCharConstant(const SourcePos& sourcePos, const std::u32string& s);
    Node* MakeStringConstant(const SourcePos& sourcePos, const std::u32string& s);
    Node* MakeClsIdConstant(const SourcePos& sourcePos, const std::u32string& s);
    void IncLine() { ++line; }
    void EmptyLine();
    void InstructionLine();
    SourcePos GetSourcePos(soulng::lexer::Lexer& lexer, int64_t pos);
    void EmitOpCode(uint8_t opCode);
    void EmitRegisterValue(const std::string& paramName, const cmsx::object::Value& registerValue);
    void EmitPureByteValue(const std::string& paramName, const cmsx::object::Value& byteValue);
    void EmitPureWydeValue(const std::string& paramName, const cmsx::object::Value& wydeValue);
    void EmitPureTetraValue(const std::string& paramName, const cmsx::object::Value& tetraValue);
    void EmitPureOctaValue(const std::string& paramName, const cmsx::object::Value& octaValue);
    void EmitSymbolOcta(const cmsx::object::Value& symbolValue);
    void EmitSetPureRegValue(const cmsx::object::Value& registerValue, const cmsx::object::Value& pureValue);
    void EmitAbsoluteRegisterValue(const std::string& paramName, const cmsx::object::Value& registerValue, const cmsx::object::Value& symbolValue);
    void EmitForwardLongJump(cmsx::object::Symbol* symbol);
    void EmitForwardShortJump(const std::string& paramName, uint8_t opcode, const cmsx::object::Value& registerValue, cmsx::object::Symbol* symbol);
    void EmitLongOffset(uint32_t offset);
    void EmitShortOffset(uint16_t offset);
    void EmitClsIdCommmand(uint64_t typeIdIndex, const SourcePos& sourcePos);
private:
    void VisitInstructions();
    void Resolve();
    void GenerateCode();
    void AlignAt();
    Stage stage;
    soulng::lexer::Lexer* lexer;
    std::string assemblyFilePath;
    std::unique_ptr<cmsx::object::ObjectFile> objectFile;
    std::vector<std::unique_ptr<Instruction>> instructions;
    Instruction* currentInstruction;
    cmsx::object::Value value;
    std::vector< cmsx::object::Value> operands;
    cmsx::object::Segment currentSegment;
    cmsx::object::Section* currentSection;
    cmsx::object::Symbol* currentSymbol;
    cmsx::object::Symbol* currentFunctionSymbol;
    cmsx::object::Symbol* currentStructureSymbol;
    std::vector<boost::uuids::uuid> typeIds;
    int64_t currentAlignment;
    bool inSpec;
    int parentIndex;
    int line;
    bool prevWasEmptyLine;
};

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_ASSEMBLER_INCLUDED
