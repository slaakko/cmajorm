// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_SIMPLE_ASSEMBLY_CODE_GENERATOR_INCLUDED
#define CMSX_INTERMEDIATE_SIMPLE_ASSEMBLY_CODE_GENERATOR_INCLUDED
#include <system-x/intermediate/Visitor.hpp>
#include <system-x/intermediate/CodeGen.hpp>
#include <system-x/assembler/AssemblyFile.hpp>

namespace cmsx::intermediate {

class RegisterAllocator;

class CMSX_INTERMEDIATE_API SimpleAssemblyCodeGenerator : public CodeGenerator, public Visitor
{
public:
    SimpleAssemblyCodeGenerator(Context* context_, cmsx::assembler::AssemblyFile* assemblyFile_);
    void AddSourceFileInfo(CompileUnit& compileUnit) override;
    void GenerateDebugInfo() override;
    void Error(const std::string& message) override;
    const SourcePos& GetSourcePos() const override;
    Context* Ctx() const override { return GetContext(); }
    RegisterAllocator* RegAllocator() const override { return registerAllocator; }
    void Emit(cmsx::assembler::Instruction* assemblyInstruction) override;
    void EmitOcta(uint64_t value) override;
    void EmitTetra(uint32_t value) override;
    void EmitWyde(uint16_t value) override;
    void EmitByte(uint8_t value) override;
    void EmitSymbol(const std::string& name) override;
    int ExitLabelId() const override;
    void EmitClsId(const std::string& typeId) override;
    void EmitDebugInfoInst(cmsx::assembler::Instruction* assemblyInstruction) override;
    void SetCurrentLineNumber(uint32_t lineNumber_) override;
    void BeginTry(uint32_t tryBlockId, uint32_t parentTryBlockId) override;
    void EndTry(uint32_t tryBlockId) override;
    void Catch(uint32_t catchBlockId, uint32_t tryBlockId, const std::string& caughtTypeIdStr) override;
    void BeginCleanup(uint32_t cleanupBlockId, uint32_t tryBlockId) override;
    void EndCleanup(uint32_t cleanupBlockId) override;
    void WriteOutputFile();
    void Visit(GlobalVariable& globalVariable) override;
    void Visit(Function& function) override;
    void Visit(BasicBlock& basicBlock) override;
    void Visit(StoreInstruction& inst) override;
    void Visit(ArgInstruction& inst) override;
    void Visit(JmpInstruction& inst) override;
    void Visit(BranchInstruction& inst) override;
    void Visit(ProcedureCallInstruction& inst) override;
    void Visit(RetInstruction& inst) override;
    void Visit(SwitchInstruction& inst) override;
    void Visit(NotInstruction& inst) override;
    void Visit(NegInstruction& inst) override;
    void Visit(SignExtendInstruction& inst) override;
    void Visit(ZeroExtendInstruction& inst) override;
    void Visit(TruncateInstruction& inst) override;
    void Visit(BitcastInstruction& inst) override;
    void Visit(IntToFloatInstruction& inst) override;
    void Visit(FloatToIntInstruction& inst) override;
    void Visit(IntToPtrInstruction& inst) override;
    void Visit(PtrToIntInstruction& inst) override;
    void Visit(AddInstruction& inst) override;
    void Visit(SubInstruction& inst) override;
    void Visit(MulInstruction& inst) override;
    void Visit(DivInstruction& inst) override;
    void Visit(ModInstruction& inst) override;
    void Visit(AndInstruction& inst) override;
    void Visit(OrInstruction& inst) override;
    void Visit(XorInstruction& inst) override;
    void Visit(ShlInstruction& inst) override;
    void Visit(ShrInstruction& inst) override;
    void Visit(EqualInstruction& inst) override;
    void Visit(LessInstruction& inst) override;
    void Visit(ParamInstruction& inst) override;
    void Visit(LoadInstruction& inst) override;
    void Visit(ElemAddrInstruction& inst) override;
    void Visit(PtrOffsetInstruction& inst) override;
    void Visit(PtrDiffInstruction& inst) override;
    void Visit(FunctionCallInstruction& inst) override;
    void Visit(TrapInstruction& inst) override;
    void Visit(PhiInstruction& inst) override;
    void Visit(NoOperationInstruction& inst) override;
    void Visit(BoolValue& value) override;
    void Visit(SByteValue& value) override;
    void Visit(ByteValue& value) override;
    void Visit(ShortValue& value) override;
    void Visit(UShortValue& value) override;
    void Visit(IntValue& value) override;
    void Visit(UIntValue& value) override;
    void Visit(LongValue& value) override;
    void Visit(ULongValue& value) override;
    void Visit(FloatValue& value) override;
    void Visit(DoubleValue& value) override;
    void Visit(NullValue& value) override;
    void Visit(AddressValue& value) override;
    void Visit(ArrayValue& value) override;
    void Visit(StructureValue& value) override;
    void Visit(StringValue& value) override;
    void Visit(StringArrayValue& value) override;
    void Visit(ConversionValue& value) override;
    void Visit(ClsIdValue& value) override;
    void Visit(SymbolValue& value) override;
    Function* CurrentFunction() const override { return currentFunction; };
    cmsx::assembler::AssemblyFile* AssemblyFile() const override { return assemblyFile; }
    int CurrentLineNumber() const override { return lineNumber; }
private:
    cmsx::assembler::AssemblySectionKind emitSection;
    cmsx::assembler::AssemblyFile* assemblyFile;
    cmsx::assembler::AssemblyFunction* assemblyFunction;
    cmsx::assembler::AssemblyStruct* assemblyStructure;
    cmsx::assembler::AssemblyDebugInfo* debugInfo;
    cmsx::assembler::Instruction* assemblyInst;
    RegisterAllocator* registerAllocator;
    Instruction* currentInst;
    Function* currentFunction;
    bool leader;
    std::string symbolName;
    std::vector<Function*> debugInfoFunctions;
    std::map<std::string, int64_t > sourceFileNameMap;
    std::map<Function*, int64_t> frameSizeMap;
    int lineNumber;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_SIMPLE_ASSEMBLY_CODE_GENERATOR_INCLUDED
