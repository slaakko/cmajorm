// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_CODE_GEN_INCLUDED
#define CMSX_INTERMEDIATE_CODE_GEN_INCLUDED
#include <system-x/intermediate/Api.hpp>
#include <system-x/assembler/AssemblyFile.hpp>

namespace cmsx::intermediate {

using soulng::lexer::SourcePos;
class Value;
class Type;
class Context;
struct Register;
struct FrameLocation;
class RegisterAllocator;
class Instruction;
class ParamInstruction;
class LoadInstruction;
class StoreInstruction;
class NotInstruction;
class NegInstruction;
class SignExtendInstruction;
class ZeroExtendInstruction;
class SwitchInstruction;
class BinaryInstruction;
class EqualInstruction;
class LessInstruction;
class JmpInstruction;
class BranchInstruction;
class NoOperationInstruction;
class RetInstruction;
class TrapInstruction;
class ArgInstruction;
class ProcedureCallInstruction;
class FunctionCallInstruction;
class ElemAddrInstruction;
class PtrOffsetInstruction;
class PtrDiffInstruction;
class BitcastInstruction;
class PtrToIntInstruction;
class IntToPtrInstruction;
class FloatToIntInstruction;
class IntToFloatInstruction;
class TruncateInstruction;
class BoolValue;
class SByteValue;
class ByteValue;
class ShortValue;
class UShortValue;
class IntValue;
class UIntValue;
class LongValue;
class ULongValue;
class FloatValue;
class DoubleValue;
class AddressValue;
class StringValue;

class CMSX_INTERMEDIATE_API CodeGenerator
{
public:
    virtual ~CodeGenerator();
    virtual Context* Ctx() const = 0;
    virtual const SourcePos& GetSourcePos() const = 0;
    virtual RegisterAllocator* RegAllocator() const = 0;
    virtual void Emit(cmsx::assembler::Instruction* assemblyInstruction) = 0;
    virtual void EmitOcta(uint64_t value) = 0;
    virtual void EmitTetra(uint32_t value) = 0;
    virtual void EmitWyde(uint16_t value) = 0;
    virtual void EmitByte(uint8_t value) = 0;
    virtual void EmitSymbol(const std::string& name) = 0;
    virtual int ExitLabelId() const = 0;
    virtual void Error(const std::string& message) = 0;
};

CMSX_INTERMEDIATE_API cmsx::assembler::Node* MakeRegOperand(const Register& reg);
CMSX_INTERMEDIATE_API int64_t GetIndex(Value* index, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API int64_t GetOffset(Type* type, int64_t index, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API int64_t GetElementSize(Type* type, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API int64_t GetPointeeSize(Type* type, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API FrameLocation GetFrameLocation(Value* value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitFrameLocationOperand(const FrameLocation& frameLocation, cmsx::assembler::Instruction* instruction, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitArgLocationOperand(cmsx::assembler::Instruction* instruction, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitPtrOperand(Value* value, cmsx::assembler::Instruction* instruction, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API cmsx::assembler::Node* MakeRegOperand(Value* value, const Register& reg, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API cmsx::assembler::Node* MakeCalleeOperand(Value* value, const Register& reg, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API cmsx::assembler::Node* MakeTrapOperand(Value* value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitParam(ParamInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitLoad(const FrameLocation& frameLocation, const Register& reg, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitStore(const FrameLocation& frameLocation, const Register& reg, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitLoad(LoadInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitStore(StoreInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitNot(NotInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitNeg(NegInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitSignExtension(SignExtendInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitZeroExtension(ZeroExtendInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitSwitch(SwitchInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitBinOpInst(BinaryInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitEqual(EqualInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitLess(LessInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitJmp(JmpInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitBranch(BranchInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitArg(ArgInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitProcedureCall(ProcedureCallInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitFunctionCall(FunctionCallInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitRet(RetInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitTrap(TrapInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitNop(NoOperationInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitElemAddr(ElemAddrInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitPtrOffset(PtrOffsetInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitPtrDiff(PtrDiffInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitBitcast(BitcastInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitPtrToInt(PtrToIntInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitIntToPtr(IntToPtrInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitFloatToInt(FloatToIntInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitIntToFloat(IntToFloatInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitTruncate(TruncateInstruction& inst, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitPrologue(CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitEpilogue(CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitBool(BoolValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitSByte(SByteValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitByte(ByteValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitShort(ShortValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitUShort(UShortValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitInt(IntValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitUInt(UIntValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitLong(LongValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitULong(ULongValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitFloat(FloatValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitDouble(DoubleValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitNull(CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitAddress(AddressValue& value, CodeGenerator& codeGen);
CMSX_INTERMEDIATE_API void EmitString(StringValue& value, CodeGenerator& codeGen);

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_CODE_GEN_INCLUDED
