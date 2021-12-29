// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_VISITOR_INCLUDED
#define CMSX_INTERMEDIATE_VISITOR_INCLUDED
#include <system-x/intermediate/Api.hpp>

namespace cmsx::intermediate {

class Context;
class StructureType;
class ArrayType;
class FunctionType;
class GlobalVariable;
class Function;
class BasicBlock;
class StoreInstruction;
class ArgInstruction;
class JmpInstruction;
class BranchInstruction;
class ProcedureCallInstruction;
class RetInstruction;
class SwitchInstruction;
class NotInstruction;
class NegInstruction;
class SignExtendInstruction;
class ZeroExtendInstruction;
class TruncateInstruction;
class BitcastInstruction;
class IntToFloatInstruction;
class FloatToIntInstruction;
class IntToPtrInstruction;
class PtrToIntInstruction;
class AddInstruction;
class SubInstruction;
class MulInstruction;
class DivInstruction;
class ModInstruction;
class AndInstruction;
class OrInstruction;
class XorInstruction;
class ShlInstruction;
class ShrInstruction;
class EqualInstruction;
class LessInstruction;
class ParamInstruction;
class LocalInstruction;
class LoadInstruction;
class ElemAddrInstruction;
class PtrOffsetInstruction;
class PtrDiffInstruction;
class FunctionCallInstruction;
class TrapInstruction;
class PhiInstruction;
class NoOperationInstruction;

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
class NullValue;
class AddressValue;
class ArrayValue;
class StructureValue;
class StringValue;
class StringArrayValue;
class ConversionValue;
class ClsIdValue;
class SymbolValue;

class CMSX_INTERMEDIATE_API Visitor
{
public:
    Visitor(Context* context_);
    Context* GetContext() const { return context; }
    virtual void Visit(StructureType& structureType) {}
    virtual void Visit(ArrayType& arrayType) {}
    virtual void Visit(FunctionType& functionType) {}
    virtual void Visit(GlobalVariable& globalVariable) {}
    virtual void Visit(Function& function) {}
    virtual void Visit(BasicBlock& basicBlock) {}
    virtual void Visit(StoreInstruction& inst) {}
    virtual void Visit(ArgInstruction& inst) {}
    virtual void Visit(JmpInstruction& inst) {}
    virtual void Visit(BranchInstruction& inst) {}
    virtual void Visit(ProcedureCallInstruction& inst) {}
    virtual void Visit(RetInstruction& inst) {}
    virtual void Visit(SwitchInstruction& inst) {}
    virtual void Visit(NotInstruction& inst) {}
    virtual void Visit(NegInstruction& inst) {}
    virtual void Visit(SignExtendInstruction& inst) {}
    virtual void Visit(ZeroExtendInstruction& inst) {}
    virtual void Visit(TruncateInstruction& inst) {}
    virtual void Visit(BitcastInstruction& inst) {}
    virtual void Visit(IntToFloatInstruction& inst) {}
    virtual void Visit(FloatToIntInstruction& inst) {}
    virtual void Visit(IntToPtrInstruction& inst) {}
    virtual void Visit(PtrToIntInstruction& inst) {}
    virtual void Visit(AddInstruction& inst) {}
    virtual void Visit(SubInstruction& inst) {}
    virtual void Visit(MulInstruction& inst) {}
    virtual void Visit(DivInstruction& inst) {}
    virtual void Visit(ModInstruction& inst) {}
    virtual void Visit(AndInstruction& inst) {}
    virtual void Visit(OrInstruction& inst) {}
    virtual void Visit(XorInstruction& inst) {}
    virtual void Visit(ShlInstruction& inst) {}
    virtual void Visit(ShrInstruction& inst) {}
    virtual void Visit(EqualInstruction& inst) {}
    virtual void Visit(LessInstruction& inst) {}
    virtual void Visit(ParamInstruction& inst) {}
    virtual void Visit(LocalInstruction& inst) {}
    virtual void Visit(LoadInstruction& inst) {}
    virtual void Visit(ElemAddrInstruction& inst) {}
    virtual void Visit(PtrOffsetInstruction& inst) {}
    virtual void Visit(PtrDiffInstruction& inst) {}
    virtual void Visit(FunctionCallInstruction& inst) {}
    virtual void Visit(TrapInstruction& inst) {}
    virtual void Visit(PhiInstruction& inst) {}
    virtual void Visit(NoOperationInstruction& inst) {}
    virtual void Visit(BoolValue& value) {}
    virtual void Visit(SByteValue& value) {}
    virtual void Visit(ByteValue& value) {}
    virtual void Visit(ShortValue& value) {}
    virtual void Visit(UShortValue& value) {}
    virtual void Visit(IntValue& value) {}
    virtual void Visit(UIntValue& value) {}
    virtual void Visit(LongValue& value) {}
    virtual void Visit(ULongValue& value) {}
    virtual void Visit(FloatValue& value) {}
    virtual void Visit(DoubleValue& value) {}
    virtual void Visit(NullValue& value) {}
    virtual void Visit(AddressValue& value) {}
    virtual void Visit(ArrayValue& value) {}
    virtual void Visit(StructureValue& value) {}
    virtual void Visit(StringValue& value) {}
    virtual void Visit(StringArrayValue& value) {}
    virtual void Visit(ConversionValue& value) {}
    virtual void Visit(ClsIdValue& value) {}
    virtual void Visit(SymbolValue& value) {}
private:
    Context* context;
};

} // cmsx::intermediate

#endif //
