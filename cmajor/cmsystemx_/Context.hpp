// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSX_CONTEXT_INCLUDED
#define CMAJOR_CMSX_CONTEXT_INCLUDED
#include <cmajor/cmsystemx/Type.hpp>
#include <cmajor/cmsystemx/Value.hpp>
#include <cmajor/cmsystemx/BasicBlock.hpp>

namespace cmsystemx {

class CMSYSTEMX_API SymbolicRegisters
{
public:
    SymbolicRegisters(Context& context);
    SymbolicRegisters(const SymbolicRegisters&) = delete;
    SymbolicRegisters& operator=(const SymbolicRegisters&) = delete;
    SymbolicRegister* Ax() { return &ax; }
    SymbolicRegister* Bx() { return &bx; }
    SymbolicRegister* Cx() { return &cx; }
    SymbolicRegister* Dx() { return &dx; }
    SymbolicRegister* Ex() { return &ex; }
    int Count() const { return 5; }
    SymbolicRegister* GetSymbolicRegister(int index);
private:
    SymbolicRegister ax;
    SymbolicRegister bx;
    SymbolicRegister cx;
    SymbolicRegister dx;
    SymbolicRegister ex;
};

class CMSYSTEMX_API Context
{
public:
    Context();
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    void SetCurrentBasicBlock(BasicBlock* bb) { currentBasicBlock = bb; }
    BasicBlock* GetCurrentBasicBlock() const { return currentBasicBlock; }
    Instruction* CreateRetVoid();
    Instruction* CreateRet(Value* value);
    Instruction* CreateInstruction(int opCode);
    SymbolicRegisters& GetSymbolicRegisters() { return symbolicRegisters; }
    FunctionType* GetFunctionType(Type* returnType, const std::vector<Type*>& parameterTypes) { return typeMap.GetFunctionType(returnType, parameterTypes); }
    Type* GetVoidType() { return typeMap.GetVoidType(); }
    Type* GetSByteType() { return typeMap.GetSByteType(); }
    Type* GetByteType() { return typeMap.GetByteType(); }
    Type* GetShortType() { return typeMap.GetShortType(); }
    Type* GetUShortType() { return typeMap.GetUShortType(); }
    Type* GetIntType() { return typeMap.GetIntType(); }
    Type* GetUIntType() { return typeMap.GetUIntType(); }
    Type* GetLongType() { return typeMap.GetLongType(); }
    Type* GetULongType() { return typeMap.GetULongType(); }
    Type* GetBoolType() { return typeMap.GetBoolType(); }
    Type* GetCharType() { return typeMap.GetCharType(); }
    Type* GetWCharType() { return typeMap.GetWCharType(); }
    Type* GetUCharType() { return typeMap.GetUCharType(); }
    Type* GetFloatType() { return typeMap.GetFloatType(); }
    Type* GetDoubleType() { return typeMap.GetDoubleType(); }
    PtrType* GetVoidPtrType() { return typeMap.GetVoidPtrType(); }
    Type* GetPtrType(Type* baseType) { return typeMap.GetPtrType(baseType); }
    Type* GetClassType(const std::vector<Type*>& memberTypes) { return typeMap.GetClassType(memberTypes); }
    Value* GetBoolValue(bool value);
    Value* GetSByteValue(int8_t value);
    Value* GetByteValue(uint8_t value);
    Value* GetShortValue(int16_t value);
    Value* GetUShortValue(uint16_t value);
    Value* GetIntValue(int32_t value);
    Value* GetUIntValue(uint32_t value);
    Value* GetLongValue(int64_t value);
    Value* GetULongValue(uint64_t value);
    Value* GetZero() { return &zero; }
    Value* GetOne() { return &one; }
    Value* GetEight() { return &eight; }
    Value* GetFloatValue(float value);
    Value* GetDoubleValue(double value);
    Value* GetCharValue(char value);
    Value* GetWCharValue(char16_t value);
    Value* GetUCharValue(char32_t value);
    Value* GetNullValue(PtrType* ptrType);
    Value* CreateUnaryExpr(Operator op, Value* value);
    Value* CreateBinaryExpr(Operator op, Value* left, Value* right);
    Value* CreateParenExpr(Value* value);
    Value* CreateBinaryOpInst(Value* left, Value* right, int opCode);
    Value* CreateAdd(Value* left, Value* right);
    Value* CreateSub(Value* left, Value* right);
    Value* CreateMul(Value* left, Value* right);
    Value* CreateDiv(Value* left, Value* right);
    Value* CreateRem(Value* left, Value* right);
    void AddValue(Value* value);
    SymbolicRegister* Ax() { return symbolicRegisters.Ax(); }
    SymbolicRegister* Bx() { return symbolicRegisters.Bx(); }
    SymbolicRegister* Cx() { return symbolicRegisters.Cx(); }
    SymbolicRegister* Dx() { return symbolicRegisters.Dx(); }
    SymbolicRegister* Ex() { return symbolicRegisters.Ex(); }
    SymbolicRegister* FP() { return &fp; }
    SymbolicRegister* SP() { return &sp; }
    SymbolicRegister* Ix() { return &ix; }
    SymbolicRegister* RR() { return &rR; }
private:
    TypeMap typeMap;
    BasicBlock* currentBasicBlock;
    SymbolicRegisters symbolicRegisters;
    SymbolicRegister fp;
    SymbolicRegister sp;
    SymbolicRegister ix;
    SymbolicRegister rR;
    std::vector<std::unique_ptr<Value>> values;
    ULongValue zero;
    ULongValue one;
    ULongValue eight;
};

} // namespace cmsystemx

#endif // CMAJOR_CMSX_CONTEXT_INCLUDED
