// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_CONTEXT_INCLUDED
#define CMAJOR_CMSXI_CONTEXT_INCLUDED
#include <cmajor/cmsxi/Type.hpp>

namespace cmsxi {

class Function;
class BasicBlock;
class Instruction;

class CMSXI_API Context
{
public:
    Context();
    TypeRepository& GetTypeRepository() { return typeRepository; }
    Type* GetVoidType() { return typeRepository.GetVoidType(); }
    Type* GetBoolType() { return typeRepository.GetBoolType(); }
    Type* GetSByteType() { return typeRepository.GetSByteType(); }
    Type* GetByteType() { return typeRepository.GetByteType(); }
    Type* GetShortType() { return typeRepository.GetShortType(); }
    Type* GetUShortType() { return typeRepository.GetUShortType(); }
    Type* GetIntType() { return typeRepository.GetIntType(); }
    Type* GetUIntType() { return typeRepository.GetUIntType(); }
    Type* GetLongType() { return typeRepository.GetLongType(); }
    Type* GetULongType() { return typeRepository.GetULongType(); }
    Type* GetFloatType() { return typeRepository.GetFloatType(); }
    Type* GetDoubleType() { return typeRepository.GetDoubleType(); }
    Type* GetPtrType(Type* baseType) { return typeRepository.GetPtrType(baseType); }
    Type* GetStructureType(const std::vector<Type*>& memberTypes) { return typeRepository.GetStructureType(memberTypes); }
    Type* CreateStructureType() { return typeRepository.CreateStructureType(); }
    Type* GetArrayType(Type* elementType, uint64_t size) { return typeRepository.GetArrayType(elementType, size); }
    Type* GetFunctionType(Type* returnType, const std::vector<Type*>& paramTypes) { return typeRepository.GetFunctionType(returnType, paramTypes); }
    Value* GetDefaultBoolValue() { return GetBoolType()->DefaultValue(); }
    Value* GetDefaultSByteValue() { return GetSByteType()->DefaultValue(); }
    Value* GetDefaultByteValue() { return GetByteType()->DefaultValue(); }
    Value* GetDefaultShortValue() { return GetShortType()->DefaultValue(); }
    Value* GetDefaultUShortValue() { return GetUShortType()->DefaultValue(); }
    Value* GetDefaultIntValue() { return GetIntType()->DefaultValue(); }
    Value* GetDefaultUIntValue() { return GetUIntType()->DefaultValue(); }
    Value* GetDefaultLongValue() { return GetLongType()->DefaultValue(); }
    Value* GetDefaultULongValue() { return GetULongType()->DefaultValue(); }
    Value* GetDefaultFloatValue() { return GetFloatType()->DefaultValue(); }
    Value* GetDefaultDoubleValue() { return GetDoubleType()->DefaultValue(); }
    Value* GetNullValue(PtrType* ptrType) { return ptrType->DefaultValue(); }
    Value* GetBoolValue(bool value);
    Value* GetSByteValue(int8_t value);
    Value* GetByteValue(uint8_t value);
    Value* GetShortValue(int16_t value);
    Value* GetUShortValue(uint16_t value);
    Value* GetIntValue(int32_t value);
    Value* GetUIntValue(uint32_t value);
    Value* GetLongValue(int64_t value);
    Value* GetULongValue(uint64_t value);
    Value* GetFloatValue(float value);
    Value* GetDoubleValue(double value);
    void AddValue(Value* value);
    void SetCurrentBasicBlock(BasicBlock* bb) { currentBasicBlock = bb; }
    BasicBlock* GetCurrentBasicBlock() const { return currentBasicBlock; }
    Instruction* CreateNot(Value* arg);
    Instruction* CreateNeg(Value* arg);
    Instruction* CreateAdd(Value* left, Value* right);
    Instruction* CreateSub(Value* left, Value* right);
    Instruction* CreateMul(Value* left, Value* right);
    Instruction* CreateDiv(Value* left, Value* right);
    Instruction* CreateMod(Value* left, Value* right);
    Instruction* CreateAnd(Value* left, Value* right);
    Instruction* CreateOr(Value* left, Value* right);
    Instruction* CreateXor(Value* left, Value* right);
    Instruction* CreateShl(Value* left, Value* right);
    Instruction* CreateShr(Value* left, Value* right);
    Instruction* CreateEqual(Value* left, Value* right);
    Instruction* CreateLess(Value* left, Value* right);
    Instruction* CreateSignExtend(Value* arg, Type* destType);
    Instruction* CreateZeroExtend(Value* arg, Type* destType);
    Instruction* CreateTruncate(Value* arg, Type* destType);
    Instruction* CreateBitCast(Value* arg, Type* destType);
    Instruction* CreateIntToFloat(Value* arg, Type* destType);
    Instruction* CreateFloatToInt(Value* arg, Type* destType);
    Instruction* CreateIntToPtr(Value* arg, Type* destType);
    Instruction* CreatePtrToInt(Value* arg, Type* destType);
    Instruction* CreateParam(Type* type);
    Instruction* CreateLocal(Type* type);
    Instruction* CreateLoad(Value* ptr);
    Instruction* CreateStore(Value* value, Value* ptr);
    Instruction* CreateArg(Value* arg);
    Instruction* CreateElemAddr(Value* ptr, Value* index);
    Instruction* CreatePtrOffset(Value* ptr, Value* offset);
    Instruction* CreatePtrDiff(Value* leftPtr, Value* rightPtr);
    Instruction* CreateCall(Value* function);
    Instruction* CreateRet(Value* value);
    Instruction* CreateJump(BasicBlock* dest);
    Instruction* CreateBranch(Value* cond, BasicBlock* trueDest, BasicBlock* falseDest);
    Instruction* CreateSwitch(Value* cond);
private:
    TypeRepository typeRepository;
    std::vector<std::unique_ptr<Value>> values;
    Function* currentFunction;
    BasicBlock* currentBasicBlock;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_CONTEXT_INCLUDED
