// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Context.hpp>
#include <cmajor/cmsxi/Instruction.hpp>
#include <cmajor/cmsxi/BasicBlock.hpp>

namespace cmsxi {

Context::Context() : currentFunction(nullptr), currentBasicBlock(nullptr), metadata()
{
}

void Context::AddValue(Value* value)
{
    values.push_back(std::unique_ptr<Value>(value));
}

ConstantValue* Context::GetBoolValue(bool value)
{
    ConstantValue* boolValue = new BoolValue(value);
    AddValue(boolValue);
    return boolValue;
}

ConstantValue* Context::GetSByteValue(int8_t value)
{
    ConstantValue* sbyteValue = new SByteValue(value);
    AddValue(sbyteValue);
    return sbyteValue;
}

ConstantValue* Context::GetByteValue(uint8_t value)
{
    ConstantValue* byteValue = new ByteValue(value);
    AddValue(byteValue);
    return byteValue;
}

ConstantValue* Context::GetShortValue(int16_t value)
{
    ConstantValue* shortValue = new ShortValue(value);
    AddValue(shortValue);
    return shortValue;
}

ConstantValue* Context::GetUShortValue(uint16_t value)
{
    ConstantValue* ushortValue = new UShortValue(value);
    AddValue(ushortValue);
    return ushortValue;
}

ConstantValue* Context::GetIntValue(int32_t value)
{
    ConstantValue* intValue = new IntValue(value);
    AddValue(intValue);
    return intValue;
}

ConstantValue* Context::GetUIntValue(uint32_t value)
{
    ConstantValue* uintValue = new UIntValue(value);
    AddValue(uintValue);
    return uintValue;
}

ConstantValue* Context::GetLongValue(int64_t value)
{
    ConstantValue* longValue = new LongValue(value);
    AddValue(longValue);
    return longValue;
}

ConstantValue* Context::GetULongValue(uint64_t value)
{
    ConstantValue* ulongValue = new ULongValue(value);
    AddValue(ulongValue);
    return ulongValue;
}

ConstantValue* Context::GetFloatValue(float value)
{
    ConstantValue* floatValue = new FloatValue(value);
    AddValue(floatValue);
    return floatValue;
}

ConstantValue* Context::GetDoubleValue(double value)
{
    ConstantValue* doubleValue = new DoubleValue(value);
    AddValue(doubleValue);
    return doubleValue;
}

ArrayValue* Context::GetArrayValue(Type* arrayType, const std::vector<ConstantValue*>& elements, const std::string& prefix)
{
    ArrayValue* arrayValue = new ArrayValue(arrayType, elements, prefix);
    AddValue(arrayValue);
    return arrayValue;
}

StructureValue* Context::GetStructureValue(Type* structureType, const std::vector<ConstantValue*>& members)
{
    StructureValue* structureValue = new StructureValue(structureType, members);
    AddValue(structureValue);
    return structureValue;
}

StringValue* Context::GetStringValue(Type* stringType, const std::string& value)
{
    StringValue* stringValue = new StringValue(stringType, value);
    AddValue(stringValue);
    return stringValue;
}

ConversionValue* Context::GetConversionValue(Type* type, ConstantValue* from)
{
    ConversionValue* conversionValue = new ConversionValue(type, from);
    AddValue(conversionValue);
    return conversionValue;
}

ClsIdValue* Context::GetClsIdValue(const std::string& typeId)
{
    ClsIdValue* clsIdValue = new ClsIdValue(typeId);
    AddValue(clsIdValue);
    return clsIdValue;
}

Instruction* Context::CreateNot(Value* arg)
{
    Instruction* inst = new NotInstruction(arg);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateNeg(Value* arg)
{
    Instruction* inst = new NegInstruction(arg);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateAdd(Value* left, Value* right)
{
    Instruction* inst = new AddInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateSub(Value* left, Value* right)
{
    Instruction* inst = new SubInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateMul(Value* left, Value* right)
{
    Instruction* inst = new MulInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateDiv(Value* left, Value* right)
{
    Instruction* inst = new DivInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateMod(Value* left, Value* right)
{
    Instruction* inst = new ModInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateAnd(Value* left, Value* right)
{
    Instruction* inst = new AndInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateOr(Value* left, Value* right)
{
    Instruction* inst = new OrInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateXor(Value* left, Value* right)
{
    Instruction* inst = new XorInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateShl(Value* left, Value* right)
{
    Instruction* inst = new ShlInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateShr(Value* left, Value* right)
{
    Instruction* inst = new ShrInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateEqual(Value* left, Value* right)
{
    Instruction* inst = new EqualInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateLess(Value* left, Value* right)
{
    Instruction* inst = new LessInstruction(left, right);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateSignExtend(Value* arg, Type* destType)
{
    Instruction* inst = new SignExtendInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateZeroExtend(Value* arg, Type* destType)
{
    Instruction* inst = new ZeroExtendInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateTruncate(Value* arg, Type* destType)
{
    Instruction* inst = new TruncateInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateBitCast(Value* arg, Type* destType)
{
    Instruction* inst = new BitCastInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateIntToFloat(Value* arg, Type* destType)
{
    Instruction* inst = new IntToFloatInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateFloatToInt(Value* arg, Type* destType)
{
    Instruction* inst = new FloatToIntInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateIntToPtr(Value* arg, Type* destType)
{
    Instruction* inst = new IntToPtrInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreatePtrToInt(Value* arg, Type* destType)
{
    Instruction* inst = new PtrToIntInstruction(arg, destType);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateParam(Type* type)
{
    Instruction* inst = new ParamInstruction(type);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateLocal(Type* type)
{
    Instruction* inst = new LocalInstruction(type);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateLoad(Value* ptr)
{
    Instruction* inst = new LoadInstruction(ptr);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateStore(Value* value, Value* ptr)
{
    Instruction* inst = new StoreInstruction(value, ptr);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateArg(Value* arg)
{
    Instruction* inst = new ArgInstruction(arg);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateElemAddr(Value* ptr, Value* index)
{
    Instruction* inst = new ElemAddrInstruction(ptr, index);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreatePtrOffset(Value* ptr, Value* offset)
{
    Instruction* inst = new PtrOffsetInstruction(ptr, offset);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreatePtrDiff(Value* leftPtr, Value* rightPtr)
{
    Instruction* inst = new PtrDiffInstruction(leftPtr, rightPtr);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateCall(Value* function)
{
    Instruction* inst = new CallInstruction(function);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateRet(Value* value)
{
    Instruction* inst = new RetInstruction(value);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateJump(BasicBlock* dest)
{
    Instruction* inst = new JumpInstruction(dest);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateBranch(Value* cond, BasicBlock* trueDest, BasicBlock* falseDest)
{
    Instruction* inst = new BranchInstruction(cond, trueDest, falseDest);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateSwitch(Value* cond, BasicBlock* defaultDest)
{
    Instruction* inst = new SwitchInstruction(cond, defaultDest);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

Instruction* Context::CreateTrap(const std::vector<Value*>& args)
{
    Value* b0 = nullptr;
    Value* b1 = nullptr;
    Value* b2 = nullptr;
    int n = args.size();
    for (int i = 0; i < n; ++i)
    {
        Value* arg = args[i];
        if (i == 0)
        {
            b0 = arg;
        }
        else if (i == 1)
        {
            b1 = arg;
        }
        else if (i == 2)
        {
            b2 = arg;
        }
        else
        {
            CreateArg(arg);
        }
    }
    Instruction* inst = new TrapInstruction(b0, b1, b2);
    currentBasicBlock->AddInstruction(inst);
    return inst;
}

GlobalVariable* Context::GetOrInsertGlobal(const std::string& name, Type* type)
{
    return dataRepository.GetOrInsertGlobal(name, type);
}

GlobalVariable* Context::CreateGlobalStringPtr(const std::string& stringValue)
{
    return dataRepository.CreateGlobalStringPtr(*this, stringValue);
}

void Context::SetCompileUnitId(const std::string& compileUnitId_)
{
    dataRepository.SetCompileUnitId(compileUnitId_);
}

void Context::AddMDStructItem(MDStruct* mdStruct, const std::string& fieldName, MDItem* item)
{
    mdStruct->AddItem(fieldName, item);
}

} // namespace cmsxi
