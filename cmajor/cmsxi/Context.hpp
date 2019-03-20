// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_CONTEXT_INCLUDED
#define CMAJOR_CMSXI_CONTEXT_INCLUDED
#include <cmajor/cmsxi/Data.hpp>
#include <cmajor/cmsxi/Metadata.hpp>

namespace cmsxi {

class Function;
class BasicBlock;
class Instruction;

class CMSXI_API Context
{
public:
    Context();
    TypeRepository& GetTypeRepository() { return typeRepository; }
    DataRepository& GetDataRepository() { return dataRepository; }
    Metadata& GetMetadata() { return metadata; }
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
    ConstantValue* GetDefaultBoolValue() { return GetBoolType()->DefaultValue(); }
    ConstantValue* GetDefaultSByteValue() { return GetSByteType()->DefaultValue(); }
    ConstantValue* GetDefaultByteValue() { return GetByteType()->DefaultValue(); }
    ConstantValue* GetDefaultShortValue() { return GetShortType()->DefaultValue(); }
    ConstantValue* GetDefaultUShortValue() { return GetUShortType()->DefaultValue(); }
    ConstantValue* GetDefaultIntValue() { return GetIntType()->DefaultValue(); }
    ConstantValue* GetDefaultUIntValue() { return GetUIntType()->DefaultValue(); }
    ConstantValue* GetDefaultLongValue() { return GetLongType()->DefaultValue(); }
    ConstantValue* GetDefaultULongValue() { return GetULongType()->DefaultValue(); }
    ConstantValue* GetDefaultFloatValue() { return GetFloatType()->DefaultValue(); }
    ConstantValue* GetDefaultDoubleValue() { return GetDoubleType()->DefaultValue(); }
    ConstantValue* GetNullValue(PtrType* ptrType) { return ptrType->DefaultValue(); }
    ConstantValue* GetBoolValue(bool value);
    ConstantValue* GetSByteValue(int8_t value);
    ConstantValue* GetByteValue(uint8_t value);
    ConstantValue* GetShortValue(int16_t value);
    ConstantValue* GetUShortValue(uint16_t value);
    ConstantValue* GetIntValue(int32_t value);
    ConstantValue* GetUIntValue(uint32_t value);
    ConstantValue* GetLongValue(int64_t value);
    ConstantValue* GetULongValue(uint64_t value);
    ConstantValue* GetFloatValue(float value);
    ConstantValue* GetDoubleValue(double value);
    ArrayValue* GetArrayValue(Type* arrayType, const std::vector<ConstantValue*>& elements, const std::string& prefix);
    StructureValue* GetStructureValue(Type* structureType, const std::vector<ConstantValue*>& members);
    StringValue* GetStringValue(Type* stringType, const std::string& value);
    ConversionValue* GetConversionValue(Type* type, ConstantValue* from);
    ClsIdValue* GetClsIdValue(const std::string& typeId);
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
    Instruction* CreateSwitch(Value* cond, BasicBlock* defaultDest);
    Instruction* CreateTrap(const std::vector<Value*>& args);
    Instruction* CreateNop();
    GlobalVariable* GetOrInsertGlobal(const std::string& name, Type* type);
    GlobalVariable* CreateGlobalStringPtr(const std::string& stringValue);
    void SetCompileUnitId(const std::string& compileUnitId_);
    MDBool* CreateMDBool(bool value) { return metadata.CreateMDBool(value); }
    MDLong* CreateMDLong(int64_t value) { return metadata.CreateMDLong(value); }
    MDString* CreateMDString(const std::string& value) { return metadata.CreateMDString(value); }
    MDStructRef* CreateMDStructRef(int id) { return metadata.CreateMDStructRef(id); }
    MDStruct* CreateMDStruct() { return metadata.CreateMDStruct();  }
    void AddMDStructItem(MDStruct* mdStruct, const std::string& fieldName, MDItem* item);
    void SetCurrentLineNumber(int lineNumber);
    void AddLineInfo(Instruction* inst);
    MDStructRef* GetMDStructRefForSourceFile(const std::string& sourceFileName);
    void SetMetadataRef(Instruction* inst, MDStructRef* metadataRef);
private:
    TypeRepository typeRepository;
    DataRepository dataRepository;
    Metadata metadata;
    std::vector<std::unique_ptr<Value>> values;
    Function* currentFunction;
    BasicBlock* currentBasicBlock;
    int currentLineNumber;
    std::unordered_map<int, MDStructRef*> lineNumberInfoMap;
    std::unordered_map<std::string, MDStructRef*> sourceFileMap;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_CONTEXT_INCLUDED
